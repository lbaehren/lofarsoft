/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Receiver.h"
#include "Factory.h"

#include <string.h>

using namespace std;

Pulsar::Receiver* Pulsar::Receiver::load (const string& filename) try {

  Functor< Receiver*(string) > constructor( &new_Receiver );

  return Factory::load (filename, constructor, Archive::verbose == 3);

}
catch (Error& error) {
  throw error += "Pulsar::Receiver::load";
}

/*!  \param name for now, the Receiver base class has no derived types
  and this method always returns a new Receiver instance with name set
  to name.
*/
Pulsar::Receiver* Pulsar::Receiver::new_Receiver (const string& name)
{
  Receiver* receiver = new Receiver;
  receiver->set_name (name);
  return receiver;
}

//! Parses the values of model parameters and fit flags from a string
void Pulsar::Receiver::parse (string line)
{
  // the key should be the name of the parameter to be set
  string key = stringtok (line, " \t");

  if (Archive::verbose == 3)
    cerr << "Pulsar::Receiver::parse key '" << key << "'" << endl;

  string value = stringtok (line, " \t");

  if (Archive::verbose == 3)
    cerr << "Pulsar::Receiver::parse value '" << value << "'" << endl;

  if ( strncasecmp (key.c_str(), "track", 5) == 0 ) {

    if ( strcasecmp (value.c_str(), "FA") == 0 )
      tracking_mode = Feed;
    else if ( strcasecmp (value.c_str(), "CPA") == 0 )
      tracking_mode = Celestial;
    else if ( strcasecmp (value.c_str(), "GPA") == 0 )
      tracking_mode = Galactic;
    else
      throw Error (InvalidParam, "Pulsar::Receiver::parse",
		   "unrecognized tracking mode: '" + value + "'");
  }

  else if ( strcasecmp (key.c_str(), "basis") == 0 ) {

    if ( strncasecmp (value.c_str(), "lin", 3) == 0 )
      set_basis( Signal::Linear );
    else if ( strncasecmp (value.c_str(), "circ", 4) == 0 )
      set_basis( Signal::Circular );
    else
      throw Error (InvalidParam, "Pulsar::Receiver::parse",
		   "unrecognized basis: '" + value + "'");

  }

  else {

    double param_value;
    if (sscanf (value.c_str(), "%lf", &param_value) != 1)
      throw Error (InvalidParam, "Pulsar::Receiver::parse",
		   "value='" + value + "' could not be parsed as a double");
    
    string unit = stringtok (line, " \t");
    
    if (Archive::verbose == 3)
      cerr << "Pulsar::Receiver::parse unit '" << unit << "'" << endl;

    Angle angle;
    
    if ( strncasecmp (unit.c_str(), "deg", 3) == 0 )
      angle.setDegrees (param_value);
    else if ( strncasecmp (unit.c_str(), "rad", 3) == 0 )
      angle.setRadians (param_value);
    else
      throw Error (InvalidParam, "Pulsar::Receiver::parse",
		   "unrecognized unit: '" + unit + "'");
    
    if ( strcasecmp (key.c_str(), "angle") == 0 )
      tracking_angle = angle;
    
    else if ( strcasecmp (key.c_str(), "X_offset") == 0 )
      set_X_offset( angle );
    
    else if ( strcasecmp (key.c_str(), "Y_offset") == 0 )
      set_Y_offset( angle );
    
    else if ( strcasecmp (key.c_str(), "cal_offset") == 0 )
      set_calibrator_offset ( angle );

    else
      throw Error (InvalidParam, "Pulsar::Receiver::parse",
		   "unrecognized key: '" + key + "'");

  }
    
}

