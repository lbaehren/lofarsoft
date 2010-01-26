/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/FluxCalibratorData.h"
#include "Pulsar/FluxCalibratorExtension.h"
#include "Pulsar/StandardCandles.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/CalibratorTypes.h"
#include "Pulsar/BasisCorrection.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Receiver.h"

#include "Error.h"
#include "interpolate.h"
#include "templates.h"
#include "Pauli.h"

#include <assert.h>

using namespace std;

/*! 
  If a Pulsar::Archive is provided, and if it contains a
  PolnCalibratorExtension, then the constructed instance can be
  used to calibrate other Pulsar::Archive instances.
*/
Pulsar::FluxCalibrator::FluxCalibrator (const Archive* archive)
{
  init ();

  if (!archive)
    return;

  const FluxCalibratorExtension* fe = archive->get<FluxCalibratorExtension>();
  if (fe)
  {
    // store the calibrator archive
    set_calibrator( archive );

    // store the extension
    extension = fe;

    // store the filename
    filenames.push_back( archive->get_filename() );

    unsigned nchan = fe->get_nchan();
    data.resize( nchan );
    for (unsigned ichan=0; ichan < nchan; ichan++)
      data[ichan] = Data( fe->S_sys[ichan], fe->S_cal[ichan] );

    // disable checks for sufficient data
    have_on = have_off = true;
  }
  else
    add_observation (archive);
}

Pulsar::FluxCalibrator::~FluxCalibrator ()
{
}

void Pulsar::FluxCalibrator::init ()
{
  type = new CalibratorTypes::Flux;
  calculated = have_on = have_off = false;
}

string Pulsar::FluxCalibrator::get_standard_candle_info () const
{
  return standard_candle_info;
}

double Pulsar::FluxCalibrator::meanTsys ()
{
  MeanEstimate<double> mean;

  for (unsigned i = 0; i < data.size(); i++)
    if (get_valid(i))
      mean += data[i].get_S_sys ();
  
  return mean.get_Estimate().val;
}

double Pulsar::FluxCalibrator::Tsys (unsigned ichan)
{
  if (ichan >= data.size())
    throw Error (InvalidParam, "Pulsar::FluxCalibrator::Tsys",
                 "ichan=%d > data.size=%d", ichan, data.size());
  
  return data[ichan].get_S_sys().get_value();
}

//! Return an estimate of the artificial cal Stokes parameters
/*! This method uses the flux cal measurement to determine 
 * the intrinsic Stokes parameters of the artifical cal 
 * source (reference source).  This is based on the assumptions 
 * that the standard candle is unpolarized, and that the reference
 * source signal is 100% correlated in each receptor with zero phase,
 * but may have unequal power in the two sides.
 */
Pulsar::CalibratorStokes* Pulsar::FluxCalibrator::get_CalibratorStokes ()
{
  // Check that we have both polns
  if (get_nreceptor() != 2) 
    throw Error (InvalidState, "Pulsar::FluxCalibrator::get_CalibratorStokes",
        "nreceptor=%d != 2", get_nreceptor());

  Reference::To<CalibratorStokes> calstokes = new CalibratorStokes;
  calstokes->set_nchan(get_nchan());

  const Receiver* receiver = get_Archive()->get<Receiver>();
  
  // Loop over chans
  for (unsigned ichan=0; ichan<get_nchan(); ichan++)
  {
    Estimate<double> diff, cross;
    Stokes < Estimate<double> > stokes;

    // Skip invalid channels
    if (get_valid(ichan)==false || data[ichan].get_S_cal()==0.0 || 
        data[ichan].get_S_cal(0)==0.0 || data[ichan].get_S_cal(1)==0.0)
    { 
      stokes[0] = Estimate<double> (0.0, 0.0);
      stokes[1] = Estimate<double> (0.0, 0.0);
      stokes[2] = Estimate<double> (0.0, 0.0);
      stokes[3] = Estimate<double> (0.0, 0.0);
      calstokes->set_stokes(ichan, stokes);
      calstokes->set_valid(ichan,false);
      continue;
    }

    // CalibratorStokes only stores fractional Stokes params
    stokes[0] = Estimate<double> (1.0,0.0);

    diff = (data[ichan].get_S_cal(0) - data[ichan].get_S_cal(1))
      / data[ichan].get_S_cal();

    cross = data[ichan].get_S_cal(0) * data[ichan].get_S_cal(1);
    if (cross<0.0) { cross = -cross; }
    cross = 2.0 * sqrt(cross) / data[ichan].get_S_cal(); 

    // This method may be more general.. applies the inverse basis
    // correction for the receiver, to produce the cal stokes parameters
    // "in the sky".  Also tries to account for the reference source 
    // phase setting.  This may still have trouble with circular feeds
    // as discussed below.
    if (receiver) {

      Jones<double> basis_correction (1.0);
      BasisCorrection corr;
      basis_correction = corr(receiver);

      double phi = receiver->get_reference_source_phase().getRadians();

      stokes[1] = diff;
      stokes[2] = Estimate<double> (cross.val * cos(phi), cross.var);
      stokes[3] = Estimate<double> (cross.val * sin(phi), cross.var);
      stokes = transform(stokes, inv(basis_correction));

    }
    
    // If no receiver extension, fall back on the original code..
    else {

      // Take care of hand convention here
      if (receiver && receiver->get_hand()==Signal::Left)
        diff = -diff;

      // This uses the Archive basis as a proxy for the cal signal 
      // properties.  It could fail if for example the signal was
      // converted from lin to circ using a hybrid AFTER the cal signal
      // was coupled in.  In that case, the Archive data would appear
      // circular, but the cal signal is really linear (nonzero Q not V).
      // TODO: Use the Receiver reference source angle setting?
      if (get_Archive()->get_basis() == Signal::Linear)
      {
        // Linear feeds, Q = diff, U = cross, V = 0
        stokes[1] = diff;
        stokes[2] = cross;
        stokes[3] = Estimate<double> (0.0, cross.var);
      }
      else if (get_Archive()->get_basis() == Signal::Circular)
      {
        // Circular feeds, Q = cross, U = 0, V = diff
        stokes[1] = cross;
        stokes[2] = Estimate<double> (0.0, cross.var);
        stokes[3] = diff;
      }

    }

    calstokes->set_stokes(ichan, stokes);
  }

  return calstokes.release();
}

