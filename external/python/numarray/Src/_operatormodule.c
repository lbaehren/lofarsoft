#include "Python.h"
#include "structmember.h"
#include "libnumarray.h"

char *_operator__doc__ =
"_operator is a module which supplies the _operator baseclass of Operator.  Class\n"
"_operator is a baseclass used to accelerate selected methods of Operator by\n"
"handling simple cases directly in C.  More complex cases are delegated to\n"
"python coded methods.\n";

#define DEFERRED_ADDRESS(ADDR) 0

staticforward PyTypeObject _operator_type;

static PyGetSetDef _operator_getsets[] = {
	{0},
};

static PyObject *_operator_compute(PyObject *self, PyObject *indices, PyObject *shape);

static PyObject *
_operator_new(PyTypeObject *type, PyObject  *args, PyObject *kwds)
{
	PyOperatorObject *self = 
		(PyOperatorObject *) PyType_GenericNew(type, args, kwds);
	if (!self) return NULL;

	self->compute = _operator_compute;
	self->inputs   = Py_None; Py_INCREF(Py_None);
	self->outputs  = Py_None; Py_INCREF(Py_None);
	self->cfunc    = Py_None; Py_INCREF(Py_None);
	self->striding = 0;

	if (!self->inputs || !self->outputs || !self->cfunc)
		return NULL; 

	return (PyObject *) self;
}

static int
_operator_init(PyObject *self, PyObject  *args, PyObject *kwds)
{
	PyOperatorObject *me = (PyOperatorObject *) self;
	int striding = 0;

	Py_DECREF(me->inputs);
	Py_DECREF(me->outputs);
	Py_DECREF(me->cfunc);

	if (!PyArg_ParseTuple(args, "OOO|i", &me->cfunc, 
			      &me->inputs, &me->outputs, &striding))
		return -1;

	if (!PySequence_Check(me->inputs) || 
	    !PySequence_Check(me->outputs)) {
		PyErr_Format(PyExc_TypeError,
		    "_operator_init: inputs or outputs not a sequence.");
		return -1;
	}

	Py_INCREF(me->inputs);
	Py_INCREF(me->outputs);
	Py_INCREF(me->cfunc);
	me->striding = striding;

	return 0;
}

static void
_operator_dealloc(PyObject *self)
{
	PyOperatorObject *me = (PyOperatorObject *) self;
	Py_XDECREF(me->inputs);
	Py_XDECREF(me->outputs);
	Py_XDECREF(me->cfunc);
	self->ob_type->tp_free(self);
}

#define MAXIO (MAXDIM+10)

static PyObject *
_operator_buffer(PyOperatorObject *self, PyObject *obj, int i)
{
	PyObject *buffer;
	assert(NA_NDArrayCheck(obj));
	buffer = ((PyArrayObject *) obj)->_data;
	if (!buffer)
		PyErr_Format( PyExc_RuntimeError,
		      "_operator_compute: obj[%d] problem with _data.", i);
	return buffer;
}

static int 
_operator_buffer_offset(PyOperatorObject *self, PyObject *obj, 
			maybelong nindices, maybelong *indices, long *offset)
{
	assert(NA_NDArrayCheck(obj));
	if (((PyArrayObject *) obj)->nstrides >= 0)  /* < 0  -->  a._strides is None */
		return NA_getByteOffset((PyArrayObject *) obj, nindices, indices, offset);
	*offset = 0;
	return 0;
}

static void
_operator_buffer_strides(PyOperatorObject *self, PyObject *obj,
			 maybelong nshape, maybelong *shape, int itemsize,
			 maybelong  *nstrides, maybelong *strides)
{
	PyArrayObject *obja = (PyArrayObject *) obj;
	int i;
	assert(NA_NDArrayCheck(obj));
	if (obja->nstrides >= 0) { /* not a _UBuffer */
		*nstrides = obja->nstrides;
		for(i=0; i<obja->nstrides; i++)
			strides[i] = obja->strides[i];
	} else {
		NA_stridesFromShape( nshape, shape, itemsize, strides);
		*nstrides = nshape;
	}
}	

static int
_operator_data_offset(PyOperatorObject *self, int slen, PyObject *sequence,
		      maybelong nindices, maybelong *indices,
		      PyObject **buffer, long *offset)
{
	int i;
	for(i=0; i<slen; i++) {
		PyObject *obj = PySequence_GetItem(sequence, i);
		if  (!obj) goto _fail2;
		if (!(buffer[i] = _operator_buffer(self, obj, i)))
			goto  _fail2;
		if (_operator_buffer_offset(self, obj, nindices, indices, offset+i) < 0)
			goto _fail2;
		Py_DECREF(obj);
		continue;
	  _fail2:		       
		Py_XDECREF(obj);
		return -1;
	}
	return 0;
}

