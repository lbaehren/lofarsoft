#include <stdio.h>
#include "arrayobject.h"
#include "libnumarray.h"

static void
libnumeric_init(void) 
{
	import_libnumarray();
}

static PyArray_Descr *
PyArray_DescrFromType(int type) { 
	return NA_DescrFromType(type); 
}

static PyObject *
PyArray_FromDimsAndDataAndDescr(int nd, int *d, PyArray_Descr *descr, char *data) 
{
	maybelong i, shape[MAXDIM];
	if (nd > MAXDIM) return PyErr_Format(PyExc_ValueError, "too many dimensions: %d", nd);
	for (i=0; i<nd; i++) shape[i] = d[i];
	return (PyObject *) NA_FromDimsStridesDescrAndData(nd, shape, NULL, descr, data);
}

static PyObject *
PyArray_FromDimsAndData(int nd, int *d, int type, char *data) 
{
    PyArray_Descr *descr = PyArray_DescrFromType(type);
    return PyArray_FromDimsAndDataAndDescr(nd, d, descr, data);
}

static PyObject *
PyArray_FromDims(int nd, int *d, int type) 
{
    PyArray_Descr *descr = PyArray_DescrFromType(type);
    return PyArray_FromDimsAndDataAndDescr(nd, d, descr, NULL);
}


/*
PyObject *PyArray_ContiguousFromObject(op, type, min_dim, max_dim)

Returns a contiguous numarray of 'type' created from the sequence object 'op'.
If 'op' is a contiguous, aligned, non-byteswapped numarray of the correct
'type' and dimensions, then the op is INCREF'ed rather than copied.

Otherwise a new numarray will be created from 'op' and returned.

min_dim and max_dim bound the expected rank as in Numeric.

min_dim==max_dim specifies an exact rank.  min_dim==max_dim==0 specifies *any*
rank.
*/

static PyObject *
PyArray_ContiguousFromObject(PyObject  *op, int type, int min_dim, int max_dim)
{
	PyArrayObject *num = NA_IoArray(op, type, C_ARRAY);
	PyObject *num2 = (PyObject *) num;
	if (num && ((min_dim && (num->nd < min_dim)) || 
		    (max_dim && (num->nd > max_dim)))) {
		Py_DECREF(num);
		num2 = PyErr_Format(PyExc_ValueError,
				    "PyArray_ContiguousFromObject: array rank:%d "
				    "but required rank between %d and %d.",
				    (int) num->nd, min_dim, max_dim);
	}
	return num2;
}

/*
PyObject *PyArray_CopyFromObject(op,type,min_dim,max_dim)

Returns a contiguous array, similar to PyArray_FromContiguousObject, but
always returning a new array representation of the original sequence.

*/
static PyObject *
PyArray_CopyFromObject(PyObject  *op, int type, int min_dim, int max_dim)
{
	PyArrayObject *num = NA_InputArray(op, type, C_ARRAY);
	PyObject *num2 = (PyObject *) num;
	if (num && 
	    ((min_dim && (num->nd < min_dim)) || 
	     (max_dim && (num->nd > max_dim)))) {
		Py_DECREF(num);
		num2 = PyErr_Format(PyExc_ValueError,
				    "PyArray_CopyFromObject: array rank:%d"
				    "but required rank between %d and %d.",
				    (int) num->nd, min_dim, max_dim);
	}
	if (num2) {
		if (op == num2) {
			num2 = PyObject_CallMethod(op, "copy", NULL);
			if (!num2) goto _exit;
			/* Clear extra refcount from NA_InputArray. */
			Py_DECREF(num); 
		} else {  /* well-behaved temp becomes permanent copy */
			/* Detach from original since we want a copy, not a proxy. */
			if (num->_shadows) {
				Py_DECREF(num->_shadows);
				num->_shadows = NULL;
			}
		}
	} 
_exit:
	return num2;
}

/*
PyObject *PyArray_FromObject(op, type, min_dim, max_dim)

returns an array object based on 'op', possibly discontiguous.  The
strides array must be used to access elements of the array.

If 'op' is a byteswapped or misaligned numarray, FromObject creates a
temporary copy and the array object refers to it.

If 'op' is a nonswapped, aligned numarray, the emulation object refers to it.

If 'op' is some other sequence, it is converted to a numarray and the
array object refers to that.
*/
static PyObject *
PyArray_FromObject(PyObject  *op, int type, int min_dim, int max_dim)
{
	PyArrayObject *num = NA_IoArray(op, type, NUM_NOTSWAPPED | NUM_ALIGNED);
	if (!num) return NULL;
	if (((!min_dim || (num->nd >= min_dim)) &&
	     (!max_dim || (num->nd <= max_dim)))) {
		return (PyObject *) num;
	} 
	Py_DECREF(num);
	return PyErr_Format(PyExc_ValueError,
			    "PyArray_FromObject: array rank:%d "
			    " but required rank between %d and %d.",
			    (int) num->nd, min_dim, max_dim);
}

/* PyObject *PyArray_Return(apr)

Returns array object 'apr' to python.  In the case of "misbehaved" numarray,
apr must be a "well-behaved" array referring to the misbehaved original
via its _shadows attribute.

An additional check is (or should be) performed to guarantee that rank-0
numarray are converted to appropriate python scalars.

PyArray_Return has no net effect on the reference count of the underlying
numarray.
*/
static PyObject *
PyArray_Return(PyArrayObject *a)
{
	if (a->nd == 0) {
		PyObject *rval = NA_getPythonScalar(a, 0);
		Py_DECREF(a);
		return rval;
	}
	if (a->_shadows) {
		PyObject *original = a->_shadows;
		/* Py_INCREF(original); */
		Py_DECREF(a);
		return original;
	}
	return (PyObject *) a;
}

static int 
PyArray_Check(PyObject *op) {
	return NA_NumArrayCheck(op);
}

static int 
PyArray_Size(PyObject *op) {
	PyArrayObject *a = (PyArrayObject *) op;
	int i, size;
	if (PyArray_Check(op)) {
		for(i=0, size=1; i<a->nd; i++)
			size *= a->dimensions[i];
		return size;
	} else {
		return 0;
	}
}

static int 
PyArray_CopyArray(PyArrayObject *a, PyArrayObject *b)
{
	PyObject *r;
	int rval;
	r = PyObject_CallMethod((PyObject *) a, "_copyFrom", "O", b);
	if (r) {
		Py_DECREF(r);
		rval = 0;		
	} else {
		rval = -1;
	}
	return rval;
}

static PyObject *
PyArray_Copy(PyArrayObject *a) {
	int i, dims[MAXDIM];
	PyObject *ret;
	for(i=0; i<MAXDIM; i++) {
		dims[i] = (int) a->dimensions[i];
	}
	ret = PyArray_FromDims(a->nd, dims, a->descr->type_num);
	if (ret && PyArray_CopyArray((PyArrayObject *) ret, a) == -1) {
		Py_XDECREF( ret );
		return NULL;
	}
	return ret;
}

