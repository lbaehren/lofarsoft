/* $Id: _chararraymodule.c,v 1.24 2005/10/27 21:06:35 jaytmiller Exp $ */

#include <Python.h>

#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <ctype.h>

#include <libnumarray.h>

static PyObject *_Error;

/* returns length of string 's', or 'maxlen', whichever is smaller. */
#if !defined(strnlen)
size_t strnlen(const char *s, size_t maxlen)
{
	size_t i;
	for(i=0; i<maxlen; i++)
		if (s[i] == 0)
			return i;
	return maxlen;
}
#endif

#if !defined(min)
static int min(int x, int y)
{
	if (x < y)
		return x;
	else
		return y;
}
#endif

#if !defined(max)
static int max(int x, int y)
{
	if (x > y)
		return x;
	else
		return y;
}
#endif

/* mystrdup duplicates the given string 's', copying up to 'size' bytes.
   This still works when 's' is not terminated within 'size' bytes. 
   Also uses python memory allocation.
*/
static char *mystrdup(char *s, int size)
{
	char *u = PyMem_Malloc(size), *v = u;
	if (!u)
		return (char *) PyErr_Format(_Error,
					     "mycat: Error allocating memory.");
	while(size-- && (*u++ = *s++));
	return v;
}

/* rstripw strips whitespace from the right side of 's', which can be up to
'n' characters long.  rstripw interprets 0 as terminating 's'.
rstripw never strips a string down to length 0; the last whitespace is always
spared.
*/

#if defined(isspace)
#undef isspace
#define isspace(c)  ((c==' ')||(c=='\t')||(c=='\n')||(c=='\r')||(c=='\v')||(c=='\f'))
#endif

static void rstripw(char *s, int n)
{
	int i;
	for(i=strnlen(s,n)-1; i>=1; i--)  /* Never strip to length 0. */
	{
		int c = s[i];
		if (!c || isspace(c))
			s[i] = 0;
		else
			break;
	}
}

/* padw pads string 's' with blanks, for a total length of 'n' characters. */
static void 
padw(char *s, int n, char padc)
{
	int i;
	for(i=strnlen(s, n); i<n; i++)
		s[i] = padc;
}

/* Strip executes rstripw on a python string. */
static PyObject *
Strip(PyObject *obj, PyObject *args)
{
	char *s, *t;
	int  slen;  

	if (!PyArg_ParseTuple(args, "s#:Strip", &s, &slen))
		return NULL;
	if ((t = mystrdup(s, slen)))
	{
		rstripw(t, slen);
		obj = Py_BuildValue("s#", t, strnlen(t, slen));
		PyMem_Free(t);
		return obj;
	}
	else
		return NULL;
}


/* Pad executes padw on a python string. */
static PyObject *
Pad(PyObject *obj, PyObject *args)
{
	char *s, *t;
	int  slen, rsize;
	char padc;

	if (!PyArg_ParseTuple(args, "s#ib:Pad", &s, &slen, &rsize, &padc))
		return NULL;
	rsize = max(slen, rsize);
	if ((t = mystrdup(s, rsize)))
	{
		padw(t, rsize, padc);
		obj = Py_BuildValue("s#", t, rsize);
		PyMem_Free(t);
		return obj;
	}
	else
		return NULL;
}

static int 
Concat(PyObject *aux, long nnumarray, PyArrayObject *numarray[], char *data[])
{
	size_t rused, bused;
	PyArrayObject *ai = numarray[0];
	PyArrayObject *bi = numarray[1];
	PyArrayObject *ri = numarray[2];
	char *a = data[0];
	char *b = data[1];
	char *r = data[2];
	if (!PyInt_Check(aux)) {
		PyErr_SetString(_Error, "Bad call to Concat.");
		return -1;
	}
	if (PyInt_AsLong(aux)) {
		memcpy(r, a, ai->itemsize);
		memcpy(r+ai->itemsize, b, bi->itemsize);
	} else {
		strncpy(r, a, ai->itemsize);
		rused = strnlen(r, ai->itemsize);
		bused = min(strnlen(b, bi->itemsize), ri->itemsize-rused);
		strncpy(r+rused, b, bused);
		memset(r+rused+bused, 0, ri->itemsize-rused-bused);
	}
	return 0;
}

