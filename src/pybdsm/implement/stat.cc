/*!
  \file stat.cc
  
  \ingroup pybdsm
  
  \author Oleksandr Usov
*/

#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle
#define NO_IMPORT_ARRAY

#include "aux.h"
#include "stat.h"

#include <num_util/num_util.h>
#include <cfloat>

using namespace boost::python;
using namespace std;
namespace n = num_util;

// check if obj is numpy scalar object and it's value is val
static bool
npybool_check(PyObject *obj, bool val)
{
  npy_bool tmp;

  if (PyArray_IsScalar(obj, Bool)) {
    PyArray_ScalarAsCtype(obj, &tmp);
    return tmp == val;
  }

  return false;
}

// calculate clipped mean/rms for 2d array
template<class T>
static pair<double, double>
_stat_2d(numeric::array arr, double _mean, double _threshold)
{
  // ensure contiguous memory access
  vector<int> shape = n::shape(arr);
  vector<int> strides = n::strides(arr);
  strides[0] /= sizeof(T); strides[1] /= sizeof(T);
  if (strides[0] < strides[1]) {
    swap(strides[0], strides[1]);
    swap(shape[0], shape[1]);
  }

  double sum = 0;
  double sumsq = 0;
  int N = n::size(arr);

  T *data = (T *)n::data(arr);
  const int adv0 = strides[0] - shape[1]*strides[1];
  const int adv1 = strides[1];
  int cnt1 = shape[0];

  while (cnt1--) {
    int cnt2 = shape[1];
    while (cnt2--) {
      double v = *data;
      if (fabs(v - _mean) > _threshold)
	--N;
      else {
	sum += v;
	sumsq += v*v;
      }
      data += adv1;
    }
    data += adv0;
  }

  double mean = sum/N;
  double dev  = sqrt((sumsq + N*mean*mean - 2*mean*sum)/(N-1));

  return make_pair(mean, dev);
}

// calculate clipped mean/rms for 2d masked array
template<class T>
static pair<double, double>
_stat_2d_m(numeric::array arr, numeric::array mask, double _mean, double _threshold)
{
  // ensure contiguous memory access
  vector<int> shape = n::shape(arr);
  vector<int> strides = n::strides(arr);
  vector<int> mstrides = n::strides(mask);
  strides[0] /= sizeof(T); strides[1] /= sizeof(T);
  mstrides[0] /= sizeof(npy_bool); mstrides[1] /= sizeof(npy_bool);
  if (strides[0] < strides[1]) {
    swap(shape[0], shape[1]);
    swap(strides[0], strides[1]);
    swap(mstrides[0], mstrides[1]);
  }

  double sum = 0;
  double sumsq = 0;
  int N = n::size(arr);

  T *data = (T *)n::data(arr);
  npy_bool *mdata = (npy_bool *)n::data(mask);
  const int adv0 = strides[0] - shape[1]*strides[1];
  const int adv1 = strides[1];
  const int madv0 = mstrides[0] - shape[1]*mstrides[1];
  const int madv1 = mstrides[1];
  int cnt1 = shape[0];

  while (cnt1--) {
    int cnt2 = shape[1];
    while (cnt2--) {
      double v = *data;
      if (*mdata || fabs(v - _mean) > _threshold)
	--N;
      else {
	sum += v;
	sumsq += v*v;
      }
      data += adv1;
      mdata += madv1;
    }
    data += adv0;
    mdata += madv0;
  }

  double mean = sum/N;
  double dev  = sqrt((sumsq + N*mean*mean - 2*mean*sum)/(N-1));

  return make_pair(mean, dev);
}

// dispatch calculation to the correct _stat_FOO function
template<class T>
static pair<double, double>
_stat(numeric::array arr, object mask, double _mean, double _threshold)
{
  if (mask.ptr() == Py_None || mask.ptr() == Py_False 
      || npybool_check(mask.ptr(), false))
    return _stat_2d<T>(arr, _mean, _threshold);
  else
    return _stat_2d_m<T>(arr, extract<numeric::array>(mask), _mean, _threshold);
}

template<class T>
static object _bstat(numeric::array arr, object mask, double kappa)
{
  const int max_iter = 100;
  vector<double> mean, dev;
  pair<double, double> res(0, DBL_MAX);
  int cnt = 0;

  do {
    ++cnt;
    mean.push_back(res.first);
    dev.push_back(res.second);
    res = _stat<T>(arr, mask, mean.back(), kappa*dev.back());
  } while (res.second != dev.back() && cnt < max_iter);

  py_assert(res.second == dev.back(),
	    PyExc_RuntimeError, "clipped RMS calculation does not converge");

  return make_tuple(mean[1], dev[1], mean.back(), dev.back());
}

object bstat(numeric::array arr, object mask, double kappa)
{
  NPY_TYPES type = n::type(arr);

  // only 2d arrays are supported
  n::check_rank(arr, 2);
  if (PyArray_ISBYTESWAPPED(arr.ptr()))
    goto fail;

  if (mask.ptr() != Py_None && mask.ptr() != Py_False 
      && !npybool_check(mask.ptr(), false)) {
    numeric::array amask = extract<numeric::array>(mask);

    n::check_type(amask, NPY_BOOL);
    n::check_rank(amask, 2);
    n::check_size(amask, n::size(arr));
    // this is pointless on pc, but might matter somewhere else
    if (PyArray_ISBYTESWAPPED(amask.ptr()))
      goto fail;
  }

  switch (type) {
  case NPY_DOUBLE:
    return _bstat<npy_double>(arr, mask, kappa);
  case NPY_FLOAT:
    return _bstat<npy_float>(arr, mask, kappa);
  default:
    goto fail;
  }

 fail:
  py_assert(false, 
	    PyExc_RuntimeError, "bstat dispatch failed: not implemented for this datatype/layout");
  return tuple(); // this is fake return-statement to silence the compiler
}
