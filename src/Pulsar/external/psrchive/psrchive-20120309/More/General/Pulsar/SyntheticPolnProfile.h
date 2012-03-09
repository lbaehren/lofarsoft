//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/SyntheticPolnProfile.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __Pulsar_SyntheticPolnProfile_h
#define __Pulsar_SyntheticPolnProfile_h

#include "MEAL/Univariate.h"
#include "MEAL/Scalar.h"

namespace Pulsar {

  class PolnProfile;

  //! Simulated polarimetric pulse profile
  /*! Each function should accept the pulse phase in radians */
  class SyntheticPolnProfile : public Reference::Able {

  public:

    //! Default constructor
    SyntheticPolnProfile ();

    //! Destructor
    ~SyntheticPolnProfile ();

    //! Set the functional form of the total intensity
    void set_total_intensity (MEAL::Univariate<MEAL::Scalar>*);

    //! Set the functional form of the degree of polarization
    void set_degree (MEAL::Univariate<MEAL::Scalar>*);

    //! Set the constant degree of polarization
    void set_degree (float);

    //! Set the variance of the noise
    void set_noise_variance (float);

    //! Set the functional form of the position angle
    void set_position_angle (MEAL::Univariate<MEAL::Scalar>*);

    //! Set the constant position angle
    void set_position_angle (float);

    //! Set the functional form of the linear polarization
    void set_linear (MEAL::Univariate<MEAL::Scalar>*);

    //! Set the functional form of the circular polarization
    void set_circular (MEAL::Univariate<MEAL::Scalar>*);

    //! Get the polarimetric profile
    PolnProfile* get_PolnProfile (unsigned nbin) const;

  protected:

    //! The functional form of the total intensity
    Reference::To< MEAL::Univariate<MEAL::Scalar> > total_intensity;

    //! The variance of the noise
    float noise_variance;

    //! The functional form of the degree of polarization
    Reference::To< MEAL::Univariate<MEAL::Scalar> > degree;

    //! The constant degree of polarization
    float constant_degree;

    //! The functional form of the position angle
    Reference::To< MEAL::Univariate<MEAL::Scalar> > position_angle;

    //! The constant position angle
    float constant_position_angle;

    //! The functional form of the linear polarization
    Reference::To< MEAL::Univariate<MEAL::Scalar> > linear;

    //! The functional form of the circular polarization
    Reference::To< MEAL::Univariate<MEAL::Scalar> > circular;

  };

}

#endif
