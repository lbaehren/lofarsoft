//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/BrittonInfo.h,v $
   $Revision: 1.1 $
   $Date: 2009/03/03 11:48:14 $
   $Author: straten $ */

#ifndef __Pulsar_BrittonInfo_H
#define __Pulsar_BrittonInfo_H

#include "Pulsar/BackendFeedInfo.h"

namespace Pulsar {

  //! Communicates Britton (2000) parameters to plotting routines
  class BrittonInfo : public BackendFeedInfo {

  public:

    //! Constructor
    BrittonInfo (const PolnCalibrator* calibrator);
    
    //! Return the title
    std::string get_title () const;

    //! Return the name of the specified class
    std::string get_name_feed (unsigned iclass) const;
    
    //! Return the estimate of the specified parameter
    Estimate<float> get_param_feed (unsigned ichan, unsigned iclass,
				    unsigned iparam) const;
  };

}

#endif
