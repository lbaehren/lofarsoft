/*!
  \file MGFunction.cc
  
  \ingroup pybdsm
  
  \author Oleksandr Usov
  
  \date 15/10/2007
*/

#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle
#define NO_IMPORT_ARRAY

#include "aux.h"
#include "MGFunction.h"

#include <ext/algorithm>
#include <num_util/num_util.h>
#include <cfloat>

using namespace __gnu_cxx;
using namespace std;
namespace n = num_util;

vector<MGFunction::dcache_t> MGFunction::mm_data;
vector<MGFunction::fcache_t> MGFunction::mm_fcn;
void * MGFunction::mm_obj = 0;
unsigned long MGFunction::mm_cksum = -1;
static const double deg = M_PI/180;


//////////////////////////
// low-level interface
//////////////////////////

/// copy all tunable parameters to buf
void MGFunction::get_parameters(double *buf) const
{
  double *chk = buf;
  for (unsigned i = 0; i < m_gaul.size(); ++i) {
    copy_n(m_parameters[i].begin(), m_gaul[i], buf);
    buf += m_gaul[i];
  }
  assert(buf - chk == (int)m_npar);
}

/// set all tunable parameters from buf
void MGFunction::set_parameters(const double *buf)
{
  const double *chk = buf;
  for(unsigned i = 0; i < m_gaul.size(); ++i) {
    copy_n(buf, m_gaul[i], m_parameters[i].begin());
    buf += m_gaul[i];
  }
  assert(buf - chk == (int)m_npar);
}

/// copy all tunable non-linear parameters to buf
void MGFunction::get_nlin_parameters(double *buf) const
{
  double *chk = buf;
  for (unsigned i = 0; i < m_gaul.size(); ++i) {
    copy_n(m_parameters[i].begin() + 1, m_gaul[i] - 1, buf);
    buf += m_gaul[i] - 1;
  }
  assert(buf - chk == (int)(m_npar - m_gaul.size()));
}

/// set all tunable non-linear parameters from buf
void MGFunction::set_nlin_parameters(const double *buf)
{
  const double *chk = buf;
  for(unsigned i = 0; i < m_gaul.size(); ++i) {
    copy_n(buf, m_gaul[i] - 1, m_parameters[i].begin() + 1);
    buf += m_gaul[i] - 1 ;
  }
  assert(buf - chk == (int)(m_npar - m_gaul.size()));
}

/// set all tunable linear parameters from buf
void MGFunction::set_lin_parameters(const double *buf)
{
  for(unsigned i = 0; i < m_gaul.size(); ++i, ++buf)
    m_parameters[i][0] = *buf;
}

/// copy (unmasked) data into buf
void MGFunction::data(double *buf) const
{
  _update_fcache();
  double *chk = buf;

  for (dcache_it d = mm_data.begin(); d != mm_data.end(); ++d, ++buf)
    *buf = d->d;
  assert(buf - chk == (int)m_ndata);
}

/// evaluate mgfunction (unmasked pixels only) and store it in buf
void MGFunction::fcn_value(double *buf) const
{
  _update_fcache();
  double *chk = buf;

  fcache_it f = mm_fcn.begin();
  for (unsigned didx = 0; didx < m_ndata; ++didx, ++buf) {
    *buf = 0;
    for (unsigned gidx = 0; gidx < m_gaul.size(); ++gidx, ++f)
      *buf += m_parameters[gidx][0] * f->val;
  }
  assert(buf - chk == (int)m_ndata);
}

/// evaluate (data - mgfunction) and store it in buf
void MGFunction::fcn_diff(double *buf) const
{
  _update_fcache();
  double *chk = buf;

  fcache_it f = mm_fcn.begin();
  for (dcache_it d = mm_data.begin(); d != mm_data.end(); ++d, ++buf) {
    *buf = d->d;
    for (unsigned gidx = 0; gidx < m_gaul.size(); ++gidx, ++f)
      *buf -= m_parameters[gidx][0] * f->val;
  }
  assert(buf - chk == (int)m_ndata);
}

/// evaluate non-linear part of mgfunction
/// each sub-function is evaluated for all data points and stored contiguously
void MGFunction::fcn_partial_value(double *buf) const
{
  _update_fcache();

  fcache_it f = mm_fcn.begin();
  unsigned didx, gidx = 0;
  for (didx = 0; didx < m_ndata; ++didx) {
    for (gidx = 0; gidx < m_gaul.size(); ++gidx, ++f)
      buf[gidx * m_ndata + didx] = f->val;
  }
  assert((gidx - 1) * m_ndata + didx == m_ndata * m_gaul.size());
}

