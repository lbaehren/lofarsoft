/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/MeanFeed.h"
#include "Pulsar/Feed.h"
#include "EstimateStats.h"

void Calibration::MeanFeed::update (MEAL::Complex2* model) const
{
  Feed* feed = dynamic_cast<Feed*>(model);
  if (!feed)
    throw Error (InvalidParam, "Calibration::MeanFeed::update",
		 "Complex2 model is not a Feed");

  update (feed);
}

void Calibration::MeanFeed::update (Feed* feed) const
{
  for (unsigned i=0; i<2; i++) {
    feed->set_orientation (i, 0.5 * mean_orientation[i].get_Estimate());
    feed->set_ellipticity (i, 0.5 * mean_ellipticity[i].get_Estimate());
  }
}

void Calibration::MeanFeed::integrate (const MEAL::Complex2* model)
{
  const Feed* feed = dynamic_cast<const Feed*>(model);
  if (!feed)
    throw Error (InvalidParam, "Calibration::MeanFeed::update",
		 "Complex2 model is not a Feed");

  integrate (feed);
}

void Calibration::MeanFeed::integrate (const Feed* feed)
{
  for (unsigned i=0; i<2; i++) {
    mean_orientation[i] += 2.0 * feed->get_orientation (i);
    mean_ellipticity[i] += 2.0 * feed->get_ellipticity (i);
  }
}

double Calibration::MeanFeed::chisq (const MEAL::Complex2* model) const
{
  const Feed* feed = dynamic_cast<const Feed*>(model);
  if (!feed)
    throw Error (InvalidParam, "Calibration::MeanFeed::chisq",
		 "Complex2 model is not a Feed");
  
  return chisq (feed);
}

double Calibration::MeanFeed::chisq (const Feed* feed) const
{
  double retval = 0.0;

  for (unsigned i=0; i<2; i++) {
    MeanRadian<double> otemp (2.0 * feed->get_orientation (i));
    MeanRadian<double> etemp (2.0 * feed->get_ellipticity (i));

    retval += ::chisq (mean_orientation[i], otemp);
    retval += ::chisq (mean_ellipticity[i], etemp);
  }

  return 0.25 * retval;
}
