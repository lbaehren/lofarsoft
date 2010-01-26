/***************************************************************************
 *
 *   Copyright (C) 2003-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/PolnCalibratorExtension.h"

#include "Pulsar/Receiver.h"
#include "Pulsar/BasisCorrection.h"
#include "Pulsar/BackendCorrection.h"
#include "Pulsar/FeedExtension.h"

#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/Profile.h"

#include "Pauli.h"
#include "Error.h"

#include "interpolate.h"
#include "templates.h"
#include "median_smooth.h"

#ifdef sun
#include <ieeefp.h>
#endif

using namespace std;

/*! 
  If a Pulsar::Archive is provided, and if it contains a
  PolnCalibratorExtension, then the constructed instance can be
  used to calibrate other Pulsar::Archive instances.
*/
Pulsar::PolnCalibrator::PolnCalibrator (const Archive* archive)
{
  if (!archive)
    return;

  // store the calibrator archive
  set_calibrator (archive);

  // store the related Extension, if any
  poln_extension = archive->get<PolnCalibratorExtension>();
  if (poln_extension)
    extension = poln_extension;

  // store the Receiver Extension, if any
  receiver = archive->get<Receiver>();

  // store the Feed Extension, if any
  feed = archive->get<FeedExtension>();

  filenames.push_back( archive->get_filename() );

  built = false;

  observation_nchan = 0;
}

//! Copy constructor
Pulsar::PolnCalibrator::PolnCalibrator (const PolnCalibrator& calibrator)
{
  built = false;
  observation_nchan = 0;
}

//! Destructor
Pulsar::PolnCalibrator::~PolnCalibrator ()
{
}

//! Set the number of frequency channels in the response array
void Pulsar::PolnCalibrator::set_response_nchan (unsigned nchan)
{
  if (built && response.size() == nchan)
    return;

  build (nchan);
}

//! Get the number of frequency channels in the response array
unsigned Pulsar::PolnCalibrator::get_response_nchan () const
{
  if (!built || response.size() == 0)
    const_cast<PolnCalibrator*>(this)->build();

  return response.size ();
}

Jones<float> Pulsar::PolnCalibrator::get_response (unsigned ichan) const
{
  if (!built || response.size() == 0)
    const_cast<PolnCalibrator*>(this)->build();

  if (ichan >= response.size())
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::get_response",
		 "ichan=%d >= nchan=%d", ichan, response.size());

  return response[ichan];
}


//! Return true if the Receiver is set
bool Pulsar::PolnCalibrator::has_Receiver () const
{
  return receiver;
}

//! Return the Receiver
const Pulsar::Receiver* Pulsar::PolnCalibrator::get_Receiver () const
{
  return receiver;
}

/*!  

  Some calibrators (e.g. HybridCalibrator) may be capable of shrinking
  the size of the transformation array to match the number of
  frequency channels in the observation.  If this is done, and the
  next observation to be calibrated has more channels than the first,
  then the transformation array should be recomputed to match the new
  frequency resolution.

  However, to date, the decision to recompute the transformation array
  has been based on it having zero size.  It is not possible to
  compare tranformation.size() with get_nchan(), because
  PolnCalibrator::get_nchan makes the same comparison and returns
  transformation.size().

  Rather than add a new method that must be implemented by all of the
  derived classes, this new method gives shrink-capable classes the
  opportunity to communicate this feature.

  By default, the method returns zero.  Otherwise, it should return
  the maximum possible size of the transformation array that can be
  supported.

*/
unsigned Pulsar::PolnCalibrator::get_maximum_nchan ()
{
  return 0;
}


//! Get the number of frequency channels in the calibrator
unsigned Pulsar::PolnCalibrator::get_nchan () const
{
  if (transformation.size() == 0)
    setup_transformation();

  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::get_nchan nchan="
         << transformation.size() << endl;

  return transformation.size();
}

//! Return the system response for the specified channel
bool Pulsar::PolnCalibrator::get_transformation_valid (unsigned ichan) const
{
  if (transformation.size() == 0)
    setup_transformation();

  if (ichan >= transformation.size())
    throw Error (InvalidParam,
		 "Pulsar::PolnCalibrator::get_transformation_valid",
		 "ichan=%d >= nchan=%d", ichan, transformation.size());

  return transformation[ichan];
}

