//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/BackendFeedInfo.h,v $
   $Revision: 1.1 $
   $Date: 2009/03/03 11:47:52 $
   $Author: straten $ */

#ifndef __Pulsar_BackendFeedInfo_H
#define __Pulsar_BackendFeedInfo_H

#include "Pulsar/SingleAxisCalibrator.h"

namespace Pulsar {

  //! Communicates BackendFeed parameters to plotting routines
  class BackendFeedInfo : public SingleAxisCalibrator::Info {

  public:

    //! Constructor
    BackendFeedInfo (const PolnCalibrator* calibrator);
    
    //! Return the number of parameter classes
    unsigned get_nclass () const;
    
    //! Return the name of the specified class
    std::string get_name (unsigned iclass) const;
    
    //! Return the name of the specified class
    virtual std::string get_name_feed (unsigned iclass) const = 0;
    
    //! Return the number of parameters in the specified class
    unsigned get_nparam (unsigned iclass) const;
    
    //! Return the estimate of the specified parameter
    Estimate<float> get_param (unsigned ichan, unsigned iclass,
			       unsigned iparam) const;

    //! Return the estimate of the specified parameter
    virtual Estimate<float> get_param_feed (unsigned ichan, unsigned iclass,
					    unsigned iparam) const = 0;

    int get_colour_index (unsigned iclass, unsigned iparam) const;

  };

}

#endif
