//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/SolverInfo.h,v $
   $Revision: 1.3 $
   $Date: 2008/06/17 08:05:33 $
   $Author: straten $ */

#ifndef __Pulsar_SolverInfo_H
#define __Pulsar_SolverInfo_H

#include "Pulsar/PolnCalibrator.h"

namespace Pulsar {

  class CalibratorStokes;

  //! Communicates PolnCalibrator solver parameters to plotting routines
  class SolverInfo : public Calibrator::Info 
  {

  public:

    //! Constructor
    SolverInfo (const PolnCalibrator* calibrator);
    
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

  protected:

    //! The CalibratorStokes to be plotted
    Reference::To<const PolnCalibrator> poln_calibrator;

  };

}

#endif
