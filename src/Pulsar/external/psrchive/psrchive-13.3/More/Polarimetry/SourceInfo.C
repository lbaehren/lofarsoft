/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SourceInfo.h"
#include "Pulsar/SystemCalibrator.h"

#include <string.h>

//! Constructor
Pulsar::SourceInfo::SourceInfo (const SourceEstimate* estimate)
{
  source = estimate;
  together = false;
  title = "Stokes Parameters";
}

void Pulsar::SourceInfo::set_together (bool flag)
{
  together = flag;
}

void Pulsar::SourceInfo::set_label (const std::string& lab)
{
  label = lab;
}

void Pulsar::SourceInfo::set_title (const std::string& t)
{
  title = t;
}

std::string Pulsar::SourceInfo::get_title () const
{
  return title;
}

//! Return the number of parameter classes
unsigned Pulsar::SourceInfo::get_nclass () const
{
  if (together)
    return 1;
  else
    return 4;
}

//! Return the name of the specified class
std::string Pulsar::SourceInfo::get_name (unsigned iclass) const
{
  if (!label.empty())
    return label;

  static char default_label [64] = "\\fiS'\\b\\d\\frk";
  static char* replace = strchr (default_label, 'k');

  if (!together)
    *replace = '0' + iclass;

  return default_label;
}


//! Return the number of parameters in the specified class
unsigned Pulsar::SourceInfo::get_nparam (unsigned iclass) const
{
  if (together)
    return 4;
  else
    return 1;
}

//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::SourceInfo::get_param (unsigned ichan, unsigned iclass,
			       unsigned iparam) const
{
  if (!source)
    return 0.0;

  if (!source->valid.size() || !source->source.size())
    return 0.0;

  if (!source->valid[ichan])
    return 0.0;

  unsigned index = 0;

  if (together)
    index = iparam;
  else
    index = iclass;

  return source->source[ichan]->get_Estimate(index);
}
