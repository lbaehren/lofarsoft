#include "Python.h"

#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <ctype.h>

#include "libnumarray.h"

static PyObject *_convolveError;

static void 
Convolve1d(long ksizex, Float64 *kernel, 
	   long dsizex, Float64 *data, 
	   Float64 *convolved)
{
	long xc;
	long halfk = ksizex/2;

	for(xc=0; xc<halfk; xc++)
		convolved[xc] = data[xc];

	for(xc=halfk; xc<dsizex-halfk; xc++) {
		long xk;
		double temp = 0;
		for (xk=0; xk<ksizex; xk++)
			temp += kernel[xk]*data[xc-halfk+xk];
		convolved[xc] = temp;
	}
		     
	for(xc=dsizex-halfk; xc<dsizex; xc++)
		convolved[xc] = data[xc];
}

static void 
Convolve2d(long krows, long kcols, Float64 *kernel, 
	   long drows, long dcols, Float64 *data, 
	   Float64 *convolved)
{
	long ki, kj, di, dj;
	long halfkrows = krows/2;
	long halfkcols = kcols/2;

	/* Copy the data in the half kernel "frame" straight through. */
	for(di=0; di<halfkrows; di++) {
		for(dj=0; dj<dcols; dj++)
			convolved[di*dcols+dj] = data[di*dcols+dj];
	}
	for(di=drows-halfkrows; di<drows; di++) {
		for(dj=0; dj<dcols; dj++)
			convolved[di*dcols+dj] = data[di*dcols+dj];
	}
	for(di=halfkrows; di<drows-halfkrows; di++) {
		for(dj=0; dj<halfkcols; dj++)
			convolved[di*dcols+dj] = data[di*dcols+dj];
	}
	for(di=halfkrows; di<drows-halfkrows; di++) {
		for(dj=dcols-halfkcols; dj<dcols; dj++)
			convolved[di*dcols+dj] = data[di*dcols+dj];
	}

	/* Do the convolution on the data within the frame. */
	for(di=halfkrows; di<drows-halfkrows; di++) {
		for(dj=halfkcols; dj<dcols-halfkcols; dj++) {
			Float64 temp = 0;
			for(ki=0; ki<krows; ki++) {
				long pi = di + ki - halfkrows;
				for(kj=0; kj<kcols; kj++) {
					long pj = dj + kj - halfkcols;
					temp += data[pi*dcols+pj] * 
						kernel[ki*kcols+kj];
				}
			}
			convolved[di*dcols+dj] = temp;
		}
	}
}


static PyObject *
Py_Convolve1d(PyObject *obj, PyObject *args)
{
	PyObject   *okernel, *odata, *oconvolved=Py_None;
	PyArrayObject *kernel, *data, *convolved;

	if (!PyArg_ParseTuple(args, "OO|O", &okernel, &odata, &oconvolved))
		return PyErr_Format(_convolveError, 
				    "Convolve1d: Invalid parameters.");

	/* Align, Byteswap, Contiguous, Typeconvert */
	kernel	= NA_IoArray(okernel, tFloat64, NUM_C_ARRAY);
	data	= NA_IoArray(odata, tFloat64, NUM_C_ARRAY);
	convolved = NA_OptionalOutputArray(oconvolved, tFloat64, NUM_C_ARRAY, data);

	if (!kernel || !data || !convolved) {
		PyErr_Format( _convolveError, 
			      "Convolve1d: error converting array inputs.");
		goto _fail;
	}

	if ((kernel->nd != 1) || (data->nd != 1)) {
		PyErr_Format(_convolveError,
			     "Convolve1d: numarray must have 1 dimensions.");
		goto _fail;
	}

	if (!NA_ShapeEqual(data, convolved)) {
		PyErr_Format(_convolveError,
			     "Convolve1d: data and output numarray need"
			     "identitcal shapes.");
		goto _fail;
	}

	Convolve1d(kernel->dimensions[0], 
		   NA_OFFSETDATA(kernel),
		   data->dimensions[0],   
		   NA_OFFSETDATA(data),
		   NA_OFFSETDATA(convolved));

	Py_XDECREF(kernel);
	Py_XDECREF(data);

	/* Align, Byteswap, Contiguous, Typeconvert */
	return NA_ReturnOutput(oconvolved, convolved);

  _fail:
	Py_XDECREF(kernel);
	Py_XDECREF(data);
	Py_XDECREF(convolved);
	return NULL;      
}

static PyObject *
Py_Convolve2d(PyObject *obj, PyObject *args)
{
	PyObject   *okernel, *odata, *oconvolved=Py_None;
	PyArrayObject *kernel, *data, *convolved;

	if (!PyArg_ParseTuple(args, "OO|O", &okernel, &odata, &oconvolved))
		return PyErr_Format(_convolveError, 
				    "Convolve2d: Invalid parameters.");

	/* Align, Byteswap, Contiguous, Typeconvert */
	kernel	= NA_InputArray(okernel, tFloat64, NUM_C_ARRAY);
	data	= NA_InputArray(odata, tFloat64, NUM_C_ARRAY);
	convolved = NA_OptionalOutputArray(oconvolved, tFloat64, NUM_C_ARRAY, data);

	if (!kernel || !data || !convolved) {
		PyErr_Format( _convolveError, 
			      "Convolve2d: error converting array inputs.");
		goto _fail;
	}

	if ((kernel->nd != 2) || (data->nd != 2)) {
		PyErr_Format(_convolveError,
			     "Convolve2d: numarray must have 2 dimensions.");
		goto _fail;
	}

	if (!NA_ShapeEqual(data, convolved)) {
		PyErr_Format(_convolveError,
			     "Convolve2d: data and output numarray need identitcal shapes.");
		goto _fail;
	}

	Convolve2d(kernel->dimensions[0], 
		   kernel->dimensions[1], 
		   NA_OFFSETDATA(kernel),
		   data->dimensions[0],   
		   data->dimensions[1],   
		   NA_OFFSETDATA(data),
		   NA_OFFSETDATA(convolved));

	Py_XDECREF(kernel);
	Py_XDECREF(data);

	/* Align, Byteswap, Contiguous, Typeconvert */
	return NA_ReturnOutput(oconvolved, convolved);

  _fail:
	Py_XDECREF(kernel);
	Py_XDECREF(data);
	Py_XDECREF(convolved);
	return NULL;
}

static PyMethodDef _convolveMethods[] = {
    {"Convolve1d", Py_Convolve1d, METH_VARARGS},
    {"Convolve2d", Py_Convolve2d, METH_VARARGS},
    {NULL, NULL} /* Sentinel */
};

/* platform independent*/
#ifdef MS_WIN32
__declspec(dllexport)
#endif

void inithigh_level(void)
{
	PyObject *m, *d;
	m = Py_InitModule("high_level", _convolveMethods);
	d = PyModule_GetDict(m);
	_convolveError = PyErr_NewException("_high_level.error", NULL, NULL);
	PyDict_SetItemString(d, "error", _convolveError);
	import_libnumarray();
}

/*
 * Local Variables:
 * mode: C
 * c-file-style: "python"
 * End:
 */
