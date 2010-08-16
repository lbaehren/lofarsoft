/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ComplexRVMFit.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/PhaseWeight.h"

#include "MEAL/ComplexRVM.h"
#include "MEAL/RotatingVectorModel.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/LevenbergMarquardt.h"

#include "templates.h"

#include <assert.h>

using namespace std;

bool Pulsar::ComplexRVMFit::verbose = false;

Pulsar::ComplexRVMFit::ComplexRVMFit()
{
  threshold = 3.0;
  chisq_map = false;

  range_alpha = range_beta = range_zeta = range (0,0);
}

//! Set the threshold below which data are ignored
void Pulsar::ComplexRVMFit::set_threshold (float sigma)
{
  threshold = sigma;
}

//! Get the threshold below which data are ignored
float Pulsar::ComplexRVMFit::get_threshold () const
{
  return threshold;
}

//! Set the data to which model will be fit
void Pulsar::ComplexRVMFit::set_observation (const PolnProfile* _data)
{
  if (_data->get_state() != Signal::Stokes)
    throw Error (InvalidParam, "Pulsar::ComplexRVMFit::set_observation"
		 "data are not Stokes parameters");

  data = _data;

  if (model->get_nstate())
    model = 0;

  std::vector< std::complex< Estimate<double> > > linear;
  data->get_linear (linear, threshold);

  const unsigned nbin = data->get_nbin();

  int max_bin = data->get_Profile(0)->find_max_bin();
  peak_phase = (max_bin+ 0.5)*2*M_PI / nbin;

  unsigned count = 0;

  double max_sin = 0;
  double delpsi_delphi = 0;

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    if (linear[ibin].real().get_variance() > 0)
    {
      data_x.push_back ( state.get_Value(count) );
      data_y.push_back ( linear[ibin] );
      
      double phase = (ibin + 0.5)*2*M_PI / nbin;
      double L = sqrt( norm(linear[ibin]).val );

      if (is_opm(phase))
      {
	if (verbose)
	  cerr << "Pulsar::ComplexRVMFit::set_observation OPM at "
	       << phase << " rad" << endl;
	L *= -1.0;
      }

      if (verbose)
        cerr << "Pulsar::ComplexRVMFit::set_observation phase=" << phase 
             << " L=" << L << endl;

      get_model()->add_state (phase, L);

      if (data_y.size() > 1)
      {
	/*

	Find the maximum slope in dpsi/dphi

	This defines both phi0 and psi0 and will be used to pick the
	first guess of alpha and zeta

	*/

	std::complex< Estimate<double> > c0 ( data_y[ data_y.size()-2 ] );
	std::complex< Estimate<double> > c1 ( data_y[ data_y.size()-1 ] );

	// sin of the angle between the vectors, weighted by their amplitude
	Estimate<double> s = c0.real()*c1.imag() - c0.imag()*c1.real();

	if ( fabs(s.val) > max_sin )
	{
	  max_sin = fabs(s.val);

	  double phi_per_bin = 2*M_PI / nbin;

	  Estimate<double> c = c0.real()*c1.real() + c0.imag()*c1.imag();

	  delpsi_delphi = 0.5 * atan2(s.val,c.val) / phi_per_bin;
	  peak_pa = 0.5 * atan2(c0.imag().val, c0.real().val);
	  peak_phase = (ibin - 0.5) * phi_per_bin;
	}
      }

      count ++;
    }
  }

  if (verbose)
    cerr << "Pulsar::ComplexRVMFit::set_observation"
      " peak phase=" << peak_phase*180/M_PI << " deg;"
      " PA=" << peak_pa*180/M_PI << " deg;"
      " dpsi/dphi=" << delpsi_delphi << endl;
  
  MEAL::RotatingVectorModel* RVM = get_model()->get_rvm();

  if (RVM->reference_position_angle->get_param(0) == 0)
  {
    cerr << "Pulsar::ComplexRVMFit::set_observation using"
      " psi0=" << peak_pa*180/M_PI << endl;

    RVM->reference_position_angle->set_param (0, peak_pa);
  }

  if (RVM->magnetic_meridian->get_param(0) == 0)
  {
    cerr << "Pulsar::ComplexRVMFit::set_observation using"
      " phi0=" << peak_phase*180/M_PI << endl;

    RVM->magnetic_meridian->set_param (0, peak_phase);
  }

  if (RVM->line_of_sight->get_param(0) == 0 &&
      RVM->magnetic_axis->get_param(0) == 0)
  {
    cerr << "Pulsar::ComplexRVMFit::set_observation using"
      " delpsi_delphi=" << delpsi_delphi << endl;

    /*

    Choose a reasonable first guess for alpha and zeta
      
    Use Equation 5 of Everett & Weisberg (2001) and alpha = (pi + beta) / 2

    */

    double alpha = acos (1/(2*delpsi_delphi));
    double beta = 2 * alpha - M_PI;

    if (verbose)
      cerr << "Pulsar::ComplexRVMFit::set_observation"
	" alpha=" << alpha << " beta=" << beta << endl;

    double zeta = alpha + beta;

    RVM->magnetic_axis->set_param (0, alpha);
    RVM->line_of_sight->set_param (0, zeta);
  }

  state.signal.connect (model, &MEAL::ComplexRVM::set_state);
  
  assert( count == model->get_nstate() );

  cerr <<"Pulsar::ComplexRVMFit::set_observation "<< count <<" bins"<< endl;
}

