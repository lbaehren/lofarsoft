//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/AuxColdPlasmaMeasures.h,v $
   $Revision: 1.2 $
   $Date: 2010/09/22 02:18:38 $
   $Author: straten $ */

#ifndef __Pulsar_AuxColdPlasmaMeasures_h
#define __Pulsar_AuxColdPlasmaMeasures_h

#include "Pulsar/IntegrationExtension.h"

namespace Pulsar {
  
  //! Stores parameters used to correct auxiliary dispersion and birefringence
  class AuxColdPlasmaMeasures : public Pulsar::Integration::Extension {
    
  public:
    
    //! Default constructor
    AuxColdPlasmaMeasures ();

    //! Return a newly constructed copy of self
    AuxColdPlasmaMeasures* clone () const;

    //! Return a text interfaces that can be used to access this instance
    TextInterface::Parser* get_interface();
    
    //! Set the auxiliary dispersion measure
    void set_dispersion_measure (double measure);
    //! Get the auxiliary dispersion measure
    double get_dispersion_measure () const;

    //! Set the auxiliary rotation measure
    void set_rotation_measure (double measure);
    //! Get the auxiliary rotation measure
    double get_rotation_measure () const;

    //! Text interface to a AuxColdPlasmaMeasures instance
    class Interface : public TextInterface::To<AuxColdPlasmaMeasures>
    {
    public:
      Interface (AuxColdPlasmaMeasures* = NULL);
    };

  protected:

    //! The auxiliary dispersion measure
    double dispersion_measure;

    //! The auxiliary rotation measure
    double rotation_measure;
  };
  

}

#endif