/* PyArray_Cast:  see newarray.ch  */

static PyObject *
PyArray_GetItem(PyArrayObject *a, char *where)
{
	long offset = (where - a->data) - a->byteoffset;
	return NA_getPythonScalar( a, offset );
}

static int
PyArray_SetItem(PyArrayObject *a, char *where, PyObject *what)
{
	long offset = (where - a->data) - a->byteoffset;
	return NA_setFromPythonScalar( a, offset, what );
}

static char *
PyArray_Zero(PyArrayObject *a)
{
	static Bool zBool = 0;
	static Int8 zInt8 = 0;
	static UInt8 zUInt8 = 0;
	static Int16 zInt16 = 0;
	static UInt16 zUInt16 = 0;
	static Int32  zInt32 = 0;
	static UInt32 zUInt32 = 0;
	static Int64  zInt64 = 0;
	static UInt64 zUInt64 = 0;
	static Float32 zFloat32 = 0.0;
	static Float64 zFloat64 = 0.0;
	static Complex32 zComplex32 = { 0, 0 };
	static Complex64 zComplex64 = { 0, 0 }; 

	char *rval;

	switch(a->descr->type_num) {
	case tBool:      rval = (char *) &zBool; break;
	case tInt8:      rval = (char *) &zInt8; break;
	case tUInt8:     rval = (char *) &zUInt8; break;
	case tInt16:     rval = (char *) &zInt16; break;
	case tUInt16:    rval = (char *) &zUInt16; break;
	case tInt32:     rval = (char *) &zInt32; break;
	case tUInt32:    rval = (char *) &zUInt32; break;
	case tInt64:     rval = (char *) &zInt64; break;
	case tUInt64:    rval = (char *) &zUInt64; break;
	case tFloat32:   rval = (char *) &zFloat32; break;
	case tFloat64:   rval = (char *) &zFloat64; break;
	case tComplex32: rval = (char *) &zComplex32; break;
	case tComplex64: rval = (char *) &zComplex64; break;
	default:
		PyErr_Format( PyExc_TypeError, 
			      "Unknown type %d in PyArray_Zero", 
			      a->descr->type_num);
		rval =  NULL;
	}
	return rval;
}

static char *
PyArray_One(PyArrayObject *a)
{
	static Bool zBool = 1;
	static Int8 zInt8 = 1;
	static UInt8 zUInt8 = 1;
	static Int16 zInt16 = 1;
	static UInt16 zUInt16 = 1;
	static Int32  zInt32 = 1;
	static UInt32 zUInt32 = 1;
	static Int64  zInt64 = 1;
	static UInt64 zUInt64 = 1;
	static Float32 zFloat32 = 1.0;
	static Float64 zFloat64 = 1.0;
	static Complex32 zComplex32 = { 1, 0 };
	static Complex64 zComplex64 = { 1, 0 }; 

	char *rval;

	switch(a->descr->type_num) {
	case tBool:      rval = (char *) &zBool; break;
	case tInt8:      rval = (char *) &zInt8; break;
	case tUInt8:     rval = (char *) &zUInt8; break;
	case tInt16:     rval = (char *) &zInt16; break;
	case tUInt16:    rval = (char *) &zUInt16; break;
	case tInt32:     rval = (char *) &zInt32; break;
	case tUInt32:    rval = (char *) &zUInt32; break;
	case tInt64:     rval = (char *) &zInt64; break;
	case tUInt64:    rval = (char *) &zUInt64; break;
	case tFloat32:   rval = (char *) &zFloat32; break;
	case tFloat64:   rval = (char *) &zFloat64; break;
	case tComplex32: rval = (char *) &zComplex32; break;
	case tComplex64: rval = (char *) &zComplex64; break;
	default:
		PyErr_Format( PyExc_TypeError, 
			      "Unknown type %d in PyArray_Zero", 
			      a->descr->type_num);
		rval =  NULL;
	}

	return rval;
}

static int 
PyArray_CanCastSafely(int fromtype, int totype) 
{
    if (fromtype == totype) return 1;

    switch(fromtype) {
    case PyArray_CHAR:
        return 0;
#if PyArray_CHAR != PyArray_UBYTE
    case PyArray_UBYTE:
#else
    case PyArray_SBYTE:
#endif
    case PyArray_SHORT:
        return (totype >= fromtype);
    case PyArray_INT:
        return (totype >= PyArray_INT) && (totype != PyArray_FLOAT);
#if LP64                 /* Else PyArray_LONG == PyArray_INT */
    case PyArray_LONG:   
	    return 0;    /* 64-bit ints lose precision no matter what */
#endif
    case PyArray_FLOAT:
        return (totype > PyArray_FLOAT);
    case PyArray_DOUBLE:
        return (totype == PyArray_CDOUBLE);
    case PyArray_CFLOAT:
        return (totype == PyArray_CDOUBLE);
    case PyArray_CDOUBLE:
        return 0;
    default:
        return 0;
    }
}

static int
PyArray_ObjectType (PyObject *op, int minimum_type) 
{ 
	int result;
	PyObject *ip;

	if (NA_NumArrayCheck(op))
		return MAX( ((PyArrayObject *)op)->descr->type_num, minimum_type);

	if (PyObject_HasAttrString(op, "__array__")) {
		ip = PyObject_CallMethod(op, "__array__", NULL);
		if(!ip) return -1;
		result = MAX(minimum_type, (int)((PyArrayObject *)ip)->descr->type_num);
		Py_DECREF(ip);
		return result;
	}

	if (PySequence_Check(op)) {
		long l = PyObject_Length(op);
		if (l < 0) {
			PyErr_Format(PyExc_RuntimeError, "PyArray_ObjectType: sequence length error.");
			return -1;
		}
		if (l == 0 && minimum_type == 0) 
			minimum_type = PyArray_LONG;
		while (--l >= 0) {
			ip = PySequence_GetItem(op, l);
			minimum_type = PyArray_ObjectType(ip, minimum_type);
			Py_DECREF(ip);
		}
		return minimum_type;
	}
	
	if (PyInt_Check(op)) 
		return MAX(minimum_type, (int) PyArray_LONG);
	if (PyFloat_Check(op)) 
		return MAX(minimum_type, PyArray_DOUBLE);
	if (PyComplex_Check(op))
	    return MAX(minimum_type, PyArray_CDOUBLE);
	
	PyErr_Format(PyExc_TypeError, "PyArray_ObjectType: unknown type");
	return -1;
}

static int 
_PyArray_multiply_list(int *l1, int n) {
    int s=1, i=0;
    while (i < n) s *= l1[i++];
    return s;
}

static int 
_PyArray_compare_lists(int *l1, int *l2, int n) {
    int i;
    for(i=0;i<n;i++) {
	if (l1[i] != l2[i]) return 0;
    } 
    return 1;
}

static PyObject *
PyArray_Cast(PyArrayObject *a, int type) { return NA_Cast(a, type); }

