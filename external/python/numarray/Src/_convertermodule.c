#include "Python.h"
#include "structmember.h"
#include "libnumarray.h"

char *_converter__doc__ =
"_converter is a module which supplies the _converter baseclass of Converter.  Class\n"
"_converter is a baseclass used to accelerate selected methods of Converter by\n"
"handling simple cases directly in C.  More complex cases are delegated to\n"
"python coded methods.\n";

#define DEFERRED_ADDRESS(ADDR) 0

staticforward PyTypeObject _converter_type;

#define REPLACE(x0, y0) {             \
     PyObject *x = (PyObject *) x0;   \
     PyObject *y = (PyObject *) y0;   \
     Py_INCREF(y);                    \
     x0 = y;                          \
     Py_DECREF(x);                    \
}

static PyObject *
_converter_compute(PyObject *self, PyObject *indices, PyObject*shape);

static PyObject *
_converter_rebuffer(PyObject *self, PyObject *arr, PyObject *inbuffer);

static void
_converter_clean(PyObject *me, PyObject *arr);
 
static PyObject *
_converter_arr_position_get(PyConverterObject *self)
{
	return PyInt_FromLong(self->arr_position);
}

static int
_converter_arr_position_set(PyConverterObject *self, PyObject *s)
{
	if (!s) { 
		PyErr_Format(PyExc_RuntimeError, "can't delete _arr_position"); 
		return -1; 
	}
	if (!PyInt_Check(s)) {
		PyErr_Format(PyExc_TypeError, 
			     "_converter_arr_position_set:  arr_position must be an int.");
		return -1;
	}
	self->arr_position = PyInt_AsLong(s);
	if (self->arr_position < 0 || self->arr_position > 3) {
		PyErr_Format(PyExc_ValueError,
			     "_converter_arr_position_set:  arr_position out of range 0..3");
		self->arr_position = 0;
		return -1;
	}
	return 0;
}

static PyObject *
_converter_inb_position_get(PyConverterObject *self)
{
	return PyInt_FromLong(self->inb_position);
}

static int
_converter_inb_position_set(PyConverterObject *self, PyObject *s)
{
	if (!s) { 
		PyErr_Format(PyExc_RuntimeError, "can't delete _inb_position"); 
		return -1; 
	}
	if (!PyInt_Check(s)) {
		PyErr_Format(PyExc_TypeError, 
			     "_converter_inb_position_set:  inb_position must be an int.");
		return -1;
	}
	self->inb_position = PyInt_AsLong(s);
	if (self->inb_position < 0 || self->inb_position > 3) {
		PyErr_Format(PyExc_ValueError,
			     "_converter_inb_position_set:  inb_position out of range 0..3");
		self->inb_position = 0;
		return -1;
	}
	return 0;
}

static PyObject *
_converter_direction_get(PyConverterObject *self)
{
	return PyInt_FromLong(self->direction);
}

static int
_converter_direction_set(PyConverterObject *self, PyObject *s)
{
	if (!s) { 
		PyErr_Format(PyExc_RuntimeError, "can't delete _direction"); 
		return -1; 
	}
	if (!PyInt_Check(s)) {
		PyErr_Format(PyExc_TypeError, 
			     "_converter_direction_set:  direction must be an int.");
		return -1;
	}
	self->direction = PyInt_AsLong(s);
	if (self->direction < 0 || self->direction > 1) {
		PyErr_Format(PyExc_ValueError,
			     "_converter_direction_set:  direction out of range 0..1");
		self->direction = 0;
		return -1;
	}
	return 0;
}

static PyObject *
_converter_generated_get(PyConverterObject *self)
{
	return PyInt_FromLong(self->generated);
}