void Pulsar::ComplexRVMFit::add_opm (range r)
{
  if (r.first > r.second)
    r.second += 2*M_PI;

  opm.push_back (r);
}

bool between (const range& r, double p)
{
  return p > r.first && p < r.second;
}

bool Pulsar::ComplexRVMFit::is_opm (double phase) const
{
  for (unsigned i=0; i<opm.size(); i++)
    if (between(opm[i],phase) || between(opm[i],phase+2*M_PI))
      return true;
  return false;
}

//! Get the data to which model will be fit
const Pulsar::PolnProfile* Pulsar::ComplexRVMFit::get_observation () const try
{
  return data;
}
catch (Error& error)
{
  throw error += "Pulsar::ComplexRVMFit::get_observation";
}

//! Get the model to be fit to the data
MEAL::ComplexRVM* Pulsar::ComplexRVMFit::get_model ()
{
  if (!model)
    model = new MEAL::ComplexRVM;

  return model;
}

template<typename T, typename U>
double chisq (const Estimate<T,U>& e)
{
  return e.val * e.val / e.var;
}

//! Fit data to the model
void Pulsar::ComplexRVMFit::solve ()
{
  if (!model)
    model = new MEAL::ComplexRVM;

  MEAL::LevenbergMarquardt< complex<double> > fit;
  fit.verbose = MEAL::Function::verbose;

  chisq = fit.init (data_x, data_y, *model);

  if (verbose)
    cerr << "Pulsar::ComplexRVMFit::solve initial chisq = " << chisq << endl;

  float close = 1e-3;
  unsigned iter = 1;
  unsigned not_improving = 0;
  while (not_improving < 25)
  {
    if (verbose)
      cerr << "Pulsar::ComplexRVMFit::solve iteration " << iter << endl;

    float nchisq = fit.iter (data_x, data_y, *model);

    if (verbose)
      cerr << "     chisq = " << nchisq << endl;

    if (!isfinite (nchisq))
      throw Error (InvalidState, "Pulsar::ComplexRVMFit::solve",
		   "non-finite chisq");

    if (nchisq < chisq)
    {
      float diffchisq = chisq - nchisq;
      chisq = nchisq;
      not_improving = 0;

      if (diffchisq/chisq < close && diffchisq > 0)
	break;
    }
    else
      not_improving ++;

    iter ++;
  }

  check_parameters ();

  if (chisq_map)
    return;

  std::vector<std::vector<double> > covariance;
  fit.result (*model, covariance);

  nfree = 2 * model->get_nstate();
  for (unsigned iparm=0; iparm < model->get_nparam(); iparm++)
  {
    if (model->get_infit(iparm))
      nfree --;

    if (nfree == 0)
      throw Error (InvalidState, "Pulsar::ComplexRVMFit::solve",
		   "nfree <= 0");

    model->set_variance (iparm, 2.0*covariance[iparm][iparm]);
  }
}

// ensure that phi lies on 0 -> 2*PI
double twopi (double phi)
{
  const double s = 2*M_PI;
  phi /= s;
  return s*(phi-floor(phi));
}

