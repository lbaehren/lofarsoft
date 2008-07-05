#include "Python.h"
#include "pythread.h"

#include "structmember.h"
#include "libnumarray.h"
#include "cfunc.h"

/* MEASURE_TIMING is only used on i386-Linux to profile performance */
#include "tc.h"

char *_ufunc__doc__ =
	"_ufunc is a module which supplies the _ufunc baseclass of Ufunc.  Class\n"
	"_ufunc is a baseclass used to accelerate selected methods of Ufunc by\n"
	"handling simple cases directly in C.  More complex cases are delegated to\n"
	"python coded methods.\n";

/* ====================================================================== */

#define DEFERRED_ADDRESS(ADDR) 0

/* ====================================================================== */

static long buffersize;
static PyObject *pOperatorClass;
static PyObject *pHandleErrorFunc;
static PyObject *p_blockingParametersCache;
static PyObject *p_getBlockingMissFunc;
static PyObject *p_callOverDimensionsFunc;
static PyObject *p_copyFromAndConvertFunc;
static PyObject *p_copyFromAndConvertMissFunc;
static PyObject *p_copyCacheDict;
static PyObject *p_getThreadIdent;
static PyObject *pUnknownOperator;

/* ====================================================================== */

#define _PYTHON_CALLBACKS 0

#if _PYTHON_CALLBACKS
#define ConverterRebuffer(conv,arr,inb)				\
	PyObject_CallMethod(conv, "rebuffer", "(OO)", arr, inb)
#else
#define ConverterRebuffer(conv,arr,inb)				\
	((PyConverterObject *)conv)->rebuffer(conv, arr, inb)
#endif

#define ConverterClean(conv, arr)			\
        ((PyConverterObject *)conv)->clean(conv, arr)

/* ====================================================================== */
 

static PyObject *
_callOverDimensions(PyObject *objects, PyObject *outshape, 
		    int indexlevel, PyObject *blocking, int overlap, 
		    int level);

static PyObject *
_getBlockingParameters(PyObject *oshape, int niter, int overlap);

static PyObject *
_copyFromAndConvert(PyObject *from, PyObject *to);


static int deferred_ufunc_init(void);

static PyObject *
_cached_dispatch1(PyObject *self, PyObject *in1, PyObject *out);

static PyObject *
_cached_dispatch2(PyObject *self, PyObject *in1, PyObject *in2, PyObject *out);

/* ====================================================================== */

static PyObject * 
_normalize_results(int nin,     PyObject **inputs,
		   int nout,    PyObject **outputs,
		   int nresult, PyObject **results,
		   int return_rank1)
{
	PyObject *rval;
	int i, r;
	
	if ((nresult == 0) 
	    || ((nout == 1) && (outputs[0] != Py_None)) 
	    || (nout > 1)) {
		Py_INCREF(Py_None);
		return Py_None;
	}
	
	rval = PyTuple_New(nresult);
	if (!rval) return NULL;

	if (!NA_NumArrayCheck(results[0]))
		return PyErr_Format(
			PyExc_TypeError,
			"_normalize_results: results[0] is not an array.");
	
	if (((PyArrayObject *)*results)->nd == 0) {
		for (i=0; i<nin; i++) {
			PyObject *input = inputs[i];
                        /* At least 1 rank0 -> rank0 result */
			if (NA_NumArrayCheck(input) && ((PyArrayObject *)input)->nd == 0) {  
				for (r=0; r<nresult; r++) {
					PyArrayObject *result = (PyArrayObject *) results[r];
					if (!NA_NumArrayCheck((PyObject *)result)) 
						return PyErr_Format( PyExc_TypeError, "_normalize_results: result is not an array.");
					if (return_rank1) {
						result->nd = result->nstrides = 1;
						result->dimensions[0] = 1;
						result->strides[0] = result->bytestride;
					}
					Py_INCREF(result);
					PyTuple_SET_ITEM(
						rval, r, (PyObject*)result);
				}
				break;
			}
		}
		if (i == nin) {	/* no rank0 -> scalar results */	
			for (r=0; r<nresult; r++) {
				PyObject *new_result = NA_getPythonScalar
					((PyArrayObject *) results[r], 0L);
				if (!new_result) return NULL;
				PyTuple_SET_ITEM(rval, r, new_result);
			}
		}
	} else {
		for(r=0; r<nresult; r++) {
			PyTuple_SET_ITEM(rval, r, results[r]);
			Py_INCREF(results[r]);
		}	    
	}

	/* Convert single result tuples into simply the result */
	if (nresult == 1) {
		PyObject *result = PyTuple_GetItem(rval, 0);
		Py_INCREF(result);
		Py_DECREF(rval);
		return result;
	} else {
		return rval;
	}
}

static int
_tuple_check(PyObject *o, char *name)
{
	if (PyTuple_Check(o)) return 0;

	PyErr_Format(PyExc_TypeError, "%s should be a tuple", name);

	return -1;
}

static PyObject * 
_Py_normalize_results(PyObject *module, PyObject *args)
{
	PyObject *tinputs, *toutputs, *tresults;
	PyObject **outputs;
	int return_rank1 = 0;
	int nin, nout, nresult;

	if (!PyArg_ParseTuple(args, "OOO|i:_normalize_results", 
			      &tinputs, &toutputs, &tresults, &return_rank1))
		return NULL;

	if (_tuple_check(tinputs, "inputs") < 0)
		return NULL;
	if (_tuple_check(tresults, "results") < 0)
		return NULL;

	nin = PyTuple_Size(tinputs);
    
	if (toutputs == Py_None) {
		nout = 0;
		outputs = NULL;
	} else {
		if (_tuple_check(toutputs, "outputs") < 0)
			return NULL;
		nout = PyTuple_Size(toutputs);
		outputs = (PyObject **) &((PyTupleObject *)toutputs)->ob_item; 
	}
	
	nresult = PyTuple_Size(tresults);

	if ((return_rank1 != 0) && (return_rank1 != 1))
		return PyErr_Format(
			PyExc_ValueError, "return_rank1 should be 0 or 1.");

	return _normalize_results(
		nin,     (PyObject **) &((PyTupleObject *)tinputs)->ob_item, 
		nout,    outputs,
		nresult, (PyObject **) &((PyTupleObject *)tresults)->ob_item, 
		return_rank1);
}

/* _firstcol is an array mutator that changes an array into
   a view of its first column.  _firstcol_undo undoes the mutation.
   This approach is an optimization over the Python prototype's clean
   but slow view based approach.  This ugliness avoids 1-2 view method
   callbacks which are fairly expensive.
*/
typedef struct 
{
	int nd;
	int nstrides;
} firstcol_undo;

static firstcol_undo
_firstcol(PyObject *arr)
{
	PyArrayObject *arra = (PyArrayObject *) arr;
	firstcol_undo undo;
	undo.nd = arra->nd;
	undo.nstrides = arra->nstrides;
	if (arra->nd > 0) 
		-- arra->nd;
	if (arra->nstrides > 0)
		-- arra->nstrides;
	NA_updateStatus(arra);
	return undo;
}

static void 
_firstcol_undo(PyObject *arr, firstcol_undo *undo)
{
	PyArrayObject *arra = (PyArrayObject *) arr;
	arra->nd = undo->nd;
	arra->nstrides = undo->nstrides;
	NA_updateStatus(arra);
}

static PyObject *
CheckFPErrors(PyObject *self, PyObject *args) {
	return Py_BuildValue("i", NA_checkFPErrors());
}


static PyObject *
_getNewArray(PyObject *master, PyObject *type)
{
	PyArrayObject  *amaster = (PyArrayObject *) master;
	PyObject  *out;
	int       typenum = NA_typeObjectToTypeNo(type);
	if (typenum < 0) goto _fail;

	if (NA_NumArrayCheckExact(master)) {
		out = (PyObject *) NA_vNewArray( NULL, typenum, 
						 amaster->nd, amaster->dimensions);
	} else {
		out = PyObject_CallMethod(master, "new", "(O)", type);
	}
	return out;
  _fail:
	return NULL;
}

/* ====================================================================== */

typedef enum
{
	DIG_ARRAY,
	DIG_NONE,
	DIG_INT,
	DIG_LONG,
	DIG_FLOAT,
	DIG_COMPLEX,
	DIG_STRING,
	DIG_UNKNOWN
} digest_kind;

typedef union {
	struct 
	{
		unsigned aligned : 1;
		unsigned contig : 1;
		unsigned byteswap : 1;
		unsigned kind : 3;
		unsigned type : 26;
	} f;
	int ival;
} _digestbits;

#if (MAXDIM > 256)
#error "MAXDIM is too large for _ufunc _digestbits"
#endif

static _digestbits
_digest(PyObject *x)
{
	_digestbits bits;
	bits.ival = 0;
	if (NA_NumArrayCheck(x)) {
		PyArrayObject *me = (PyArrayObject *) x;
		bits.f.kind      = DIG_ARRAY;
		bits.f.aligned   = PyArray_ISALIGNED(me);
		bits.f.contig    = PyArray_ISCONTIGUOUS(me);
		bits.f.byteswap  = PyArray_ISBYTESWAPPED(me);
		bits.f.type      = me->descr->type_num;
	} else if (x == Py_None) {
		bits.f.kind      = DIG_NONE;
	} else if (PyFloat_Check(x)) {
		bits.f.kind      = DIG_FLOAT;
	} else if (PyInt_Check(x)) {
		bits.f.kind      = DIG_INT;
	} else if (PyLong_Check(x)) {
		bits.f.kind      = DIG_LONG;
	} else if (PyComplex_Check(x)) {
		bits.f.kind      = DIG_COMPLEX;
	} else if (PyString_Check(x)) {
		bits.f.kind      = DIG_STRING;
	} else {
		bits.f.kind      = DIG_UNKNOWN;
	}
	return bits;
}

static PyObject *
_ldigest(PyObject *x)
{
	return PyInt_FromLong(_digest(x).ival);
}

static PyObject *
_Py_digest(PyObject *module, PyObject *args)
{
	PyObject *x;
	_digestbits bits;
	if (!PyArg_ParseTuple(args, "O:digest", &x))
		return NULL;
	bits = _digest(x);
	if (bits.f.kind == DIG_UNKNOWN) {
		return PyErr_Format(
			PyExc_KeyError, "_digest force cache miss");
	} else {
		return Py_BuildValue("l", _digest(x).ival);
	}
}

/* staticforward PyTypeObject _ufunc_type;   XXX delete me */

static int
_reportErrors(PyObject *ufunc, PyObject *result, PyObject *out)
{
	PyUfuncObject *self = (PyUfuncObject *) ufunc;
	if (deferred_ufunc_init() < 0) 
		return -1;
	if (!result) return -1;
	Py_DECREF(result);  /* Py_None */
	return NA_checkAndReportFPErrors(PyString_AS_STRING(self->oprator));
}

#if defined(UNTHREADED)
static long PyThread_get_thread_ident() { return 0; }
#endif
 
static void 
_cache_insert(_ufunc_cache *cache, PyObject *ctuple,
	      long in1, long in2, long out, 
	      char *cumop, PyObject *type)
{
	_cache_entry *entry = &cache->entry[cache->insert];
	++ cache->insert;
	cache->insert %= ELEM(cache->entry);
	Py_XDECREF(entry->type);
	Py_XDECREF(entry->ctuple);
	entry->in1 = in1;
	entry->in2 = in2;
	entry->out = out;
	entry->thread_id = PyThread_get_thread_ident();
	if (cumop) 
		strcpy(entry->cumop, cumop);
	else
		entry->cumop[0] = 0;
	entry->type = type;
	if (type) Py_INCREF(type);
	entry->ctuple = ctuple;
	Py_INCREF(ctuple);
}

