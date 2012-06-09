/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/StepsInfo.h"
#include "templates.h"

#include <assert.h>

using namespace std;

//! Constructor
Pulsar::StepsInfo::StepsInfo (const SystemCalibrator* cal, Which w)
  : VariationInfo (cal, w)
{
  if (Calibrator::verbose)
    cerr << "Pulsar::StepsInfo::StepsInfo" << endl;

  unsigned nchan = calibrator->get_nchan ();
  
  for (unsigned ichan = 0; ichan < nchan; ichan++)
    add_steps( get_Steps( ichan ) );

  if (Calibrator::verbose)
    cerr << "Pulsar::StepsInfo nstep=" << steps.size() << endl;

  if (which == Rotation)
    mean_radian.resize( nchan );
  else
    mean.resize( nchan );

  for (unsigned ichan = 0; ichan < nchan; ichan++)
    for (unsigned istep = 0; istep < steps.size(); istep++)
      if (which == Rotation)
	mean_radian[ichan] += get_step( ichan, istep );
      else
	mean[ichan] += get_step( ichan, istep );

}

const MEAL::Steps* Pulsar::StepsInfo::get_Steps (unsigned ichan) const
{
  return dynamic_cast<const MEAL::Steps*>( get_Scalar(ichan) );
}

// 10 minutes, expressed in days
static double max_diff = 10.0 / 60.0 / 24.0;

template<typename T>
void add (vector<T>& data, T value)
{
  unsigned index = 0;
  while (index < data.size())
    if ( fabs( data[index] - value ) < max_diff )
      break;
    else
      index ++;

  if (index == data.size())
  {
#ifdef _DEBUG
    cerr << "add new step[" << data.size() << "]=" << value << endl;
#endif
    data.push_back( value );
    std::sort (data.begin(), data.end());
  }
}

void Pulsar::StepsInfo::add_steps (const MEAL::Steps* function)
{
  if (!function)
    return;

#ifdef _DEBUG
  cerr << "Pulsar::StepsInfo::add_steps function nstep="
       << function->get_nstep() << endl;
#endif

  for (unsigned istep=0; istep < function->get_nstep(); istep++)
    add (steps, function->get_step(istep));
}

string Pulsar::StepsInfo::get_title () const
{
  return "Steps";
}

//! Return the number of parameter classes
unsigned Pulsar::StepsInfo::get_nclass () const
{
  // a separate panel for each step, plus one for the mean
  return steps.size() + 1;
}
    
//! Return the name of the specified class
string Pulsar::StepsInfo::get_name (unsigned iclass) const
{
  if (iclass == 0)
    return VariationInfo::get_name (iclass) + "(mean)";

  iclass --;

  return VariationInfo::get_name (iclass) 
    + "(" + tostring( steps[iclass], 4 )+ ")";
}

//! Return the number of parameters in the specified class
unsigned Pulsar::StepsInfo::get_nparam (unsigned iclass) const
{
  return 1;
}

//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::StepsInfo::get_param (unsigned ichan, unsigned iclass,
			      unsigned iparam) const
{
  Estimate<float> the_mean;
  if (which == Rotation)
    the_mean = mean_radian[ichan].get_Estimate();
  else
    the_mean = mean[ichan].get_Estimate();

  if (iclass == 0)
    return the_mean;
  else
  {
    Estimate<float> step = get_step (ichan, iclass - 1);
    if (step.var == 0)
      return 0;
    else
      return step - the_mean;
  }
}

Estimate<float>
Pulsar::StepsInfo::get_step (unsigned ichan, unsigned kstep) const
{
  assert (kstep < steps.size());

  const MEAL::Steps* function = get_Steps (ichan);
  if (!function)
    return 0;
 
  for (unsigned istep=0; istep < function->get_nstep(); istep++)
  {
    if ( fabs(function->get_step(istep) - steps[kstep]) < max_diff )
      return function->get_Estimate(istep);
  }

  return 0;
}

