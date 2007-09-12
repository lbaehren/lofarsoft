#include <Python.h>
#include <stdio.h>
#include <math.h>
#include <signal.h>

#include "arrayobject.h"
#include "libnumarray.h"
#include "structmember.h"

#define _PROTOTYPE 0

#if _PROTOTYPE
   #define ARRAY_COMPARE 0
   #define ARRAY_RICHCOMPARE 0
   #define ARRAY_AS_NUMBER 0

#else
   #define ARRAY_COMPARE array_compare
   #define ARRAY_RICHCOMPARE array_richcompare
   #define ARRAY_AS_NUMBER &array_as_number
#endif

/* Compile _numarray type for Pythons >= 2.2 */
char *_numarray__doc__ = 
"_numarray is a module which supplies C-helpers for numarray, including\n"
"the _numarray baseclass of numarray for Python version >= 2.2.  \n"
"Class _numarray is used to accelerate selected methods of _numarray by \n"
"handling simple cases directly in C.\n";

#define DEFERRED_ADDRESS(ADDR) 0

staticforward PyTypeObject _numarray_type;


static int
deferred_numarray_init(void);

static int
PyArray_SetNumericOps(PyObject *dict);

static PyObject *
_copyFrom(PyObject *self, PyObject *arr0);


static int
_numarray_init(PyArrayObject *self, PyObject *args, PyObject *kwds)
{
        static char *kwlist[] = {"shape", "type", "buffer", "byteoffset", 
				 "bytestride", "byteorder", "aligned", 
				 "real", "imag", NULL};
	PyObject *shape = NULL;
	PyObject *type  = NULL;
	PyObject *buffer = Py_None;
	int       byteoffset = 0;
	PyObject *bytestride = Py_None;
	char     *byteorder = NULL;
	int       aligned = 1;
	PyObject *real = Py_None, *imag=Py_None;

	int       typeno;
	PyObject *args2;

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OOOiOsiOO", kwlist, 
					 &shape, &type, &buffer, &byteoffset, 
					 &bytestride, &byteorder, &aligned,
					 &real, &imag))
		return -1;

	if (deferred_numarray_init() < 0)
		return -1;

	if (type) {
		type = NA_getType(type);
		if (!type) return -1;
		if ((typeno = NA_typeObjectToTypeNo( type )) < 0) {
			PyErr_Format(PyExc_RuntimeError, 
				     "_numarray_init: can't get typeno for type");
			return -1;
		}
		Py_DECREF(type);
	} else {
		typeno = tAny;
	}
	
	if (!(self->descr = NA_DescrFromType( typeno ))) {
		PyErr_Format(PyExc_RuntimeError, 
			     "_numarray_init: bad type number");
		return -1;
	}
	
	if (byteorder) {
		if (!strcmp(byteorder, "little"))
			self->byteorder = NUM_LITTLE_ENDIAN;
		else if (!strcmp(byteorder, "big"))
			self->byteorder = NUM_BIG_ENDIAN;
		else {
			PyErr_Format(PyExc_ValueError, 
				     "_numarray_init: byteorder must be 'little' or 'big'");
			return -1;
		}
	} else
		self->byteorder = NA_ByteOrder();
	NA_updateByteswap(self);
	

	args2 = Py_BuildValue("OiOiOi", shape, self->descr->elsize, buffer,
			      byteoffset, bytestride, aligned);
	if (!args2) return -1;

	if (_numarray_type.tp_base->tp_init((PyObject *)self, args2, NULL) < 0)
		return -1;

	Py_DECREF(args2);
	
	self->_shadows = NULL;

	/* Since the follow attrs are defined in Python, _numarray is abstract */
	if (real != Py_None) {
		if (PyObject_SetAttrString(
			    (PyObject *) self, "real", real) < 0)
			return -1;
	}
	if (imag != Py_None) {
		if (PyObject_SetAttrString(
			    (PyObject *) self, "imag", imag) < 0)
			return -1;
	}

	return 0;
}


static PyObject *
_numarray_type_get(PyArrayObject *self)
{
	return NA_typeNoToTypeObject(self->descr->type_num);
}

static int
_numarray_type_set(PyArrayObject *self, PyObject *s)
{
	PyObject *item;
	long ntype;
	if (!s) { 
		PyErr_Format(PyExc_RuntimeError, "can't delete _type"); 
		return -1; 
	}
	if (!(item = PyObject_GetAttrString(s, "name"))) 
		return -1;
	if (!PyString_Check(item)) {
		PyErr_Format(PyExc_TypeError, "type name is not a string");
		return -1;
	}	       
	ntype = NA_nameToTypeNo( PyString_AsString(item) );
	if (ntype < 0) {
		PyErr_Format(PyExc_ValueError, "_numarray_type_set: unknown type:'%s'",
			     PyString_AsString(item));
		return -1;
	}
	Py_DECREF(item);
	self->descr = NA_DescrFromType( ntype );
	return 0;
}

static PyObject *
_numarray_scipy_typestr_get(PyArrayObject *self)
{
	char typestr[5];
	if (NA_scipy_typestr(self->descr->type_num, self->byteorder, typestr) < 0)
		return PyErr_Format( 
			PyExc_TypeError,
			"__array_typstr__ not implemented for type.");
	return PyString_FromString(typestr);
}

static void
_free_cobj_array_struct(void *arrayifptr, void *arr)
{
  PyArrayInterface* arrayif;

  arrayif = (PyArrayInterface*)arrayifptr;
  Py_DECREF((PyObject*)arr);
  PyMem_Free(arrayif->shape);
  PyMem_Free(arrayif->strides);

  PyMem_Free(arrayif);
}

static PyObject *
_numarray_scipy_array_struct_get(PyArrayObject *self)
{
	char typestr[5];
	PyArrayInterface *arrayif = PyMem_Malloc(sizeof(PyArrayInterface));
	int i;
	if (!arrayif) return NULL;

	arrayif->version = 2;
	arrayif->nd = self->nd;

	/* We have to allocate memory because self->dimensions is of
	   type maybelong* while arrayif->shape is of type
	   Py_intptr_t*. The sizes of these types are not equal on
	   some platforms. */

	arrayif->shape = PyMem_Malloc(sizeof(Py_intptr_t)*self->nd);
	if (!arrayif->shape) return NULL;
	arrayif->strides = PyMem_Malloc(sizeof(Py_intptr_t)*self->nd);
	if (!arrayif->strides) return NULL;
	
	for (i=0;i<self->nd;++i) {
	  arrayif->shape[i] = self->dimensions[i];
	  arrayif->strides[i] = self->strides[i];
	}

	arrayif->itemsize = self->itemsize;

	if (NA_scipy_typestr( self->descr->type_num,
			      self->byteorder,
			      typestr) < 0)
		return NULL;

	arrayif->typekind = typestr[1];

	NA_updateStatus(self);
	arrayif->flags = self->flags;
	arrayif->flags |= (self->flags & FORTRAN_CONTIGUOUS) ? FORTRAN : 0;

	NA_updateDataPtr(self);
	arrayif->data = (void *) self->data;

	Py_INCREF(self); /* ensure view of array is valid */
	return PyCObject_FromVoidPtrAndDesc((void *)arrayif, (void*)self, _free_cobj_array_struct);	
}

