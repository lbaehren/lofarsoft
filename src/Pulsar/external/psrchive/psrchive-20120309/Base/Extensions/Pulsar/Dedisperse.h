//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Dedisperse.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:05:50 $
   $Author: straten $ */

#ifndef __Pulsar_Dedisperse_h
#define __Pulsar_Dedisperse_h

#include "Pulsar/ColdPlasmaHistory.h"

namespace Pulsar {
  
  //! Stores parameters used to correct dispersion in each Integration
  class Dedisperse : public ColdPlasmaHistory {
    
  public:
    
    //! Default constructor
    Dedisperse ();

    //! Copy constructor
    Dedisperse (const Dedisperse& extension);

    //! Assignment operator
    const Dedisperse& operator= (const Dedisperse& extension);
    
    //! Clone method
    Dedisperse* clone () const { return new Dedisperse( *this ); }

    //! Set the dispersion measure
    void set_dispersion_measure (double dispersion_measure)
    { set_measure (dispersion_measure); }

    //! Get the dispersion measure
    double get_dispersion_measure () const
    { return get_measure (); }

  };
  
}

#endif

