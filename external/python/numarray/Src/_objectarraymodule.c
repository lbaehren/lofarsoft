#include "Python.h"
#include "structmember.h"
#include "libnumarray.h"

char *_objectarray__doc__ =
"_objectarray is a module which supplies the _objectarray baseclass of ObjectArray.  Class\n"
"_objectarray is a baseclass used to accelerate selected methods of ObjectArray.\n";

/* ======================================================================= */

staticforward PyTypeObject _objectarray_type;

#define DEFERRED_ADDRESS(ADDR) 0

typedef struct 
{
	PyArrayObject array;
	PyObject *objects;         /* PyObjectBuffer* */
} PyObjectArray;

static PyArray_Descr ObjectArrayDescr;  /* FORWARD */

static PyObject *
_objectarray_new(PyTypeObject *type, PyObject  *args, PyObject *kwds)
{
	PyObjectArray *self = (PyObjectArray *) 
		_objectarray_type.tp_base->tp_new( type, args, kwds );
	if (!self) return NULL;

	self->objects = Py_None;
	self->array.descr = &ObjectArrayDescr;
	Py_INCREF(Py_None);

	return (PyObject *) self;
}

static int
_objectarray_init(PyObjectArray *self, PyObject *args)
{
	maybelong i, ishape[MAXDIM], nshape, nelements;
	PyObject *shape, *objects, *args2;

	static PyObject *pdummyBuff;

	if (!PyArg_ParseTuple(args, "OO:_objectarray_init", 
			      &shape, &objects))
		return -1;

	nshape = NA_maybeLongsFromIntTuple(MAXDIM, ishape, shape);
	if (nshape < 0) return -1;

	for(i=0, nelements=1; i<nshape; i++)
		nelements *= ishape[i];

	if (objects != Py_None) {
		int seqlen = PySequence_Length(objects);
		if (seqlen < 0) return -1;
		if (seqlen != nelements) {
			PyErr_Format(PyExc_ValueError, 
				     "_objectarray_init: shape/objects mismatch");
			return -1;
		}
	} 
	
	Py_XDECREF(self->objects);
	self->objects = PyList_New(nelements);
	if (!self->objects) return -1;

	if (objects != Py_None) {
		for(i=0; i<nelements; i++) {
			PyObject *obj = PySequence_GetItem(objects, i);
			if (!obj) return -1;
			if (PyList_SetItem(self->objects, i, obj) < 0)
				return -1;
		}
	} else {
		for(i=0; i<nelements; i++) {
			Py_INCREF(Py_None);
			if (PyList_SetItem(self->objects, i, Py_None) < 0)
				return -1;
		}
	}	

	if (!pdummyBuff) 
		pdummyBuff = NA_initModuleGlobal(
			"numarray.objects", "_dummyBuffer");
	if (!pdummyBuff) return -1;

	args2 = Py_BuildValue("(OiOiii)", shape, 1, pdummyBuff, 0, 1, 1);
	if (!args2) return -1;

	if (_objectarray_type.tp_base->tp_init( 
		    (PyObject *) &self->array, args2, NULL) < 0) {
		return -1;
	}

	Py_DECREF(args2);
		
	return 0;
}

static void
_objectarray_dealloc(PyObject *self)
{
	PyObjectArray *me = (PyObjectArray *) self;
	Py_XDECREF( me->objects );
	_objectarray_type.tp_base->tp_dealloc(self);
}

static PyObject *
_objectarray_objects_get(PyObjectArray *self)
{
	Py_INCREF(self->objects);
	return self->objects;
}

static int
_objectarray_objects_set(PyObjectArray *self, PyObject *s)
{
	if (!s) { 
		PyErr_Format(PyExc_RuntimeError, "can't delete objects"); 
		return -1; 
	}
	Py_XDECREF(self->objects);
	self->objects = s;
	Py_INCREF(s);
	return 0;
}

static PyGetSetDef _objectarray_getsets[] = {
	{"_objects", 
	 (getter)_objectarray_objects_get, 
	 (setter)_objectarray_objects_set, 
	 "_objectbuffer object store"}, 
	{0}
};

/* ======================================================================== */

static PyObject *
_objectarray_get(PyArrayObject *self, long offset)
{
	PyObjectArray *me = (PyObjectArray *) self;
	return PySequence_GetItem(me->objects, offset + self->byteoffset);
}

static PyObject *
_Py_objectarray_getitem(PyObjectArray *self, PyObject *args)
{

	long offset;
	if (!PyArg_ParseTuple(args, "l:_getitem", &offset))
		return NULL;

	return _objectarray_get(&self->array, offset);
}

static int
_objectarray_set(PyArrayObject *self, long offset, PyObject *value)
{
	PyObjectArray *me = (PyObjectArray *) self;
	return PySequence_SetItem(me->objects, offset + self->byteoffset, value);
}

