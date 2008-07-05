"""NumArray Universal Function Module

$Id: ufunc.py,v 1.157 2006/05/12 17:41:01 jaytmiller Exp $
"""

import types
import memory
import _sort, _bytes
from operator import and_  
import _ufuncall
import generic as _gen
import _ufunc
import _converter
import numerictypes as _nt
import numarraycore as _nc
import safethread
import warnings as _warnings

# note more imports are at end of file

#import pywin.debugger

"""DEBUGGING NOTE:

     A reasonable amount of care went into maintaining the Python
     prototype for _callOverDimensions, _UnaryUFunc, _BinaryUFunc,
     _Converter, and _Operator.  Thus, problems in the C
     implementation can sometimes be isolated by substituting the
     original Python code back in, either in whole or part.
     Substituting in the whole is easy: set _PROTOTYPE to 1 below.
"""
_PROTOTYPE = 0

_FPE_DIVIDE_BY_ZERO = 1
_FPE_OVERFLOW       = 2
_FPE_UNDERFLOW      = 4
_FPE_INVALID        = 8

class _NumErrorMode:
    
    _errorset = ("ignore", "warn", "raise")

    def _num_mode_test(self, val, all, default):
        if val not in self._errorset and val is not None:
            raise ValueError("Error mode value must be in " +
                             repr(self._errorset) + " or None.")
        if all is not None:
            if val is not default:
                return 1
            else:
                return 0
        else:
            return 1
        
    
    def __init__(self, all=None, overflow="warn", underflow="ignore",
                 dividebyzero="warn", invalid="warn"):
        
        if all in self._errorset:
            self.overflow = all
            self.underflow = all
            self.dividebyzero = all
            self.invalid = all

        if self._num_mode_test(overflow, all, "warn"):
            self.overflow = overflow
            
        if self._num_mode_test(underflow, all, "ignore"):
            self.underflow = underflow
            
        if self._num_mode_test(dividebyzero, all,  "warn"):
            self.dividebyzero = dividebyzero
            
        if self._num_mode_test(invalid, all, "warn"):
            self.invalid = invalid

    def __repr__(self):
        return ("_NumErrorMode(overflow='%s', underflow='%s', dividebyzero='%s', invalid='%s')" % (self.overflow, self.underflow, self.dividebyzero, self.invalid))

class NumError:
    """Defines how numeric errors should be handled.

    Error configuration is managed as a mapping (by thread) of stacks.
    Thus it is possible for each thread to have an independent error
    configuration, and also for multiple routines within the same
    thread to save and restore error configurations as needed.
    
    """
    def __init__(self, all=None,
                 overflow="warn", underflow="ignore", dividebyzero="warn",
                 invalid="warn"):
        self._modestack = {}  # map of stacks indexed by thread id
        self._defaultmode = _NumErrorMode()
        self.setMode(all=all, underflow=underflow, overflow=overflow,
                     dividebyzero=dividebyzero, invalid=invalid)

    def setDefaultMode(self, all=None, overflow="warn", underflow="ignore",
                       dividebyzero="warn", invalid="warn"):
        self._defaultmode = _NumErrorMode(all, overflow, underflow,
                                         dividebyzero, invalid)

    def setMode(self, all=None, overflow="warn", underflow="ignore",
                      dividebyzero="warn", invalid="warn"):
        self._setmodes( _NumErrorMode(all, overflow, underflow,
                                      dividebyzero, invalid) )

    def pushMode(self, all=None, overflow="warn", underflow="ignore",
                      dividebyzero="warn", invalid="warn"):
        self._pushmodes( _NumErrorMode(all, overflow, underflow,
                                      dividebyzero, invalid) )

    def _getmodestack(self):
        id = safethread.get_ident()
        try:
            l = self._modestack[id]
        except KeyError:
            l = [ self._defaultmode ]
            self._modestack[id] = l
        if l == []:
            l = [ self._defaultmode ]
        return l

    def _setmodes(self, modes):
        l = self._getmodestack()
        l[-1] = modes

    def getMode(self):
        l = self._getmodestack()
        return l[-1]

    def _pushmodes(self, modes):
        l = self._getmodestack()
        l.append(modes)

    def popMode(self):
        l = self._getmodestack()
        return l.pop()
    
Error = NumError()

class MathDomainError(ArithmeticError): pass
class UnderflowError(ArithmeticError): pass
class NumOverflowError(OverflowError, ArithmeticError): pass

def handleError(errorStatus, sourcemsg):
    """Take error status and use error mode to handle it."""
    modes = Error.getMode()
    if errorStatus & _FPE_INVALID:
        if modes.invalid == "warn":
            print "Warning: Encountered invalid numeric result(s)", sourcemsg
        if modes.invalid == "raise":
            raise MathDomainError(sourcemsg)
    if errorStatus & _FPE_DIVIDE_BY_ZERO:
        if modes.dividebyzero == "warn":
            print "Warning: Encountered divide by zero(s)", sourcemsg
        if modes.dividebyzero == "raise":
            raise ZeroDivisionError(sourcemsg)
    if errorStatus & _FPE_OVERFLOW:
        if modes.overflow == "warn":
            print "Warning: Encountered overflow(s)", sourcemsg
        if modes.overflow == "raise":
            raise NumOverflowError(sourcemsg)
    if errorStatus & _FPE_UNDERFLOW:
        if modes.underflow == "warn":
            print "Warning: Encountered underflow(s)", sourcemsg
        if modes.underflow == "raise":
            raise UnderflowError(sourcemsg)

def _nIOArgs(cfuncs):

    """Determine number of arguments from signature

    Returns tuple of number of input and output arguments.
    Also check for consistency in numbers of arguments.
    """

    # The number of items in the optype key implies the number of inputs,
    # hence the number of inputs = # of underscores+1
    noptypeinputs = []
    ninputs = []
    noutputs = []
    for key in cfuncs.keys():
        if 'R' in key or 'A' in key:  # reduce, accumulate
            continue
        noptypeinputs.append(key.count('_'))
        for informkey in cfuncs[key].keys():
            ninputs.append(len(informkey))
            noutputs.append(len(cfuncs[key][informkey][0]))
            
    return max(ninputs), max(noutputs)

if _PROTOTYPE:
    def _getBlockingParameters(shape, niter, overlap=0):
        try:
            return _blockingParametersCache[ (shape, niter, overlap) ]
        except KeyError:
            return _getBlockingMiss(shape, niter, overlap)

def _getBlockingMiss(shape, niter, overlap=0):
    """Figure out how to split the input array into subnumarray
    close (but less than) the size of the block buffers.
    overlap is used in cases where it is necessary to overlap the
    last dimension in computations (for example for reduce and
    accumulate.)

    returns:

    level:      index at which the c functions should be called
    blockingparameters:  tuple consisting of
         number of full subnumarray at the given level
         subarray shape
         a flag indicating if there is a leftover part of the array
             which is smaller than the regular subarray (leftover)
         the shape of the leftover subarray
    """
    
    if shape == ():
        retval = 0, (1, (), 0, None)
        _blockingParametersCache[(shape,niter,overlap)] = retval
        return retval
    
    ndim = len(shape)
    csize = long(shape[-1]) # cumulative shape
    if csize > niter:
        # last dimension is too big for buffer
        # special overlap handling in this case
        nblocks, leftover = divmod(csize, niter-overlap)
        nblocks, leftover = int(nblocks), int(leftover)
        if overlap:
            if leftover:
                retval = ndim-1, (nblocks, (niter,), 1, (leftover,))
            else:
                retval = ndim-1, (nblocks-1, (niter,), 1, (niter-1,))
        else:
            if leftover:
                retval = ndim-1, (nblocks, (niter,), 1, (leftover,))
            else:
                retval = ndim-1, (nblocks, (niter,), 0, None)
    else:
        for i in xrange(2,ndim+1):
            csize *= shape[-i]
            if csize >= niter:
                # need to break up this dimension
                tdim = long(shape[-i])
                blockdim = niter*tdim/csize
                nblocks, leftover = divmod(tdim, blockdim)
                nblocks, leftover = int(nblocks), int(leftover)
                stail = shape[-i+1:]
                if leftover:
                    retval = ndim-i, (nblocks, (blockdim,)+stail,
                        1, (leftover,) + stail)
                else:
                    retval = ndim-i, (nblocks, (blockdim,)+stail, 0, None)
                break
        else:
            # don't need to break up
            retval = 0, (1, shape, 0, None)
    _blockingParametersCache[(shape,niter,overlap)] = retval
    return retval

# _doOverDimensions has been re-implemented in C.
def _doOverDimensions(objects, outshape,
            dims, indexlevel, blockingparameters, overlap=0, level=0):
    """This version handles alignment, byteswapping and n-dimensions

    This is a recursive method dims starts out as an empty list, and
    accumulates the indices for each dimension. Eg. if the array has
    shape (7, 3, 5) on the initial call, the next level call will
    specify the the 'first' index (corresponding to the 7), the next
    will have the first two indices (and that would be the last level
    since the lowest dimension is taken care of by the cfuncs.)

    """
    if level == indexlevel:
        nregShapeIters, shape, leftover, leftoverShape, = blockingparameters
        if shape != ():
            dimval = shape[0]-overlap
            for i in xrange(nregShapeIters):
                for o in objects:
                    o.compute(dims+[i*dimval], shape)
            if leftover:
                for o in objects:
                    o.compute(dims+[(i+1)*dimval], leftoverShape)
        else:
                for o in objects:
                    o.compute([0], shape)
    else:
        # recurse
        for i in xrange(outshape[level]):
            _doOverDimensions(objects, outshape,
                              dims+[i], indexlevel, blockingparameters,
                              overlap, level+1)

def _callOverDimensions(objects, outshape, index, blocking, overlap=0, level=0):
    return _doOverDimensions(objects, outshape, [], index, blocking, overlap, level)

# Replace the Python version of _callOverDimensions with one implemented in C.
# Although _doOverDimensions is not replaced here, it has been re-implemented
# in C as well
if not _PROTOTYPE:
    from _ufunc import _callOverDimensions

def _copyFromAndConvertMiss(inarr, outarr):
    key = (_digest(inarr), _digest(outarr), safethread.get_ident())
    # Type conversion done using inputconverter
    input = _InputConverter(inarr, outarr._type.name)
    # Ensure output step uses output buffer of input step
    output = _OutputConverter(outarr, outarr._type.name,
                              inbuffer=input.result_buff,
                              forcestride=1)
    maxitemsize = max(inarr._type.bytes, outarr._type.bytes)
    niter = _ufunc.getBufferSize()/maxitemsize
    if (isinstance(inarr, _nc.NumArray) and
        isinstance(outarr, _nc.NumArray)):
        _copyCache[ key ] = input, output, niter
    return input, output, niter

