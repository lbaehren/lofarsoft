#include "Python.h"
#include "structmember.h"
#include "libnumarray.h"

char *_numerictype__doc__ = "_numerictype defines the _numerictype type which is the basetype for numarray's type objects.  It exists in C primarily to support fast comparisons.";

#define DEFERRED_ADDRESS(ADDR) 0

typedef struct {
	PyObject_HEAD
	NumarrayType typeno;
	long         hash;
	PyObject     *name;
	PyObject     *defval;
	PyObject     *bytes;
} PyNumericTypeObject;
 
staticforward PyTypeObject _numerictype_type;



static PyObject *typeDict;  /* dictionary of aliases    name:numerictype */


static PyObject *
_numerictype_name_get(PyNumericTypeObject *self)
{
	Py_INCREF(self->name);
	return self->name;
}

static PyObject *
_numerictype_typeno_get(PyNumericTypeObject *self)
{
	return PyInt_FromLong(self->typeno);
}

static PyObject *
_numerictype_bytes_get(PyNumericTypeObject *self)
{
	Py_INCREF(self->bytes);
	return self->bytes;
}

static PyObject *
_numerictype_defval_get(PyNumericTypeObject *self)
{
	Py_INCREF(self->defval);
	return self->defval;
}

static PyGetSetDef _numerictype_getsets[] = {
	{"name", 
	 (getter)_numerictype_name_get, 
	 (setter)0, 
	 "determines if Numerictype is unary or binary"},
	{"typeno", 
	 (getter)_numerictype_typeno_get, 
	 (setter)0,
	 "type number / rank of the numerictype"},
	{"bytes", 
	 (getter)_numerictype_bytes_get, 
	 (setter)0,
	 "number of bytes of the numerictype"},
	{"default", 
	 (getter)_numerictype_defval_get, 
	 (setter)0,
	 "default value of the numerictype"},
	{0},
};


static PyObject *
_numerictype_new(PyTypeObject *type, PyObject  *args, PyObject *kwds)
{
	PyNumericTypeObject *self = 
		(PyNumericTypeObject *) PyType_GenericNew(type, args, kwds);
	if (!self) return NULL;
	self->name = PyString_FromString("no name");
	if (!self->name) return NULL;		
	self->defval = Py_None;
	Py_INCREF(Py_None);
	self->bytes = Py_None;
	Py_INCREF(Py_None);
	self->typeno = -1;
	return (PyObject *) self;
}

static int
_numerictype_init(PyNumericTypeObject *self, PyObject  *args, PyObject *kwds)
{
	PyObject *name, *bytes, *defval;
	long typeno;

	if (!PyArg_ParseTuple(args, "OOOl", &name, &bytes, &defval, &typeno))
		return -1;

	if (!PyString_Check(name)) {
		PyErr_Format(PyExc_TypeError, "name must be a string");
		return -1;
	}

	Py_DECREF(self->name);
	self->name = name;
	Py_INCREF(self->name);

	Py_DECREF(self->bytes);
	self->bytes = bytes;
	Py_INCREF(bytes);

	Py_DECREF(self->defval);
	self->defval = defval;
	Py_INCREF(defval);

	self->hash = PyObject_Hash(self->name);

	self->typeno = typeno;

	return 0;
}

static void
_numerictype_dealloc(PyNumericTypeObject *self)
{
	Py_DECREF(self->name);
	Py_DECREF(self->bytes);
	Py_DECREF(self->defval);
	self->ob_type->tp_free((PyObject *)self);
}

static int
_numerictype_compare(PyNumericTypeObject *self, PyObject *other)
{
	int diff;
	NumarrayType otherno = 1000;
	if (PyObject_IsInstance((PyObject *) other, 
				(PyObject *) &_numerictype_type)) {
		otherno = ((PyNumericTypeObject *)other)->typeno;
	} else if (PyString_Check(other)) { 		
		PyObject *otherObj = PyDict_GetItem(typeDict, other);
		if (otherObj)
			return _numerictype_compare(self, otherObj);
	}
	diff = self->typeno - otherno;
	if (diff < 0)
		return -1;
	else if (diff == 0)
		return 0;
	else
		return 1;
}

static PyObject *
_numerictype_richcompare(PyNumericTypeObject *self, PyObject *other, int op)
{
	int result = _numerictype_compare(self, other);
	int rval;

	switch(op) {
	case Py_LT:  rval = (result < 0); break;
	case Py_LE:  rval = (result <= 0); break;
	case Py_GT:  rval = (result > 0); break;
	case Py_GE:  rval = (result >= 0); break;
	case Py_EQ:  rval = (result == 0); break;
	case Py_NE:  rval = (result != 0); break;
	default:   
		rval = -1;
		PyErr_Format(PyExc_RuntimeError,
			     "_numerictype: unknown rich comparison operator");
		break;
	}
#if PY_VERSION_HEX < 0x02030000
	return PyInt_FromLong(rval);
#else
	return PyBool_FromLong(rval);
#endif
}


static PyObject *
_numerictype_repr(PyNumericTypeObject *self)
{
	Py_INCREF(self->name);
	return self->name;
}

static long
_numerictype_hash(PyNumericTypeObject *self)
{
	return self->hash;
}

static PyMethodDef _numerictype_methods[] = {
	{NULL,	NULL},
};

static PyTypeObject _numerictype_type = {
	PyObject_HEAD_INIT(DEFERRED_ADDRESS(&PyType_Type))
	0,
	"numarray._numerictype._numerictype",
	sizeof(PyNumericTypeObject),
	0,
	(destructor) _numerictype_dealloc,      /* tp_dealloc */
	0,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	(cmpfunc) _numerictype_compare,   	/* tp_compare */
	(reprfunc) _numerictype_repr,		/* tp_repr */
	0,					/* tp_as_number */
	0,                                      /* tp_as_sequence */
	0,			                /* tp_as_mapping */
	(hashfunc) _numerictype_hash,		/* tp_hash */
	0,					/* tp_call */
	(reprfunc) _numerictype_repr,		/* tp_str */
	0,					/* tp_getattro */
	0,					/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | 
        Py_TPFLAGS_BASETYPE,                    /* tp_flags */
	0,					/* tp_doc */
	0,					/* tp_traverse */
	0,					/* tp_clear */
	(richcmpfunc) _numerictype_richcompare, /* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	_numerictype_methods,			/* tp_methods */
	0,					/* tp_members */
	_numerictype_getsets,			/* tp_getset */
	0,                                      /* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	(initproc) _numerictype_init,           /* tp_init */
	0, 		                        /* tp_alloc */
	_numerictype_new,		        /* tp_new */
};

static PyMethodDef _numerictype_functions[] = {
	{NULL,		NULL}		/* sentinel */
};

DL_EXPORT(void)
init_numerictype(void)
{
	PyObject *m;

	_numerictype_type.tp_alloc = PyType_GenericAlloc;
	

	if (PyType_Ready(&_numerictype_type) < 0)
		return;

	m = Py_InitModule3("_numerictype",
			   _numerictype_functions,
			   _numerictype__doc__);
	if (m == NULL)
		return;

	Py_INCREF(&_numerictype_type);
	if (PyModule_AddObject(
		    m, "_numerictype", (PyObject *) &_numerictype_type) < 0)
		return;

	typeDict = PyDict_New();
	if (PyModule_AddObject(m, "typeDict", typeDict) < 0)
		return;

	ADD_VERSION(m);

	import_libnumarray();
}

