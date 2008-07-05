#include <Python.h>
#include <stdio.h>
#include "arraybase.h"   /* For Int64 */
#include "nummacro.h"

#if defined(_MSC_VER)
#define SIZE_MAX 0x7CFFFFFFL
#elif defined(sun) || defined(__sgi)
#include <limits.h>
#elif defined(__IBMC__)
#include <limits.h>
#define SIZE_MAX ULONG_MAX
#else
#include <inttypes.h>
#endif

#ifndef SIZE_MAX
#define SIZE_MAX ULONG_MAX
#endif

staticforward PyTypeObject MemoryType;

static PyObject *memoryError;

typedef struct {
  PyObject_HEAD
  char     *ptr;
  char     *base;
  Int64     size;
  PyObject *master;
} MemoryObject;

static PyObject *
_new_memory(Int64 size)
{
	MemoryObject *memory;
	unsigned long base, align;

	if (size < 0)
		return PyErr_Format(
			PyExc_ValueError, 
			"new_memory: invalid region size.");
	if (size > SIZE_MAX)
		return PyErr_Format(
			PyExc_MemoryError,
			"new_memory: region size too large for size_t.");
	if (!(memory = PyObject_New(MemoryObject, &MemoryType))) 
		return NULL;

	memory->base = (char *) PyMem_New(
		double, size/sizeof(double) + (size%sizeof(double)!=0) + 1);
	if (!memory->base) {
		PyErr_Format(PyExc_MemoryError, "Couldn't allocate requested memory");
		return NULL;
	}
	base = ((unsigned long) memory->base) / sizeof(double);
	align = (((unsigned long) memory->base) % sizeof(double)) != 0;
	memory->ptr = (char *) ((base+align) * sizeof(double));
	memory->size = size;
	memory->master = NULL;
	return (PyObject *) memory;
}

static void
memory_dealloc(PyObject* self)
{
	MemoryObject *me = (MemoryObject  *) self;
	if (me->master) {
		Py_XDECREF(me->master);
	} else {
		PyMem_Free(me->base);
	}
	PyObject_Del(self);
}

static PyObject *
new_memory(PyObject* self, PyObject* args)
{
	Int64 size;
	if (!PyArg_ParseTuple(args,"L", &size)) 
		return NULL;
	return _new_memory(size);
}

static PyObject *
memory_buffer(PyObject *self, PyObject *args)  /* deprecated */
{
	return new_memory(self, args);
}

static PyObject *
memory_alias(PyObject *master, char *ptr, int size)
{
	MemoryObject *memory;
	if (size < 0)
		return PyErr_Format(
			PyExc_ValueError, "new_memory: invalid region size.");

	if (!(memory = PyObject_New(MemoryObject, &MemoryType)))
		return NULL;
	memory->base = memory->ptr = ptr;
	memory->size = size;
	memory->master = master;
	Py_INCREF(master);
	return (PyObject *) memory;
}

static PyObject *
writeable_buffer(PyObject *self, PyObject *args)
{
  PyObject *ob, *buf;
  int offset = 0;
  int size = Py_END_OF_BUFFER;
  
  if ( !PyArg_ParseTuple(args, "O|ii:writeable_buffer", &ob, &offset, &size) )
    return NULL;
  buf = PyBuffer_FromReadWriteObject(ob, offset, size);
  if (!buf) {
    PyErr_Clear();
    buf = PyObject_CallMethod(ob, "__buffer__", NULL);
    if (!buf) {
	    return PyErr_Format(PyExc_TypeError, 
				"couldn't get writeable buffer from object");
    }
  }
  return buf;
}

static PyObject *
memory_str(PyObject *self)
{
	MemoryObject *me = (MemoryObject *) self;
	return PyString_FromStringAndSize(me->ptr, me->size);
}

static PyObject * 
memory_repr(PyObject *self)
{
	MemoryObject *me = (MemoryObject *) self;
	char buffer[128];
	sprintf(buffer, 
		"<memory at 0x%08lx with size:0x%08lx held by object 0x%08lx aliasing object 0x%08lx>",
		(long) me->ptr, (long) me->size, (long) me, (long) me->master);
	return PyString_FromString(buffer);
}

/* Buffer methods */
static int
memory_getbuf(MemoryObject *self, int idx, void **pp)
{
	if ( idx != 0 ) {
		PyErr_SetString(memoryError,
				"memory objects only support one segment");
		return -1;
	}
	*pp = self->ptr;
	return self->size;
}

static int
memory_getsegcount(MemoryObject *self, int *lenp)
{
	if ( lenp )
		*lenp = self->size;
	return 1;
}