NSTRIDING_DESCR3(Concat);

#define SMALL_STRING 4096

static char *
_chararray_copy_and_strip(char *original, char *temp, int nc, int is_raw)
{
	if (nc > SMALL_STRING) {
		temp = malloc(nc);
		if (!temp) {
			PyErr_SetString(_Error, "Couldn't malloc memory for CharArray string comparison.");
			return NULL;
		}
	}
	memcpy(temp, original, nc);
	if (!is_raw) {
		rstripw(temp, nc);
	}
	return temp;
}

static void
_chararray_release(char *ptr, int nc)
{
	if (nc > SMALL_STRING) {
		free(ptr);
	}
}

enum {
	STR_EQ,
	STR_NE,
	STR_LT,
	STR_GT,
	STR_LE,
	STR_GE,

	RAWSTR_EQ,
	RAWSTR_NE,
	RAWSTR_LT,
	RAWSTR_GT,
	RAWSTR_LE,
	RAWSTR_GE,
};

static int 
StrCmp(PyObject *aux, long nnumarray, PyArrayObject *numarray[], char *data[])
{
	PyArrayObject *ai = numarray[0];
	PyArrayObject *bi = numarray[1];
	char   *a = data[0];
	char   *b = data[1];
	Bool   *r = (Bool *) data[2];
	int nc = min(ai->itemsize, bi->itemsize);	
	char atemp[SMALL_STRING], btemp[SMALL_STRING];
	char *aptr, *bptr;
	long mode = (long) aux;
	int is_raw = mode > RAWSTR_EQ;

	aptr = _chararray_copy_and_strip(a, atemp, nc, is_raw);
	bptr = _chararray_copy_and_strip(b, btemp, nc, is_raw);
	if (!aptr || !bptr) return -1;

	switch(mode) 
	{
	case STR_EQ:  *r = strncmp(aptr, bptr, nc) == 0; break;
	case STR_NE:  *r = strncmp(aptr, bptr, nc) != 0; break;
	case STR_LT:  *r = strncmp(aptr, bptr, nc) < 0;  break;
	case STR_GT:  *r = strncmp(aptr, bptr, nc) > 0;  break;
	case STR_LE:  *r = strncmp(aptr, bptr, nc) <= 0; break;
	case STR_GE:  *r = strncmp(aptr, bptr, nc) >= 0; break;
		
	case RAWSTR_EQ:  *r = memcmp(aptr, bptr, nc) == 0; break;
	case RAWSTR_NE:  *r = memcmp(aptr, bptr, nc) != 0; break;
	case RAWSTR_LT:  *r = memcmp(aptr, bptr, nc) < 0;  break;
	case RAWSTR_GT:  *r = memcmp(aptr, bptr, nc) > 0;  break;
	case RAWSTR_LE:  *r = memcmp(aptr, bptr, nc) <= 0; break;
	case RAWSTR_GE:  *r = memcmp(aptr, bptr, nc) >= 0; break;

	default:
		PyErr_Format(PyExc_ValueError, 
			     "StrCmp: invalid comparison mode.");
		return -1;
	}	

	_chararray_release(aptr, nc);
	_chararray_release(bptr, nc);
	return 0;
}

