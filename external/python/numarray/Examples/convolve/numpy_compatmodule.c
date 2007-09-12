#include "Python.h"

#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <ctype.h>

/* The following makes sure the API variable is declared extern rather
   than static so it won't conflict with the real definition below */
#define NO_IMPORT_ARRAY 1   

#include "arrayobject.h"

void **PyArray_API;   /* Define this in only one of the compilation units
			 compiled and linked with NO_IMPORT_ARRAY */

static PyObject *_Error;

extern int Convolve1d(PyArrayObject *kernel, PyArrayObject *data, 
		       PyArrayObject *convolved);

extern void Convolve2d(PyArrayObject *kernel, PyArrayObject *data, 
		       PyArrayObject *convolved);

static PyObject *
Py_Convolve1d(PyObject *obj, PyObject *args)
{
	PyObject   *okernel, *odata, *oconvolved=Py_None;
	PyArrayObject *kernel, *data, *convolved;
	int i, dims[MAXDIM];
	
	if (!PyArg_ParseTuple(args, "OO|O", &okernel, &odata, &oconvolved))
		return PyErr_Format(_Error, 
				    "Convolve2d: Invalid parameters.");

	kernel = (PyArrayObject *) PyArray_ContiguousFromObject(okernel, 
							     PyArray_DOUBLE, 1, 1);
	data = (PyArrayObject *) PyArray_ContiguousFromObject(odata, 
							     PyArray_DOUBLE, 1, 1);
	if (!kernel || !data) return NULL;

	if (oconvolved == Py_None) {
		for(i=0; i<data->nd; i++)
			dims[i] = data->dimensions[i];
		convolved = (PyArrayObject *) PyArray_FromDims(
			data->nd, dims, PyArray_DOUBLE);
	} else
		convolved = (PyArrayObject *) PyArray_ContiguousFromObject(
			oconvolved, PyArray_DOUBLE, 1, 1);
	if (!convolved) return NULL;

	if (Convolve1d(kernel, data, convolved) < 0) {
		Py_XDECREF(kernel);
		Py_XDECREF(data);
		Py_XDECREF(convolved);
		return NULL;
	}

	Py_XDECREF(kernel);
	Py_XDECREF(data);

	if (oconvolved != Py_None) {
		Py_XDECREF(convolved);
		Py_INCREF(Py_None);
		return Py_None;
	} else
		return PyArray_Return(convolved);
}

static PyObject *
Py_Convolve2d(PyObject *obj, PyObject *args)
{
	PyObject   *okernel, *odata, *oconvolved=Py_None;
	PyArrayObject *kernel, *data, *convolved;
	int i, dims[MAXDIM];

	if (!PyArg_ParseTuple(args, "OO|O", &okernel, &odata, &oconvolved))
		return PyErr_Format(_Error, 
				    "Convolve2d: Invalid parameters.");

	kernel = (PyArrayObject *) PyArray_ContiguousFromObject(
		okernel, PyArray_DOUBLE, 2, 2);
	data = (PyArrayObject *) PyArray_ContiguousFromObject(
		odata, PyArray_DOUBLE, 2, 2);

	if (!kernel || !data) return NULL;

	if (oconvolved == Py_None) {
		for(i=0; i<data->nd; i++)
			dims[i] = data->dimensions[i];
		convolved = (PyArrayObject *) PyArray_FromDims(
			data->nd, dims, PyArray_DOUBLE);
	} else
		convolved = (PyArrayObject *) PyArray_ContiguousFromObject(
			oconvolved, PyArray_DOUBLE, 2, 2);
	if (!convolved) return NULL;

	Convolve2d(kernel, data, convolved);

	Py_XDECREF(kernel);
	Py_XDECREF(data);

	if (oconvolved != Py_None) {
		Py_XDECREF(convolved);
		Py_INCREF(Py_None);
		return Py_None;
	} else
		return PyArray_Return(convolved);
}

static PyMethodDef _convolveMethods[] = {
	{"Convolve1d", Py_Convolve1d, METH_VARARGS, 
	 "Convolve1d(kernel, data, output=None) convolves 1d array 'kernel' " \
	 "with 1d array 'data' either returning the result or silently storing " \
	 "it in 'output'."},
	{"Convolve2d", Py_Convolve2d, METH_VARARGS,
	 "Convolve2d(kernel, data, output=None) convolves 2d array 'kernel' " \
	 "with 2d array 'data' either returning the result or silently storing " \
	 "it in 'output'."},
	{NULL, NULL} /* Sentinel */
};

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif

void initnumpy_compat(void) {
	PyObject *m, *d;
	m = Py_InitModule("numpy_compat", _convolveMethods);
	d = PyModule_GetDict(m);
	_Error = PyErr_NewException("_numpy_compat.error", NULL, NULL);
	PyDict_SetItemString(d, "error", _Error);
	import_array();
}



/*
 * Local Variables:
 * mode: C
 * c-file-style: "python"
 * End:
 */
