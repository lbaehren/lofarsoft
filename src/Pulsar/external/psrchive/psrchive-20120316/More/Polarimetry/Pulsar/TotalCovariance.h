//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/TotalCovariance.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Calibration_TotalCovariance_H
#define __Calibration_TotalCovariance_H

#include "Pulsar/TemplateUncertainty.h"
#include "Matrix.h"

namespace Calibration {

  //! Combines the uncertainty of the template and the observation
  class TotalCovariance : public TemplateUncertainty {

  public:

    //! Default constructor
    TotalCovariance ();

    //! Set the covariances of the observed Stokes parameters
    void set_covariance (const Matrix<4,4,double>& opt);

    //! Set the optimizing transformation applied to the template
    void set_optimizing_transformation (const Matrix<4,4,double>& opt);

    //! Given a coherency matrix, return the weighted norm
    double get_weighted_norm (const Jones<double>&) const;
    
    //! Given a coherency matrix, return the weighted conjugate matrix
    Jones<double> get_weighted_conjugate (const Jones<double>&) const;

  protected:

    //! The covariances of the observed Stokes parameters
    Matrix<4,4,double> observation_covariance;

    bool observation_covariance_set;

    //! The inverse of the total covariance matrix
    Matrix<4,4,double> inv_covar;

    //! The optimizing transformation
    Matrix<4,4,double> optimizer;

    //! Compute inv_covar attribute
    void build ();

  };

}

#endif