static int
_converter_generated_set(PyConverterObject *self, PyObject *s)
{
	if (!s) { 
		PyErr_Format(PyExc_RuntimeError, "can't delete _generated"); 
		return -1; 
	}
	if (!PyInt_Check(s)) {
		PyErr_Format(PyExc_TypeError, 
			     "_converter_generated_set:  generated must be an int.");
		return -1;
	}
	self->generated = PyInt_AsLong(s);
	if (self->generated < 0 || self->generated > 1) {
		PyErr_Format(PyExc_ValueError,
			     "_converter_generated_set:  generated out of range 0..1");
		self->generated = 0;
		return -1;
	}
	return 0;
}

static PyObject *
_converter_conversion_required_get(PyConverterObject *self)
{
	return PyInt_FromLong(self->conversion_required);
}

static int
_converter_conversion_required_set(PyConverterObject *self, PyObject *s)
{
	if (!s) { 
		PyErr_Format(PyExc_RuntimeError, "can't delete conversion_required"); 
		return -1; 
	}
	if (!PyInt_Check(s)) {
		PyErr_Format(PyExc_TypeError, 
			     "_converter_conversion_required_set:  conversion_required must be an int.");
		return -1;
	}
	self->conversion_required = PyInt_AsLong(s);
	if (self->conversion_required < 0 || self->conversion_required > 1) {
		PyErr_Format(PyExc_ValueError,
			     "_converter_conversion_required_set:  conversion_required out of range 0..1");
		self->conversion_required = 0;
		return -1;
	}
	return 0;
}

static PyObject *
_converter_buffers_get(PyConverterObject *self)
{
	PyObject *result = PyList_New(ELEM(self->buffers));
	int i;
	if (!result) return NULL;
	for (i=0; i<ELEM(self->buffers); i++) {
		Py_INCREF(self->buffers[i]);
		PyList_SET_ITEM(result, i, self->buffers[i]);
	}
	return result;
}

static int
_converter_buffers_set(PyConverterObject *self, PyObject *s)
{
	int i;
	if (!s) { 
		PyErr_Format(PyExc_RuntimeError, 
			     "_converter_buffers_set: can't delete buffers"); 
		return -1; 
	}
	if (!PyList_Check(s) || PyList_GET_SIZE(s)<ELEM(self->buffers)) {
		PyErr_Format(PyExc_ValueError,
			     "_converter_buffers_set: buffers must be a %d element list.",
			     (int) ELEM(self->buffers));
		return -1;
	}
	for(i=0; i<ELEM(self->buffers); i++) {
		PyObject *new = PyList_GET_ITEM(s, i);
		REPLACE(self->buffers[i], new);
	}
	return 0;
}

static PyObject *
_converter_bytestrides_get(PyConverterObject *self)
{
	PyObject *result = PyList_New(ELEM(self->bytestrides));
	int i;
	if (!result) return NULL;
	for (i=0; i<ELEM(self->bytestrides); i++) {
		Py_INCREF(self->bytestrides[i]);
		PyList_SET_ITEM(result, i, self->bytestrides[i]);
	}
	return result;
}

static int
_converter_bytestrides_set(PyConverterObject *self, PyObject *s)
{
	int i;
	if (!s) { 
		PyErr_Format(PyExc_RuntimeError, 
			     "_converter_bytestrides_set: can't delete bytestrides"); 
		return -1; 
	}
	if (!PyList_Check(s) || PyList_GET_SIZE(s)<ELEM(self->bytestrides)) {
		PyErr_Format(PyExc_ValueError,
			     "_converter_bytestrides_set: bytestrides must be a %d element list.",
			     (int) ELEM(self->bytestrides));
		return -1;
	}
	for(i=0; i<ELEM(self->bytestrides); i++) {
		PyObject *new = PyList_GET_ITEM(s, i);
		REPLACE(self->bytestrides[i], new);
	}
	return 0;
}

static PyObject *
_converter_convfunction_get(PyConverterObject *self)
{
	Py_INCREF(self->convfunction);
	return self->convfunction;
}