static PyObject *
_numarray_byteorder_get(PyArrayObject *self)
{
	if (self->byteorder)
		return PyString_FromString("big");
	else
		return PyString_FromString("little");
}

static int
_numarray_byteorder_set(PyArrayObject *self, PyObject *s)
{
	char *order;
	if (!s) { PyErr_Format(PyExc_RuntimeError, "can't delete _byteorder"); return -1; }
	if (!PyString_Check(s)) {
		PyErr_Format(PyExc_TypeError, 
			     "_numarray_byteorder_set: must be 'little' or 'big'");
		return -1;
	}
	order = PyString_AsString(s);
	if (!strcmp(order, "big"))
		self->byteorder = NUM_BIG_ENDIAN;
	else if (!strcmp(order, "little"))
		self->byteorder = NUM_LITTLE_ENDIAN;
	else {
		PyErr_Format(PyExc_ValueError, 
			     "_numarray_byteorder_set: only accepts 'little' or 'big'");
		return -1;
	}
	NA_updateByteswap(self); 
	return 0;
}

static PyObject *
_numarray_check_overflow_get(PyArrayObject *self)
{
	return PyInt_FromLong((self->flags & CHECKOVERFLOW) != 0);
}

static int
_numarray_check_overflow_set(PyArrayObject *self, PyObject *s)
{
	if (!s) { 
		PyErr_Format(
			PyExc_RuntimeError, "can't delete _check_overflow"); 
		return -1; 
	}
	if  (!PyInt_Check(s)) {
		PyErr_Format(
			PyExc_TypeError, "_check_overflow must be an integer.");
		return -1;
	}
	if (PyInt_AsLong(s)) {
		self->flags |= CHECKOVERFLOW;
	} else {
		self->flags &= ~CHECKOVERFLOW;
	}
	return 0;
}

static PyGetSetDef _numarray_getsets[] = {
 	{"_type", 
	 (getter)_numarray_type_get, 
	 (setter) _numarray_type_set, "numeric type object"}, 
 	{"__array_typestr__", 
	 (getter)_numarray_scipy_typestr_get, 
	 (setter)0, "type specifier (scipy newcore array interface)"}, 
 	{"__array_struct__", 
	 (getter)_numarray_scipy_array_struct_get, 
	 (setter)0, "fast C-based array descriptor (scipy newcore array interface)"}, 
	{"_byteorder", 
	 (getter)_numarray_byteorder_get, 
	 (setter) _numarray_byteorder_set, "byteorder/endian-ness of array, 'big' or 'little'"}, 
	{"_check_overflow", 
	 (getter)_numarray_check_overflow_get, 
	 (setter) _numarray_check_overflow_set, "byteorder/endian-ness of array, 'big' or 'little'"}, 
	{0}
};

static PyObject *
_numarray_isbyteswapped(PyArrayObject *self, PyObject *args)
{
	NA_updateByteswap(self);
	return PyInt_FromLong((self->flags & NOTSWAPPED) == 0);
}

static PyObject *
fromlist(PyObject *self, PyObject *args)
{
	PyObject *seq;

	if (!PyArg_ParseTuple(args, "O:fromlist", &seq))
		return NULL;

	return NA_setArrayFromSequence((PyArrayObject *)self, seq);
}

static PyObject *p_copyFromAndConvert;
static PyObject *p_copyBytes[16];
static PyObject *p_copyNbytes;

static PyObject *
_getCopyByte(int n)
{
	char name[80];
	PyObject *dict, *function;
	if (n <= ELEM(p_copyBytes))
		sprintf(name, "copy%dbytes", n);
	else
		sprintf(name, "copyNbytes");
	dict = NA_initModuleGlobal("numarray._bytes", "functionDict");
	if (!dict) return NULL;
	function = PyDict_GetItemString(dict, name);
	Py_DECREF(dict);
	Py_INCREF(function);
	return function;
}
		
static int initialized = 0;

static int
deferred_numarray_init(void)
{
	int i;
	PyObject *nm, *ufunc_dict;
		
	if (initialized) return 0;

	p_copyFromAndConvert = 
		NA_initModuleGlobal("numarray.ufunc", "_copyFromAndConvert");
	if (!p_copyFromAndConvert) return -1;
	
	p_copyNbytes = _getCopyByte(ELEM(p_copyBytes)+1);
	if (!p_copyNbytes) return -1;
	
	for(i=0; i<ELEM(p_copyBytes); i++) {
		p_copyBytes[i] = p_copyNbytes;
		Py_INCREF(p_copyNbytes);
	}

	for(i=1; i<=ELEM(p_copyBytes)+1; i*=2) {
		Py_DECREF(p_copyBytes[i-1]);
		p_copyBytes[i-1] = _getCopyByte(i);
		if (!p_copyBytes[i-1]) return -1;
	}

	if (!(nm = PyImport_ImportModule("numarray.ufunc")))  /* NEW */ {
		PyErr_Format(
			PyExc_ImportError,
			"_numarray: can't import ufunc module.");
		return -1;
	}
	ufunc_dict = PyModule_GetDict(nm);

	if (PyArray_SetNumericOps(ufunc_dict) < 0)
		return 0;

	initialized = 1;
	return 0;
}

static int
_noZeros(int n, maybelong *array)
{
	int i;
	for(i=0; i<n; i++)
		if (!array[i]) return 0;
	return 1;
}

