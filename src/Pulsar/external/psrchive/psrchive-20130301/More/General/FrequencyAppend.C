/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FrequencyAppend.h"
#include "Pulsar/WeightInterpreter.h"
#include "Pulsar/Dispersion.h"
#include "Pulsar/FaradayRotation.h"

#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/ArchiveMatch.h"

#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Profile.h"
#include <Pulsar/DigitiserCounts.h>

#include "Error.h"

using namespace Pulsar;
using namespace std;

void ensure_FrequencyAppend_linkage()
{
}

Pulsar::Option<FrequencyAppend::Weight> FrequencyAppend::weight_strategy
(
 new Pulsar::WeightInterpreter (FrequencyAppend::weight_strategy),

 "FrequencyAppend::weight", "radiometer",

 "Weighting policy",

 "The name of the algorithm used to weight each profile.\n"
 "Possible values: radiometer, time, snr, none"
);

const Pulsar::Archive::Match*
Pulsar::FrequencyAppend::get_mixable_policy (const Archive* a)
{
  Reference::To<Archive::Match> mixable = a->get_mixable()->clone();
  mixable->set_check_centre_frequency (false);
  mixable->set_check_bandwidth (false);
  mixable->set_check_nchan (false);

  return mixable.release();
}

bool Pulsar::FrequencyAppend::stop (Archive* into, const Archive* from)
{
  IntegrationOrder* order_into = into->get<IntegrationOrder>();
  const IntegrationOrder* order_from = from->get<IntegrationOrder>();
  
  if (order_into || order_from)
    throw Error (InvalidState, "Pulsar::FrequencyAppend::stop",
		  "IntegrationOrder extension support not yet implemented");

  return Append::stop (into, from);
}

void Pulsar::FrequencyAppend::check (Archive* into, const Archive* from)
{
  Append::check (into, from);

  if (into->get_nsubint() != from->get_nsubint()) 
    throw Error (InvalidState, "Pulsar::FrequencyAppend::check",
		 "Archives have different numbers of sub-integrations;"
		 " into=%u from=%u", into->get_nsubint(), from->get_nsubint());
}

void Pulsar::FrequencyAppend::combine (Archive* into, Archive* from)
{
  unsigned nsubint = into->get_nsubint();

  check_phase = true;

  if (from->get_dedispersed() && from->has_model())
  {
    double into_frequency = into->get_model()->get_observing_frequency ();
    double from_frequency = from->get_model()->get_observing_frequency ();
    double centre_frequency = from->get_centre_frequency ();

    if (from_frequency == centre_frequency)
    {
      if (Archive::verbose > 2)
        cerr << "Pulsar::FrequencyAppend::combine "
          "dedispersed and phase at infinite frequency" << endl;

      equal_models = true;
      check_phase = false;
      insert_model = false;
    }
    else if (from_frequency == into_frequency)
    {
      if (Archive::verbose > 2)
        cerr << "Pulsar::FrequencyAppend::combine "
          "dedispersed and matching predictor frequencies" << endl;

      insert_model = false;
    }
    else
      throw Error (InvalidState, "Pulsar::FrequencyAppend::combine",
		   "predictor frequency mismatch: from=%lf != into=%lf \n\t"
	           "from centre frequency=%lf",
		   from_frequency, into_frequency, centre_frequency);
  }
  else
    check_phase = true;

  for (unsigned isub=0; isub < nsubint; isub++)
    combine (into->get_Integration(isub), from->get_Integration(isub));

  into->expert()->set_nchan( into->get_nchan() + from->get_nchan() );

  double total_bandwidth = into->get_bandwidth() + from->get_bandwidth();

  double weighted_centre_frequency =
    ( into->get_bandwidth() * into->get_centre_frequency() +
      from->get_bandwidth() * from->get_centre_frequency() ) / total_bandwidth;

  into->set_bandwidth( total_bandwidth );
  into->set_centre_frequency( weighted_centre_frequency );

  // Sum the digitiser counts
  DigitiserCounts *into_counts = into->get<DigitiserCounts>();
  DigitiserCounts *from_counts = from->get<DigitiserCounts>();
  if( into_counts != NULL && from_counts != NULL )
    into_counts->Accumulate( *from_counts );

  if (Archive::verbose > 2)
    cerr << "Pulsar::FrequencyAppend::combine result"
      " nchan=" << into->get_nchan() << 
      " freq=" << into->get_centre_frequency() << 
      " bw=" << into->get_bandwidth() << endl;
}

//! Initialize an archive for appending
void Pulsar::FrequencyAppend::init (Archive* into)
{
  if (!weight_strategy.get_value())
    return;

  const unsigned nsubint = into->get_nsubint();

  for (unsigned isub=0; isub < nsubint; isub++)
    weight_strategy.get_value() ( into->get_Integration (isub) );
}

void Pulsar::FrequencyAppend::combine (Integration* into, Integration* from)
try 
{
  if (weight_strategy.get_value())
    weight_strategy.get_value() ( from );

  into->expert()->insert(from);

  if (check_phase && into->get_dedispersed())
  {
    Dispersion xform;
    xform.match (into, from);
  }

  if (into->get_faraday_corrected())
  {
    FaradayRotation xform;
    xform.match (into, from);
  }
}
catch (Error& error)
{
  throw error += "Pulsar::FrequencyAppend::combine";
}