if _PROTOTYPE:
    def _copyFromAndConvert(inarr, outarr):
        """Copy from one array to another handling strides, form & conv"""
        if inarr._shape != outarr._shape:
            raise ValueError("Arrays must have the same shape")

        key = (_digest(inarr), _digest(outarr), safethread.get_ident())
        try:
            input, output, niter = _copyCache[ key ]
        except KeyError:
            input, output, niter = _copyFromAndConvertMiss(inarr, outarr)

        input.rebuffer(inarr)
        output.rebuffer(outarr, input.result_buff)
        
        indexlevel, blockingparameters = \
                    _ufunc._getBlockingParameters(outarr._shape, niter)
        objects = (input, output)
        _ufunc.CheckFPErrors()
        _callOverDimensions(objects, outarr._shape, indexlevel,
                            blockingparameters)
        errorstatus = _ufunc.CheckFPErrors()

        input.clean(inarr)
        output.clean(inarr)
        output.clean(outarr)

        if errorstatus:
            handleError(errorstatus, " during type conversion")
else:
    from _ufunc import _copyFromAndConvert

def _noConversionNeeded(x, xt):
    """returns 1 iff x is a c_array of type xt."""
    if isinstance(x, _nt.scalarTypes):
        return True
    return x.is_c_array() and (x._type.name == xt or x._type == xt)

def _inputcheck(*inargs):
    """Check input args for type, convert sequences to numnumarray

    Return list of 1) input arg tuple with scalars and numnumarray only
                   2) signature string indicating which args are scalars
                      and which are numnumarray
                   3) flag indicating if only scalar arguments are present
    """

    retargs = []
    retsig = []
    scalar = 1
    for inarg in inargs:
        if isinstance(inarg, _nc.NumArray):
            scalar = 0
            retsig.append('v')
        elif _nc.PyNUMERIC_TYPES.has_key(type(inarg)):
            retsig.append('s')
        else:
            scalar = 0
            retsig.append('v')
            # see if it can be made into an array
            try:
                inarg = _nc.array(inarg)
            except TypeError:
                raise TypeError(
                 "UFunc arguments must be numarray, scalars or numeric sequences")
        retargs.append(inarg)
    return retargs, ''.join(retsig), scalar

def _pseudoArrayFromScalars(scalarvalues, type):
    """Wrap a scalar in a buffer so it can be used as an array"""
    arr = _bufferPool.getBuffer()
    arr._check_overflow = 1
    newtype = type # _numtypedict[type]
    arr._strides = (newtype.bytes,)
    arr._type = newtype
    arr._itemsize = newtype.bytes
    arr._strides = None
    if isinstance(scalarvalues, (list, tuple)):
        arr._shape = (len(scalarvalues),)
        for i in xrange(len(scalarvalues)):
            arr[i] = scalarvalues[i]
    else:
        arr._shape = ()
        arr[()] = scalarvalues
    #  Modify block buffer attributes to look like vector/vector setup.
    return arr

def _normalize_results(inputs, outputs, results, return_rank1=False):
    """normalize_results() does standard return value handling for
    a given set of ufunc actual parameters and results.  Ufunc inputs
    are assumed to be arrays or scalars, ufunc outputs can be arrays
    or unspecified, and ufunc "preliminary results" must be arrays.
    
    The standard processing assumptions and rules are summarized as
    follows:

    1. All ufunc inputs must be specified with *some* value.

    2. If any outputs were specified, all outputs must be specified.
       If no outputs were specified, outputs is None or ().  Specified
       outputs are always arrays.

    3. If any outputs were specified, the return value is supressed
       (is None).

    4. If no outputs were specified, at least one array value will be
       returned.  All results are assumed to have the same shape.

    a. If len(results)==1, the tuple is discarded and the return value
    is a single array or scalar.

    b. If len(results) > 1,  the return value is a tuple.

    5. If the result array(s) are rank-0, either a scalar, rank-0, or
    rank-1 array is returned.

    a. If at least 1 input is an array, the result will be an array.

    b. If all inputs are scalars, the result(s) is(are) a scalar.

    c. If return_rank1 is True, rank-0 results are converted to rank-1-len-1.
    
    6. Arrays of dimension > 0 have no special shape handling.    
    """
    # catch "void" ufuncs here just in case, also specified outputs => None
    if len(results) == 0 or (outputs is not None and outputs is not ()):
        return None

    if results[0].rank == 0:  # scalar, rank-0, or rank-1 results
        for i in inputs:  # any rank0 input implies rank0 output
            if isinstance(i, _nc.NumArray) and i.rank==0:
                if return_rank1: # convert rank0 arrays to rank1
                    for result in results:
                        result.shape = (1,)
                break
        else:  # all scalar inputs implies scalar outputs
            results = list(results)
            for i in range(len(results)):
                results[i] = results[i][()]
            results = tuple(results)
    else: # rank >= 1 arrays are returned unaltered
        pass

    if len(results) == 1:
        # len-1 results converts to the result itself
        results = results[0]

    return results


if not _PROTOTYPE:
    from numarray._ufunc import _normalize_results
    
# Portions of the UFunc classes have been re-implemented in C.
class _UFunc(_ufunc._ufunc):
    """Class to handle all element-by-element mathematical functions

    This is the base class -- subclasses must implement __call__ and
    (if allowed) the outer and _cumulative methods.
    """

    def __init__(self, operator, ufuncs, inputs, outputs, identity):
        self._cfuncs = self._organize_cfuncs(ufuncs)
        self._coercions = _nt.genericCoercions
        self._promotionExclusions = _nt.genericPromotionExclusions
        self._typerank = _nt.genericTypeRank
        self._cache = {}

    def _evaltypes(self, intypes):
        return tuple([ eval("_nt." + t) for t in intypes ])

    def _sort_typesigs(self, cfuncs):
        l = []
        for intypes, rest in cfuncs.items():  # input signatures
            it = self._evaltypes( intypes )
            l.append((it,)+rest)
        l.sort() # intype tuples should sort in promotion order!?
        return l        

    def _organize_cfuncs(self, ufuncs):
        """Replace unordered cfunc dictionary with list ordered by
        input signature type comparison."""
        ufs = {}
        for form in ufuncs:
            ufs[form] = self._sort_typesigs(ufuncs[form])
        return ufs

    def _getidentity(self):
        return _nc.array([self._identity])

    if _PROTOTYPE:
        def _cache_flush(self):
            # print "flushing cache for", self
            self._cache = {}

        def __call__(self, *args):
            raise NotImplementedError("__call__ is not implemented by base UFunc class")

    def __repr__(self):
        return "<UFunc: '%s'>" % self.operator

    def _scan_sigs(self, types, binTypeList):
        """scan_sigs matches a list of input types against a sorted list of
        signature,cfunc tuples.
        """
        inplen = len(types)
        for bt in binTypeList:
            btsig = bt[0]
            if inplen != len(btsig):  # same parameter count
                continue
            for i in range(inplen): # find first sig >= types
                t = types[i]
                if (isinstance(t, _nt.NumericType) and 
                    (t > btsig[i] or 
                     btsig[i].name in self._promotionExclusions[t])):
                    break
            else:  # entire sig "fits",  first match.
                return bt
        raise TypeError("Couldn't find a function which matches the inputs")

    def _typematch_N(self, types, form):
        """Determine which C function is most appropriate for the given
        types.  Returns a new form (possibly the all-vector default form),
        the input types to be converted to,  the output types, and the
        c function.
        """
        for t in types:
            assert isinstance(t, _nt.NumericType) or t in _nt.scalarTypes
        assert isinstance(form, str)

        try:  # Look for an exact match of the current form
            form2 = form
            binTypeList = self._cfuncs[ form2 ]
        except KeyError:  # Look for an all "vector" standby
            form2 = "v".join(form.split("s"))  
            binTypeList = self._cfuncs[ form2 ]

        try:  # Do "binary" ufunc type coercions
            intype = _nt.getType(self._coercions[ tuple(types) ])
            types1 = (intype,)*len(types)
        except KeyError:
            types1 = types

        try:  # Attempt with coerced types...
            return (form2,) + self._scan_sigs(types1, binTypeList)
        except TypeError: # fail back to non-coerced match
            if form == "s"*len(types):  # all scalars --> scalars must match
                types = [ eval("_nt."+_nt.scalarTypeMap[t]) for t in types ]
            return (form2,) + self._scan_sigs(types, binTypeList)

    def outer(self, inarr1, inarr2, outarr=None):
        raise ValueError("Outer only available for binary functions")


_blockingParametersCache = {}
_copyCache = {}

def flush_caches():
    """flush_cache elminates all ufunc cache entries."""
    import sys
    global _blockingParametersCache, _copyCache
    module = sys.modules[__name__]
    for n in module.__dict__.keys():
        f = module.__dict__[n]
        if isinstance(f, _UFunc):
            f._cache_flush()
    _blockingParametersCache = {}
    _copyCache = {}

class _BufferPool:
    """A Pool of available buffers available for block compuations

    Buffers can be gotten. When they are deleted they are automatically
    returned to the buffer pool so they can be reused.
    """
    def __init__(self, buffersize=100000):
        # buffers are initially allocated as needed
        self.buffers = []
        self.setBufferSize(buffersize)
        
    def getBuffer(self):
        try:
            return _nc._UBuffer(self.buffers.pop())
        except IndexError:
            # out of buffers, create a new one
            return _nc._UBuffer(memory.new_memory(_ufunc.getBufferSize()))
    def getBufferSize(self):
        return _ufunc.getBufferSize()
    
    def setBufferSize(self, buffersize):
        """Change buffer size (useful mainly for testing)"""
        oldsize = _ufunc.getBufferSize()
        if buffersize != oldsize:
            _ufunc.setBufferSize(buffersize)
            # delete existing (wrong-sized) buffers
            flush_caches()
            self.buffers = []
        return oldsize

_bufferPool = _BufferPool()

def _sequence(x):
    return isinstance(x, list) or isinstance(x, tuple)

if _PROTOTYPE:
    def _digest(x):
        if isinstance(x, _nc.NumArray):
            return ("array",
                    x.iscontiguous(),
                    x.isaligned(),
                    x.isbyteswapped(),
                    x.type())
        elif _isScalar(x):
            return type(x), x   # for demo only
        elif x is None:
            return None
        else:
            raise KeyError, "_digest force cache miss"

else:
    _digest = _ufunc.digest


def _restuff_pseudo(p, x):
    if _isScalar(x):
        if p is None:
            raise RuntimeError, "scalar value with no pseudo array"
        else: # restuff p with x
            p[0] = x
            return p
    else:
        if p is not None: 
            return p
        else:
            return x
_restuff_pseudo = _ufunc.restuff_pseudo

def _cache(original_input, pseudo_array):
    if _isScalar(original_input):
        return pseudo_array
    else:
        return None

def _rank(seq, r=0):
    if isinstance(seq, _nc.NumArray):
        return r + len(seq._shape)
    try:
        x = len(seq)
    except:
        return r
    else:
        if x == 0:
            return r
        else:
            return _rank(seq[0], r+1)

