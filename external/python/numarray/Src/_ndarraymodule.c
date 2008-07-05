#include "Python.h"
#include "structmember.h"
#include "libnumarray.h"

#include <math.h>

#define DEBUG(x) 

/* Set _PYTHON_CALLBACKS to 1 for debugging with prototype Python code */
#define _PYTHON_CALLBACKS 0

char *_ndarray__doc__ =
"_ndarray is a module which supplies the _ndarray baseclass of NDArray.  Class\n"
"_ndarray is a baseclass used to accelerate selected methods of NDArray by\n"
"handling simple cases directly in C.  More complex cases are delegated to\n"
"python coded methods.\n";

#define DEFERRED_ADDRESS(ADDR) 0

static PyObject *pNewMemoryFunc;
static PyObject *_alignment;

staticforward PyTypeObject _ndarray_type;

static PyObject *zero, *one, *NewAxisSlice, *FullSlice, *NullArgs, *NullKeywds;

static PyObject *
_x_view(PyArrayObject *self);

static PyArrayObject *
_view(PyArrayObject *self);

static PyObject *
_ndarray_rank_get(PyArrayObject *self)
{
	return PyInt_FromLong(self->nd);
}

static PyObject *
_ndarray_flags_get(PyArrayObject *self)
{
	NA_updateStatus(self);
	return PyInt_FromLong(self->flags);
}

static int
_ndarray_flags_set(PyArrayObject *self, PyObject *f)
{
	if (!f) {
		PyErr_Format( PyExc_RuntimeError, "can't delete flags");
		return -1;			
	}
	if (!PyInt_Check(f)) {
		PyErr_Format( PyExc_TypeError, "flags must be an integer value");
		return -1;
	}
	self->flags = PyInt_AsLong(f);
	NA_updateStatus(self);
	return 0;
}


static PyObject *
_ndarray_shape_get(PyArrayObject *self)
{
	return NA_intTupleFromMaybeLongs(self->nd, self->dimensions);
}

static int
_ndarray_shape_set(PyArrayObject *self, PyObject *s)
{
	long i, ndim;
	if (!s) { 
		PyErr_Format(PyExc_RuntimeError, "can't delete _shape"); 
		return -1; 
	}
	ndim = NA_maybeLongsFromIntTuple(MAXDIM, self->dimensions, s);
	for(i=0; i<ndim; i++)
		if (self->dimensions[i] < 0)
			ndim = -1;
	if (ndim < 0) {
		PyErr_Format( PyExc_ValueError, "invalid shape tuple");
		return -1;
	} else {
		self->nd = ndim;
		NA_updateContiguous(self);
		return 0;
	}
}

static PyObject *
_ndarray_strides_get(PyArrayObject *self)
{
	if (self->nstrides >= 0) 
		return NA_intTupleFromMaybeLongs(self->nstrides, self->strides);
	else {
		Py_INCREF(Py_None);
		return Py_None;
	}
}

static int
_ndarray_strides_set(PyArrayObject *self, PyObject *s)
{
	long nstrides;
	if (s == Py_None){
		self->nstrides = -1;
	} else {
		nstrides = NA_maybeLongsFromIntTuple(MAXDIM, self->strides, s);
		if (!s) { 
			PyErr_Format(PyExc_RuntimeError, "can't delete _strides"); 
			return -1; 
		}
		if (nstrides < 0) {
			return -1;
		} else {
			self->nstrides = nstrides;
			NA_updateStatus(self);
		}
	}
	return 0;
}

/* NOTE:   ->data is a C pointer to the actual buffer region.
           ->_data is a pointer to a "bufferable" python object.
*/
static PyObject *
_ndarray_data_get(PyArrayObject *self)
{
	
	Py_INCREF(self->_data);
	return self->_data;
}

static PyObject *
_ndarray_fragile_data_get(PyArrayObject *self)
{
	return PyLong_FromUnsignedLong((unsigned long) self->data);
}

static PyObject *
_ndarray_dataptr_get(PyArrayObject *self)
{
	char address[40], *ap;
	snprintf(address, sizeof(address), "%p", self->data);
	ap = address;
	if (strncmp(address, "0x", 2) == 0) {
		ap += 2;
	}	
	return Py_BuildValue("(s,i)", ap, (self->flags & WRITABLE) == 0);
}

static int
_ndarray_data_set(PyArrayObject *self, PyObject *s)
{
	if (!s) { 
		PyErr_Format(PyExc_RuntimeError, "can't delete _data"); 
		return -1; 
	}
	Py_INCREF(s);
	Py_DECREF(self->_data);
	self->_data = s;
	if (!NA_updateDataPtr(self))
		return -1;
	NA_updateAlignment(self);
	return 0;
}

static PyObject *
_ndarray_itemsize_get(PyArrayObject *self)
{
	return PyInt_FromLong(self->itemsize);
}

static int
_ndarray_itemsize_set(PyArrayObject *self, PyObject *s)
{
	long n, rval = 0;
	if (!s) { 
		PyErr_Format(PyExc_RuntimeError, "can't delete _itemsize"); 
		return -1; 
	}
	if (PyInt_Check(s) && ((n=PyInt_AsLong(s)) >= 0)) {
		self->itemsize = n;
	} else if (s == Py_None) {      /* support unpickling */
		self->itemsize = 1;
	} else {
		PyErr_Format(PyExc_TypeError, "_itemsize must be an integer >= 0.");
		rval = -1;
	}
	NA_updateStatus(self);
	return rval;
}

static PyObject *
_ndarray_bytestride_get(PyArrayObject *self)
{
	return PyInt_FromLong(self->bytestride);
}

static int
_ndarray_bytestride_set(PyArrayObject *self, PyObject *s)
{
	long rval = 0;
	if (!s) { 
		PyErr_Format(PyExc_RuntimeError, "can't delete _bytestride"); 
		return -1; 
	}
	if (s == Py_None) {
		self->bytestride = self->itemsize;
	} else {
		if (PyInt_Check(s)) {
			self->bytestride = PyInt_AsLong(s);
		} else {
			PyErr_Format(PyExc_TypeError, 
			     "_ndarray_init: non-integer bytestride.");
			return -1;
		}
	}
	NA_updateContiguous(self);
	return rval;
}

static PyObject *
_ndarray_byteoffset_get(PyArrayObject *self)
{
	return PyInt_FromLong(self->byteoffset);
}

static int
_ndarray_byteoffset_set(PyArrayObject *self, PyObject *s)
{
	long rval = 0;
	if (!s) { 
		PyErr_Format(PyExc_RuntimeError, "can't delete _byteoffset"); 
		return -1; 
	}
	if (PyInt_Check(s)) {
		self->byteoffset = PyInt_AsLong(s);
	} else {
		PyErr_Format(PyExc_TypeError, "_byteoffset must be an integer.");
		rval = -1;
	}
	if (!NA_updateDataPtr(self))
		return -1;
	NA_updateStatus(self);
	return rval;
}