static PyObject *
_copyFrom(PyObject *self, PyObject *arr0)
{
	PyObject *arr, *barr, *result;
	PyArrayObject *selfa = (PyArrayObject *) self;
	PyArrayObject *arra;

	if (deferred_numarray_init() < 0)
		return NULL;

	arra = NA_InputArray(arr0, tAny, 0);
	if (!arra) return NULL;
	arr = (PyObject *) arra;

	if (NA_NumArrayCheck(arr)) {
		if (!NA_elements(selfa) && !NA_elements(arra)) {
			Py_INCREF(Py_None);
			return Py_None;
		}
		if ((selfa->descr->type_num == arra->descr->type_num) &&
		    NA_ShapeEqual(selfa, arra) &&
		    (selfa->byteorder == arra->byteorder) &&
		    PyArray_ISALIGNED(self) &&
		    PyArray_ISALIGNED(arr) &&
		    _noZeros(arra->nstrides, arra->strides))
		{
			PyObject *cfunc;
			if (selfa->itemsize <= ELEM(p_copyBytes))
				cfunc = p_copyBytes[ selfa->itemsize - 1 ];
			else
				cfunc = p_copyNbytes;
			result = NA_callStrideConvCFuncCore(
				cfunc, selfa->nd, selfa->dimensions,
				arra->_data, arra->byteoffset, arra->nstrides, arra->strides,
				selfa->_data, selfa->byteoffset, selfa->nstrides, selfa->strides,
				selfa->itemsize);
			Py_DECREF(arr);
			return result;
		}
	} 
	barr = PyObject_CallMethod(self, "_broadcast", "(O)", arr);
	Py_DECREF(arr);
	if (barr == Py_None) {
		Py_DECREF(barr);
		return PyErr_Format(
			PyExc_ValueError, "array sizes must be consistent.");
				    
	}
	if (!barr) return NULL;
	result = PyObject_CallFunction(
		p_copyFromAndConvert, "(OO)", barr, self);
	if (!result) return NULL;
	Py_DECREF(barr);
	return result;
}

static PyObject *
_Py_copyFrom(PyObject *self, PyObject *args)
{
	PyObject *a;

	if (!PyArg_ParseTuple(args, "O:_copyFrom", &a))
		return NULL;
	return _copyFrom(self, a);
}

static PyObject *
_numarray_getitem(PyObject *self, PyObject *args)
{
	PyArrayObject *me = (PyArrayObject *) self;
	long offset;
	if (!PyArg_ParseTuple(args, "l:_getitem", &offset))
		return NULL;
	if (!NA_updateDataPtr(me)) 
		return NULL;
	return NA_getPythonScalar(me, offset-me->byteoffset);
}

static PyObject *
_numarray_setitem(PyObject *self, PyObject *args)
{
	PyArrayObject *me = (PyArrayObject *) self;
	PyObject *value;
	long offset;
	if (!PyArg_ParseTuple(args, "lO:_setitem", &offset, &value))
		return NULL;
	if (!NA_updateDataPtr(me)) 
		return NULL;
	if (NA_setFromPythonScalar(me, offset-me->byteoffset, value) < 0)
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}


static PyMethodDef _numarray_methods[] = {
	{"isbyteswapped", (PyCFunction)_numarray_isbyteswapped, METH_VARARGS,
	 "isbyteswapped() -> 0 if data is in natural machine order, 1 otherwise."},
	{"_getitem", (PyCFunction) _numarray_getitem, METH_VARARGS, 
	 "_getitem(offset) -> scalar value at offset"},
	{"_setitem", (PyCFunction) _numarray_setitem, METH_VARARGS, 
	 "_setitem(offset, value) sets array at offset to scalar value"},
	{"fromlist", fromlist, METH_VARARGS,
	 "fromlist()   loads sequence into array."},
	{"_copyFrom", _Py_copyFrom, METH_VARARGS,
	 "_copyFrom(a)  copies 'a' onto self."},
	{NULL,	NULL},
};


/* -------------------------------------------------------------- */

/* 
   Code for the "number protocol" was copied from Numeric and modified to suit
   numarray.
*/

typedef struct {
	PyUfuncObject *add, 
		*subtract, 
		*multiply, 
		*divide, 
		*remainder, 
		*power, 
		*minus, 
		*abs, 
		*bitwise_not, 
		*lshift, 
		*rshift, 
		*bitwise_and, 
		*bitwise_xor,
		*bitwise_or,
		*less,     /* Added by Scott N. Gunyan */
		*less_equal,         /* for rich comparisons */
		*equal,
		*not_equal,
		*greater,
		*greater_equal,
		*floor_divide, /* Added by Bruce Sherwood */
		*true_divide;            /* for floor and true divide */
} NumericOps;

static NumericOps n_ops;

#define GET(op) if ((n_ops.op = (PyUfuncObject *)PyDict_GetItemString(dict, #op)) == 0) { \
		PyErr_Format(PyExc_RuntimeError, "numarray module init failed for ufunc: '%s'", #op); \
		return -1; \
        }

static int 
PyArray_SetNumericOps(PyObject *dict) {
	GET(add);
	GET(subtract);
	GET(multiply);
	GET(divide);
	GET(remainder);
	GET(power);
	GET(minus);
	GET(abs);
	GET(bitwise_not);
	GET(lshift);
	GET(rshift);
	GET(bitwise_and);
	GET(bitwise_or);
	GET(bitwise_xor);
	GET(less);         /* Added by Scott N. Gunyan */
	GET(less_equal);   /* for rich comparisons */
	GET(equal);
	GET(not_equal);
	GET(greater);
	GET(greater_equal);
	GET(floor_divide);  /* Added by Bruce Sherwood */
	GET(true_divide);   /* for floor and true divide */
	return 0;
}

static int
_uses_op_priority(PyObject *in1, PyObject *in2)
{
	int rval = 0;
	PyObject *prior2 = PyObject_GetAttrString(in2, "op_priority");
	if (prior2) {
		PyObject *prior1 = PyObject_GetAttrString(in1, "op_priority");
		if (!prior1) goto _clear;
		if (!PyFloat_Check(prior1) || 
		    !PyFloat_Check(prior2)) {
			PyErr_Format(PyExc_TypeError, 
				     "Non-integer op_priority.");
			return -1;
		}
		rval = PyFloat_AsDouble(prior1) < PyFloat_AsDouble(prior2);
		Py_DECREF(prior1);
		Py_DECREF(prior2);
		return rval;
	}
  _clear:
	PyErr_Clear();
	Py_XDECREF(prior2);
	return 0;
}

