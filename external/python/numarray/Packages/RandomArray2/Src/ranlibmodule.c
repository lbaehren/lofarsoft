#include "Python.h"

#include <stdio.h>

#include "arrayobject.h"
#include "ranlib.h"

static PyObject *ErrorObject;

/* ----------------------------------------------------- */

static PyObject*
get_continuous_random(int num_dist_params, PyObject* self, PyObject* args, void* fun) {
  PyObject *ao, *bo, *co;
  PyArrayObject *op, *aa = NULL, *ba = NULL, *ca = NULL;
  double *out_ptr;
  int i, n=-1, sz;
  float a = 0.0, b = 0.0, c = 0.0;
  float *ap = NULL, *bp = NULL, *cp = NULL;

  switch(num_dist_params) {
  case 0:
    if( !PyArg_ParseTuple(args, "|i", &n) )
      return NULL;
    break;
  case 1:
    if( !PyArg_ParseTuple(args, "O|i", &ao, &n) )
      return NULL;
    aa = (PyArrayObject*)PyArray_ContiguousFromObject(ao, PyArray_FLOAT, 0, 0);
    if (!aa) {
      PyErr_SetString(PyExc_RuntimeError, ("cannot convert input"));
      return NULL;
    }      
    break;
  case 2:
    if( !PyArg_ParseTuple(args, "OO|i", &ao, &bo, &n) )
      return NULL;
    aa = (PyArrayObject*)PyArray_ContiguousFromObject(ao, PyArray_FLOAT, 0, 0);
    ba = (PyArrayObject*)PyArray_ContiguousFromObject(bo, PyArray_FLOAT, 0, 0);
    if (!aa || !ba) {
      Py_XDECREF(aa);
      Py_XDECREF(ba);
      PyErr_SetString(PyExc_RuntimeError, ("cannot convert input"));
      return NULL;
    }
  case 3:
    if( !PyArg_ParseTuple(args, "OOO|i", &ao, &bo, &co, &n) )
      return NULL;
    aa = (PyArrayObject*)PyArray_ContiguousFromObject(ao, PyArray_FLOAT, 0, 0);
    ba = (PyArrayObject*)PyArray_ContiguousFromObject(bo, PyArray_FLOAT, 0, 0);
    ca = (PyArrayObject*)PyArray_ContiguousFromObject(co, PyArray_FLOAT, 0, 0);
    if (!aa || !ba || !ca) {
      Py_XDECREF(aa);
      Py_XDECREF(ba);
      Py_XDECREF(ca);
      PyErr_SetString(PyExc_RuntimeError, ("cannot convert input"));
      return NULL;
    }
    break;
  }

  if( n == -1 )
    n = 1;

  if (aa) {
    sz = PyArray_Size((PyObject*)aa);
    ap = (float*) aa->data;
    if (sz == 1) {
      a = *ap;
      Py_XDECREF(aa);
      aa = NULL;
    } else if (sz != n) {
      Py_XDECREF(aa);
      Py_XDECREF(ba);
      Py_XDECREF(ca);
      PyErr_SetString(PyExc_RuntimeError, ("array size not correct"));
      return NULL;
    }
  }
  if (ba) {
    sz = PyArray_Size((PyObject*)ba);
    bp = (float*) ba->data;
    if (sz == 1) {
      b = *bp;
      Py_XDECREF(ba);
      ba = NULL;
    } else if (sz != n) {
      Py_XDECREF(aa);
      Py_XDECREF(ba);
      Py_XDECREF(ca);
      PyErr_SetString(PyExc_RuntimeError, ("array size not correct"));
      return NULL;
    }
  }
  if (ca) {
    sz = PyArray_Size((PyObject*)ca);
    cp = (float*) ca->data;
    if (sz == 1) {
      c = *cp;
      Py_XDECREF(ca);
      ca = NULL;
    } else if (sz != n) {
      Py_XDECREF(aa);
      Py_XDECREF(ba);
      Py_XDECREF(ca);
      PyErr_SetString(PyExc_RuntimeError, ("array size not correct"));
      return NULL;
    }
  }

  /* Create a 1 dimensional array of length n of type double */
  op = (PyArrayObject*) PyArray_FromDims(1, &n, PyArray_DOUBLE);
  if( op == NULL )
    return NULL;

  out_ptr = (double *) op->data;
  for(i=0; i<n; i++) {
    switch(num_dist_params) {
    case 0:
      *out_ptr = (double) ((float (*)(void)) fun)();
      break;
    case 1:
      if (aa) a = *ap++;
      *out_ptr = (double) ((float (*)(float)) fun)(a);
      break;
    case 2:
      if (aa) a = *ap++;
      if (ba) b = *bp++;
      *out_ptr = (double) ((float (*)(float, float)) fun)(a,b);
      break;
    case 3:
      if (aa) a = *ap++;
      if (ba) b = *bp++;
      if (ca) c = *cp++;
      *out_ptr = (double) ((float (*)(float, float, float)) fun)(a,b,c);
      break;
    }
    out_ptr++;
  }

  Py_XDECREF(aa);
  Py_XDECREF(ba);
  Py_XDECREF(ca);

  return PyArray_Return(op);
}