static int
_converter_convfunction_set(PyConverterObject *self, PyObject *s)
{
	if (!s) { 
		PyErr_Format(PyExc_RuntimeError, "can't delete _convfunction"); 
		return -1; 
	}
	if ((!NA_CfuncCheck(s)) && s != Py_None) {
		PyErr_Format(PyExc_TypeError, 
			     "_converter_convfunction_set:  convfunction must be a cfunc.");
		return -1;
	}
	Py_INCREF(s);
	Py_DECREF(self->convfunction);
	self->convfunction = s;
	return 0;
}

static PyObject *
_converter_stridefunction_get(PyConverterObject *self)
{
	Py_INCREF(self->stridefunction);
	return self->stridefunction;
}

static int
_converter_stridefunction_set(PyConverterObject *self, PyObject *s)
{
	if (!s) { 
		PyErr_Format(PyExc_RuntimeError, "can't delete _stridefunction"); 
		return -1; 
	}
	if ((!NA_CfuncCheck(s)) && s != Py_None) {
		PyErr_Format(PyExc_TypeError, 
			     "_converter_stridefunction_set:  stridefunction must be a cfunc.");
		return -1;
	}
	Py_INCREF(s);
	Py_DECREF(self->stridefunction);
	self->stridefunction = s;
	return 0;
}

static PyObject *
_converter_result_buff_get(PyConverterObject *self)
{
	Py_INCREF(self->result_buff);
	return self->result_buff;
}

static int
_converter_result_buff_set(PyConverterObject *self, PyObject *s)
{
	if (!s) {
		PyErr_Format(PyExc_RuntimeError, "can't delete _result_buff");
		return -1;
	}
	Py_INCREF(s);
	Py_DECREF(self->result_buff);
	self->result_buff = s;
	return 0;
}

static PyGetSetDef _converter_getsets[] = {
	{"arr_position", 
	 (getter)_converter_arr_position_get, 
	 (setter)_converter_arr_position_set, 
	 "position of array in buffer list"}, 
	{"inb_position", 
	 (getter)_converter_inb_position_get, 
	 (setter)_converter_inb_position_set, 
	 "position of inbuffer in buffer list"}, 
	{"direction", 
	 (getter)_converter_direction_get, 
	 (setter)_converter_direction_set, 
	 "direction of conversion: input=0 output=1"}, 
	{"generated", 
	 (getter)_converter_generated_get, 
	 (setter)_converter_generated_set, 
	 "generated of conversion: input=0 output=1"}, 
	{"conversion_required", 
	 (getter)_converter_conversion_required_get, 
	 (setter)_converter_conversion_required_set, 
	 "does converter stride or type convert?"}, 
	{"buffers", 
	 (getter)_converter_buffers_get, 
	 (setter)_converter_buffers_set, 
	 "buffer chain for conversions"}, 
	{"bytestrides", 
	 (getter)_converter_bytestrides_get, 
	 (setter)_converter_bytestrides_set, 
	 "buffer chain for conversions"}, 
	{"convfunction", 
	 (getter)_converter_convfunction_get, 
	 (setter)_converter_convfunction_set, 
	 "convfunction of conversion"}, 
	{"stridefunction", 
	 (getter)_converter_stridefunction_get, 
	 (setter)_converter_stridefunction_set, 
	 "stridefunction of conversion: input=0 output=1"}, 
	{"result_buff", 
	 (getter)_converter_result_buff_get, 
	 (setter)_converter_result_buff_set, 
	 "result_buff of conversion: input=0 output=1"}, 
	{0}
};

static PyObject *
_converter_new(PyTypeObject *type, PyObject  *args, PyObject *kwds)
{
	PyConverterObject *self = 
		(PyConverterObject *) PyType_GenericNew(type, args, kwds);
	int i;

	if (!self)
		return NULL;

	self->arr_position = self->inb_position = 0;

	for(i=0; i<ELEM(self->buffers); i++) {
		Py_INCREF(Py_None);
		self->buffers[i] = Py_None;
	}
	for(i=0; i<ELEM(self->bytestrides); i++) {
		Py_INCREF(Py_None);
		self->bytestrides[i] = Py_None;
	}

	self->rebuffer = _converter_rebuffer;
	self->compute = _converter_compute;
	self->clean = _converter_clean;

	self->result_buff = self->convfunction = 
		self->stridefunction = Py_None;
	Py_INCREF(Py_None);
	Py_INCREF(Py_None);
	Py_INCREF(Py_None);

	return (PyObject *) self;
}

