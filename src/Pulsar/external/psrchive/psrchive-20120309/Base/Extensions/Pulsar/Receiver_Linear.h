//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Receiver_Linear.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:05:50 $
   $Author: straten $ */

#ifndef __Receiver_Linear_h
#define __Receiver_Linear_h

#include "Pulsar/Receiver.h"

namespace Pulsar {

  //! Stores the X and Y receptor and calibrator orientations
  class Receiver::Linear : public Receiver::State {

  public:

    //! Default constructor
    Linear ();

    //! Get the basis of the feed receptors
    Signal::Basis get_basis () const;
    
    //! Get the hand of the basis
    Signal::Hand get_hand () const;
    
    //! Get the orientation of the basis about the line of sight
    Angle get_orientation () const;
    
    //! Get the phase of the reference source
    Angle get_reference_source_phase () const;
    
    //! Copy the state from another
    void copy (const State*);
    
    //! Get the offset of the feed X axis with respect to the platform zero
    Angle get_X_offset () const;
    //! Set the offset of the feed X axis with respect to the platform zero
    void set_X_offset (const Angle& offset);
    
    //! Get the offset of the feed Y axis from its nominal value
    Angle get_Y_offset () const;
    //! Set the offset of the feed Y axis from its nominal value
    void set_Y_offset (const Angle& offset);

    //! Get the offset of the feed calibrator axis from its nominal value
    Angle get_calibrator_offset () const;
    //! Set the offset of the feed calibrator axis from its nominal value
    void set_calibrator_offset (const Angle& offset);

  protected:

    //! Set true if the Y-axis is offset by 180 degrees
    bool y_offset;

    //! Set true if the calibrator is offset by 90 degrees
    bool calibrator_offset;

    //! The orientation of the x-asis about the line of sight
    Angle x_offset;
    
  };

}

#endif