//! Return the system response for the specified channel
void Pulsar::PolnCalibrator::set_transformation_invalid (unsigned ichan)
{
  if (transformation.size() == 0)
    setup_transformation();

  if (ichan >= transformation.size())
    throw Error (InvalidParam,
		 "Pulsar::PolnCalibrator::set_transformation_invalid",
		 "ichan=%d >= nchan=%d", ichan, transformation.size());

  transformation[ichan] = 0;

}

//! Return the system response for the specified channel
const ::MEAL::Complex2*
Pulsar::PolnCalibrator::get_transformation (unsigned ichan) const
{
  if (transformation.size() == 0)
    setup_transformation();

  if (ichan >= transformation.size())
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::get_transformation",
		 "ichan=%d >= nchan=%d", ichan, transformation.size());

  return transformation[ichan];
}

//! Return the system response for the specified channel
MEAL::Complex2*
Pulsar::PolnCalibrator::get_transformation (unsigned ichan)
{
  if (transformation.size() == 0)
    setup_transformation();

  if (ichan >= transformation.size())
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::get_transformation",
		 "ichan=%d >= nchan=%d", ichan, transformation.size());

  return transformation[ichan];
}

//! Return true if parameter covariances are stored
bool Pulsar::PolnCalibrator::has_covariance () const
{
  return (covariance.size() == get_nchan());
}

//! Return the covariance matrix vector for the specified channel
void Pulsar::PolnCalibrator::get_covariance (unsigned ichan,
					     std::vector<double>& c) const
{
  assert (ichan < covariance.size());
  c = covariance[ichan];
}

//! Return true if least squares minimization solvers are available
bool Pulsar::PolnCalibrator::has_solver () const
{
  return poln_extension && poln_extension->get_has_solver();
}

class TmpSolver : public MEAL::LeastSquares
{
public:
  TmpSolver (float _chisq, unsigned _nfree)
  {
    best_chisq = _chisq; nfree = _nfree; solved = true;
  }
  string get_name () const { return "TmpSolver"; }
};

//! Return the transformation for the specified channel
const MEAL::LeastSquares*
Pulsar::PolnCalibrator::get_solver (unsigned ichan) const
{
  if (!has_solver())
    throw Error (InvalidState, "Pulsar::PolnCalibrator::get_solver",
		 "no solver available");

  tmp_solver.resize( get_nchan() );

  if (!tmp_solver[ichan])
  {
    const PolnCalibratorExtension::Transformation* xform 
      = poln_extension->get_transformation (ichan);

    tmp_solver[ichan] = new TmpSolver (xform->get_chisq(), xform->get_nfree());
  }

  return tmp_solver[ichan];
}

MEAL::LeastSquares* Pulsar::PolnCalibrator::get_solver (unsigned ichan)
{
  const PolnCalibrator* thiz = this;
  return const_cast<MEAL::LeastSquares*> (thiz->get_solver (ichan));
}

void Pulsar::PolnCalibrator::setup_transformation () const try
{
  if (receiver)
    Pauli::basis().set_basis( receiver->get_basis() );
  const_cast<PolnCalibrator*>(this)->calculate_transformation();
}
catch (Error& error) {
  throw error += "Pulsar::PolnCalibrator::setup_transformation";
}

//! Derived classes can create and fill the transformation array
void Pulsar::PolnCalibrator::calculate_transformation () try
{
  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::calculate_transformation" << endl;

  if (!poln_extension)
    throw Error (InvalidState,
		 "Pulsar::PolnCalibrator::calculate_transformation",
		 "no PolnCalibratorExtension available");

  unsigned nchan = poln_extension->get_nchan();
  transformation.resize (nchan);

  if (poln_extension->get_has_covariance())
    covariance.resize(nchan);

  for (unsigned i=0; i < nchan; i++)
  {
    transformation[i] = new_transformation (poln_extension, i);
    if (poln_extension->get_has_covariance())
      poln_extension->get_transformation(i)->get_covariance(covariance[i]);
  }
}
catch (Error& error) {
  throw error += "Pulsar::PolnCalibrator::calculate_transformation";
}



