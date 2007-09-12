"""This module generates the "bytes" module which contains various
byte munging C functions: copying, alignment, byteswapping, choosing,
putting, taking.

WARNING: This module exists solely as a mechanism to generate a
portion of numarray and is not intended to provide any
post-installation functionality.  
"""

from basecode import CodeGenerator, template, _HEADER

BYTES_HEADER       = _HEADER + \
'''
#include <assert.h>

#define NA_ACOPYN(i, o) memcpy(o, i, N)

/* The following is used to copy nbytes of data for each element.   **
** As such it can be used to align any sort of data provided the    **
** output pointers used are aligned                                 */

static int copyNbytes(long dim, long nbytes, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i, j;
    char *tin  = (char *) input  + inboffset;
    char *tout = (char *) output + outboffset;
    if (dim == 0) {
        for (i=0; i<niters[dim]; i++) {
            for (j=0; j<nbytes; j++) {
                *tout++ = *tin++;
            }
            tin = tin + inbstrides[dim] - nbytes;
            tout = tout + outbstrides[dim]- nbytes;
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            copyNbytes(dim-1, nbytes, niters,
                input,  inboffset  + i*inbstrides[dim],  inbstrides,
                output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
    return 0;
}

STRIDING_DESCR2(copyNbytes, !CHECK_ALIGN, -1, -1);

/* Copy a data buffer to a new string
**
** Arguments:
**
**   Tuple of iteration values for each dimension of input array.
**   Input buffer object.
**   Input byte offset.
**   Tuple of input byte strides.
**   Size of input data item in bytes.
**
** Returns Python string.
*/

static PyObject *copyToString(PyObject *self, PyObject *args) {
    PyObject *inbuffObj;
    PyObject *nitersObj, *inbstridesObj;
    PyObject *otemp, *outstring;
    long ltemp;

    int nniters, ninbstrides, nargs;
    long nbytes;
    maybelong niters[MAXDIM], inbstrides[MAXDIM], outbstrides[MAXDIM];
    void *inbuffer, *outbuffer;
    long i, inbsize, outbsize, nelements=1, inboffset;
    
    nargs = PyObject_Length(args);
    if (!PyArg_ParseTuple(args, "OOlOl",
            &nitersObj, &inbuffObj,  &inboffset, &inbstridesObj, &nbytes))
        return NULL;
        
    if (!PySequence_Check(nitersObj))
        return PyErr_Format(PyExc_TypeError,
                   "copyToString: invalid shape object");
    if (!PySequence_Check(inbstridesObj))
        return PyErr_Format(PyExc_TypeError,
                   "copyToString: invalid strides object");
        
    nniters = PyObject_Length(nitersObj);
    ninbstrides = PyObject_Length(inbstridesObj);
    if (nniters != ninbstrides)
        return PyErr_Format(PyExc_ValueError,
        "copyToString: shape & strides don't match");
        
    for (i=nniters-1; i>=0; i--) {
        otemp = PySequence_GetItem(nitersObj, i);
        if (PyInt_Check(otemp))
           ltemp = PyInt_AsLong(otemp);
        else if (PyLong_Check(otemp))
           ltemp = PyLong_AsLong(otemp);
        else
           return PyErr_Format(PyExc_TypeError,
                 "copyToString: non-integer shape element");
        nelements *= ltemp;
        niters[nniters-i-1] = ltemp;
        Py_DECREF(otemp);
        otemp = PySequence_GetItem(inbstridesObj, i);
        if (PyInt_Check(otemp))
            inbstrides[nniters-i-1] = PyInt_AsLong(otemp);
        else if (PyLong_Check(otemp))
            inbstrides[nniters-i-1] = PyLong_AsLong(otemp);
        else
           return PyErr_Format(PyExc_TypeError,
                 "copyToString: non-integer stride element");
        Py_DECREF(otemp);
    }
    if (!nelements)
       return PyString_FromStringAndSize("", 0);
    outbstrides[0] = nbytes;
    for (i=1; i<nniters; i++) {
        outbstrides[i] = outbstrides[i-1]*niters[i-1];
    }
    outbsize = outbstrides[nniters-1]*niters[nniters-1];
    outstring = PyString_FromStringAndSize(NULL, outbsize);
    if (!outstring)
        return NULL;
    outbuffer = (void *) PyString_AsString(outstring);
    
    if ((inbsize = NA_getBufferPtrAndSize(inbuffObj, 1, &inbuffer)) < 0)
       return PyErr_Format(PyExc_TypeError,
           "copyToString: Problem with array buffer");

    if (NA_checkOneStriding("copyToString", nniters, niters,
			  inboffset, inbstrides, inbsize, nbytes, 0) ||
	NA_checkOneStriding("copyToString", nniters, niters,
			  0, outbstrides, outbsize, nbytes, 0))
                          return NULL;

    BEGIN_THREADS
    copyNbytes(nniters-1, nbytes, niters,
          inbuffer, inboffset, inbstrides, outbuffer, 0, outbstrides);
    END_THREADS
    return outstring;
}

/* chooseXbytes functions are called as uFuncs... */

enum CLIP_MODE {
  CLIPPED,
  WRAPPED,
  RAISE
};

#define wrap(i, max) \
     while(i < 0)    \
         i += max;   \
     while(i >= max)  \
        i -= max;

static int takeNbytes(long niter, long ninargs, long noutargs,
                         void **buffers, long *bsizes)
{
  maybelong  i, cMode, N;
  maybelong *scatteredstrides, *scatteredshape, **indices;
  char  *gathered, *scattered;
  maybelong nindices = ninargs-4, outi = ninargs+noutargs-1;

  if (NA_checkIo("takeNbytes", 4, 1, MIN(ninargs, 4), noutargs))
     return -1;

  if (nindices == 0)
     return 0;
  
  if (NA_checkOneCBuffer("takeNbytes", 2, buffers[0], bsizes[0], sizeof(maybelong)))
     return -1;
  else {
     cMode            =  ((maybelong *) buffers[0])[0];
     N                =  ((maybelong *) buffers[0])[1];
  }

  if (NA_checkOneCBuffer("takeNbytes", nindices, buffers[2], bsizes[2], sizeof(maybelong)))
     return -1;
  else {
     scatteredstrides =  (maybelong *)  buffers[2];
  }
  
  if (NA_checkOneCBuffer("takeNbytes", nindices, buffers[3], bsizes[3], sizeof(maybelong)))
     return -1;
  else {
     scatteredshape   =  (maybelong *)  buffers[3];
  }
  
  if (NA_checkOneStriding("takeNBytes", nindices, scatteredshape, 0, scatteredstrides, bsizes[1], N, 0))
     return -1;
  else
     scattered        =  (char *)   buffers[1];

  for(i=4; i<nindices; i++)
     if (NA_checkOneCBuffer("takeNbytes", niter, buffers[i], bsizes[i], sizeof(maybelong)))
        return -1;
  indices          =  (maybelong **) &buffers[4];

  if (NA_checkOneCBuffer("takeNbytes", niter*N, buffers[outi], bsizes[outi], 1))
     return -1;
  else 
     gathered         =  (char *)  buffers[ninargs+noutargs-1];

  switch( cMode )
    {
    case WRAPPED:
      for(i=0; i<niter; i++)
        {
          maybelong j, index;
          for(j=index=0; j<nindices; j++)
            {
              maybelong k = indices[j][i];
	      wrap(k, scatteredshape[j]);
              index += scatteredstrides[j]*k;
            }
          memcpy( &gathered[i*N], scattered+index, N);
        }
      break;
    case CLIPPED:
    default:
      for(i=0; i<niter; i++)
        {
          maybelong j, index;
          for(j=index=0; j<nindices; j++)
            {
              maybelong k = indices[j][i];
              if (k < 0)
                k = 0;
              else if (k >= scatteredshape[j])
                k = scatteredshape[j]-1;
              index += scatteredstrides[j]*k;
            }
          memcpy( &gathered[i*N], scattered+index, N);
        }
      break;
      case RAISE:
      for(i=0; i<niter; i++)
        {
          maybelong j, index;
          for(j=index=0; j<nindices; j++)
            {
              maybelong k = indices[j][i];
              if (k < 0)
                k += scatteredshape[j];
              if (k >= scatteredshape[j]) {
                 PyErr_Format(PyExc_IndexError, "Index out of range");
                 return -1;
              }
              index += scatteredstrides[j]*k;
            }
          memcpy( &gathered[i*N], scattered+index, N);
        }
      break;
    }
  return 0;
}

SELF_CHECKED_CFUNC_DESCR(takeNbytes, CFUNC_UFUNC);

static int putNbytes(long niter, long ninargs, long noutargs,
                         void **buffers, long *bsizes)
{
  maybelong  i, cMode, N;
  maybelong  *scatteredstrides, *scatteredshape, **indices;
  char   *gathered, *scattered;
  long nindices = ninargs-4, outi = ninargs+noutargs-1;

  if (nindices == 0)
     return 0;
  
  if (NA_checkIo("putNbytes", 4, 1, MIN(ninargs, 4), noutargs))
     return -1;

  if (NA_checkOneCBuffer("putNbytes", 2, buffers[0], bsizes[0], sizeof(maybelong)))
     return -1;
  else {
     cMode            =  ((maybelong *) buffers[0])[0];
     N                =  ((maybelong *) buffers[0])[1];
  }

  if (NA_checkOneCBuffer("putNbytes", niter*N, buffers[1], bsizes[1], 1))
     return -1;
  else 
     gathered  =  (char *)  buffers[1];

  if (NA_checkOneCBuffer("putNbytes", nindices, buffers[2], bsizes[2], sizeof(maybelong)))
     return -1;
  else {
     scatteredstrides =  (maybelong *)  buffers[2];
  }
  
  if (NA_checkOneCBuffer("putNbytes", nindices, buffers[3], bsizes[3], sizeof(maybelong)))
     return -1;
  else {
     scatteredshape   =  (maybelong *)  buffers[3];
  }

  for(i=4; i<nindices; i++)
     if (NA_checkOneCBuffer("putNbytes", niter, buffers[i], bsizes[i], sizeof(maybelong)))
        return -1;
  indices          =  (maybelong **) &buffers[4];

  if (NA_checkOneStriding("putNBytes", nindices, scatteredshape, 0, scatteredstrides, bsizes[outi], N, 0))
    return -1;
  else
    scattered        =  (char *)   buffers[outi];

  switch( cMode )
    {
    case WRAPPED:
      for(i=0; i<niter; i++)
        {
          maybelong j, index;
          for(j=index=0; j<nindices; j++)
            {
              maybelong k = indices[j][i];
	      wrap(k, scatteredshape[j]);
              index += scatteredstrides[j]*k;
            }
          memcpy( scattered+index, &gathered[i*N], N);
        }
      break;
    case CLIPPED:
    default:
      for(i=0; i<niter; i++)
        {
          maybelong j, index;
          for(j=index=0; j<nindices; j++)
            {
              maybelong k = indices[j][i];
              if (k < 0)
                k = 0;
              else if (k >= scatteredshape[j])
                k = scatteredshape[j]-1;
              index += scatteredstrides[j]*k;
            }
          memcpy( scattered+index, &gathered[i*N], N);
        }
      break;
    case RAISE:
      for(i=0; i<niter; i++)
        {
          maybelong j, index;
          for(j=index=0; j<nindices; j++)
            {
              maybelong k = indices[j][i];
              if (k < 0)
                k += scatteredshape[j];
              if (k >= scatteredshape[j]) {
                 PyErr_Format(PyExc_IndexError, "Index out of range");
                 return -1;              
              }
              index += scatteredstrides[j]*k;
            }
          memcpy( scattered+index, &gathered[i*N], N);
        }
      break;
    }
  return 0;
}

SELF_CHECKED_CFUNC_DESCR(putNbytes, CFUNC_UFUNC);
'''

