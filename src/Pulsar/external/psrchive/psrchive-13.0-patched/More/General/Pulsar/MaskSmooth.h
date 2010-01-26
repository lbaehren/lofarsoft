//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/MaskSmooth.h,v $
   $Revision: 1.1 $
   $Date: 2007/07/12 05:59:08 $
   $Author: straten $ */

#ifndef __Pulsar_MaskSmooth_h
#define __Pulsar_MaskSmooth_h

#include "Pulsar/PhaseWeightSmooth.h"

namespace Pulsar {

  class Profile;

  //! Smooths a mask by masking elements with masked neighbours
  class MaskSmooth : public PhaseWeightSmooth {

  public:

    //! Default constructor
    MaskSmooth ();

    //! Set the smoothing window fraction
    void set_window (double);
    //! Get the smoothing window fraction
    double get_window () const;

    //! Set the fraction of masked neighbours required
    void set_masked (double);
    //! Get the fraction of masked neighbours required
    double get_masked () const;

  protected:

    double window;
    double masked;

    void calculate (PhaseWeight*);

  };

}


#endif // !defined __Pulsar_BaselineWeight_h