static int 
PyArray_Converter(PyObject *object, PyObject **address) {
    if (PyArray_Check(object)) {
        *address = object;
        return 1;
    }
    else {
        PyErr_SetString(PyExc_TypeError,"expected Array object in one of the arguments");
        return 0;
    }
}

static int 
PyArray_ValidType(int type) {
    switch(type) {
    case 'c':
    case 'b':
    case 'B':  /* Bool */
    case '1':
    case 's':
    case 'w':
    case 'i':
    case 'N':  /* Int64 */
    case 'u':
    case 'U':  /* UInt64 */
    case 'l':
    case 'f':
    case 'd':
    case 'F':
    case 'D':
	    /* case 'O': */
        break;
    default:
	    return 0;
    }
    return 1;
}

static int       
XXX_PyArray_SetNumericOps(PyObject *a) { return -1; }

static PyObject * 
XXX_PyArrayError(void) { return NULL; }

static void 
XXX_PyArray_SetStringFunction (PyObject *op, int repr) {}

static PyObject *
XXX_PyArray_FromDimsAndDataAndDescr (int nd, int *shape, PyArray_Descr *t, char *data) { return NULL; }

static int
XXX_PyArray_INCREF(PyArrayObject *a)
{
	return -1;
}

static int 
XXX_PyArray_XDECREF(PyArrayObject *a)
{
	return -1;
}

static PyObject *
PyArray_Type (void) { return NULL; }


/* ===================================================================================== */
/*  The code below is directly from Numeric... possibly with minor tweaks.               */
/* ===================================================================================== */

static int 
PyArray_As1D(PyObject **op, char **ptr, int *d1, int typecode) {
    PyArrayObject *ap;
	
    if ((ap = (PyArrayObject *)PyArray_ContiguousFromObject(*op, typecode, 1,
							    1)) == NULL)
	return -1;
	
    *op = (PyObject *)ap;
    *ptr = ap->data;
    *d1 = ap->dimensions[0];
    return 0;
}

static int 
PyArray_As2D(PyObject **op, char ***ptr, int *d1, int *d2, int typecode) {
    PyArrayObject *ap;
    int i, n;
    char **data;
	
    if ((ap = (PyArrayObject *)PyArray_ContiguousFromObject(*op, typecode, 2, 2)) == NULL)
	return -1;
	
    n = ap->dimensions[0];
    data = (char **)malloc(n*sizeof(char *));
    for(i=0; i<n; i++) {
	data[i] = ap->data + i*ap->strides[0];
    }
    *op = (PyObject *)ap;
    *ptr = data;
    *d1 = ap->dimensions[0];
    *d2 = ap->dimensions[1];
    return 0;
}


static int 
PyArray_Free(PyObject *op, char *ptr) {
    PyArrayObject *ap = (PyArrayObject *)op;
    int rval = 0;
    if (ap->nd > 2) { rval = -1; goto _exit ; }
    if ((ap->nd == 2) && ptr) {
	    free(ptr);
    }
    Py_DECREF(ap);
  _exit:
    return rval;
}

static int mxx ( int * i , int len)
{
	/* find the index of the maximum element of an integer array */
	int j, mx = 0, max = i[0];
	for (j=1; j<len; j++) {
		if (i[j] > max)	{
			max = i[j];
			mx = j;
		}
	}
	return mx;
}

static int mnx ( int * i , int len)
{
	/* find the index of the minimum element of an integer array */
	int j, mn = 0, min = i[0];
	for (j=1; j<len; j++) {
		if (i[j] < min) {
			min = i[j];
			mn = j;
		}
	}
	return mn;
}

static char arr_histogram__doc__[] =
"histogram(list, weights=None)\n"
"     If weight is not present:\n"
"       histogram(list)[i] is the number of occurrences of i in list.\n"
"     If weight is present:\n"
"       histogram(list, weight)[i] is the sum of all weight [j]\n"
"     where list[j] == i.\n"
"     Both list and weights must be 1D.\n"
"     list must be non-negative.\n"
"     weights must be the same length as lst.\n";

static PyObject *
arr_histogram(PyObject *self, PyObject *args)
{
    /* histogram accepts one or two arguments. The first is an array
     * of non-negative integers and the second, if present, is an
     * array of weights, which must be promotable to double.
     * Call these arguments list and weight. Both must be one-
     * dimensional. len (weight) >= max (list) + 1.
     * If weight is not present:
     *   histogram (list) [i] is the number of occurrences of i in list.
     * If weight is present:
     *   histogram (list, weight) [i] is the sum of all weight [j]
     * where list [j] == i.                                              */
    /* self is not used */

    PyObject * list = NULL, * weight = NULL ;
    PyArrayObject *lst, *wts , *ans;
    int * numbers, *ians, len , mxi, mni, i, ans_size;
    double * weights , * dans ;

    if (!PyArg_ParseTuple(args, "O|O", &list, &weight))
	    return NULL;

    lst = (PyArrayObject *) PyArray_ContiguousFromObject(
	    list,PyArray_INT,1,1);
    if (!lst) return NULL;

    len = PyArray_Size((PyObject *)lst);
    numbers = (int *) lst->data;
    mxi = mxx (numbers, len);
    mni = mnx (numbers, len);

    if (numbers [mni] < 0) {
	    PyErr_Format(PyExc_ValueError,
			 "First argument of histogram must be nonnegative.");
	    Py_DECREF(lst);
	    return NULL;
    }
    ans_size = numbers [mxi] + 1 ;

    if (weight == NULL)	{
	    ans = (PyArrayObject *) PyArray_FromDims(
		    1, &ans_size, PyArray_INT);
	    if (!ans) return NULL;
	    ians = (int *) ans->data;
	    for (i = 0 ; i < len ; i++)
		    ians [numbers [i]] += 1 ;
	    Py_DECREF(lst);
    } else {
	    wts = (PyArrayObject *) PyArray_ContiguousFromObject(
		    weight,PyArray_DOUBLE,1,1);
	    if (!wts) return NULL;
	    weights = (double *) wts->data;
	    
	    if (PyArray_Size((PyObject *)wts) != len) {
		    PyErr_Format(PyExc_ValueError,
				 "histogram: length of weights does not match that of list.");
		    Py_DECREF(lst);
		    Py_DECREF(wts);
		    return NULL;
	    }
	    ans = (PyArrayObject *) PyArray_FromDims(
		    1, &ans_size, PyArray_DOUBLE);
	    if (!ans) {
		    Py_DECREF(lst);
		    Py_DECREF(wts);
		    return NULL;
	    }
	    dans = (double *) ans->data;
	    for (i = 0 ; i < len ; i++) {
		    dans [numbers [i]] += weights [i];
	    }
	    Py_DECREF(lst);
	    Py_DECREF(wts);
    }
    return PyArray_Return (ans);
}

