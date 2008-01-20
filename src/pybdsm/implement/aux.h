#ifndef _AUX_H_INCLUDED
#define _AUX_H_INCLUDED

/**
   Miscellaneous usefull routines
*/

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>

inline void py_assert(bool cond, PyObject *exc, const char *msg)
{
  if(!cond) {
    PyErr_SetString(exc, msg);
    throw boost::python::error_already_set();
  }
}

#endif // _AUX_H_INCLUDED