void Pulsar::ComplexRVMFit::check_parameters ()
{
  MEAL::ComplexRVM* cRVM = get_model();
  MEAL::RotatingVectorModel* RVM = cRVM->get_rvm();

  const unsigned nstate = cRVM->get_nstate();

  double total_linear = 0.0;
  unsigned negative_count = 0;
  for (unsigned i=0; i<nstate; i++)
  {
    if (verbose)
      cerr << "Pulsar::ComplexRVMFit::check_parameters L[" << i << "]="
           << cRVM->get_linear(i).get_value() << endl;

    total_linear += cRVM->get_linear(i).get_value();

    if (cRVM->get_linear(i).get_value() <= 0)
      negative_count ++;
  }

  double PA0 = RVM->reference_position_angle->get_param(0);
  double zeta = RVM->line_of_sight->get_param(0);
  double alpha = RVM->magnetic_axis->get_param(0);
  double phi0 = RVM->magnetic_meridian->get_param(0);

  if (verbose)
    cerr << "Pulsar::ComplexRVMFit::check_parameters initial result:\n"
            "  PA_0=" << PA0*180/M_PI << " deg \n"
            "  zeta=" << zeta*180/M_PI << " deg \n"
            "  alpha=" << alpha*180/M_PI << " deg \n"
            "  phi0=" << phi0*180/M_PI << " deg" << endl;

  if (total_linear < 0.0)
  {
    if (verbose)
      cerr << "Pulsar::ComplexRVMFit::check_parameters negative gain" << endl;

    for (unsigned i=0; i<nstate; i++)
      cRVM->set_linear(i,-cRVM->get_linear(i).get_value());

    // shift the reference P.A. by 90 degrees
    PA0 += M_PI/2;
  }

  // ensure that PA0 lies on -pi/2 -> pi/2
  PA0 = atan ( tan(PA0) );

  // ensure that alpha lies on 0 -> pi
  alpha = twopi (alpha);
  if (alpha > M_PI)
  {
    if (verbose)
      cerr << "Pulsar::ComplexRVMFit::check_parameters alpha -= pi" << endl;

    alpha -= M_PI;
  }

  // ensure that zeta lies on 0 -> pi
  zeta = twopi (zeta);
  if (zeta > M_PI)
  {
    if (verbose)
      cerr << "Pulsar::ComplexRVMFit::check_parameters zeta=2pi-zeta" << endl;

    zeta = 2*M_PI - zeta;
    phi0 -= M_PI;
  }

  DEBUG("alpha=" << alpha << " zeta=" << zeta << " neg=" << negative_count);

  // ensure that phi0 lies on 0 -> 2pi
  phi0 = twopi (phi0);

  RVM->magnetic_axis->set_param(0, alpha);
  RVM->line_of_sight->set_param(0, zeta);
  RVM->magnetic_meridian->set_param(0, phi0);
  RVM->reference_position_angle->set_param (0, PA0);
}

#if 0
  double chisq2 = 0;
  double deg=180/M_PI;

  for (unsigned i=0; i < data_x.size(); i++)
  {
    data_x[i].apply();
    std::complex<double> model = cRVM->evaluate();
    double gain = cRVM->get_linear(i).get_value();

    cerr << i << " data=" << data_y[i] << " arg=" << arg(data_y[i])*deg/2
	 << " model=" << model << " PA=" << RVM->evaluate()*deg 
	 << " Q=" << gain*cos(2*RVM->evaluate())
	 << " U=" << gain*sin(2*RVM->evaluate()) << endl;

    data_y[i] -= model;

    chisq2 += ::chisq(data_y[i].real()) + ::chisq(data_y[i].imag());
  }

  cerr << "CHISQ=" << chisq2 << endl;
#endif

