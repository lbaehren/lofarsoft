//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/Pulsar/ScintArchive.h,v $
   $Revision: 1.7 $
   $Date: 2008/11/07 22:15:36 $
   $Author: straten $ */

#ifndef __Scint_Archive_h
#define __Scint_Archive_h

#include "TimerArchive.h"
#include "baseband_header.h"

#include "Pulsar/Passband.h"
#include "Pulsar/dspReduction.h"
#include "Pulsar/TwoBitStats.h"

namespace Pulsar {

  //! A TimerArchive with transposed phase and frequency axis
  class ScintArchive : public TimerArchive {

  public:

    //! Default constructor
    ScintArchive ();

    //! Copy constructor
    ScintArchive (const ScintArchive& archive);

    //! Destructor
    ~ScintArchive ();

    //! Assignment operator
    const ScintArchive& operator = (const ScintArchive& archive);
    
    //! Copy all of the class attributes and the selected Integration data
    void copy (const Archive& archive);

    //! Return a pointer to a new copy constructed instance equal to this
    virtual ScintArchive* clone () const;

    // //////////////////////////////////////////////////////////////////
    //
    // ScintArchive specific
    //

    //! Return a reference to the bandpass in the given channel
    const std::vector<float>& get_passband (unsigned channel) const;

    bool takes (const ScintArchive* archive) const;

    void integrate (const ScintArchive* archive);

  protected:

    std::vector< std::vector<float> > spectra;

    friend class Archive::Advocate<ScintArchive>;

    // Advocates the use of the ScintArchive plugin
    class Agent : public Archive::Advocate<ScintArchive> {

      public:

        //! Default constructor (necessary even when empty)
        Agent () {}

        //! Advocate the use of ScintArchive to interpret filename
        bool advocate (const char* filename);

        //! Return the name of the ScintArchive plugin
        std::string get_name () { return "Scint"; }

        //! Return description of this plugin
        std::string get_description ();

    };

    //! Update the spectra attribute
    void init_spectra ();

    //! Load ScintArchive specific information
    void backend_load (FILE* fptr);

    //! Unload ScintArchive specific information
    void backend_unload (FILE* fptr) const;

    //! Over-ride TimerArchive::correct
    void correct ();

    void set_be_data_size ();

  };


}

#endif