static PyObject *
PyArray_Take(PyObject *self0, PyObject *indices0, int axis) 
{
    PyArrayObject *self, *indices, *ret;
    int nd, shape[MAXDIM];
    int i, j, chunk, n, m, max_item, tmp;
    char *src, *dest;
	
    indices = ret = NULL;
    self = (PyArrayObject *)PyArray_ContiguousFromObject(self0, PyArray_NOTYPE, 1, 0);
    if (self == NULL) return NULL;
	
    if (axis < 0) axis = axis + self->nd;
    if ((axis < 0) || (axis >= self->nd)) {
	PyErr_SetString(PyExc_ValueError, "Invalid axis for this array");
	goto fail;
    }
	
    indices = (PyArrayObject *)PyArray_ContiguousFromObject(indices0, PyArray_LONG, 1, 0);
    if (indices == NULL) goto fail;
	
    n = m = chunk = 1;
    nd = self->nd + indices->nd - 1;
    for (i=0; i< nd; i++) {
	if (i < axis) {
	    shape[i] = self->dimensions[i];
	    n *= shape[i];
	} else {
	    if (i < axis+indices->nd) {
		shape[i] = indices->dimensions[i-axis];
		m *= shape[i];
	    } else {
		shape[i] = self->dimensions[i-indices->nd+1];
		chunk *= shape[i];
	    }
	}
    }
    ret = (PyArrayObject *)PyArray_FromDims(nd, shape, self->descr->type_num);
	
    if (ret == NULL) goto fail;
	
    max_item = self->dimensions[axis];
    chunk = chunk * ret->descr->elsize;
    src = self->data;
    dest = ret->data;
	
    for(i=0; i<n; i++) {
	for(j=0; j<m; j++) {
	    tmp = ((long *)(indices->data))[j];
	    if (tmp < 0) tmp = tmp+max_item;
	    if ((tmp < 0) || (tmp >= max_item)) {
		PyErr_SetString(PyExc_IndexError, "Index out of range for array");
		goto fail;
	    }
	    memmove(dest, src+tmp*chunk, chunk);
	    dest += chunk;
	}
	src += chunk*max_item;
    }
	
    Py_XDECREF(indices);
    Py_XDECREF(self);

    return (PyObject *)ret;
	
	
 fail:
    Py_XDECREF(ret);
    Py_XDECREF(indices);
    Py_XDECREF(self);
    return NULL;
}

static PyObject *
PyArray_Put(PyObject *self0, PyObject *indices0, 
                             PyObject* values0) 
{
    PyArrayObject  *indices, *values, *self;
    int i, chunk, ni, max_item, nv, tmp; 
    char *src, *dest;

    indices = NULL;
    values = NULL;

    if (!PyArray_Check(self0)) {
	PyErr_SetString(PyExc_ValueError, "put: first argument must be an array");
	return NULL;
    }
    self = (PyArrayObject*) self0;
    if (!PyArray_ISCONTIGUOUS(self)) {
	PyErr_SetString(PyExc_ValueError, "put: first argument must be contiguous");
	return NULL;
    }
    max_item = PyArray_SIZE(self);
    dest = self->data;
    chunk = self->descr->elsize;

    indices = (PyArrayObject *)PyArray_ContiguousFromObject(
	    indices0, PyArray_LONG, 0, 0);
    if (indices == NULL) goto fail;
    ni = PyArray_SIZE(indices);

    values = (PyArrayObject *)PyArray_ContiguousFromObject(
	    values0, self->descr->type, 0, 0);
    if (values == NULL) goto fail;
    nv = PyArray_SIZE(values);
    if (nv > 0) { /* nv == 0 for a null array */
        for(i=0; i<ni; i++) {
            src = values->data + chunk * (i % nv);
            tmp = ((long *)(indices->data))[i];
            if (tmp < 0) tmp = tmp+max_item;
            if ((tmp < 0) || (tmp >= max_item)) {
                PyErr_SetString(
			PyExc_IndexError, "Index out of range for array");
                goto fail;
            }
            memmove(dest + tmp * chunk, src, chunk);
        }
    }

    Py_XDECREF(values);
    Py_XDECREF(indices);
    Py_INCREF(Py_None);
    return Py_None;
	
 fail:
    Py_XDECREF(indices);
    Py_XDECREF(values);
    return NULL;
}

static PyObject *
PyArray_PutMask(PyObject *self0, PyObject *mask0, 
                                 PyObject* values0) 
{
    PyArrayObject  *mask, *values, *self;
    int i, chunk, ni, max_item, nv, tmp; 
    char *src, *dest;

    mask = NULL;
    values = NULL;

    if (!PyArray_Check(self0)) {
	PyErr_SetString(
		PyExc_ValueError, 
		"putmask: first argument must be an array");
	return NULL;
    }
    self = (PyArrayObject*) self0;
    if (!PyArray_ISCONTIGUOUS(self)) {
	PyErr_SetString(
		PyExc_ValueError, 
		"putmask: first argument must be contiguous");
	return NULL;
    }
    max_item = PyArray_SIZE(self);
    dest = self->data;
    chunk = self->descr->elsize;

    mask = (PyArrayObject *)PyArray_ContiguousFromObject(
	    mask0, PyArray_LONG, 0, 0);
    if (mask == NULL) goto fail;
    ni = PyArray_SIZE(mask);
    if (ni != max_item) {
	PyErr_SetString(
		PyExc_ValueError, 
		"putmask: mask and data must be the same size.");
	goto fail;
    }

    values = (PyArrayObject *)PyArray_ContiguousFromObject(
	    values0, self->descr->type, 0, 0);
    if (values == NULL) goto fail;
    nv = PyArray_SIZE(values);   /* zero if null array */
    if (nv > 0) {
        for(i=0; i<ni; i++) {
            src = values->data + chunk * (i % nv);
            tmp = ((long *)(mask->data))[i];
            if (tmp) {
                memmove(dest + i * chunk, src, chunk);
            }
        }
    }

    Py_XDECREF(values);
    Py_XDECREF(mask);
    Py_INCREF(Py_None);
    return Py_None;
	
 fail:
    Py_XDECREF(mask);
    Py_XDECREF(values);
    return NULL;
}

static PyObject * 
PyArray_Reshape(PyArrayObject *self, PyObject *shape) {
    int i, n, s_original, i_unknown, s_known;
    int *dimensions;
    PyArrayObject *ret;
	
    if (!PyArray_ISCONTIGUOUS(self)) {
	PyErr_SetString(PyExc_ValueError, "reshape only works on contiguous arrays");
	return NULL;
    }
	
    if (PyArray_As1D(&shape, (char **)&dimensions, &n, PyArray_INT) == -1) 
	    return NULL;
	
    s_known = 1;
    i_unknown = -1;
    for(i=0; i<n; i++) {
	if (dimensions[i] < 0) {
	    if (i_unknown == -1) {
		i_unknown = i;
	    } else {
		PyErr_SetString(
			PyExc_ValueError, 
			"can only specify one unknown dimension");
		goto fail;
	    }
	} else {
	    s_known *= dimensions[i];
	}
    }
	
    s_original = PyArray_SIZE(self);
	
    if (i_unknown >= 0) {
	if ((s_known == 0) || (s_original % s_known != 0)) {
	    PyErr_SetString(
		    PyExc_ValueError, 
		    "total size of new array must be unchanged");
	    goto fail;
	}
	dimensions[i_unknown] = s_original/s_known;
    } else {
	if (s_original != s_known) {
	    PyErr_SetString(
		    PyExc_ValueError, 
		    "total size of new array must be unchanged");
	    goto fail;
	}
    }
    if ((ret = (PyArrayObject *)PyArray_FromDimsAndDataAndDescr(
		 n, dimensions, self->descr, self->data)) == NULL)
	    goto fail;
    
    Py_INCREF(self);
    ret->base = (PyObject *)self;
	
    PyArray_Free(shape, (char *)dimensions);
    return (PyObject *)ret;
	
 fail:
    PyArray_Free(shape, (char *)dimensions);
    return NULL;
}

