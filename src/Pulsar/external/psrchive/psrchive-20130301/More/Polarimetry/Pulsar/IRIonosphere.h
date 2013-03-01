//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/IRIonosphere.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Calibration_IRIonosphere_H
#define __Calibration_IRIonosphere_H

#include "Pulsar/Faraday.h"
#include "Horizon.h"

namespace Calibration {

  //! International Reference Ionosphere (IRI) transformation
  /*! This class computes the Faraday rotation due to ionospheric
    electrons. */
  class IRIonosphere : public MEAL::Complex2 {

  public:

    //! Default constructor
    IRIonosphere ();

    //! Return the name of the class
    std::string get_name () const;

    //! Set the reference frequency in MHz
    void set_reference_frequency (double MHz);
    //! Get the reference frequency in MHz
    double get_reference_frequency () const;

    //! Set the frequency in MHz
    void set_frequency (double MHz);
    //! Get the frequency in MHz
    double get_frequency () const;

    //! Set the Horizon pointing parameters
    void set_horizon (const Horizon&);

    //! Get the Faraday rotation measure
    double get_rotation_measure () const;

  protected:

    //! The Faraday rotation transformation
    Calibration::Faraday faraday;

    //! The Horizon pointing paramters
    Horizon horizon;

    //! Compute the rotation measure
    void build ();

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result,
      std::vector<Jones<double> >*) {}

  };

}

#endif