PyObject *
_Py_StrCmp(PyObject *module, PyObject *args)
{
	long i, op, raw, mode;
	PyArrayObject *arrays[3] = {0, 0, 0};
	char *data[3];
	if (!PyArg_ParseTuple( args, "OllO:StrCmp", arrays, &op, &raw, arrays+1))
		return NULL;
	if (!NA_NDArrayCheck((PyObject *) arrays[0]))
		return PyErr_Format(PyExc_TypeError, 
				    "StrCmp: Invalid 1st parameter type.");
	if (!NA_NDArrayCheck((PyObject *) arrays[1]) 
	    || (arrays[0]->ob_type != arrays[1]->ob_type)
	    || (arrays[0]->itemsize != arrays[1]->itemsize)
	    || !NA_ShapeEqual(arrays[0], arrays[1])) {
		return PyObject_CallMethod(
			(PyObject *) arrays[0], "_StrCmp", "llO", 
			op, raw, arrays[1]);
	} 
	if (!NA_updateDataPtr(arrays[0]))
		return NULL;
	if (!NA_updateDataPtr(arrays[1]))
		return NULL;
	mode = op + RAWSTR_EQ*raw;
	arrays[2] = NA_vNewArray( NULL, tBool, 
				  arrays[0]->nd, 
				  arrays[0]->dimensions);
	if (!arrays[2]) return NULL;
	for(i=0; i<3; i++)
		data[i] = arrays[i]->data;
	if (_NA_callStridingHelper(
		    (PyObject *) mode, arrays[0]->nd, 3, arrays, data, StrCmp) < 0) {
		Py_DECREF(arrays[2]);
		return NULL;
	}
	return (PyObject *) arrays[2];
}

/* StripAll applies rstripw to every element of a CharArray.  This can be used
to normalize charnumarray initialized from a binary source.
*/
static int 
StripAll( PyObject *aux, int nnumarray, PyArrayObject *numarray[], char *data[])
{
	PyArrayObject *ai = numarray[0];
	char   *a  = data[0];

	if (nnumarray != 1) {
		PyErr_Format(_Error, "StripAll: invalid parameters.");
		return -1;
	}
	if (!PyArray_ISWRITABLE(ai)) {
		PyErr_Format(_Error, "StripAll: result array not writeable.");
		return -1;
	}
	rstripw(a, ai->itemsize);
	return 0;
}

NSTRIDING_DESCR1(StripAll);

static int 
PadAll( PyObject *aux, int nnumarray, PyArrayObject *numarray[], char *data[])
{
	PyArrayObject *ai = numarray[0];
	char   *a  = data[0];

	if (nnumarray != 1) {
		PyErr_Format(PyExc_ValueError, 
			     "PadAll: invalid parameters.");
		return -1;
	}
	if (!PyArray_ISWRITABLE(ai)) {
		PyErr_Format(PyExc_ValueError, 
			     "PadAll: result array not writeable.");
		return -1;
	}
	if (!PyString_Check(aux) || PyString_Size(aux) != 1) {
		PyErr_Format(PyExc_ValueError, 
			     "aux parameter must be a len-1-padding-string");
		return -1;
	}	
	padw(a, ai->itemsize, *PyString_AsString(aux)); 
	return 0;
}

NSTRIDING_DESCR1(PadAll);

static int 
ToUpper( PyObject *aux, int nnumarray, PyArrayObject *numarray[], char *data[])
{
	PyArrayObject *ai = numarray[0];
	char   *a  = data[0];
	long i;

	if (nnumarray != 1) {
		PyErr_Format(_Error, "ToUpper: invalid parameters.");
		return -1;
	}
	if (!PyArray_ISWRITABLE(ai)) {
		PyErr_Format(_Error, "ToUpper: result array not writeable.");
		return -1;
	}
	for(i=0; (a[i]!=0) && (i<ai->itemsize); i++)
		a[i] = toupper(a[i]);
	return 0;
}

NSTRIDING_DESCR1(ToUpper);

static int 
ToLower( PyObject *aux, int nnumarray, PyArrayObject *numarray[], char *data[])
{
	PyArrayObject *ai = numarray[0];
	char   *a  = data[0];
	long i;

	if (nnumarray != 1) {
		PyErr_Format(_Error, "ToLower: invalid parameters.");
		return -1;
	}
	if (!PyArray_ISWRITABLE(ai)) {
		PyErr_Format(_Error, "ToLower: result array not writeable.");
		return -1;
	}
	for(i=0; (a[i]!=0) && (i<ai->itemsize); i++)
		a[i] = tolower(a[i]);
	return 0;
}

