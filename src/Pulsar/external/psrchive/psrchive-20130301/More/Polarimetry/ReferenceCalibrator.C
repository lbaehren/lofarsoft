/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ReferenceCalibrator.h"
#include "Pulsar/BackendCorrection.h"

#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"

#include "Pulsar/Receiver.h"
#include "Pulsar/Backend.h"

#include "MEAL/Invariant.h"
#include "Pauli.h"
#include "Error.h"

#include "interpolate.h"
#include "median_smooth.h"

#include <assert.h>

using namespace std;

Pulsar::Option<bool>
Pulsar::ReferenceCalibrator::smooth_bandpass
("ReferenceCalibrator::smooth_bandpass", false);

Pulsar::ReferenceCalibrator::~ReferenceCalibrator ()
{
}

//! Filter access to the calibrator
void Pulsar::ReferenceCalibrator::set_calibrator (const Archive* archive)
{
  if (verbose > 2)
    cerr << "Pulsar::ReferenceCalibrator::set_calibrator" << endl;

  if ( !archive->type_is_cal() )
    throw Error (InvalidParam, "Pulsar::ReferenceCalibrator::set_calibrator",
		 "Archive='" + archive->get_filename() + "' is not a Cal");
  
  // Here the decision is made about full stokes or dual band observations.
  Signal::State state = archive->get_state();

  bool fullStokes = state == Signal::Stokes 
    || state == Signal::Coherence 
    || state == Signal::PseudoStokes;
  bool calibratable = fullStokes || state == Signal::PPQQ;

  if (!calibratable)
    throw Error (InvalidParam, "Pulsar::ReferenceCalibrator::set_calibrator", 
		 "Archive='" + archive->get_filename() + "'\n\t"
		 "invalid state=" + State2string(state));

  Archive* clone = 0;

  if (fullStokes && state != Signal::Coherence)
  {
    clone = archive->clone();
    clone -> convert_state (Signal::Coherence);
  }

  BackendCorrection correct_backend;
  if (correct_backend.required(archive))
  {
    if (!clone)
      clone = archive->clone();
    correct_backend (clone);
  }

  if (clone)
    archive = clone;

  PolnCalibrator::set_calibrator (archive);
}

Pulsar::ReferenceCalibrator::ReferenceCalibrator (const Archive* archive)
  : PolnCalibrator (archive)
{
  if (!archive)
    throw Error (InvalidState, "Pulsar::ReferenceCalibrator", "no Archive");

  if (verbose > 2)
    cerr << "Pulsar::ReferenceCalibrator archive nchan="
	 << archive->get_nchan() << endl;

  set_calibrator (archive);
  requested_nchan = get_calibrator()->get_nchan();

  if (receiver)
  {
    Pauli::basis().set_basis( receiver->get_basis() );    
    Stokes<double> cal = receiver->get_reference_source ();
    if (verbose > 2)
      cerr << "Pulsar::ReferenceCalibrator reference source " << cal << endl;
    set_reference_source (cal);
  }
}

//! Set the Stokes parameters of the reference source
void Pulsar::ReferenceCalibrator::set_reference_source
 (const Stokes< Estimate<double> >& stokes)
{
  reference_source = stokes;
  source_set = true;
}

//! Get the Stokes parameters of the reference source
Stokes< Estimate<double> >
Pulsar::ReferenceCalibrator::get_reference_source () const
{
  if (source_set)
    return reference_source;

  return Stokes< Estimate<double> > (1.0, 0.0, 1.0, 0.0);
}

//! Set the number of frequency channels in the response array
void Pulsar::ReferenceCalibrator::set_nchan (unsigned nchan)
{
  if (requested_nchan == nchan)
    return;

  requested_nchan = nchan;

  // ensure that the transformation matrix is re-computed
  transformation.resize (0);

  PolnCalibrator::set_response_nchan (nchan);
}

void distribute_variance (vector<Estimate<double> >& to,
                          const vector<Estimate<double> >& from)
{
  unsigned ratio = to.size() / from.size();
  assert (to.size() % from.size() == 0);

  unsigned ito = 0;

  for (unsigned ifrom=0; ifrom<from.size(); ifrom++)
    for (unsigned i=0; i<ratio; i++)
    {
      // variance is increased by the distribution of information
      to[ito].var = from[ifrom].var * ratio;
      ito ++;
    }
}