class _UnaryUFunc(_UFunc):
    """Class for ufuncs with 1 input and 1 output argument"""

    if _PROTOTYPE:
        def __call__(self, inarr1, outarr=None):
            """The standard calling interface for UFuncs"""
            return self._cached_dispatch1(inarr1, outarr)

        def _fast_exec1(self, in1, out, cached):
            mode, outtype, cfunc, ufargs_junk, inform, cin1 = cached
            ufargs = (out.nelements(), 1, 1,
                      ((in1._data, in1._byteoffset),
                       (out._data, out._byteoffset)))
            apply(cfunc, ufargs)

        def _slow_exec1(self, in1, out, cached):
            mode, outtype, cfunc, ufargs, inform, cin1 = cached
            inputs, outputs, maxitemsize = ufargs
            niter = _ufunc.getBufferSize()/maxitemsize
            indexlevel, blockingparameters = \
                        _ufunc._getBlockingParameters(out._shape, niter)
            operator = _Operator(cfunc,
                                 [inputs[0].rebuffer(in1)],
                                 [outputs[0].rebuffer(out)])
            objects = (inputs[0],operator,outputs[0])
            _callOverDimensions(objects, out._shape, indexlevel, blockingparameters)
            inputs[0].clean(in1)
            outputs[0].clean(out)

        def _cache_exec1(self, in1, out, cached):
            if out.nelements(): # skip 0-element arrays
                _ufunc.CheckFPErrors()
                if cached[0] == "fast":
                    self._fast_exec1(in1, out, cached)
                else:
                    self._slow_exec1(in1, out, cached)
                error = _ufunc.CheckFPErrors()
                if error:
                    handleError(error, " in "+self.operator)
            return out

        def _cache_lookup1(self, in1, out):
            """_cache_lookup1 checks the ufunc cache for an entry corresponding to
            (in1, in2, out) and returns it if it is found.  If the cache misses,
            _cache_lookup1 performs a full ufunc setup and returns it.  In either
            case,  vector_vector inputs are dual broadcast to match shapes.
            """
            win1 = in1
            try:
                key = (_digest(win1), _digest(out), safethread.get_ident())
                cached = self._cache[ key ]
            except KeyError:
                return self._cache_miss1(win1, out)
            else:
                mode, otype, cfunc, ufargs, inform, cin1 = cached
                cin1 = _restuff_pseudo(cin1, in1)
                if out is None:
                    if inform == "v":
                        wout = win1.new(otype)
                    else: # scalar
                        wout = _nc.zeros((), type=otype)
                else:
                    if inform == "v":
                        shape = win1._shape
                    else: # scalar
                        shape = ()
                    if shape != out._shape:
                        raise ValueError("Supplied output array does not have appropriate shape")
                    wout = out
            return cin1, wout, cached

        def _cached_dispatch1(self, inarr, outarr):
            params = self._cache_lookup1(inarr, outarr)
            result = self._cache_exec1(*params)
            return _normalize_results((inarr,),outarr,(result,),False)

    def _cache_miss1(self, n1, out):
        (in1,), inform, scalar = _inputcheck(n1)
        
        mode, win1, wout, cfunc, ufargs = \
              self._setup(in1, inform, out)
        
        cached = mode, wout._type, cfunc, ufargs, inform, _cache(n1, win1)

        try:
            key = (_digest(n1), _digest(out), safethread.get_ident())
        except KeyError:
            pass
        else:
            if _PROTOTYPE:
                self._cache[ key ] = cached
            else:
                self._cache_insert( cached, n1, None, out)                
        return win1, wout, cached
        
    def _setup(self, inarr, inform, outarr):
        """Setup for unary ufunc"""
        if inform == "s": # scalar
            t =  _nc.array(inarr)  # create rank-0
            t._check_overflow = 1  # enable overflow checking for t
            t[()] = inarr           # force any overflow
            inarr = t
        shape = inarr._shape
        fform, convtypes, outtypes, cfunc = self._typematch_N((inarr._type,), inform)
        intype = convtypes[0]
        if outarr is None:
            outarr = inarr.new(_numtypedict[outtypes[0]])
        if shape != outarr._shape:
            raise ValueError("Supplied output array does not have appropriate shape")
        # If fast case, return the arguments for an immediate call to the
        #  C function
        if (_noConversionNeeded(inarr, intype) and
            _noConversionNeeded(outarr, outtypes[0])):
            return ("fast", inarr, outarr, cfunc, None)

        # slower, general case
        inputs = (_InputConverter(inarr, intype), )
        outputs = (_OutputConverter(arr=outarr, type=outtypes[0]),)
        # find largest itemsize involved, it will determine how many
        # iterations can be done on the temporary buffers (i.e., niter)
        maxitemsize = max(_numtypedict[inarr._type.name].bytes,
                          _numtypedict[intype].bytes,
                          _numtypedict[outtypes[0]].bytes,
                          _numtypedict[outarr._type.name].bytes)
        preprocessing_outbuffers = (inputs[0].result_buff,)
        postprocessing_inbuffers = (outputs[0].result_buff,)
        return ("slow", inarr, outarr, cfunc, 
                (inputs, outputs, maxitemsize))

def _firstcol(arr):
    rval = arr.view()
    rval._shape = rval._shape[:-1]
    rval._strides = rval._strides[:-1]
    return rval

def _moveToLast(dim, iseq):
    oseq = []
    for i in range(len(iseq)):
        if i != dim:
            oseq.append(iseq[i])
        else:
            s = iseq[i]
    oseq.append(s)
    return oseq

def _fixdim(axis, dim):
    """support the deprecation of the 'dim' keyword in favor of 'axis'"""
    if dim is not None:
        if axis != 0:
            raise RuntimeError("Specify 'axis' or 'dim', but not both.  'dim' is deprecated.")
        _warnings.warn("The 'dim' keyword is deprecated.  Specify 'axis' instead.",
                       DeprecationWarning,
                       stacklevel=3)
        return dim
    return axis