NSTRIDING_DESCR1(ToLower);

/* StrLen computes the lengths of all the items in a chararray. */
static int 
StrLen(PyObject *args, int nnumarray, PyArrayObject *numarray[], char *data[])
{
	PyArrayObject  *ai = numarray[0];
	PyArrayObject  *ni = numarray[1];
	char *a;
	Int32 *n;
	if ((nnumarray < 2) || 
	    !NA_NDArrayCheck((PyObject *)ai) || 
	    !NA_NumArrayCheck((PyObject *)ni)) {
		PyErr_Format(PyExc_ValueError, 
			     "StrLen requires one string and one numerical array");
		return -1;
	}
	a = data[0];
	n = (Int32 *) data[1];
	rstripw(a, ai->itemsize);
	*n = strnlen(a, ai->itemsize);
	return 0;
}
NSTRIDING_DESCR2(StrLen);

static int 
Eval(PyObject *args, int nnumarray, PyArrayObject *numarray[], char *data[])
{
	PyArrayObject *ai = numarray[0];
	char *a = data[0];
	Float64 *n = (Float64 *) data[1];
	char buffer[64], *ptr;
	int len;

	len = strnlen(a, ai->itemsize);
	if  (len > ELEM(buffer)-1) {
		PyErr_Format(PyExc_ValueError, 
			     "string too long to convert.");
		return -1;
	}
	memcpy(buffer, a, len);
	buffer[len] = 0;
	*(double *) n = strtod(buffer, &ptr);
	if ((ptr == buffer) && (*n == 0)) {
		PyErr_Format(_Error, "Eval: error evaluating string."); 
		return -1;
	}
	return 0;
}
NSTRIDING_DESCR2(Eval);

static int 
Format(PyObject *format, int nnumarray, PyArrayObject *numarray[], char *data[])
{
	PyArrayObject *ni = numarray[0];
	PyArrayObject *ai = numarray[1];
	char *np = data[0];
	char *a = data[1];
	maybelong offset = np - ni->data;
	Float64 n = NA_get_Float64(ni, offset);
	PyObject *args, *astr;
	char *s;
	if (!(args = Py_BuildValue("(d)", n))) {
		PyErr_Format(_Error, 
			     "Format: error building args tuple.");
		return -1;
	}
	if (!(astr = PyString_Format(format, args)))
		return -1;
	s = PyString_AsString(astr);
	if (strlen(s) > ai->itemsize) {
		PyErr_Warn(PyExc_RuntimeWarning,
			   "formatted value too large"
			   " for CharArray itemsize.");
	}
	strncpy(a, s, ai->itemsize);
	Py_DECREF(astr);
	Py_DECREF(args);
	return 0;
}

NSTRIDING_DESCR2(Format);

static PyMethodDef _chararrayMethods[] = {
	{"Strip",  Strip, METH_VARARGS},
	{"Pad",    Pad, METH_VARARGS},
	{"StrCmp", _Py_StrCmp, METH_VARARGS},
	{NULL,     NULL}        /* Sentinel */
};

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif

void init_chararray(void) {
	PyObject *m, *d;
	m = Py_InitModule("_chararray", _chararrayMethods);
	d = PyModule_GetDict(m);
	_Error = PyErr_NewException("numarray._chararray.error", NULL, NULL);
	PyDict_SetItemString(d, "error", _Error);

	ADD_VERSION(m);

	import_libnumarray();

	NA_add_cfunc(d, "Concat", &Concat_descr);
	NA_add_cfunc(d, "Format", &Format_descr);
	NA_add_cfunc(d, "Eval", &Eval_descr);
	NA_add_cfunc(d, "StrLen", &StrLen_descr);
	NA_add_cfunc(d, "StripAll", &StripAll_descr);
	NA_add_cfunc(d, "PadAll", &PadAll_descr);
	NA_add_cfunc(d, "ToUpper", &ToUpper_descr);
	NA_add_cfunc(d, "ToLower", &ToLower_descr);
}