static void
_converter_dealloc(PyObject *self)
{
	PyConverterObject *me = (PyConverterObject *) self;
	int i;
	for(i=0; i<ELEM(me->buffers); i++)
		Py_XDECREF(me->buffers[i]);
	for(i=0; i<ELEM(me->bytestrides); i++)
		Py_XDECREF(me->bytestrides[i]);
	Py_XDECREF(me->convfunction);
	Py_XDECREF(me->stridefunction);
	Py_XDECREF(me->result_buff);
	self->ob_type->tp_free(self);
}

static int
_stridesFromShape(PyArrayObject *arr, maybelong *strides)
{
    int i;
    if (arr->nd > 0) {
	for(i=0; i<arr->nd; i++)
	    strides[i] = arr->itemsize;
	for(i=arr->nd-2; i>=0; i--)
	    strides[i] = strides[i+1]*arr->dimensions[i+1];
	return arr->nd;
    } else 
	    return 0;
}

static void
_converter_clean(PyObject *me, PyObject *arr)
{
	PyConverterObject *self = (PyConverterObject *) me;
	int i;

	assert(libnumarray_API);
	assert(NA_ConverterCheck(me));
	assert(NA_NumArrayCheck(arr));

	for(i=0; i<ELEM(self->buffers); i++)
		if (self->buffers[i] == arr) {
			REPLACE(self->buffers[i], Py_None);
		}
	if (self->result_buff == arr) {
		REPLACE(self->result_buff, Py_None);
	}	
}

static PyObject *
_converter_rebuffer(PyObject *me, PyObject *arr, PyObject *inbuffer)
{
	PyConverterObject *self = (PyConverterObject *) me;

	assert(NA_ConverterCheck(me));
	assert(NA_NumArrayCheck(arr) || arr == Py_None);
	assert(NA_NumArrayCheck(inbuffer) || inbuffer == Py_None);

	if (!self->conversion_required) {
		REPLACE(self->result_buff, arr);
		Py_INCREF(arr);
		return arr;
	}

	REPLACE(self->buffers[self->arr_position], arr);

	if (!self->generated && inbuffer != Py_None) {
		REPLACE(self->buffers[self->inb_position], inbuffer);
		REPLACE(self->result_buff, inbuffer);
	}
	
	if (arr != Py_None && (self->bytestrides[0] != Py_None || 
			       self->bytestrides[1] != Py_None))
	{
		PyObject *arr_strides, *ustrides;
		PyArrayObject *array = (PyArrayObject *) arr;

		arr_strides = NA_intTupleFromMaybeLongs(
			array->nstrides, array->strides);
		if (!arr_strides) return NULL;
		
		if (self->generated) {
			maybelong nstrides, strides[MAXDIM];
			if (!NA_NumArrayCheck(arr))
				return PyErr_Format(PyExc_TypeError,
				    "_converter_rebuffer: non-numarray in stride compute block");
			nstrides = _stridesFromShape((PyArrayObject*)arr, strides);
			ustrides = NA_intTupleFromMaybeLongs(nstrides, strides);
			if (!ustrides) return NULL;
		} else {
			if (inbuffer != Py_None) {
				PyArrayObject *inbuffera = (PyArrayObject *) inbuffer;
				ustrides = NA_intTupleFromMaybeLongs(inbuffera->nstrides,
							       inbuffera->strides);
				if (!ustrides) return NULL;
			} else {
				ustrides = self->bytestrides[ !self->direction ];
				Py_INCREF(ustrides);
			}
		}
		
		Py_DECREF(self->bytestrides[0]);
		Py_DECREF(self->bytestrides[1]);
		
		if  (self->direction) {
			self->bytestrides[0] = ustrides;
			self->bytestrides[1] = arr_strides;
		} else {
			self->bytestrides[0] = arr_strides;		
			self->bytestrides[1] = ustrides;
		}
	}
	Py_INCREF(self->result_buff);
	return self->result_buff;
}
 