static PyObject *
PyUFunc_BinaryFunction(PyUfuncObject *s, PyArrayObject *in1, PyObject *in2, char *rmeth) 
{
	PyObject *ins[2];
	PyObject *outs[1]; 
	int uses;
	ins[0] = (PyObject *) in1;
	ins[1] = (PyObject *) in2;	
	outs[0] = Py_None;
	uses = _uses_op_priority((PyObject *)in1, in2);
	if (uses < 0) return NULL;
	if (uses) {
		if (rmeth)
			return PyObject_CallMethod(in2, rmeth, "(O)", in1);
			
	} 
	return s->call((PyObject *)s, 2, ins, 1, outs);
}

/*This method adds the augmented assignment*/
/*functionality that was made available in Python 2.0*/
static PyObject *
PyUFunc_InplaceBinaryFunction(PyUfuncObject *s, PyArrayObject *in1, PyObject *in2) 
{
	PyObject *result;
	PyObject *ins[2];
	PyObject *outs[1];
	ins[0] = (PyObject *) in1;
	ins[1] = (PyObject *) in2;
	outs[0] = (PyObject *) in1;
	result = s->call((PyObject *)s, 2, ins, 1, outs);
	if (result) {
		Py_DECREF(result);
		Py_INCREF(outs[0]);
		return outs[0];
	} else
		return NULL;
}

static PyObject *PyUFunc_UnaryFunction(PyUfuncObject *s, PyArrayObject *in1) 
{
	PyObject *outs[1];  outs[0] = Py_None;
	return s->call((PyObject *)s, 1, (PyObject **) &in1, 1, outs);
}

static PyObject *array_add(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_BinaryFunction(n_ops.add, m1, m2, "__radd__");
}
static PyObject *array_subtract(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_BinaryFunction(n_ops.subtract, m1, m2, "__rsub__");
}
static PyObject *array_multiply(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_BinaryFunction(n_ops.multiply, m1, m2, "__rmul__");
}
static PyObject *array_divide(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_BinaryFunction(n_ops.divide, m1, m2, "__rdiv__");
}
static PyObject *array_remainder(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_BinaryFunction(n_ops.remainder, m1, m2, "__rmod__");
}
static PyObject *array_power(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_BinaryFunction(n_ops.power, m1, m2, "__rpow__");
}
static PyObject *array_minus(PyArrayObject *m1) { 
	deferred_numarray_init();
	return PyUFunc_UnaryFunction(n_ops.minus, m1);
}
static PyObject *array_abs(PyArrayObject *m1) { 
	deferred_numarray_init();
	return PyUFunc_UnaryFunction(n_ops.abs, m1);
}
static PyObject *array_bitwise_not(PyArrayObject *m1) { 
	deferred_numarray_init();
	return PyUFunc_UnaryFunction(n_ops.bitwise_not, m1);
}
static PyObject *array_lshift(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_BinaryFunction(n_ops.lshift, m1, m2, "__rlshift__");
}
static PyObject *array_rshift(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_BinaryFunction(n_ops.rshift, m1, m2, "__rrshift__");
}
static PyObject *array_bitwise_and(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_BinaryFunction(n_ops.bitwise_and, m1, m2, "__rand__");
}
static PyObject *array_bitwise_or(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_BinaryFunction(n_ops.bitwise_or, m1, m2, "__ror__");
}
static PyObject *array_bitwise_xor(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_BinaryFunction(n_ops.bitwise_xor, m1, m2, "__rxor__");
}

/*These methods add the augmented assignment*/
/*functionality that was made available in Python 2.0*/
static PyObject *array_inplace_add(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_InplaceBinaryFunction(n_ops.add, m1, m2);
}
static PyObject *array_inplace_subtract(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_InplaceBinaryFunction(n_ops.subtract, m1, m2);
}
static PyObject *array_inplace_multiply(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_InplaceBinaryFunction(n_ops.multiply, m1, m2);
}
static PyObject *array_inplace_divide(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_InplaceBinaryFunction(n_ops.divide, m1, m2);
}
static PyObject *array_inplace_remainder(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_InplaceBinaryFunction(n_ops.remainder, m1, m2);
}
static PyObject *array_inplace_power(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_InplaceBinaryFunction(n_ops.power, m1, m2);
}
static PyObject *array_inplace_lshift(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_InplaceBinaryFunction(n_ops.lshift, m1, m2);
}
static PyObject *array_inplace_rshift(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_InplaceBinaryFunction(n_ops.rshift, m1, m2);
}
static PyObject *array_inplace_bitwise_and(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_InplaceBinaryFunction(n_ops.bitwise_and, m1, m2);
}
static PyObject *array_inplace_bitwise_or(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_InplaceBinaryFunction(n_ops.bitwise_or, m1, m2);
}
static PyObject *array_inplace_bitwise_xor(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_InplaceBinaryFunction(n_ops.bitwise_xor, m1, m2);
}

/*Added by Bruce Sherwood Dec 2001*/
/*These methods add the floor and true division*/
/*functionality that was made available in Python 2.2*/
static PyObject *array_floor_divide(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_BinaryFunction(n_ops.floor_divide, m1, m2, "__rfloordiv__");
}
static PyObject *array_true_divide(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_BinaryFunction(n_ops.true_divide, m1, m2, "__rtruediv__");
}
static PyObject *array_inplace_floor_divide(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_InplaceBinaryFunction(n_ops.floor_divide, m1, m2);
}
static PyObject *array_inplace_true_divide(PyArrayObject *m1, PyObject *m2) {
	deferred_numarray_init();
	return PyUFunc_InplaceBinaryFunction(n_ops.true_divide, m1, m2);
}
/*End of methods added by Bruce Sherwood*/

static PyObject *array_divmod(PyArrayObject *op1, PyObject *op2) {
	PyObject *divp, *modp, *result;

	divp = array_divide(op1, op2);
	if (divp == NULL) return NULL;
	modp = array_remainder(op1, op2);
	if (modp == NULL) {
		Py_DECREF(divp);
		return NULL;
	}
	result = Py_BuildValue("OO", divp, modp);
	Py_DECREF(divp);
	Py_DECREF(modp);
	return result;
}


/* methods added by Travis Oliphant, Jan 2000 */
PyObject *array_int(PyArrayObject *v) {        
	PyObject *pv, *pv2;
	if (PyArray_SIZE(v) != 1) {
		PyErr_SetString(PyExc_TypeError, "only length-1 arrays can be converted to Python scalars.");
		return NULL;
	}
	pv = v->descr->_get(v, 0);
	if (pv == NULL) return NULL;
	if (pv->ob_type->tp_as_number == 0) {
		PyErr_SetString(PyExc_TypeError, "cannot convert to an int, scalar object is not a number.");
		Py_DECREF(pv);
		return NULL;
	}
	if (pv->ob_type->tp_as_number->nb_int == 0) {
		PyErr_SetString(PyExc_TypeError, "don't know how to convert scalar number to int");
		Py_DECREF(pv);
		return NULL;
	}
	
	pv2 = pv->ob_type->tp_as_number->nb_int(pv);
	Py_DECREF(pv);
	return pv2;        
}

