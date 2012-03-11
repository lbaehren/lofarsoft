
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "tempo++.h"
#include "tempo_impl.h"
#include "Angle.h"

#include <fstream>
#include <math.h>

/*

THIS FILE IMPLEMENTS CODE THAT PARSES THE TEMPO
OBSERVATORY COORDINATE FILE (typically 'obsys.dat')

This file contains geographical locations of observatories.  There is
one line per observatory.  The information in the first line is used
for TOAs tagged with observatory number 1, the second line for number
2, and so on.  The tenth line corresponds to observatory 'a', the
eleventh to 'b', and so on.

    Col       Item
   -----     ---------------
    1-15     coordinate 1
   16-30     coordinate 2
   31-45     coordinate 3
   48-48     flag
   51-62     observatory name
   71-71     observatory number
   74-75     observatory code

If the flag is blank or zero, the coordinates are taken to be latitude
(ddmmss.ss), longitude (ddmmss.ss), and elevation (m).  If the flag is
non-blank, the coordinates are taken as X, Y, Z (m) about the
geocenter.

The observatory code is a 2-character code used in ITOA format.

NOTE: TEMPO longitude is measured in degrees West of Greenwich

*/

using namespace std;

vector< Reference::To<Tempo::Observatory> > Tempo::antennae;

static bool obsys_loaded = false;

void Tempo::load_obsys () try
{
  if (obsys_loaded)
    return;

  string filename = get_configuration ("OBSYS");

  ifstream input (filename.c_str());
  if (!input)
    throw Error (FailedSys, "Tempo::obsys",
		 "ifstream (" + filename + ")");

  string line;

  while (!input.eof())
  {
    getline (input, line);

    if (!line.length())
      continue;

    double coordinate[3];
    unsigned offset[3] = { 0, 15, 30 };

    for (unsigned i=0; i<3; i++)
      if (sscanf (line.c_str()+offset[i], "%lf", coordinate+i) != 1)
	throw Error (InvalidParam, "Tempo::obsys",
		     "failed to parse coordinate[%d] from '%s'",
		     i, line.c_str());

    Reference::To<Observatory> observatory;

    if (line[47] == '1')
      observatory = new ObservatoryITRF (coordinate[0],
					 coordinate[1],
					 coordinate[2]);
    else
    {
      Angle alat, alon;
      alat.setDegMS (coordinate[0]);
      // obsys.dat lists degrees West longitude!
      alon.setDegMS (-coordinate[1]);

      // cerr << "ddmmss=" << coordinate[0] << " " << coordinate[1] << endl;
      // cerr << "again=" << alat.getDegMS() << " " << alon.getDegMS() << endl;

      observatory = new ObservatoryIAU1976 (alat.getRadians(),
					    alon.getRadians(),
					    coordinate[2]);
    }

    observatory->set_name( line.substr (50, 19) );
    observatory->set_code( line[70] );
    observatory->set_itoa_code( line.substr (73, 2) );

    antennae.push_back( observatory );
  }

  obsys_loaded = true;
}
catch (Error& error)
{
  throw error += "Tempo::load_obsys";
}
