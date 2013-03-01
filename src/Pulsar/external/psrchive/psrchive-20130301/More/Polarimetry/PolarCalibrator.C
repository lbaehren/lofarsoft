/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/CalibratorType.h"

#include "MEAL/Rotation.h"
#include "Pauli.h"

using namespace std;

//! Construct from an single PolnCal Pulsar::Archive
Pulsar::PolarCalibrator::PolarCalibrator (const Archive* archive) 
  : ReferenceCalibrator (archive)
{
  type = Calibrator::Type::factory (this);
}

Pulsar::PolarCalibrator::~PolarCalibrator ()
{
}


/*! Given the measured Stokes parameters of the linear calibrator
  noise diode, \f$ S_{\rm cal}=(I=1,Q=0,U=1,V=0)\f$, and the
  unpolarized system temperature, \f$ S_{\rm sys}=(T,0,0,0) \f$, this
  method solves for the instrumental gain, boost, and two of the three
  basis rotations that may be compared to the differential phase, and
  the orientation of the feed.

  \post source and sky will be modified to represent the un-boosted
  Stokes parameters.  */
void solve (MEAL::Polar* polar,
	    Quaternion<Estimate<double>, Hermitian>& source,
	    Quaternion<Estimate<double>, Hermitian>& sky)
{
  // Assuming that the off pulse radiation is unpolarized, the boost
  // component of the system response is simply the square root of the
  // off-pulse Stokes (normalized by the sqrt of the determinant)
  Quaternion<Estimate<double>, Hermitian> boost = sqrt( sky );
  boost /= sqrt( det(boost) );

#ifdef _DEBUG
  Quaternion<double,Hermitian> b;
  for (unsigned i=0; i<4; i++)
    b[i] = boost[i].val;
  Jones< double > j = convert (b);

  std::complex<double > d;
  Quaternion<double, Hermitian> hq;
  Quaternion<double, Unitary> uq;

  ::polar (d, hq, uq, j);

  cerr << "hq=" << hq << endl;
  cerr << "uq=" << uq << endl;
  cerr << "d=" << d << endl;
#endif

  for (unsigned i=0; i<3; i++)
    polar->set_boost (i, boost[i+1]);

  // Undo the boost of the on-pulse coherencies
  // remember: herm(unboost) = unboost
  Jones< Estimate<double> > unboost = convert (inv (boost));
  source = real(convert(unboost * source * unboost));
  sky = real(convert(unboost * sky * unboost));

  Jones< Estimate<double> > jones_cal = convert (source);

  // The differential phase is given by the phase of j01, regardless of basis
  //  Estimate<double> phi = arg (jones_cal.j01);
  Estimate<double> phi = atan2(jones_cal.j01.imag(),jones_cal.j01.real());
  polar->set_rotation (0, 0.5 * phi);
  
  // This solution tells nothing about the rotation about the U-axis
  polar->set_rotation (1, 0.0);

  /* This works only if the feeds are linear.  Otherwise, differential phase
     and feed orientation are degenerate with each other. */
  Estimate<double> theta = atan2 ( source.s1, source.s2/cos(phi) );
  polar->set_rotation (2, 0.5 * theta);

  /* After correcting the cal for the boost, the total intensity represents
     the total gain of the system in units of calibrator flux */

  Estimate<double> G = source.s0;
  polar->set_gain (sqrt(G));
}

//! Return the system response as determined by the Polar Transformation
::MEAL::Complex2*
Pulsar::PolarCalibrator::solve (const vector<Estimate<double> >& source,
				const vector<Estimate<double> >& sky)
{
  if ( source.size() != 4 || sky.size() != 4 )
    throw Error (InvalidParam, "Pulsar::PolarCalibrator::solve",
		 "source.size=%d or sky.size=%d != 4", 
		 source.size(), sky.size());

  if (source_set)
    throw Error (InvalidState, "Pulsar::PolarCalibrator::solve",
		 "arbitrary reference source not yet implemented");

  if (verbose > 2)
    cerr << "Pulsar::PolarCalibrator::solve" << endl;

  // Convert the coherency vectors into Stokes parameters.  
  Stokes< Estimate<double> > s_source = coherency( convert(source) );
  Stokes< Estimate<double> > s_sky = coherency( convert (sky) );

  // Convert to the natural basis
  Quaternion<Estimate<double>,Hermitian> q_source = natural( s_source );
  Quaternion<Estimate<double>,Hermitian> q_sky = natural( s_sky );

  Reference::To<MEAL::Polar> polar = new MEAL::Polar;

  ::solve (polar, q_source, q_sky);

  return polar.release();
}


Pulsar::PolarCalibrator::Info::Info (const PolnCalibrator* cal) 
  : PolnCalibrator::Info (cal)
{
}

string Pulsar::PolarCalibrator::Info::get_title () const
{
  return "Polar Decomposition of Jones Matrix";
}

//! Return the number of parameter classes
unsigned Pulsar::PolarCalibrator::Info::get_nclass () const
{
  return 3; 
}

//! Return the name of the specified class
string Pulsar::PolarCalibrator::Info::get_name (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return "G (c\\d0\\u)";
  case 1:
    return "sinh\\(2128)\\.m\\b\\u \\(0832)";
  case 2:
    return "sin\\gf\\.n\\b\\u\\(0832)";
  default:
    return "";
  }
}
  
//! Return the number of parameters in the specified class
unsigned Pulsar::PolarCalibrator::Info::get_nparam (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return 1;
  case 1:
  case 2:
    return 3;
  default:
    return 0;
  }
}

template<typename T, typename U>
const Estimate<T,U> sinc (const Estimate<T,U>& u)
{
  T val = 1.0;
  T dval_du = 0.0;

  if (u.val != 0.0)
  {
    val = ::sin(u.val) / u.val; 
    dval_du = (::cos(u.val) - val) / u.val;
  }

  return Estimate<T,U> (val, dval_du*dval_du*u.var);
}

//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::PolarCalibrator::Info::get_param (unsigned ichan, unsigned iclass,
					  unsigned iparam) const
{
  if( ! calibrator->get_transformation_valid (ichan) )
    return 0;

  if (iclass < 2)
    return PolnCalibrator::Info::get_param (ichan, iclass, iparam);

  const MEAL::Polar* polar
    = dynamic_cast<const MEAL::Polar*> 
    ( calibrator->get_transformation (ichan) );

  if (!polar)
    throw Error (InvalidState, "Pulsar::PolarCalibrator::Info::get_param",
		 "transformation[%d] is not an Polar", ichan);

  const MEAL::Rotation* rotation = polar->get_rotation_transformation();

  vector< Jones<double> > grad;
  Jones<double> R = rotation->evaluate (&grad);

  Quaternion<double, Unitary> q = real( unitary(R) );
  double val = q[iparam + 1];
  double var = 0.0;
  for (unsigned i=0; i<3; i++)
  {
    Quaternion<double, Unitary> dq_dri = real( unitary(grad[i]) );
    double dval_dri = dq_dri[iparam + 1];
    var += dval_dri * dval_dri * rotation->get_variance(i);
  }

  return Estimate<float> (val, var);
}

Pulsar::PolarCalibrator::Info*
Pulsar::PolarCalibrator::get_Info () const
{
  return new PolarCalibrator::Info (this);
}