static long 
memory_length(MemoryObject *self)
{
	return self->size;
}

PyObject *
memory_from_string(PyObject *module, PyObject *args)
{
	int    size;
	char  *buffer;
	MemoryObject *memory;

	if (!PyArg_ParseTuple(args, "s#", &buffer, &size))
	  return NULL;

	memory = (MemoryObject *) _new_memory(size);
	if (!memory) return NULL;

	memcpy( memory->ptr, buffer, size);
	return (PyObject *) memory;
}

static PyObject *
memory_reduce(PyObject *self)
{
	PyObject *memory_module, *mdict, *factory, *string;
	MemoryObject *me = (MemoryObject *) self;
	if (!(memory_module = PyImport_ImportModule("numarray.memory")))
		return NULL;
	if (!(mdict = PyModule_GetDict(memory_module)))
		return NULL;
	if (!(factory = PyDict_GetItemString(mdict, "memory_from_string")))
		return PyErr_Format(memoryError, 
				    "can't find memory_from_string");
	if (!(string = PyString_FromStringAndSize(me->ptr, me->size)))
		return NULL;
	return Py_BuildValue("(O(N))", factory, string);
}

static PyObject *
memory_reduce_func(PyObject *module, PyObject *args)
{
  PyObject *memory;
  if (!PyArg_ParseTuple(args, "O", &memory))
    return NULL;
  return memory_reduce(memory);
}

static PyObject *
memory_sq_item(MemoryObject *self, int i)
{
	if (i < 0 || i >= self->size)
		return PyErr_Format(PyExc_IndexError, "index out of range");
	return PyInt_FromLong(self->ptr[i]);
}

/* slice is an alias of the region of the original buffer */
static PyObject *
memory_sq_slice(MemoryObject *self, int i, int j)
{
	if (i < 0) 
		i = 0;
	else if (i > self->size)
		i = self->size;
	if (j < i) 
		j = i;
	else if (j > self->size)
		j = self->size;
	return memory_alias((PyObject *) self, self->ptr+i,  j-i);
}

static int
memory_sq_ass_item(MemoryObject *self, int i, PyObject *obj)
{
	long value;
	
	if ((i < 0) || (i >= self->size)) {
		PyErr_Format(PyExc_IndexError, "index out of range");
		return -1;
	}
	if (PyInt_Check(obj)) {
		value = PyInt_AsLong(obj);
	} else if (PyString_Check(obj)) {
		if (PyString_Size(obj) > 1) {
			PyErr_Format(PyExc_IndexError, "can only assign single char strings");
			return -1;
		}
		value = *PyString_AsString(obj);
	} else {
		PyErr_Format(PyExc_TypeError, "argument must be an int or 1 char string.");
		return -1;
	}
	self->ptr[i] = value;
	return 0;
}

static int
memory_sq_ass_slice(MemoryObject *self, int i, int j, PyObject *obj)
{
	const char *source;

	if (i < 0) 
		i = 0;
	else if (i > self->size)
		i = self->size;
	if (j < i) 
		j = i;
	else if (j > self->size)
		j = self->size;

	if (PyObject_CheckReadBuffer(obj)) {
		int length;
		long rval = PyObject_AsReadBuffer(
			obj, (const void **) &source, &length);
		if (rval < 0)	return -1;
		if (length != j-i) {
			PyErr_Format(PyExc_ValueError, "buffer size mismatch");
			return -1;
		}
		memmove(self->ptr+i, source, length);
		return 0;
	}
	if (PySequence_Check(obj)) {
		long k, length = PySequence_Length(obj);
		if (length < 0) return -1;
		if (length != j-i) {
			PyErr_Format(PyExc_ValueError, "buffer size mismatch");
			return -1;
		}
		for(k=i; k<j; k++) { 
			PyObject *it = PySequence_GetItem(obj, k-i);
			if (!it) return -1;
			if (memory_sq_ass_item(self, k, it) < 0) return -1;
			Py_DECREF(it);
		}
		return 0;
	}
	PyErr_Format(PyExc_TypeError, 
		     "argument must support buffer protocol or be a sequence of ints or 1 char strings");
	return -1;
}

static PySequenceMethods memory_as_sequence = {
	(inquiry)memory_length, /*sq_length*/
	(binaryfunc)0, /*sq_concat*/
	(intargfunc)0, /*sq_repeat*/
	(intargfunc)      memory_sq_item,      /*sq_item*/
	(intintargfunc)   memory_sq_slice,     /*sq_slice*/
	(intobjargproc)   memory_sq_ass_item,  /*sq_ass_item*/
	(intintobjargproc)memory_sq_ass_slice, /*sq_ass_slice*/
};