void Pulsar::ComplexRVMFit::global_search (unsigned nalpha, unsigned nzeta)
{
  MEAL::ComplexRVM* cRVM = get_model();
  MEAL::RotatingVectorModel* RVM = cRVM->get_rvm();

  const unsigned nstate = cRVM->get_nstate();
  if (!nstate)
    throw Error (InvalidState, "Pulsar::ComplexRVMFit::global_search",
		 "no data");

  vector<double> linear (nstate);
  for (unsigned i=0; i<nstate; i++)
    linear[i] = cRVM->get_linear(i).get_value();

  bool map_beta = range_beta.first != range_beta.second;
  if (map_beta)
    range_zeta = range_beta;

  double step_alpha = M_PI/nalpha;
  double step_zeta = M_PI/nzeta;

  if ( range_alpha.first == range_alpha.second )
  {
    range_alpha.first = step_alpha/2;
    range_alpha.second = M_PI;
  }
  else
  {
    step_alpha = (range_alpha.second - range_alpha.first) / (nalpha - 1);
    // ensure that for loop ends on range_alpha.second within rounding error
    range_alpha.second += step_alpha / 2;
  }

  if ( range_zeta.first == range_zeta.second )
  {
    range_zeta.first = step_zeta/2;
    range_zeta.second = M_PI;
  }
  else
  {
    step_zeta = (range_zeta.second - range_zeta.first) / (nzeta - 1);
    // ensure that for loop ends on range_zeta.second within rounding error
    range_zeta.second += step_zeta / 2;
  }

  float best_chisq = 0.0;
  float best_alpha = 0.0;
  float best_zeta = 0.0;
  
  vector<double> chisq_surface;
  unsigned chisq_index = 0;

  if (chisq_map)
  {
    RVM->magnetic_axis->set_infit (0, false);
    RVM->line_of_sight->set_infit (0, false);
    chisq_surface.resize (nalpha * nzeta);
  }

  for (double alpha=range_alpha.first; 
       alpha < range_alpha.second; 
       alpha += step_alpha)
  {
    for (double zeta=range_zeta.first; 
	 zeta < range_zeta.second;
	 zeta += step_zeta) try
    {
      RVM->magnetic_axis->set_value (alpha);

      if (map_beta)
	RVM->line_of_sight->set_value (alpha + zeta);
      else
	RVM->line_of_sight->set_value (zeta);

      // ensure that each attempt starts with the same guess
      RVM->magnetic_meridian->set_value (peak_phase);
      RVM->reference_position_angle->set_value (peak_pa);
      for (unsigned i=0; i<nstate; i++)
	cRVM->set_linear(i, linear[i]);

      solve ();

      if (chisq_map)
      {
	assert (chisq_index < chisq_surface.size());
	chisq_surface[chisq_index] = chisq;
      }

      if (best_chisq == 0 || chisq < best_chisq)
      {
	// cerr << "current best chisq=" << chisq << endl;
	best_chisq = chisq;
	best_alpha = alpha;
	best_zeta = zeta;
      }

      chisq_index ++;
    }
    catch (Error& error)
    {
      if (verbose)
	cerr << "exception thrown alpha=" << alpha << " zeta=" << zeta << endl
	     << error.get_message() << endl;

      chisq_index ++;
    }
  }

  if (chisq_map)
  {
    assert (chisq_index == chisq_surface.size());
    double min=0, max=0;
    minmax (chisq_surface, min, max);
    for (unsigned i=0; i<chisq_index; i++)
      chisq_surface[i] -= min;

    chisq_index = 0;

    for (double alpha=range_alpha.first; 
	 alpha < range_alpha.second; 
	 alpha += step_alpha)
    {
      for (double zeta=range_zeta.first; 
	   zeta < range_zeta.second;
	   zeta += step_zeta)
      {
	const double deg = 180/M_PI;
	cout << alpha*deg << " " << zeta*deg 
	     << " " << chisq_surface[chisq_index] << endl;
	chisq_index ++;
      }
      cout << endl;
    }
  }

  // cerr << "BEST chisq=" << best_chisq << endl;

  RVM->magnetic_axis->set_value (best_alpha);

  if (map_beta)
    RVM->line_of_sight->set_value (best_alpha + best_zeta);
  else
    RVM->line_of_sight->set_value (best_zeta);

  // ensure that each attempt starts with the same guess
  RVM->magnetic_meridian->set_value (peak_phase);
  RVM->reference_position_angle->set_value (peak_pa);
  for (unsigned i=0; i<nstate; i++)
    cRVM->set_linear(i, linear[i]);

  chisq_map = false;

  solve ();

  for (unsigned i=0; i<nstate; i++)
    if (cRVM->get_linear(i).get_value() < 0)
      cerr << "orthogonal mode L=" << cRVM->get_linear(i).val 
	   << " phase=" << cRVM->get_phase(i)*0.5/M_PI  << endl;
}