class _BinaryUFunc(_UFunc):
    """Class for ufuncs with 2 input and 1 output arguments"""

    if _PROTOTYPE:
        def _fast_exec2(self, in1, in2, out, cached):
            mode, outtype, cfunc, ufargs_junk, inform, cin1, cin2 = cached
            ufargs = (out.nelements(), 2, 1,
                      ((in1._data, in1._byteoffset),
                       (in2._data, in2._byteoffset),
                       (out._data, out._byteoffset)))
            apply(cfunc, ufargs)

        def _slow_exec2(self, in1, in2, out, cached):
            mode, outtype, cfunc, ufargs, inform, cin1, cin2 = cached
            inputs, outputs, maxitemsize = ufargs
            niter = _ufunc.getBufferSize()/maxitemsize
            indexlevel, blockingparameters = \
                        _ufunc._getBlockingParameters(out._shape, niter)
            operator = _Operator(cfunc,[inputs[0].rebuffer(in1),
                                        inputs[1].rebuffer(in2)],
                                 [outputs[0].rebuffer(out)])
            objects = inputs + (operator,) + outputs
            _callOverDimensions(objects, out._shape, indexlevel,
                                blockingparameters)
            inputs[0].clean(in1)
            inputs[1].clean(in2)
            outputs[0].clean(out)

        def _cache_exec2(self, in1, in2, out, cached):
            if out.nelements():  # skip 0-element arrays
                _ufunc.CheckFPErrors()
                if cached[0] == "fast":
                    self._fast_exec2(in1, in2, out, cached)
                else:
                    self._slow_exec2(in1, in2, out, cached)
                error = _ufunc.CheckFPErrors()
                if error:
                    handleError(error, " in "+self.operator)
            return out

        def _cache_lookup2(self, in1, in2, out):
            """_cache_lookup2 checks the ufunc cache for an entry corresponding to
            (in1, in2, out) and returns it if it is found.  If the cache misses,
            _cache_lookup2 performs a full ufunc setup and returns it.  In either
            case,  vector_vector inputs are dual broadcast to match shapes.
            """
            if (isinstance(in1, _nc.NumArray) and
                isinstance(in2, _nc.NumArray)):
                win1, win2 = in1._dualbroadcast(in2)
            else:
                win1, win2 = in1, in2
            try:
                key = (_digest(win1), _digest(win2), _digest(out), safethread.get_ident())
                cached = self._cache[ key ]
            except KeyError:
                return self._cache_miss2(win1, win2, out)
            else:
                mode, otype, cfunc, ufargs, inform, cin1, cin2 = cached
                cin1 = _restuff_pseudo(cin1, win1)
                cin2 = _restuff_pseudo(cin2, win2)
                if out is None:
                    if inform in ["vs", "vv"]: # vector_scalar, vector_vector
                        wout = win1.new(cached[1])
                    elif inform == "sv":       # scalar_vector
                        wout = win2.new(cached[1])
                    else: # scalar_scalar
                        wout = _nc.zeros((), type=otype)
                else:
                    if inform in ["vs", "vv"]:
                        shape = win1._shape
                    elif inform == "sv":
                        shape = win2._shape
                    else: # scalar_scalar
                        shape = ()
                    if shape != out._shape:
                        raise ValueError("Supplied output array does not have appropriate shape")
                    wout = out
            return cin1, cin2, wout, cached

        def _cached_dispatch2(self, inarr1, inarr2, outarr):
            params = self._cache_lookup2(inarr1, inarr2, outarr)
            result = self._cache_exec2(*params)
            return _normalize_results((inarr1, inarr2), outarr, (result,), False)
        
        def __call__(self, inarr1, inarr2, outarr=None):
            """The standard calling interface for UFuncs"""
            return self._cached_dispatch2(inarr1, inarr2, outarr)

    def _cache_miss2(self, n1, n2, out):
        (in1, in2), inform, scalar = _inputcheck(n1, n2)
        
        mode, win1, win2, wout, cfunc, ufargs = \
              self._setup(in1, in2, inform, out)
        
        cached = mode, wout._type, cfunc, ufargs, inform, \
                 _cache(n1, win1), _cache(n2, win2)
            
        try:
            key = (_digest(n1), _digest(n2), _digest(out),
                   safethread.get_ident())
        except KeyError:
            pass
        else:
            if _PROTOTYPE:
                self._cache[ key ] = cached
            else:
                self._cache_insert(cached, n1, n2, out)
        
        return win1, win2, wout, cached
        
    def _setup(self, in1, in2, inform, out):
        """Setup for binary ufunc"""
        wout = out
        if inform == "ss":
            # do it as two 0-d numarray.
            in1, in2 = _nc.array(in1), _nc.array(in2)
            inform = "vv"
            if out is None: wout = in1.copy()
        elif inform == "vs":
            intypes = (in1._type, type(in2))
            fform, convtypes, outtypes, cfunc = self._typematch_N(intypes, inform)
            if fform == "vs":
                inarr1, inarr2 = in1, _pseudoArrayFromScalars(in2, convtypes[1])
            else:
                in1, in2 = in1, _nc.array(in2, type=convtypes[1])
                inform = "vv"
            if out is None: wout = in1.new(outtypes[0])
        elif inform == "sv":
            intypes = (type(in1), in2._type)
            fform, convtypes, outtypes, cfunc = self._typematch_N(intypes, inform)
            if fform == "sv":
                inarr1, inarr2 = _pseudoArrayFromScalars(in1, convtypes[0]), in2
            else:
                in1, in2 = _nc.array(in1, type=convtypes[0]), in2
                inform = "vv"
            if out is None: wout = in2.new(outtypes[0])
        if inform == "vv":
            intypes = (in1._type, in2._type)
            inarr1, inarr2 = in1._dualbroadcast(in2)
            fform, convtypes, outtypes, cfunc = self._typematch_N(intypes, inform)
            if out is None: wout = inarr1.new(outtypes[0])
        
        #If fast case, return the arguments for an immediate call to the
        #  C function
        if (_noConversionNeeded(inarr1, convtypes[0]) and
            _noConversionNeeded(inarr2, convtypes[1]) and
            _noConversionNeeded(wout, _numtypedict[outtypes[0]])):
            return ("fast", inarr1, inarr2, wout, cfunc, None)

        # slower, general case
        inputs = (_InputConverter(inarr1, convtypes[0]),
                  _InputConverter(inarr2, convtypes[1]))
        outputs = (_OutputConverter(arr=wout, type=outtypes[0]),)

        # find largest itemsize involved, it will determine how many
        # iterations can be done on the temporary buffers (i.e., niter)
        maxitemsize = max(_numtypedict[inarr1._type.name].bytes,
                          _numtypedict[inarr2._type.name].bytes,
                          _numtypedict[convtypes[0]].bytes,
                          _numtypedict[convtypes[1]].bytes,
                          _numtypedict[outtypes[0]].bytes,
                          _numtypedict[wout._type.name].bytes)
        return ("slow", inarr1, inarr2, wout, cfunc,
                (inputs, outputs, maxitemsize))

    def outer(self, inarr1, inarr2, outarr=None):
        """Return outer product of 2 numarray"""
        (inarr1, inarr2), inform, scalar = _inputcheck(inarr1, inarr2)
        ndim2 = len(inarr2._shape)
        indexarg = (slice(None, None, None),)*len(inarr1._shape) + (None,)*ndim2
        if outarr is not None:
            self(inarr1.__getitem__(indexarg), inarr2, outarr)
        else:
            return self(inarr1.__getitem__(indexarg), inarr2)

    def zreduce(self, array, axis=0, out=None, type=None, dim=None):
        """zreduce returns a rank-0 array as the result of the
        reduction of array <= rank-1 array, similar to Numeric.
        """
        axis = _fixdim(axis, dim)
        in1 = _nc.asarray(array)
        r = self._cum_swapped(in1, axis, out, "R", type)
        if in1.rank <= 1:
            r.shape = ()
            r.strides = ()
        return r

    if _PROTOTYPE:
        def accumulate(self, array, axis=0, out=None, type=None, dim=None):
            """accumulate applies the binary operator 'self' at successive
            pairs of elements of 'array' along dimension 'dim', storing the
            result in 'out'.  If no 'out' is provided, the result of the
            accumulation will be returned.  The result of accumulating
            'array' has the same shape as 'array'.
            """
            axis = _fixdim(axis, dim)
            in1 = _nc.asarray(array)
            return self._cum_swapped(in1, axis, out, "A", type)

        def areduce(self, array, axis=0, out=None, type=None,
            dim=None):
            """areduce applies the operator of ufunc 'self' to reduce
            'array' along its 'dim' axis.  If no 'out' is provided,
            the result of the reduction is returned.  The result of
            reducing an N-dimensional array is an N-1 dimensional
            array.  The result of reducing a rank-1 array is *still* a
            rank-1 array.

            see also 'reduce'.
            """
            axis = _fixdim(axis, dim)
            in1 = _nc.asarray(array)
            return self._cum_swapped(in1, axis, out, "R", type)

        def reduce(self, array, axis=0, out=None, type=None, dim=None):
            """areduce applies the operator of ufunc 'self' to reduce
            'array' along its 'dim' axis.  If no 'out' is provided,
            the result of the reduction is returned.  The result of
            reducing an N-dimensional array is an N-1 dimensional array.
            The result of reducing a rank-1 array is a scalar.

            see also 'areduce'.
            """
            axis = _fixdim(axis, dim)
            in1 = _nc.asarray(array)
            r = self._cum_swapped(in1, axis, out, "R", type)
            if r is not None and in1.rank <= 1:
                if r._shape == (1,):
                    r = r[0]
                elif r._shape == ():
                    r = r[()]
            return r

        def _cum_cached(self, cumop, in1, out, type):
            if (out is not None):
                if not isinstance(out, _nc.NumArray):
                    raise TypeError("output array must be a NumArray")
                if (not out.isaligned() or out.isbyteswapped()):
                    raise ValueError("Reduce/Accumulate: no support for misaligned / byteswapped output numarray")
            if cumop not in ["R", "A"]:
                raise ValueError("Unknown cumulative option")
            
            if in1.rank == 0:
                if out is None:
                    out = in1.astype(type)
                out[()] = in1[()]
                return out
                
            params = self._cum_lookup(cumop, in1, out, type)
            wout = self._cum_exec(*params)
            
            if cumop == "R":
                wout._shape = in1._shape[:-1]
                wout._strides = wout._stridesFromShape()
                if wout._shape == ():
                    wout._shape = (1,)
                    wout._strides = (wout._itemsize,)
                    
            if out is None:
                return wout
            i, o, cached = params
            mode, otype, cfunc, ufargs = cached
            if out._type != otype: 
                _copyFromAndConvert(wout, out)
            return out

        def _cum_lookup(self, cumop, in1, out, type):
            try:
                key = (_digest(in1), _digest(out), cumop, safethread.get_ident(), type)
                cached = self._cache[ key ]
            except KeyError:  # lists always miss
                return self._cum_cache_miss(cumop, in1, out, type)
            else:
                mode, otype, cfunc, ufargs = cached
                if cumop == "R":
                    wout = self._reduce_out(in1, out, otype)
                else:
                    wout = self._accumulate_out(in1, out, otype)
                return in1, wout, cached

        def _cum_exec(self, in1, out, cached):
            mode, otype, cfunc, ufargs = cached
	    if otype == _nt.Bool:
	        if in1._type != _nt.Bool:
		   in1 = in1.astype(_nt.Bool)
            if in1.nelements():  # skip 0-element arrays
                _ufunc.CheckFPErrors()
                if mode == "fast":
                    self._cum_fast_exec(in1, out, cached)
                else:
                    self._cum_slow_exec(in1, out, cached)
                errorstatus = _ufunc.CheckFPErrors()
                if errorstatus:
                    handleError(errorstatus,
                                "in cumulative application of " + self.operator)
            return out

        def _cum_fast_exec(self, in1, out, cached):
            mode, otype, cfunc, ufargs = cached
            cfunc(in1._shape,
                  in1._data,  in1._byteoffset,  in1._strides,
                  out._data, out._byteoffset, out._strides)

        def _cum_slow_exec(self, in1, out, cached):
            mode, otype, cfunc, ufargs = cached
            input, output, maxitemsize = ufargs
            niter = _ufunc.getBufferSize()/maxitemsize
            if in1._shape[-1] > niter:
                overlap = 1
            else:
                overlap = 0
            indexlevel, blockingparameters = \
                        _ufunc._getBlockingParameters(in1._shape, niter, overlap)
            operator = _Operator(cfunc,
                                 [input.rebuffer(in1)],
                                 [output.rebuffer(out)],
                                 otype.bytes)
            objects = (input, operator, output)
            _callOverDimensions(objects, out._shape,
                                indexlevel, blockingparameters, overlap)
            input.clean(in1)
            output.clean(out)

        def _accumulate_out(self, inarr, outarr, outtype):
            """_accumulate_out creates output array for accumulate"""
            # Create output array if not supplied.
            if outarr is None:
                toutarr = inarr.new( outtype )
            else:
                if outarr._shape != inarr._shape:
                    raise ValueError(
                       "Supplied output array does not have the appropriate shape")
                toutarr = outarr
            # It is necessary to initialize the first subarray of the output
            # to the first subarray of the input (C functions depend on it).
            # toutarr[...,0] = inarr[...,0]
            if inarr.nelements():
                _firstcol(toutarr)._copyFrom(_firstcol(inarr))
            return toutarr

        def _reduce_out(self, inarr, outarr, outtype):
            """_reduce_out creates output array for reduce"""
            firstcol = _firstcol(inarr) # inarr[..., 0]
            # Create output array if not supplied or wrong type
            if outarr is None or outarr._type != outtype:
                toutarr = firstcol.new(outtype)
            else:
                toutarr = outarr
            if len(inarr._shape) == 1 and toutarr._shape == (1,):
                toutarr._shape, toutarr._strides = (), ()
            if firstcol._shape != toutarr._shape:
                raise ValueError("output array shape mismatch")
            if inarr.nelements(): # skip 0-element arrays
                toutarr._copyFrom(firstcol)  # Assign first subarray of output
            else:
                toutarr._copyFrom(self._identity)
            toutarr._strides += (0,)
            toutarr._shape = inarr._shape
            diff = len(toutarr._strides) - len(toutarr._shape)
            # if diff > 0:
            #    toutarr._strides  = toutarr._strides[diff:]
            return toutarr

        def _cum_swapped(self, in1, dim, out, cumop, type=None):
            if in1.rank == 0:
                return in1.copy()
            if dim == in1.rank-1:
                dim = -1
            if dim != -1:
                _in1 = _gen.swapaxes(in1, -1, dim)
                _out = _gen.swapaxes(out, -1, dim)
            else:
                _in1, _out = in1, out
            _out1 = self._cum_cached(cumop, _in1, _out, type)
            if cumop == "A":
                if dim != -1:
                    _out1.swapaxes(-1, dim) # swap axes to "fix" the result
            else:
                if dim != -1:
                    if in1.rank:
                        _out1._shape = _moveToLast(dim, _out1._shape)
                        _out1._strides = _moveToLast(dim, _out1._strides)
            if out is None:
                return _out1        

    def _cum_cache_miss(self, cumop, in1, out, type):
        """computes the setup values and caches them."""
        mode, win1, wout, cfunc, ufargs = \
              self._cum_setup(cumop, in1, out, type)
        
        cached = mode, wout._type, cfunc, ufargs
        try:
            key = (_digest(in1), _digest(out), cumop,
                   safethread.get_ident(), type)
        except KeyError:
            pass
        else:
            if _PROTOTYPE:
                self._cache[ key ] = cached
            else:
                self._cache_insert(cached, in1, None, out, cumop, type);
        return win1, wout, cached

    '''
        try:
            intype  = _numtypedict[self._typePromoter(win1._type.name, typekeys)]
        except TypeError: 
            assert 0

        otype, cfunc = ufdict[(optype,)]
    '''
            
    def _cum_setup(self, cumop, in1, out, type=None):
        """Used by both reduce and accumulate to compute accumulations.

        Assumes that the dimension of accumulation/reduction is 0.

            in1        -- The input array to accumulate/reduce
            out       -- Optional output array

        The output array must have a consistent shape with the input
        Array (the same for accumulate and minus the accumulated dimension
        for reduce).

        The output type must match that produced by default
        and cannot be nonaligned or byteswapped. (It is a real mess to
        remove this restriction. (The original numeric doesn't even support
        output numarray.)
        """
        (win1,), inform, scalar = _inputcheck(in1)  # win1 substitute for lists
        if scalar:
            raise ValueError("First argument must be an array or sequence")

        # The original numeric returns the operations identity for empty numarray.
        sourcemsg = "in %s.%s" % (self.operator, cumop) # For error message

        # Get appropriate function, figure out any needed input type conversions
        # and what the output type will be
        intype = win1._type
        if out is None:
            if type is None:
                optype = intype
            else:
                optype = _nt.getType(type)
        else:
            if type is not None:
                raise ValueError("Can't specify both 'type' and 'out'")
            else:
                optype = out.type()
        try:
            fform, convtypes, otypes, cfunc = self._typematch_N(
                (optype,), cumop)
        except TypeError:  # Hack for logical operators
            ufdict = self._cfuncs[cumop]
            typekeys  = [tupl[0] for tupl in ufdict]
            if len(typekeys) == 1 and typekeys[0] == (_nt.Bool,):  
                return self._cum_setup( cumop, win1.astype('Bool'), out)
            else:
                raise       
        otype = _numtypedict[otypes[0]]

        if out is None and type is None:
            optype = otype

        # A few words on shape/stride manipulations:
        # The C function assumes the last dimension is being accumulated.
        # Create the accumulate/reduce specific output array, tout
        #   type of reduce tout is always otype to avoid output conversion
        #   with mismatched shapes.
        if cumop == "R":
            tout = self._reduce_out(win1, out, otype)
        else:
            tout = self._accumulate_out(win1, out, otype)

        # check for fast case (Unlike Ufuncs, contiguous numarray not required)
        if (not win1.isbyteswapped() and win1.isaligned()
            and otype == intype
            and not tout.isbyteswapped()
            and tout.isaligned()):
            return "fast", win1, tout, cfunc, None
        else:
            # slow case
            # And now a few words about overlap. It is used to allow the
            # accumulate C function to work across blocks. Overlap in effect
            # makes the first subarray of the next block the last subarray
            # of the previous block (or last array value for 1-d numarray) so
            # the accumulate can carry on. The computation of the block size
            # must take it into account as does computeOverDimensions in its
            # computation of the offsets for the blocks. This is only needed
            # where the accumulated dimension is larger than a block. It is
            # not needed at all for reduce since the output array contains
            # the net accumulation.
            # check special case where last dimension is bigger than blocksize
            maxitemsize = max(intype.bytes, optype.bytes, otype.bytes)

            niter = _ufunc.getBufferSize()/maxitemsize
            
            # This is a bit tricky. If the accumulated dimension is bigger
            # than the block size, it is necessary to overlap the block
            # computations by one element so that the last value of the
            # previous block can continue the count into the next block.
            if win1._shape[-1] > niter:
                overlap = 1
            else:
                overlap = 0
            indexlevel, blockingparameters = \
                _ufunc._getBlockingParameters(win1._shape, niter, overlap)
            # The usual compute stuff
            input = _InputConverter(win1, type=convtypes[0].name)

            # Since cfunc is selected by output type, no type conversion
            #  is required.  Since the cfuncs are striding, no striding
            #  is required.  Byteswapping and aligning are disallowed.
            #  Thus, the output converter is a NULL converter.
            output = _OutputConverter(tout, type=optype.name, nonstriding=1)
            return "slow", win1, tout, cfunc, (input, output, maxitemsize)

