//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/IntegrationMeta.h,v $
   $Revision: 1.1 $
   $Date: 2008/02/05 05:09:55 $
   $Author: straten $ */

#ifndef __Pulsar_Integration_Meta_h
#define __Pulsar_Integration_Meta_h

#include "Pulsar/Integration.h"

namespace Pulsar {

  //! Stores otherwise shared attributes of orphaned Integrations

  class Integration::Meta : public Reference::Able 
  {
  
  public:

    //! Construct from the parent Archive instance
    Meta (const Archive*);

    //! Get the centre frequency (in MHz)
    double get_centre_frequency() const;
    void set_centre_frequency (double);

    //! Get the bandwidth (in MHz)
    double get_bandwidth() const;
    void set_bandwidth (double);

    //! Get the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    double get_dispersion_measure () const;
    void set_dispersion_measure (double);

    //! Inter-channel dispersion delay has been removed
    bool get_dedispersed () const;
    void set_dedispersed (bool);

    //! Get the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    double get_rotation_measure () const;
    void set_rotation_measure (double);

    //! Data has been corrected for ISM faraday rotation
    bool get_faraday_corrected () const;
    void set_faraday_corrected (bool);

    //! Get the feed configuration of the receiver
    Signal::Basis get_basis () const;
    void set_basis (Signal::Basis);

    //! Get the polarimetric state of the profiles
    Signal::State get_state () const;
    void set_state (Signal::State);

  protected:

    double centre_frequency;
    double bandwidth;
    double dispersion_measure;
    bool dedispersed;
    double rotation_measure;
    bool faraday_corrected;
    Signal::Basis basis;
    Signal::State state;

  };

}

#endif
