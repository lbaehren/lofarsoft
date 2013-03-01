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

    //! Return a copy constructed instance of self
    MaskSmooth* clone () const;

    //! Set the smoothing window fraction in turns
    void set_turns (double);
    //! Get the smoothing window fraction in turns
    double get_turns () const;

    //! Set the fraction of masked neighbours required
    void set_masked_fraction (double);
    //! Get the fraction of masked neighbours required
    double get_masked_fraction () const;

    //! Set the width of the smoothing window in phase bins
    void set_bins (unsigned);
    //! Get the width of the smoothing window in phase bins
    unsigned get_bins () const;

    //! Set the number of masked neighbours required
    void set_masked_bins (unsigned);
    //! Get the number of masked neighbours required
    unsigned get_masked_bins () const;


  protected:

    double turns;
    double masked_fraction;

    unsigned bins;
    unsigned masked_bins;

    float mask_value;

    void calculate (PhaseWeight*);

  };

}


#endif // !defined __Pulsar_BaselineWeight_h