//! Return true if the flux scale for the specified channel is valid
bool Pulsar::FluxCalibrator::get_valid (unsigned ch) const
{
  return data[ch].get_valid();
}

//! Set the flux scale invalid flag for the specified channel
void Pulsar::FluxCalibrator::set_invalid (unsigned ch)
{
  data[ch].set_valid (false);
}

void Pulsar::FluxCalibrator::add_observation (const Archive* archive)
{
  if (!archive)
    throw Error (InvalidParam, "Pulsar::FluxCalibrator::add_observation",
                 "invalid Pulsar::Archive pointer");

  if (verbose > 2)
    cerr << "Pulsar::FluxCalibrator::add_observation source name=" 
         << archive->get_source() << " type=" 
         << Signal::Source2string(archive->get_type()) << endl;

  if ( archive->get_type() != Signal::FluxCalOn &&
       archive->get_type() != Signal::FluxCalOff )

    throw Error (InvalidParam, "Pulsar::FluxCalibrator::add_observation",
		 "Pulsar::Archive='" + archive->get_filename() + "'"
		 "is not a FluxCal");

  string reason;
  if (has_calibrator() &&
      !(get_calibrator()->calibrator_match (archive, reason) &&
	get_calibrator()->processing_match (archive, reason)))
    throw Error (InvalidParam, "Pulsar::FluxCalibrator::add_observation",
		 "mismatch between\n\t" + get_calibrator()->get_filename() +
                 " and\n\t" + archive->get_filename() + reason);

  unsigned nchan = archive->get_nchan ();
  unsigned nreceptor = (archive->get_npol() == 1) ? 1 : 2;

  string filename = archive->get_filename ();
  bool rename_calibrator = false;

  if (!has_calibrator()) {

    set_calibrator(archive);
    resize (nchan, nreceptor);

  }
  else if (get_calibrator()->get_type() != Signal::FluxCalOn &&
	   archive->get_type() == Signal::FluxCalOn)  {

    // Keep the FPTM naming convention in which the
    // Pulsar::FluxCalibrator is named for the first on-source
    // observation
    set_calibrator(archive);
    rename_calibrator = true;

  }

  assert (data.size() == nchan);

  if (archive->get_state () == Signal::Stokes) {

    if (verbose > 2)
      cerr << "Pulsar::FluxCalibrator::add_observation clone Stokes->Coherence"
           << endl;

    Pulsar::Archive* clone = archive->clone();
    clone->convert_state (Signal::Coherence);

    archive = clone;

  }


  vector< vector< Estimate<double> > > cal_hi;
  vector< vector< Estimate<double> > > cal_lo;

  unsigned nsub = archive->get_nsubint();

  for (unsigned isub=0; isub < nsub; isub++) {

    const Pulsar::Integration* integration = archive->get_Integration (isub);

    if (verbose > 2) 
      cerr << "Pulsar::FluxCalibrator call Integration::cal_levels" << endl;

    integration->cal_levels (cal_hi, cal_lo);
    Estimate<double> unity(1.0);

    for (unsigned ichan=0; ichan<nchan; ++ichan) {
      
      if (integration->get_weight(ichan) == 0)
	continue;

      for (unsigned ir=0; ir < nreceptor; ir++) {

	if (cal_lo[ir][ichan].val == 0)  {
	  if (verbose > 2)
	    cerr << "Pulsar::FluxCalibrator::add_observation ir="
		 << ir << " ichan=" << ichan
		 << " division by zero" << endl;
	  continue;
	}
      
	// Take the ratio of the flux
	Estimate<double> ratio = cal_hi[ir][ichan]/cal_lo[ir][ichan] - unity ;
	if (archive->get_type() == Signal::FluxCalOn)
	  data[ichan].add_ratio_on (ir, ratio);
	else if (archive->get_type() == Signal::FluxCalOff)
	  data[ichan].add_ratio_off (ir, ratio);
      
      } // for each receptor

    } // for each frequency channel

  } // for each sub-integration

  if (rename_calibrator)
    filenames.insert (filenames.begin(), filename);
  else
    filenames.push_back (filename);

  calculated = false;
  if (archive->get_type() == Signal::FluxCalOn)
    have_on = true;
  if (archive->get_type() == Signal::FluxCalOff)
    have_off = true;

}