static PyObject *
_Py_objectarray_setitem(PyObjectArray *self, PyObject *args)
{
	long offset;
	PyObject *value;
	if (!PyArg_ParseTuple(args, "lO:_setitem", &offset, &value))
		return NULL;
	if (_objectarray_set(&self->array, offset, value) < 0)
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}

static PyArray_Descr 
ObjectArrayDescr = {
	tObject, sizeof(PyObject *), 'O', 
	_objectarray_get,
	_objectarray_set
};

static int 
_copyObject(PyObjectArray *self, PyObjectArray *other, long selfix, long otherix)
{
	int rval;
	PyObject *obj = PySequence_GetItem(other->objects, otherix);
	if (!obj) return -1;
	rval = PySequence_SetItem(self->objects, selfix, obj);
	Py_DECREF(obj);
	return rval;
}

static int
_copyObjects( PyObjectArray *self, PyObjectArray *other,
	      long selfoffset, long otheroffset, long dim )
{
	long i;
	if (self->array.nd == dim+1) {
		for(i=0; i<self->array.dimensions[dim]; i++) {
			long selfix = selfoffset + i*self->array.strides[dim];
			long otherix = otheroffset + i*other->array.strides[dim];
			if (_copyObject(self, other, selfix, otherix) < 0)
				return -1;
		}
	} else if (self->array.nd == 0) {
		if (_copyObject(self, other, selfoffset, otheroffset) < 0)
			return -1;
	} else {
		for(i=0; i<self->array.dimensions[dim]; i++) 
			if (_copyObjects( self, other, 
				      selfoffset+self->array.strides[dim]*i,
				      otheroffset+other->array.strides[dim]*i,
				      dim+1) < 0)
				return -1;
	}
	return 0;
}

