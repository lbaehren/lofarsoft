
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "T2Observatory.h"
#include "stringtok.h"

#include <fstream>

#include <math.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

Tempo2::Observatory::Observatory ()
{
  code = 0;
  x = y = z = 0;
}

char Tempo2::Observatory::get_code () const
{
  return code;
}

void Tempo2::Observatory::set_code (char c)
{
  code = c;
}

std::string Tempo2::Observatory::get_old_code () const
{
  return old_code;
}

void Tempo2::Observatory::set_old_code (const std::string& c)
{
  old_code = c;
}

std::string Tempo2::Observatory::get_name () const
{
  return name;
}

void Tempo2::Observatory::set_name (const std::string& n)
{
  name = n;
}

void Tempo2::Observatory::set_xyz (double _x, double _y, double _z)
{
  x = _x; y = _y; z = _z;
}

void Tempo2::Observatory::get_xyz (double& _x, double& _y, double& _z) const
{
  _x = x; _y = y; _z = z;
}

//! Get the latitude and longitude in radians
void Tempo2::Observatory::get_sph (double& lat,
				   double& lon,
				   double& rad) const
{
#ifdef _DEBUG
  cerr << "Tempo2::Observatory::get_sph"
          " x=" << x << " y=" << y << " z=" << z << endl;
#endif

  rad = sqrt(x*x + y*y + z*z);
  lat = asin (z/rad);
  lon = atan2 (y, x);
}

static vector< Reference::To<Tempo2::Observatory> > antennae;

static void load_observatories ();

const Tempo2::Observatory*
Tempo2::observatory (const string& telescope_name)
{
  load_observatories ();

  // if the name is a single character, then it is likely a tempo site number
  if (telescope_name.length() == 1)
    for (unsigned i=0; i < antennae.size(); i++)
      if (antennae[i]->get_code() == telescope_name[0])
	return antennae[i];

  for (unsigned i=0; i < antennae.size(); i++)
    if (strcasestr( antennae[i]->get_name().c_str(), telescope_name.c_str() ))
      return antennae[i];

  for (unsigned i=0; i < antennae.size(); i++)
    if (antennae[i]->get_old_code() == telescope_name)
      return antennae[i];

  throw Error (InvalidParam, "Tempo2::observatory",
               "no antennae named '" + telescope_name + "'");
}

static void load_aliases ()
{
  char* tempo2_dir = getenv ("TEMPO2");
  if (!tempo2_dir)
    throw Error (FailedSys, "Tempo2::load_aliases",
		 "TEMPO2 environment variable not defined");

  string filename = tempo2_dir + string("/observatory/aliases");

  ifstream input (filename.c_str());
  if (!input)
    throw Error (FailedSys, "Tempo2::load_aliases",
		 "ifstream (" + filename + ")");

  string line;

  while (!input.eof())
  {
    getline (input, line);
    line = stringtok (line, "#\n", false);  // get rid of comments

    if (!line.length())
      continue;

    char name[32], code;

    if (sscanf (line.c_str(), "%s %c", name, &code) != 2)
      throw Error (InvalidParam, "Tempo::load_aliases",
		   "failed to parse '%s'", line.c_str());

    for (unsigned i=0; i < antennae.size(); i++)
      if (antennae[i]->get_old_code() == name)
	antennae[i]->set_code (code);
  }
}

void load_observatories ()
{
  static bool observatories_loaded = false;

  if (observatories_loaded)
    return;

  char* tempo2_dir = getenv ("TEMPO2");
  if (!tempo2_dir)
    throw Error (FailedSys, "Tempo2::load_observatories",
		 "TEMPO2 environment variable not defined");

  string filename = tempo2_dir + string("/observatory/observatories.dat");

  ifstream input (filename.c_str());
  if (!input)
    throw Error (FailedSys, "Tempo2::load_observatories",
		 "ifstream (" + filename + ")");

  string line;

  while (!input.eof())
  {
    getline (input, line);
    line = stringtok (line, "#\n", false);  // get rid of comments

    if (!line.length())
      continue;

    double x, y, z;
    char name[32], code[32];

    if (sscanf (line.c_str(), "%lf %lf %lf %s %s", &x,&y,&z, name, code) != 5)
      throw Error (InvalidParam, "Tempo::observatories",
		   "failed to parse '%s'", line.c_str());

    Reference::To<Tempo2::Observatory> observatory = new Tempo2::Observatory;

    observatory->set_xyz (x, y, z);
    observatory->set_name( name );
    observatory->set_old_code( code );

    antennae.push_back( observatory );
  }

  load_aliases ();

  observatories_loaded = true;
}