static PyGetSetDef _ndarray_getsets[] = {
	{"rank", 
	 (getter)_ndarray_rank_get, 
	 (setter) 0, 
	 "number of array dimensions"}, 
	{"ndim", 
	 (getter)_ndarray_rank_get, 
	 (setter) 0, 
	 "number of array dimensions (same as rank, for numpy)"}, 
	{"_flags", 
	 (getter)_ndarray_flags_get, 
	 (setter)_ndarray_flags_set, 
	 "integer value of _flags"}, 
	{"_shape", 
	 (getter)_ndarray_shape_get, 
	 (setter)_ndarray_shape_set, 
	 "tuple of array dimensions"}, 
	{"__array_shape__", /* scipy newcore */
	 (getter)_ndarray_shape_get, 
	 (setter)_ndarray_shape_set, 
	 "tuple of array dimensions (scipy newcore array interface)"}, 
	{"_strides", 
	 (getter)_ndarray_strides_get, 
	 (setter)_ndarray_strides_set, 
	 "tuple of byte steps"}, 
	{"__array_strides__",  /* scipy newcore */
	 (getter)_ndarray_strides_get, 
	 (setter)_ndarray_strides_set, 
	 "tuple of byte steps (scipy newcore array interface)"}, 
	{"_data", 
	 (getter)_ndarray_data_get,
	 (setter)_ndarray_data_set, 
	 "buffer object holding array data"}, 
	{"__array_data__", 
	 (getter)_ndarray_dataptr_get, 0,
	 "(dataptr, readonly) (scipy newcore array interface)"}, 
	{"_fragile_data", 
	 (getter)_ndarray_fragile_data_get,
	 (setter)NULL, 
	 "pointer to first array element, DEBUG ONLY"}, 
	{"_byteoffset", 
	 (getter)_ndarray_byteoffset_get,
	 (setter)_ndarray_byteoffset_set, 
	 "shortest seperation between elements in bytes"}, 
	{"_bytestride", 
	 (getter)_ndarray_bytestride_get,
	 (setter)_ndarray_bytestride_set, 
	 "shortest seperation between elements in bytes"}, 
	{"_itemsize", 
	 (getter)_ndarray_itemsize_get,
	 (setter)_ndarray_itemsize_set, 
	 "length of one element in bytes"}, 
	{0}
};

static PyObject *
_ndarray_new(PyTypeObject *type, PyObject  *args, PyObject *kwds)
{
	PyArrayObject *self = (PyArrayObject *) type->tp_alloc(type, 0);
	if (!self) return NULL;
	Py_INCREF(Py_None);
	self->_data = Py_None;
	self->data = NULL;
	self->nd = 0;
	self->dimensions = self->_dimensions;
	self->strides    = self->_strides;
	self->base = NULL;
	if (!(self->descr = NA_DescrFromType( tAny))) {
		PyErr_Format(PyExc_RuntimeError, 
			     "_ndarray_new: bad type number");
		return NULL;
	}
	self->flags = ALIGNED | NOTSWAPPED | CONTIGUOUS;
	self->itemsize = 0;
	self->bytestride = 0;
	self->byteoffset = 0;
	self->_shadows = NULL;
	return (PyObject *) self;
}

static void
_stridesFromShape(PyArrayObject *self)
{
    int i;
    if (self->nd > 0) {
	for(i=0; i<self->nd; i++)
	    self->strides[i] = self->bytestride;
	for(i=self->nd-2; i>=0; i--)
	    self->strides[i] = 
		self->strides[i+1]*self->dimensions[i+1];
	self->nstrides = self->nd;
    } else 
	self->nstrides = 0;
}
 
static int
_ndarray_init(PyArrayObject *self, PyObject *args, PyObject *kwds)
{
        static char *kwlist[] = {"shape", "itemsize", "buffer", 
				 "byteoffset", "bytestride", "aligned", NULL};
	PyObject *shape = NULL, *shape0;
	int       itemsize = 0;
	PyObject *buffer = Py_None;
	int       byteoffset = 0;
	PyObject *bytestride = Py_None;
	int       aligned = 1;

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OiOiOi", kwlist, 
			&shape, &itemsize, &buffer, 
			&byteoffset, &bytestride, &aligned))
		return -1;
		
	if (!shape) {
		shape0 = PyTuple_New(0);
		if (!shape0) {
			PyErr_Format(PyExc_RuntimeError, 
			     "_numarray_init: can't allocate shape tuple");
			return -1;
		}
	} else {
		if (PyInt_Check(shape) || PyLong_Check(shape)) {
			shape0 = Py_BuildValue("(O)", shape);
			if (!shape0) return -1;
		} else {
			shape0 = shape;
			Py_INCREF(shape0);
		}
	}

	if (_ndarray_shape_set(self, shape0) < 0) {
		Py_DECREF(shape0);
		return -1;
	}
	Py_DECREF(shape0);
		
	self->itemsize = itemsize;
	self->byteoffset = byteoffset;

	if (_ndarray_bytestride_set(self, bytestride) < 0)
		return -1;

	Py_XDECREF(self->_data);
	if (buffer != Py_None) {
		self->_data = buffer;
		Py_INCREF(buffer);
	} else {
		Int64 size = self->bytestride * 
			(Int64) NA_elements(self);
		self->_data = PyObject_CallFunction(
			pNewMemoryFunc, "(L)", size);
		if (!self->_data) return -1;
	}
	
	if (!NA_updateDataPtr(self))
		return -1;

	_stridesFromShape(self);
	self->_shadows = NULL;
	NA_updateStatus(self);
	return 0;
}

static void
_ndarray_dealloc(PyObject *self)
{
	PyArrayObject *me = (PyArrayObject *) self;
	PyObject *shadowed = me->_shadows;
	me->_shadows = NULL;
	if (shadowed) {
		PyObject *result, *err_type, *err_value, *err_traceback;
		int have_error = PyErr_Occurred() ? 1 : 0;
		if (have_error)
			PyErr_Fetch(&err_type, &err_value, &err_traceback);
		DEBUG(fprintf(stderr, "_ndarray_dealloc: shadowed.\n"));
		me->ob_refcnt = 1;
		result = PyObject_CallMethod(
			shadowed, "_copyFrom", "(O)", me);
		assert(me->ob_refcnt > 0);
		if (--me->ob_refcnt == 0) {
			DEBUG(fprintf(stderr, "_ndarray_dealloc: zero references remaining.\n"));
			Py_DECREF(shadowed);
			if (result == NULL) {
				PyObject *context = PyObject_GetAttrString(
					shadowed, "_copyFrom");
				if (context) {
					PyErr_WriteUnraisable( context );
					Py_DECREF(context);
				}
			} else
				Py_DECREF(result);
		} else {  /* _copyFrom brought self back to life */
			int refcnt = me->ob_refcnt;
			DEBUG(fprintf(stderr, "_ndarray_dealloc: resurrected.\n"));
 			_Py_NewReference((PyObject *)me);
			me->ob_refcnt = refcnt;
			return;
		}
		if (have_error)
			PyErr_Restore(err_type, err_value, err_traceback);
	}
	Py_XDECREF(me->base);
	me->base = NULL;
	Py_XDECREF(me->_data);
	me->_data = NULL;
	self->ob_type->tp_free(self);
}

