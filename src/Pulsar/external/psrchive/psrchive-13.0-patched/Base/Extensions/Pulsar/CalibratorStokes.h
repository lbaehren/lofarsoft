//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/CalibratorStokes.h,v $
   $Revision: 1.9 $
   $Date: 2009/03/24 03:00:36 $
   $Author: straten $ */

#ifndef __CalibratorStokes_h
#define __CalibratorStokes_h

#include "Pulsar/ArchiveExtension.h"
#include "Stokes.h"
#include "Estimate.h"

namespace Pulsar {
  
  //! Stores the Stokes parameters of the reference source
  /*! This Archive::Extension class provides a simple container for storing
   the Stokes parameters of the artificial (reference) calibrator source. */
  class CalibratorStokes : public Pulsar::Archive::Extension {
    
  public:
    
    //! Default constructor
    CalibratorStokes ();

    //! Copy constructor
    CalibratorStokes (const CalibratorStokes& extension);

    //! Assignment operator
    const CalibratorStokes& operator= (const CalibratorStokes& extension);

    //! Destructor
    ~CalibratorStokes ();

    //! Clone method
    CalibratorStokes* clone () const { return new CalibratorStokes( *this ); }

    //! Get the text ineterface 
    TextInterface::Parser* get_interface();

    //! Implements the text interface
    class Interface;

    //! Return a short name
    std::string get_short_name () const { return "ref"; }

    //! Set the number of frequency channels
    void set_nchan (unsigned nchan);
    //! Get the number of frequency channels
    unsigned get_nchan () const;

    //! Remove the inclusive range of channels
    void remove_chan (unsigned first, unsigned last);

    //! Set the validity flag for the specified channel
    void set_valid (unsigned ichan, bool valid);
    //! Get the validity flag for the specified channel
    bool get_valid (unsigned ichan) const;

    //! Set the Stokes parameters of the specified frequency channel
    void set_stokes (unsigned ichan, const Stokes< Estimate<float> >& stokes);
    //! Get the Stokes parameters of the specified frequency channel
    Stokes< Estimate<float> > get_stokes (unsigned ichan) const;

  protected:

    //! The Stokes parameters for each frequency channel
    /*! Stokes I is used to represent data validity */
    std::vector< Stokes< Estimate<float> > > stokes;

    //! Ensure that ichan <= get_nchan
    void range_check (unsigned ichan, const char* method) const;

    friend class Interface;

    // Internal convenience interface to the Stokes polarization vector data
    class PolnVector;

    //! Interface to StokesVector
    PolnVector* get_poln (unsigned ichan);

    Reference::To<PolnVector> current;
  };
 

}

#endif