/*!
  \param integration the calibrator Integration from which to derive levels
  \param request_nchan the desired frequency resolution
  \retval cal_hi the mean levels of the calibrator hi state
  \retval cal_lo the mean levels of the calibrator lo state
*/
void Pulsar::ReferenceCalibrator::get_levels
(const Integration* integration, unsigned request_nchan,
 vector<vector<Estimate<double> > >& cal_hi,
 vector<vector<Estimate<double> > >& cal_lo) try 
{
  if (!integration)
    throw Error (InvalidState,
                 "Pulsar::ReferenceCalibrator::get_levels",
                 "no calibrator Integration");

  unsigned nchan = integration->get_nchan();

  if (verbose > 2)
    cerr << "Pulsar::ReferenceCalibrator::get_levels Integration"
      " nchan=" << nchan << " required nchan=" << request_nchan << endl;

  Reference::To<Integration> clone;

  if (nchan > request_nchan)
  {
    clone = integration->clone();
    clone->expert()->fscrunch( nchan/request_nchan );
    nchan = clone->get_nchan();
    integration = clone;
  }
 
  integration->cal_levels (cal_hi, cal_lo);

  unsigned ipol, npol = integration->get_npol();

  for (unsigned ichan=0; ichan<nchan; ichan++)
    if (integration->get_weight (ichan) == 0)
    {
      for (ipol=0; ipol<npol; ipol++)
	cal_hi[ipol][ichan] = cal_lo[ipol][ichan] = 0.0;
    }

  if (smooth_bandpass)
  {
    unsigned window = unsigned (integration->get_nchan() * median_smoothing);

    if (verbose > 2)
      cerr << "Pulsar::ReferenceCalibrator::get_levels median window = "
	   << window << " channels" << endl;

    // even a 3-window sort can zap a single channel birdie
    if (window < 3)
      window = 3;

    for (ipol=0; ipol < npol; ipol++)
    {
      fft::median_smooth (cal_lo[ipol], window);
      fft::median_smooth (cal_hi[ipol], window);
    }
  }

  if (integration->get_nchan() == request_nchan)
    return;

  // make hi and lo the right size of cal_hi and cal_lo
  vector<vector<Estimate<double> > > hi (npol);
  vector<vector<Estimate<double> > > lo (npol);
 
  for (ipol=0; ipol < npol; ipol++)
  {
    lo[ipol].resize (request_nchan);
    hi[ipol].resize (request_nchan);

    if (verbose > 2)
      cerr << "Pulsar::ReferenceCalibrator::get_levels interpolate from "
	   << cal_lo[ipol].size() << " to " << lo[ipol].size() << endl;

    fft::interpolate (lo[ipol], cal_lo[ipol]);
    distribute_variance (lo[ipol], cal_lo[ipol]);
    fft::interpolate (hi[ipol], cal_hi[ipol]);
    distribute_variance (hi[ipol], cal_lo[ipol]);
  }

  cal_lo = lo;
  cal_hi = hi;
}
catch (Error& error)
{
  throw error += "Pulsar::ReferenceCalibrator::get_levels";
}

/*! This method takes care of averaging the calibrator levels from multiple
  sub-integrations */
void Pulsar::ReferenceCalibrator::get_levels
(const Archive* archive, unsigned request_nchan,
 vector<vector<Estimate<double> > >& cal_hi,
 vector<vector<Estimate<double> > >& cal_lo)
{
  if (!archive)
    throw Error (InvalidState,
		 "Pulsar::ReferenceCalibrator::get_levels",
		 "no calibrator Archive");

  unsigned nsub = archive->get_nsubint();
  unsigned npol = archive->get_npol();
  unsigned nchan = archive->get_nchan();

  if (verbose > 2)
    cerr << "Pulsar::ReferenceCalibrator::get_levels nsub=" << nsub 
	 << " npol=" << npol << " nchan=" << nchan << endl;

  nchan = request_nchan;

  // the mean calibrator hi and lo levels from the PolnCal archive
  vector<vector<MeanEstimate<double> > > total_hi;
  vector<vector<MeanEstimate<double> > > total_lo;

  for (unsigned isub=0; isub<nsub; isub++)
  {
    const Integration* integration = archive->get_Integration(isub);

    get_levels (integration, nchan, cal_hi, cal_lo);

    if (nsub > 1)
    {
      if (isub == 0)
      {
	total_hi.resize (cal_hi.size());
	total_lo.resize (cal_lo.size());
	for (unsigned ipol=0; ipol<npol; ipol++)
	{
	  total_hi[ipol].resize (nchan);
	  total_lo[ipol].resize (nchan);
	}
      }
      for (unsigned ipol=0; ipol<npol; ipol++)
      {
	for (unsigned ichan=0; ichan<nchan; ichan++)
	{
	  total_hi[ipol][ichan] += cal_hi[ipol][ichan];
	  total_lo[ipol][ichan] += cal_lo[ipol][ichan];
	}
      }
    }

  }

  if (nsub > 1)
  {
    for (unsigned ipol=0; ipol<npol; ipol++)
    {
      for (unsigned ichan=0; ichan<nchan; ichan++)
      {
	cal_hi[ipol][ichan] = total_hi[ipol][ichan].get_Estimate();
	cal_lo[ipol][ichan] = total_lo[ipol][ichan].get_Estimate();
      }
    }
  }
}