/*!  
  The unitary component of each Jones matrix is parameterized by a
  half angle, i.e. -pi/2 < angle < pi/2, and any wrap between +/- pi/2
  will appear to the Fourier transform as a discontinuity in phase.

  To compensate for this, the unitary component of each Jones matrix
  is first squared, so that the angle spans 2pi smoothly.  The result
  is then decomposed and the square root of each unitary component is
  taken before recomposing the final result.
*/
template<typename T>
void interpolate( std::vector< Jones<T> >& to,
		  const std::vector< Jones<T> >& from,
		  const vector<bool>* bad )
{
  const complex<T> zero (0.0);

  complex<T> determinant;
  Quaternion<T, Hermitian> hermitian;
  Quaternion<T, Unitary> unitary;

  unsigned ichan = 0;
  const unsigned nchan_from = from.size();
  const unsigned nchan_to = to.size();

  vector< Jones<T> > copy (nchan_from);

  for (ichan=0; ichan<nchan_from; ichan++)
  {
    if (det(from[ichan]) == zero)
    {
      copy[ichan] = from[ichan];
    }
    else
    {
      polar (determinant, hermitian, unitary, from[ichan]);
      unitary *= unitary;
      copy[ichan] = determinant * (hermitian * unitary);
    }
  }

  fft::interpolate (to, copy);
  
  unsigned factor = nchan_to / nchan_from;
  
  for (ichan=0; ichan<nchan_to; ichan++)
  {
    unsigned orig_ichan = ichan / factor;

    if (orig_ichan >= nchan_from || (bad && (*bad)[orig_ichan]))
    {
      to[ichan] = 0;
      continue;
    }
    
    polar (determinant, hermitian, unitary, to[ichan]);
    unitary = sqrt(unitary);

    // I don't remember why I did this, sorry ... WvS 16 Oct 2008
    if (unitary[0] < 0.05)
      polar (determinant, hermitian, unitary, from[orig_ichan]);

    to[ichan] = determinant * (hermitian * unitary);
  }
}


