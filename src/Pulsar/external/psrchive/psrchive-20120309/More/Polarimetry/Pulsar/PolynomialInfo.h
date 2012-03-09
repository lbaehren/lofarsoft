//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolynomialInfo.h,v $
   $Revision: 1.3 $
   $Date: 2008/06/17 07:59:23 $
   $Author: straten $ */

#ifndef __Pulsar_PolynomialInfo_H
#define __Pulsar_PolynomialInfo_H

#include "Pulsar/VariationInfo.h"
#include "MEAL/Polynomial.h"

namespace Pulsar {

  //! Communicates Polynomial function parameters to plotting routines
  class PolynomialInfo : public VariationInfo {

  public:

    //! Construct with code of which function to plot
    PolynomialInfo (const SystemCalibrator* calibrator, Which which);
    
    //! Return the title
    std::string get_title () const;

    //! Return the number of parameter classes
    unsigned get_nclass () const;
    
    //! Return the name of the specified class
    std::string get_name (unsigned iclass) const;
    
    //! Return the number of parameters in the specified class
    unsigned get_nparam (unsigned iclass) const;
    
    //! Return the estimate of the specified parameter
    Estimate<float> get_param (unsigned ichan, unsigned iclass,
			       unsigned iparam) const;

    //! Return the Polynomial function for the specified channel
    const MEAL::Polynomial* get_Polynomial (unsigned ichan) const;

  protected:

    //! number of polynomial coefficients
    unsigned ncoef;

  };

}

#endif
