/* This module shows how to turn a static C array into a NumArray. */


#include "Python.h"

#include "numarray/arrayobject.h"

static double data[4][3] = {{1, 2, 3},
			    {4, 5, 6},
			    {7, 8, 9},
			    {10, 11, 12}};
static int shape[2] = { 4, 3 };

static PyObject *
_getarray(PyObject *module, PyObject *args)
{
	if (!PyArg_ParseTuple(args,":getarray"))
		return NULL;
	return PyArray_FromDimsAndData(2, shape, PyArray_DOUBLE, (void *)data);
}

static PyMethodDef _dimsdata_functions[] = {
	{"getarray", _getarray, METH_VARARGS, 
	 "getarray() returns a static array cast as a numarray."},
	{NULL,      NULL}        /* Sentinel */
};

static char * _dimsdata_doc = "Example module for PyArray_DimsAndData.";

void initdimsdata(void)
{
	PyObject  *m = Py_InitModule3("dimsdata",
				      _dimsdata_functions,
				      _dimsdata_doc);
	if (m == NULL) return;

	import_array();
}

