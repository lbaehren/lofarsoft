/***************************************************************************
 *
 *   Copyright (C) 2007 by Jonathan Khoo
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FortranSNR.h"
#include "Pulsar/Profile.h"

#include <math.h>
#include <config.h>

using namespace std;
using namespace Pulsar;

#define F77_smooth_mw F77_FUNC_(smooth_mw,SMOOTH_MW)

extern "C" void F77_smooth_mw(float* period, int* nbin, int* maxw, float* rms,
                               int * kwmax, float * snrmax, float * smmax,
                               float * workspace);

#define F77_smooth_mmw F77_FUNC_(smooth_mmw,SMOOTH_MMW)

extern "C" void F77_smooth_mmw(float* period, int* nbin, int* minw, int* maxw, 
			       float* rms,
                               int * kwmax, float * snrmax, float * smmax,
                               float * workspace);

FortranSNR::FortranSNR() 
{
  minwidthbins = 0;
  rms = 0;
  rms_set = false;
  bestwidth = 0;
}

float FortranSNR::get_snr (const Profile* profile)
{
  int nb = profile->get_nbin();
  int kwmax;
  int maxw;
  if (maxwidthbins != 0){
    maxw = maxwidthbins;
  }
  else
    maxw = nb/2;

  float snrmax,smmax;

  float * workspace = new float[nb];

  float* amps = const_cast<float*>( profile->get_amps() );

  if (minwidthbins == 0) {
    F77_smooth_mw(amps,&nb,&maxw,&rms,&kwmax,&snrmax,&smmax,workspace);
    set_bestwidth(kwmax);
  }
  else
    {
      //      cerr << "calling mmw " << minwidthbins <<" " <<maxwidthbins << endl;
    F77_smooth_mmw(amps,&nb,&minwidthbins,&maxw,&rms,&kwmax,&snrmax,&smmax,workspace);
    set_bestwidth(kwmax);
    }
  delete [] workspace;
  return snrmax;
}



