/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <string>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "resio.h"
#include "residual.h"
#include "MJD.h"
#include "Error.h"

bool Tempo::residual::verbose = 0;

Tempo::residual::residual (int lun)
{
  init();
  if (load(lun) < 0)
    throw Error (FailedCall, "Tempo::residual load error");
}

void Tempo::residual::init ()
{
  mjd = turns = time = binaryphase = obsfreq 
    = weight = error = preres = 0.0;
  valid = false;
}

int Tempo::residual::load (int lun)
{
  int status = 0;
  obsfreq = -20.0;
  resread_ (&lun, &mjd, &turns, &time, &binaryphase,
	    &obsfreq, &weight, &error, &preres, &status);
  if (status == 0) {
    time *= 1e6;     // convert residual to micro-seconds
    preres *= 1e6;
    valid = true;
  }
  return status;
}

// ////////////////////////////////////////////////////////////////////////
// Tempo::residual::load
//
// Loads residual objects from the fortran binary file produced by TEMPO
// (usually resid2.tmp).
//
// r2flun    - fortran logical unit number with which to open the file
// filename  - name of the file
// residuals - vector of residual objects into which .size() residuals
//             will be loaded
// ////////////////////////////////////////////////////////////////////////
int Tempo::residual::load (int r2flun, char* filename, 
			   std::vector<residual>* residuals)
{
  fortopen_ (&r2flun, filename, (int) strlen(filename));
  if (verbose)
    fprintf (stderr, "residual::load (vector<residual>) '%s' opened\n", 
	     filename);

  residuals->erase (residuals->begin(), residuals->end());
  residual res_rd;

  while (res_rd.load (r2flun) == 0)
    residuals->push_back (res_rd);

  fortclose_ (&r2flun);
  return 0;
}

double Tempo::residual::dayofyear () const
{
  return fmod(UTCdoy(mjd), 1) * 365.25;
}

double Tempo::residual::utcyear () const
{
  return UTCdoy (mjd);
}

double UTCdoy (double mjd)
{
  MJD mjd1 (mjd);
  utc_t utc;
  mjd1.UTC (&utc);

  return double(utc.tm_year)
    + (double(utc.tm_yday) 
       + (double(utc.tm_hour)
	  + (double(utc.tm_min)
	     + double(utc.tm_sec)/60.0)/60.0)/24.0)/365.25;
}
