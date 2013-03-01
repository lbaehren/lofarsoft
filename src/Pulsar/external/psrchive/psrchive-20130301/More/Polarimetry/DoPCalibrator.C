/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DoPCalibrator.h"
#include "Pulsar/CalibratorTypes.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pauli.h"

#include <assert.h>

using namespace std;

Pulsar::DoPCalibrator::DoPCalibrator (const Archive* archive) 
  : SingleAxisCalibrator (archive)
{
  type = new CalibratorTypes::DoP;
  // the Multibeam noise diode is approximately 90% linearly polarized
  dop_reference = 0.9;
}

Pulsar::DoPCalibrator::~DoPCalibrator ()
{
}

//! Calculate the scale factor array
void Pulsar::DoPCalibrator::extra (unsigned ichan,
				   const vector< Estimate<double> >& source,
				   const vector< Estimate<double> >& sky)
{
  unsigned nchan = transformation.size();
  dop_scale.resize (nchan);

  if (!transformation[ichan]) {
    dop_scale[ichan] = 0.0;
    return;
  }
    
  // Convert the coherency vector into Stokes parameters.
  Stokes< Estimate<double> > obs_cal = coherency( convert (source) );

  // Evaluate
  Jones<double> response = transformation[ichan] -> evaluate ();

  // Calibrate the observed Stokes parameters
  Stokes<Estimate<double> > cal = transform (obs_cal, inv(response));

  dop_scale[ichan] = dop_reference * cal[0] / cal.abs_vect() ;
}

/*! The required transformation cannot be represented by a Jones matrix */
void Pulsar::DoPCalibrator::calibrate (Archive* archive)
{
  cerr << "Pulsar::DoPCalibrator::calibrate **********************" << endl;
  calibration_setup (archive);

  unsigned nsub  = archive->get_nsubint();
  unsigned nchan = archive->get_nchan();
  unsigned npol  = archive->get_npol();

  assert (nchan == dop_scale.size());

  if (npol != 4)
    throw Error (InvalidState, "Pulsar::DoPCalibrator::calibrate",
		 "npol != 4");

  for (unsigned isub=0; isub < nsub; isub++) {
    Integration* subint = archive->get_Integration (isub);
    
    for (unsigned ichan=0; ichan < nchan; ichan++) {
      subint->get_Profile (2, ichan) -> scale (dop_scale[ichan].val);
      subint->get_Profile (3, ichan) -> scale (dop_scale[ichan].val);
    }
  }

  SingleAxisCalibrator::calibrate (archive);
}


Pulsar::DoPCalibrator::Info::Info (const DoPCalibrator* cal) 
  : SingleAxisCalibrator::Info (cal)
{
  dopcal = cal;
}

string Pulsar::DoPCalibrator::Info::get_title () const
{
  return "Degree of Polarization Fudge Factors";
}

//! Return the number of parameter classes
unsigned Pulsar::DoPCalibrator::Info::get_nclass () const
{
  return 4;
}

//! Return the name of the specified class
string Pulsar::DoPCalibrator::Info::get_name (unsigned iclass) const
{
  if (iclass < 3)
    return SingleAxisCalibrator::Info::get_name (iclass);

  return "dop fix";
} 
  
//! Return the number of parameters in the specified class
unsigned Pulsar::DoPCalibrator::Info::get_nparam (unsigned iclass) const
{
  if (iclass < 4)
    return 1;
  return 0;
}

Estimate<float>
Pulsar::DoPCalibrator::Info::get_param (unsigned ichan, 
					unsigned iclass,
					unsigned iparam) const
{
  if (iclass < 3)
    return SingleAxisCalibrator::Info::get_param (ichan, iclass, iparam);

  return dopcal->dop_scale[ichan];
}

Pulsar::DoPCalibrator::Info* 
Pulsar::DoPCalibrator::get_Info () const
{
  return new DoPCalibrator::Info (this);
}