static PyObject *_operator_compute(PyObject *me, PyObject *oindices, PyObject *oshape)
{
	PyOperatorObject *self = (PyOperatorObject*) me;
	PyObject *result = NULL;
	int ninputs = PySequence_Size(self->inputs);
	int noutputs = PySequence_Size(self->outputs);
	maybelong nshape, shape[MAXDIM];
	maybelong nindices, indices[MAXDIM];
	
	nshape = NA_maybeLongsFromIntTuple(MAXDIM, shape, oshape);
	if (nshape < 0) return NULL;

	nindices = NA_maybeLongsFromIntTuple(MAXDIM, indices, oindices);
	if (nindices < 0) return NULL;

	if (ninputs+noutputs > MAXIO)
		return PyErr_Format( PyExc_ValueError, 
		     "_operator_compute: too many inputs + outputs");

	if (self->striding) {
		PyObject *input=NULL, *output=NULL;
		PyObject *inbuffer, *outbuffer;
		long inoffset, outoffset;
		maybelong ninstrides, instrides[MAXDIM];
		maybelong noutstrides, outstrides[MAXDIM], 
			*poutstrides = outstrides;
		assert(ninputs==1 && noutputs==1);
		input = PySequence_GetItem(self->inputs, 0);
		if (!input) return NULL;
		output = PySequence_GetItem(self->outputs, 0);
		if (!output) return NULL;
		if (_operator_buffer_offset(
			    self, input, nindices, indices, &inoffset) < 0)
			goto _fail;
		if (_operator_buffer_offset(
			    self, output, nindices, indices, &outoffset) < 0)
			goto _fail;
		if (!(inbuffer = _operator_buffer(self, input, 0)))
			goto _fail;
		if (!(outbuffer = _operator_buffer(self, output, 1)))
			goto _fail;
		_operator_buffer_strides(
			self, input, nshape, shape, self->striding,
			&ninstrides, instrides);
		_operator_buffer_strides(
			self, output, nshape, shape, self->striding,
			&noutstrides, outstrides);
		if (noutstrides > ninstrides) { /* hack for reductions */
			poutstrides += noutstrides - ninstrides;
			noutstrides = ninstrides;
		}
		result = NA_callStrideConvCFuncCore( 
			self->cfunc, nshape, shape,
			inbuffer, inoffset, ninstrides, instrides,
			outbuffer, outoffset, noutstrides, poutstrides, 0);
		Py_XDECREF(input);
		Py_XDECREF(output);
		goto _done;
	  _fail:
		assert(0);
		Py_XDECREF(input);
		Py_XDECREF(output);
	} else {
		PyObject *buffers[MAXIO];
		long niter, offsets[MAXIO];
		if (NA_intTupleProduct(oshape, &niter) < 0) {
			PyErr_Format(PyExc_RuntimeError, 
				     "_operator_compute: problem with shape");
			goto _fail2;
		}
		if (_operator_data_offset(
			    self, ninputs, self->inputs, nindices, indices,
			    buffers, offsets) < 0)
			goto _fail2;
		if (_operator_data_offset(
			    self, noutputs, self->outputs, nindices, indices,
			    buffers+ninputs, offsets+ninputs) < 0)
			goto _fail2;
		result = NA_callCUFuncCore(
			self->cfunc, niter, ninputs, noutputs, buffers, offsets);
	}			
  _done:	    
  _fail2:
	return result;
}

static PyObject *
_Py_operator_compute(PyObject *self, PyObject *args)
{
	PyObject *indices, *shape;
	
	if (!PyArg_ParseTuple(args, "OO:_operator_compute", 
			      &indices, &shape))
		return NULL;
	return _operator_compute(self, indices, shape);
}

static PyMethodDef _operator_methods[] = {
	{"compute", (PyCFunction)_Py_operator_compute, METH_VARARGS,
	   "compute(self, indices, shape)  computes one block at 'indices' with 'shape'"},
	{NULL,	NULL},
};

static PyTypeObject _operator_type = {
	PyObject_HEAD_INIT(DEFERRED_ADDRESS(&PyType_Type))
	0,
	"numarray._operator._operator",
	sizeof(PyOperatorObject),
	0,
	_operator_dealloc,	                /* tp_dealloc */
	0,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	0,					/* tp_repr */
	0,					/* tp_as_number */
	0,                                      /* tp_as_sequence */
	0,			                /* tp_as_mapping */
	0,					/* tp_hash */
	0,					/* tp_call */
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
	_operator_methods,			/* tp_methods */
	0,					/* tp_members */
	_operator_getsets,			/* tp_getset */
	0,                                      /* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	_operator_init,	                        /* tp_init */
	0, 		                        /* tp_alloc */
	_operator_new,				/* tp_new */
};

static PyMethodDef _operator_functions[] = {
/*	{"_isIntegerSequence", 
	 (PyCFunction) _operator_isIntegerSequence, 
	 METH_VARARGS,
	 "_isIntegerSequence(s) returns 1 if all elements of sequence s are integers, else 0"},
*/
	{NULL,		NULL}		/* sentinel */
};

DL_EXPORT(void)
init_operator(void)
{
	PyObject *m;

	_operator_type.tp_alloc = PyType_GenericAlloc;

	if (PyType_Ready(&_operator_type) < 0)
		return;

	m = Py_InitModule3("_operator",
			   _operator_functions,
			   _operator__doc__);
	if (m == NULL)
		return;

	Py_INCREF(&_operator_type);
	if (PyModule_AddObject(m, "_operator",
			       (PyObject *) &_operator_type) < 0)
		return;

	ADD_VERSION(m);

	import_libnumarray();
}
