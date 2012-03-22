//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Craig West & Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/dspsr/dspsr/Kernel/Formats/lbadr64/dsp/LBADR64_TwoBitCorrection.h,v $
   $Revision: 1.1 $
   $Date: 2007/02/23 04:29:36 $
   $Author: ahotan $ */

#ifndef __LBADR64_TwoBitCorrection_h
#define __LBADR64_TwoBitCorrection_h

#include "dsp/TwoBitCorrection.h"

namespace dsp {
  
  //! Converts LBADR64 data from 2-bit digitized to floating point values
  class LBADR64_TwoBitCorrection: public TwoBitCorrection {

  public:

    //! Constructor initializes base class atributes
    LBADR64_TwoBitCorrection ();

    //! Return true if LBADR64_TwoBitCorrection can convert the Observation
    virtual bool matches (const Observation* observation);

  };
  
}

#endif