void Pulsar::PolnCalibrator::build (unsigned nchan) try
{
  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::build transformation size="
	 << transformation.size() << " nchan=" << nchan << endl;

  bool transformation_built = transformation.size() > 0;

  unsigned maximum_nchan = get_maximum_nchan ();

  /*
    If the derived class can shrink the transformation array, then
    it may be necessary to rebuild the transformation array to match
    the number of frequency channels in the observation
  */

  if (maximum_nchan)
  {
    /*
      target: the number of channels in the observation, up to the 
      maximum number of channels supported by the derived class.
    */
    unsigned target_nchan = std::min (nchan, maximum_nchan);

    if (transformation.size() != target_nchan)
      transformation_built = false;
  }

  if (!built || !transformation_built)
  {
    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::build setup transformation" << endl;

    observation_nchan = nchan;
    setup_transformation();
  }

  if (!nchan)
    nchan = transformation.size();

  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::build nchan=" << nchan 
	 << " transformation.size=" << transformation.size() << endl;

  response.resize( transformation.size() );

  vector<bool> bad ( transformation.size(), false );
  unsigned bad_count = 0;

  unsigned ichan=0;

  for (ichan=0; ichan < response.size(); ichan++)
  {
    if (transformation[ichan])
    {
      // sanity check of model parameters
      unsigned nparam = transformation[ichan]->get_nparam();
      for (unsigned iparam=0; iparam < nparam; iparam++)
        if ( !finite(transformation[ichan]->get_param(iparam)) )
	{
	  if (verbose > 2)
	    cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan
		 << " " << transformation[ichan]->get_param_name(iparam)
		 << " not finite" << endl;

	  bad[ichan] = true;
        }

      double normdet = norm(det( transformation[ichan]->evaluate() ));

      if ( normdet < 1e-9 || !finite(normdet) )
      {
	if (verbose > 2)
	  cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan <<
	    " faulty response" << endl;

	bad[ichan] = true;
      }
      else
      {
        response[ichan] = transformation[ichan]->evaluate();

        if (verbose > 2)
          cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan <<
            " response=\n" << response[ichan] << endl;
      }
    }
    else
    {
      if (verbose > 2) cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan 
                        << " no transformation" << endl;

      bad[ichan] = true;
    }

    if (bad[ichan])
    {
      response[ichan] = 0.0;
      bad_count ++;
    }
  }

  /* If there are bad channels and the solution must be interpolated to
     a higher number of frequency channels, then the solution must be 
     patched up */

  bool need_patching = bad_count && nchan > response.size();

  if (median_smoothing)
  {
    unsigned window = unsigned (float(response.size()) * median_smoothing);
    
    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::build median smoothing window = "
	   << window << " channels" << endl;

    // even a 3-channel sort can zap a single channel birdie
    if (window < 3)
      window = 3;

    throw Error (InvalidState, "Pulsar::PolnCalibrator::build",
		 "median smoothing of Jones matrices not yet implemented");
  }
  else if (interpolating || need_patching)
  {
    if (!interpolating)
      cerr << "Pulsar::PolnCalibrator::build patch up"
	" before interpolation in Fourier domain" << endl;

    for (ichan = 0; ichan < response.size(); ichan++)
    {
      if (!bad[ichan])
	continue;

      unsigned ifind;
	
      if (verbose > 2)
	cerr << "Pulsar::PolnCalibrator::build interpolating ichan="
	     << ichan << endl;
      
      // find preceding good
      for (ifind=response.size()+ichan-1; ifind > ichan; ifind--)
	if (!bad[ifind%response.size()])
	  break;
      
      if (ifind == ichan)
	throw Error (InvalidState, "Pulsar::PolnCalibrator::build",
		     "no good data");
      
      Jones<float> left = response[ifind%response.size()];
      
      // find next good
      for (ifind=ichan+1; ifind < nchan+ichan; ifind++)
	if (!bad[ifind%response.size()])
	  break;
      
      if (ifind == ichan)
	throw Error (InvalidState, "Pulsar::PolnCalibrator::build",
		     "no good data");
      
      Jones<float> right = response[ifind%response.size()];
      
      // TO-DO: It is probably more accurate to first form Mueller
      // matrices and average these as a function of frequency, in order
      // to describe any instrumental bandwidth depolarization.

      response[ichan] = float(0.5) * (left + right);
    }
  }

  if (nchan < response.size())
  {
    // TO-DO: It is probably more accurate to first form Mueller
    // matrices and average these as a function of frequency, in order
    // to describe any instrumental bandwidth depolarization.

    unsigned factor = response.size() / nchan;
    if (nchan * factor != response.size())
      cerr << "Pulsar::PolnCalibrator::build WARNING calibrator nchan="
	   << response.size() << " mod requested nchan=" << nchan << " != 0" 
	   << endl;

    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::build integrate by " << factor << endl;

    // integrate the Jones matrices
    scrunch (response, factor, true);
  }

  if (nchan > response.size())
  {
    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::build interpolating from nchan="
	   << response.size() << " to " << nchan << endl;

    vector< Jones<float> > out_response (nchan);
    interpolate (out_response, response, &bad);
    response = out_response;
  }

  Jones<double> feed_xform = 1.0;

  // if known, add the feed transformation
  if (feed)
  {
    feed_xform = feed->get_transformation();
    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::build known feed:\n"
           << feed_xform << endl;
  }

  Jones<double> rcvr_xform = 1.0;

  // if known, add the receiver transformation
  if (receiver)
  {
    BasisCorrection basis_correction;
    rcvr_xform = basis_correction (receiver);
    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::build known receiver:\n"
           << rcvr_xform << endl;
  }

  for (ichan=0; ichan < nchan; ichan++)
  {
    if (!get_valid(ichan))
    {
      response[ichan] = 0.0;
      continue;
    }

    // add the known feed transformation
    response[ichan] *= feed_xform;

    // add the known receiver transformation
    response[ichan] *= rcvr_xform;

    // invert: the response must undo the effect of the instrument
    response[ichan] = inv (response[ichan]);
  }

  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::build built" << endl;

  built = true;
}
catch (Error& error)
{
  throw error += "Pulsar::PolnCalibrator::build";
}

//! Return true if the response for the specified channel is valid
bool Pulsar::PolnCalibrator::get_valid (unsigned ichan) const
{
  if (ichan >= response.size())
    return false;

  static const complex<float> zero (0.0);
  return det(response[ichan]) != zero;
}

void Pulsar::PolnCalibrator::calibration_setup (Archive* arch)
{
  string reason;
  if (!get_calibrator()->calibrator_match (arch, reason))
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::calibration_setup",
		 "mismatch between calibrator\n\t" 
		 + get_calibrator()->get_filename() +
                 " and\n\t" + arch->get_filename() + reason);

  if (response.size() != arch->get_nchan()) try
  {
    build( arch->get_nchan() );
  }
  catch (Error& error)
  {
    throw error += "Pulsar::PolnCalibrator::calibration_setup";
  }
}

/*! Upon completion, the flux of the archive will be normalized with
  respect to the flux of the calibrator, such that a FluxCalibrator
  simply scales the archive by the calibrator flux. */