/// gradient of fcn_value; all derivatives are evaluated for each data point and stored all together
void MGFunction::fcn_gradient(double *buf) const
{
  _update_fcache();
  double *chk = buf;

  fcache_it f = mm_fcn.begin();
  for (unsigned didx = 0; didx < m_ndata; ++didx)
    for (unsigned gidx = 0; gidx < m_gaul.size(); ++gidx, ++f) {
      const vector<double> &p = m_parameters[gidx];
      double cs = f->cs;
      double sn = f->sn;
      double f1 = f->f1;
      double f2 = f->f2;
      double V  = p[0] * f->val;

      *(buf++) = f->val;
      *(buf++) = (V * (f1*cs/p[3] - f2*sn/p[4]));
      *(buf++) = (V * (f1*sn/p[3] + f2*cs/p[4]));
      if (m_gaul[gidx] == G_Gaussian) {
	*(buf++) = (V * f1*f1/p[3]);
	*(buf++) = (V * f2*f2/p[4]);
	*(buf++) = (V * deg * f1 * f2 * (p[3]/p[4] - p[4]/p[3]));
      }
    }
  assert(buf - chk == (int)(m_ndata * m_npar));
}

/// gradient of fcn_diff; all derivatives are evaluated for each data point and stored all together
void MGFunction::fcn_diff_gradient(double *buf) const
{
  _update_fcache();
  double *chk = buf;

  fcache_it f = mm_fcn.begin();
  for (unsigned didx = 0; didx < m_ndata; ++didx)
    for (unsigned gidx = 0; gidx < m_gaul.size(); ++gidx, ++f) {
      const vector<double> &p = m_parameters[gidx];
      double cs = f->cs;
      double sn = f->sn;
      double f1 = f->f1;
      double f2 = f->f2;
      double V  = - p[0] * f->val; // EXTRA MINUS SIGN INCLUDED

      *(buf++) = - f->val; // EXTRA MINUS SIGN INCLUDED
      *(buf++) = (V * (f1*cs/p[3] - f2*sn/p[4]));
      *(buf++) = (V * (f1*sn/p[3] + f2*cs/p[4]));
      if (m_gaul[gidx] == G_Gaussian) {
	*(buf++) = (V * f1*f1/p[3]);
	*(buf++) = (V * f2*f2/p[4]);
	*(buf++) = (V * deg * f1 * f2 * (p[3]/p[4] - p[4]/p[3]));
      }
    }
  assert(buf - chk == (int)(m_ndata * m_npar));
}

/// gradient of fcn_value; each derivative is evaluated for all datapoints and stored contiguously
void MGFunction::fcn_transposed_gradient(double *buf) const
{
  _update_fcache();

  fcache_it f = mm_fcn.begin();
  unsigned didx, gidx = 0, ggidx = 0;
  for (didx = 0; didx < m_ndata; ++didx) {
    ggidx = 0;
    for (gidx = 0; gidx < m_gaul.size(); ++gidx, ++f) {
      const vector<double> &p = m_parameters[gidx];
      double cs = f->cs;
      double sn = f->sn;
      double f1 = f->f1;
      double f2 = f->f2;
      double V  = p[0] * f->val;

      buf[(0 + ggidx)*m_ndata + didx] = f->val;
      buf[(1 + ggidx)*m_ndata + didx] = (V * (f1*cs/p[3] - f2*sn/p[4]));
      buf[(2 + ggidx)*m_ndata + didx] = (V * (f1*sn/p[3] + f2*cs/p[4]));
      if (m_gaul[gidx] == G_Gaussian) {
	buf[(3 + ggidx)*m_ndata + didx] = (V * f1*f1/p[3]);
	buf[(4 + ggidx)*m_ndata + didx] = (V * f2*f2/p[4]);
	buf[(5 + ggidx)*m_ndata + didx] = (V * deg * f1 * f2 * (p[3]/p[4] - p[4]/p[3]));
      }
      ggidx += m_gaul[gidx];
    }
  }
  assert(ggidx * m_ndata == m_ndata * m_npar);
}

/// gradient of fcn_diff; each derivative is evaluated for all datapoints and stored contiguously
void MGFunction::fcn_diff_transposed_gradient(double *buf) const
{
  _update_fcache();

  fcache_it f = mm_fcn.begin();
  unsigned didx, gidx = 0, ggidx = 0;
  for (didx = 0; didx < m_ndata; ++didx) {
    ggidx = 0;
    for (gidx = 0; gidx < m_gaul.size(); ++gidx, ++f) {
      const vector<double> &p = m_parameters[gidx];
      double cs = f->cs;
      double sn = f->sn;
      double f1 = f->f1;
      double f2 = f->f2;
      double V  = - p[0] * f->val; // EXTRA MINUS SIGN INCLUDED

      buf[(0 + ggidx)*m_ndata + didx] = - f->val; // EXTRA MINUS SIGN INCLUDED
      buf[(1 + ggidx)*m_ndata + didx] = (V * (f1*cs/p[3] - f2*sn/p[4]));
      buf[(2 + ggidx)*m_ndata + didx] = (V * (f1*sn/p[3] + f2*cs/p[4]));
      if (m_gaul[gidx] == G_Gaussian) {
	buf[(3 + ggidx)*m_ndata + didx] = (V * f1*f1/p[3]);
	buf[(4 + ggidx)*m_ndata + didx] = (V * f2*f2/p[4]);
	buf[(5 + ggidx)*m_ndata + didx] = (V * deg * f1 * f2 * (p[3]/p[4] - p[4]/p[3]));
      }
      ggidx += m_gaul[gidx];
    }
  }
  assert(ggidx * m_ndata == m_ndata * m_npar);
}

