/** convolve example using element-wise API */

#include "Python.h"

#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <ctype.h>

#include "libnumarray.h"

static PyObject *_Error;

static int 
Convolve1d(PyArrayObject *kernel, PyArrayObject *data, PyArrayObject *convolved)
{
	int xc, xk;
	int ksizex = kernel->dimensions[0];
	int halfk = ksizex / 2;
	int dsizex = data->dimensions[0];

	for(xc=0; xc<halfk; xc++)
		NA_set1_Float64(convolved, xc, NA_get1_Float64(data, xc));
		     
	for(xc=dsizex-halfk; xc<dsizex; xc++)
		NA_set1_Float64(convolved, xc, NA_get1_Float64(data, xc));

	for(xc=halfk; xc<dsizex-halfk; xc++) {
		Float64 temp = 0;
		for (xk=0; xk<ksizex; xk++) {
			int i = xc - halfk + xk;
			temp += NA_get1_Float64(kernel, xk) * 
				NA_get1_Float64(data, i);
		}
		NA_set1_Float64(convolved, xc, temp);
		
		/* Check for errors in get/set functions periodically */
	}
	if (PyErr_Occurred())
		return -1;
	else
		return 0;
}

static int 
Convolve2d(PyArrayObject *kernel, PyArrayObject *data, PyArrayObject *convolved)
{
	int ki, kj, di, dj;
	int krows = kernel->dimensions[0], kcols = kernel->dimensions[1];
	int drows = data->dimensions[0], dcols = data->dimensions[1];
	int halfkrows = krows/2;
	int halfkcols = kcols/2;

	/* Copy the data in the half kernel "frame" straight through. */
	for(di=0; di<halfkrows; di++) {
		for(dj=0; dj<dcols; dj++)
			NA_SET2(convolved, Float64, di, dj,
				NA_GET2(data, Float64, di, dj));
	}
	for(di=drows-halfkrows; di<drows; di++) {
		for(dj=0; dj<dcols; dj++)
			NA_SET2(convolved, Float64, di, dj,
				NA_GET2(data, Float64, di, dj));
	}
	for(di=halfkrows; di<drows-halfkrows; di++) {
		for(dj=0; dj<halfkcols; dj++)
			NA_SET2(convolved, Float64, di, dj,
				NA_GET2(data, Float64, di, dj));
	}
	for(di=halfkrows; di<drows-halfkrows; di++) {
		for(dj=dcols-halfkcols; dj<dcols; dj++)
			NA_SET2(convolved, Float64, di, dj,
				NA_GET2(data, Float64, di, dj));
	}

	for(di=halfkrows; di<drows-halfkrows; di++) {
		for(dj=halfkcols; dj<dcols-halfkcols; dj++) {
			Float64 temp = 0;
			for(ki=0; ki<krows; ki++) {
				int pi = di + ki - halfkrows;
				for(kj=0; kj<kcols; kj++) {
					int pj = dj + kj - halfkcols;
					temp += 
					     NA_GET2(data, Float64, pi, pj) *
					     NA_GET2f(kernel, Float64, ki, kj);
				}
			}
			NA_SET2(convolved, Float64, di, dj, temp);
		}
	}
	return 0;
}