static PyObject *array_float(PyArrayObject *v) {
	PyObject *pv, *pv2;
	if (PyArray_SIZE(v) != 1) {
		PyErr_SetString(PyExc_TypeError, "only length-1 arrays can be converted to Python scalars.");
		return NULL;
	}
	pv = v->descr->_get(v, 0);
	if (pv == NULL) return NULL;
	if (pv->ob_type->tp_as_number == 0) {
		PyErr_SetString(PyExc_TypeError, "cannot convert to an int, scalar object is not a number.");
		Py_DECREF(pv);
		return NULL;
	}
	if (pv->ob_type->tp_as_number->nb_float == 0) {
		PyErr_SetString(PyExc_TypeError, "don't know how to convert scalar number to float");
		Py_DECREF(pv);
		return NULL;
	}
	pv2 = pv->ob_type->tp_as_number->nb_float(pv);
	Py_DECREF(pv);
	return pv2;        
}

static PyObject *array_long(PyArrayObject *v) {        
	PyObject *pv, *pv2;
	if (PyArray_SIZE(v) != 1) {
		PyErr_SetString(PyExc_TypeError, "only length-1 arrays can be converted to Python scalars.");
		return NULL;
	}
	pv = v->descr->_get(v, 0);
	if (pv->ob_type->tp_as_number == 0) {
		PyErr_SetString(PyExc_TypeError, "cannot convert to an int, scalar object is not a number.");
		return NULL;
	}
	if (pv->ob_type->tp_as_number->nb_long == 0) {
		PyErr_SetString(PyExc_TypeError, "don't know how to convert scalar number to long");
		return NULL;
	}
	pv2 = pv->ob_type->tp_as_number->nb_long(pv);
	Py_DECREF(pv);
	return pv2;        
}

static PyObject *array_oct(PyArrayObject *v) {        
	PyObject *pv, *pv2;
	if (PyArray_SIZE(v) != 1) {
		PyErr_SetString(PyExc_TypeError, "only length-1 arrays can be converted to Python scalars.");
		return NULL;
	}
	pv = v->descr->_get(v, 0);
	if (pv->ob_type->tp_as_number == 0) {
		PyErr_SetString(PyExc_TypeError, "cannot convert to an int, scalar object is not a number.");
		return NULL;
	}
	if (pv->ob_type->tp_as_number->nb_oct == 0) {
		PyErr_SetString(PyExc_TypeError, "don't know how to convert scalar number to oct");
		return NULL;
	}
	pv2 = pv->ob_type->tp_as_number->nb_oct(pv);
	Py_DECREF(pv);
	return pv2;        
}

static PyObject *array_hex(PyArrayObject *v) {        
	PyObject *pv, *pv2;
	if (PyArray_SIZE(v) != 1) {
		PyErr_SetString(PyExc_TypeError, "only length-1 arrays can be converted to Python scalars.");
		return NULL;
	}
	pv = v->descr->_get(v, 0);
	if (pv->ob_type->tp_as_number == 0) {
		PyErr_SetString(PyExc_TypeError, "cannot convert to an int, scalar object is not a number.");
		return NULL;
	}
	if (pv->ob_type->tp_as_number->nb_hex == 0) {
		PyErr_SetString(PyExc_TypeError, "don't know how to convert scalar number to hex");
		return NULL;
	}
	pv2 = pv->ob_type->tp_as_number->nb_hex(pv);
	Py_DECREF(pv);
	return pv2;        
}

/* end of methods added by Travis Oliphant */

static PyArrayObject *
array_copy(PyArrayObject *copied)
{
	return NA_copy(copied);
}

/* Array evaluates as "true" if any of the elements are non-zero */
static int 
array_nonzero(PyArrayObject *mp) 
{
	PyErr_Format(PyExc_RuntimeError,
		     "An array doesn't make sense as a truth value.  "
		     "Use any(a) or all(a).");
	return -1;
}

static PyNumberMethods array_as_number = {
	(binaryfunc)array_add,                  /*nb_add*/
	(binaryfunc)array_subtract,             /*nb_subtract*/
	(binaryfunc)array_multiply,             /*nb_multiply*/
	(binaryfunc)array_divide,               /*nb_divide*/
	(binaryfunc)array_remainder,            /*nb_remainder*/
	(binaryfunc)array_divmod,               /*nb_divmod*/
	(ternaryfunc)array_power,               /*nb_power*/
	(unaryfunc)array_minus,  
	(unaryfunc)array_copy,                  /*nb_pos*/ 
	(unaryfunc)array_abs,                   /*(unaryfunc)array_abs,*/
	(inquiry)array_nonzero,                 /*nb_nonzero*/
	(unaryfunc)array_bitwise_not,           /*nb_invert*/
	(binaryfunc)array_lshift,               /*nb_lshift*/
	(binaryfunc)array_rshift,               /*nb_rshift*/
	(binaryfunc)array_bitwise_and,          /*nb_and*/
	(binaryfunc)array_bitwise_xor,          /*nb_xor*/
	(binaryfunc)array_bitwise_or,           /*nb_or*/
	(coercion) 0,                           /*nb_coerce*/
	(unaryfunc)array_int,                   /*nb_int*/
	(unaryfunc)array_long,                  /*nb_long*/
	(unaryfunc)array_float,                 /*nb_float*/
	(unaryfunc)array_oct,	                /*nb_oct*/
	(unaryfunc)array_hex,	                /*nb_hex*/

	/*This code adds augmented assignment functionality*/
	/*that was made available in Python 2.0*/
	(binaryfunc)array_inplace_add,          /*inplace_add*/
	(binaryfunc)array_inplace_subtract,     /*inplace_subtract*/
	(binaryfunc)array_inplace_multiply,     /*inplace_multiply*/
	(binaryfunc)array_inplace_divide,       /*inplace_divide*/
	(binaryfunc)array_inplace_remainder,    /*inplace_remainder*/
	(ternaryfunc)array_inplace_power,       /*inplace_power*/
	(binaryfunc)array_inplace_lshift,       /*inplace_lshift*/
	(binaryfunc)array_inplace_rshift,       /*inplace_rshift*/
	(binaryfunc)array_inplace_bitwise_and,  /*inplace_and*/
	(binaryfunc)array_inplace_bitwise_xor,  /*inplace_xor*/
	(binaryfunc)array_inplace_bitwise_or,   /*inplace_or*/

        /* Added in release 2.2 */
	/* The following require the Py_TPFLAGS_HAVE_CLASS flag */
#if PY_VERSION_HEX >= 0x02020000
	(binaryfunc)array_floor_divide,          /*nb_floor_divide*/
	(binaryfunc)array_true_divide,           /*nb_true_divide*/
	(binaryfunc)array_inplace_floor_divide,  /*nb_inplace_floor_divide*/
	(binaryfunc)array_inplace_true_divide,   /*nb_inplace_true_divide*/
#endif
};

