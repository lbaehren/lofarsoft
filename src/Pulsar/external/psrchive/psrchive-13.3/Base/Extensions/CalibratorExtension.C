/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalibratorExtensionInterface.h"
#include "Pulsar/Calibrator.h"
#include "templates.h"

using namespace Pulsar;

//! Default constructor
CalibratorExtension::CalibratorExtension (const char* name)
  : Extension (name)
{
}

//! Copy constructor
CalibratorExtension::CalibratorExtension (const CalibratorExtension& copy)
  : Extension (copy.get_extension_name().c_str())
{
  operator = (copy);
}

//! Operator =
const CalibratorExtension&
CalibratorExtension::operator= (const CalibratorExtension& copy)
{
  if (this == &copy)
    return *this;

  epoch = copy.epoch;
  weight = copy.weight;
  centre_frequency = copy.centre_frequency;

  return *this;
}

//! Destructor
CalibratorExtension::~CalibratorExtension ()
{
}

void CalibratorExtension::set_type (const Calibrator::Type* _type)
{
  type = _type;
}

const Calibrator::Type* CalibratorExtension::get_type () const
{
  return type;
}

MJD CalibratorExtension::get_epoch () const
{
  return epoch;
}

void CalibratorExtension::set_epoch (const MJD& _epoch)
{
  epoch = _epoch;
}


//! Set the number of frequency channels
void CalibratorExtension::set_nchan (unsigned nchan)
{
  weight.resize( nchan );
  centre_frequency.resize( nchan );
}

void CalibratorExtension::remove_chan (unsigned first, unsigned last)
{
  remove (weight, first, last);
  remove (centre_frequency, first, last);
}

//! Get the number of frequency channels
unsigned CalibratorExtension::get_nchan () const
{
  return weight.size();
}

float CalibratorExtension::get_weight (unsigned ichan) const
{
  range_check (ichan, "CalibratorExtension::get_weight");
  return weight[ichan];
}

void CalibratorExtension::set_weight (unsigned ichan, float _weight)
{
  range_check (ichan, "CalibratorExtension::set_weight");
  weight[ichan] = _weight;
}


double CalibratorExtension::get_centre_frequency (unsigned ichan) const
{
  range_check (ichan, "CalibratorExtension::get_centre_frequency");
  return centre_frequency[ichan];
}

void CalibratorExtension::set_centre_frequency (unsigned ichan, double freq)
{
  range_check (ichan, "CalibratorExtension::set_centre_frequency");
  centre_frequency[ichan] = freq;
}

void CalibratorExtension::range_check (unsigned ichan,
				       const char* method) const
{
  if (ichan >= weight.size())
    throw Error (InvalidRange, method, "ichan=%d >= nchan=%d",
		 ichan, weight.size());
}

// Text interface to a CalibratorExtension extension
CalibratorExtension::Interface::Interface (CalibratorExtension *s_instance)
{
  if( s_instance )
    set_instance( s_instance );

  // read-only: requires resize
  add( &CalibratorExtension::get_nchan,
       "nchan", "Number of frequency channels" );

  add( &CalibratorExtension::get_epoch,
       &CalibratorExtension::set_epoch,
       "mjd", "Epoch of calibration observation" );

  VGenerator<double> dgenerator;
  add_value(dgenerator( "freq", "Centre frequency of each channel (MHz)",
			&CalibratorExtension::get_centre_frequency,
			&CalibratorExtension::set_centre_frequency,
			&CalibratorExtension::get_nchan ));

  VGenerator<float> fgenerator;
  add_value(fgenerator( "wt", "Weight assigned to each channel",
			&CalibratorExtension::get_weight,
			&CalibratorExtension::set_weight,
			&CalibratorExtension::get_nchan ));
}

//! Return a text interfaces that can be used to access this instance
TextInterface::Parser* CalibratorExtension::get_interface()
{
  return new CalibratorExtension::Interface( this );
}