static PyObject*
get_discrete_scalar_random(int num_integer_args, PyObject* self, PyObject* args, void* fun) {
  long int_arg = 0;
  int n=-1, i, sz;
  long* out_ptr, *ip = NULL;
  PyArrayObject* op, *ia = NULL, *fa = NULL;
  float float_arg = 0.0, *fp = NULL;
  PyObject *io, *fo;

  switch( num_integer_args ) {
  case 0:
    if( !PyArg_ParseTuple(args, "O|i", &fo, &n) ) {
      return NULL;
    }
    fa = (PyArrayObject*)PyArray_ContiguousFromObject(fo, PyArray_FLOAT, 0, 0);
    if (!fa) {
      PyErr_SetString(PyExc_RuntimeError, ("cannot convert input"));
      return NULL;
    }      
    break;
  case 1:
    if( !PyArg_ParseTuple(args, "OO|i", &io, &fo, &n) ) {
      return NULL;
    }
    ia = (PyArrayObject*)PyArray_ContiguousFromObject(io, PyArray_LONG, 0, 0);
    fa = (PyArrayObject*)PyArray_ContiguousFromObject(fo, PyArray_FLOAT, 0, 0);
    if (!fa || !ia) {
      Py_XDECREF(fa);
      Py_XDECREF(ia);
      PyErr_SetString(PyExc_RuntimeError, ("cannot convert input"));
      return NULL;
    }
    break;
  }

  if (fa) {
    sz = PyArray_Size((PyObject*)fa);
    fp = (float*) fa->data;
    if (sz == 1) {
      float_arg = *fp;
      Py_XDECREF(fa);
      fa = NULL;
    } else if (sz != n) {
      Py_XDECREF(ia);
      Py_XDECREF(fa);
      PyErr_SetString(PyExc_RuntimeError, ("array size not correct"));
      return NULL;
    }
  }
  if (ia) {
    sz = PyArray_Size((PyObject*)ia);
    ip = (long*) ia->data;
    if (sz == 1) {
      int_arg = *ip;
      Py_XDECREF(ia);
      ia = NULL;
    } else if (sz != n) {
      Py_XDECREF(ia);
      Py_XDECREF(fa);
      PyErr_SetString(PyExc_RuntimeError, ("array size not correct"));
      return NULL;
    }
  }

  if( n==-1 ) {
    n = 1;
  }
  
  /* Create a 1 dimensional array of length n of type long */
  op = (PyArrayObject*) PyArray_FromDims(1, &n, PyArray_LONG);
  if( op == NULL ) {
    return NULL;
  }
  
  out_ptr = (long*) op->data;
  for(i=0; i<n; i++) {
    switch( num_integer_args ) {
    case 0:
      if (fa) float_arg = *fp++;
      *out_ptr = ((long (*)(float)) fun)(float_arg);
      break;
    case 1:
      if (ia) int_arg = *ip++;
      if (fa) float_arg = *fp++;
      *out_ptr = ((long (*)(long, float)) fun)(int_arg, float_arg);
      break;
    }
    out_ptr++;
  }

  Py_XDECREF(fa);
  Py_XDECREF(ia);
  
  return PyArray_Return(op);
}


static char random_sample__doc__[] ="";

static PyObject *
random_sample(PyObject *self, PyObject *args) {
  return get_continuous_random(0, self, args, ranf);
}


static char standard_normal__doc__[] ="";

static PyObject *
standard_normal(PyObject *self, PyObject *args) {
  return get_continuous_random(0, self, args, snorm);
}


static char beta__doc__[] ="";

static PyObject *
beta(PyObject *self, PyObject *args) {
  return get_continuous_random(2, self, args, genbet);
}


static char gamma__doc__[] ="";

static PyObject *
/* there is a function named `gamma' in some libm's */
_gamma(PyObject *self, PyObject *args) { 
  return get_continuous_random(2, self, args, gengam);
}


static char f__doc__[] ="";

static PyObject *
f(PyObject *self, PyObject *args) {
  return get_continuous_random(2, self, args, genf);
}


static char noncentral_f__doc__[] ="";

static PyObject *
noncentral_f(PyObject *self, PyObject *args) {
  return get_continuous_random(3, self, args, gennf);
}


static char noncentral_chisquare__doc__[] ="";

static PyObject *
noncentral_chisquare(PyObject *self, PyObject *args) {
  return get_continuous_random(2, self, args, gennch);
}


static char chisquare__doc__[] ="";