/* Changed by Travis Oliphant Jan, 2000 to allow comparison of scalars.*/
static int 
array_compare(PyArrayObject *self, PyObject *other) 
{
	PyArrayObject *aother;
	PyObject *o1, *o2;
	int val, result;
	aother = (PyArrayObject *)other;
	if (self->nd != 0 || aother->nd != 0) {
		PyErr_SetString(PyExc_TypeError,
				"Comparison of multiarray objects other than rank-0 arrays is not implemented.");
		return -1;
	}
	o1 = self->descr->_get(self, 0);
	o2 = aother->descr->_get(aother, 0);
	if (o1 == NULL || o2 == NULL) return -1;
	val = PyObject_Cmp(o1,o2,&result);
	Py_DECREF(o1);
	Py_DECREF(o2);
	if (val < 0) {
		PyErr_SetString(PyExc_TypeError, "objects cannot be compared.");
		return -1;
	}
	return result;
}

/* Added by Scott N. Gunyan  Feb, 2001
   This will implement the new Rich Comparisons added in Python 2.1
   to allow returning an array object */

static PyObject *
array_richcompare(PyArrayObject *self, PyObject *other, int cmp_op) 
{
	deferred_numarray_init();
	switch(cmp_op) 
	{
	case Py_LT: case Py_LE: case Py_GT: case Py_GE:
		if ((self->descr->type_num == tComplex32) || 
		    (self->descr->type_num == tComplex64))
			return PyErr_Format(
				PyExc_TypeError,
				"Complex NumArrays don't support >, >=, <, <= operators");
		break;
	case Py_EQ:
		if (other == Py_None) 
			return PyInt_FromLong(0);
		break;
	case Py_NE:
		if (other == Py_None)
			return PyInt_FromLong(1);
		break;
	}
	switch (cmp_op) 
        {
        case Py_LT:
		return PyUFunc_BinaryFunction(n_ops.less, self, other, NULL);
        case Py_LE:
		return PyUFunc_BinaryFunction(n_ops.less_equal, self, other, NULL);
        case Py_GT:
		return PyUFunc_BinaryFunction(n_ops.greater, self, other, NULL);
        case Py_GE:
		return PyUFunc_BinaryFunction(n_ops.greater_equal, self, other, NULL);
        case Py_EQ:
		return PyUFunc_BinaryFunction(n_ops.equal, self, other, NULL);
        case Py_NE:
		return PyUFunc_BinaryFunction(n_ops.not_equal, self, other, NULL);
        }
	return NULL;
}


static char _numarray_tpdoc[]  = 
"_numarray is a private baseclass of NumArray and implements some features\n"
"of NumArray in C for better performance.\n"
"\n"
"_numarray(shape=None, type=None, buffer=None, byteoffset=0, bytestride=None,\n"
"          byteorder=sys.byteorder, aligned=1, real=None, imag=None)\n"
"\n"
"    shape      The shape of the resulting array.\n"
"\n"
"    type       The type of each data element, e.g. Int32.\n"
"\n"
"    buffer     An object which supports the Python buffer protocol in C.\n"
"\n"
"    byteoffset Offset in bytes from the start of 'buffer' to the array data.\n""\n"
"    bytestride Distance in bytes between single elements.\n"
"\n"
"    byteorder  The actual ordering of bytes in buffer: 'big' or 'little'.\n"
"\n"
"    aligned    Flag Declaring that the array data is aligned, or not.\n"
"\n"
"    real       Initializer for real component of complex arrays.\n"
"\n"
"    imag       Initializer for imaginary component of complex arrays.\n";

static PyTypeObject _numarray_type = {
	PyObject_HEAD_INIT(DEFERRED_ADDRESS(&PyType_Type))
	0,
	"numarray._numarray._numarray",
	sizeof(PyArrayObject),
        0,
	0,                  			/* tp_dealloc */
	0,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	(cmpfunc) ARRAY_COMPARE,		/* tp_compare */
	0,					/* tp_repr */
	ARRAY_AS_NUMBER,	        	/* tp_as_number */
        0,                                      /* tp_as_sequence */
	0,                       		/* tp_as_mapping */
	0,					/* tp_hash */
	0,					/* tp_call */
	0,					/* tp_str */
	0,					/* tp_getattro */
	0,					/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT |
#if !_PROTOTYPE
	Py_TPFLAGS_CHECKTYPES |
#endif
	Py_TPFLAGS_BASETYPE,                    /* tp_flags */
	_numarray_tpdoc,			/* tp_doc */
	0,					/* tp_traverse */
	0,					/* tp_clear */
	(richcmpfunc) ARRAY_RICHCOMPARE,	/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	_numarray_methods,			/* tp_methods */
	0,					/* tp_members */
	_numarray_getsets,			/* tp_getset */
	DEFERRED_ADDRESS(&_ndarray_type),       /* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	(initproc)_numarray_init,		/* tp_init */
	0,					/* tp_alloc */
	0,      				/* tp_new */
};

typedef void Sigfunc(int);

static PyObject *
_maxtype(PyObject *module, PyObject *args)
{
	long maxtype;
	PyObject *seq;
	if (!PyArg_ParseTuple(args, "O", &seq))
		return NULL;
	maxtype = NA_maxType(seq);
	if (maxtype < 0) 
		return NULL;
	else
		return PyInt_FromLong(maxtype);
}


#define IP(type, sumtype)						\
	static void							\
	_ip##type(type *a, type *b, type *r,				\
		  maybelong imax, maybelong jmax, maybelong kmax)	\
	{								\
		maybelong   i, j, k;					\
									\
		for(i=0; i<imax; i++)					\
			for(j=0; j<jmax; j++, r++)			\
			{						\
				sumtype s=0;				\
				type *ap = a + i*kmax;			\
				type *bp = b + j*kmax;			\
				for(k=0; k<kmax; k++, ap++, bp++) {	\
					s += ((sumtype) *ap) * ((sumtype) *bp);	\
				}					\
				*r = s;					\
			}						\
	}