class _CacheEntry:
    def __init__(self, **keys):
        self.__dict__.update(keys)

class _NaryUFunc(_UFunc):
    """Class for ufuncs with M input and N output arguments"""
    def __init__(self, operator, ufuncs, inputs, outputs, identity=None):
        _UFunc.__init__(self, operator, ufuncs, inputs, outputs, identity)
        self._cfuncs = self._organize_cfuncs(ufuncs)
        self._coercions = _nt.genericCoercions
        self._promotionExclusions = _nt.genericPromotionExclusions
        self._typerank = _nt.genericTypeRank
        self._cache = {}

    def _cache_flush(self):
        self._cache = {}

    def __repr__(self):
        return "_NaryUfunc(%s, inputs=%d, outputs=%d)" % \
               (self.operator, self.n_inputs, self.n_outputs)

    def __call__(self, *args):        
        """The standard calling interface for UFuncs"""
        if (len(args) < self.n_inputs or 
            self.n_inputs+self.n_outputs < len(args)):
            raise ValueError(
                "Ufunc %s takes %d inputs and %d outputs "
                "but %d parameters given." %
                (self.operator, self.n_inputs, self.n_outputs, len(args)))
        if self.n_inputs < len(args) < self.n_inputs + self.n_outputs:
            raise ValueError("Ufunc %s takes %d inputs and %d outputs; "
                             "if any outputs are specified, all outputs "
                             "must be specified." %
                             (self.operator, self.n_inputs, self.n_outputs))
        inputs = tuple(args[:self.n_inputs])
        outputs  = tuple(args[self.n_inputs:])                            
        params = self._cache_lookup(inputs, outputs)
        results = self._cache_exec(*params)
        return _normalize_results(inputs, outputs, results, False)

    def _fast_path(self, parameters):
        """_fast_path decides if a set of parameters is suitable for
        small array performance optimiation.

        The fast path is as follows:

        1. Well-behaved NumArray and scalar parameters are supported
        2. Broadcast arrays are not supported
        3. Misbehaved arrays are not supported
        4. Type converted arrays are not supported        
        """
        shape = None
        type = None
        for p in parameters:
            if isinstance(p, _nc.NumArray):
                if shape is None:
                    shape = p._shape
                else:
                    if p._shape != shape:
                        return False, None
                if type is None:
                    type = p._type
                else:
                    if type != p._type:
                        return False, None
                if not p.is_c_array():
                    return False, None
            elif not _isScalar(p):
                return False, None
        else:
            if shape is None:
                shape = ()
            return True, shape

    def _cache_lookup(self, inputs, outputs):
        """_cache_lookup checks the ufunc cache for an entry
        corresponding to (inputs... outputs...) and returns it if it
        is found.  If the cache misses, _cache_lookup performs a full
        ufunc setup and returns it.  In either case, inputs are nWay
        broadcast to maintain shape.

        """
        fast, shape = self._fast_path(inputs + outputs)
        if fast:
            try:
                key = self._key(inputs, outputs)  # some inputs fail to key
                cached = self._cache[ key ]
            except KeyError:
                winputs, woutputs, cached = self._cache_miss(inputs, outputs)
                key = self._key(inputs, outputs)
                self._cache[ key ] = cached
            else:
                winputs = self._manage_cached_inputs(cached.cinputs, inputs)
                woutputs = self._manage_outputs(
                    shape, cached.outtypes, outputs)
        else:
            winputs, woutputs, cached = self._cache_miss(inputs, outputs)
        return winputs, woutputs, cached
            
    def _key(self, inputs, outputs):
        if outputs is None:
            outputs = [ None ]
        digests = [ _digest(x) for x in inputs + outputs ]
        return tuple([safethread.get_ident()] + digests)

    def _manage_cached_inputs(self, cinputs, winputs):
        # stuff pseudo buffers with current scalar values
        cinputs = list(cinputs)
        for i in range(len(winputs)):
            cinputs[i] = _restuff_pseudo(cinputs[i], winputs[i])
        return tuple(cinputs)        

    def _manage_outputs(self, shape, outtypes, outputs):
        if outputs == (): # create output arrays
            woutputs = []
            for ot in outtypes:
                woutputs.append(_nc.NumArray(shape=shape, type=ot))
        else:  # check specified output arrays
            if len(outputs) != len(outtypes):
                raise RuntimeError("Wrong number of output arrays.  Either supply no output arrays, or supply all output arrays.")
            for out in outputs:
                if shape != out._shape:
                    raise ValueError("Supplied output array does not"
                                     " have appropriate shape")
            woutputs = outputs
        return tuple(woutputs)

    def _cache_miss(self, inputs, outputs):
        mode, outtypes, winputs, woutputs, cfunc, ufargs = \
              self._setup(inputs, outputs)
        cached_inputs = [_cache(inputs[i], winputs[i]) \
                         for i in range(self.n_inputs)] 
        cached = _CacheEntry(mode=mode, outtypes=outtypes, cfunc=cfunc,
                             ufargs=ufargs, cinputs=cached_inputs)
        return winputs, woutputs, cached

    def _setup(self, inputs, outputs):
        """Setup for nary ufunc"""
        inputs, inform, scalar = _inputcheck(*tuple(inputs))
        intypes = []
        for i in inputs:
            if isinstance(i, _nc.NumArray):
                intypes.append(i._type)
            else:
                intypes.append(type(i))
        inform1, convtypes, outtypes, cfunc = \
                 self._typematch_N(intypes, inform)
        for i in range(len(inform1)):
            if isinstance(inputs[i], _nt.scalarTypes):
                if inform1[i] == "s":
                    inputs[i] = _pseudoArrayFromScalars(
                        [inputs[i]], type=convtypes[i])
                elif inform1[i] == "v":
                    inputs[i] = _nc.array(inputs[i], type=convtypes[i])
                else:
                    raise RuntimeError("Unexpected array form.")
        common_shape = _gen._common_shapes(inputs)
        winputs = _gen._broadcast_all(inputs, common_shape)
        woutputs = self._manage_outputs(common_shape, outtypes, outputs)
        
        #If fast case, return the arguments for an immediate call to the
        #  C function
        types = convtypes + outtypes
        parameters = list(winputs) + list(outputs)
        for i in range(len(parameters)):
            if not _noConversionNeeded(parameters[i], types[i]):
                break
        else:
            return ("fast", outtypes, winputs, woutputs, cfunc, None)

        # slower, general case
        input_convs = tuple([ _InputConverter(a, ctype) for a,ctype in
                        zip(winputs, convtypes) ])
        output_convs = tuple([_OutputConverter(a, ctype) for a,ctype in
                        zip(woutputs, outtypes) ])
        
        # find largest itemsize involved, it will determine how many
        # iterations can be done on the temporary buffers (i.e., niter)
        types = tuple([ a._type.name for a in winputs + woutputs ]) + \
                convtypes + outtypes 
        maxitemsize = 0
        for t in types:
            tsize = _numtypedict[t].bytes
            if tsize > maxitemsize:
                maxitemsize = tsize
                
        return ("slow", outtypes, winputs, woutputs, cfunc,
                (input_convs, output_convs, maxitemsize))
    
    def _cache_exec(self, inputs, outputs, cached):
        if outputs[0].nelements(): # skip 0-element arrays
            _ufunc.CheckFPErrors()
            if cached.mode == "fast":
                self._fast_exec(inputs, outputs, cached)
            else:
                self._slow_exec(inputs, outputs, cached)
            error = _ufunc.CheckFPErrors()
            if error:
                handleError(error, " in "+self.operator)
        return outputs

    def _fast_exec(self, inputs, outputs, cached):
        arrays = inputs + outputs
        buffers = []
        ne = 1
        for i in range(self.n_inputs+self.n_outputs):
            a = arrays[i]
            buffers.append((a._data, a._byteoffset))
            ne = max(ne, a.nelements())
        ufargs = (ne, self.n_inputs, self.n_outputs,
                  tuple(buffers))
        apply(cached.cfunc, ufargs)

    def _slow_exec(self, inputs, outputs, cached):
        input_convs, output_convs, maxitemsize = cached.ufargs
        niter = _ufunc.getBufferSize()/maxitemsize
        indexlevel, blockingparameters = _ufunc._getBlockingParameters(
            outputs[0]._shape, niter)
        rebuffered_inputs = []
        for i in range(self.n_inputs):
            rebuffered_inputs.append(input_convs[i].rebuffer(inputs[i]))
        rebuffered_outputs = []
        for i in range(self.n_outputs):
            rebuffered_outputs.append(output_convs[i].rebuffer(outputs[i]))
        operator = _Operator(cached.cfunc, rebuffered_inputs,
                             rebuffered_outputs)
        objects = input_convs + (operator,) + output_convs
        _callOverDimensions(objects, outputs[0]._shape, indexlevel,
                            blockingparameters)
        for i in range(len(input_convs)):            
            input_convs[i].clean(inputs[i])
        for i in range(len(output_convs)):
            output_convs[i].clean(outputs[i])