/// gradient of fcn_partial_value; each derivative is evaluated for all datapoints and stored contiguously
void MGFunction::fcn_partial_gradient(double *buf) const
{
  _update_fcache();
  double *chk = buf;

  fcache_it f = mm_fcn.begin();
  unsigned didx, gidx = 0, ggidx = 0;
  for (didx = 0; didx < m_ndata; ++didx) {
    ggidx = 0;
    for (gidx = 0; gidx < m_gaul.size(); ++gidx, ++f) {
      const vector<double> &p = m_parameters[gidx];
      double cs = f->cs;
      double sn = f->sn;
      double f1 = f->f1;
      double f2 = f->f2;
      double V  = f->val;

      buf[(0 + ggidx)*m_ndata + didx] = (V * (f1*cs/p[3] - f2*sn/p[4]));
      buf[(1 + ggidx)*m_ndata + didx] = (V * (f1*sn/p[3] + f2*cs/p[4]));
      if (m_gaul[gidx] == G_Gaussian) {
	buf[(2 + ggidx)*m_ndata + didx] = (V * f1*f1/p[3]);
	buf[(3 + ggidx)*m_ndata + didx] = (V * f2*f2/p[4]);
	buf[(4 + ggidx)*m_ndata + didx] = (V * deg * f1 * f2 * (p[3]/p[4] - p[4]/p[3]));
      }
      ggidx += m_gaul[gidx] - 1;
    }
  }
  assert(ggidx * m_ndata == m_ndata * (m_npar - m_gaul.size()));
}

double MGFunction::chi2() const
{
  _update_fcache();

  double res = 0;
  fcache_it f = mm_fcn.begin();
  for (dcache_it d = mm_data.begin(); d != mm_data.end(); ++d) {
    double v = d->d;
    for (unsigned gidx = 0; gidx < m_gaul.size(); ++gidx, ++f)
      v -= m_parameters[gidx][0] * f->val;
    v /= m_weight;
    res += v*v;
  }

  return res;
}


//////////////////////////////
// cache-handling
//////////////////////////////
unsigned long MGFunction::_cksum() const
{
  typedef unsigned long T;

  T res = 0;

  for (unsigned i = 0; i < m_gaul.size(); ++i) {
    T *buf = (T *)&m_parameters[i][0];
    int size = m_parameters[i].size() * sizeof(double) / sizeof(T);
    for (int j = 0; j < size; ++j)
      res ^= buf[j];
  }

  return res;
}

template<class T>
void MGFunction::__update_dcache() const
{
  PyObject *data = (PyObject *)m_data.ptr();
  PyObject *mask = (PyObject *)m_mask.ptr();
  vector<int> shape = n::shape(m_data);
  dcache_t t;

  mm_data.clear();
  mm_data.reserve(m_ndata);

  for (int i = 0; i < shape[0]; ++i)
    for (int j = 0; j < shape[1]; ++j)
      if (!*(npy_bool *)PyArray_GETPTR2(mask, i, j)) {
	t.x1 = i;
	t.x2 = j;
	t.d  = *(T *)PyArray_GETPTR2(data, i, j);
	mm_data.push_back(t);
      }

  assert(mm_data.size() == m_ndata);
}

void MGFunction::_update_dcache() const
{
  PyArray_TYPES type = n::type(m_data);

  switch (type) {
  case NPY_DOUBLE:
    return __update_dcache<npy_double>();
  case NPY_FLOAT:
    return __update_dcache<npy_float>();
  default:
    py_assert(false, 
	      PyExc_TypeError, "Incorrect data datatype");
  }
}

void MGFunction::_update_fcache() const
{
  unsigned long cksum = _cksum();
  unsigned ngaul = m_gaul.size();

  // reallocate function/data arrays
  if (mm_fcn.size() != m_ndata * ngaul || mm_obj != (void *)this) {
    if (mm_obj != (void *)this) {
      _update_dcache();
      mm_obj = (void *)this;
    }

    mm_fcn.resize(m_ndata * ngaul);
    mm_cksum = cksum-1; // force wrong mm_cksum
  }

  if (mm_cksum != cksum) {
    fcache_it f = mm_fcn.begin();
    for (dcache_it d = mm_data.begin(); d != mm_data.end(); ++d)
      for (unsigned gidx = 0; gidx < ngaul; ++gidx, ++f) {
	const vector<double> &p = m_parameters[gidx];
	int x1 = d->x1;
	int x2 = d->x2;
	double cs = cos(p[5]*deg);
	double sn = sin(p[5]*deg);
	double f1 = ( (x1 - p[1]) * cs + (x2 - p[2]) * sn)/p[3];
	double f2 = (-(x1 - p[1]) * sn + (x2 - p[2]) * cs)/p[4];
	double v  = exp((f1*f1 + f2*f2)/(-2.L));

	f->sn = sn; f->cs = cs;
	f->f1 = f1; f->f2 = f2;
	f->val = v;
      }

    mm_cksum = cksum;
  }
}
