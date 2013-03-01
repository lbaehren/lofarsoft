/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/StandardCandles.h"
#include "Pulsar/Pulsar.h"

#include "Error.h"
#include "strutil.h"

#include <fstream>
#include <algorithm>

#include <math.h>
#include <ctype.h>

using namespace std;

bool Pulsar::StandardCandles::verbose = false;

static std::string get_default ()
{
  return Pulsar::Config::get_runtime() + "/fluxcal.cfg";
}

Pulsar::Option<string>
Pulsar::StandardCandles::default_filename 
(
 "fluxcal::database", get_default(),

 "Standard candle input filename",

 "The name of the file from which flux densities, reference frequencies,\n"
 "and spectral indeces of commonly used flux calibrator sources are read."
);

Pulsar::Option<float>
Pulsar::StandardCandles::on_radius
(
 "fluxcal::on_radius", 0.25,

 "Maximum distance to on-source [degrees]",

 "The maximum separation between the sky coordinates of an 'on-source'\n"
 "observation and those of the standard candle."
);

Pulsar::Option<float>
Pulsar::StandardCandles::off_radius
(
 "fluxcal::off_radius", 2.5,

 "Maximum distance to off-source [degrees]",

 "The maximum separation between the sky coordinates of an 'off-source'\n"
 "observation and those of the standard candle."
);

// //////////////////////////////////////////////////////////////////////
//
// Pulsar::StandardCandles::Entry
//
// filename type posn MJD bandwidth cfrequency nchan instrument
//

//! Initialise all variables
void Pulsar::StandardCandles::Entry::init ()
{
  reference_frequency = 0.0;
  reference_flux = 0.0;
  spectral_index = 0.0;
}

//! Destructor
Pulsar::StandardCandles::Entry::~Entry ()
{
  
}

// load from ascii string
void Pulsar::StandardCandles::Entry::load (const string& str) 
{
  const string whitespace = " \t\n";

  string temp = str;

  if( temp[0] == '&'){

    temp.erase(0,1);

    // Load-mode type II
    // log(S) = a_0 + a_1*log(f) + a_2*(log(f))^2 + a_3*(log(f))^4 + ...
    vector<string> words = stringdecimate(temp, whitespace);

    if( words.size() < 5 )
      throw Error(InvalidState,"Pulsar::StandardCandles::Entry::load",
		  "could not parse five words from '"+temp+"'");

    source_name.push_back( words[0] );

    string coordstr = words[1] + " " + words[2];
    position = sky_coord (coordstr.c_str());

    for( unsigned i=3; i<words.size(); i++)
      spectral_coeffs.push_back( fromstring<double>(words[i]) );
    return;
  }

  // /////////////////////////////////////////////////////////////////
  // NAME
  source_name.push_back( stringtok (temp, whitespace) );

  // /////////////////////////////////////////////////////////////////
  // RA DEC
  string coordstr = stringtok (temp, whitespace);
  coordstr += " " + stringtok (temp, whitespace);
  position = sky_coord (coordstr.c_str());

  int s = sscanf (temp.c_str(), "%lf %lf %lf",
		  &reference_frequency,
		  &reference_flux, 
		  &spectral_index);

  if (s != 3)
    throw Error (FailedSys, "Pulsar::StandardCandles::Entry::load",
                 "could not parse three doubles from '"+temp+"'");
}

// unload to a string
void Pulsar::StandardCandles::Entry::unload (string& str)
{
  if( !spectral_coeffs.empty() ){
    str = '&' + source_name[0];

    for( unsigned i=0; i<spectral_coeffs.size(); i++)
      str += stringprintf (" %.*lf", 6, spectral_coeffs[i]);
  }
  else
    str = source_name[0] + stringprintf (" %lf %lf %lf",
					 reference_frequency,
					 reference_flux, 
					 spectral_index);

  for (unsigned iname=1; iname < source_name.size(); iname ++)
    str += "\naka " + source_name[iname];
}

bool close_enough (string A, string B)
{
  std::transform (A.begin(), A.end(), A.begin(), ::tolower);
  std::transform (B.begin(), B.end(), B.begin(), ::tolower);

  return A.find(B)!=string::npos || B.find(A)!=string::npos;
}

//! return true if the source name matches
bool Pulsar::StandardCandles::Entry::matches (const string& name) const
{
  for (unsigned iname=0; iname < source_name.size(); iname ++)
    if (close_enough(source_name[iname], name))
      return true;

  return false;
}