COPY_TEMPLATE = \
'''
/*******************************************
*                                          *
* These copy data to a contiguous buffer.  *
* They do not handle non-aligned data.     *
* Offsets and Strides are in byte units    *
*                                          *
*******************************************/

static int copy<size>bytes(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    char *tin     = (char *) input  + inboffset;
    char *tout    = (char *) output + outboffset;
    if (dim == 0) {
        for (i=0; i<niters[dim]; i++) {
            NA_ACOPY<size>(tin, tout);
            tin  += inbstrides[dim];
            tout += outbstrides[dim];
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            copy<size>bytes(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
    return 0;
}

STRIDING_DESCR2(copy<size>bytes, CHECK_ALIGN, <size>, <size>);
'''

ALIGN_TEMPLATE  = \
'''
static int align<size>bytes(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
	return copyNbytes(dim, <size>, niters, input, inboffset, inbstrides, 
			  output, outboffset, outbstrides);
}

STRIDING_DESCR2(align<size>bytes, !CHECK_ALIGN, <size>, <size>);

'''

BYTESWAP_TEMPLATE = \
'''
/******* byteswap *****/

static int byteswap<sizename>(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {

    long i;
    char *tin  = (char *) input  + inboffset;
    char *tout = (char *) output + outboffset;
    if (dim == 0) {
        for (i=0; i<niters[dim]; i++) {
            char t[<size>];
            NA_COPY<size>(tin, t);
            <swapkind><size>(t, tout);
            tin  += inbstrides[dim];
            tout += outbstrides[dim];
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            byteswap<sizename>(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
    return 0;
}

STRIDING_DESCR2(byteswap<sizename>, !CHECK_ALIGN, <size>, <size>);

'''