#define CIP(type, sumtype)						\
	static void							\
	_ip##type(type *a, type *b, type *r,				\
		  maybelong imax, maybelong jmax, maybelong kmax)	\
	{								\
		maybelong   i, j, k;					\
									\
		for(i=0; i<imax; i++)					\
			for(j=0; j<jmax; j++, r++)			\
			{						\
				sumtype s, t, sa, sb;			\
				type *ap = a + i*kmax;			\
				type *bp = b + j*kmax;			\
				s.r = 0; s.i=0;				\
				for(k=0; k<kmax; k++, ap++, bp++) {	\
					sa.r = ap->r; sa.i = ap->i;	\
					sb.r = bp->r; sb.i = bp->i;	\
					NUM_CMUL(sa, sb, t); NUM_CADD(s, t, s);	\
				}					\
				r->r = s.r;  r->i = s.i;		\
			}						\
	}

IP(Long, Long)
	IP(Float32, Float64)
	IP(Float64, Float64)
	CIP(Complex32, Complex64)
	CIP(Complex64, Complex64)

	static PyArrayObject *
_rank0_to_rank1(PyArrayObject *ao)
{
	PyArrayObject *bo;
	if (ao->nd != 0) {
		Py_INCREF(ao);
		return ao;
	}
	bo = NA_copy(ao);
	if (!bo) return NULL;

	bo->dimensions[0] = 1;
	bo->nd = 1;
	bo->strides[0] = bo->itemsize;
	return bo;
}

static PyObject *
_innerproduct(PyArrayObject *a, PyArrayObject *b, NumarrayType maxt,
	      char *kind)
{
	PyArrayObject *r;
	maybelong ae, be, i, imax, jmax, kmax, dots[2*MAXDIM];
	char *ap, *bp ,*rp;

	a = _rank0_to_rank1(a);
	b = _rank0_to_rank1(b);
	if (!a || !b) return NULL;

	/* Result shape =  a.shape[:-1] + b.shape[:-1] */
	for(i=0; i<a->nd-1; i++)
		dots[i] = a->dimensions[i];
	for(i=a->nd-1; i<b->nd+a->nd-2; i++)
		dots[i] = b->dimensions[i-a->nd+1];

	r = NA_vNewArray(NULL, maxt, a->nd + b->nd - 2, dots);
	if (!r)
	{
		Py_DECREF(a);
		Py_DECREF(b);
		return NULL;
	}

	ae = NA_elements(a);
	be = NA_elements(b);
	if (ae==0 || be==0)
		return (PyObject *) r;

	imax = ae / a->dimensions[ a->nd - 1];
	jmax = be / b->dimensions[ b->nd - 1];
	kmax = a->dimensions[ a->nd - 1 ];

	NA_clearFPErrors();

	ap = a->data;
	bp = b->data;
	rp = r->data;

	switch((int) maxt) 
	{
	case tLong: 
		_ipLong((Long *) ap, (Long *) bp, (Long *) rp,
			imax, jmax, kmax);     
		break;
	case tFloat32: 
		_ipFloat32((Float32 *) ap, (Float32 *) bp, (Float32 *) rp, 
			   imax, jmax, kmax);
		break;
	case tFloat64: 
		_ipFloat64((Float64 *) ap, (Float64 *) bp, (Float64 *) rp, 
			   imax, jmax, kmax);
		break;
	case tComplex32: 
		_ipComplex32((Complex32 *) ap, (Complex32 *) bp, 
			     (Complex32 *) rp, imax, jmax, kmax);  
		break;
	case tComplex64: 
		_ipComplex64((Complex64 *) ap, (Complex64 *) bp, 
			     (Complex64 *) rp, imax, jmax, kmax);  
		break;
	}

	Py_DECREF(a);
	Py_DECREF(b);

	if (NA_checkAndReportFPErrors(kind) < 0)
	{
		Py_DECREF(r);
		return NULL;
	}
	
	return PyArray_Return(r);
}

static NumarrayType 
_dot_type(PyObject *seq)
{
	if (NA_NumArrayCheck(seq)) {
		PyArrayObject *a  = (PyArrayObject *) seq;
		switch(a->descr->type_num) {
		case tFloat32: return tFloat32;
		case tFloat64: return tFloat64;
		case tComplex32: return tComplex32;
		case tComplex64: return tComplex64;
		default: return tLong;
		}
	} else {
		return NA_NumarrayType(seq);
	}
}

static PyObject *
innerproduct(PyObject *module, PyObject *args)
{
	PyObject *ao, *bo;
	PyArrayObject *a=NULL, *b=NULL;
	PyObject  *result=NULL;
	NumarrayType at, bt, maxt;

	if (!PyArg_ParseTuple(args, "OO:innerproduct", &ao, &bo))
		goto _done;
	
	if ((at = _dot_type(ao)) < 0)
		goto _done;

	if ((bt = _dot_type(bo)) < 0)
		goto _done;
	
	maxt = MAX(at, bt);

	if (maxt == tBool) maxt = tLong;
	
	a = NA_InputArray(ao, maxt, NUM_C_ARRAY);
	if (!a) goto _done;

	b = NA_InputArray(bo, maxt, NUM_C_ARRAY);
	if (!b) goto _done;

	if (a->dimensions[a->nd-1] != b->dimensions[b->nd-1])
	{
		PyErr_Format(
			PyExc_ValueError, 
			"innerproduct: last sequence dimensions must match.");
		goto _done;
	}

	result = _innerproduct(a,b,maxt,"innerproduct");

  _done:
	Py_XDECREF(a);
	Py_XDECREF(b);
	return result;
}