double Pulsar::StandardCandles::Entry::get_flux_mJy (double MHz)
{
  if( !spectral_coeffs.empty() )
  {
    /*

    From http://www.vla.nrao.edu/astro/calib/manual/baars.html

    Below we provide the current (1999.2) best analytic
    expression for their flux densities.

    Log S = A + B * Log v + C * (Log v)**2 + D * (Log v)**3

    where S is the flux density in Jy and v is the frequency in GHz.
    These expressions are valid between 300 MHz and 50 GHz.

    */

    if (MHz < 300 | MHz > 50e3)
      warning << "Pulsar::StandardCandles::Entry::get_flux_mJy\n"
	"\tfrequency=" << MHz << " MHz outside of range of validity" << endl;

    double GHz = MHz * 1e-3;

    double log_freq = log(GHz)/log(10.0);

    double log_flux = spectral_coeffs[0];
    for( unsigned i=1; i<spectral_coeffs.size(); i++)
      log_flux += spectral_coeffs[i] * pow(log_freq, double(i));

    if( verbose )
      cerr << "Pulsar::StandardCandles::Entry::get_flux_mJy"
	" f=" << GHz << "GHz; log(f)=" << log_freq 
	   << "; log(S)=" << log_flux << endl;

    // return in mJy
    return 1e3 * pow(10.0,log_flux);
  }

  return pow (MHz/reference_frequency, -spectral_index) * reference_flux * 1e3;
}

Pulsar::StandardCandles::StandardCandles ()
{
  if (verbose)
    cerr << "Pulsar::StandardCandles load " << default_filename << endl;

  load (default_filename);
}

Pulsar::StandardCandles::~StandardCandles ()
{
}

Pulsar::StandardCandles::StandardCandles (const std::string& f)
{
  if (verbose)
    cerr << "Pulsar::StandardCandles load " << f << endl;

  load (f);
}

//! Loads an entire database from a file
void Pulsar::StandardCandles::load (const std::string& filename)
{
  std::ifstream input (filename.c_str());
  if (!input)
    throw Error (FailedSys, "Pulsar::StandardCandles::load",
		 "ifstream (" + filename + ")");

  string line;

  while (!input.eof()) {

    getline (input, line);

    line = stringtok (line, "#\n", false);  // get rid of comments

    if (!line.length())
      continue;

    if (verbose)
      cerr << "Pulsar::StandardCandles::load '"<< line << "'" << endl;

    if (line.substr(0,3) == "aka") {

      if (entries.size() == 0)
	throw Error (InvalidState, "Pulsar::StandardCandles::load",
		     "cannot add aliases before entries");

      // take off the aka
      string name = stringtok (line, " \t\n");
      // get the name
      name = stringtok (line, " \t\n");

      if (verbose)
	cerr << "Pulsar::StandardCandles::load alias: " 
	     << entries.back().source_name[0] << " = " << name << endl;

      entries.back().source_name.push_back (name);

    }
    else {
      entries.push_back( Entry(line) );

      if (verbose)
	cerr << "Pulsar::StandardCandles::load added: " 
	     << entries.back().source_name[0] << endl;

    }

  }

  loaded_filename = filename;
}

//! Unloads entire database to file
void Pulsar::StandardCandles::unload (const std::string& filename)
{
  FILE* fptr = fopen (filename.c_str(), "w");
  if (!fptr)
    throw Error (FailedSys, "Pulsar::StandardCandles::unload" 
		 "fopen (" + filename + ")");
  
  string out;
  for (unsigned ie=0; ie<entries.size(); ie++) {
    entries[ie].unload(out);
    fprintf (fptr, "%s\n", out.c_str());
  }
  fclose (fptr);
}

Pulsar::StandardCandles::Entry
Pulsar::StandardCandles::match (const string& source, double MHz) const
{
  Entry best_match;

  for (unsigned ie=0; ie<entries.size(); ie++) {
    if (entries[ie].matches(source)) {
      if (best_match.reference_frequency == 0)
	best_match = entries[ie];
      else {
	double diff = fabs(entries[ie].reference_frequency - MHz);
	double best_diff = fabs(best_match.reference_frequency - MHz);
	if (diff < best_diff)
	  best_match = entries[ie];
      }
    }
  }

  if (best_match.reference_frequency == 0)
    throw Error (InvalidParam, "Pulsar::StandardCandles::match",
		 "No match for source=" + source);

  return best_match;
}

Signal::Source
Pulsar::StandardCandles::guess (string& name, sky_coord& p) const
{
  if (!entries.size())
  {
    if (verbose)
      cerr << "Pulsar::StandardCandles::guess no entries loaded" << endl;
    return Signal::Unknown;
  }

  double closest = 0.0;
  Entry match;

  for (unsigned i=0; i<entries.size(); i++)
  {
    double separation = entries[i].position.angularSeparation(p).getDegrees();

    if (verbose)
      cerr << "Pulsar::StandardCandles::guess"
	   << " name=" << entries[i].source_name[0]
	   << " separation=" << separation << endl;

    if (i == 0 || separation < closest)
    {
      if (verbose)
	cerr << "Pulsar::StandardCandles::guess closest" << endl;
      closest = separation;
      match = entries[i];
    }
  }

  if (verbose && closest > off_radius)
    cerr << "Pulsar::StandardCandles::guess nothing close" << endl;

  if (match.reference_frequency == 0 || closest > off_radius)
    return Signal::Unknown;

  // a suitable match has been found

  // if the source name does not match, replace it with best match name
  if (!match.matches(name))
    name = match.source_name[0];

  if (closest > on_radius)
    return Signal::FluxCalOff;
  else
    return Signal::FluxCalOn;
}