static PyObject *
_Py_objectarray_copyObjects(PyObjectArray *self, PyObject *args)
{
	long      selfoffset, otheroffset, dim;
	PyObjectArray  *other;
	int result;

	if (!PyArg_ParseTuple(
		    args, "Olll:_copyObjects", &other, 
		    &selfoffset, &otheroffset, &dim))
		return NULL;

	result = _copyObjects(self, other, selfoffset, otheroffset, dim);
	if  (result < 0) return NULL;

	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef 
_objectarray_methods[] = {
	{"_getitem", 
	 (PyCFunction)_Py_objectarray_getitem, METH_VARARGS,
	 "_getitem(key) -> object at key"},
	{"_setitem", 
	 (PyCFunction)_Py_objectarray_setitem, METH_VARARGS,
	 "_setitem(key, value) -> set array[key] to value"},
	{"_copyObjects", 
	 (PyCFunction)_Py_objectarray_copyObjects, METH_VARARGS,
	 "_copyObjects(self, other, selfoffset, otheroffset, dim) "
	 "copies objects from 'other' onto 'self'."},
	{NULL,	NULL},
};

static PyTypeObject 
_objectarray_type = {
	PyObject_HEAD_INIT(DEFERRED_ADDRESS(&PyType_Type))
	0,
	"numarray._objectarray._objectarray",
	sizeof(PyObjectArray),
	0,
	_objectarray_dealloc,	                /* tp_dealloc */
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
	_objectarray_methods,			/* tp_methods */
	0,					/* tp_members */
	_objectarray_getsets,  			/* tp_getset */
	DEFERRED_ADDRESS(&_ndarray_type),       /* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	(initproc)_objectarray_init,		/* tp_init */
	0, 		                        /* tp_alloc */
	_objectarray_new,			/* tp_new */
};

static int
_applyObjects1(long dim, 
	       PyObject *f, PyArrayObject *in1, PyArrayObject *out,
	       long in1off, long outoff)
{
	int rval = -1;
	if (dim == in1->nd) {
		PyObject *v1, *vo;
		v1 = _objectarray_get(in1, in1off);
		if (!v1) goto _exit;
		vo = PyObject_CallFunction(f, "(O)", v1);
		Py_DECREF(v1);
		if (!vo) goto _exit;
		if  (_objectarray_set(out, outoff, vo) < 0)
			goto _exit;
		Py_DECREF(vo);
	} else {
		long i;
		for (i=0; i<in1->dimensions[dim]; i++) {
			_applyObjects1(dim+1, f, in1, out,
				      in1off + in1->strides[dim]*i,
				      outoff + out->strides[dim]*i);
		}
	}
	rval = 0;
  _exit:
	return rval;
}

static PyObject *
_objectarray_applyObjects1(PyObject *module, PyObject *args)
{
	PyObject *f;
	PyObject *in1, *out;
	PyArrayObject *in1a, *outa;

	if (!PyArg_ParseTuple(args, "OOO:_applyObjects1",
			      &f, &in1, &out))
		return NULL;

	if (!NA_NDArrayCheck(in1) || !NA_NDArrayCheck(out))
		return PyErr_Format(PyExc_TypeError, 
				    "_applyObjects1: non-NDArray parameter");

	in1a = (PyArrayObject *) in1;
	outa = (PyArrayObject *) out;

	if (!NA_ShapeEqual(in1a, outa))
		return PyErr_Format(PyExc_ValueError, 
				    "_applyObjects1: array shape mismatch");

	if (_applyObjects1(0, f, in1a, outa, 0, 0) < 0)
		return NULL;

	Py_INCREF(Py_None);
	return Py_None;
}


static int
_applyObjects2(long dim, 
	       PyObject *f, PyArrayObject *in1, PyArrayObject  *in2, PyArrayObject *out,
	       long in1off, long in2off, long outoff)
{
	int rval = -1;
	if (dim == in1->nd) {
		PyObject *v1, *v2, *vo;
		v1 = _objectarray_get(in1, in1off);
		v2 = _objectarray_get(in2, in2off);
		if (!(v1 && v2)) goto _exit;
		vo = PyObject_CallFunction(f, "(OO)", v1, v2);
		Py_DECREF(v1);
		Py_DECREF(v2);
		if (!vo) goto _exit;
		if  (_objectarray_set(out, outoff, vo) < 0)
			goto _exit;
		Py_DECREF(vo);
	} else {
		long i;
		for (i=0; i<in1->dimensions[dim]; i++) {
			_applyObjects2(dim+1, f, in1, in2, out,
				      in1off + in1->strides[dim]*i,
				      in2off + in2->strides[dim]*i,
				      outoff + out->strides[dim]*i);
		}
	}
	rval = 0;
  _exit:
	return rval;
}

static PyObject *
_objectarray_applyObjects2(PyObject *module, PyObject *args)
{
	PyObject *f;
	PyObject *in1, *in2, *out;
	PyArrayObject *in1a, *in2a, *outa;

	if (!PyArg_ParseTuple(args, "OOOO:_applyObjects2",
			      &f, &in1, &in2, &out))
		return NULL;

	if (!NA_NDArrayCheck(in1) || 
	    !NA_NDArrayCheck(in2) ||
	    !NA_NDArrayCheck(out))
		return PyErr_Format(PyExc_TypeError, 
				    "_applyObjects2: non-NDArray parameter");

	in1a = (PyArrayObject *) in1;
	in2a = (PyArrayObject *) in2;
	outa = (PyArrayObject *) out;

	if (!NA_ShapeEqual(in1a, in2a) || !NA_ShapeEqual(in1a, outa))
		return PyErr_Format(PyExc_ValueError, 
				    "_applyObjects2: array shape mismatch");

	if (_applyObjects2(0, f, in1a, in2a, outa, 0, 0, 0) < 0)
		return NULL;

	Py_INCREF(Py_None);
	return Py_None;
}


static PyMethodDef 
_objectarray_functions[] = {
	{"_applyObjects1", 
	 (PyCFunction) _objectarray_applyObjects1, METH_VARARGS,
	 "_applyObjects1(f, *args) -> None  "
	 "recursively applies f to a tuple constructed elementwise from args"},
	{"_applyObjects2", 
	 (PyCFunction) _objectarray_applyObjects2, METH_VARARGS,
	 "_applyObjects2(f, *args) -> None  "
	 "recursively applies f to a tuple constructed elementwise from args"},
	{NULL,		NULL}		/* sentinel */
};

DL_EXPORT(void)
init_objectarray(void)
{
	PyObject *m;
	PyObject *nm, *nd, *nt;

	if (!(nm = PyImport_ImportModule("numarray._ndarray")))  /* NEW */ {
		PyErr_Format(
			PyExc_ImportError,
			"_objectarray: can't import ndarraytype extension.");
		return;
	}
	nd = PyModule_GetDict(nm);                          /* BOR */
	if (!(nt = PyDict_GetItemString(nd, "_ndarray")))   /* BOR */ {
		PyErr_Format(
			PyExc_ImportError,
			"_objectarray: can't get type _ndarray._ndarray");
		return;
	}
	if (!PyType_Check(nt)) {
		PyErr_Format(
			PyExc_ImportError,
			"_objectarray: _ndarray._ndarray isn't a type object");
		return;
	}

	_objectarray_type.tp_base = (PyTypeObject *) nt;
	_objectarray_type.tp_alloc = PyType_GenericAlloc;

	Py_INCREF(nt);
	Py_DECREF(nm); 

	if (PyType_Ready(&_objectarray_type) < 0)
		return;

	m = Py_InitModule3("_objectarray",
			   _objectarray_functions,
			   _objectarray__doc__);
	if (m == NULL)
		return;

	Py_INCREF(&_objectarray_type);
	if (PyModule_AddObject(m, "_objectarray",
			       (PyObject *) &_objectarray_type) < 0)
		return;

	ADD_VERSION(m);

	import_libnumarray();
}