void Pulsar::ReferenceCalibrator::get_levels 
(unsigned nchan,
 vector<vector<Estimate<double> > >& cal_hi,
 vector<vector<Estimate<double> > >& cal_lo) const
{
  get_levels (get_calibrator(), nchan, cal_hi, cal_lo);
}


void Pulsar::ReferenceCalibrator::calculate_transformation ()
{
  // the calibrator hi and lo levels from the PolnCal archive
  vector<vector<Estimate<double> > > cal_hi;
  vector<vector<Estimate<double> > > cal_lo;

  get_levels (get_calibrator(), requested_nchan, cal_hi, cal_lo);

  unsigned npol = get_calibrator()->get_npol();
  unsigned nchan = requested_nchan;

  if (verbose > 2)
    cerr << "Pulsar::ReferenceCalibrator::calculate_transformation"
      " npol=" << npol << " nchan=" << nchan << endl;

  baseline.resize (nchan);

  transformation.resize (nchan);

  // coherency products in a single channel
  vector<Estimate<double> > source (npol);
  vector<Estimate<double> > sky (npol);

  for (unsigned ichan=0; ichan<nchan; ++ichan) try
  {
    if (verbose > 2)
      cerr << "Pulsar::ReferenceCalibrator::calculate_transformation"
	" ichan=" << ichan << endl;

    for (unsigned ipol=0; ipol<npol; ++ipol)
    {
      source[ipol] = cal_hi[ipol][ichan] - cal_lo[ipol][ichan];
      sky[ipol] = cal_lo[ipol][ichan];
    }

    Estimate<double> cal_AA = source[0];
    Estimate<double> cal_BB = source[1];

    bool bad = false;

    if (cal_AA.val <= 0 || cal_BB.val <= 0)
    {
      if (verbose > 2)
	cerr << "Pulsar::ReferenceCalibrator::calculate_transformation"
	  " ichan=" << ichan << " bad levels" << endl;
      bad = true;
    }

    if (det_threshold > 0 && npol == 4)
    {
      // this class correctly handles the propagation of error and bias
      static MEAL::Invariant invariant;

      Stokes< Estimate<double> > stokes = coherency( convert(source) );
      invariant.set_Stokes (stokes);

      Estimate<double> inv = invariant.get_invariant();
      double bias = invariant.get_bias();

      /*

	data are considered bad if: inv - T*bias < -T*err
	where:
	inv = inv.get_value()
	err = inv.get_error()
	T   = det_threshold

      */

      double cutoff = det_threshold * (bias - inv.get_error());

      if (inv.get_value() < cutoff)
      {
	if (verbose)
	  cerr << "Pulsar::ReferenceCalibrator::calculate_transformation"
	    " ichan=" << ichan << "\n  invariant=" << inv.get_value()
	       << " < cutoff=" << cutoff << 
	    " (threshold=" << det_threshold 
	       << " error=" << inv.get_error() 
	       << " bias=" << bias << ")" << endl;
	bad = true;
      }
    }

    if (bad)
    {
      baseline[ichan] = 0;
      transformation[ichan] = 0;
      // derived classes may need to initialize bad values
      extra (ichan, source, sky);
      continue;
    }

    // baseline intensity, normalized by cal flux, C
    baseline[ichan] = sky[0]/cal_AA + sky[1]/cal_BB;

    // store the transformation appropriate for inverting the system response
    transformation[ichan] = solve (source, sky);

    // enable derived classes to store extra information
    extra (ichan, source, sky);

  }
  catch (Error& error) {
    cerr << "Pulsar::ReferenceCalibrator::calculate_transformation error"
      " ichan=" << ichan << "\n  " << error.get_message() << endl;
    transformation[ichan] = 0;
  }

  if (verbose > 2)
    cerr << "Pulsar::ReferenceCalibrator::calculate_transformation exit"
	 << endl;
}