void Pulsar::PolnCalibrator::calibrate (Archive* arch) try
{
  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::calibrate" << endl;

  calibration_setup (arch);

  if (arch->get_npol() == 4)
  {
    BackendCorrection correct_backend;
    correct_backend (arch);

    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::calibrate Archive::transform" <<endl;

    arch->transform (response);
    arch->set_poln_calibrated (true);

    if (receiver)
    {
      Receiver* rcvr = arch->get<Receiver>();
      if (!rcvr)
	throw Error (InvalidState, "Pulsar::PolnCalibrator::calibrate",
		     "Archive has no Receiver Extension");
      
      rcvr->set_basis_corrected (true);
    }
  }
  else if (arch->get_npol() == 1)
  {
    if (Archive::verbose)
      cerr << "Pulsar::PolnCalibrator::calibrate WARNING"
	" calibrating only absolute gain" << endl;

    unsigned nsub = arch->get_nsubint ();
    unsigned nchan = arch->get_nchan ();

    for (unsigned isub=0; isub < nsub; isub++)
    {
      Integration* subint = arch->get_Integration (isub);
      for (unsigned ichan=0; ichan < nchan; ichan++)
      {
	double gain = abs(det( response[ichan] ));
	Profile* profile = subint->get_Profile (0, ichan);

	profile -> scale (gain);
	profile -> set_weight ( profile->get_weight() / gain );
      }
    }
  }
  else
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::calibrate",
		 "Archive::npol == %d not yet implemented", arch->get_npol());

  arch->set_scale (Signal::ReferenceFluxDensity);
}
catch (Error& error)
{
  throw error += "Pulsar::PolnCalibrator::calibrate";
}


const Pulsar::Calibrator::Type* Pulsar::PolnCalibrator::get_type () const
{
  if (type)
    return type;

  if (!poln_extension)
    throw Error (InvalidState,
		 "Pulsar::PolnCalibrator::get_type",
		 "type not set and no extension available");

  return poln_extension->get_type();
}


Pulsar::CalibratorExtension*
Pulsar::PolnCalibrator::new_Extension () const
{
  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::new_Extension" << endl;

  return new PolnCalibratorExtension (this);
}


//! Constructor
Pulsar::PolnCalibrator::Info::Info (const PolnCalibrator* cal)
{
  nparam = 0;

  if (!cal)
    return;

  calibrator = cal;
  
  unsigned nchan = cal->get_nchan ();

  // find the first valid transformation
  const MEAL::Complex2* xform = 0;
  for (unsigned ichan = 0; ichan < nchan; ichan++)
    if ( cal->get_transformation_valid (ichan) )
    {
      xform = cal->get_transformation (ichan);
      break;
    }

  if (!xform)
    return;

  nparam = xform->get_nparam();
}
      
/*! Each parameter of the Transformation is treated as a separate class. */
unsigned Pulsar::PolnCalibrator::Info::get_nclass () const
{
  return nparam;
}

/*! The name of each parameter is unknown */
string Pulsar::PolnCalibrator::Info::get_title () const
{
  return "Unknown Calibrator Parameters";
}

/*! The name of each parameter is unknown */
string Pulsar::PolnCalibrator::Info::get_name (unsigned iclass) const
{
  return "unknown";
}

/*! Each parameter of the Transformation is treated as a separate class. */
unsigned Pulsar::PolnCalibrator::Info::get_nparam (unsigned iclass) const
{
  return 1;
}
      
//! Return the estimate of the specified parameter
Estimate<float> Pulsar::PolnCalibrator::Info::get_param (unsigned ichan, 
							 unsigned iclass,
							 unsigned iparam) const
{
  if (! calibrator->get_transformation_valid(ichan) )
  {
    if (verbose > 2) cerr << "Pulsar::PolnCalibrator::Info::get_param"
		   " invalid ichan=" << ichan << endl;
    return 0;
  }

  unsigned offset = 0;
  for (unsigned jclass=1; jclass<=iclass; jclass++)
    offset += get_nparam (jclass-1);

  return calibrator->get_transformation(ichan)->get_Estimate(iparam+offset);
}


//! Return the colour index
int Pulsar::PolnCalibrator::Info::get_colour_index (unsigned iclass,
						    unsigned iparam) const
{
  unsigned colour_offset = 1;
  if (get_nparam (iclass) == 3)
    colour_offset = 2;

  return colour_offset + iparam;
}

//! Return the graph marker
int Pulsar::PolnCalibrator::Info::get_graph_marker (unsigned iclass, 
						    unsigned iparam) const
{
  if (iparam == 0 || iparam > 3)
    return -1;

  iparam --;

  int nice_markers[3] = { 2, 5, 4 };

  return nice_markers[iparam];
}


Pulsar::Calibrator::Info* Pulsar::PolnCalibrator::get_Info () const
{
  return PolnCalibrator::Info::create (this);
}

