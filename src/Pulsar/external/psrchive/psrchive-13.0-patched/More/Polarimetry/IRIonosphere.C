/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/IRIonosphere.h"
#include "MEAL/NoParameters.h"
#include "MEAL/ForwardResult.h"

#include "iri2001/iri2rm.h"

using namespace std;

Calibration::IRIonosphere::IRIonosphere () 
{
  parameter_policy  = new MEAL::NoParameters;
  evaluation_policy = new MEAL::ForwardResult<Complex2> (this, &faraday);
}

//! Return the name of the class
string Calibration::IRIonosphere::get_name () const
{
  return "IRIonosphere";
}

//! Set the reference frequency in MHz
void Calibration::IRIonosphere::set_reference_frequency (double MHz)
{
  faraday.set_reference_frequency (MHz);
}

//! Get the reference frequency in MHz
double Calibration::IRIonosphere::get_reference_frequency () const
{
  return faraday.get_reference_frequency ();
}

//! Set the frequency in MHz
void Calibration::IRIonosphere::set_frequency (double MHz)
{
  faraday.set_frequency (MHz);
}

//! Get the frequency in MHz
double Calibration::IRIonosphere::get_frequency () const
{
  return faraday.get_frequency ();
}

//! Set the Horizon pointing parameters
void Calibration::IRIonosphere::set_horizon (const Horizon& _horizon)
{
  horizon = _horizon;
  build ();
}

//! Get the Faraday rotation measure
double Calibration::IRIonosphere::get_rotation_measure () const
{
  return faraday.get_rotation_measure().get_value();
}

void Calibration::IRIonosphere::build ()
{
  /* iri2rm expects the following:
     C INPUT:  Glong         Geographic Longitude East in degrees
     C         Glati         Geographic Latitude North in degrees
     C         year          Year, e.g. 1985
     C         mmdd (-ddd)   Date (or Day of Year as a negative number)
     C         UT            Universal Time in decimal hours
     C         az            Azimuth in degrees
     C         el            Elevation in degrees
  */

  double Glong = horizon.get_observatory_longitude() * 180/M_PI;
  double Glati = horizon.get_observatory_latitude()  * 180/M_PI;

  struct tm greg;
  horizon.get_epoch().gregorian(&greg);

  /* the struct tm has the following data members:
     tm_sec       Seconds after the minute [0-60]
     tm_min       Minutes after the hour [0-59]
     tm_hour      Hours since midnight [0-23]
     tm_mday      Day of the month [1-31]
     tm_mon       Months since January [0-11]
     tm_year      Years since 1900
   */

  float year = 1900 + greg.tm_year;
  int   mmdd = 100 * (greg.tm_mon + 1) + greg.tm_mday;
  double  UT = double(greg.tm_hour) + double(greg.tm_min)/60.0
    + double(greg.tm_sec)/3600.0;
  
  float az = horizon.get_azimuth()   * 180/M_PI;
  float el = horizon.get_elevation() * 180/M_PI;

  float RM;

  iri2rm (&Glong, &Glati, &year, &mmdd, &UT, &az, &el, &RM);

  faraday.set_rotation_measure (RM);
}