static int
_ndarray_length(PyArrayObject *self)
{
	if (self->nd)
		return self->dimensions[0];
	else {
		PyErr_Format(PyExc_ValueError, "Rank-0 array has no length.");
		return -1;
	}
}

static int
_getByteOffset(PyArrayObject *self, PyObject *indiceso, long *offset)
{
	maybelong nindices, indices[MAXDIM];
	if ((nindices = NA_maybeLongsFromIntTuple(MAXDIM, indices, indiceso)) < 0)
		return -1;
	if (NA_getByteOffset(self, nindices, indices, offset) < 0)
		return -1;
	return 0;
}

static PyObject *
_ndarray_getByteOffset(PyArrayObject *self, PyObject *args)
{
	PyObject *indiceso;
	long offset;
	if (!PyArg_ParseTuple(args, "O:_getByteOffset", &indiceso))
		return NULL;
	if (_getByteOffset(self, indiceso, &offset) < 0) 
		return NULL;
	return PyInt_FromLong(offset);
}

static PyObject *
_ndarray_getitem(PyArrayObject *self, long offset)
{
	PyObject *result = NULL;
	if (self->descr && self->descr->_get) {
		result = self->descr->_get(self, offset - self->byteoffset);
	} else
		result = PyObject_CallMethod(
			(PyObject *) self, "_getitem", "(i)", offset);
	return result;
}

static int
_ndarray_setitem(PyArrayObject *self, long offset, PyObject *value)
{
	int rval = -1;
	
	if (value == NULL) {
		PyErr_Format(PyExc_TypeError,
			     "_ndarray_setitem: can't delete elements from an array");
		return -1;
	}

	if (self->descr && self->descr->_set) {
	  NA_updateDataPtr(self);
	  rval = self->descr->_set(self, offset - self->byteoffset, value);
	} else {
		PyObject *result = PyObject_CallMethod(
			(PyObject *) self, "_setitem", "(iO)", offset, value);
		if (result) {
			Py_DECREF(result);
			rval = 0;
		}
	}
	return rval;
}

static PyObject *
_simpleIndexingCore(PyArrayObject *self, long offset, int nindices, PyObject *value)
{
	PyArrayObject *result = NULL;

	if (nindices > self->nd) {
		PyErr_Format(PyExc_IndexError, "Too many indices");
		goto _exit;
	} else if (nindices == self->nd) {   /* fully subscripted --> scalar */
		if (value == Py_None)
			result = (PyArrayObject *) _ndarray_getitem(self, offset);
		else {
			if (_ndarray_setitem(self, offset, value) < 0)
				goto _exit;
			result = (PyArrayObject *) Py_None;
			Py_INCREF(Py_None);
		}
	} else {  /* partially subscripted --> subarray */
		long i;

		result = _view(self);
		if (!result) goto _exit;
		
		result->nd = result->nstrides = self->nd - nindices;
		for(i=nindices; i<self->nd; i++) {
			result->dimensions[i-nindices] = self->dimensions[i];
			result->strides[i-nindices] = self->strides[i];
		}
		
		result->byteoffset = offset;
		if (!NA_updateDataPtr(result))
			return NULL;

		NA_updateStatus(result);

		if (value == Py_None)
			goto _exit;
		else {
			PyObject *r2 = (PyObject *) result;
			result = (PyArrayObject *) PyObject_CallMethod(
				r2, "_copyFrom", "(O)", value);
			Py_DECREF(r2);
		}
	}	
  _exit:
	return  (PyObject *) result;
}

static PyObject *
_simpleIndexing(PyArrayObject *self, PyObject *key, PyObject *value)
{
	long offset;
	int nindices = PySequence_Length(key);
	if (nindices < 0) 
		return NULL;
	if (_getByteOffset(self, key, &offset) < 0)
		return NULL;
	return _simpleIndexingCore(self, offset, nindices, value);
}

static PyObject *
_ndarray_simpleIndexing(PyArrayObject *self, PyObject *args)
{
	PyObject *key, *value;
	if (!PyArg_ParseTuple(args, "OO:_simpleIndexing", &key, &value))
		return NULL;
	return _simpleIndexing(self, key, value);
}

/* Copied from Python-2.3.3 Objects/sliceobject.c */
int
_GetIndicesEx(PySliceObject *r, int length,
		     int *start, int *stop, int *step, int *slicelength)
{
	/* this is harder to get right than you might think */

	int defstart, defstop;

	if (r->step == Py_None) {
		*step = 1;
	} 
	else {
		if (!_PyEval_SliceIndex(r->step, step)) return -1;
		if (*step == 0) {
			PyErr_SetString(PyExc_IndexError,
					"slice step of zero not allowed");
			return -1;
		}
	}

	defstart = *step < 0 ? length-1 : 0;
	defstop = *step < 0 ? -1 : length;

	if (r->start == Py_None) {
		*start = defstart;
	}
	else {
		if (!_PyEval_SliceIndex(r->start, start)) return -1;
		if (*start < 0) *start += length;
		if (*start < 0) *start = (*step < 0) ? -1 : 0;
		if (*start >= length) 
			*start = (*step < 0) ? length - 1 : length;
	}

	if (r->stop == Py_None) {
		*stop = defstop;
	}
	else {
		if (!_PyEval_SliceIndex(r->stop, stop)) return -1;
		if (*stop < 0) *stop += length;
		if (*stop < 0) *stop = -1;
		if (*stop > length) *stop = length;
	}

	if ((*step < 0 && *stop >= *start) 
	    || (*step > 0 && *start >= *stop)) {
		*slicelength = 0;
	}
	else if (*step < 0) {
		*slicelength = (*stop-*start+1)/(*step)+1;
	}
	else {
		*slicelength = (*stop-*start-1)/(*step)+1;
	}

	return 0;
}

static int 
_snip( int narr, maybelong *arr, int i)
{
	int j;
	-- narr;
	for(j=i; j<narr; j++)
		arr[j] = arr[j+1];
	return narr;		
}


