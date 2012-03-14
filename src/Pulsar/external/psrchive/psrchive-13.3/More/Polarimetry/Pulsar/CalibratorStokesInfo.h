//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/CalibratorStokesInfo.h,v $
   $Revision: 1.8 $
   $Date: 2008/06/17 08:02:52 $
   $Author: straten $ */

#ifndef __Pulsar_CalibratorStokesInfo_H
#define __Pulsar_CalibratorStokesInfo_H

#include "Pulsar/Calibrator.h"

namespace Pulsar {

  class CalibratorStokes;

  //! Communicates CalibratorStokes parameters to plotting routines
  class CalibratorStokesInfo : public Calibrator::Info {

  public:

    //! Constructor
    CalibratorStokesInfo (const CalibratorStokes* calibrator);
    
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
    Reference::To<const CalibratorStokes> calibrator_stokes;

    //! Plot all of the Stokes parameters in one panel
    bool together;

  };

}

#endif
