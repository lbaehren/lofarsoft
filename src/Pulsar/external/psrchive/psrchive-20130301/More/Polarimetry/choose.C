/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Stokes.h"

using namespace std;

void choose (vector<unsigned>& bins, Pulsar::Archive* archive)
{
  if (bins.size() == 0)
    return;

  vector< Stokes<float> > stokes;

  archive->get_Integration(0)->get_Stokes( stokes, 0 );

  unsigned nbin = stokes.size();

  vector<bool> chosen (nbin, false);

  float maximum = 0.0;
  unsigned imax = 0;

  // first choose the phase bin with the largest polarization vector
  for (unsigned ibin=0; ibin<nbin; ibin++) {
    float ranking = stokes[ibin].abs_vect();
    if (ranking > maximum) {
      maximum = ranking;
      imax = ibin;
    }
  }

  bins[0] = imax;
  chosen[imax] = true;
  Vector<3,float> current_best = stokes[imax].get_vector ();

  // cerr << "c:" << imax << endl;

  for (unsigned ichoose=1; ichoose < bins.size(); ichoose++) {

    maximum = 0.0;

    for (unsigned ibin=0; ibin<nbin; ibin++) {

      // choose a mix of high p and I
      float ranking = stokes[ibin][0];
      if (ichoose % 2 == 0)
	ranking = stokes[ibin].abs_vect();

      if (ranking > maximum && !chosen[ibin]) {
	maximum = ranking;
	imax = ibin;
      }

    }

    bins[ichoose] = imax;
    chosen[imax] = true;
    current_best = stokes[imax].get_vector ();
 
    // cerr << "c:" << imax << endl;

  }

}