char *_numeric__doc__ = 
"libnumeric contains miscellaneous functions either provided for Numeric\n"
"compatibility or directly ported from Numeric.\n";

#define DEFERRED_ADDRESS(ADDR) 0

static char doc_take[] = "take(a, indices, axis=0).  Selects the elements in indices from array a along the given axis.";

static PyObject *
array_take(PyObject *dummy, PyObject *args, PyObject *kwds) 
{
    int dimension;
    PyObject *a, *indices, *ret;
    static char *kwlist[] = {"array", "indices", "axis", NULL};
	
    dimension=0;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OO|i", kwlist, 
                                     &a, &indices, &dimension))
	return NULL;
	
    ret = PyArray_Take(a, indices, dimension);
    return ret;
}

static char doc_put[] = "put(a, indices, values) sets a.flat[n] = v[n] for each n in indices. v can be scalar or shorter than indices, will repeat.";

static PyObject *
array_put(PyObject *dummy, PyObject *args) 
{
    PyObject *a, *indices, *ret, *values;
	
    if (!PyArg_ParseTuple(args, "OOO", &a, &indices, &values))
	return NULL;
    ret = PyArray_Put(a, indices, values);
    return ret;
}

static char doc_putmask[] = "putmask(a, mask, values) sets a.flat[n] = v[n] for each n where mask.flat[n] is true. v can be scalar.";

static PyObject *
array_putmask(PyObject *dummy, PyObject *args) 
{
    PyObject *a, *mask, *ret, *values;	
    if (!PyArg_ParseTuple(args, "OOO", &a, &mask, &values))
	return NULL;
    ret = PyArray_PutMask(a, mask, values);
    return ret;
}

#define COMPARE(type) static int type##_compare(type *ip1, type *ip2) { return *ip1 < *ip2 ? -1 : *ip1 == *ip2 ? 0 : 1; }

COMPARE( Bool)

COMPARE( Int8)
COMPARE( UInt8)

COMPARE( Int16)
COMPARE( UInt16)

COMPARE( Int32)
COMPARE( UInt32)

COMPARE( Int64)
COMPARE( UInt64)

COMPARE( Float32)
COMPARE( Float64)

typedef int (*CompareFunction) (const void *, const void *);

static CompareFunction compare_functions[] =
{
	NULL,                           
	(CompareFunction)Bool_compare,  

	(CompareFunction)Int8_compare,
	(CompareFunction)UInt8_compare,

	(CompareFunction)Int16_compare,
	(CompareFunction)UInt16_compare,

	(CompareFunction)Int32_compare,
	(CompareFunction)UInt32_compare,

	(CompareFunction)Int64_compare,
	(CompareFunction)UInt64_compare,

	(CompareFunction)Float32_compare,
	(CompareFunction)Float64_compare,

	NULL,             /* Complex32 */
	NULL,             /* Complex64 */
};

static long 
local_where(char *ip, char *vp, int elsize, int elements, 
	    CompareFunction compare) 
{
    long min_i, max_i, i;
    int location;
	
    min_i = 0;
    max_i = elements;
	
    while (min_i != max_i) {
	i = (max_i-min_i)/2 + min_i;
	location = compare(ip, vp+elsize*i);
	if (location == 0) {
	    while (i > 0) {
		if (compare(ip, vp+elsize*(--i)) != 0) {
		    i = i+1; break;
		}
	    }
	    return i;
	}
	if (location < 0) {
	    max_i = i;
	} else {
	    min_i = i+1;
	}
    }
    return min_i;
}

static PyObject *
PyArray_BinarySearch(PyObject *op1, PyObject *op2) {
    PyArrayObject *ap1, *ap2, *ret;
    int m, n, i, elsize;
    char *ip;
    long *rp;
    CompareFunction compare_func;
    int typenum = 0;

    typenum = PyArray_ObjectType(op1, 0);
    typenum = PyArray_ObjectType(op2, typenum);	
    ret = NULL;
    ap1 = (PyArrayObject *)PyArray_ContiguousFromObject(op1, typenum, 1, 1);
    if (ap1 == NULL) return NULL;
    ap2 = (PyArrayObject *)PyArray_ContiguousFromObject(op2, typenum, 0, 0);
    if (ap2 == NULL) goto fail;
	
    ret = (PyArrayObject *)PyArray_FromDims(ap2->nd, ap2->dimensions,
					    PyArray_LONG);
    if (ret == NULL) goto fail;
	
    compare_func = compare_functions[ap2->descr->type_num];
    if (compare_func == NULL) {
	PyErr_SetString(PyExc_TypeError, "compare not supported for type");
	goto fail;
    }
	
    elsize = ap1->descr->elsize;
    m = ap1->dimensions[ap1->nd-1];
    n = PyArray_Size((PyObject *)ap2);
	
    for (rp = (long *)ret->data, ip=ap2->data, i=0; i<n; i++, ip+=elsize, rp++) {
	*rp = local_where(ip, ap1->data, elsize, m, compare_func);
    }
	
    Py_DECREF(ap1);
    Py_DECREF(ap2);
    return PyArray_Return(ret);
	
 fail:
    Py_XDECREF(ap1);
    Py_XDECREF(ap2);
    Py_XDECREF(ret);
    return NULL;
}

static PyObject *
PyArray_Sort(PyObject *op) 
{
    PyArrayObject *ap;
    CompareFunction compare_func;
    char *ip;
    int i, n, m, elsize;
	
    ap = (PyArrayObject *)PyArray_CopyFromObject(op, PyArray_NOTYPE,
						 1, 0);
    if (ap == NULL) return NULL;
	
    compare_func = compare_functions[ap->descr->type_num];
    if (compare_func == NULL) {
	PyErr_SetString(PyExc_TypeError, 
			"compare not supported for type");
	Py_XDECREF(ap);
	return NULL;
    }
	
    elsize = ap->descr->elsize;
    m = ap->dimensions[ap->nd-1];
    if (m == 0) {
	return PyArray_Return(ap);
    }
    n = PyArray_SIZE(ap)/m;
    for (ip = ap->data, i=0; i<n; i++, ip+=elsize*m) {
	qsort(ip, m, elsize, compare_func);
    }
	
    return PyArray_Return(ap);
	
}  

/* Using these pseudo global varibales can not possibly be a good idea, but... */