static PyObject *
_slicedIndexing0(PyArrayObject *self, PyObject *key, PyObject *value, int dim)
{
	PyObject *result=NULL;
	int keylen = PyList_Size(key);
	if (keylen < 0) return NULL;

	if (keylen == 0) {
		if (!NA_updateDataPtr(self)) {
			return NULL;
		}
		if (value == Py_None) { /* get slice */
			NA_updateStatus(self);
			result = (PyObject *) self;
		} else {                /* set slice */
			if (self->nd == 0)
				result = _simpleIndexing(self, key, value);
			else {
				result = PyObject_CallMethod(
					(PyObject *) self, "_copyFrom", "(O)", value);
			}
			Py_DECREF(self);
		}
	} else {
		PyObject *slice, *rest;
		slice = PyList_GetItem(key, 0);
		if (!slice) return NULL;
		rest = PyList_GetSlice(key, 1, keylen);
		if (!rest) return NULL;
		if (PyInt_Check(slice)) {
			long islice = PyInt_AsLong(slice);
			if (islice < 0)
				islice += self->dimensions[dim];
			if ((islice < 0) || (islice >= self->dimensions[dim])) {
				Py_DECREF(rest);
				return PyErr_Format(
					PyExc_IndexError, "Index out of range");
			}
			self->byteoffset += islice * self->strides[dim];
			self->nd       = _snip( self->nd, self->dimensions, dim);
			self->nstrides = _snip( self->nstrides, self->strides, dim);
		} else if (PySlice_Check(slice)) {
			int start, stop, step, strided;
			if (_GetIndicesEx( (PySliceObject *) slice, 
					   self->dimensions[dim], 
					   &start, &stop, &step, &strided) < 0)
				return NULL;
			if ((strided == 0) && (value != Py_None)) { 
                            /* zero length setslice */
				Py_DECREF(self);
				Py_DECREF(rest);
				Py_INCREF(Py_None);
				return Py_None;
			}
			self->byteoffset += self->strides[dim] * start;
			self->dimensions[dim] = strided;
			self->strides[dim] *= step;
			++ dim;
		} else {
			Py_DECREF(rest);
			return PyErr_Format(
				PyExc_TypeError, 
				"_ndarray._slicedIndexing0: bad key item");
		}
		result = _slicedIndexing0( self, rest, value, dim);
		Py_DECREF(rest);
	}
	return result;
}

int _splice(int narr, maybelong *arr, maybelong new, int i)
{
	int j;
	for(j=narr; j>i; j--)
		arr[j] = arr[j-1];
	arr[i] = new;
	return narr+1;
}

static PyObject *
_slicedIndexing(PyArrayObject *self, PyObject *key, PyObject *value)
{
	PyArrayObject *result;
	int i, indexed, keylen, keylen2;

	result = (PyArrayObject *) PyObject_CallMethod(
		(PyObject *) self, "view", NULL);
	if (!result) goto _fail;

	keylen = PyList_Size(key);
	if (keylen < 0) goto _fail;

	/* Replace the first Ellipsis with the missing dimensions of the key,
	   compensating for any NewAxis elements in the key. */	
	for(i=indexed=0; i<keylen; i++) {
		PyObject *keyi = PyList_GetItem(key, i);
		if (!keyi) return NULL;
		if (PyInt_Check(keyi) || PyLong_Check(keyi) || PySlice_Check(keyi))
			++ indexed;
		else if (keyi == Py_Ellipsis) {
			int newdims, j, non_new = 0;
			PyObject *FullSliceList;
			/* NewAxis in key remainder messes up dimension count */
			for (j=i+1; j<keylen; j++) {
				PyObject *k = PyList_GetItem(key, j);
				if (k != Py_None)
					++ non_new;
			}
			newdims = self->nd - indexed - non_new;
			FullSliceList = PyList_New(newdims);
			if (!FullSliceList) goto _fail;
			for(j=0; j<newdims; j++) {
				Py_INCREF( FullSlice );
				if (PyList_SetItem(
					    FullSliceList, j, FullSlice) < 0) {
					Py_DECREF(FullSliceList);
					goto _fail;
				}
			}
			if (PyList_SetSlice(key, i, i+1, FullSliceList) < 0) {
				Py_DECREF(FullSliceList);
				goto _fail;
			}
			Py_DECREF(FullSliceList);
			break;
		}
	}

	keylen = PyList_Size(key);
	if (keylen < 0) goto _fail;

	/* Replace Ellipsis after the first with a single FullSlice. */
	for( ; i<keylen; i++) {
		PyObject *keyi = PyList_GetItem(key, i);
		if (!keyi) goto _fail;
		if (keyi == Py_Ellipsis) {
			Py_INCREF(FullSlice);
			if (PyList_SetItem(key, i, FullSlice) < 0)
				goto _fail;
		}
	}

	/* Insert any NewAxis into array shape and strides.  Replace the 
	   NewAxis key element with a single step slice */
	keylen2 = keylen;
	for(i=0; i<keylen; i++) {
		PyObject *keyi;
		keyi = PyList_GetItem(key, i);
		if (!keyi) goto _fail;
		if (keyi == Py_None) {  /* NewAxis == None */
			int which;
			Py_INCREF(NewAxisSlice);
			if (PyList_SetItem(key, i, NewAxisSlice) < 0)
				goto _fail;
			if (i > 0)
				which = i - 1;
			else
				which = 0;
			result->nstrides = _splice( 
				result->nstrides, result->strides, 
				result->strides[which], i);
			result->nd = _splice( 
				result->nd, result->dimensions, 
				1, i);
			-- keylen2;
		}
	}	

	if (keylen2 > self->nd) {
		PyErr_Format(PyExc_IndexError, 
			     "too many slices for array shape");
		goto _fail;
	}

	return _slicedIndexing0(result, key, value, 0);
  _fail:
	Py_XDECREF(result);
	return NULL;
}

static int
_isSlice(PyObject *key)
{
	int i, keylen = PyList_Size(key);
	for(i=0; i<keylen; i++) {
		PyObject *item = PyList_GetItem(key, i);
		if (!item) return -1;
		if (NA_NumArrayCheck(item) || PyList_Check(item)) {
			return 0;
		}
		if (!(PyInt_Check(item) || (item == Py_None) || 
		      PySlice_Check(item) || (item = Py_Ellipsis))) {
			PyErr_Format(PyExc_IndexError, "index is not of legal form");
			return -1;
		}
	}
	return 1;
}