static PyBufferProcs memory_as_buffer = {
	(getreadbufferproc)memory_getbuf,
	(getwritebufferproc)memory_getbuf,
	(getsegcountproc)memory_getsegcount,
	(getcharbufferproc)memory_getbuf,
};

static PyObject *
memory_copy(MemoryObject *self, PyObject *args)
{
	MemoryObject *other;

	if (!PyArg_ParseTuple(args, ":copy")) return NULL;

	other = (MemoryObject *) _new_memory(self->size);
	if (!other) return NULL;

	memcpy(other->ptr, self->ptr, self->size);

	return (PyObject *) other;
}

static PyObject *
memory_clear(MemoryObject *self, PyObject *args)
{
	if (!PyArg_ParseTuple(args, ":clear")) return NULL;
	memset(self->ptr, 0, self->size);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
memory_tolist(MemoryObject *self, PyObject  *args)
{
	PyObject *l;
	int i;
	if (!PyArg_ParseTuple(args, ":tolist")) return NULL;
	l = PyList_New(self->size);
	if (!l) return NULL;
	for(i=0; i<self->size; i++) {
		PyObject *o = PyInt_FromLong(((unsigned char *)self->ptr)[i]);
		if (!o) { 
			Py_DECREF(l);
			return NULL;
		}
		if (PyList_SetItem(l, i, o) < 0) {
			Py_DECREF(l);
			return NULL;
		}
	}
	return l;
}

static PyMethodDef memory_methods[] = {
    {"__reduce__", (PyCFunction) memory_reduce, METH_VARARGS,
     "Reduces a memory buffer to (memory.memory_from_string, data_string)."},
    {"copy", (PyCFunction) memory_copy, METH_VARARGS,
     "Returns a copy of the memory buffer"},
    {"clear", (PyCFunction) memory_clear, METH_VARARGS,
     "Sets the contents of a buffer to 0"},
    {"tolist", (PyCFunction) memory_tolist, METH_VARARGS,
     "Returns a list of unsigned char values."},
    { NULL, NULL }   /* sentinel */
};

static PyObject *
memory_getattr(PyObject *obj, char *name)
{
    return Py_FindMethod(memory_methods, (PyObject *)obj, name);
}

static PyTypeObject MemoryType = {
    PyObject_HEAD_INIT(NULL)
    0,
    "numarray.memory.Memory",
    sizeof(MemoryObject),
    1,                           /* per item cost */
    memory_dealloc,              /* tp_dealloc */
    0,                           /* tp_print */
    memory_getattr,              /* tp_getattr */
    0,                           /* tp_setattr */
    0,                           /* tp_compare */
    memory_repr,                 /* tp_repr */
    0,                           /* tp_as_number */
    &memory_as_sequence,         /* tp_as_sequence */
    0,                           /* tp_as_mapping */
    0,                           /* tp_hash */
    0,                           /* tp_call */
    memory_str,                  /* tp_str */
    0,                           /* tp_getattro */
    0,                           /* tp_setattro */
    &memory_as_buffer,           /* tp_as_buffer */
    0,                           /* tp_flags */
    "allocates memory for use by numarray.",           /* tp_doc */
    0,    			 /* tp_traverse */
    0,				 /* tp_clear */
    0				 /* tp_richcompare */
};

static PyMethodDef module_methods[] = {
    {"new_memory", new_memory, METH_VARARGS,
     "Create a new Memory object."},
    {"memory_buffer", memory_buffer, METH_VARARGS,
     "Create a new buffer object based on a Memory object."},
    {"writeable_buffer", writeable_buffer, METH_VARARGS,
     "Create a writeable buffer object referencing another python object"},
    {"memory_from_string", memory_from_string, METH_VARARGS,
     "Factory function to restore a memory object from a string."},
    {"memory_reduce", memory_reduce_func, METH_VARARGS,
     "Function to convert memory into unpickling reduction tuple."},
    {NULL, NULL}    /* sentinel */
};

DL_EXPORT(void)
initmemory(void) 
{
	PyObject *d, *m;
	MemoryType.ob_type = &PyType_Type;
	m = Py_InitModule("memory", module_methods);
	d = PyModule_GetDict(m);
	memoryError = PyErr_NewException("numarray.memory.error", NULL, NULL);
	PyDict_SetItemString(d, "error", memoryError);
	PyDict_SetItemString(d, "MemoryType", (PyObject *) &MemoryType);
	ADD_VERSION(m);
}