static PyObject *
chisquare(PyObject *self, PyObject *args) {
  return get_continuous_random(1, self, args, genchi);
}


static char binomial__doc__[] ="";

static PyObject *
binomial(PyObject *self, PyObject *args) {
  return get_discrete_scalar_random(1, self, args, ignbin);
}


static char negative_binomial__doc__[] ="";

static PyObject *
negative_binomial(PyObject *self, PyObject *args) {
  return get_discrete_scalar_random(1, self, args, ignnbn);
}

static char poisson__doc__[] ="";

static PyObject *
poisson(PyObject *self, PyObject *args) {
  return get_discrete_scalar_random(0, self, args, ignpoi);
}


static char multinomial__doc__[] ="";

static PyObject*
multinomial(PyObject* self, PyObject* args) {
  int n=-1, i;
  long num_trials, num_categories;
  char* out_ptr;
  PyArrayObject* priors_array;
  PyObject* priors_object;
  PyArrayObject* op;
  int out_dimensions[2];

  if( !PyArg_ParseTuple(args, "lO|i", &num_trials, &priors_object, &n) ) {
    return NULL;
  }
  priors_array = (PyArrayObject*) PyArray_ContiguousFromObject(priors_object, PyArray_FLOAT, 1, 1);
  if( priors_array == NULL ) {
    return NULL;
  }
  num_categories = priors_array->dimensions[0]+1;
  if( n==-1 ) {
    n = 1;
  }
  
  /* Create an n by num_categories array of long */
  out_dimensions[0] = n;
  out_dimensions[1] = num_categories;
  op = (PyArrayObject*) PyArray_FromDims(2, out_dimensions, PyArray_LONG);
  if( op == NULL ) {
    return NULL;
  }
  
  out_ptr = op->data;
  for(i=0; i<n; i++) {
    genmul(num_trials, (float*)(priors_array->data), num_categories, (long*) out_ptr);
    out_ptr += op->strides[0];
  }
  Py_XDECREF( priors_array );
  return PyArray_Return(op);
}


static PyObject *
random_set_seeds(PyObject *self, PyObject *args)
{
  long seed1, seed2;

  if (!PyArg_ParseTuple(args, "ll", &seed1, &seed2)) return NULL;


  setall(seed1, seed2);
  if (PyErr_Occurred ()) return NULL;
  Py_INCREF(Py_None);
  return (PyObject *)Py_None;
}


static PyObject *
random_get_seeds(PyObject *self, PyObject *args)
{
  long seed1, seed2;

  if (!PyArg_ParseTuple(args, "")) return NULL;

  getsd(&seed1, &seed2);

  return Py_BuildValue("ll", seed1, seed2);
}


/* Missing interfaces to */
/* exponential (genexp), multivariate normal (genmn), 
   normal (gennor), permutation (genprm), uniform (genunf),
   standard exponential (sexpo), standard gamma (sgamma) */

/* List of methods defined in the module */

static struct PyMethodDef random_methods[] = {
 {"sample",     random_sample,          1,      random_sample__doc__},
 {"standard_normal", standard_normal,   1,      standard_normal__doc__},
 {"beta",	beta,                   1,      beta__doc__},
 {"gamma",	_gamma,                  1,      gamma__doc__},
 {"f",	        f,                      1,      f__doc__},
 {"noncentral_f", noncentral_f,         1,      noncentral_f__doc__},
 {"chisquare",	chisquare,              1,      chisquare__doc__},
 {"noncentral_chisquare", noncentral_chisquare,
                                        1,      noncentral_chisquare__doc__},
 {"binomial",	binomial,               1,      binomial__doc__},
 {"negative_binomial", negative_binomial,
                                        1,      negative_binomial__doc__},
 {"multinomial", multinomial,           1,      multinomial__doc__},
 {"poisson",    poisson,                1,      poisson__doc__},
 {"set_seeds",  random_set_seeds,       1, },
 {"get_seeds",  random_get_seeds,       1, },
 {NULL,		NULL}		/* sentinel */
};


/* Initialization function for the module (*must* be called initranlib) */

static char random_module_documentation[] = 
""
;

DL_EXPORT(void)
initranlib2(void)
{
	PyObject *m, *d;

	/* Create the module and add the functions */
	m = Py_InitModule4("ranlib2", random_methods,
		random_module_documentation,
		(PyObject*)NULL,PYTHON_API_VERSION);

	/* Import the array object */
	import_array();

	/* Add some symbolic constants to the module */
	d = PyModule_GetDict(m);
	ErrorObject = PyString_FromString("ranlib2.error");
	PyDict_SetItemString(d, "error", ErrorObject);

	/* XXXX Add constants here */
	
	/* Check for errors */
	if (PyErr_Occurred())
		Py_FatalError("can't initialize module ranlib2");
}
