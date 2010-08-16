/***************************************************************************
 *
 *   Copyright (C) 2002-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileExtension.h"
#include "templates.h"

using namespace std;

void weighted_average (Pulsar::Profile* result, const Pulsar::Profile* other,
		       bool second_moment = false)
{
  if (result->get_nbin() != other->get_nbin())
    throw Error (InvalidRange, "Pulsar::Profile::weighted_average",
		 "result.nbin=%d != other.nbin=%d",
		 result->get_nbin(), other->get_nbin());

  float* amps1 = result->get_amps();
  const float* amps2 = other->get_amps();
    
  double weight1 = result->get_weight();
  double weight2 = other->get_weight();

  double weight = weight1 + weight2;
    
  double norm = 0.0;
  if (weight != 0)
    norm = 1.0 / weight;

  if (second_moment)
  {
    // square the various scale factors
    weight1 *= weight1;
    weight2 *= weight2;
    norm *= norm;
  }
  
  const unsigned nbin = result->get_nbin();
  for (unsigned ibin=0; ibin<nbin; ibin++)
    amps1[ibin] = norm * ( amps1[ibin]*weight1 + amps2[ibin]*weight2 );

  result->set_weight (weight);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::average
//
/*!
  A series of additions using this operator preserves the relationship:

  \f$ \bar{x} = \sum_{i=1}^N W(x_i) x_i / W(\bar{x}) \f$

  where \f$ W(x_i) \f$ is the weight assigned to \f$ x_i \f$ and

  \f$ W(\bar{x}) = \sum_{i=1}^N W(x_i) \f$
*/
void Pulsar::Profile::average (const Profile* profile) try
{
  /*
    It is important to integrate the extensions first, as weighted_average
    will call set_weight, which will prematurely set weight information in 
    extensions such as MoreProfiles
  */
  foreach (extension, &Profile::Extension::integrate, profile);
  weighted_average (this, profile);
}
catch (Error& error)
{
  throw error += "Pulsar::Profile::average";
}


