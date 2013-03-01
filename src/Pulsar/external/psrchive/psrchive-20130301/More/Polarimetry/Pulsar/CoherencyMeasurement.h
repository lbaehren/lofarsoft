//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/CoherencyMeasurement.h,v $
   $Revision: 1.12 $
   $Date: 2009/08/14 07:35:30 $
   $Author: straten $ */

#ifndef __Calibration_CoherencyMeasurement_H
#define __Calibration_CoherencyMeasurement_H

#include "Reference.h"
#include "Estimate.h"
#include "Stokes.h"
#include "Jones.h"

#include <complex>

namespace Calibration {

  //! A coherency matrix measurement and its estimated error
  /* Among other things, this class implements the interface of the
     MEAL::WeightingScheme class used by MEAL::LevenbergMacquardt */
  class CoherencyMeasurement {

  public:

    //! Base class of error propagation handlers
    class Uncertainty;

    //! Default constructor
    CoherencyMeasurement (unsigned input_index = 0);

    //! Set the index of the input to which the measurement corresponds
    void set_input_index (unsigned index);

    //! Set the index of the input to which the measurement corresponds
    unsigned get_input_index () const;

    //! Set the measured Stokes parameters
    void set_stokes (const Stokes< Estimate<double> >& stokes);

    //! Set the measured complex Stokes parameters
    void set_stokes (const Stokes< std::complex<double> >& stokes,
		     const Stokes<double>& variance);

    //! Set the measured complex Stokes parameters and the variance function
    void set_stokes (const Stokes< std::complex<double> >& stokes, 
		     const Uncertainty* var);

    //! Get the number of constraints provided by this measurement
    unsigned get_nconstraint () const;

    //! Get the measured coherency matrix
    Jones<double> get_coherency () const;

    //! Get the measured Stokes parameters
    Stokes< Estimate<double> > get_stokes () const;

    //! Get the measured Stokes parameters
    Stokes< std::complex< Estimate<double> > > get_complex_stokes () const;

    //! Given a coherency matrix, return the weighted norm
    double get_weighted_norm (const Jones<double>& matrix) const;

    //! Given a coherency matrix, return the weighted conjugate matrix
    Jones<double> get_weighted_conjugate (const Jones<double>& matrix) const;

    void get_weighted_components (const Jones<double>&,
				  std::vector<double>& components) const;

  protected:

    //! Index of the input to which the measurement corresponds
    unsigned input_index;

    //! The number of constraints provided by this measurement
    unsigned nconstraint;

    //! The coherency matrix measurement
    Jones<double> rho;

    //! The variance (if given)
    Stokes<double> variance;

    //! The uncertainty of the measurement
    Reference::To<const Uncertainty> uncertainty;

  };

  //! Estimates the uncertainty of a CoherencyMeasurement
  class CoherencyMeasurement::Uncertainty : public Reference::Able {
  public:

    //! Given a coherency matrix, return the weighted norm
    virtual 
    double
    get_weighted_norm (const Jones<double>&) const = 0;
    
    //! Given a coherency matrix, return the weighted conjugate matrix
    virtual 
    Jones<double>
    get_weighted_conjugate (const Jones<double>&)const = 0;

    virtual 
    Stokes< std::complex<double> > 
    get_weighted_components (const Jones<double>&) const = 0;

    //! Return the variance of each Stokes parameter
    virtual Stokes< std::complex<double> > get_variance () const = 0;

  };
}



#endif