static PyObject *
_Py_converter_rebuffer(PyObject *self, PyObject *args)
{
	PyObject *arr, *inbuffer=Py_None;
	if (!PyArg_ParseTuple(args, "O|O:_converter_rebuffer", &arr, &inbuffer))
		return NULL;
	return _converter_rebuffer(self, arr, inbuffer);
}

static int
_converter_convert(PyConverterObject *self, int buf, PyObject *indiceso, PyObject *shape)
{
	long niter;
	PyObject *result, *buffers[2];
	long offsets[2];
	maybelong nindices, indices[MAXDIM];
	PyArrayObject  *arr1, *arr2;

	assert(NA_ConverterCheck((PyObject *)self));

	arr1 = (PyArrayObject *) self->buffers[buf*2+0];
	arr2 = (PyArrayObject *) self->buffers[buf*2+1];

	if (!self->convfunction) return -1;
	if (self->convfunction == Py_None) {
		return 0;
	}
	if (NA_intTupleProduct(shape, &niter) < 0) 
		return -1;
	if ((nindices = NA_maybeLongsFromIntTuple(
		     MAXDIM, indices, indiceso)) < 0)
		return -1;
	if (NA_getByteOffset(arr1, nindices, indices, &offsets[0]) < 0)
		return -1;
	if (NA_getByteOffset(arr2, nindices, indices, &offsets[1]) < 0)
		return -1;
	if (!(buffers[0] = arr1->_data))
		return -1;
	if (!(buffers[1] = arr2->_data))
		return -1;

	result = NA_callCUFuncCore( self->convfunction, niter, 1, 1, buffers, offsets);
	if (result) {
		Py_DECREF(result);
		return 0;
	}
	return -1;
}


static int 
_converter_stride(PyConverterObject *self, int buf, PyObject *indiceso, 
		  PyObject *shapeo)
{
	PyObject *result, *buffers[2], *bytestrides[2];
	long offsets[2];
	maybelong nindices, indices[MAXDIM], nshape, ishape[MAXDIM], 
		nstrides[2], istrides[2][MAXDIM];
	PyArrayObject  *arr1, *arr2;

	if (!self->stridefunction) return -1;
	if (self->stridefunction == Py_None) {
		return 0;
	}

	arr1 = (PyArrayObject *) self->buffers[buf*2+0];
	arr2 = (PyArrayObject *) self->buffers[buf*2+1];

	if ((nindices = NA_maybeLongsFromIntTuple(
		     MAXDIM, indices, indiceso)) < 0)
		return -1;
	if ((nshape = NA_maybeLongsFromIntTuple(MAXDIM, ishape, shapeo)) < 0)
		return -1;

	bytestrides[0] = PySequence_GetSlice(
		self->bytestrides[0], -nshape, INT_MAX);
	if (!bytestrides[0]) return -1;
	bytestrides[1] = PySequence_GetSlice(
		self->bytestrides[1], -nshape, INT_MAX);
	if (!bytestrides[1]) return -1;
	if ((nstrides[0] = NA_maybeLongsFromIntTuple(
		     MAXDIM, &istrides[0][0], bytestrides[0])) < 0)
		return -1;
	if ((nstrides[1] = NA_maybeLongsFromIntTuple(
		     MAXDIM, &istrides[1][0], bytestrides[1])) < 0)
		return -1;
	Py_DECREF(bytestrides[0]);
	Py_DECREF(bytestrides[1]);	

	if (NA_getByteOffset(arr1, nindices, indices, &offsets[0]) < 0)
		return -1;
	if (NA_getByteOffset(arr2, nindices, indices, &offsets[1]) < 0)
		return -1;
	if (!(buffers[0] = arr1->_data))
		return -1;
	if (!(buffers[1] = arr2->_data))
		return -1;
	
	/* args = Py_BuildValue("OOiOOiO", shape, 
			     buffers[0], offsets[0], bytestrides[0],
			     buffers[1], offsets[1], bytestrides[1]);
	if (!args) return -1;
	result = PyObject_CallObject(self->stridefunction, args);

	Py_DECREF(args);
	*/
	result = NA_callStrideConvCFuncCore( 
		self->stridefunction, nshape, ishape,
		buffers[0], offsets[0], nstrides[0], istrides[0],
		buffers[1], offsets[1], nstrides[1], istrides[1], 0);

	if (result) {
		Py_DECREF(result);
		return 0;
	}
	return -1;
}

