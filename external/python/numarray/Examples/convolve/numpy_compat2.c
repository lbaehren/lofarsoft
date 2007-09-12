#include "Python.h"

#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <ctype.h>

#define NO_IMPORT_ARRAY 1

#include "arrayobject.h"

#define A_GET1(a, t, i)  (*((t *) ((a)->data+((i)*(a)->strides[0]))))
#define A_SET1(a, t, i, v)  (A_GET1(a, t, i) = (v))

#define A_GET2(a, t, i, j)  (*((t *) ((a)->data+((i)*(a)->strides[0] +        \
                                                 (j)*(a)->strides[1]))))
#define A_SET2(a, t, i, j, v)  (A_GET2(a, t, i, j) = (v))


int
Convolve1d(PyArrayObject *kernel, PyArrayObject *data, 
	   PyArrayObject *convolved)
{
	int xc, xk;
	int ksizex = kernel->dimensions[0];
	int halfk = ksizex / 2;
	int dsizex = data->dimensions[0];

	/* This code is to demonstrate that API functions still work. */
	if (!PyArray_Check((PyObject *) kernel) || 
	    !PyArray_Check((PyObject *) data) ||
	    !PyArray_Check((PyObject *) convolved)) {
		PyErr_Format(PyExc_TypeError, 
			     "Convolve1d: expected PyArrayObjects...");
		return -1;
	}

	for(xc=0; xc<halfk; xc++)
		A_SET1(convolved, Float64, xc, A_GET1(data, Float64, xc));
		     
	for(xc=dsizex-halfk; xc<dsizex; xc++)
		A_SET1(convolved, Float64, xc, A_GET1(data, Float64, xc));

	for(xc=halfk; xc<dsizex-halfk; xc++) {
		Float64 temp = 0;
		for (xk=0; xk<ksizex; xk++) {
			int i = xc - halfk + xk;
			Float64 k, d;
			k = A_GET1(kernel, Float64, xk);
			d = A_GET1(data, Float64, i);
			temp += k * d;
		}
		A_SET1(convolved, Float64, xc, temp);
	}

	return 0;
}

void 
Convolve2d(PyArrayObject *kernel, PyArrayObject *data, 
	   PyArrayObject *convolved)
{
	int ki, kj, di, dj;
	int krows = kernel->dimensions[0], kcols = kernel->dimensions[1];
	int drows = data->dimensions[0], dcols = data->dimensions[1];
	int halfkrows = krows/2;
	int halfkcols = kcols/2;

	/* Copy the data in the half kernel "frame" straight through. */
	for(di=0; di<halfkrows; di++) {
		for(dj=0; dj<dcols; dj++)
			A_SET2(convolved, Float64, di, dj,
				A_GET2(data, Float64, di, dj));
	}
	for(di=drows-halfkrows; di<drows; di++) {
		for(dj=0; dj<dcols; dj++)
			A_SET2(convolved, Float64, di, dj,
				A_GET2(data, Float64, di, dj));
	}
	for(di=halfkrows; di<drows-halfkrows; di++) {
		for(dj=0; dj<halfkcols; dj++)
			A_SET2(convolved, Float64, di, dj,
				A_GET2(data, Float64, di, dj));
	}
	for(di=halfkrows; di<drows-halfkrows; di++) {
		for(dj=dcols-halfkcols; dj<dcols; dj++)
			A_SET2(convolved, Float64, di, dj,
				A_GET2(data, Float64, di, dj));
	}

	for(di=halfkrows; di<drows-halfkrows; di++) {
		for(dj=halfkcols; dj<dcols-halfkcols; dj++) {
			Float64 temp = 0;
			for(ki=0; ki<krows; ki++) {
				int pi = di + ki - halfkrows;
				for(kj=0; kj<kcols; kj++) {
					int pj = dj + kj - halfkcols;
					temp += 
					     A_GET2(data, Float64, pi, pj) *
					     A_GET2(kernel, Float64, ki, kj);
				}
			}
			A_SET2(convolved, Float64, di, dj, temp);
		}
	}
}

