/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BackendFeedInfo.h"

using namespace std;

//! Constructor
Pulsar::BackendFeedInfo::BackendFeedInfo (const PolnCalibrator* calibrator) :
  SingleAxisCalibrator::Info (calibrator)
{
}

//! Return the number of parameter classes
unsigned Pulsar::BackendFeedInfo::get_nclass () const
{
  // two extra classes: ellipticities and orientations, or sums and differences
  return SingleAxisCalibrator::Info::get_nclass() + 2;
}
    
//! Return the name of the specified class
string Pulsar::BackendFeedInfo::get_name (unsigned iclass) const
{
  if (iclass < SingleAxisCalibrator::Info::get_nclass())
    return SingleAxisCalibrator::Info::get_name(iclass);
  
  iclass -= SingleAxisCalibrator::Info::get_nclass();

  return get_name_feed (iclass);
}

//! Return the number of parameters in the specified class
unsigned Pulsar::BackendFeedInfo::get_nparam (unsigned iclass) const
{
  if (iclass < SingleAxisCalibrator::Info::get_nclass())
    return SingleAxisCalibrator::Info::get_nparam(iclass);
  
  iclass -= SingleAxisCalibrator::Info::get_nclass();
  
  if (iclass < 2)
    return 2;

  return 0;
}

//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::BackendFeedInfo::get_param (unsigned ichan, unsigned iclass,
				   unsigned iparam) const
{
  if( ! calibrator->get_transformation_valid (ichan) )
    return 0;
 
  if (iclass < SingleAxisCalibrator::Info::get_nclass())
    return SingleAxisCalibrator::Info::get_param (ichan, iclass, iparam);
  
  iclass -= SingleAxisCalibrator::Info::get_nclass();

  return get_param_feed (ichan, iclass, iparam);
}

//! Return the colour index
int Pulsar::BackendFeedInfo::get_colour_index (unsigned iclass,
					      unsigned iparam) const
{
  if (iparam == 0)
    return 1;
  else
    return 2;
}
