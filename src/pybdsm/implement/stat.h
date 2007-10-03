#ifndef _CBDSM_STAT_H_INCLUDED_
#define _CBDSM_STAT_H_INCLUDED_

#include <boost/python.hpp>

boost::python::object bstat(boost::python::numeric::array arr, boost::python::object mask, double kappa);

#endif // _CBDSM_STAT_H_INCLUDED_