# Portions of the following class are implemented in C. See _convertermodule.c.
# In particular, compute, stride, convert, and rebuffer.
class _Converter(_converter._converter):
    def __init__(self, arr, type, inbuffer=None, forcestride=0, nonstriding=0):
        self.stridefunction = None
        self.convfunction = None
        self.type = type

        if arr.isbyteswapped():
            if isinstance(arr._type, _nt.ComplexType):
                fname = "byteswap" + arr._type.name
            else:
                fname = "byteswap"+`arr._itemsize`+"bytes"
            self.stridefunction = _bytes.functionDict[fname]
        elif not arr.isaligned():
            fname = "align"+`arr._itemsize`+"bytes"
            self.stridefunction = (_bytes.functionDict.get(fname) or
                                   _bytes.functionDict["copyNbytes"])
        elif not nonstriding and not arr.iscontiguous() or forcestride:
            fname = "copy"+`arr._itemsize`+"bytes"
            self.stridefunction = (_bytes.functionDict.get(fname) or
                                   _bytes.functionDict["copyNbytes"])
        if type != arr._type.name:
            self.convfunction = \
                          _numtypedict[type]._conv.astype[arr._type.name]
            
        if self.stridefunction:
            # Since the output array is iterated over dimensions, even
            # the input array, always contiguous, needs strides defined.
            # But first see if input buffer is a real array.
            if (inbuffer is not None) and inbuffer._strides:
                if not inbuffer.is_c_array():
                    raise ValueError("input buffer is not a c_array")
                self.bytestrides = [ inbuffer._strides,
                                     list(arr._strides) ]
                self.generated = 0
            else:
                self.bytestrides = [ _stridesFromShape(arr._shape,
                                                       arr._itemsize),
                                     list(arr._strides) ]
                self.generated = 1
        else:
            self.bytestrides = [ None, None ]
                
        # If inbuffer not specified, allocate one
        if (inbuffer is None) and (self.stridefunction or self.convfunction):
            inbuffer = _bufferPool.getBuffer()
            
        self.conversion_required = 1
        if self.convfunction:
            if self.stridefunction:
                buff = _bufferPool.getBuffer()
                buffers = [inbuffer, buff, buff, arr]
                self.arr_position = 3
                self.inb_position = 0
            else:
                buffers = [inbuffer, arr, None, None]
                self.arr_position = 1
                self.inb_position = 0
        else:
            if self.stridefunction:
                buffers = [None, None,inbuffer, arr]
                self.arr_position = 3
                self.inb_position = 2
            else:
                if inbuffer is not None:
                    raise ValueError(
                     "Specified both input and output but no conversion implied")
                buffers = [None, None, None, None]
                inbuffer = arr
                self.conversion_required = 0

        self.buffers = buffers
        self.result_buff = inbuffer

    def clean(self, arr):
        l = self.buffers
        for i in range(len(l)):
            if l[i] is arr:
                l[i] = None
        self.buffers = l
        if self.result_buff is arr:
            self.result_buff = None

    if _PROTOTYPE:
        # The following method has been re-implemented in C.
        def rebuffer(self, arr, inbuffer=None):
            if not self.conversion_required:
                self.result_buff = arr
                return arr
            l = self.buffers
            l[self.arr_position] = arr
            if not self.generated and inbuffer is not None:
                self.result_buff = inbuffer
                l[self.inb_position] = inbuffer
            self.buffers = l
            if arr is not None and self.bytestrides != [None, None]:
                if self.generated:
                    ustrides = _stridesFromShape(arr._shape, arr._itemsize)
                else:
                    if inbuffer is not None:
                        ustrides = inbuffer._strides
                    else:
                        ustrides = self.bytestrides[ not self.direction ]
                if self.direction:
                    self.bytestrides = [ ustrides, arr._strides ]
                else:
                    self.bytestrides = [ arr._strides, ustrides ]
            return self.result_buff

        def convert(self, buf, indices, shape):
            """Perform type conversions."""
            if self.convfunction:
                niter = _gen.product(shape)
                inoffset = self.buffers[buf*2+0]._getByteOffset(indices)
                offset = self.buffers[buf*2+1]._getByteOffset(indices)
                self.convfunction(niter, 1, 1, 
                                  ((self.buffers[buf*2+0]._data, inoffset),
                                   (self.buffers[buf*2+1]._data, offset)))

        def stride(self, buf, indices, shape):
            """Perform copies and re-alignments."""
            if self.stridefunction:
                inoffset = self.buffers[buf*2+0]._getByteOffset(indices)
                offset = self.buffers[buf*2+1]._getByteOffset(indices)
                self.stridefunction(shape, 
                                    self.buffers[buf*2+0]._data, inoffset,
                                    self.bytestrides[0][-len(shape):],
                                    self.buffers[buf*2+1]._data, offset,
                                    self.bytestrides[1][-len(shape):])

        # The following code has been implemented in C.
        # Renaming "compute" below affects compute, stride, and convert.
        def compute(self, indices, shape):
            """ Perform the necessary transformations on the array.
            """
            if self.direction:
                self.convert(0, indices, shape)
                self.stride(1, indices, shape)
            else:
                self.stride(0, indices, shape)
                self.convert(1, indices, shape)

class _InputConverter(_Converter):
    """This class handles alignment, byteswaping, copying and type conversions

    Stride here is a bit of a misnomer, it refers to one of
    functions (three currently) that deal with strides. These are:
    byteswap, align, copy operations. The conv functions are type conversion
    operations. They never deal with strides. At most one of each is
    necessary; neither is required. Strides and offsets are *byte*!
    """
    def __init__(self, arr, type, inbuffer=None, forcestride=0):
        _Converter.__init__(self, arr, type, inbuffer, forcestride)
        self.direction = 0 # input
            
        if type != arr._type.name:
            self.convfunction = arr._type._conv.astype[type]

        l = self.bytestrides
        l.reverse()
        self.bytestrides = l
        
        l = self.buffers
        l.reverse()
        self.buffers = l
        
        if self.conversion_required:
            self.arr_position = {1:2, 3:0}[self.arr_position]
            self.inb_position = {0:3, 2:1}[self.inb_position]
            

class _OutputConverter(_Converter):
    """This class handles alignment, byteswaping, copying and type
    conversions processing the output of a ufunc.
    """
    def __init__(self, arr, type, inbuffer=None, forcestride=0, nonstriding=0):
        _Converter.__init__(self, arr, type, inbuffer, forcestride,
                            nonstriding)
        self.direction = 1  # output


# The following class has been re-implemented in C.  See _operatormodule.c.
class _Operator:
    """performs the operation"""

    def __init__(self, cfunction, inputs, outputs, striding=0):
        self.cfunction = cfunction
        self.inputs = inputs
        self.outputs = outputs
        self.striding = striding

    def _buffer_offset(self, buffer, indices):
        if buffer._strides:
            return buffer._getByteOffset(indices)
        else:
            return 0

    def _buffer_strides(self, buffer, shape, itemsize):
        if buffer._strides:
            return buffer._strides
        else:
            return _stridesFromShape(shape, itemsize)

    def compute(self, indices, shape):
        if self.striding:
            assert len(self.inputs) == 1 and len(self.outputs) == 1
            input, output = self.inputs[0], self.outputs[0]
            inbuffer, outbuffer = input._data, output._data
            inoffset = self._buffer_offset(input, indices)
            outoffset = self._buffer_offset(output, indices)
            instrides = self._buffer_strides(input, shape, self.striding)
            outstrides = self._buffer_strides(output, shape, self.striding)
            if len(instrides) < len(outstrides):  # hack for reductions
                outstrides = outstrides[len(outstrides)-len(instrides):]
            self.cfunction(shape, inbuffer, inoffset, instrides,
                           outbuffer, outoffset, outstrides)
        else:
            niter = _gen.product(shape)
            args = []
            for item in self.inputs+self.outputs:
                args.append((item._data, self._buffer_offset(item, indices)))
            self.cfunction(
                niter, len(self.inputs), len(self.outputs), tuple(args))

# Replace the Python version of "_Operator" with a C version.
if not _PROTOTYPE:
    from _operator import _operator as _Operator

def _stridesFromShape(shape, bytestride):

    """Compute the strides from shape for a contiguous array, sort of"""
    if shape != ():
        ndim = len(shape)
        strides = [0]*ndim
        strides[-1] = bytestride
        for i in xrange(ndim-2, -1, -1):
            strides[i] = strides[i+1] * shape[i+1]
    else:
        strides = ()
    return strides

def _makeCUFuncDict(functionDict):
    """Organize C functions by function name and signature"""
    dict = {}
    for keystr in functionDict.keys():
        operator, optype, signature = keystr
        if not dict.has_key(operator):
            dict[operator] = {}
        if not dict[operator].has_key(optype):
            dict[operator][optype] = {}
        dict[operator][optype][signature[0]] = (signature[1],functionDict[keystr])
    return dict

def _isScalar(x):
    return isinstance(x, _nt.scalarTypes)

def _maxPopType(xs):
    """_maxPopType determines the maximum type of a sequence of
    numarray and scalars.  see _nt.genericCoercions.
    """
    if isinstance(xs[0], types.InstanceType) and \
       not isinstance(xs[0], _nc.NumArray):
        return None  # If they're not all NumArrays, quit now.

    maxT = None
    for x in xs:
        if isinstance(x, _nc.NumArray):
            t = x._type
        elif _nc.PyNUMERIC_TYPES.has_key(type(x)):
            t = type(x)
        else:
            t = _nc._maxtype(x)
            
        if maxT is None:
            maxT = t
        else:
            maxT = _nt.genericCoercions[(maxT, t)]
    return maxT

CLIP = 0    # Peg indices > N-1 at N-1, indices < 0 at 0
WRAP = 1    # Index = Index % N
RAISE = 2   # Raise an exception for out of range indices