static CompareFunction argsort_compare_func;
static char *argsort_data;
static int argsort_elsize;

static int 
argsort_static_compare(long *ip1, long *ip2) 
{
	return argsort_compare_func(argsort_data+(argsort_elsize * *ip1), 
				    argsort_data+(argsort_elsize * *ip2));
}

static PyObject *
PyArray_ArgSort(PyObject *op) 
{
    PyArrayObject *ap, *ret;
    long *ip;
    int i, j, n, m;
	
    ap = (PyArrayObject *)PyArray_ContiguousFromObject(op, PyArray_NOTYPE, 1, 0);
    if (ap == NULL) return NULL;
	
    ret = (PyArrayObject *)PyArray_FromDims(ap->nd, ap->dimensions, PyArray_LONG);
    if (ret == NULL) goto fail;
	
    argsort_compare_func = compare_functions[ap->descr->type_num];
    if (argsort_compare_func == NULL) {
	PyErr_SetString(PyExc_TypeError, "compare not supported for type");
	goto fail;
    }
	
    ip = (long *)ret->data;
    argsort_elsize = ap->descr->elsize;
    m = ap->dimensions[ap->nd-1];
    if (m == 0) {
	Py_XDECREF(ap);
	return PyArray_Return(ret);
    }
    n = PyArray_SIZE(ap)/m;
    argsort_data = ap->data;
    for (i=0; i<n; i++, ip+=m, argsort_data += m*argsort_elsize) {
	for(j=0; j<m; j++) ip[j] = j;
	qsort((char *)ip, m, sizeof(long),
	      (CompareFunction)argsort_static_compare);
    }
	
    Py_DECREF(ap);
    return PyArray_Return(ret);

 fail:
    Py_XDECREF(ap);
    Py_XDECREF(ret);
    return NULL;

}  

/*Sets the maximum number of dimensions in an array to 40.
  If you ever need to change this I'd love to know more about your arrays.
*/
#define MAX_DIMS 30
static int compare_lists(int *l1, int *l2, int n) {
    int i;
    for(i=0;i<n;i++) {
	if (l1[i] != l2[i]) return 0;
    } 
    return 1;
}

/*op is a python object supporting the sequence interface.
  Its elements will be concatenated together to form a single 
  multidimensional array.*/
static PyObject *
PyArray_Concatenate(PyObject *op) 
{
    PyArrayObject *ret, **mps;
    PyObject *otmp;
    int i, n, type_num, tmp, nd, new_dim;
    char *data;
	
    n = PySequence_Length(op);
    if (n == -1) {
	return NULL;
    }
    if (n == 0) {
	PyErr_SetString(PyExc_ValueError, 
			"Concatenation of zero-length tuples is impossible.");
	return NULL;
    }

    ret = NULL;
	
    mps = (PyArrayObject **)malloc(n*sizeof(PyArrayObject *));
    if (mps == NULL) {
	PyErr_SetString(PyExc_MemoryError, "memory error");
	return NULL;
    }
	
    /* Make sure these arrays are legal to concatenate. */
    /* Must have same dimensions except d0, and have coercible type. */
	
    type_num = 0;
    for(i=0; i<n; i++) {
	otmp = PySequence_GetItem(op, i);
	type_num = PyArray_ObjectType(otmp, type_num);
	mps[i] = NULL;
	Py_XDECREF(otmp);
    }
    if (type_num == -1) {
	PyErr_SetString(PyExc_TypeError, 
			"can't find common type for arrays to concatenate");
	goto fail;
    }
	
    for(i=0; i<n; i++) {
	if ((otmp = PySequence_GetItem(op, i)) == NULL) goto fail;
	mps[i] = (PyArrayObject*)
	    PyArray_ContiguousFromObject(otmp, type_num, 0, 0);
	Py_DECREF(otmp);
    }
	
    new_dim = 0;
    nd = mps[0]->nd;
    for(i=0; i<n; i++) {
	if (mps[i] == NULL) goto fail;
	if (i != 0) {
		if (nd != mps[i]->nd) {
			PyErr_SetString(
				PyExc_ValueError, 
				"arrays must have same number of dimensions");
			goto fail;
		}
		if (!compare_lists(mps[0]->dimensions+1, 
				   mps[i]->dimensions+1, nd-1)) {
			PyErr_SetString(
				PyExc_ValueError, 
				"array dimensions must agree except for d_0");
			goto fail;
		}
	}
	if (nd == 0) {
	    PyErr_SetString(PyExc_ValueError, 
			    "0d arrays can't be concatenated");
	    goto fail;
	}
	new_dim += mps[i]->dimensions[0];
    }
	
    tmp = mps[0]->dimensions[0];
    mps[0]->dimensions[0] = new_dim;
    ret = (PyArrayObject *)PyArray_FromDims(nd, mps[0]->dimensions,
					    type_num);
    mps[0]->dimensions[0] = tmp;
	
    if (ret == NULL) goto fail;
	
    data = ret->data;
    for(i=0; i<n; i++) {
	memmove(data, mps[i]->data, PyArray_NBYTES(mps[i]));
	data += PyArray_NBYTES(mps[i]);
    }
	
    for(i=0; i<n; i++) Py_XDECREF(mps[i]);
    free(mps);
    return (PyObject *)ret;
	
 fail:
    Py_XDECREF(ret);
    for(i=0; i<n; i++) Py_XDECREF(mps[i]);
    free(mps);
    return NULL;
}

typedef int (*ArgFunction) (void*, long, long*);

#define ARGFUNC(type) int type##_argmax(type *ip, long n, long *ap) { \
           long i; type mp=ip[0]; *ap=0; \
           for(i=1; i<n; i++) { if (ip[i] > mp) { mp = ip[i]; *ap=i; } } return 0;}

ARGFUNC(Bool)
ARGFUNC(Int8)
ARGFUNC(UInt8)
ARGFUNC(Int16)
ARGFUNC(UInt16)
ARGFUNC(Int32)
ARGFUNC(UInt32)
ARGFUNC(Int64)
ARGFUNC(UInt64)
ARGFUNC(Float32)
ARGFUNC(Float64)

static ArgFunction argmax_functions[] = {
    NULL,
    (ArgFunction) Bool_argmax,
    (ArgFunction) Int8_argmax,
    (ArgFunction) UInt8_argmax,
    (ArgFunction) Int16_argmax,
    (ArgFunction) UInt16_argmax,
    (ArgFunction) Int32_argmax,
    (ArgFunction) UInt32_argmax,
    (ArgFunction) Int64_argmax,
    (ArgFunction) UInt64_argmax,
    (ArgFunction) Float32_argmax,
    (ArgFunction) Float64_argmax, 
    NULL,
    NULL,
};

