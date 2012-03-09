/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TimeAppend.h"
#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Config.h"
#include <Pulsar/DigitiserCounts.h>

#include "Error.h"

using Pulsar::Option;
using Pulsar::TimeAppend;
using std::cerr;
using std::endl;


Pulsar::Option<bool> dflt_chronological
(
 "TimeAppend::chronological", false,

 "Ensure chronological order [bool]",

 "If true, then TimeAppend::transform (used by psradd and Archive::append)\n"
 "will fail if sub-integrations are not appended in chronological order."
);

Pulsar::Option<double> dflt_max_overlap 
(
 "TimeAppend::max_overlap", 30.0,

 "Maximum time overlap [seconds]",

 "If TimeAppend::chronological is set, then this parameter determines\n"
 "the maximum time by which the start time of a sub-integration may precede\n"
 "the end time of the previous sub-integration."
);

Pulsar::TimeAppend::TimeAppend ()
{
  chronological = dflt_chronological;
  max_overlap = dflt_max_overlap;
}

bool Pulsar::TimeAppend::stop (Archive* into, const Archive* from)
{
  // Account for custom Integration ordering:
  
  IntegrationOrder* order_into = into->get<IntegrationOrder>();
  const IntegrationOrder* order_from = from->get<IntegrationOrder>();
  
  if (order_into && order_from)
  {
    if (order_into->get_IndexState() == order_from->get_IndexState())
    {
      order_into->append(into, from);
      return true;
    }
    else
      throw Error(InvalidState, "TimeAppend::stop",
		  "Archives have incompatible IntegrationOrder extensions");
  }
  else if (!order_into && order_from)
  {
    throw Error(InvalidState, "TimeAppend::stop",
		"Archives have incompatible IntegrationOrder extensions");
  }
  else if (order_into && !order_from)
  {
    Reference::To<Pulsar::Archive> copy = from->clone();
    copy->add_extension(order_into->clone());
    // This next line is a bit tricky... There are issues when the index
    // you are using is cyclical and you have more than one wrap across
    // which to define the subint resolution. In into case, the second
    // argument to the organise function needs more thought.
    // AWH 30/12/2003
    copy->get<IntegrationOrder>()->organise(copy, copy->get_nsubint());
    order_into->append(into, copy);
    return true;
  }

  return Append::stop (into, from);
}

void Pulsar::TimeAppend::check (Archive* into, const Archive* from)
{
  Append::check (into, from);

  if (chronological)
  {   
    if (Archive::verbose == 3) 
      cerr << "Pulsar::TimeAppend::check ensuring chronological order" << endl;
    
    MJD curlast  = into->end_time ();
    MJD newfirst = from->start_time();

    if (curlast > newfirst + max_overlap)
      throw Error (InvalidState, "TimeAppend::check",
		   "startime overlaps or precedes endtime");
  }
}

void Pulsar::TimeAppend::combine (Archive* into, Archive* from)
{
  if (Archive::verbose == 3)
    cerr << "Pulsar::TimeAppend::combine call IntegrationManager::manage"
	 << endl;
 
  into->expert()->manage (from);
  into->expert()->sort ();

  // Append the Digitiser counts
  DigitiserCounts *into_counts = into->get<DigitiserCounts>();
  DigitiserCounts *from_counts = from->get<DigitiserCounts>();

  try
  {
    if (into_counts && from_counts)
      into_counts->Append(*from_counts);
  }
  catch (Error& error)
  {
    throw error += "Pulsar::TimeAppend::combine";
  }
}
