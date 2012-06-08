//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/FortranSNR.h,v $
   $Revision: 1.2 $
   $Date: 2009/03/17 05:58:13 $
   $Author: straten $ */

#ifndef __Pulsar_FortranSNR_h
#define __Pulsar_FortranSNR_h

#include "ReferenceAble.h"
#include <Pulsar/Profile.h>

namespace Pulsar {


  //! Calculates the signal-to-noise ratio by fitting against a standard
  class FortranSNR : public Reference::Able {

  public:

    FortranSNR(); 
    
    //! Return the signal to noise ratio based on the shift
    float get_snr (const Profile* profile);
    int get_bestwidth(){ return bestwidth; }

    void set_rms( float new_rms ) { rms = new_rms; rms_set = true; }
    void set_minwidthbins ( int bins ) { minwidthbins = bins; }
    void set_maxwidthbins ( int bins ) { maxwidthbins = bins; }
    void set_bestwidth(int bw) { bestwidth=bw; };

  private:
    float rms;
    int minwidthbins;
    int maxwidthbins;
    int bestwidth;
    bool rms_set;
  };

}

#endif