class _ChooseUFunc(_UFunc):
    
    """Class for building the "choose" ufunc.
    Class for ufuncs with 2 input (S, [~S...]) and 1 output (S) arguments"""

    def _doit(self, computation_mode, outarr, cfunc, ufargs):
        _ufunc.CheckFPErrors()
        if computation_mode == "fast":
            apply(cfunc, ufargs)
        else:
            inputs, outputs, preprocessing_outbuffers, \
            postprocessing_inbuffers, maxitemsize = ufargs
            niter = _ufunc.getBufferSize()/maxitemsize

            outshape = outarr._shape
            indexlevel, blockingparameters = \
                _ufunc._getBlockingParameters(outshape, niter)
            operator = _Operator(cfunc, preprocessing_outbuffers,
                                 postprocessing_inbuffers)
            objects = inputs + (operator,)+ outputs
            _callOverDimensions(objects, outshape, indexlevel,
                                blockingparameters)
            
        errorstatus = _ufunc.CheckFPErrors()
        if errorstatus:
            handleError(errorstatus, " in "+self.operator)
        return outarr

    def __call__(self, inarr1, inarr2, outarr=None, clipmode=RAISE):
        """The standard calling interface for UFuncs"""
        computation_mode, woutarr, cfunc, ufargs = \
                          self._setup(inarr1, inarr2, outarr, clipmode)

        result = self._doit(computation_mode, woutarr, cfunc, ufargs)
        if outarr is not None:
            outarr = (outarr,)
        return _normalize_results((inarr1,)+tuple(inarr2), outarr,
                                  (result,), False)

    def _setup(self, in1, in2, outarr=None, clipmode=RAISE):
        """Setup for choose()"""

        in1 = _nc.asarray(in1, type=MaybeLong)
        in2 = list(in2)

        if outarr is None:
            convType = _maxPopType(in2)
        else:
            convType = outarr._type

        for i in range(len(in2)):
            if isinstance(in2[i], _gen.NDArray):
                pass
            else:
                in2[i] = _nc.array(in2[i], type=convType)

        result = _gen._nWayBroadcast( [in1] + in2 )
        in1, in2 = result[0], result[1:]

        if outarr is None:
            outarr = in1.new(convType)
            
        if in1._shape != outarr._shape:
            raise ValueError("Supplied output array does not have appropriate shape")
        N = outarr._itemsize
        if N in [1,2,4,8,16]:
            ucfname = "choose" + `N` + "bytes"
            cfunc = _bytes.functionDict[ucfname]
        else:
            cfunc = _bytes.functionDict["chooseNbytes"]

        # pars = clipmode, population count, itemsize
        pars = _pseudoArrayFromScalars([clipmode, len(in2),N],
                                       _nt.MaybeLong)
        args = [pars, in1] + in2 + [outarr]
        fastargs = reduce(and_,
            [ _noConversionNeeded(x, convType) for x in in2 + [outarr]])
        fastargs = fastargs and _noConversionNeeded(in1, _nt.MaybeLong)
        if fastargs:
            # If fast case, return the arguments for an immediate call to the
            #  C function
            assert outarr._byteoffset == 0
            fastparms = tuple([ (x._data, x._byteoffset) for x in args])
            return ("fast", outarr, cfunc,
                    (outarr.nelements(), len(args)-1, 1, fastparms))

        # slower, general case
        cpars         = _InputConverter(pars, _nt.MaybeLong)
        selector      = _InputConverter(in1, _nt.MaybeLong)
        population    = [_InputConverter(x, convType) for x in in2 ]

        inputs = (cpars, selector,) + tuple(population)
        outputs = (_OutputConverter(arr=outarr, type=convType),)


        alltypes = [_numtypedict[convType], _nt.MaybeLong] + \
                   [a.type() for a in in2]
        maxitemsize = max([t.bytes for t in alltypes])
        preprocessing_outbuffers = tuple([x.result_buff for x in inputs])
        postprocessing_inbuffers = tuple([x.result_buff for x in outputs])
        return ("slow", outarr, cfunc, 
                (inputs, outputs, preprocessing_outbuffers,
                 postprocessing_inbuffers, maxitemsize))

_choose = _ChooseUFunc("choose", [], 0, 0, None)

def choose(selector, population, outarr=None, clipmode=RAISE):
    """
    choose() returns a new array shaped like 'selector' with elements
    chosen from members of sequence 'population' by the values of
    selector.  The shape of each member of 'population' must be
    broadcastable to the shape of 'selector'.  The value of each
    member of 'selector' must satisfy: 0 <= value < len(population).

    clipmode=RAISE  if clipmode is CLIP, out of range selector values
                        are clipped at [0, shape[i]).

                    if clipmode is WRAP, out of range selector values
                        are wrapped around from 0 to shape[i] (<0) or
                        from shape[i] to 0 (>= shape[i]).

                    if clipmode is RAISE, selector values out of range
                       [0, shape[i]) result in an exception.
    """
    return _choose(selector, population, outarr, clipmode)

def _scatteredPseudos( scattered ):
    scatteredStrides = _pseudoArrayFromScalars(scattered._strides,
                                               type=MaybeLong)
    scatteredShape   = _pseudoArrayFromScalars(scattered._shape,
                                               type=MaybeLong)
    return scatteredStrides, scatteredShape

class _TakeUFunc(_ChooseUFunc):
    """take(scattered, indexArrays, gathered=None)

    scattered:     input,        array from which elements are "gathered"

    indexArrays:   input,        tuple of index numarray or scalars

    gathered:      input/output  result array

    take plucks the elements of 'scattered' specified by the tuple of
    index numarray, 'indexArrays', and stores the result in the output array
    'gathered'.

    'indexArrays' may be partial, i.e.  incompletely specified,
    hence the shape of 'gathered' is derived from both the shape of
    'indexArrays' (primarily) and the shape of the trailing unspecified
    dimensions of 'scattered'.
    """
    def _setup(self, scattered, indexArrays, gathered=None, clipmode=RAISE):
        """Setup for Nary ufunc
        """
        scattered = _nc.asarray(scattered)
        indexArrays = list(indexArrays)
        gatheredWasSpecified = (gathered is not None)
        if gatheredWasSpecified:
            ctype = gathered._type
        else:
            ctype = scattered._type

        # *All* of the scattered array must be simultaneously available.
        #    So blocked conversions won't work.
        if not _noConversionNeeded(scattered, ctype):
            scattered = scattered.astype(ctype)

        impliedShape, N = _gen._takeShape(scattered, indexArrays)
        scatteredStrides, scatteredShape = _scatteredPseudos(scattered) 

        if gatheredWasSpecified:
            if gathered._shape != impliedShape:
                raise ValueError("Inconsistent array shapes...")
        else:
            gathered = scattered.__class__(shape=impliedShape,
                                           type=scattered._type)
        # scattered and gathered should now be identically typed.

        # Since N can be arbitrarily large depending on the trailing
        # dimensions of scattered, may have to make gathered contiguous
        # to ensure a buffer large enough to store at least N.
        # if (N > _ufunc.getBufferSize()
        
        if not _noConversionNeeded(gathered, scattered._type):
            raise ValueError("take() destination array must be contiguous, aligned, not byteswapped, and of the same type as the source array")

        niter = (gathered.nelements()*gathered._itemsize)/N
        cfunc = _bytes.functionDict[self.operator + "Nbytes"]

        pars = _pseudoArrayFromScalars([clipmode, N], type=MaybeLong)

        # Index arrays must be aligned, not byteswapped, and correctly typed.
        #  Throw in contiguous for now.
        for i in range(len(indexArrays)):
            x = indexArrays[i]
            if not _noConversionNeeded(x, MaybeLong):
                indexArrays[i] = x.astype(MaybeLong)

        args = [pars, scattered, scatteredStrides, scatteredShape] + \
               indexArrays + [gathered]
        fastparms = tuple([ (x._data, x._byteoffset) for x in args])
        return ("fast", gathered, cfunc, (niter, len(args)-1, 1, fastparms))

_take = _TakeUFunc("take", [], 0, 0, None)

def take(array, indices, axis=0, outarr=None, clipmode=RAISE):    
    """take() picks elements of 'array' specified by sequence of
    numerical sequences 'indices'.

    parameters which must be specified by keyword:

    array           data to be indexed & collected (taken from).

    indices         An integer sequence, or tuple of integer sequences
                    specifying the array coordinates from which data
                    is to be taken.  Partial indices result in entire
                    inner blocks being taken.

    axis=0          selects the axis (or axes) along which the take
                    should be performed.

    clipmode=RAISE  if clipmode is CLIP, out of range indices are clipped
                        at [0, shape[i]).

                    if clipmode is WRAP, out of range indices are wrapped
                        around from 0 to shape[i] (<0) or from shape[i] to
                        0 (>= shape[i]).

                    if clipmode is RAISE, indices in the range -N..N-1 are
                       treated like Python sequence indices.  Out of range
                       indices result in an exception.
    """
    if axis == 0:
        array = _nc.asarray(array)
        return array._take((indices,), outarr=outarr, clipmode=clipmode)
    elif isinstance(axis, (int, long)):
        if isinstance(indices, (int,long,float)):
            raise ValueError("indices must be a sequence")
        work = _gen.swapaxes(array, 0, axis)
        work = work._take((indices,), outarr=outarr, clipmode=clipmode)
        return _gen.swapaxes(work, 0, axis)
    else:
        def_axes = range(array.rank)
        for x in axis:
            def_axes.remove(x)
        axis = list(axis) + def_axes
        work = _gen.transpose(array, axis)
        return work._take(indices, outarr=outarr, clipmode=clipmode)

def _nonIteratedArray(a):
    ni = a.view()
    ni._shape=(0,)
    ni._strides = (0,)
    ni._contiguous = 1
    return ni

class _PutUFunc(_TakeUFunc):
    """put(scattered, indexArrays, gathered) is the inverse function of "take",
    and scatters the elements of array 'gathered' across array 'scattered' as
    specified by tuple of index numarray, 'indexArrays'.

    scattered: input/output           Array where "stuff" gets put
    indexArrays: input                Arrays of indexArrays of scattered where
                                         corresponding pieces of gathered go.
    gathered: input                  Source array

    gathered can be an array which is broadcastable to the array shape
    implied by scattered and indexArrays  -or-  gathered can be a scalar.
    """
    def _setup(self, scattered, indexArrays, gathered, clipmode=RAISE):
        """Setup for Nary ufunc
        """
        indexArrays = list(indexArrays)

        if ((len(indexArrays) < len(scattered._shape)) and
            (scattered.isbyteswapped() or not scattered.iscontiguous())):
            raise ValueError("Invalid destination array: partial indices require contiguous non-byteswapped destination")

        impliedShape, N = _gen._takeShape(scattered, indexArrays)
        scatteredStrides, scatteredShape = _scatteredPseudos(scattered)
        scattered = _nonIteratedArray(scattered)

        pars = _pseudoArrayFromScalars([clipmode, N], type=MaybeLong)

        gathered = _nc.asarray(gathered, type=scattered._type)
        gathered = _gen._broadcast(gathered, impliedShape)

        if not gathered.iscontiguous():
            gathered = gathered.copy()

        # gathered and scattered should now be identically typed.
        # gathered need not be contiguous since it is "bufferable".
        # Since N can be arbitrarily large depending on the trailing
        # dimensions of scattered, may have to pre-process gathered.
        # if N > _ufunc.getBufferSize() and \
        if not _noConversionNeeded(gathered, scattered._type):
            gathered = gathered.astype( scattered._type )

        niter = (gathered.nelements()*gathered._itemsize)/N
        cfunc = _bytes.functionDict[self.operator + "Nbytes"]

        for i in range(len(indexArrays)):
            x = indexArrays[i]
            if not _noConversionNeeded(x, MaybeLong):
                indexArrays[i] = x.astype(MaybeLong)
        
        args = [pars, gathered, scatteredStrides, scatteredShape] + \
               indexArrays + [scattered]
        fastparms = tuple([ (x._data, x._byteoffset) for x in args])
        return ("fast", scattered, cfunc, (niter, len(args)-1, 1, fastparms))