//! Set the database containing flux calibrator information
void Pulsar::FluxCalibrator::set_database (const StandardCandles* d)
{
  database = d;
}

//! Calibrate the flux in the given archive
void Pulsar::FluxCalibrator::calibrate (Archive* arch)
{
  if (!has_calibrator())
    throw Error (InvalidState, "Pulsar::FluxCalibrator::calibrate",
		 "no FluxCal Archive");

  if (arch->get_scale() != Signal::ReferenceFluxDensity)
    throw Error (InvalidParam, "Pulsar::FluxCalibrator::calibrate", 
                 "Archive scale != ReferenceFluxDensity");

  string reason;
  if (!get_calibrator()->calibrator_match (arch, reason))
    throw Error (InvalidParam, "Pulsar::FluxCalibrator::add_observation",
		 "mismatch between calibrator\n\t" 
		 + get_calibrator()->get_filename() +
                 " and\n\t" + arch->get_filename() + reason);

  if (verbose > 2)
    cerr << "Pulsar::FluxCalibrator::calibrate call create" << endl;

  create (arch->get_nchan());

  for (unsigned isub=0; isub < arch->get_nsubint(); isub++)
    calibrate (arch->get_Integration(isub));

  arch->set_scale (Signal::Jansky);
}


void Pulsar::FluxCalibrator::create (unsigned required_nchan)
{
  if (!has_calibrator())
    throw Error (InvalidState, "Pulsar::FluxCalibrator::create",
		 "no FluxCal Archive");

  unsigned nchan = data.size ();

  if (!required_nchan)
    required_nchan = nchan;

  if (verbose > 2)
    cerr << "Pulsar::FluxCalibrator::create nchan=" << nchan 
	 << " required nchan=" << required_nchan << endl;

  if (calculated && gain.size() == required_nchan)
    return;

  if (!have_on)
    throw Error (InvalidState, "Pulsar::FluxCalibrator::create",
                 "no FluxCal-On data");
  if (!have_off)
    throw Error (InvalidState, "Pulsar::FluxCalibrator::create",
                 "no FluxCal-Off data");

  setup ();

  gain.resize (nchan);

  unsigned successful = 0;

  for (unsigned ichan=0; ichan<nchan; ++ichan) try {
    gain[ichan] = data[ichan].get_S_cal().get_value();
    successful ++;
  }
  catch (Error& error) {
    gain[ichan] = 0;
  }

  if (!successful)
    throw Error (InvalidState, "Pulsar::FluxCalibrator::create",
		 "failed in all %u channels", nchan);

  resize (required_nchan);

  calculated = true;
}

void Pulsar::FluxCalibrator::resize (unsigned nchan, unsigned nreceptor)
{
  data.resize( nchan );
  for (unsigned i=0; i < nchan; i++)
    data[i].set_nreceptor (nreceptor);
}

