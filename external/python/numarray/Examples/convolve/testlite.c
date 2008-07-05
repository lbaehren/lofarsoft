/*

This module demonstrates how to make a weak linkage between an extension
module (which might consider numarray to be optional) and numarray.

There are essentially two parts to the "weak linkage":

1.  PyArray_isArray(o) identifies if an arbitrary Python object is a NumArray.
Once it is known that an object is a numarray, assume that PyArray_API has
been initialized and call API functions.  PyArray_isArray(o) is unique in the
sense that it will work correctly even if numarray is not installed.  In that
case, it always reports false.  Because of this, PyArray_isArray() is useful
as a guard around arbitrary API functions which should never be called when
numarray is not present.  Doing so results in a call to NULL and ensuing
segfault.

2.  At module init time, prior to calling and numarray API functions, call
import_array().  Since, in the case where numarray has *not* been installed,
the import_array() call will fail, check for and clear the Python error state.
This will prevent the Python exception handling mechanisms from inadvertantly
trapping the error later on.

*/


#include "Python.h"

#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <ctype.h>

#include "arrayobject.h"

static PyObject *_Error;

static PyObject *
Py_testit(PyObject *obj, PyObject *args)
{
	PyObject   *it;

	if (!PyArg_ParseTuple(args, "O", &it))
		return PyErr_Format(_Error, "testit: Invalid parameters.");

	if (PyArray_isArray(it)) {
		fprintf(stderr, "It's an array.\n");

		/* It's safe to call API functions in here */
	} else {
		fprintf(stderr, "It's not an array.\n");

		/* But never call numarray API functions out here. */
	}
	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef _Methods[] = {
	{"testit", Py_testit, METH_VARARGS, 
	 "testit(obj) prints a message identifying an object."},
	{NULL, NULL} /* Sentinel */
};

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif

void inittestlite(void) {
	PyObject *m, *d;
	m = Py_InitModule("testlite", _Methods);
	d = PyModule_GetDict(m);
	_Error = PyErr_NewException("testlite.error", NULL, NULL);
	PyDict_SetItemString(d, "error", _Error);

	import_array();
	if (PyErr_Occurred())
		PyErr_Clear();
}



/*
 * Local Variables:
 * mode: C
 * c-file-style: "python"
 * End:
 */