extern PyObject *
PyArray_ArgMax(PyObject *op) 
{
    PyArrayObject *ap, *rp;
    ArgFunction arg_func;
    char *ip;
    int i, n, m, elsize;
	
    rp = NULL;
    ap = (PyArrayObject *)PyArray_ContiguousFromObject(op, PyArray_NOTYPE, 1, 0);
    if (ap == NULL) return NULL;
	
    arg_func = argmax_functions[ap->descr->type_num];
    if (arg_func == NULL) {
	PyErr_SetString(PyExc_TypeError, "type not ordered");
	goto fail;
    }
    rp = (PyArrayObject *)PyArray_FromDims(ap->nd-1, ap->dimensions, PyArray_LONG);
    if (rp == NULL) goto fail;


    elsize = ap->descr->elsize;
    m = ap->dimensions[ap->nd-1];
    if (m == 0) {
	PyErr_SetString(PyExc_ValueError, "Attempt to get argmax/argmin of an empty sequence??");
	goto fail;
    }
    n = PyArray_SIZE(ap)/m;
    for (ip = ap->data, i=0; i<n; i++, ip+=elsize*m) {
	arg_func(ip, m, ((long *)rp->data)+i);
    }
    Py_DECREF(ap);
    return PyArray_Return(rp);
	
 fail:
    Py_DECREF(ap);
    Py_XDECREF(rp);
    return NULL;
}  


/* Check whether the given array is stored contiguously (row-wise) in
   memory. */
static int 
array_really_contiguous(PyArrayObject *ap) 
{
    int sd;
    int i;

    sd = ap->descr->elsize;
    for (i = ap->nd-1; i >= 0; --i) {
	/* contiguous by definition */
	if (ap->dimensions[i] == 0) return 1; 
		      
	if (ap->strides[i] != sd) return 0;
	sd *= ap->dimensions[i];
    }
    return 1;
}

/* Changed to be able to deal with non-contiguous arrays. */
static PyObject *
PyArray_Transpose(PyArrayObject *ap, PyObject *op) 
{
    long *axes, axis;
    int i, n;
    int *permutation = NULL;
    PyArrayObject *ret = NULL;
	
    if (op == Py_None) {
      n = ap->nd;
      permutation = (int *)malloc(n*sizeof(int));
      for(i=0; i<n; i++)
	permutation[i] = n-1-i;
    } else {
      if (PyArray_As1D(&op, (char **)&axes, &n, PyArray_LONG) == -1)
	return NULL;
	
      permutation = (int *)malloc(n*sizeof(int));
	
      for(i=0; i<n; i++) {
	axis = axes[i];
	if (axis < 0) axis = ap->nd+axis;
	if (axis < 0 || axis >= ap->nd) {
	  PyErr_SetString(PyExc_ValueError, 
			  "invalid axis for this array");
	  goto fail;
	}
	permutation[i] = axis;
      }
    }
	

    ret = NA_copy(ap);

/* this allocates memory for dimensions and strides (but fills them
       incorrectly), sets up descr, and points data at ap->data. 
    ret = (PyArrayObject *)PyArray_FromDimsAndData(n, ap->dimensions,
						   ap->descr->type_num,
						   ap->data);
*/
    if (ret == NULL) goto fail;
	
    /* point at true owner of memory:
    ret->base = (PyObject *)ap;
    Py_INCREF(ap);
    */	

    for(i=0; i<n; i++) {
	ret->dimensions[i] = ap->dimensions[permutation[i]];
	ret->strides[i] = ap->strides[permutation[i]];
    }
    if (array_really_contiguous(ret)) ret->flags |= CONTIGUOUS;
    else ret->flags &= ~CONTIGUOUS;
	
    if (op != Py_None)
      PyArray_Free(op, (char *)axes);
    free(permutation);
    return (PyObject *)ret;
	
 fail:
    Py_XDECREF(ret);
    if (permutation != NULL) free(permutation);
    if (op != Py_None)
      PyArray_Free(op, (char *)axes);
    return NULL;
}

static PyObject *
PyArray_Repeat(PyObject *aop, PyObject *op, int axis) 
{
    long *counts;
    int n, n_outer, i, j, k, chunk, total, tmp;
    PyArrayObject *ret=NULL, *ap;
    char *new_data, *old_data;
	
    ap = (PyArrayObject *)PyArray_ContiguousFromObject(aop, 
						       PyArray_NOTYPE, 0, 0);
	
    if (axis < 0) axis = ap->nd+axis;
    if (axis < 0 || axis >= ap->nd) {
	PyErr_SetString(PyExc_ValueError, "axis is invalid");
	return NULL;
    }
	
    if (PyArray_As1D(&op, (char **)&counts, &n, PyArray_LONG) == -1) 
	return NULL;
	
    if (n != ap->dimensions[axis]) {
	PyErr_SetString(PyExc_ValueError, 
			"len(n) != a.shape[axis]");
	goto fail;
    }
	
    total = 0;
    for(j=0; j<n; j++) {
	if (counts[j] == -1) {
	    PyErr_SetString(PyExc_ValueError, "count < 0");
	    goto fail;
	}
	total += counts[j];
    }
    tmp = ap->dimensions[axis];
    ap->dimensions[axis] = total;
    ret = (PyArrayObject *)PyArray_FromDims(ap->nd, ap->dimensions,
					    ap->descr->type_num);
    ap->dimensions[axis] = tmp;
	
    if (ret == NULL) goto fail;
	
    new_data = ret->data;
    old_data = ap->data;
	
    chunk = ap->descr->elsize;
    for(i=axis+1; i<ap->nd; i++) {
	chunk *= ap->dimensions[i];
    }
	
    n_outer = 1;
    for(i=0; i<axis; i++) n_outer *= ap->dimensions[i];
	
    for(i=0; i<n_outer; i++) {
	for(j=0; j<n; j++) {
	    for(k=0; k<counts[j]; k++) {
		memmove(new_data, old_data, chunk);
		new_data += chunk;
	    }
	    old_data += chunk;
	}
    }

    Py_XDECREF(ap);
    PyArray_Free(op, (char *)counts);

    return (PyObject *)ret;
	
 fail:
    Py_XDECREF(ap);
    Py_XDECREF(ret);
    PyArray_Free(op, (char *)counts);
    return NULL;
}


