//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Smooth.h,v $
   $Revision: 1.10 $
   $Date: 2007/11/02 04:25:27 $
   $Author: straten $ */

#ifndef __Pulsar_Smooth_h
#define __Pulsar_Smooth_h

#include "Pulsar/Transformation.h"
#include "TextInterface.h"

namespace Pulsar {

  class Profile;

  //! Profile smoothing algorithms
  class Smooth : public Transformation<Profile> {

  public:

    //! Default constructor
    Smooth ();

    //! Destructor
    virtual ~Smooth ();

    //! Return a copy constructed instance of self
    virtual Smooth* clone () const = 0;

    //! Return a text interface that can be used to configure this instance
    virtual TextInterface::Parser* get_interface () = 0;

    //! Text-based interface to Smooth class properties
    class Interface;

    //! Set the width of the smoothing window in turns
    void set_turns (float);

    //! Get the width of the smoothing window in turns
    float get_turns () const;

    //! Get the width of the smoothing window in turns from the last transform
    float get_last_turns () const;

    //! Set the width of the smoothing window in phase bins
    void set_bins (float);

    //! Get the width of the smoothing window in phase bins
    float get_bins () const;

    //! Get the width of the smoothing window in turns from the last transform
    float get_last_bins () const;

    //! Construct a new Smooth instance from a string
    static Smooth* factory (const std::string& name_and_options);

  protected:

    //! Get the width of the smoothing window in phase bins
    float get_bins (const Profile*);

    //! Get the width of the smoothing window in turns
    float get_turns (const Profile*);

    //! The width of the bins used to smooth
    float turns;

    //! The number of phase bins in the bins used to smooth
    float bins;

    float last_turns, last_bins;

  };

  //! 
  class Smooth::Interface : public TextInterface::To<Smooth>
  {
  public:
    Interface (Smooth* instance);
  };
}

#endif
