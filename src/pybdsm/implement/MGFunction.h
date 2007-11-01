/*
 *  MGFunction.h
 *
 *  Created by Oleksandr Usov on 15/10/2007.
 *
 */

#ifndef _MGFUNCTION_H_INCLUDED
#define _MGFUNCTION_H_INCLUDED

#include <vector>
#include <utility>
#include <boost/python.hpp>

using namespace boost::python;

/**
Multi-Gaussian function.

This class allows you to manage multi-gaussian function and
implements all math required to use it for fitting.
*/

class MGFunction
{
public:
  MGFunction(numeric::array data, numeric::array mask, double weight);
  ~MGFunction();

  ////////////////////////////////
  // High-level Python interface
  ////////////////////////////////
  enum Gtype {
    G_Reduced_Gaussian = 3,
    G_Gaussian = 6,
  };

  void py_reset();
  void py_add_gaussian(Gtype type, object parameters);
  void py_remove_gaussian(int idx);
  tuple py_get_gaussian(int idx);
  void py_set_gaussian(int idx, object parameters);
  list py_get_parameters();
  void py_set_parameters(object parameters);
  list py_get_errors();
  tuple py_find_peak();
  static void register_class();

  ////////////////////////////////
  // Low-level interface for fitting routines
  ////////////////////////////////
  int data_size() const { return m_ndata; }
  int gaul_size() const { return m_gaul.size(); }
  int parameters_size() const { return m_npar; }
  int gaussian_size(unsigned idx) const { assert(idx < m_gaul.size()); return m_gaul[idx]; }

  void get_parameters(double *buf) const;
  void set_parameters(const double *buf);
  void get_nlin_parameters(double *buf) const;
  void set_nlin_parameters(const double *buf);
  void set_lin_parameters(const double *buf);

  void data(double *buf) const; /// data (unmasked pixels only)
  void fcn_value(double *buf) const; /// value of the function (unmasked pixels only)
  void fcn_diff(double *buf) const; /// data - value (unmasked pixels only)
  void fcn_partial_value(double *buf) const; /// values of single (unscaled) gaussians
  void fcn_gradient(double *buf) const; /// fcn_value gradient
  void fcn_diff_gradient(double *buf) const; /// fcn_diff gradient
  void fcn_transposed_gradient(double *buf) const; /// fcn_value gradient (transposed)
  void fcn_diff_transposed_gradient(double *buf) const; /// fcn_diff gradient (transposed)
  void fcn_partial_gradient(double *buf) const; /// fcn_partial_value gradient

  double chi2() const; /// calculate chi^2 of the residual image

protected:
  std::vector<int> m_gaul; /// gaussians types (lengths)
  std::vector<std::vector<double> > m_parameters; /// parameters of gaussians
  std::vector<std::vector<double> > m_errors; /// error bars

  double m_weight; /// weight for chi^2 calculation
  unsigned m_npar; /// number of fitted parameters
  unsigned m_ndata; /// number of fitted (unmasked) datapoints

  numeric::array m_data; /// data array
  numeric::array m_mask; /// mask array

private:
  MGFunction(MGFunction const &); // prevent copying of the MGFunction objects

  /// these are used to cache intermediate calculations
  template<class T>
    void __update_dcache() const;
  void _update_dcache() const; /// update cached data
  void _update_fcache() const; /// update cached function
  unsigned long _cksum() const; /// cksum of m_parameters


  typedef struct {
    int x1, x2;
    double d;
  } dcache_t;

  typedef struct {
    double sn, cs, f1, f2, val;
  } fcache_t;

  typedef std::vector<dcache_t>::iterator dcache_it;
  typedef std::vector<fcache_t>::iterator fcache_it;

  static std::vector<dcache_t> mm_data; // data cache
  static std::vector<fcache_t> mm_fcn; // cache for function/gradient evaluations

  // these are used to verify whether cached values are up-to-date
  static void *mm_obj;
  static unsigned long mm_cksum;
};

#endif // _MGFUNCTION_H_INCLUDED
