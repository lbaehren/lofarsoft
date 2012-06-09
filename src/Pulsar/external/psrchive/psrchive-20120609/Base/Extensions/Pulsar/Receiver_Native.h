//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Receiver_Native.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:05:50 $
   $Author: straten $ */

#ifndef __Receiver_Native_h
#define __Receiver_Native_h

#include "Pulsar/Receiver.h"

namespace Pulsar {

  //! Stores receptor basis, hand, and orientation, and calibrator phase
  class Receiver::Native : public Receiver::State {

  public:

    //! Default constructor
    Native ();

    //! Get the basis of the feed receptors
    Signal::Basis get_basis () const
    { return basis; }

    //! Set the basis of the feed receptors
    void set_basis (Signal::Basis _basis)
    { basis = _basis; }
    
    //! Get the hand of the basis
    Signal::Hand get_hand () const
    { return hand; }
    
    //! Set true if the basis is right-handed
    void set_hand (Signal::Hand h)
    { hand = h; }
    
    //! Get the orientation of the basis about the line of sight
    Angle get_orientation () const
    { return orientation; }

    //! Set the orientation of the basis about the line of sight
    void set_orientation (const Angle& angle)
    { orientation = angle; }

    //! Get the phase of the reference source
    Angle get_reference_source_phase () const
    { return reference_source_phase; }

    //! Set the phase of the reference source
    void set_reference_source_phase (const Angle& phase)
    { reference_source_phase = phase; }
    
    //! Copy the state from another
    void copy (const State*);
    
  protected:
    
    //! Basis of the feed receptors
    Signal::Basis basis;
    
    //! The hand of the basis
    Signal::Hand hand;
    
    //! The orientation of the basis about the line of sight
    Angle orientation;
    
    //! Phase of p^* q for reference source
    Angle reference_source_phase;

  };

}

#endif
