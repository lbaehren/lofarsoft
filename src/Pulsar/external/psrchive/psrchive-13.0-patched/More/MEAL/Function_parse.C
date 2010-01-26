/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Function.h"
#include "stringtok.h"

#include <stdio.h>

using namespace std;

//! Parses the values of model parameters and fit flags from a string
void MEAL::Function::parse (const string& line)
{
  string temp = line;

  // the key should be the name of the parameter to be set
  string key = stringtok (temp, " \t");

  if (verbose)
    cerr << "MEAL::Function::parse key '" << key << "'" << endl;

  unsigned iparam=0;
  for (iparam=0; iparam < get_nparam(); iparam++)
  {
    if (verbose)
      cerr << "MEAL::Function::parse param[" << iparam << "]='"
	   << get_param_name(iparam) << "'" << endl;

    if ( key == get_param_name(iparam) )
    {
      if (verbose)
	cerr << "MEAL::Function::parse match" << endl;

      break;
    }
  }

  if (iparam == get_nparam())
  {
    string names;
    for (iparam=0; iparam < get_nparam(); iparam++)
      names += get_param_name(iparam) + " ";

    throw Error (InvalidParam, "MEAL::Function::parse",
		 "key='%s' does not match any name of %d parameters:\n\t%s",
		 key.c_str(), get_nparam(), names.c_str());
  }

  string value = stringtok (temp, " \t");

  if (verbose)
    cerr << "MEAL::Function::parse value " << value << endl;

  double param_value;
  if ( sscanf (value.c_str(), "%lf", &param_value) != 1 )
    throw Error (InvalidParam, "MEAL::Function::parse",
		 "value='" + value + "' could not be parsed as a double");

  set_param (iparam, param_value);

  value = stringtok (temp, " \t");

  if (!value.length())
    return;

  if (value=="true" || value=="1")
  {
    if (verbose)
      cerr << "MEAL::Function::parse fit " << key << endl;
    set_infit (iparam, true);
  }
  else if (value=="false" || value=="0")
  {
    if (verbose)
      cerr << "MEAL::Function::parse do not fit " << key << endl;
    set_infit (iparam, false);
  }
  else
    throw Error (InvalidParam, "MEAL::Function::parse",
		 "value='" + value + "' could not be parsed as a boolean");

  value = stringtok (temp, " \t");

  if (!value.length())
    return;

  if (verbose)
    cerr << "MEAL::Function::parse standard deviation " << value << endl;

  if ( sscanf (value.c_str(), "%lf", &param_value) != 1 )
    throw Error (InvalidParam, "MEAL::Function::parse",
		 "sigma='" + value + "' could not be parsed as a double");

  set_variance (iparam, param_value*param_value);
}