_put = _PutUFunc("put", [], 0, 0, None)

def put(array, indices, values, axis=0, clipmode=RAISE):
    """put(array, indices, values, clipmode=RAISE, axis=0)
    stores 'values' into 'array' at 'indices...'.

    parameters which must be specified by keyword:

    array           data to be indexed & stuffed (put to).

    indices         An integer sequence, or tuple of integer sequences
                    specifying the array coordinates to hich data
                    is to be put.  Partial indices result in entire
                    inner blocks being overwritten.

    values          A sequence of values to be written to the specified
                    indices of array.

    axis=0          selects the axis along which the put should be performed.
    
    clipmode=RAISE  if clipmode is CLIP, out of range indices are clipped
                        at [0, shape[i]).

                    if clipmode is WRAP, out of range indices are wrapped
                        around from 0 to shape[i] (<0) or from shape[i] to
                        0 (>= shape[i])

                    if clipmode is RAISE, indices in the range -N..N-1 are
                       treated like Python sequence indices.  Out of range
                       indices result in an exception.
    """
    if not isinstance(array, _gen.NDArray):
        raise TypeError("put only works on NDArray and its subclasses")

    work = _nc.asarray(array)
    if not work.is_c_array():
        work = work.copy()
    if axis == 0:
        work._put((indices,), values, clipmode=clipmode)
    elif isinstance(axis, (int, long)):
        if isinstance(indices, (int,long,float)):
            raise ValueError("indices must be a sequence")
        work = _gen.swapaxes(work, 0, axis)
        work._put((indices,), values, clipmode=clipmode)
        work = _gen.swapaxes(work, 0, axis)
    else:
        def_axes = range(work.rank)
        for x in axis:
            def_axes.remove(x)
        axis = list(axis) + def_axes
        work = _gen.transpose(work, axis)
        work._put(indices, values, clipmode=clipmode)
        work = _gen.transpose(work, axis)
    if work is not array:
        if isinstance(array, _gen.NDArray):
            array._copyFrom(work)

class _NonzeroUFunc:
    """nonzero(array)

    array: input,        array scanned for non-zero elements

    Nonzero scans "array" for nonzero elements, and returns a tuple
    of coordinate numarray corresponding to the nonzero elements.

    """
    def __call__(self, inarr1):

        """The standard calling interface for UFuncs"""
        nz = _nc.asarray(not_equal(inarr1, 0))
        if nz.rank < 1:
            nz.shape = (1,)
        nonzeroCount = add.reduce(_gen.ravel(nz).astype(MaybeLong))
        outarr = [ _nc.NumArray(shape=(nonzeroCount,), type=Long)
                   for i in range(nz.rank) ]
        sstrides = _nc.array(nz._strides, type=MaybeLong)
        sstrides /= nz._bytestride

        niter = nz.nelements()
        cfunc = _sort.functionDict[repr(("Bool", "nonzeroCoords"))]

        args = [ nz, sstrides] + outarr
        fastparms = tuple([ (x._data, x._byteoffset) for x in args])
        cfunc(niter, 2, len(args)-2, fastparms)
        return tuple(outarr)

def searchsorted(bins, values):
    """searchsort(bins, values) returns the array A[j] of
    greatest indices 'i' such that each values[j] <= bins[i].
    """
    bins = _nc.asarray(bins)
    values = _nc.asarray(values)
    if len(bins._shape) > 1 or len(values._shape) > 1:
        raise ValueError("Multi-dimensional searchsort not supported.")
    outarr = _nc.NumArray(shape=values.shape, type=Long)
    maxtype = bins._type
    if maxtype < values._type:
        maxtype = values._type
    if not _noConversionNeeded(bins, maxtype):
        bins = bins.astype(maxtype)
    if not _noConversionNeeded(values, maxtype):
        values = values.astype(maxtype)
    cfunc = _sort.functionDict[repr((bins._type.name, 'searchsorted'))]
    nbins = _pseudoArrayFromScalars([len(bins)], MaybeLong)
    args = [(x._data, x._byteoffset) for x in [nbins, bins, values, outarr]]
    cfunc(values.nelements(), 3, 1, tuple(args))
    if outarr.rank == 0:
        return outarr[()]
    else:
        return outarr

nonzero = _NonzeroUFunc()

def _sort1(inarr1):
    """1D in-place Sort"""
    conversionNeeded = not _noConversionNeeded(inarr1, inarr1._type)
    contigarr1 = _nc.array(inarr1, copy=conversionNeeded)
    cfunc = _sort.functionDict[repr((contigarr1._type.name, 'sort'))]
    cfunc(contigarr1.nelements(), 0, 1,
          ((contigarr1._data, contigarr1._byteoffset),))
    if conversionNeeded:
        inarr1._copyFrom(contigarr1)
 
def _argsort1(inarr1, witness=None):
    """1D ArgSort"""
    niter = inarr1.nelements()
    inarr1 = _nc.array(inarr1) # make a discardable copy
    if witness is None:
        witness =_nc.arange(niter,type=Long)
    elif not _noConversionNeeded(witness, Long):
        raise ValueError("No support for converting the witness array")
    cfunc = _sort.functionDict[repr((inarr1._type.name,'asort'))]
    cfunc(niter, 0, 2,
          tuple([ (x._data, x._byteoffset) for x in [inarr1, witness]]))
    return witness

## # =====================================================================


def _fbroadcast(f, N, shape, args, params=()):
    """_fbroadcast(f, N, args, shape, params=()) calls 'f' for each of the
    'N'-dimensional inner subnumarray of 'args'.  Each subarray has
    .shape == 'shape'[-N:].  There are a total of product(shape[:-N])
    calls to 'f'.
    """
    if len(shape) == N:
        apply(f, tuple(args)+params)
    else:
        for i in range(shape[0]):
            _fbroadcast(f, N, shape[1:], [x[i] for x in args], params)
            
            
def _sortN(a):
    """_sortN implements N-D in-place sort in terms of 1D sort"""
    _fbroadcast(_sort1, 1, a._shape, (a,))

def _argsortN(a, w):
    """_argsortN implements N-D in-place argsort in terms of 1D argsort"""
    _fbroadcast(_argsort1, 1, a._shape, (a,w))

#
# Direct sort types
#

def _broadcast_direct_sort(a, name):
    """Helper function to broadcast direct sorts.

    Need to finish error handling.
    """
    if len(a.shape) == 1 :
        cfunc = _sort.functionDict[repr((a._type.name, name))]
        if  a.is_c_array()  :
            err = cfunc(a.nelements(), 0, 1, ((a._data, a._byteoffset),))
        else :
            b = _nc.array(a, copy=1)
            err = cfunc(b.nelements(), 0, 1, ((b._data, b._byteoffset),))
            a._copyFrom(b)
    else :
        for i in range(a.getshape()[0]) :
            _broadcast_direct_sort(a[i], name)

def _direct_sort(a, name, axis=-1):
    """Sorts an array in-place along the specified axis.

    It takes care of the sort axis.
    """
    if axis==-1:
        _broadcast_direct_sort(a, name)
    else:
        a.swapaxes(axis,-1)
        _broadcast_direct_sort(a, name)
        a.swapaxes(axis,-1)

#
# Indirect sort types.
#

def _broadcast_indirect_sort(a, w, name) :
    """Helper function to broadcast indirect sorts.

    The witness array 'w' is assumed set in _indirect_sort
    It must be of the same shape as a, be of c_type, and
    contain longs. It is not checked. Need to finish error
    handling.
    """
    if len(a.shape) == 1 :
        cfunc = _sort.functionDict[repr((a._type.name, name))]
        if  a.is_c_array()  :
            err = cfunc(a.nelements(), 1, 1, ((a._data, a._byteoffset), (w._data, w._byteoffset)))
        else :
            b = _nc.array(a, copy=1)
            err = cfunc(a.nelements(), 1, 1, ((b._data, b._byteoffset), (w._data, w._byteoffset)))
            a._copyFrom(b)
    else :
        for i in range(a.getshape()[0]) :
            _broadcast_indirect_sort(a[i], w[i], name)


def _indirect_sort(a, name, axis=-1) :
    """Helper function to setup indirect sorts.

    It takes care of the sort axis and  sets
    up the witness array.
    """
    if axis == -1 :
        ashape = a.getshape()
        w = _nc.array(shape=ashape, type=_nt.Long)
        w[...,:] = _nc.arange(ashape[-1], type=_nt.Long)
        _broadcast_indirect_sort(a, w, 'a' +  name)
        return w
    else :
        a.swapaxes(axis,-1)
        w =  _indirect_sort(a, name, axis=-1)
        a.swapaxes(axis,-1)
        w.swapaxes(axis,-1)
        return w

                  

def divide_remainder(a,b):
    """returns (a/b, a%b)."""
    a, b = _nc.asarray(a), _nc.asarray(b)
    return (a/b,a%b)


Long = _nt.Long
MaybeLong = _nt.MaybeLong

# module-level variables
_numtypedict = _nt.typeDict

# Short term solution -- identity needs to be defined in _ufuncall module XXX
_identities = {"add":0, "subtract":0, "multiply":1, "divide":1,
               "bitwise_and":1, "bitwise_or":0, "bitwise_xor":0,
               "logical_and":1, "logical_or":0, "logical_xor":0,
               }

def ufuncFactory(operator, cfuncs, identity=None):
    """Create UFunc instance based on signature of functions"""
    ninputs, noutputs = _nIOArgs(cfuncs)
    if ninputs==1 and noutputs==1:
        return _UnaryUFunc(operator, cfuncs, 1, 1, identity)
    elif ninputs==2 and noutputs==1:
        return _BinaryUFunc(operator, cfuncs, 2, 1, identity)
    else:
        return _NaryUFunc(operator, cfuncs, ninputs, noutputs, identity)

def make_ufuncs(m):
    """Creates a dictionary of UFunc objects from a C module."""
    _cufuncs = _makeCUFuncDict(m.functionDict)
    _UFuncs = {}
    for operator in _cufuncs.keys():
        _UFuncs[operator] = ufuncFactory(operator, _cufuncs[operator],
                                         identity=_identities.get(operator))
    return _UFuncs

_UFuncs = make_ufuncs(_ufuncall)
globals().update(_UFuncs)
