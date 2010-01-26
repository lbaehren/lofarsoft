//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/InstrumentInfo.h,v $
   $Revision: 1.4 $
   $Date: 2009/03/03 11:48:29 $
   $Author: straten $ */

#ifndef __Pulsar_InstrumentInfo_H
#define __Pulsar_InstrumentInfo_H

#include "Pulsar/BackendFeedInfo.h"
#include "Pulsar/Instrument.h"

namespace Pulsar {

  //! Communicates Instrument parameters to plotting routines
  class InstrumentInfo : public BackendFeedInfo {

  public:

    //! Constructor
    InstrumentInfo (const PolnCalibrator* calibrator);
    
    //! Return the title
    std::string get_title () const;

    //! Return the name of the specified class
    std::string get_name_feed (unsigned iclass) const;
    
    //! Return the estimate of the specified parameter
    Estimate<float> get_param_feed (unsigned ichan, unsigned iclass,
				    unsigned iparam) const;

  protected:

    //! set true when the orientation of receptor 0 is set to zero
    bool fixed_orientation;

  };

}

#endif