CHOOSE_TEMPLATE = \
'''
static int choose<size>bytes(long niter, long ninargs, long noutargs,
                         void **buffers, long *bsizes)
{
  maybelong i, cMode, maxP, N, *selector;
  char **population, *output;
  int outi = ninargs + noutargs - 1;

  if (NA_checkIo("choose<size>bytes", 2, 1, MIN(ninargs,2), noutargs))
     return -1;

  if (NA_checkOneCBuffer("choose<size>bytes", 2, buffers[0], bsizes[0], sizeof(maybelong)))
     return -1;
  else {
     cMode        =  ((maybelong *) buffers[0])[0];
     N            =  ((maybelong *) buffers[0])[1];
  }

  if (NA_checkOneCBuffer("choose<size>bytes", niter, buffers[1], bsizes[1],
     sizeof(maybelong)))
        return -1;
  else
     selector     =  (maybelong *)  buffers[1];

  if (ninargs-2 == 0)
    return 0;
  else
    maxP = ninargs-2;
  for(i=2; i<ninargs; i++)
     if (NA_checkOneCBuffer("choose<size>bytes", niter,
        buffers[i], bsizes[i], <size>))
           return -1;
  population      =  (char **) &buffers[2];

  if (NA_checkOneCBuffer("choose<size>bytes", niter,
     buffers[outi], bsizes[outi], <size>))
        return -1;
  else
     output       =  (char *)   buffers[outi];

  if (maxP == 0) 
	  return 0;

  switch(cMode)
    {
    case WRAPPED:
      for(i=0; i<niter; i++)
        {
          maybelong j = selector[i];
	  wrap(j, maxP);
          NA_ACOPY<size>(&population[j][i*<size>], &output[i*<size>]);
        }
      break;
    default:
    case CLIPPED:
      for(i=0; i<niter; i++)
        {
          maybelong j = selector[i];
          if (j < 0)
            j = 0;
          else if (j >= maxP)
            j = maxP-1;
          NA_ACOPY<size>(&population[j][i*<size>], &output[i*<size>]);
        }
      break;
    case RAISE:
      for(i=0; i<niter; i++)
        {
          maybelong j = selector[i];
          if ((j < 0) || (j >= maxP)) {
             PyErr_Format(PyExc_IndexError, "Index out of range");
             return -1;                           
          }
          NA_ACOPY<size>(&population[j][i*<size>], &output[i*<size>]);
        }
      break;
    }
  return 0;
}

SELF_CHECKED_CFUNC_DESCR(choose<size>bytes, CFUNC_UFUNC);
'''