static PyObject *
_universalIndexing(PyArrayObject *self, PyObject *key, PyObject *value)
{
	PyObject *fvalue=NULL, *tkey=NULL, *tkey2=NULL, *result=NULL;

	/* Do this once for the sake of simplicity; accept slow down for already
	   slow code like slicing or array indexing. */
	if (!NA_updateDataPtr(self))
		goto _exit;

	if (PyInt_Check(key) && self->nd == 1) {
		long offset, index = PyInt_AsLong(key);
		if (index < 0) index += self->dimensions[0];
		if ((index < 0) || (index >= self->dimensions[0])) {
			PyErr_Format(PyExc_IndexError, "Index out of range");
			goto _exit;
		}
		offset = self->strides[0]*index + self->byteoffset;
		if (value == Py_None) {
			result = _ndarray_getitem(self, offset);
		} else {
			if (_ndarray_setitem(self, offset, value) == 0) {
				result = Py_None;
				Py_INCREF(result);
			}
		}
		goto _exit;
	}


	if (PyTuple_Check(value) || PyList_Check(value)) {
		fvalue = PyObject_CallMethod( (PyObject*) self, 
					      "factory", "(O)", value);
		if (!fvalue) goto _exit;
	} else {
		Py_INCREF(value);
		fvalue = value;
	}

	if (PySlice_Check(key) || (key == Py_Ellipsis) || 
	    PyInt_Check(key) || PyLong_Check(key) ||
	    NA_NumArrayCheck(key)) {
		tkey = Py_BuildValue("(O)" , key);
	} else if (PyList_Check(key)) {
		PyObject *key0 = PyList_GetItem(key, 0);
		if (!key0) goto _exit;
		if (PySlice_Check(key0)) {
			tkey = PyObject_CallFunction(
				(PyObject *) &PyTuple_Type, "(O)", key);
			if (!tkey) goto _exit;
		} else {
			tkey = Py_BuildValue("(O)", key);
			if (!tkey) goto _exit;
		}
	} else {
		Py_INCREF(key);
		tkey = key;
	}

	if (!PyTuple_Check(tkey)) {
		PyErr_Format(PyExc_IndexError, "Illegal index");
		goto _exit;
	}

	tkey2 = PyObject_CallFunction((PyObject *) &PyList_Type, "(O)", tkey);
	if (!tkey2) goto _exit;
	
	if (NA_isIntegerSequence(tkey2)) {
#if _PYTHON_CALLBACKS
		result = PyObject_CallMethod( 
			(PyObject *) self, "_simpleIndexing", "(OO)", 
			tkey2, fvalue);
#else
		result = _simpleIndexing(self, tkey2, fvalue);
		
#endif
	} else {
		long isslice;

#if _PYTHON_CALLBACKS
		PyObject *ans = PyObject_CallMethod(
			(PyObject *) self, "_isSlice", "(O)", tkey2);
		if (!ans) goto _exit;
		
		if (!PyInt_Check(ans)) {
			PyErr_Format(PyExc_TypeError, 
				     "_isSlice returned non-int");
			Py_DECREF(ans);
			goto _exit;
		}
		isslice = PyInt_AsLong(ans);
		Py_DECREF(ans);
#else
		isslice = _isSlice(tkey2); 
		if (isslice < 0) goto _exit;
#endif
		if (isslice) {
#if _PYTHON_CALLBACKS
			result = PyObject_CallMethod(
				(PyObject *) self, "_slicedIndexing", "(OO)",
				tkey2, fvalue);
#else
			result = _slicedIndexing(self, tkey2, fvalue);
#endif
		} else {
			result = PyObject_CallMethod(
				(PyObject *) self, "_arrayIndexing", "(OO)",
				tkey2, fvalue);
		}
	}

  _exit:
	Py_XDECREF(fvalue);
	Py_XDECREF(tkey);
	Py_XDECREF(tkey2);
	return result;
}


static PyObject *
_ndarray_universalIndexing(PyArrayObject *self, 
			   PyObject  *args, PyObject *kwdict)
{
	PyObject *key, *value=Py_None;
	static char *keywds[] = {"key","value",NULL};
	if (!PyArg_ParseTupleAndKeywords(
		    args, kwdict, "O|O:_universalIndexing",
		    keywds, &key, &value) < 0)
		return NULL;
	
	return _universalIndexing(self, key, value);
}


static PyObject*
_ndarray_subscript(PyArrayObject* self, PyObject* key)
   
{
    PyObject *result;
    if (PyInt_CheckExact(key)) {
            maybelong ikey = PyInt_AsLong(key);
            long offset;
            if (NA_getByteOffset(self, 1, &ikey, &offset) < 0)
		    return NULL;
            if (!NA_updateDataPtr(self))
		    return NULL;
            return _simpleIndexingCore(self, offset, 1, Py_None);
    }
#if _PYTHON_CALLBACKS
    result = PyObject_CallMethod(
        (PyObject *) self, "_universalIndexing", "(OO)", key, Py_None);
#else
    result = _universalIndexing(self, key, Py_None);
#endif
    return result;
}

static int 
_ndarray_ass_subscript(PyArrayObject * self, PyObject* key, PyObject* value)
{
	PyObject *result;

	if (!value) {
		PyErr_Format(
			PyExc_TypeError, 
			"_ndarray_ass_subscript: can't delete elements from an array.");
		return -1;
	}

#if _PYTHON_CALLBACKS
	result = PyObject_CallMethod(
		(PyObject *) self, "_universalIndexing", "(OO)", key, value);
#else
	result = _universalIndexing(self, key, value);
#endif
	if (!result) {
		return -1;
	} else {
		Py_DECREF(result);
		return 0;
	}
}

static PyMappingMethods 
_ndarray_as_mapping = {
	(inquiry)_ndarray_length,
	(binaryfunc)_ndarray_subscript,
	(objobjargproc)_ndarray_ass_subscript
};


#if PY_VERSION_HEX >= 0x02030000 
static PyObject *
_ndarray_item(PyArrayObject *self, int i)
{
    long offset;
    maybelong mi = i;
    if (NA_getByteOffset(self, 1, &mi, &offset) < 0)
        return NULL;
    if (!NA_updateDataPtr(self))
        return NULL;
    return _simpleIndexingCore(self, offset, 1, Py_None);
}

static PySequenceMethods 
_ndarray_as_sequence = {
	(inquiry)_ndarray_length,		/* sq_length */
	(binaryfunc)0,             		/* sq_concat */
	(intargfunc)0,		                /* sq_repeat */
	(intargfunc)_ndarray_item,	        /* sq_item */
};
#endif

/* Beginning of methods added by Scott N. Gunyan
   to alloy export of array as a Python buffer object.

   ported from Numeric to numarray by Todd Miller
*/

#define NBYTES(a) (NA_elements(a) * a->itemsize)

static int array_getsegcount(PyArrayObject *self, int *lenp) {
    int i, elsize;
    int num_segments=1;

    if (lenp) 
	    *lenp = NBYTES(self);

    elsize = self->itemsize;  /* self->descr->elsize; */

    /* Check how self is CONTIGUOUS:  see if strides match 
       dimensions */
    for (i=self->nd-1; i>=0; i--) {
	if (self->strides[i] == elsize) {
	    elsize *= self->dimensions[i];
	} else {
	    break;
	}
    }
    if (i >= 0) {
        for (; i >= 0; i--) {
            num_segments *= self->dimensions[i];
	}
    }
    return num_segments;
}


/* this will probably need some work, could be faster */
int get_segment_pointer(PyArrayObject *self, int segment, int i) {
    if (i >= 0) {
    return ((segment%self->dimensions[i])*self->strides[i] +
            get_segment_pointer(self,segment/self->dimensions[i],i-1));
    } else {
        return 0;
    }
}



static int array_getreadbuf(PyArrayObject *self, int segment, void **ptrptr) {
    int num_segments, product=1, i=0;
    
    if ((segment < 0) || 
        (segment > (num_segments = array_getsegcount(self,NULL)))) {
        PyErr_SetString(PyExc_SystemError,
                        "Accessing non-existent array segment");
        return -1;
    }

    if (!PyArray_ISALIGNED(self) || PyArray_ISBYTESWAPPED(self)) {
	    PyErr_SetString(PyExc_ValueError, 
			    "Can't get buffer pointer from byteswapped or misaligned array.");
	    return -1;
    }
    
    if (num_segments > 1) {
        /* find where the break in contiguity is */    
        while ((product != num_segments) && (i < self->nd)) {
            product *= self->dimensions[i++];
        }
        /* now get the right segment pointer (i is indexing break in contiguity) */
        *ptrptr = self->data + get_segment_pointer(self,segment,i-1);
    } else {
        *ptrptr = self->data;
    }
    /* return the size in bytes of the segment (i is indexing break in contiguity) */
    return NBYTES(self);
}


