//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/AuxColdPlasma.h,v $
   $Revision: 1.2 $
   $Date: 2010/09/22 02:18:38 $
   $Author: straten $ */

#ifndef __AuxColdPlasma_h
#define __AuxColdPlasma_h

#include "Pulsar/ArchiveExtension.h"

namespace Pulsar {

  //! Auxiliary cold plasma dispersion and birefringence corrections
  /*! This class stores the names of any models used to compute auxiliary
    DM and RM, and records if these effects have been corrected. */
  class AuxColdPlasma : public Archive::Extension {

    std::string dispersion_model_name;
    bool dispersion_corrected;

    std::string birefringence_model_name;
    bool birefringence_corrected;

  public:
    
    //! Default constructor
    AuxColdPlasma ();

    //! Clone method
    AuxColdPlasma* clone () const { return new AuxColdPlasma( *this ); }

    //! Return a text interfaces that can be used to access this instance
    TextInterface::Parser* get_interface();

    //! Return an abbreviated name that can be typed relatively quickly
    std::string get_short_name () const { return "aux"; }

    void set_dispersion_model_name (const std::string& name)
    { dispersion_model_name = name; }
    std::string get_dispersion_model_name () const
    { return dispersion_model_name; }

    void set_dispersion_corrected (bool flag) 
    { dispersion_corrected = flag; }
    bool get_dispersion_corrected () const
    { return dispersion_corrected; }

    void set_birefringence_model_name (const std::string& name)
    { birefringence_model_name = name; }
    std::string get_birefringence_model_name () const
    { return birefringence_model_name; }

    void set_birefringence_corrected (bool flag)
    { birefringence_corrected = flag; }
    bool get_birefringence_corrected () const
    { return birefringence_corrected; }

    //! Text interface to a AuxColdPlasma instance
    class Interface : public TextInterface::To<AuxColdPlasma>
    {
    public:
      Interface (AuxColdPlasma* = NULL);
    };

  };
 

}

#endif
