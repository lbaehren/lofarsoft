//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/DisperseWeight.h,v $
   $Revision: 1.2 $
   $Date: 2009/06/24 05:02:08 $
   $Author: straten $ */

#ifndef __Pulsar_DisperseWeight_h
#define __Pulsar_DisperseWeight_h

#include "Pulsar/Algorithm.h"

namespace Pulsar {

  class Integration;

  class PhaseWeight;
  class PhaseWeightShift;
  class Dispersion;

  //! Shift phase weight mask by dispersion delay
  class DisperseWeight : public Algorithm
  {

  public:

    //! Default constructor
    DisperseWeight (const Integration*);

    //! Destructor
    ~DisperseWeight  ();

    //! Return a copy constructed instance of self
    DisperseWeight* clone () const;

    //! Set the PhaseWeight mask to be shifted
    void set_weight (const PhaseWeight*);

    //! Get the shifted PhaseWeight mask for the specified channel
    void get_weight (unsigned ichan, PhaseWeight*);

  protected:

    //! Used to shift PhaseWeight masks by dispersion delay
    mutable Reference::To<PhaseWeightShift> shift;

    //! Used to compute dispersion delay for PhaseWeightShift
    mutable Reference::To<Dispersion> dispersion;

    //! The Integration instance for whom this class is working
    Reference::To<const Integration> integration;

  }; 

}

#endif
