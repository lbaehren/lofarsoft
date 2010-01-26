/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "tempo++.h"
#include "toa.h"
#include "residual.h"
#include "resio.h"
#include "Error.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

using namespace std;

// ////////////////////////////////////////////////////////////////////////
// Tempo::fit
//
// runs TEMPO on a vector of toa objects, using a given Parameters object.
// returns a vector of residual objects, along with a new "post-fit"
// Parameters object.
//
// model     - input TEMPO-style PSR ephemeris
// toas      - input toas
// postfit   - new PSR ephemeris output by TEMPO
// residuals - vector of residual objects into which tempo output will 
//             be loaded
// ////////////////////////////////////////////////////////////////////////

void Tempo::fit (const Pulsar::Parameters* model, vector<toa>& toas,
		 Pulsar::Parameters* postfit,
		 bool track,
		 Tempo::toa::State min_state)
{
  char* tempo_tim = "arrival.tim";
  char* tempo_par = "arrival.par";
  char* tempo_res = "resid2.tmp";
  
  int   r2flun = 32;

  if (verbose)
    cerr << "Tempo::fit writing TOAs to '" << tempo_tim << "'" << endl;

  // unload the toas into a temporary file
  FILE* fptr = fopen (tempo_tim, "w");
  if (fptr==NULL) {
    fprintf (stderr, "fit error opening %s:\n", tempo_tim);
    throw Error (FailedCall, "fit() cannot open file");
  }

  int      unloaded = 0;       // count of toas unloaded
  double   sumphase = 0.0;     // phase sum used for tracking mode
  unsigned iarr;
  
  for (iarr=0; iarr < toas.size(); iarr++)  {
    
    if (toas[iarr].get_state() < min_state)
      if ((toas[iarr].get_format() != Tempo::toa::Command) &&
	  (toas[iarr].get_format() != Tempo::toa::Comment))
	continue;
    
    if ((toas[iarr].get_format() != Tempo::toa::Command) &&
	(toas[iarr].get_format() != Tempo::toa::Comment))
      if (track && !toas[iarr].resid.valid) {
	cerr << "Tempo::fit invalid residual for toa #" << unloaded + 1
	     << " ... tracking disabled" << endl;
	track = false;
      }
    
    if (track && ((toas[iarr].get_format() != Tempo::toa::Command) &&
		  (toas[iarr].get_format() != Tempo::toa::Comment))) {
      
      if (unloaded == 0)
	sumphase = toas[iarr].resid.turns;
      
      /* straten copied this from psrclock:
	 This is the agonising bit. 
	 Following makes up for things not starting at zero.
	 This is equivalent to standard tempo only: we can
	 simulate tracking mode by making sure what is plotted is
	 how tempo will interpret it. 
      */
      
      double current_phase = toas[iarr].resid.turns;
      float  sumout = 0.0;
      
      while (current_phase - sumphase > 0.5) {
	sumphase ++; sumout ++;
      }
      while (current_phase - sumphase < -0.5) {
	sumphase --; sumout --;
      }
      if (sumout > 0.5) fprintf  (fptr, "PHASE +%f\n", sumout);
      if (sumout < -0.5) fprintf (fptr, "PHASE %f\n", sumout);
      
    }
    toas[iarr].Tempo_unload (fptr);
    unloaded ++;
  }

  fclose (fptr); 
  if (verbose)
    cerr << "Tempo::fit unloaded " << unloaded << " TOAs to '" 
	 << tempo_tim << "'" << endl;

  // unload the ephemeris
  if (verbose)
    cerr << "Tempo::fit writing parameters to '" << tempo_par << "'" << endl;
  model->unload(tempo_par);

  // run TEMPO
  string runtempo = get_system() + " " + string(tempo_tim) + " > /dev/null";

  if (verbose)
    cerr << "Tempo::fit system (" << runtempo << ")" << endl;

  int retval = system (runtempo.c_str());
  if (retval != 0)  {
    cerr << "Tempo::fit ERROR system (\"" << runtempo 
	 << "\") returns " << retval;
    if (retval < 0)
      perror (" ");
    else
      cerr << ":: tempo returns:" << WIFEXITED(retval) << endl;
    throw Error (FailedCall, "Tempo::fit");
  }
  
  // load the new ephemeris (PSRNAME.par in current working directory)
  if (postfit)
    postfit->load( (model->get_name() + ".par").c_str() );

  // load the residuals from resid2.tmp
  fortopen_ (&r2flun, tempo_res, (int) strlen(tempo_res));

  for (iarr=0; iarr < toas.size(); iarr++)  {
    
    if ((toas[iarr].get_format() == Tempo::toa::Command) ||
	(toas[iarr].get_format() == Tempo::toa::Comment)) {
      unloaded --;
      continue;
    }
    
    if (toas[iarr].get_state() < min_state)
      continue;
    
    if (unloaded == 0)
      throw Error (InvalidParam, "Tempo::fit error attempting to read more than unloaded");

    if (toas[iarr].resid.load (r2flun) != 0)
      throw Error (FailedCall, "Tempo::fit error reading residual file");
    unloaded --;
  }
  if (unloaded != 0)
    throw Error (InvalidParam, "Tempo::fit error read fewer residuals than unloaded toas");
  
  fortclose_ (&r2flun);
}


