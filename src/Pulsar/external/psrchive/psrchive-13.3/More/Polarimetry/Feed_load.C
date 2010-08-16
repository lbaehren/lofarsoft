/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Feed.h"
#include "MEAL/parse.h"
#include "Angle.h"
#include <stdio.h>
#include <string.h>

using namespace std;

//! Parses the values of model parameters and fit flags from a string
void Calibration::Feed::load (const string& filename)
{
  MEAL::parse (filename, this);
}


//! Parses the values of model parameters and fit flags from a string
void Calibration::Feed::parse (string line)
{
  // the key should be the name of the parameter to be set
  string key = stringtok (line, " \t");

  if (verbose)
    cerr << "Calibration::Feed::parse key '" << key << "'" << endl;

  string value = stringtok (line, " \t");

  if (verbose)
    cerr << "Calibration::Feed::parse value '" << value << "'" << endl;

  double param_value;
  if (sscanf (value.c_str(), "%lf", &param_value) != 1)
    throw Error (InvalidParam, "Calibration::Feed::parse",
		 "value='" + value + "' could not be parsed as a double");
    
  string unit = stringtok (line, " \t");
    
  if (verbose)
    cerr << "Calibration::Feed::parse unit '" << unit << "'" << endl;
  
  Angle angle;
    
  if ( strncasecmp (unit.c_str(), "deg", 3) == 0 )
    angle.setDegrees (param_value);
  else if ( strncasecmp (unit.c_str(), "rad", 3) == 0 )
    angle.setRadians (param_value);
  else
    throw Error (InvalidParam, "Calibration::Feed::parse",
		 "unrecognized unit: '" + unit + "'");
    
  if ( strcasecmp (key.c_str(), "el0") == 0 )
    set_ellipticity (0, angle.getRadians());
    
  else if ( strcasecmp (key.c_str(), "el1") == 0 )
    set_ellipticity (1, angle.getRadians());

  else if ( strcasecmp (key.c_str(), "or0") == 0 )
    set_orientation (0, angle.getRadians());

  else if ( strcasecmp (key.c_str(), "or1") == 0 )
    set_orientation (1, angle.getRadians());

  else
    throw Error (InvalidParam, "Calibration::Feed::parse",
		 "unrecognized key: '" + key + "'");

}