static int array_getwritebuf(PyArrayObject *self, int segment, void **ptrptr) {
    if (!PyArray_ISWRITABLE(self)) {
	    PyErr_SetString(PyExc_ValueError,
			    "Can't get writable buffer from read-only array.");
	    return -1;
    }
    return array_getreadbuf(self, segment, (void **) ptrptr);
}

static int array_getcharbuf(PyArrayObject *self, int segment, const char **ptrptr) {
    if (self->descr->type_num == PyArray_CHAR)
        return array_getreadbuf(self, segment, (void **) ptrptr);
    else {
        PyErr_SetString(PyExc_TypeError, 
                        "Non-character array cannot be interpreted as character buffer.");
        return -1;
    }
}

static PyBufferProcs array_as_buffer = {
    (getreadbufferproc)array_getreadbuf,    /*bf_getreadbuffer*/
    (getwritebufferproc)array_getwritebuf,  /*bf_getwritebuffer*/
    (getsegcountproc)array_getsegcount,     /*bf_getsegcount*/
    (getcharbufferproc)array_getcharbuf,    /*bf_getcharbuffer*/
};
/* End methods added by Scott N. Gunyan for buffer extension */


static PyObject *
_ndarray_nelements(PyArrayObject *self, PyObject  *args)
{
	if (!PyArg_ParseTuple(args, ":nelements"))
		return NULL;
	return PyInt_FromLong(NA_elements(self));
}

static PyObject *
_ndarray_isaligned(PyArrayObject *self, PyObject  *args)
{
	if (!PyArg_ParseTuple(args, ":isaligned"))
		return NULL;
	if (!NA_updateDataPtr(self))
		return NULL;
	NA_updateAlignment(self);
	return PyInt_FromLong((self->flags & ALIGNED) != 0);
}
        
static PyObject *
_ndarray_iscontiguous(PyArrayObject *self, PyObject *args)
{
	if (!PyArg_ParseTuple(args, ":iscontiguous"))
		return NULL;
	return PyInt_FromLong((self->flags & CONTIGUOUS) != 0);
}

static PyObject *
_ndarray_is_fortran_contiguous(PyArrayObject *self, PyObject *args)
{
	if (!PyArg_ParseTuple(args, ":is_fortran_contiguous"))
		return NULL;
	NA_updateContiguous(self);
	return PyInt_FromLong((self->flags & FORTRAN_CONTIGUOUS) != 0);
}

static PyObject *
_x_view(PyArrayObject *self)
{
	PyArrayObject *view=NULL;
	PyObject *vd=NULL, *sd=NULL;
	int i;

	view = (PyArrayObject *) self->ob_type->tp_new(
		self->ob_type, NullArgs, NullKeywds);
	if (!view) goto _fail;

	if (self->flags & UPDATEDICT) {
		vd = PyObject_GetAttrString((PyObject *) view, "__dict__");
		if (!vd) goto _fail;		
		sd = PyObject_GetAttrString((PyObject *) self, "__dict__");
		if (!sd) goto _fail;	      
		if (PyDict_Update(vd, sd) < 0) goto _fail;
	}

	for(i=0; i<self->nd; i++)
		view->dimensions[i] = self->dimensions[i];
	view->nd = self->nd;
	for(i=0; i<self->nstrides; i++)
		view->strides[i] = self->strides[i];
	view->nstrides = self->nstrides;

	view->byteoffset = self->byteoffset;
	view->bytestride = self->bytestride;
	view->itemsize = self->itemsize;
	view->flags = self->flags;
	
	/* Numarray hack here; since fields are always present, copy them. */
	view->descr = self->descr;
	view->byteorder = self->byteorder;

	Py_XDECREF(view->_data);
	view->_data = self->_data;  
	Py_INCREF(self->_data);
	if (!NA_updateDataPtr(view)) 
		goto _fail;

	NA_updateStatus(view);
	goto _exit;

  _fail:
	Py_XDECREF(view);
	view = NULL;

  _exit:
	Py_XDECREF(vd);
	Py_XDECREF(sd);
	return (PyObject *) view;
}

static PyArrayObject *
_view(PyArrayObject *self)
{
	PyObject *result;
	if (NA_NumArrayCheck((PyObject *)self))		      
		result = _x_view(self);
	else
		result = PyObject_CallMethod((PyObject *) self,"view",NULL);
	return (PyArrayObject *) result;
}

static PyObject *
_ndarray_view(PyArrayObject *self, PyObject *args, PyObject *keywds)
{
	if (!PyArg_ParseTuple(args, ":view"))
		return NULL;
	return _x_view(self);
}

static PyObject *
_tolist_helper(PyArrayObject *self, int ndim, long offset)
{
	long i;
	PyObject *list;

	if (ndim == self->nd) {
		return _ndarray_getitem(self, offset);
	}
	list = PyList_New(self->dimensions[ndim]);
	if (!list) return NULL;
	for(i=0; i<self->dimensions[ndim]; i++) {
		long newoffset = offset+self->strides[ndim]*i;
		PyObject *sublist = _tolist_helper(self, ndim+1, newoffset);
		if (!sublist || (PyList_SetItem(list, i, sublist) < 0))
		{
			Py_DECREF(list);
			Py_XDECREF(sublist);
			return NULL;
		}
	}
	return list;
}

static PyObject *
tolist(PyObject *self, PyObject *args)
{
	PyArrayObject *me = (PyArrayObject *) self;
	if (!PyArg_ParseTuple(args, ":tolist"))
		return NULL;
	if (me->nd == 0) {
		return PyErr_Format(PyExc_ValueError, 
				    "rank-0 arrays don't convert to lists.");
	}
	if (!NA_updateDataPtr(me))
		return NULL;
	return _tolist_helper(me, 0, me->byteoffset);
}

