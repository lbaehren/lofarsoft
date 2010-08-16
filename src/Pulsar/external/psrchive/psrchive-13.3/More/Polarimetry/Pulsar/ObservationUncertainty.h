//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ObservationUncertainty.h,v $
   $Revision: 1.6 $
   $Date: 2009/08/14 07:35:30 $
   $Author: straten $ */

#ifndef __Calibration_ObservationUncertainty_H
#define __Calibration_ObservationUncertainty_H

#include "Pulsar/CoherencyMeasurement.h"

namespace Calibration {

  //! Combines the uncertainty of the template and the observation
  class ObservationUncertainty : public CoherencyMeasurement::Uncertainty
  {

  public:

    //! Default constructor
    ObservationUncertainty () { }

    //! Construct with the uncertainty of the observation
    ObservationUncertainty (const Stokes<double>& variance);

    //! Set the uncertainty of the observation
    virtual void set_variance (const Stokes<double>&);

    //! Set the uncertainty of the observation
    virtual void set_variance (const Stokes< std::complex<double> >&);

    //! Given a coherency matrix, return the weighted norm
    virtual double get_weighted_norm (const Jones<double>&) const;
    
    //! Given a coherency matrix, return the weighted conjugate matrix
    virtual Jones<double> get_weighted_conjugate (const Jones<double>&) const;

    //! Given a coherency matrix, return the weighted Stokes parameters
    Stokes< std::complex<double> >
    get_weighted_components (const Jones<double>&) const;

    //! Return the variance of each Stokes parameter
    Stokes< std::complex<double> > get_variance () const;

  protected:

    //! The inverse of the observation variance
    Stokes< std::complex<double> > inv_variance;

  };

}

#endif