void Pulsar::FluxCalibrator::resize (unsigned required_nchan)
{
  unsigned nchan = gain.size();

  if (nchan == required_nchan)
    return;

  if (required_nchan < nchan)  {

    unsigned nscrunch = nchan / required_nchan;
  
    if (verbose > 2)
        cerr << "Pulsar::FluxCalibrator::resize required nchan="
	     << required_nchan << " < nchan=" << nchan 
	     << " nscrunch=" << nscrunch << endl;

    scrunch (gain, nscrunch, true);

  }
  else {

    if (verbose > 2)
        cerr << "Pulsar::FluxCalibrator::resize required nchan="
             << required_nchan << " > nchan=" << nchan << endl;

    vector<float> temp (required_nchan);
    fft::interpolate (temp, gain);
    gain = temp;

  }
}


void Pulsar::FluxCalibrator::setup () try {
  
  // if built from an extension, the data are already available
  if (extension)
    return;

  if (!database) {
    if (verbose > 2)
      cerr << "Pulsar::FluxCalibrator::calculate using default database"<<endl;
    database = new StandardCandles;
  }

  if (verbose > 2)
    cerr << "Pulsar::FluxCalibrator::calculate search for source=" 
	 << get_calibrator()->get_source() << endl;

  StandardCandles::Entry entry;
  entry = database->match (get_calibrator()->get_source(),
			   get_calibrator()->get_centre_frequency());

  if (verbose > 2)
    cerr << "Pulsar::FluxCalibrator::calculate found matching source=" 
	 << entry.source_name[0]<< endl;

  unsigned nchan = data.size();
  
  const Integration* subint = get_calibrator()->get_Integration(0);

  double MHz = subint->get_centre_frequency();

  standard_candle_info = entry.source_name[0] + " -- at " 
    + tostring(MHz,4) + " MHz, S = "
    + tostring(entry.get_flux_mJy(MHz)/1e3,4) + " Jy";

  for (unsigned ichan=0; ichan<nchan; ++ichan) {

    double frequency = subint->get_centre_frequency(ichan);
    double source_mJy = entry.get_flux_mJy (frequency);

    if (verbose > 2)
      cerr << "Pulsar::FluxCalibrator::calculate channel=" << ichan << 
	" freq=" << frequency << " flux=" << source_mJy << endl;

    data[ichan].set_S_std (source_mJy);
    
  }

}
catch (Error& error) {
  throw error += "Pulsar::FluxCalibrator::setup";
}

void Pulsar::FluxCalibrator::calibrate (Integration* subint)
{
  unsigned npol = subint->get_npol();
  unsigned nchan = subint->get_nchan();

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (verbose > 2)
      cerr << "Pulsar::FluxCalibrator::calibrate ichan=" << ichan 
	   << " gain=" << gain[ichan] << endl;

    if (gain[ichan] == 0)
      subint->set_weight (ichan, 0.0);
    else
      for (unsigned ipol=0; ipol<npol; ipol++)
	subint->get_Profile (ipol, ichan) -> scale (gain[ichan]);
  }
}

//! Get the number of frequency channels in the calibrator
unsigned Pulsar::FluxCalibrator::get_nchan () const
{
  return data.size();
}

//! Get the number of receptors
unsigned Pulsar::FluxCalibrator::get_nreceptor () const
{
  if (data.size())
    return data[0].get_nreceptor();
  else
    return 0;
}

string Pulsar::FluxCalibrator::Info::get_title () const
{
  return "System and Noise Source Equivalent Flux Densities";
}

//! Return the name of the specified class
std::string Pulsar::FluxCalibrator::Info::get_name (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return "\\fiS\\dcal\\u\\fr (Jy)";
  case 1:
    return "\\fiS\\dsys\\u\\fr (Jy)";
  default:
    return "";
  }
}
    
//! Return the number of parameters in the specified class
unsigned Pulsar::FluxCalibrator::Info::get_nparam (unsigned iclass) const
{
  return instance->get_nreceptor();
}
    
//! Return the estimate of the specified parameter
Estimate<float> Pulsar::FluxCalibrator::Info::get_param (unsigned ichan,
							 unsigned iclass,
							 unsigned iparam) const
{
  Estimate<float> retval;

  if (iclass == 0)
    retval = instance->data[ichan].get_S_cal(iparam);
  else if (iclass == 1)
    retval = instance->data[ichan].get_S_sys(iparam);
  
  // convert to Jy
  retval *= 1e-3;
  return retval;
}

Pulsar::Calibrator::Info* Pulsar::FluxCalibrator::get_Info () const
{
  const_cast<FluxCalibrator*>(this)->setup();
  return new FluxCalibrator::Info (this);
}

Pulsar::CalibratorExtension*
Pulsar::FluxCalibrator::new_Extension () const
{
  const_cast<FluxCalibrator*>(this)->setup();
  return new FluxCalibratorExtension (this);
}