static PyObject *
_ndarray_swapaxes(PyArrayObject *self, PyObject *args)
{
	int axis1, axis2;
	if (!PyArg_ParseTuple(args, "ii:swapaxes", &axis1, &axis2))
		return NULL;

	if (NA_swapAxes(self, axis1, axis2) < 0) {
		return NULL;
	}

	NA_updateStatus(self);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
_ravel(PyObject *self)
{
	PyArrayObject *me = (PyArrayObject *) self;
	NA_updateStatus(me);
	if (PyArray_ISCONTIGUOUS(me)) {
		me->dimensions[0] = NA_elements(me);
		me->strides[0] = me->bytestride;
		me->nstrides = me->nd = 1;
		return (PyObject *) me;
	} else {
		return PyErr_Format(PyExc_ValueError,
				    "Can't reshape non-contiguous numarray.");
	}
}

static PyObject *
_ndarray_ravel(PyObject *self, PyObject *args)
{
	if (!PyArg_ParseTuple(args, ":ravel"))
		return NULL;
	if (!_ravel(self))
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}

/*  

See generic._takeShape to see where result.shape comes from and how it is
related to self.shape and indices.shape.

Indices is essentially a tuple of index arrays converted to a single array
where the last dimension contains one key per row.  Thus, _taker recurses over
the outer dimensions of indices and result, and treats the last dimension of
indices as a key into self.

*/

static int 
_taker(PyArrayObject *self, int dim,
       PyArrayObject *indices, long indoff,
       PyArrayObject *result, long resoff)
{
	int i, dimlen = indices->dimensions[dim];
	if (dim == indices->nd - 1) {
		PyObject *obj, *ans;
		long selfoff;
		char *index = (char *) indices->data;
		index += indoff;
		if (NA_getByteOffset(
			    self, dimlen, (maybelong *) index, &selfoff) < 0)
			return -1;
		obj = _simpleIndexingCore(self, selfoff, dimlen, Py_None);
		if (!obj) return -1;
		ans = _simpleIndexingCore(result, resoff, dim, obj);
		Py_DECREF(obj);
		if (!ans) return -1;
		Py_DECREF(ans); /* Py_None */
	} else {		
		for(i=0; i<dimlen; i++) {
			if (_taker(self, dim+1, 
				   indices, indoff+indices->strides[dim]*i,
				   result,  resoff+result->strides[dim]*i) < 0)
				return -1;
		}
	}
	return 0;
}

static PyObject *
_pt_setup(PyArrayObject *self, PyArrayObject *indices, PyArrayObject *result)
{
	if (!NA_NDArrayCheck((PyObject *) self) || 
	    !NA_NumArrayCheck((PyObject *)indices) ||
	    !NA_NDArrayCheck((PyObject *)result))
		return PyErr_Format(PyExc_TypeError, "args must be NDArrays");
	    
	if (!NA_updateDataPtr(self) || 
	    !NA_updateDataPtr(indices) ||
	    !NA_updateDataPtr(result))
		return NULL;
	return Py_None;
}

static PyObject *
_ndarray_taker(PyArrayObject *self, PyObject *args)
{
	PyArrayObject *indices, *result;
	if (!PyArg_ParseTuple(args, "OO:_ndarray_taker", &indices, &result))
		return NULL;

	if (!_pt_setup(self, indices, result))
		return NULL;

	if (_taker(self, 0, indices, 0, result, 0) < 0)
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}

static int 
_putter(PyArrayObject *self, int dim,
       PyArrayObject *indices, long indoff,
       PyArrayObject *values, long valoff)
{
	int i, dimlen = indices->dimensions[dim];
	if (dim == indices->nd - 1) {
		PyObject *obj, *ans;
		long selfoff;
		char *index = (char *) indices->data;
		index += indoff;
		if (NA_getByteOffset(
			    self, dimlen, (maybelong *) index, &selfoff) < 0)
			return -1;
		obj = _simpleIndexingCore(values, valoff+values->byteoffset, dim, Py_None);
		if (!obj) return -1;
		ans = _simpleIndexingCore(self, selfoff, dimlen, obj);
		Py_DECREF(obj);
		if (!ans) return -1;
		Py_DECREF(ans); /* Py_None */
	} else {		
		for(i=0; i<dimlen; i++) {
			if (_putter(self, dim+1, 
				   indices, indoff+indices->strides[dim]*i,
				   values,  valoff+values->strides[dim]*i) < 0)
				return -1;
		}
	}
	return 0;
}

static PyObject *
_ndarray_putter(PyArrayObject *self, PyObject *args)
{
	PyArrayObject *indices, *values;
	if (!PyArg_ParseTuple(args, "OO:_ndarray_putter", &indices, &values))
		return NULL;
	
	if (!_pt_setup(self, indices, values))
		return NULL;

	if (_putter(self, 0, indices, 0, values, 0) < 0)
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef 
_ndarray_methods[] = {
	{"_taker", (PyCFunction)_ndarray_taker, 
	 METH_VARARGS,
	   "_taker(axis1, axis2) -> None"},
	{"_putter", (PyCFunction)_ndarray_putter, 
	 METH_VARARGS,
	   "_putter(axis1, axis2) -> None"},
	{"swapaxes", (PyCFunction)_ndarray_swapaxes, 
	 METH_VARARGS,
	   "swapaxes(axis1, axis2) -> None"},
	{"_getByteOffset", (PyCFunction)_ndarray_getByteOffset, 
	 METH_VARARGS,
	   "_getByteOffset(indices) -> byte offset of indices within array"},
	{"_universalIndexing", (PyCFunction)_ndarray_universalIndexing, 
	 METH_VARARGS | METH_KEYWORDS,
	 "_universalIndexing(key, value=None) gets(value=None) or sets(value!=None) at key"},
	{"_simpleIndexing", (PyCFunction)_ndarray_simpleIndexing, 
	 METH_VARARGS | METH_KEYWORDS,
	 "_simpleIndexing(key, value=None) gets or sets for integer sequence key"},
	{"ravel", (PyCFunction) _ndarray_ravel, METH_VARARGS,
	 "ravel() ->  1D view of self."},
	{"tolist", tolist, METH_VARARGS,
	 "tolist() ->  array converted to nested list of scalars."},
	{"nelements", (PyCFunction)_ndarray_nelements, METH_VARARGS,
	   "nelements() -> total number of elements in array."},
	{"isaligned", (PyCFunction)_ndarray_isaligned, METH_VARARGS,
	   "isaligned() -> 1 if array is aligned, 0 otherwise."},
	{"iscontiguous", (PyCFunction)_ndarray_iscontiguous, METH_VARARGS,
	   "iscontiguous() -> 1 if array is contiguous, 0 otherwise."},
	{"is_fortran_contiguous", (PyCFunction)_ndarray_is_fortran_contiguous, 
	 METH_VARARGS,
	   "is_fortran_contiguous() -> 1 if array is fortran-contiguous, 0 otherwise."},
	{"view", (PyCFunction)_ndarray_view, METH_VARARGS | METH_KEYWORDS,
	   "view() -> new array object referring to same data buffer"},
	{NULL,	NULL},
};

static char _ndarray_tpdoc[] = 
"_ndarray is a private C baseclass used in the implementation of NDArray.\n"
"\n"
"_ndarray(shape=None, itemsize=0, buffer=None, byteoffset=0, bytestride=0, aligned=1)\n"
"\n"
"shape               The shape of the array.\n"
"\n"
"itemsize            The size in bytes of a single element.\n"
"\n"
"buffer              An object meeting the Python buffer protocol which will\n"
"                    store the array data.  If None is specified,  a memory \n"
"                    oject is created.\n"
"\n"
"byteoffset          The offset in bytes from the base of buffer to the array data.\n"
"\n"
"bytestride          The distance in bytes between elements.  Defaults to itemsize.\n"
"\n"
"aligned             A flag describing whether buffer+offset is aligned for itemsize.\n";

static PyTypeObject 
_ndarray_type = {
	PyObject_HEAD_INIT(DEFERRED_ADDRESS(&PyType_Type))
	0,
	"numarray._ndarray._ndarray",
	sizeof(PyArrayObject),
	0,
	_ndarray_dealloc,	                /* tp_dealloc */
	0,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	0,					/* tp_repr */
	0,					/* tp_as_number */
#if (PY_VERSION_HEX >= 0x02030000)
	&_ndarray_as_sequence,                  /* tp_as_sequence */
#else
	0,                                      /* tp_as_sequence */
#endif
	&_ndarray_as_mapping,			/* tp_as_mapping */
	0,					/* tp_hash */
	0,					/* tp_call */
	0,					/* tp_str */
	0,					/* tp_getattro */
	0,					/* tp_setattro */
	&array_as_buffer,			/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | 
        Py_TPFLAGS_BASETYPE,                    /* tp_flags */
	_ndarray_tpdoc, 			/* tp_doc */
	0,					/* tp_traverse */
	0,					/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	_ndarray_methods,			/* tp_methods */
	0,					/* tp_members */
	_ndarray_getsets,			/* tp_getset */
	0,                                      /* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	(initproc)_ndarray_init,		/* tp_init */
	0,      	                        /* tp_alloc */
	_ndarray_new,				/* tp_new */
	0,                                      /* tp_free */
};

static PyObject *
_ndarray_isIntegerSequence(PyObject *module, PyObject *args)
{
	PyObject *sequence;
	if (!PyArg_ParseTuple(args, "O:isIntegerSequence", &sequence))
		return NULL;
	return PyInt_FromLong(NA_isIntegerSequence(sequence));
}

static PyObject *
_ndarray_product(PyObject *module, PyObject *args)
{
	PyObject *sequence;
	long prod;
	if (!PyArg_ParseTuple(args, "O:product", &sequence))
		return NULL;
	if (NA_intTupleProduct(sequence, &prod) < 0)
		return NULL;
	return Py_BuildValue("i", prod);
}

static PyObject *
_ndarray_lp64(PyObject *module, PyObject *args)
{
	if (!PyArg_ParseTuple(args, ":lp64"))
		return NULL;
	return PyInt_FromLong(LP64);
}

static PyObject *
_ndarray_hasUInt64(PyObject *module, PyObject *args)
{
	if (!PyArg_ParseTuple(args, ":hasUInt64"))
		return NULL;
	return PyInt_FromLong(HAS_UINT64);
}


static PyObject *
_ndarray_is_buffer(PyObject *module, PyObject *args)
{
	PyObject *o;
	int ans;
	if (!PyArg_ParseTuple(args, "O:is_buffer", &o))
		return NULL;
	if ((ans = isBuffer(o)) < 0) 
		return NULL;
	return PyInt_FromLong(ans);
}

PyObject *
_ndarray_ravel_function(PyObject *module, PyObject *args)
{
	PyObject *o, *b;
	PyArrayObject *a;
	if (!PyArg_ParseTuple(args, "O:ravel", &o))
		return NULL;
	if (NA_NDArrayCheck(o)) {
		a = _view((PyArrayObject *)o);
	} else {
		a = NA_InputArray(o, tAny, 0);
	}
	if (!a) return NULL;
	NA_updateStatus(a);
	if (!PyArray_ISCONTIGUOUS(a)) {
		b = (PyObject *) NA_copy(a);
		if (!b) return NULL;
		Py_DECREF(a);
		a = (PyArrayObject *) b;
	}
	if (!_ravel((PyObject *) a)) return NULL;
	return (PyObject *) a;
}

static PyMethodDef 
_ndarray_functions[] = {
	{"_isIntegerSequence", (PyCFunction) _ndarray_isIntegerSequence, METH_VARARGS,
	 "_isIntegerSequence(s) returns 1 if all elements of sequence s are integers, else 0"},
	{"ravel", (PyCFunction) _ndarray_ravel_function, METH_VARARGS,
	 "ravel(x) converts x to an array and either returns a flattened view or copy as needed."},
	{"product", (PyCFunction) _ndarray_product, METH_VARARGS,
	 "product(t)   returns the product of all elements of an integer sequence"},
	{"lp64", (PyCFunction) _ndarray_lp64, METH_VARARGS,
	 "lp64()  returns flag indicating whether this system and compiler use the 64-bit LP64 model."},
	{"hasUInt64", (PyCFunction) _ndarray_hasUInt64, METH_VARARGS,
	 "hasUInt64()  returns flag indicating whether this system has 64-bit unsigned integer support."},
	{"is_buffer", (PyCFunction) _ndarray_is_buffer, METH_VARARGS,
	 "is_buffer(o)  returns 1 iff 'o' should work as a buffer."},
	{NULL,		NULL}		/* sentinel */
};

#define ALIGN_2  offsetof(struct { char c;    Int16 v; }, v)
#define ALIGN_4  offsetof(struct { char c;    Int32 v; }, v)
#define ALIGN_8  offsetof(struct { char c;  Float64 v; }, v)

#if HAS_FLOAT128
#define ALIGN_16 offsetof(struct { char c; Float128 v; }, v)
#else
#define ALIGN_16 ALIGN_8
#endif

DL_EXPORT(void)
init_ndarray(void)
{
	PyObject *m;

	_ndarray_type.tp_alloc = PyType_GenericAlloc;

	if (PyType_Ready(&_ndarray_type) < 0)
		return;

	m = Py_InitModule3("_ndarray",
			   _ndarray_functions,
			   _ndarray__doc__);
	if (m == NULL)
		return;

	Py_INCREF(&_ndarray_type);
	if (PyModule_AddObject(m, "_ndarray",
			       (PyObject *) &_ndarray_type) < 0)
		return;

	_alignment = Py_BuildValue("{i:i,i:i,i:i,i:i,i:i}",
					1,1, 2,ALIGN_2, 4,ALIGN_4, 8,ALIGN_8, 16,ALIGN_16);
	if (PyModule_AddObject(m, "_alignment", _alignment) < 0)
		return;

	ADD_VERSION(m);

	import_libnumarray();

	pNewMemoryFunc = NA_initModuleGlobal("numarray.memory","new_memory");
	if (!pNewMemoryFunc)
		PyErr_Format(
			PyExc_ImportError, "init_ndarray: can't find memory.new_memory");
	
	one = PyInt_FromLong( 1 );
	zero = PyInt_FromLong( 0 );
	if (!one || !zero) return;
	Py_INCREF(one);
	NewAxisSlice = PySlice_New(zero, one, one);			
	if (!NewAxisSlice) return;
	FullSlice = PySlice_New(NULL,NULL,NULL);
	if (!FullSlice) return;
	NullArgs = PyTuple_New(0);
	if (!NullArgs) return;
	NullKeywds = PyDict_New();
	if (!NullKeywds) return;

}