BYTES_TEMPLATE = ( COPY_TEMPLATE +
                   ALIGN_TEMPLATE +
                   BYTESWAP_TEMPLATE +
                   CHOOSE_TEMPLATE )

# ============================================================================
#          IMPORTANT:  no <>-sugared strings below this point

# translate <var> --> %(var)s in templates seen *so far*
template.sugar_dict(globals())  

# ============================================================================
    

bytesconfig = [
    ["1", "Int8"],
    ["2", "Int16"],
    ["4", "Int32"],
    ["8", "Float64"],
    ["16", "Complex64"],  
    ];

class BytesParams:
  def __init__(self, size, type):
    self.size = size
    self.sizename = str(size) + "bytes"
    self.typename = type
    self.swapkind = "NA_SWAP"

NBytesParams = BytesParams("N","AnyType")

class ComplexBytesParams:
  def __init__(self, size, type):
    self.size = size
    self.sizename = type
    self.typename = type
    self.swapkind = "NA_COMPLEX_SWAP"

Complex32BytesCfg = ComplexBytesParams(8, "Complex32")
Complex64BytesCfg = ComplexBytesParams(16, "Complex64")


class BytesCodeGenerator(CodeGenerator):
    def __init__(self, *components):
        CodeGenerator.__init__(self, *components)
        self.module = "_bytes"
        self.qualified_module = "numarray._bytes"

    def gen_body(self):
        for cfg in bytesconfig:
            t = apply(BytesParams, cfg)
            self.codelist.append((self.separator + BYTES_TEMPLATE) %
                                 t.__dict__)
            self.addcfunc("copy"+ t.sizename)
            self.addcfunc("byteswap"+t.sizename)
            self.addcfunc("align"+t.sizename)
            self.addcfunc("choose"+t.sizename)
            
        self.codelist.append((self.separator + CHOOSE_TEMPLATE) %
                             NBytesParams.__dict__)
        self.addcfunc("chooseNbytes")
              
        self.addcfunc("copyNbytes")
        self.addcfunc("putNbytes")
        self.addcfunc("takeNbytes")

        # Hack in the type based (not size based) methods for complex
        self.codelist.append((self.separator + BYTESWAP_TEMPLATE) %
                             Complex32BytesCfg.__dict__)
        self.addcfunc("byteswapComplex32" )
        self.codelist.append((self.separator + BYTESWAP_TEMPLATE) %
                             Complex64BytesCfg.__dict__)
        self.addcfunc("byteswapComplex64" )
        
        self.addmethod("copyToString")
            

generate_bytes_code = BytesCodeGenerator(BYTES_HEADER)