static PyObject *
Py_Convolve1d(PyObject *obj, PyObject *args)
{
	PyObject   *okernel, *odata, *oconvolved=Py_None;
	PyArrayObject *kernel, *data, *convolved;

	if (!PyArg_ParseTuple(args, "OO|O", &okernel, &odata, &oconvolved))
		return PyErr_Format(_Error, 
				    "Convolve1d: Invalid parameters.");

	/* Accept any variety of type tFloat w/o conversion; translate
	   lists/tuples. No copy is made except for lists and non-tFloat.
	   Byteswaping, misaligment, and discontiguity must be handled by
	   Convolve1d.  */
	kernel = NA_InputArray(okernel, tFloat64, 0);

	/* Accept any variety of any type w/o conversion; translate
	   lists/tuples. No copy except for lists.  All types, byteswapping,
	   misalignment, and discontiguity are handled by Convolve1d.  */
	data   = NA_InputArray(odata, tAny, 0);

	/* Accept any variety of type tFloat w/o conversion/shadowing.  If
	   convolved is Py_None (unspecified) call data.new(Float64) to create
	   cshadow, effectively cloning cshadow from data. */
	convolved = NA_OptionalOutputArray(oconvolved, tFloat64, 0, data);
	
	if (!kernel || !data || !convolved)
		return NULL;
	if ((kernel->nd != 1) || (data->nd != 1))
		return PyErr_Format(_Error,
                       "Convolve1d: numarray must have exactly 1 dimension.");

	if (!NA_ShapeEqual(data, convolved))
		return PyErr_Format(_Error,
                       "Convolve1d: data and output numarray need same length.");
	if (!NA_ShapeLessThan(kernel, data))
		return PyErr_Format(_Error,
  		       "Convolve1d: kernel must be smaller than data in both dimensions");
	
	if (Convolve1d(kernel, data, convolved) < 0) {
		return NULL;
	} else {
		Py_XDECREF(kernel);
		Py_XDECREF(data);
		
		/* If convolved is Py_None, return cshadow.  Else, DECREF cshadow to
		   deallocate it and copy cshadow back onto convolved and return
		   Py_None. */
		return NA_ReturnOutput(oconvolved, convolved);
	}
}


static PyObject *
Py_Convolve2d(PyObject *obj, PyObject *args)
{
	PyObject   *okernel, *odata, *oconvolved = Py_None;
	PyArrayObject *kernel, *data, *convolved;
	
	if (!PyArg_ParseTuple(args, "OO|O", &okernel, &odata, &oconvolved))
		return PyErr_Format(_Error, 
				    "Convolve2d: Invalid parameters.");
	
	/* Require the kernel to be a C_array.  Make it so if it isn't.  */
	kernel = NA_InputArray(okernel, tFloat64, NUM_C_ARRAY);	

	/* Accept any variety of type tFloat w/o conversion; translate
	   lists/tuples. No copy except for lists and non-tFloat64. */
	data   = NA_InputArray(odata, tFloat64, 0);

	/* Accept any variety of type tFloat w/o conversion/shadowing.  If
	   oconvolved is Py_None (unspecified) call data.new(Float64) to create
	   convolved, effectively cloning it from data. */
	convolved = NA_OptionalOutputArray(oconvolved, tFloat64, 0, data);

	if (!kernel || !data || !convolved)
		return NULL;
	if ((kernel->nd != 2) || (data->nd != 2))
		return PyErr_Format(_Error,
				    "Convolve2d: numarray must have 2 dimensions.");
	if (!NA_ShapeEqual(data, convolved))
		return PyErr_Format(_Error,
				    "Convolve2d: data,output numarray need identical shapes.");
	if (!NA_ShapeLessThan(kernel, data))
		return PyErr_Format(_Error,
				    "Convolve2d: kernel must be smaller than data in both dimensions");

	if (Convolve2d(kernel, data, convolved) < 0)
		return NULL;
	else {

		Py_XDECREF(kernel);
		Py_XDECREF(data);
		
		/* If convolved is Py_None, return cshadow.  Else, DECREF cshadow to
		   deallocate it and copy cshadow back onto convolved 
		   and return Py_None. */
		return NA_ReturnOutput(oconvolved, convolved);
	}
}


static PyMethodDef _convolveMethods[] = {
	{"Convolve1d", Py_Convolve1d, METH_VARARGS, 
	 "Convolve1d(kernel, data, output=None) convolves 1d array 'kernel'" \
	 "with 1d array 'data' either returning the result or silently storing" \
	 "it in 'output'."},
	{"Convolve2d", Py_Convolve2d, METH_VARARGS,
	 "Convolve2d(kernel, data, output=None) convolves 2d array 'kernel'" \
	 "with 2d array 'data' either returning the result or silently storing" \
	 "it in 'output'."},
	{NULL, NULL} /* Sentinel */
};


/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif

void initelement_wise(void) {
	PyObject *m, *d;
	m = Py_InitModule("element_wise", _convolveMethods);
	d = PyModule_GetDict(m);
	_Error = PyErr_NewException("_element_wise.error", NULL, NULL);
	PyDict_SetItemString(d, "error", _Error);
	import_libnumarray();
}

/*
 * Local Variables:
 * mode: C
 * c-file-style: "python"
 * End:
 */