static PyObject *
dot(PyObject *module, PyObject *args)
{
	PyObject *ao, *bo;
	PyArrayObject *a=NULL, *b=NULL, *b_original=NULL, *b2=NULL;
	NumarrayType at, bt, maxt;
	PyObject *result=NULL;

	if (!PyArg_ParseTuple(args, "OO:dot", &ao, &bo))
		goto _done;

	if ((at = _dot_type(ao)) < 0)
		goto  _done;

	if ((bt = _dot_type(bo)) < 0)
		goto _done;

	maxt = MAX(at, bt);
	
	a = NA_InputArray(ao, maxt, NUM_C_ARRAY);
	if (!a) goto _done;

	b = NA_InputArray(bo, maxt, NUM_C_ARRAY);
	if (!b) goto _done;

	if (NA_swapAxes(b, -1, -2) < 0)
		goto _done;

	b_original = b;
	Py_INCREF(b_original);
	if (!PyArray_ISCARRAY(b)) {
		b2 = NA_copy(b);
		if (!b2) goto _done;
	} else {
		b2 = b;
		Py_INCREF(b);
	}

	if (a->dimensions[a->nd-1] != b2->dimensions[b2->nd-1]) {
		NA_swapAxes(b_original, -1, -2);
		PyErr_Format(PyExc_ValueError,
			     "innerproduct: last sequence dimensions must match.");
		goto _done;
	}

	result = _innerproduct(a,b2,maxt,"dot");

	if (NA_swapAxes(b_original, -1, -2) < 0) {
		Py_DECREF(result);
		result = NULL;
		goto _done;
	}

  _done:
	Py_XDECREF(a);
	Py_XDECREF(b);
	Py_XDECREF(b_original);
	Py_XDECREF(b2);
	return result;
}


#define _ALL(type, ptr)  for(i=0; i<n; i++) { if (((type *)ptr)[i] == 0) { Py_DECREF(a); return 0; } }  
#define ALL(type) case t##type: _ALL(type, a->data); break;
#define C_ALL(type, basetype) case t##type:  _ALL(basetype, (a->data)); _ALL(basetype, (a->data+sizeof(basetype))); break;

static int
_all(PyObject *o)
{
	maybelong i, n;
	PyArrayObject *a;

	a = NA_InputArray(o, tAny, C_ARRAY);
	if (!a) return -1;

	n = NA_elements(a);

	switch(a->descr->type_num) {
		ALL(Bool);
		ALL(Int8);
		ALL(Int16);
		ALL(Int32);
		ALL(Int64);
		ALL(UInt8);
		ALL(UInt16);
		ALL(UInt32);
		ALL(UInt64);
		ALL(Float32);
		ALL(Float64);
		C_ALL(Complex32, Float32);
		C_ALL(Complex64, Float64);
	}

	Py_DECREF(a);
	return 1;
}

static PyObject *
all(PyObject *module, PyObject *args)
{
	int r;
	PyObject *o;
	if (!PyArg_ParseTuple(args, "O:all", &o))
		return NULL;
	r = _all(o);
	if (r < 0) return NULL;
	return PyBool_FromLong(r);
}


#define _ANY(type, ptr)  for(i=0; i<n; i++) { if (((type *)ptr)[i] != 0) { Py_DECREF(a); return 1; } }  
#define CASE_ANY(type) case t##type: _ANY(type, a->data); break;
#define C_CASE_ANY(type, basetype) case t##type:  _ANY(basetype, (a->data)); _ANY(basetype, (a->data+sizeof(basetype))); break;

static int
_any(PyObject *o)
{
	maybelong i, n;
	PyArrayObject *a;

	a = NA_InputArray(o, tAny, C_ARRAY);
	if (!a) return -1;

	n = NA_elements(a);

	switch(a->descr->type_num) {
		CASE_ANY(Bool);
		CASE_ANY(Int8);
		CASE_ANY(Int16);
		CASE_ANY(Int32);
		CASE_ANY(Int64);
		CASE_ANY(UInt8);
		CASE_ANY(UInt16);
		CASE_ANY(UInt32);
		CASE_ANY(UInt64);
		CASE_ANY(Float32);
		CASE_ANY(Float64);
		C_CASE_ANY(Complex32, Float32);
		C_CASE_ANY(Complex64, Float64);
	}

	Py_DECREF(a);
	return 0;
}

static PyObject *
any(PyObject *module, PyObject *args)
{
	int r;
	PyObject *o;
	if (!PyArg_ParseTuple(args, "O:any", &o))
		return NULL;
	r = _any(o);
	if (r < 0) return NULL;
	return PyBool_FromLong(r);
}

static PyObject *
_array_from_array_struct(PyObject *module, PyObject *args)
{
	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O", &obj))
		return NULL;
	return (PyObject *) NA_FromArrayStruct(obj);
}


static PyMethodDef _numarray_functions[] = {
	{"_maxtype", _maxtype, METH_VARARGS, 
	 "_maxtype(seq) returns an integer code corresponding to the maximum type in seq.  0:Int, 1:Long, 2:Float, 3:Complex"},
	{"_array_from_array_struct", _array_from_array_struct, METH_VARARGS, 
	 "__array_from_array_struct(cobj) returns a numarray corresponding to the given  __array_struct__ cobj."},
	{"innerproduct", innerproduct, METH_VARARGS, 
	 "innerproduct(a,b) computes the inner product of two arrays"},
	{"dot", dot, METH_VARARGS, 
	 "dot(a,b) computes the matrix multiplication of two arrays"},
	{"all", all, METH_VARARGS,
	 "all(a) return True IFF every element of 'a' is nonzero."},
	{"any", any, METH_VARARGS,
	 "any(a) return True IFF every element of 'a' is nonzero."},
	{NULL,      NULL}        /* Sentinel */
};

DL_EXPORT(void)
	init_numarray(void) {
	PyObject *m;
	PyObject *nm, *nd, *nt;

	initialized = 0;

	if (!(nm = PyImport_ImportModule("numarray._ndarray")))  /* NEW */ {
		PyErr_Format(
			PyExc_ImportError,
			"_numarray: can't import ndarraytype extension.");
		return;
	}
	nd = PyModule_GetDict(nm);                          /* BOR */
	if (!(nt = PyDict_GetItemString(nd, "_ndarray")))   /* BOR */ {
		PyErr_Format(
			PyExc_ImportError,
			"_numarray: can't get type _ndarray._ndarray");
		return;
	}
	if (!PyType_Check(nt)) {
		PyErr_Format(
			PyExc_ImportError,
			"_numarray: _ndarray._ndarray isn't a type object");
		return;
	}

	Py_DECREF(nm); 
	Py_INCREF(nt);
	_numarray_type.tp_base = (PyTypeObject *) nt;

	if (PyType_Ready(&_numarray_type) < 0)
		return;

	m = Py_InitModule3("_numarray",
			   _numarray_functions,
			   _numarray__doc__);
	if (m == NULL)
		return;

	Py_INCREF(&_numarray_type);
	if (PyModule_AddObject(m, "_numarray", (PyObject *) &_numarray_type) < 0)
		return;

	ADD_VERSION(m);

	import_libnumarray();
	import_libnumeric();
}