static PyObject *
PyArray_Choose(PyObject *ip, PyObject *op) 
{
    int i, n, *sizes, m, offset, elsize, type_num;
    char *ret_data;
    PyArrayObject **mps, *ap, *ret;
    PyObject *otmp;
    long *self_data, mi;
    ap = NULL;
    ret = NULL;
	
    n = PySequence_Length(op);
	
    mps = (PyArrayObject **)malloc(n*sizeof(PyArrayObject *));
    if (mps == NULL) {
	PyErr_SetString(PyExc_MemoryError, "memory error");
	return NULL;
    }
	
    sizes = (int *)malloc(n*sizeof(int));
	
    /* Figure out the right type for the new array */
	
    type_num = 0;
    for(i=0; i<n; i++) {
	otmp = PySequence_GetItem(op, i);
	type_num = PyArray_ObjectType(otmp, type_num);
	mps[i] = NULL;
	Py_XDECREF(otmp);
    }
    if (type_num == -1) {
	PyErr_SetString(PyExc_TypeError, 
			"can't find common type for arrays to choose from");
	goto fail;
    }
	
    /* Make sure all arrays are real array objects. */
    for(i=0; i<n; i++) {
	if ((otmp = PySequence_GetItem(op, i)) == NULL) 
	    goto fail;
	mps[i] = (PyArrayObject*)
	    PyArray_ContiguousFromObject(otmp, type_num, 
					 0, 0);
	Py_DECREF(otmp);
    }
	
    ap = (PyArrayObject *)PyArray_ContiguousFromObject(ip, 
						       PyArray_LONG, 0, 0);
    if (ap == NULL) goto fail;
	
    /* Check the dimensions of the arrays */
    for(i=0; i<n; i++) {
	if (mps[i] == NULL) goto fail;
	if (ap->nd < mps[i]->nd) {
	    PyErr_SetString(PyExc_ValueError, 
			    "too many dimensions");
	    goto fail;
	}
	if (!compare_lists(ap->dimensions+(ap->nd-mps[i]->nd),
			   mps[i]->dimensions, mps[i]->nd)) {
	    PyErr_SetString(PyExc_ValueError, 
			    "array dimensions must agree");
	    goto fail;
	}
	sizes[i] = PyArray_NBYTES(mps[i]);
    }
	
    ret = (PyArrayObject *)PyArray_FromDims(ap->nd, ap->dimensions,
					    type_num);
    if (ret == NULL) goto fail;
	
    elsize = ret->descr->elsize;
    m = PyArray_SIZE(ret);
    self_data = (long *)ap->data;
    ret_data = ret->data;
	
    for (i=0; i<m; i++) {
	mi = *self_data;
	if (mi < 0 || mi >= n) {
	    PyErr_SetString(PyExc_ValueError, 
			    "invalid entry in choice array");
	    goto fail;
	}
	offset = i*elsize;
	if (offset >= sizes[mi]) {offset = offset % sizes[mi]; }
	memmove(ret_data, mps[mi]->data+offset, elsize);
	ret_data += elsize; self_data++;
    }
	
    for(i=0; i<n; i++) Py_XDECREF(mps[i]);
    Py_DECREF(ap);
    free(mps);
    free(sizes);

    return (PyObject *)ret;
	
 fail:
    for(i=0; i<n; i++) Py_XDECREF(mps[i]);
    Py_XDECREF(ap);
    free(mps);
    free(sizes);
    Py_XDECREF(ret);
    return NULL;
}


static char doc_concatenate[] = "concatenate((a1,a2,...)).";

static PyObject *
array_concatenate(PyObject *dummy, PyObject *args) 
{
    PyObject *a0;
	
    if (!PyArg_ParseTuple(args, "O", &a0)) return NULL;
    return PyArray_Concatenate(a0);
}


static char doc_transpose[] = 
"transpose(a, axes=None) returns array with dimensions permuted according to\naxes.  If axes is None (default) returns array with dimensions reversed.";

static PyObject *
array_transpose(PyObject *dummy, PyObject *args, PyObject *kwds) 
{
    PyObject *shape=Py_None, *ret, *a0;
    PyArrayObject *a;
    static char *kwlist[] = {"array", "axes", NULL};
	
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|O", kwlist, 
                                     &a0, &shape)) return NULL;
    if ((a = (PyArrayObject *)PyArray_FromObject(a0,
						 PyArray_NOTYPE,0,0))
	==NULL) return NULL;
	
    ret = PyArray_Transpose(a, shape);
    Py_DECREF(a);
    return ret;
}


static char doc_repeat[] = "repeat(a, n, axis=0)";

static PyObject *
array_repeat(PyObject *dummy, PyObject *args, PyObject *kwds) 
{
    PyObject *shape, *a0;
    int axis=0;
    static char *kwlist[] = {"array", "shape", "axis", NULL};
	
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OO|i", kwlist, 
                                     &a0, &shape, &axis)) return NULL;
	
    return PyArray_Repeat(a0, shape, axis);
}

static char doc_choose[] = "choose(a, (b1,b2,...))";

static PyObject *
array_choose(PyObject *dummy, PyObject *args) 
{
    PyObject *shape, *a0;
	
    if (!PyArg_ParseTuple(args, "OO", &a0, &shape)) return NULL;
	
    return PyArray_Choose(a0, shape);
}

static char doc_sort[] = "sort(a)";

static PyObject *array_sort(PyObject *dummy, PyObject *args) 
{
    PyObject *a0;
	
    if (!PyArg_ParseTuple(args, "O", &a0)) return NULL;
	
    return PyArray_Sort(a0);
}

static char doc_argsort[] = "argsort(a)";

static PyObject *
array_argsort(PyObject *dummy, PyObject *args) 
{
    PyObject *a0;
	
    if (!PyArg_ParseTuple(args, "O", &a0)) return NULL;
	
    return PyArray_ArgSort(a0);
}

static char doc_binarysearch[] = "binarysearch(a,v)";

static PyObject *
array_binarysearch(PyObject *dummy, PyObject *args) 
{
    PyObject *shape, *a0;
	
    if (!PyArg_ParseTuple(args, "OO", &a0, &shape)) return NULL;
	
    return PyArray_BinarySearch(a0, shape);
}

static char doc_argmax[] = "argmax(a)";

static PyObject *
array_argmax(PyObject *dummy, PyObject *args) 
{
    PyObject *a0;
	
    if (!PyArg_ParseTuple(args, "O", &a0)) return NULL;
	
    return PyArray_ArgMax(a0);
}

static PyMethodDef _libnumericMethods[] = {
	{"histogram",	arr_histogram,	
	 METH_VARARGS,	arr_histogram__doc__},
	{"take",	 (PyCFunction)array_take, 
	 METH_VARARGS|METH_KEYWORDS, doc_take},
	{"put",	(PyCFunction)array_put, METH_VARARGS, doc_put},
	{"putmask",	 (PyCFunction)array_putmask, 
	 METH_VARARGS, doc_putmask},
	{"concatenate",	 (PyCFunction)array_concatenate, 
	 METH_VARARGS, doc_concatenate},
	{"transpose",	 (PyCFunction)array_transpose, 
	 METH_VARARGS|METH_KEYWORDS, doc_transpose},
	{"repeat",	 (PyCFunction)array_repeat, 
	 METH_VARARGS|METH_KEYWORDS, doc_repeat},
	{"choose",	 (PyCFunction)array_choose, 
	 METH_VARARGS, doc_choose},	
	{"sort",	 (PyCFunction)array_sort, 
	 METH_VARARGS, doc_sort},
	{"argsort",	 (PyCFunction)array_argsort, 
	 METH_VARARGS, doc_argsort},
	{"binarysearch", (PyCFunction)array_binarysearch, 
	 METH_VARARGS, doc_binarysearch},
	{"argmax",	(PyCFunction)array_argmax, 
	 METH_VARARGS, doc_argmax},
	{NULL,      NULL}        /* Sentinel */
};

#define METHOD_TABLE_EXISTS 1

/*
 * Local Variables:
 * mode: C
 * c-file-style: "python"
 * End:
 */