static PyObject *
_Py_cache_insert(PyObject *self, PyObject *args)
{
	PyObject *ctuple, *in1, *in2, *out, *type=NULL;
	char *cumop = NULL;
	if (!PyArg_ParseTuple(
		    args, "OOOO|sO:_cache_insert",
		    &ctuple, &in1, &in2, &out, &cumop, &type))
		return NULL;
	_cache_insert(
		&((PyUfuncObject *)self)->cache, ctuple,
		_digest(in1).ival, _digest(in2).ival, _digest(out).ival,
		cumop, type);
	Py_INCREF(Py_None);
	return Py_None;
}

static void 
_cache_flush(_ufunc_cache *cache)
{
	int i;
	for(i=0; i<ELEM(cache->entry); i++) {
		Py_XDECREF(cache->entry[i].type);
		Py_XDECREF(cache->entry[i].ctuple);
		memset(&cache->entry[i], 0, sizeof(_cache_entry));
	}
}

static PyObject *
_Py_cache_flush(PyObject *self, PyObject *args)
{
	if (!PyArg_ParseTuple(args, ":_cache_flush"))
		return NULL;
	_cache_flush(&((PyUfuncObject *) self)->cache);
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *
_cache_lookup(
	_ufunc_cache *cache, PyObject *in1, PyObject *in2, PyObject *out, 
	char *cumop, PyObject  *type)
{
	long  i;
	long din1      = _digest(in1).ival;
	long din2      = _digest(in2).ival;
	long dout      = _digest(out).ival;
	long thread_id = PyThread_get_thread_ident();

	for(i=0; i<ELEM(cache->entry); i++) {
		_cache_entry *c = &cache->entry[i];
		if ((din1 == c->in1) &&
		    (din2 == c->in2) &&
		    (dout == c->out) &&
		    (thread_id == c->thread_id) &&
		    (!cumop || !strcmp(cumop, c->cumop)) &&
		    (type == c->type))
			return c->ctuple;
	}
	return NULL;
}

static PyObject *
_Py_cache_lookup(PyObject *self, PyObject *args)
{
	PyObject *ctuple, *in1, *in2, *out, *type=NULL;
	char *cumop = NULL;
	if (!PyArg_ParseTuple(
		    args, "OOO|sO:_cache_lookup", 
		    &in1, &in2, &out, &cumop, &type))
		return NULL;
	ctuple = _cache_lookup(
		&((PyUfuncObject *)self)->cache, in1, in2, out, cumop, type);
	if (!ctuple) {
		PyErr_Format(
			PyExc_KeyError, "cache entry not found");
	} else {
		Py_INCREF(ctuple);
	}
	return ctuple;
}

static PyObject *
_cached_dispatch(PyUfuncObject *self, 
		 int n_ins, PyObject *ins[], 
		 int n_outs, PyObject *outs[])
{
	if ((self->n_inputs == 1) && (self->n_outputs==1)) {
		if ((n_ins != 1) || (n_outs != 1))
			return PyErr_Format(
				PyExc_RuntimeError,
				"wrong number of parameters to unary ufunc.");
		return _cached_dispatch1(
			(PyObject *) self, ins[0], outs[0]);
	} else if ((self->n_inputs == 2) && (self->n_outputs==1)) {
		if ((n_ins != 2) || (n_outs != 1))
			return PyErr_Format(
				PyExc_RuntimeError,
				"wrong number of parameters to binary ufunc.");
		return _cached_dispatch2(
			(PyObject*) self, ins[0], ins[1], outs[0]);
	} else
		return PyErr_Format(
			PyExc_RuntimeError, 
			"N-ary ufunc C interface is not implemented yet.");
	  
}

static PyObject *
_ufunc_new(PyTypeObject *type, PyObject  *args, PyObject *kwds)
{
	PyUfuncObject *self;
	PyObject *ufuncs;
	if (deferred_ufunc_init() < 0) return NULL;

	self = (PyUfuncObject *) PyType_GenericNew(type, args, kwds);
	if (!self) return NULL;
	
	if (!PyArg_ParseTuple(args, "OOiiO", 
			      &self->oprator, &ufuncs, 
			      &self->n_inputs, &self->n_outputs,
			      &self->identity))
		return NULL;
	
	memset(&self->cache, 0, sizeof(self->cache));
	Py_INCREF(self->oprator);
	Py_INCREF(self->identity);
	
	self->call = (_ufunc_function) _cached_dispatch;

	return (PyObject *) self;
}

static void
_ufunc_dealloc(PyObject *self)
{
	PyUfuncObject *me = (PyUfuncObject*) self;
	_cache_flush(&me->cache);
	Py_DECREF(me->oprator);
	Py_DECREF(me->identity);
	self->ob_type->tp_free(self);
}

static PyObject *
_ufunc_n_inputs_get(PyUfuncObject *self)
{
	return PyInt_FromLong(self->n_inputs);
}

static PyObject *
_ufunc_n_outputs_get(PyUfuncObject *self)
{
	return PyInt_FromLong(self->n_outputs);
}

static PyObject *
_ufunc_operator_get(PyUfuncObject *self)
{
	Py_INCREF(self->oprator);
	return self->oprator;
}

static PyObject *
_ufunc_identity_get(PyUfuncObject *self)
{
	Py_INCREF(self->identity);
	return self->identity;
}

static PyGetSetDef 
_ufunc_getsets[] = 
{
	{"n_inputs", 
	 (getter)_ufunc_n_inputs_get, 
	 (setter) NULL,
	 "specifies ufunc inputs count)"},
	{"n_outputs", 
	 (getter)_ufunc_n_outputs_get, 
	 (setter) NULL,
	 "specifies ufunc outputs count)"},
	{"_identity", 
	 (getter)_ufunc_identity_get, 
	 (setter) NULL,
	 "specifies ufunc's identity value"},
	{"operator", 
	 (getter)_ufunc_operator_get, 
	 (setter)NULL,
	 "name of the ufunc operator"},
	{0}
};

static PyObject *
local_dict_item(PyObject *dict, char *name)
{
	PyObject *result = PyDict_GetItemString(dict, name);
	if (!result) {
		return PyErr_Format(
			PyExc_RuntimeError, 
			"_ufunc_init: can't find '%s'", name);
	}
	Py_INCREF(result);
	return result;
}

static int 
deferred_ufunc_init(void)
{
	PyObject *dict, *module;
	static int inited = 0;

	if (inited) return 0;

	module = PyImport_ImportModule("numarray.ufunc");
	if (!module) return -1;
	dict = PyModule_GetDict(module);
	if (!(p_copyFromAndConvertFunc = local_dict_item(dict, "_copyFromAndConvert")))
		return -1;
	if (!(p_copyFromAndConvertMissFunc = local_dict_item(dict, "_copyFromAndConvertMiss")))
		return -1;
	if (!(p_copyCacheDict = local_dict_item(dict, "_copyCache")))
		return -1;
	if (!(p_callOverDimensionsFunc = local_dict_item(dict, "_callOverDimensions")))
		return -1;
	if (!(pOperatorClass = local_dict_item(dict, "_Operator")))
		return -1;
	if (!(pHandleErrorFunc = local_dict_item(dict, "handleError")))
		return -1;
	if (!(p_getBlockingMissFunc = local_dict_item(
		      dict, "_getBlockingMiss")))
		return -1;
	if (!(p_blockingParametersCache = local_dict_item(
		      dict, "_blockingParametersCache")))
		return -1;
	if (!(p_getThreadIdent = NA_initModuleGlobal("numarray.safethread",
						     "get_ident")))
		return -1;
	if (!(pUnknownOperator = PyString_FromString("<unknown operator>")))
		return -1;
	inited = 1;
	return 0;
}

static PyObject *
_Py_setBufferSize(PyObject *module, PyObject *args)
{
	if (!PyArg_ParseTuple(args, "l", &buffersize))
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
_Py_getBufferSize(PyObject *module, PyObject *args)
{
	if (!PyArg_ParseTuple(args, ":_getBufferSize"))
		return NULL;
	return PyInt_FromLong(buffersize);
}

static PyObject *
_ufunc_call( PyUfuncObject *self, PyObject *args)
{
	PyObject *in1, *in2 = Py_None, *out = Py_None;
	char spec[200];
	switch(self->n_inputs) 
	{
	case 0: default:
		return PyErr_Format(
			PyExc_RuntimeError, 
			"_ufunc_call: __call__ is not implemented by base UFunc class");
	case 1:
		snprintf(spec, sizeof(spec), "O|O:%s", PyString_AS_STRING(self->oprator));
		if (!PyArg_ParseTuple(args, spec, &in1, &out))
			return NULL;
		return _cached_dispatch1((PyObject *) self, in1, out); 
	case 2:
		snprintf(spec, sizeof(spec), "OO|O:%s", PyString_AS_STRING(self->oprator));
		if (!PyArg_ParseTuple(args, spec, &in1, &in2, &out))
			return NULL;
		return _cached_dispatch2((PyObject *) self, in1, in2, out);
	}
}

static PyObject *
_accumulate_out(PyObject *self, PyObject *inarr, PyObject *outarr, PyObject *otype)
{
	PyObject *result, *toutarr;
	firstcol_undo fc_in, fc_out;

	if (outarr == Py_None) {
		toutarr = _getNewArray(inarr, otype);
		if (!toutarr) return NULL;
	} else {
		if (!NA_ShapeEqual((PyArrayObject *)inarr, 
				   (PyArrayObject *)outarr)) 
		{
			return PyErr_Format( 
				PyExc_ValueError,
				"Supplied output array does not have the appropriate shape");
		}
		toutarr = outarr;
		Py_INCREF(toutarr);
	}
	assert(NA_NumArrayCheck(inarr));
	if (NA_elements((PyArrayObject *)inarr)) {
		fc_in = _firstcol(inarr);
		if (inarr != toutarr) {
			fc_out = _firstcol(toutarr);
			result = PyObject_CallMethod(toutarr, "_copyFrom", "(O)", inarr);
			_firstcol_undo(inarr, &fc_in);
			_firstcol_undo(toutarr, &fc_out);
		} else {
			result = PyObject_CallMethod(toutarr, "_copyFrom", "(O)", inarr);
			_firstcol_undo(inarr, &fc_in);
		}
		if (!result) {
			Py_DECREF(toutarr);
			return NULL;
		} else {
			Py_DECREF(result); /* None */
		}
	} else {
	} /* 0-element case requires nothing */
	return toutarr;
}

static PyObject *
_Py_accumulate_out(PyObject *self, PyObject *args)
{
	PyObject *inarr, *outarr, *otype;
	if (!PyArg_ParseTuple(args, "OOO:_accumulate_out", &inarr, &outarr, &otype))
		return NULL;
	return _accumulate_out(self, inarr, outarr, otype);
}

static PyObject *
_reduce_out(PyObject *self, PyObject *inarr, PyObject *outarr, PyObject *otype)
{
	PyArrayObject *toutarra;
	PyArrayObject *inarra = (PyArrayObject *) inarr;
	PyArrayObject *outarra = (PyArrayObject *) outarr;
	PyObject *result, *toutarr;
	firstcol_undo fc_in;
	int nelements;
	int i, otypenum = NA_typeObjectToTypeNo(otype);
	if (otypenum < 0) return PyErr_Format(
		PyExc_ValueError, "_reduce_out: problem with otype");

	/* because _firstcol might drop zero dimension compute now. */
	assert(NA_NumArrayCheck(inarr));
	nelements = NA_elements((PyArrayObject *)inarr); 

	fc_in = _firstcol(inarr);
	if ((outarr == Py_None) || outarra->descr->type_num != otypenum) {
		toutarr = _getNewArray(inarr, otype);
	} else {
		toutarr = outarr;
		Py_INCREF(outarr);
	}
	if (!toutarr) goto _fail;

	toutarra = (PyArrayObject *) toutarr;
	if ((fc_in.nd == 1) && 
	    (toutarra->nd == 1) && (toutarra->dimensions[0] == 1))
		toutarra->nd = toutarra->nstrides = 0;
	if (nelements) {
		result = PyObject_CallMethod(toutarr, "_copyFrom", "(O)", inarr);
	} else { /* handle the 0-element case */
		PyObject *identity = PyObject_GetAttrString(self, "_identity");
		if (!identity) goto _fail;
		result = PyObject_CallMethod(
			toutarr, "_copyFrom", "(O)", identity);
		Py_DECREF(identity);
	}
	if (!result) {
		Py_DECREF(toutarr);
		goto _fail;
	}
	_firstcol_undo(inarr, &fc_in);
	Py_DECREF(result); /* Py_None */
	if (toutarra->nstrides < MAXDIM-1) {
		toutarra->strides[ toutarra->nstrides ] = 0;
		++ toutarra->nstrides;
		toutarra->nd = inarra->nd;
		for(i=0; i<inarra->nd; i++)
			toutarra->dimensions[i] = inarra->dimensions[i];
/*		if (toutarr->nd < toutarr->nstrides) {
  diff = toutarr->nstrides - toutarr->nd;
  for(i=0; i<toutarr->nd; i++)
  toutarr->strides[i] = toutarr->strides[i+diff];
  toutarr->nstrides = toutarr->nd;
  }
*/
		NA_updateStatus(toutarra);
	}
	return toutarr;

  _fail:
	_firstcol_undo(inarr, &fc_in);
	return NULL;
	
}

static PyObject *
_Py_reduce_out(PyObject *self, PyObject *args)
{
	PyObject *inarr, *outarr, *otype;
	if (!PyArg_ParseTuple(args, "OOO:_reduce_out", &inarr, &outarr, &otype))
		return NULL;
	return _reduce_out(self, inarr, outarr, otype);
}

static PyObject *
_cum_lookup(PyObject *self, char *cumop, PyObject *in1, PyObject *out,
	    PyObject *type)
{
	PyObject *cached;

	cached = _cache_lookup(	&((PyUfuncObject *)self)->cache,
				in1, Py_None, out, cumop, type);
	if (!cached) {
		return PyObject_CallMethod(
			self, "_cum_cache_miss", "(sOOO)", 
			cumop, in1, out, type);
	} else {
		PyObject *wout;
		PyObject *otype = PyTuple_GET_ITEM(cached, 1);
		if (!strcmp(cumop, "R")) {
#if _PYTHON_CALLBACKS
			wout = PyObject_CallMethod(
				self, "_reduce_out", "(OOO)", in1, out, otype);
#else
			wout = _reduce_out(self, in1, out, otype);
#endif
		} else {
#if _PYTHON_CALLBACKS
			wout = PyObject_CallMethod(
				self, "_accumulate_out", "(OOO)", in1, out, otype);
#else
			wout = _accumulate_out(self, in1, out, otype);
#endif
		}
		if (!wout) return NULL;
		return Py_BuildValue("(ONO)", in1, wout, cached);
	}
}

static PyObject *
_Py_cum_lookup(PyObject *self, PyObject *args)
{
	PyObject *in1, *out, *type;
	char *cumop;

	if (!PyArg_ParseTuple(args, "sOOO:_cum_lookup", 
			      &cumop, &in1, &out, &type))
		return NULL;

	if (!NA_NumArrayCheck(out) && (out != Py_None))
		return PyErr_Format(
			PyExc_TypeError, "_cum_lookup: out must be a NumArray");

	if (((PyUfuncObject*)self)->n_inputs != 2) {
		return PyErr_Format(
			PyExc_TypeError, "_cum_lookup only works on BinaryUFuncs.");
	}
	
	return _cum_lookup(self, cumop, in1, out, type);	    
}

static PyObject *
_cum_fast_exec(PyObject *self, PyObject *in1, PyObject *out, PyObject *cached)
{
	PyArrayObject *in1a = (PyArrayObject *) in1;
	PyArrayObject *outa = (PyArrayObject *) out;
	PyObject *result, *cfunc;
	cfunc = PyTuple_GET_ITEM(cached, 2);
	result = NA_callStrideConvCFuncCore( 
		cfunc, in1a->nd, in1a->dimensions,
		in1a->_data, in1a->byteoffset, in1a->nstrides, in1a->strides,
		outa->_data, outa->byteoffset, outa->nstrides, outa->strides,
		0 );
	return result;
}

static PyObject *
_Py_cum_fast_exec(PyObject *self, PyObject *args)
{
	PyObject *in1, *out, *cached;
	if (!PyArg_ParseTuple(args, "OOO:_cum_fast_exec", &in1, &out, &cached))
		return NULL;

	if (((PyUfuncObject*)self)->n_inputs != 2) {
		return PyErr_Format(
			PyExc_TypeError, 
			"_cum_fast_exec only works on BinaryUFuncs.");
	}
       	return _cum_fast_exec(self, in1, out, cached);
}

static PyObject *
_cum_slow_exec(PyObject *self, PyObject *in1, PyObject *out, PyObject *cached)
{
	PyArrayObject *in1a = (PyArrayObject *) in1;
	PyArrayObject *outa = (PyArrayObject *) out;
	PyObject *ufargs, *in1_shape, *blocking;
	PyObject *input, *output, *operator, *objects, *oshape, *result;
	PyObject *cfunc, *otype, *blockingparameters, *inbuff, *outbuff;
	long niter;
	int overlap, maxitemsize, indexlevel, typenum;
	PyArray_Descr *otypedescr;
	
	ufargs = PyTuple_GET_ITEM(cached, 3);
	if (!PyArg_ParseTuple(ufargs, "OOi:_cum_slow_exec ufargs", 
			      &input, &output, &maxitemsize))
		return NULL;

	niter = buffersize / maxitemsize;

	if (in1a->dimensions[ in1a->nd-1] > niter)
		overlap = 1;
	else
		overlap = 0;

	in1_shape = NA_intTupleFromMaybeLongs( in1a->nd, in1a->dimensions);
	if (!in1_shape) return NULL;

	blocking = _getBlockingParameters(in1_shape, niter, overlap);
	Py_DECREF(in1_shape);
	
	if (!PyArg_ParseTuple(blocking, "iO:_cum_slow_exec blocking", 
			      &indexlevel, &blockingparameters)) {
		Py_DECREF(blocking);
		return NULL;
	}
	otype = PyTuple_GET_ITEM(cached, 1);
	cfunc = PyTuple_GET_ITEM(cached, 2);

	inbuff = ConverterRebuffer(input, in1, Py_None);
  	if (!inbuff) return NULL;
	outbuff = ConverterRebuffer(output, out, Py_None);
  	if (!outbuff) return NULL;

	typenum = NA_typeObjectToTypeNo(otype);
	if (typenum < 0) return NULL;
	otypedescr = NA_DescrFromType(typenum);
	if (!otypedescr) return PyErr_Format(
		PyExc_RuntimeError, "_cum_slow_exec: problem with otype");

	operator = PyObject_CallFunction(
		pOperatorClass, "(O[O][O]i)", cfunc, inbuff, outbuff, 
		otypedescr->elsize);
	if (!operator) return NULL;
	Py_DECREF(inbuff);
	Py_DECREF(outbuff);

	objects = Py_BuildValue("(ONO)", input, operator, output);
	if (!objects) return NULL;

	oshape = NA_intTupleFromMaybeLongs(outa->nd, outa->dimensions);
	if (!oshape) return NULL;
#if _PYTHON_CALLBACKS
	result = PyObject_CallFunction(
		p_callOverDimensionsFunc, "(OOiOii)", 
		objects, oshape, indexlevel, blockingparameters, overlap, 0);
#else
	result = _callOverDimensions(objects, oshape, indexlevel, blockingparameters, overlap, 0);
#endif
	Py_DECREF(oshape);
	if (!result) return NULL;

 	inbuff = ConverterRebuffer(input, Py_None, Py_None);
  	if (!inbuff) return NULL;
  	Py_DECREF(inbuff);
 	outbuff = ConverterRebuffer(output, Py_None, Py_None);
  	if (!outbuff) return NULL;
	Py_DECREF(outbuff);
	Py_DECREF(objects);
	Py_DECREF(blocking);
	return result;
}

static PyObject *
_Py_cum_slow_exec(PyObject *self, PyObject *args)
{
	PyObject *in1, *out, *cached;
	if (!PyArg_ParseTuple(args, "OOO:_cum_slow_exec", &in1, &out, &cached))
		return NULL;
	if (((PyUfuncObject*)self)->n_inputs != 2) {
		return PyErr_Format(
			PyExc_TypeError, 
			"_cum_slow_exec only works on BinaryUFuncs.");
	}	
	return _cum_slow_exec(self, in1, out, cached);
}

static PyObject *
_cum_exec(PyObject *self, PyObject *in1, PyObject *out, PyObject *cached)
{
	PyObject *modeObj, *result;
	PyArrayObject *in1a = (PyArrayObject *) in1;
	char *mode;
	PyObject *otypeObj;
	NumarrayType otype;
	modeObj = PyTuple_GET_ITEM(cached, 0);
	otypeObj = PyTuple_GET_ITEM(cached, 1);
	otype = NA_typeObjectToTypeNo(otypeObj);
	if (otype < 0) return NULL;
	if ((otype == tBool) && (in1a->descr->type_num != tBool)) {
		in1 = PyObject_CallMethod(in1, "astype", "(s)", "Bool");
		if (!in1) return NULL;
	} else {
		Py_INCREF(in1);
	}
	assert(NA_NumArrayCheck(in1));
	if (NA_elements((PyArrayObject *)in1)) {
		NA_clearFPErrors();
		mode = PyString_AsString(modeObj);
		if (!strcmp(mode, "fast")) {
#if _PYTHON_CALLBACKS
			result = PyObject_CallMethod(
				self, "_cum_fast_exec", "(OOO)", 
				in1, out, cached);
#else
			result = _cum_fast_exec(self, in1, out, cached);
#endif
		} else {
#if _PYTHON_CALLBACKS
			result = PyObject_CallMethod(
				self, "_cum_slow_exec", "(OOO)", 
				in1, out, cached);
#else
			result = _cum_slow_exec(self, in1, out, cached);
#endif
		}
		if (_reportErrors(self, result, out) < 0) {
			Py_DECREF(in1);
			return NULL;
		}
	} else {
	} /* 0-element case doesn't require ufunc */
	Py_INCREF(out);
	Py_DECREF(in1);
	return out;
}

static PyObject *
_Py_cum_exec(PyObject *self, PyObject *args)
{
	PyObject *in1, *out, *cached;
	if (!PyArg_ParseTuple(args, "OOO:_cum_exec", 
			      &in1, &out, &cached))
		return NULL;

	if (!NA_NumArrayCheck(in1))
		return PyErr_Format(
			PyExc_TypeError, "_cum_exec: in1 must be a NumArray");

	if (!NA_NumArrayCheck(out))
		return PyErr_Format(
			PyExc_TypeError, "_cum_exec: out must be a NumArray");

	if (((PyUfuncObject*)self)->n_inputs != 2) {
		return PyErr_Format(
			PyExc_TypeError, "_cum_exec only works on BinaryUFuncs.");
	}
	
	return _cum_exec(self, in1, out, cached);	    
}

static PyObject *
_cum_cached(PyObject *self, char *cumop, PyObject *in1, PyObject *out,
	    PyObject *type)
{
	int i, otype;
	PyObject *params, *cached, *otypeObj;
	PyArrayObject *wout;
	if (deferred_ufunc_init() < 0) return NULL;
	if (out != Py_None) {
		if (!NA_NumArrayCheck(out)) {
			return PyErr_Format(PyExc_TypeError,
					    "output array must be a NumArray");
		}
		if (!PyArray_ISALIGNED(out) || PyArray_ISBYTESWAPPED(out)) {
			return PyErr_Format(
				PyExc_ValueError, 
				"misaligned or byteswapped output numarray not supported by reduce/accumulate");
		}
	}
	if (strcmp(cumop, "R") && strcmp(cumop, "A"))
		return PyErr_Format(PyExc_ValueError, 
				    "Unknown cumulative operation");

#if _PYTHON_CALLBACKS
	params = PyObject_CallMethod(
		self, "_cum_lookup", "(sOOO)", cumop, in1, out, type);
#else
	params = _cum_lookup(self, cumop, in1, out, type);
#endif
	if (!params) return NULL;

#if _PYTHON_CALLBACKS
	wout = (PyArrayObject *) PyObject_CallMethod(
		self, "_cum_exec", "O", params);
#else
	wout = (PyArrayObject *) _Py_cum_exec(self, params);
#endif
	if (!wout) {
		Py_DECREF(params);
		return NULL;
	}

	if (!strcmp(cumop, "R")) {
		PyArrayObject *in1a = (PyArrayObject *) in1;
		if (in1a->nd) {
			wout->nd = in1a->nd - 1;
			for(i=0; i<wout->nd; i++)
				wout->dimensions[i] = in1a->dimensions[i];
		} else 
			wout->nd = 0;
		if ((wout->nstrides = wout->nd)) {
			NA_stridesFromShape(
				wout->nd, wout->dimensions, wout->bytestride, 
				wout->strides);
		}
		if (!wout->nd) {
			wout->nstrides = wout->nd = 1;
			wout->dimensions[0] = 1;
			wout->strides[0] = wout->itemsize;
		}
		NA_updateStatus(wout);
	}
	if (out == Py_None) {
		Py_DECREF(params);
		return (PyObject *) wout;
	}
	cached = PyTuple_GET_ITEM(params, 2);
	otypeObj = PyTuple_GET_ITEM(cached, 1);
	otype = NA_typeObjectToTypeNo(otypeObj);
	Py_DECREF(params);
	if (PyArray(out)->descr->type_num != otype) {
		PyObject *result;
#if _PYTHON_CALLBACKS
		result = PyObject_CallFunction(
			p_copyFromAndConvertFunc, "(OO)", wout, out);
#else
		result = _copyFromAndConvert((PyObject *) wout, out);
#endif
		Py_DECREF(wout);
		if (!result) return NULL;
		Py_DECREF(result); 
	} else 
		Py_DECREF(wout);
	return (PyObject *) out;
}

static PyObject *
_Py_cum_cached(PyObject *self, PyObject *args)
{
	PyObject *in1, *out, *type;
	char *cumop;

	if (!PyArg_ParseTuple(args, "sOOO:_cum_cached", 
			      &cumop, &in1, &out, &type))
		return NULL;

	if (!NA_NumArrayCheck(out) && (out != Py_None))
		return PyErr_Format(
			PyExc_TypeError, "_cum_cached: out must be a NumArray");

	if (((PyUfuncObject*)self)->n_inputs != 2) {
		return PyErr_Format(
			PyExc_TypeError, "_cum_cached only works on BinaryUFuncs.");
	}
	
	return _cum_cached(self, cumop, in1, out, type);	    
}

static void
_moveToLast(int dim, int nshape, maybelong *shape)
{
	int i, j;
	maybelong temp[MAXDIM];
	for(i=j=0; i<nshape; i++)
		if  (i != dim)
			temp[j++] = shape[i];
		else
			temp[nshape-1] = shape[i];
	for(i=0; i<nshape; i++)
		shape[i] = temp[i];
}

static PyObject *
_cum_swapped(PyObject *self, PyObject *in1, int dim, 
	     PyObject *out, char *cumop, PyObject *type)
{
	PyObject *_out=NULL;
	PyArrayObject *in1a = (PyArrayObject  *) in1;
	
	if (deferred_ufunc_init() < 0) return NULL;
	
	if (in1a->nd == 0) /* rank-0 null reduction/accumulation */
		return PyObject_CallMethod(in1, "copy", NULL);
	
	if (dim == PyArray(in1)->nd-1) dim = -1;
	
	if (dim != -1) {
		if (NA_swapAxes(in1a, -1, dim) < 0)
			goto fail;
		if (NA_swapAxes((PyArrayObject *) out, -1, dim) < 0)
			goto fail;
	} 
	
#if _PYTHON_CALLBACKS
	_out = PyObject_CallMethod( (PyObject *) self, "_cum_cached", "sOOO", 
				    cumop, in1, out, type);
#else
	_out = _cum_cached(self, cumop, in1, out, type);
#endif
	if (!_out) goto fail;

	if (!strcmp(cumop, "A")) {
		if (dim != -1) {
			/* either _out == out, or out == None */
			if (NA_swapAxes((PyArrayObject *)_out, -1, dim) < 0) 
				goto fail;
		}
	} else { /* "reduce" */
		PyArrayObject *_outa = (PyArrayObject *) _out;
		if (dim != -1) {
			_moveToLast(dim, _outa->nd, _outa->dimensions);
			_moveToLast(dim, _outa->nstrides, _outa->strides);
			NA_updateStatus(_outa);
		}
	}

	if (dim != -1) {
		if (NA_swapAxes(in1a, -1, dim) < 0)
			goto fail;
	} 
	
	if (out == Py_None) {
		return (PyObject *) _out;
	} else {
		Py_INCREF(Py_None);
		return Py_None;
	}

  fail:
	if (out == Py_None) {
		Py_XDECREF(_out);
	}
	return NULL;
}

static PyObject *
_Py_cum_swapped(PyObject *self, PyObject *args)
{
	PyObject *in1, *out, *type=Py_None;
	int dim;
	char *cumop;

	if (!PyArg_ParseTuple(args, "OiOs|O:cumulative_swapped", 
			      &in1, &dim, &out, &cumop, &type))
		return NULL;

	if (!NA_NumArrayCheck(out) && (out != Py_None))
		return PyErr_Format(
			PyExc_TypeError, "_cum_swapped: out must be a NumArray");

	if (((PyUfuncObject*)self)->n_inputs != 2) {
		return PyErr_Format(
			PyExc_TypeError, "_cum_swapped only works on BinaryUFuncs.");
	}
	
	return _cum_swapped(self, in1, dim, out, cumop, type);	    
}

static int
_fixdim(int *axis, int *dim)
{
	if (*axis == 0 && *dim == 0) return 0;
	if (*axis && !*dim) return 0;
	if (*dim) {
		if (*axis) {
			PyErr_Format(PyExc_RuntimeError, 
				     "Specify 'axis' or 'dim', "
				     "but not both.  'dim' is deprecated.");
			return -1;
		} else {
			if (PyErr_Warn(PyExc_DeprecationWarning,
				       "The 'dim' keyword is deprecated.  "
				       "Specify 'axis' instead.") < 0) {
				return -1;
			}				
			*axis = *dim;
			return 0;
		}
	}
	return 0;
}

static PyObject *
_Py_accumulate(PyObject *self, PyObject *args, PyObject *keywds)
{
	PyObject *in1, *_in1, *r, *out=Py_None, *type=Py_None;
	int axis=0, dim=0;
	static char *kwlist[] = {"array","axis","out","type","dim",NULL};

	if (!PyArg_ParseTupleAndKeywords(args, keywds, 
					 "O|iOOi:reduce", kwlist,
					 &in1, &axis, &out, &type, &dim))
		return NULL;

	if (!NA_NumArrayCheck(out) && (out != Py_None))
		return PyErr_Format(
			PyExc_TypeError, "accumulate: out must be a NumArray");

	if (((PyUfuncObject*)self)->n_inputs != 2) {
		return PyErr_Format(
			PyExc_TypeError, "_cum_swapped only works on BinaryUFuncs.");
	}
	
	_in1 = (PyObject *) NA_InputArray(in1, tAny, 0);
	if  (!_in1) return NULL;

	if (_fixdim(&axis, &dim) < 0) return NULL;

	r = _cum_swapped(self, _in1, axis, out, "A", type);

	Py_DECREF(_in1);
	return r;
}

static PyObject *
_Py_areduce(PyObject *self, PyObject *args, PyObject *keywds)
{
	PyObject *in1, *_in1, *r, *out=Py_None, *type=Py_None;
	int axis=0, dim=0;
	static char *kwlist[] = {"array","axis","out","type","dim",NULL};

	if (!PyArg_ParseTupleAndKeywords(args, keywds, 
					 "O|iOOi:areduce", kwlist,
					 &in1, &axis, &out, &type, &dim))
		return NULL;

	if (!NA_NumArrayCheck(out) && (out != Py_None))
		return PyErr_Format(
			PyExc_TypeError, "reduce: out must be a NumArray");

	if (((PyUfuncObject*)self)->n_inputs != 2) {
		return PyErr_Format(
			PyExc_TypeError, "areduce: only works on BinaryUFuncs.");
	}

	_in1 = (PyObject *) NA_InputArray(in1, tAny, 0);
	if  (!_in1) return NULL;

	if (_fixdim(&axis, &dim) < 0) return NULL;

	r = _cum_swapped(self, _in1, axis,  out, "R", type);

	Py_DECREF(_in1);
	return r;
}

static PyObject *
_Py_reduce(PyObject *self, PyObject *args, PyObject *keywds)
{
	PyObject *in1, *_in1, *r, *out=Py_None, *type=Py_None;
	int axis=0, dim=0;
	static char *kwlist[] = {"array","axis","out","type","dim",NULL};

	if (!PyArg_ParseTupleAndKeywords(args, keywds, 
					 "O|iOOi:reduce", kwlist,
					 &in1, &axis, &out, &type, &dim))
		return NULL;

	if ((out != Py_None) && !NA_NumArrayCheck(out))
		return PyErr_Format(
			PyExc_TypeError, "reduce: out must be a NumArray");

	if (((PyUfuncObject*)self)->n_inputs != 2) {
		return PyErr_Format(
			PyExc_TypeError, "reduce only works on BinaryUFuncs.");
	}
	
	_in1 = (PyObject *) NA_InputArray(in1, tAny, 0);
	if (!_in1) return NULL;

	if (_fixdim(&axis, &dim) < 0) return NULL;

	r = _cum_swapped(self, _in1, axis, out, "R", type);
	if (!r) return NULL;
	
	/* Convert rank-1-len-1 reduction result of rank-1 to scalar */
	if (r != Py_None) {
		PyArrayObject *ra = (PyArrayObject *) r;
		PyArrayObject *_in1a = (PyArrayObject *) _in1;
		if ((((ra->nd == 1) && (ra->dimensions[0] ==1)) ||
		     (ra->nd == 0)) && 
		    ((_in1a->nd == 1) || (_in1a->nd == 0))) {
			r = NA_getPythonScalar(ra, 0);
			Py_DECREF(ra);
		}
	}

	Py_DECREF(_in1);
	return r;
}

/* ====================================================================== */

static PyMethodDef _ufunc_methods[] = {
	
	{"accumulate", (PyCFunction) _Py_accumulate, 
	 METH_VARARGS|METH_KEYWORDS,
	 "accumulate   performs the operation along the dimension, accumulating subtotals"},
	{"areduce", (PyCFunction) _Py_areduce, 
	 METH_VARARGS|METH_KEYWORDS,
	 "areduce  performs the operation along the specified dimension, eliminating it.  The result of areducing a 1D array is a 1 element 1D array."},
	{"reduce", (PyCFunction) _Py_reduce, 
	 METH_VARARGS|METH_KEYWORDS,
	 "reduce  performs the operation along the specified dimension, eliminating it.  The result of reducing a 1D array is a Python scalar."},
	{"_accumulate_out", _Py_accumulate_out, METH_VARARGS,
	 "_accumulate_out   creates the output array for an accumulation."},
	{"_reduce_out", _Py_reduce_out, METH_VARARGS,
	 "_reduce_out   creates the output array for a reduction."},
	{"_cum_lookup", _Py_cum_lookup, METH_VARARGS,
	 "_cum_lookup   handles cache entry retrieval/setup."},
	{"_cum_exec", _Py_cum_exec, METH_VARARGS,
	 "_cum_exec   handles cache entry execution."},
	{"_cum_fast_exec", _Py_cum_fast_exec, METH_VARARGS,
	 "_cum_fast_exec   handles cache entry execution without converters."},
	{"_cum_slow_exec", _Py_cum_slow_exec, METH_VARARGS,
	 "_cum_slow_exec   handles cache entry execution with converters."},
	{"_cum_cached", _Py_cum_cached, METH_VARARGS,
	 "_cum_cached   handles cumulative operation caching and execution."},
	{"_cum_swapped", _Py_cum_swapped, METH_VARARGS,
	 "_cum_swapped  handles cumulative operation axis swapping."},
	{"_cache_insert", (PyCFunction) _Py_cache_insert, METH_VARARGS,
	 "_cache_insert   adds a cache tuple to the ufunc setup cache."},
	{"_cache_lookup", (PyCFunction) _Py_cache_lookup, METH_VARARGS,
	 "_cache_lookup  finds an entry in the cache or raises KeyError."},
	{"_cache_flush", (PyCFunction) _Py_cache_flush, METH_VARARGS,
	 "_cache_flush   empties the setup cache."},
	{NULL,	NULL},
};

static PyTypeObject _ufunc_type = {
	PyObject_HEAD_INIT(DEFERRED_ADDRESS(&PyType_Type))
	0,
	"numarray._ufunc._ufunc",
	sizeof(PyUfuncObject),
	0,
	_ufunc_dealloc,	                        /* tp_dealloc */
	0,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	0,					/* tp_repr */
	0,					/* tp_as_number */
	0,                                      /* tp_as_sequence */
	0,			                /* tp_as_mapping */
	0,					/* tp_hash */
	(ternaryfunc) _ufunc_call,		/* tp_call */
	0,					/* tp_str */
	0,					/* tp_getattro */
	0,					/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | 
        Py_TPFLAGS_BASETYPE,                    /* tp_flags */
	0,					/* tp_doc */
	0,					/* tp_traverse */
	0,					/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	_ufunc_methods,			        /* tp_methods */
	0,					/* tp_members */
	_ufunc_getsets,			        /* tp_getset */
	0,                                      /* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	0,		                        /* tp_init */
	0, 		                        /* tp_alloc */
	_ufunc_new,				/* tp_new */
};

static PyObject *
_restuff_pseudo(PyObject *pseudo, PyObject *x)
{
	if (NA_isPythonScalar(x)) {
		if (pseudo == Py_None) {
			return PyErr_Format( 
				PyExc_RuntimeError, 
				"scalar input with no pseudo array");
		} else {
			if (NA_setFromPythonScalar(
				    (PyArrayObject *) pseudo, 0, x) < 0) {
				return NULL;
			}
			Py_INCREF(pseudo);
			return pseudo;
		}
	} else {
		if (pseudo != Py_None) {
			Py_INCREF(pseudo);
			return pseudo;
		} else {
			Py_INCREF(x);
			return x;
		}
	}
}

static PyObject *
_Py_restuff_pseudo(PyObject *module, PyObject *args)
{
	PyObject *x, *y;

	if (!PyArg_ParseTuple(args, "OO:restuff_pseudo", &x, &y))
		return NULL;
	return _restuff_pseudo(x, y);
}

static int
_callFs(PyObject *objects, int ndims, maybelong *dims, int newdim, 
	PyObject *shape)
{
	int j;
	PyObject *dimstuple, *result;
	dims[ ndims ] = newdim;
	dimstuple = NA_intTupleFromMaybeLongs(ndims+1, dims);
	if (!dimstuple) return -1;
	for(j=0; j<PyTuple_GET_SIZE(objects); j++) {
		PyConverterObject *f = (PyConverterObject *) PyTuple_GET_ITEM(objects, j);
		result = f->compute((PyObject *) f, dimstuple, shape);

		if (!result) return -1; else Py_DECREF(result);
	}
	Py_DECREF(dimstuple);
	return 0;
}

static PyObject *
_doOverDimensions(PyObject *objects, PyObject *outshape, 
		  int ndims, maybelong *dims,
		  int indexlevel, PyObject *blockingparams, 
		  int overlap, int level)
{
	int i;
	PyObject *result;
	if (level == indexlevel) {
		PyObject *nregShapeIters, *shape, *leftover, *leftovershape;
		shape = PyTuple_GET_ITEM(blockingparams, 1);
		if (PyTuple_GET_SIZE(shape)>0) {
			PyObject *shape0;
			int dimval;
			int nregShapeItersInt;

			shape0 = PyTuple_GET_ITEM(shape, 0) ;
			dimval = PyInt_AsLong(shape0) - overlap;
			nregShapeIters = PyTuple_GET_ITEM(blockingparams, 0);

			nregShapeItersInt = PyInt_AsLong(nregShapeIters);

			for(i=0; i<nregShapeItersInt; i++) {
				if (_callFs(objects, ndims, dims, i*dimval, shape)<0)
					return NULL;
			}
			
			leftover = PyTuple_GET_ITEM(blockingparams, 2);
			if (PyInt_AsLong(leftover)) {
				leftovershape = PyTuple_GET_ITEM(blockingparams, 3);
				if (_callFs(objects, ndims, dims, i*dimval, leftovershape) < 0)
					return NULL;
			}
		} else {
			maybelong mydims[MAXDIM];
			if (_callFs(objects, 0, mydims, 0, shape) < 0)
				return NULL;
		}
	} else {
		PyObject *outshapeLevel;
		int outshapeLevelInt;
		outshapeLevel = PyTuple_GET_ITEM(outshape, level);
		if (!PyInt_Check(outshapeLevel))
			return PyErr_Format(PyExc_TypeError,
					    "_doOverDimensions: outshape[level] is not an int.");
		outshapeLevelInt = PyInt_AsLong(outshapeLevel); 
		for(i=0; i<outshapeLevelInt; i++) {
			dims[ndims] = i;
			result = _doOverDimensions(objects, outshape, ndims+1, dims, indexlevel,
						   blockingparams, overlap, level+1);
			if (!result) return NULL; else Py_DECREF(result);				
		}
	}
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
_callOverDimensions(PyObject *objects, PyObject *outshape, 
		    int indexlevel, PyObject *blocking, int overlap, int level)
{
	maybelong idims[MAXDIM];
	PyObject *shape, *shape0, *nregShapeIters, *leftover, *result;

	shape = PyTuple_GET_ITEM(blocking, 1);
	if (!PyTuple_Check(shape))
		return PyErr_Format(PyExc_TypeError,
				    "_callOverDimensions: shape is not a tuple.");
	if (PyTuple_GET_SIZE(shape) > 0) {
		shape0 = PyTuple_GET_ITEM(shape, 0) ;
		if (!PyInt_Check(shape0) && !PyLong_Check(shape0))
			return PyErr_Format(PyExc_TypeError,
					    "_callOverDimensions: shape0 is not an int.");
	}
	nregShapeIters = PyTuple_GET_ITEM(blocking, 0);
	if (!PyInt_Check(nregShapeIters))
		return PyErr_Format(PyExc_TypeError,
				    "_callOverDimensions: nregShapeIters is not an int.");
	leftover = PyTuple_GET_ITEM(blocking, 2);
	if (!PyInt_Check(leftover))
		return PyErr_Format(PyExc_TypeError,
				    "_doOverDimensions: leftover is not an int.");
	result = _doOverDimensions(objects, outshape, 0, idims, indexlevel, blocking, overlap, level);
	return result;
}

static PyObject *
_Py_callOverDimensions(PyObject *m, PyObject *args)
{
	PyObject *objects, *outshape, *blocking;
	int i, indexlevel, overlap=0, level=0;

	if (!PyArg_ParseTuple(args, "OOiO|ii:_callOverDimensions", &objects, &outshape,  
			      &indexlevel, &blocking, &overlap, &level))
		return NULL;
	if (!PyTuple_Check(objects))
		return PyErr_Format(
			PyExc_TypeError,
			"_callOverDimensions: objects is not a tuple.");
	for(i=0; i<PyTuple_GET_SIZE(objects); i++) 
	{
		PyObject *obj = PyTuple_GET_ITEM(objects, i);
		if (!obj && !NA_ConverterCheck(obj) && !NA_OperatorCheck(obj))
			return PyErr_Format(PyExc_TypeError, 
					    "_callOverDimensions: bad converter or operator");
	}
	if (!PyTuple_Check(outshape) || PyTuple_GET_SIZE(outshape)<level)
		return PyErr_Format(PyExc_ValueError, 
				    "_callOverDimensions: problem with outshape.");
	if (!PyTuple_Check(blocking) || PyTuple_GET_SIZE(blocking)!=4)
		return PyErr_Format(PyExc_TypeError, 
				    "_callOverDimensions: problem with blockingparams tuple.");
	return _callOverDimensions(objects, outshape, indexlevel, blocking, overlap, level);
}

static PyObject *
_fast_exec1(PyObject *ufunc, PyObject *in1, PyObject *out, PyObject *cached)
{
	/* PyObject *otype = PyTuple_GET_ITEM(cached, 1); */
	PyObject *cfunc = PyTuple_GET_ITEM(cached, 2);
	PyObject *result;
	{
		PyArrayObject *in1a = (PyArrayObject*) in1;
		PyArrayObject *outa =  (PyArrayObject *) out;
		PyObject *buffers[2];
		long      offsets[2];
		buffers[0] = in1a->_data;
		buffers[1] = outa->_data;
		offsets[0] = in1a->byteoffset;
		offsets[1] = outa->byteoffset;
		result = NA_callCUFuncCore( cfunc, NA_elements(outa), 1, 1, 
					    buffers, offsets);
	} 
	return result;
}

static PyObject*
_Py_fast_exec1(PyObject *module, PyObject *args)
{
	PyObject *ufunc, *in1, *out, *cached;

	if (!PyArg_ParseTuple(args, "OOOO:_Py_fast_exec1", &ufunc, &in1, &out, 
			      &cached))
		return NULL;

	return _fast_exec1(ufunc, in1, out, cached);
}

static PyObject *
_getBlockingParameters(PyObject *oshape, int niter, int overlap)
{
	PyObject *bpKey, *cacheValue;
	if (deferred_ufunc_init() < 0) return NULL;
	bpKey = Py_BuildValue("Oii", oshape, niter, overlap);
	if (!bpKey)
		return PyErr_Format( 
			PyExc_RuntimeError, 
			"_getBlockingParameters: cache key creation failed.");
	cacheValue = PyDict_GetItem(p_blockingParametersCache, bpKey);
	Py_DECREF(bpKey);
	if (cacheValue) {
		Py_INCREF(cacheValue);
		return cacheValue;
	} else {
		return PyObject_CallFunction(
			p_getBlockingMissFunc, "(Oii)", oshape, niter, 
			overlap);
	}
}

static PyObject *
_Py_getBlockingParameters(PyObject *module, PyObject *args)
{
	PyObject *oshape;
	int      niter, overlap=0;
	if (!PyArg_ParseTuple(args, "Oi|i:_getBlockingParameters", 
			      &oshape, &niter, &overlap))
		return NULL;
	return _getBlockingParameters(oshape, niter, overlap);	    
}

static PyObject *
_slow_exec1(PyObject *ufunc, PyObject *in1, PyObject *out, PyObject *cached)
{
	PyObject *cfunc = PyTuple_GET_ITEM(cached, 2);
	PyObject *ufargs = PyTuple_GET_ITEM(cached, 3);
	PyObject *blocking;
	PyObject *inputs, *outputs, *result, *operator, *objects, *oshape;
	int maxitemsize, niter, indexlevel;
	PyArrayObject *outarr = (PyArrayObject *) out;

	if (deferred_ufunc_init() < 0)
		return NULL;
	if (!PyTuple_Check(ufargs) || PyTuple_GET_SIZE(ufargs) != 3)
		return PyErr_Format(PyExc_ValueError,
				    "_slow_exec1: problem with ufargs tuple.");
	if (!PyArg_ParseTuple(ufargs, "OOi:_slow_exec1 ufargs", 
			      &inputs, &outputs, &maxitemsize))
		return NULL;
	if (!PyTuple_Check(inputs) || PyTuple_GET_SIZE(inputs) != 1)
		return PyErr_Format(PyExc_ValueError, 
				    "_slow_exec1: problem with inputs tuple.");
	if (!PyTuple_Check(outputs) || PyTuple_GET_SIZE(outputs) != 1)
		return PyErr_Format(PyExc_ValueError, 
				    "_slow_exec1: problem with outputs tuple.");
	if (maxitemsize <= 0)
		return PyErr_Format(PyExc_ValueError,
				    "_slow_exec1: maxitemsize <= 0");
	niter = buffersize/maxitemsize;

	if (!(oshape = NA_intTupleFromMaybeLongs(outarr->nd, outarr->dimensions)))
		return NULL;
	result = _getBlockingParameters(oshape, niter, 0);
	if (!result) return NULL;
	if (!PyArg_ParseTuple(result, "iO:_slow_exec1 result", &indexlevel, &blocking))
		return NULL;
	Py_INCREF(blocking);
	Py_DECREF(result);
	{	
		PyObject *i0 = PyTuple_GET_ITEM(inputs, 0);
		PyObject *o0 = PyTuple_GET_ITEM(outputs, 0);
		if (!i0 || !o0 || 
		    !NA_ConverterCheck(i0) || !NA_ConverterCheck(o0))
			return PyErr_Format(
				PyExc_TypeError, 
				"_slow_exec1: bad converter object.");
		{
			PyObject *ri0 = ConverterRebuffer(i0, in1, Py_None);
 			PyObject *ro = ConverterRebuffer(o0, out, Py_None);
			if (!ri0 || !ro)
				return NULL;
			operator = PyObject_CallFunction(pOperatorClass, "(O[O][O]i)", 
							 cfunc, ri0, ro, 0);
			Py_DECREF(ri0);
			Py_DECREF(ro);
			if (!operator || !NA_OperatorCheck(operator)) 
				return PyErr_Format(
					PyExc_TypeError,
					"_slow_exec1: bad operator object");
		}
		objects = Py_BuildValue("ONO", i0, operator, o0);
		if (!objects) return NULL;
	}

#if _PYTHON_CALLBACKS
	result = PyObject_CallFunction(
		p_callOverDimensionsFunc, "(OOiOii)", 
		objects, oshape, indexlevel, blocking, 0, 0);
#else
	result = _callOverDimensions(objects, oshape, indexlevel, blocking, 0, 0);
#endif
	Py_DECREF(objects);
	Py_DECREF(oshape);
	Py_DECREF(blocking);      
	return result;
}

static PyObject*
_Py_slow_exec1(PyObject *module, PyObject *args)
{
	PyObject *ufunc, *in1, *out, *cached;

	if (!PyArg_ParseTuple(args, "OOOO:_Py_slow_exec1", 
			      &ufunc, &in1, &out, &cached))
		return NULL;

	if (!PyTuple_Check(cached) || PyTuple_GET_SIZE(cached) < 4)
		return PyErr_Format(PyExc_ValueError,
				    "_Py_slow_exec1: problem with cache tuple.");

	return _slow_exec1(ufunc, in1, out, cached);
}

static PyObject *
_cache_exec1(PyObject  *ufunc, PyObject *in1, PyObject *out, 
	     PyObject  *cached)
{
	PyObject *result, *mode;
	char *mode_str;
	long count;	
	assert(NA_NumArrayCheck(out));
	count = NA_elements((PyArrayObject *) out);
	if (count < 0) return NULL;
	if (count) {
		NA_clearFPErrors();
		mode = PyTuple_GET_ITEM(cached, 0);
		if (!PyString_Check(mode))
			return PyErr_Format(
				PyExc_ValueError, 
				"_cache_exec1: mode is not a string");
		mode_str = PyString_AS_STRING(mode);
		if (!strcmp(mode_str, "fast")) {
			result = _fast_exec1(ufunc, in1, out, cached);
		} else {
			result = _slow_exec1(ufunc, in1, out, cached);
		}
		if (_reportErrors(ufunc, result, out) < 0)
			return NULL;
	}
	Py_INCREF(out);
	return out;
}

static PyObject *
_Py_cache_exec1(PyObject *module, PyObject *args)
{
	PyObject *ufunc, *in1, *out, *cached;
	if (!PyArg_ParseTuple(args, "OOOO:_Py_cache_exec1", 
			      &ufunc, &in1, &out, &cached))
		return NULL;

	if (!NA_NumArrayCheck(out))
		return PyErr_Format(PyExc_TypeError,
				    "_cache_exec1: out is not a numarray.");

	if (!PyTuple_Check(cached) || PyTuple_GET_SIZE(cached) != 6)
		return PyErr_Format(PyExc_ValueError, 
				    "_cache_exec1: bad cache tuple");

	return _cache_exec1(ufunc, in1, out, cached);
}

static PyObject *
_cache_lookup1(PyObject *ufunc, PyObject *in1, PyObject *out,
	       PyObject **oin1, PyObject **oout,
	       PyObject **ocache)
{
	PyObject *ctuple;

	ctuple = _cache_lookup( &((PyUfuncObject *) ufunc)->cache, 
				in1, Py_None, out, NULL, NULL);

	if (ctuple) {  /* cache hit here */
		PyObject *insig, *cin1;
		char *insig_str;

		if (!PyTuple_Check(ctuple) || 
		    (PyTuple_GET_SIZE(ctuple) != 6)) {
			Py_DECREF(ctuple);
			return PyErr_Format(PyExc_RuntimeError,
					    "_cache_lookup1: invalid cache tuple");
		}
		cin1 = PyTuple_GET_ITEM(ctuple, 5);
		cin1 = _restuff_pseudo(cin1, in1);
		if (!cin1) return NULL;
		insig = PyTuple_GET_ITEM(ctuple, 4);
		if (!(insig_str = PyString_AsString(insig)))
			return PyErr_Format(PyExc_RuntimeError,
					    "_cache_lookup1: problem with insig string");
		if (out == Py_None) {
			PyObject *otype = PyTuple_GET_ITEM(ctuple, 1);
			out = _getNewArray(cin1, otype);
			if (!out) return NULL;
		} else { /* output array was supplied */
			if (!NA_NumArrayCheck(out))
				return PyErr_Format(PyExc_TypeError,
						    "_cache_lookup1: output array was not a numarray");
			if  (!NA_ShapeEqual((PyArrayObject *)cin1, 
					    (PyArrayObject *)out))
				return PyErr_Format(PyExc_ValueError,
						    "Supplied output array does not have appropriate shape");
			Py_INCREF(out);
		}
		*oin1 = cin1;
		*oout = out;
		*ocache = ctuple; 
		Py_INCREF(ctuple);
	} else { /* cache miss */
		PyObject *result;
		result = PyObject_CallMethod(ufunc, 
					     "_cache_miss1", "(OO)", in1, out);
		if (!result) return NULL;
		if (!PyArg_ParseTuple(result, "OOO:_cache_lookup1",
				      oin1, oout, ocache))
			return NULL;
		Py_INCREF(*oin1); 
		Py_INCREF(*oout); 
		Py_INCREF(*ocache);
		Py_DECREF(result);
	}
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
_Py_cache_lookup1(PyObject *module, PyObject *args)
{
	PyObject *ufunc, *in1, *out;
	PyObject *result;
	PyObject *oin1, *oout, *ocached;
	
	if (!PyArg_ParseTuple(args, "OOO:_Py_cache_lookup1", 
			      &ufunc, &in1, &out))
		return NULL;

	result = _cache_lookup1(ufunc, in1, out, 
				&oin1, &oout, &ocached);
	
	if (!result) return NULL;
	Py_DECREF(result);
	
	return Py_BuildValue("NNN", oin1, oout, ocached);
}

static PyObject *
_cached_dispatch1(PyObject *ufunc, PyObject *in1, PyObject *out)
{
	PyObject *result, *nresult, *oin1, *oout, *ocached;

	result = _cache_lookup1(ufunc, in1, out, &oin1, &oout, &ocached);
	if (!result) 
		return NULL;
	else
		Py_DECREF(result);
	
	result = _cache_exec1( ufunc, oin1, oout, ocached);

	Py_DECREF(oin1);
	Py_DECREF(oout);
	Py_DECREF(ocached);

	if (!result) return NULL;

	nresult = _normalize_results(1, &in1, 1, &out, 1, &result, 0);
	Py_DECREF(result);
	return nresult;
}

static PyObject *
_Py_cached_dispatch1(PyObject *module, PyObject *args)
{
	PyObject *ufunc, *in1, *out;
	if (!PyArg_ParseTuple(args, "OOO:_Py_cache_dispatch1", 
			      &ufunc, &in1, &out))
		return NULL;

	return _cached_dispatch1(ufunc, in1, out);
}

static PyObject *
_fast_exec2(PyObject *ufunc, PyObject *in1, PyObject *in2, PyObject *out, PyObject *cached)
{
	/* PyObject *otype = PyTuple_GET_ITEM(cached, 1); */
	PyObject *cfunc = PyTuple_GET_ITEM(cached, 2);
	PyObject *result;
	{
		PyArrayObject *in1a = (PyArrayObject*) in1;
		PyArrayObject *in2a = (PyArrayObject*) in2;
		PyArrayObject *outa = (PyArrayObject *) out;
		PyObject *buffers[3];
		long      offsets[3];
		buffers[0] = in1a->_data;
		buffers[1] = in2a->_data;
		buffers[2] = outa->_data;
		offsets[0] = in1a->byteoffset;
		offsets[1] = in2a->byteoffset;
		offsets[2] = outa->byteoffset;
		result = NA_callCUFuncCore( cfunc, NA_elements(outa), 2, 1, 
					    buffers, offsets);
	} 
	return result;
}

static PyObject*
_Py_fast_exec2(PyObject *module, PyObject *args)
{
	PyObject *ufunc, *in1, *in2, *out, *cached;

	if (!PyArg_ParseTuple(args, "OOOOO:_Py_fast_exec2", 
			      &ufunc, &in1, &in2, &out, &cached))
		return NULL;

	return _fast_exec2(ufunc, in1, in2, out, cached);
}

static PyObject *
_slow_exec2(PyObject *ufunc, PyObject *in1, PyObject *in2, 
	    PyObject *out, PyObject *cached)
{
	PyObject *blocking;
	PyObject *inputs, *outputs, *result, *operator, *objects, *oshape;
	int maxitemsize, niter, indexlevel;
	PyObject *cfunc;
	PyObject *ufargs;
	PyArrayObject *outarr = (PyArrayObject *) out;
	
	cfunc = PyTuple_GET_ITEM(cached, 2);
	ufargs = PyTuple_GET_ITEM(cached, 3);
	
	if (deferred_ufunc_init() < 0)
		return NULL;
	
	if (!PyTuple_Check(ufargs) || PyTuple_GET_SIZE(ufargs) != 3)
		return PyErr_Format(PyExc_ValueError,
				    "_slow_exec1: problem with ufargs tuple.");
	if (!PyArg_ParseTuple(ufargs, "OOi:_slow_exec2 ufargs", 
			      &inputs, &outputs, &maxitemsize))
		return NULL;
	if (!PyTuple_Check(inputs) || PyTuple_GET_SIZE(inputs) != 2)
		return PyErr_Format(PyExc_ValueError, 
				    "_slow_exec2: problem with inputs tuple.");
	if (!PyTuple_Check(outputs) || PyTuple_GET_SIZE(outputs) != 1)
		return PyErr_Format(PyExc_ValueError, 
				    "_slow_exec2: problem with outputs tuple.");
	if (maxitemsize <= 0)
		return PyErr_Format(PyExc_ValueError,
				    "_slow_exec2: maxitemsize <= 0");
	niter = buffersize/maxitemsize;
	
	if (!(oshape = NA_intTupleFromMaybeLongs( 
		      outarr->nd, outarr->dimensions )))
		return NULL;
	result = _getBlockingParameters(oshape, niter, 0);
	if (!result) return NULL;
	if (!PyArg_ParseTuple(
		    result, "iO:_slow_exec2 result", &indexlevel, &blocking))
		return NULL;
	Py_INCREF(blocking);
	Py_DECREF(result);
	{	
		PyObject *i0 = PyTuple_GET_ITEM(inputs, 0);
		PyObject *i1 = PyTuple_GET_ITEM(inputs, 1);
		PyObject *o0 = PyTuple_GET_ITEM(outputs, 0);
		if (!i0 || !i1 || !o0)
			return PyErr_Format( 
				PyExc_RuntimeError, 
				"_slow_exec2: NULL converter object.");
		{
 			PyObject *ri0 = ConverterRebuffer(i0, in1, Py_None);
 			PyObject *ri1 = ConverterRebuffer(i1, in2, Py_None);
 			PyObject *ro = ConverterRebuffer(o0, out, Py_None);
			if (!ri0 || !ri1 || !ro)
				return NULL;
			operator = PyObject_CallFunction( 
				pOperatorClass, "(O[OO][O]i)", cfunc, ri0, ri1, ro, 0);
			if (!operator) return NULL;
			Py_DECREF(ri0);
			Py_DECREF(ri1);
			Py_DECREF(ro);
		}
		objects = Py_BuildValue("OONO", i0, i1, operator, o0);
		if (!objects) return NULL;
	}

#if _PYTHON_CALLBACKS
        result = PyObject_CallFunction(	
		p_callOverDimensionsFunc, "(OOiOii)", objects, oshape, indexlevel, blocking, 0, 0);
#else
        result = _callOverDimensions(
		objects, oshape, indexlevel, blocking, 0, 0);
#endif
	{ /* Lose all cached references to these numarray */
		PyObject *i0 = PyTuple_GET_ITEM(inputs, 0);
		PyObject *i1 = PyTuple_GET_ITEM(inputs, 1);
		PyObject *o0 = PyTuple_GET_ITEM(outputs, 0);
		if (!i0 || !i1 || !o0)
			return PyErr_Format( 
				PyExc_RuntimeError, 
				"_slow_exec2: NULL converter object.");
		{
 			PyObject *ri0 = ConverterRebuffer(i0, Py_None, Py_None);
 			PyObject *ri1 = ConverterRebuffer(i1, Py_None, Py_None);
 			PyObject *ro = ConverterRebuffer(o0, Py_None, Py_None);
			if (!ri0 || !ri1 || !ro)
				return NULL;
			Py_DECREF(ri0);
			Py_DECREF(ri1);
			Py_DECREF(ro);
		}
	}
	Py_DECREF(objects);
	Py_DECREF(oshape);
	Py_DECREF(blocking);      
	return result;
}

static PyObject*
_Py_slow_exec2(PyObject *module, PyObject *args)
{
	PyObject *ufunc, *in1, *in2, *out, *cached;

	if (!PyArg_ParseTuple(args, "OOOOO:_Py_slow_exec2", 
			      &ufunc, &in1, &in2, &out, &cached))
		return NULL;

	if (!PyTuple_Check(cached) || PyTuple_GET_SIZE(cached) != 7)
		return PyErr_Format(PyExc_ValueError,
				    "_Py_slow_exec2: problem with cache tuple.");

	return _slow_exec2(ufunc, in1, in2, out, cached);
}

static PyObject *
_cache_exec2(PyObject  *ufunc, PyObject *in1, PyObject *in2, PyObject *out, 
	     PyObject  *cached)
{
	PyObject *result, *mode;
	char *mode_str;
	long count;

	assert(NA_NumArrayCheck(out));
	count = NA_elements((PyArrayObject *)out);
	if (count < 0) return NULL;
	if (count) {
		NA_clearFPErrors();
		mode = PyTuple_GET_ITEM(cached, 0);
		if (!PyString_Check(mode))
			return PyErr_Format(
				PyExc_ValueError, 
				"_cache_exec2: mode is not a string");
		mode_str = PyString_AS_STRING(mode);
		if (!strcmp(mode_str, "fast")) {
			tc_start_clock("_cache_exec2 fast");
			result = _fast_exec2(ufunc, in1, in2, out, cached);
			tc_stop_clock("_cache_exec2 fast");
		} else {
			tc_start_clock("_cache_exec2 slow");
			result = _slow_exec2(ufunc, in1, in2, out, cached);
			tc_stop_clock("_cache_exec2 slow");
		}
		if (_reportErrors(ufunc, result, out) < 0)
			return NULL;
	}
	Py_INCREF(out);
	return out;
}

static PyObject *
_Py_cache_exec2(PyObject *module, PyObject *args)
{
	PyObject *ufunc, *in1, *in2, *out, *cached;
	if (!PyArg_ParseTuple(args, "OOOOO:_Py_cache_exec2", 
			      &ufunc, &in1, &in2, &out, &cached))
		return NULL;
	if (!NA_NumArrayCheck(out))
		return PyErr_Format(PyExc_TypeError,
				    "_cache_exec2: out is not a numarray.");
	if (!PyTuple_Check(cached) || PyTuple_GET_SIZE(cached) != 7)
		return PyErr_Format(PyExc_ValueError, 
				    "_cache_exec2: bad cache tuple");
	return _cache_exec2(ufunc, in1, in2, out, cached);
}

static PyObject *
_cache_lookup2(PyObject *ufunc, PyObject *in1, PyObject *in2, PyObject *out,
	       PyObject **oin1, PyObject **oin2, PyObject **oout,
	       PyObject **ocache)
{
	PyObject *ctuple, *result=0, *bcast=0;

	tc_start_clock("_cache_lookup2 broadcast");
	if (NA_NumArrayCheck(in1) && NA_NumArrayCheck(in2) && 
	    !NA_ShapeEqual((PyArrayObject*)in1, (PyArrayObject*)in2)) 
	{
		bcast = PyObject_CallMethod(in1, "_dualbroadcast",
					    "(O)", in2);
		if (!bcast) return NULL;
		if (!PyTuple_Check(bcast) || (PyTuple_GET_SIZE(bcast) != 2))
			return PyErr_Format(PyExc_RuntimeError,
					    "_cache_lookup2: invalid broadcast result.");
		in1 = PyTuple_GET_ITEM(bcast, 0);
		in2 = PyTuple_GET_ITEM(bcast, 1);
		if (!NA_NumArrayCheck(in1) || !NA_NumArrayCheck(in2))
			return PyErr_Format(PyExc_RuntimeError,
					    "_cache_lookup2: invalid broadcast results.");
	}

	tc_stop_clock("_cache_lookup2 broadcast");

	tc_start_clock("_cache_lookup2 internal");
	ctuple = _cache_lookup( &((PyUfuncObject *)ufunc)->cache, 
				in1, in2, out, NULL, NULL);
	tc_stop_clock("_cache_lookup2 internal");

	tc_start_clock("_cache_lookup2 hit or miss");
	if (ctuple) {  /* cache hit here */
		PyObject *insig, *cin1, *cin2;
		char *insig_str;

		assert(PyTuple_Check(ctuple) && 
		       (PyTuple_GET_SIZE(ctuple) == 7));

		tc_start_clock("_cache_lookup2 hit pseudos");
		cin1 = PyTuple_GetItem(ctuple, 5);
		cin2 = PyTuple_GetItem(ctuple, 6);
		cin1 = _restuff_pseudo(cin1, in1);
		if (!cin1) goto _fail;
		cin2 = _restuff_pseudo(cin2, in2);
		if (!cin2) goto _fail;

		insig = PyTuple_GET_ITEM(ctuple, 4);
		if (!(insig_str = PyString_AsString(insig)))
			return PyErr_Format(PyExc_RuntimeError,
					    "_cache_lookup2: problem with insig string");
		tc_stop_clock("_cache_lookup2 hit pseudos");

		tc_start_clock("_cache_lookup2 hit output");
		if (out == Py_None) {
			PyObject *otype = PyTuple_GET_ITEM(ctuple, 1);

			if (!strcmp(insig_str, "vv") ||
			    !strcmp(insig_str, "vs")) {
				out = _getNewArray( cin1, otype);
				if (!out) goto _fail;
			} else if (!strcmp(insig_str, "sv")) {
				out = _getNewArray( cin2, otype);
				if (!out) goto _fail;
			} else { /* scalar_scalar -> rank0 */
				int otypenum = NA_typeObjectToTypeNo(otype);
				if (otypenum < 0) goto _fail;
				out = (PyObject *) NA_vNewArray(
					NULL, otypenum,
					0, NULL);
			}
			if (!out) goto _fail;
		} else { /* output array was supplied */
			PyObject *compare_to;
			if (!NA_NumArrayCheck(out))
				return PyErr_Format(PyExc_TypeError,
						    "_cache_lookup2: output array was not a numarray");
			if (!strcmp(insig_str, "vv") ||
			    !strcmp(insig_str, "vs")) {
				compare_to = cin1;
			} else if (!strcmp(insig_str, "sv")) {
				compare_to = cin2;
			} else {
				if (((PyArrayObject *)out)->nd)
					return PyErr_Format(PyExc_ValueError,
							    "_cache_lookup2: scalar output array rank > 0.");
				compare_to = out;
			}			
			if  (!NA_ShapeEqual((PyArrayObject *)out, 
					    (PyArrayObject *)compare_to))
				return PyErr_Format(PyExc_ValueError,
						    "Supplied output array does not have appropriate shape");
			Py_INCREF(out);
		}
		*oin1 = cin1;
		*oin2 = cin2;
		*oout = out;
		*ocache = ctuple; Py_INCREF(ctuple);

		tc_stop_clock("_cache_lookup2 hit output");
	} else { /* cache miss */
		tc_start_clock("_cache_lookup2 miss");
		result = PyObject_CallMethod(ufunc, 
					     "_cache_miss2", "(OOO)", in1, in2, out);
		if (!result) goto _fail;
		if (!PyArg_ParseTuple(result, "OOOO:_cache_lookup2", 
				      oin1, oin2, oout, ocache))
			goto _fail;
		Py_INCREF(*oin1); 
		Py_INCREF(*oin2); 
		Py_INCREF(*oout); 
		Py_INCREF(*ocache);
		Py_DECREF(result);
		tc_stop_clock("_cache_lookup2 miss");
	}
	tc_stop_clock("_cache_lookup2 hit or miss");
	result = Py_None;
	Py_INCREF(result);
  _fail:
	Py_XDECREF(bcast);
	return result;
}

static PyObject *
_Py_cache_lookup2(PyObject *module, PyObject *args)
{
	PyObject *ufunc, *in1, *in2, *out;
	PyObject *result;
	PyObject *oin1, *oin2, *oout, *ocached;
	
	if (!PyArg_ParseTuple(args, "OOOO:_Py_cache_lookup2", 
			      &ufunc, &in1, &in2, &out))
		return NULL;

	result = _cache_lookup2(ufunc, in1, in2, out,
				&oin1, &oin2, &oout, &ocached);
	
	if (!result) return NULL;
	Py_DECREF(result);
	
	return Py_BuildValue("NNNN", oin1, oin2, oout, ocached);
}

static PyObject *
_cached_dispatch2(PyObject *ufunc, PyObject *in1, PyObject *in2, PyObject *out)
{
	PyObject *nresult, *result, *oin1, *oin2, *oout, *ocached, *inputs[2];

	tc_start_clock("cached_dispatch2 overall");
	tc_start_clock("cached_dispatch2 lookup");
	result = _cache_lookup2( ufunc, in1, in2, out,
				 &oin1, &oin2, &oout, &ocached);
	tc_stop_clock("cached_dispatch2 lookup");
	if (!result) 
		return NULL;
	else
		Py_DECREF(result);

	tc_start_clock("cached_dispatch2 exec");
	result = _cache_exec2( ufunc, oin1, oin2, oout, ocached);
	tc_stop_clock("cached_dispatch2 exec");

	Py_DECREF(oin1);
	Py_DECREF(oin2);
	Py_DECREF(oout);
	Py_DECREF(ocached);

	tc_stop_clock("cached_dispatch2 overall");

	if (!result) return NULL;
	
	inputs[0] = in1;
	inputs[1] = in2;
	nresult = _normalize_results(2, inputs, 1, &out, 1, &result, 0);
	Py_DECREF(result);
	return nresult;
}

static PyObject *
_Py_cached_dispatch2(PyObject *module, PyObject *args)
{
	PyObject *ufunc, *in1, *in2, *out;
	if (!PyArg_ParseTuple(args, "OOOO:_Py_cached_dispatch2", 
			      &ufunc, &in1, &in2, &out))
		return NULL;
	return _cached_dispatch2(ufunc, in1, in2, out);

}

static PyObject *
_copyFromAndConvert(PyObject *inarr, PyObject *outarr)
{
	PyObject *cached, *input, *output, *blocking, *oshape;
	PyObject *blockingparameters, *inbuff, *outbuff, *objects;
	PyObject *key, *result;
	PyArrayObject  *outarra = (PyArrayObject *) outarr;
	int niter, indexlevel;

	if (deferred_ufunc_init() < 0) return NULL;

	if (!NA_ShapeEqual((PyArrayObject *)inarr, outarra))
		return PyErr_Format(
			PyExc_ValueError, 
			"_copyFromAndConvert: Arrays must have the same shape");
 	key = Py_BuildValue("(NNl)", _ldigest(inarr), _ldigest(outarr),
 			    PyThread_get_thread_ident());
 	
 	if (!key) {
 		if (!PyErr_ExceptionMatches(PyExc_KeyError))
 			return NULL;
 		cached = NULL;
 		PyErr_Clear();
 	} else {
 		cached = PyDict_GetItem(p_copyCacheDict, key);
 		if (cached) Py_INCREF(cached);
 		Py_DECREF(key);
 	}
	if (!cached) {
		cached =  PyObject_CallFunction( 
			p_copyFromAndConvertMissFunc, "(OO)", inarr, outarr);
		if (!cached) return NULL;
	}
	if (!PyArg_ParseTuple(cached, "OOi:_copyFromAndConvert cached", 
			      &input, &output, &niter))
		return NULL;

 	inbuff = ConverterRebuffer(input, inarr, Py_None);
	if (!inbuff) return NULL;

 	outbuff = ConverterRebuffer(output, outarr, inbuff);
	if (!outbuff) return NULL;

	Py_DECREF(inbuff);
	Py_DECREF(outbuff);

	oshape = NA_intTupleFromMaybeLongs(outarra->nd, outarra->dimensions);
	if (!oshape) return NULL;

	blocking = _getBlockingParameters(oshape, niter, 0);
	if (!blocking) return NULL;

	if (!PyArg_ParseTuple(blocking, "iO: _copyFromAndConvert blocking", 
			      &indexlevel, &blockingparameters))
		return NULL;

	objects = Py_BuildValue("(OO)", input, output);
	if  (!objects) return NULL;

	NA_clearFPErrors();
	{
#if _PYTHON_CALLBACKS
		result = PyObject_CallFunction( 
			p_callOverDimensionsFunc, "(OOiO)",
			objects, oshape, indexlevel, blockingparameters);
#else
		result = _callOverDimensions(
			objects, oshape, indexlevel, blockingparameters, 0, 0);
#endif
		Py_DECREF(blocking);
		Py_DECREF(oshape);
		Py_DECREF(objects);
		if (!result) return NULL; 

		ConverterClean(input, inarr);
		ConverterClean(output, inarr);
		ConverterClean(output, outarr);
	}
	Py_DECREF(cached);
	if (NA_checkAndReportFPErrors("_copyFromAndConvert") < 0) {
		Py_XDECREF(result);
		return NULL;
	}
	return result; /* Py_None */
}

static PyObject *
_Py_copyFromAndConvert(PyObject *module, PyObject *args)
{
	PyObject *from, *to;
	if (!PyArg_ParseTuple(args, "OO:_copyFromAndConvert", &from, &to))
		return NULL;

	if (!(NA_NumArrayCheck(from) && NA_NumArrayCheck(to)))
		return PyErr_Format(
			PyExc_TypeError, "_copyFromAndConvert: non-numarray input.");

	return _copyFromAndConvert(from, to);
}

static PyObject *
_Py_inputarray(PyObject *self, PyObject *args)
{
	PyObject *seq;
	if (!PyArg_ParseTuple(args, "O:_inputarray", &seq))
		return NULL;
	return (PyObject *) NA_InputArray(seq, tAny, 0);
}

static PyMethodDef _ufunc_functions[] = {
	{"_callOverDimensions", _Py_callOverDimensions, METH_VARARGS, 
	 "_callOverDimensions blockwise executes a set of functions"},
	{"digest", _Py_digest, METH_VARARGS, 
	 "digest(x) summarizes the cache-critical features of an array"},
	{"restuff_pseudo", _Py_restuff_pseudo, METH_VARARGS, 
	 "restuff_pseudo(p,x) stuffs scalar x into cached pseudo array p"},
	{"CheckFPErrors", CheckFPErrors, METH_VARARGS},
	{"_cached_dispatch1", _Py_cached_dispatch1, METH_VARARGS,
	 "_cached_dispatch1 tries to minimize the overhead of ufunc setup & call using a cache of prior setup results"},
	{"_cache_lookup1", _Py_cache_lookup1, METH_VARARGS, 
	 "_cache_lookup1  looks for a cached setup-state for a ufunc and creates one if none exists"},
	{"_cache_exec1", _Py_cache_exec1, METH_VARARGS, 
	 "_cache_exec1  looks for a cached setup-state for a ufunc and creates one if none exists"},
	{"_fast_exec1", _Py_fast_exec1, METH_VARARGS, 
	 "_fast_exec1  looks for a cached setup-state for a ufunc and creates one if none exists"},
	{"_slow_exec1", _Py_slow_exec1, METH_VARARGS, 
	 "_slow_exec1  executes a ufunc setup for converted ins and outs"},
	{"_cached_dispatch2", _Py_cached_dispatch2, METH_VARARGS,
	 "_cached_dispatch2 tries to minimize the overhead of ufunc setup & call using a cache of prior setup results"},
	{"_cache_lookup2", _Py_cache_lookup2, METH_VARARGS, 
	 "_cache_lookup2  looks for a cached setup-state for a ufunc and creates one if none exists"},
	{"_cache_exec2", _Py_cache_exec2, METH_VARARGS, 
	 "_cache_exec2  looks for a cached setup-state for a ufunc and creates one if none exists"},
	{"_fast_exec2", _Py_fast_exec2, METH_VARARGS, 
	 "_fast_exec2  executes a ufunc setup for unconverted ins and outs"},
	{"_slow_exec2", _Py_slow_exec2, METH_VARARGS, 
	 "_slow_exec2  executes a ufunc setup for converted ins and outs"},
	{"_getBlockingParameters", _Py_getBlockingParameters, METH_VARARGS,
	 "_getBlockingParameters returns block breakdown of a ufunc call"},
	{"_copyFromAndConvert", _Py_copyFromAndConvert, METH_VARARGS,
	 "_copyFromAndConvert copies from one array to another handling 'striding' and conversions"},
	{"_inputarray", _Py_inputarray, METH_VARARGS,
	 "_inputarray demonstrates results of NA_InputArray"},
	{"getBufferSize", _Py_getBufferSize, METH_VARARGS,
	 "returns the buffer size in bytes used for ufunc blocking"},
	{"setBufferSize", _Py_setBufferSize, METH_VARARGS,
	 "modifies size of new ufunc buffers allocated & ufunc blocking"},
	{"_normalize_results", _Py_normalize_results, METH_VARARGS,
	 "provides standard ufunc-like result handling"},
	{NULL,		NULL}		/* sentinel */
};



DL_EXPORT(void)
	init_ufunc(void)
{
	PyObject *m;

	_ufunc_type.tp_alloc = PyType_GenericAlloc;

	if (PyType_Ready(&_ufunc_type) < 0)
		return;

	m = Py_InitModule3("_ufunc",
			   _ufunc_functions,
			   _ufunc__doc__);
	if (m == NULL)
		return;

	Py_INCREF(&_ufunc_type);
	if (PyModule_AddObject(m, "_ufunc",
			       (PyObject *) &_ufunc_type) < 0)
		return;

	ADD_VERSION(m);

	import_libnumarray();

	import_libtc();
}

