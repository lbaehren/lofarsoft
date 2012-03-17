//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/ColdPlasmaHistory.h,v $
   $Revision: 1.3 $
   $Date: 2008/11/27 06:16:15 $
   $Author: straten $ */

#ifndef __Pulsar_ColdPlasmaHistory_h
#define __Pulsar_ColdPlasmaHistory_h

#include "Pulsar/IntegrationExtension.h"

namespace Pulsar {
  
  //! Stores parameters used to correct Faraday rotation in each Integration
  class ColdPlasmaHistory : public Pulsar::Integration::Extension {
    
  public:
    
    //! Default constructor
    ColdPlasmaHistory (const char* name);

    //! Copy constructor
    ColdPlasmaHistory (const ColdPlasmaHistory& extension);

    //! Assignment operator
    const ColdPlasmaHistory& operator= (const ColdPlasmaHistory& extension);
    
    //! Destructor
    ~ColdPlasmaHistory ();

    //! Set the correction measure
    void set_measure (double measure);
    //! Get the correction measure
    double get_measure () const;

    //! Set the reference wavelength in metres
    void set_reference_wavelength (double metres);
    //! Get the reference wavelength
    double get_reference_wavelength () const;

    //! Set the reference frequency in MHz
    void set_reference_frequency (double MHz);
    //! Get the reference frequency
    double get_reference_frequency () const;

  protected:

    //! The correction measure
    double measure;

    //! The reference wavelength in metres
    double reference_wavelength;

  };
  
}

#endif