static PyObject *
_converter_compute(PyObject *me, PyObject *indices, PyObject *shape)
{
	PyConverterObject *self = (PyConverterObject *) me;
	if (self->direction) {
		if (_converter_convert(self, 0, indices, shape) < 0) 
			return NULL;
		if (_converter_stride(self, 1, indices, shape) < 0) 
			return NULL;
	} else {
		if (_converter_stride(self, 0, indices, shape) < 0) 
			return NULL;
		if (_converter_convert(self, 1, indices, shape) < 0) 
			return NULL;
	}
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *
_Py_converter_compute(PyObject *self, PyObject *args)
{
	PyObject *indices, *shape;
	if (!PyArg_ParseTuple(args, "OO:_converter_compute", 
			      &indices, &shape))
		return NULL;
	if (!PyTuple_Check(indices))
		return PyErr_Format( PyExc_TypeError, 
			      "_converter_compute: indices is not a tuple");
	if (!PyTuple_Check(shape))
		return PyErr_Format( PyExc_TypeError, 
			      "_converter_compute: shape is not a tuple");
	return _converter_compute(self, indices, shape);
}
static PyMethodDef _converter_methods[] = {
	{"rebuffer", (PyCFunction)_Py_converter_rebuffer, METH_VARARGS,
	   "rebuffer(self, arr, inbuffer=None)  re-configures converter for 'arr' and returns buffer closest to ufunc"},
	{"compute", (PyCFunction)_Py_converter_compute, METH_VARARGS,
	   "compute(self, indices, shape)  converts one block at 'indices' with 'shape'"},
	{NULL,	NULL},
};

static PyTypeObject _converter_type = {
	PyObject_HEAD_INIT(DEFERRED_ADDRESS(&PyType_Type))
	0,
	"numarray._converter._converter",
	sizeof(PyConverterObject),
	0,
	_converter_dealloc,	                /* tp_dealloc */
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
	_converter_methods,			/* tp_methods */
	0,					/* tp_members */
	_converter_getsets,			/* tp_getset */
	0,                                      /* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	0,		                        /* tp_init */
	0,   		                        /* tp_alloc */
	_converter_new,				/* tp_new */
};

static PyMethodDef _converter_functions[] = {
/*	{"_isIntegerSequence", 
	 (PyCFunction) _converter_isIntegerSequence, 
	 METH_VARARGS,
	 "_isIntegerSequence(s) returns 1 if all elements of sequence s are integers, else 0"},
*/
	{NULL,		NULL}		/* sentinel */
};

DL_EXPORT(void)
init_converter(void)
{
	PyObject *m;

	_converter_type.tp_alloc = PyType_GenericAlloc;

	if (PyType_Ready(&_converter_type) < 0)
		return;

	m = Py_InitModule3("_converter",
			   _converter_functions,
			   _converter__doc__);
	if (m == NULL)
		return;

	Py_INCREF(&_converter_type);
	if (PyModule_AddObject(m, "_converter",
			       (PyObject *) &_converter_type) < 0)
		return;

	ADD_VERSION(m);

	import_libnumarray();
}
