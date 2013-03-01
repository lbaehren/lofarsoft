/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MeanPolar.h"
#include "MEAL/Polar.h"

void Calibration::MeanPolar::update (MEAL::Complex2* model) const
{
  MEAL::Polar* polar = dynamic_cast<MEAL::Polar*>(model);
  if (!polar)
    throw Error (InvalidParam, "Calibration::MeanPolar::update",
		 "Complex2 model is not a Polar");

  // avoid setting the gain to zero if nothing has been added to the mean
  Estimate<double> gain = mean_gain.get_Estimate();
  if (gain.get_value() == 0)
    polar->set_gain (1.0);
  else
    polar->set_gain (gain);

  for (unsigned i=0; i<3; i++)
  {
    polar->set_boost (i, mean_boost[i].get_Estimate());
    polar->set_rotation (i, 0.5 * mean_rotation[i].get_Estimate());
  }
}

void Calibration::MeanPolar::integrate (const MEAL::Complex2* model)
{
  const MEAL::Polar* polar = dynamic_cast<const MEAL::Polar*>(model);
  if (!polar)
    throw Error (InvalidParam, "Calibration::MeanPolar::integrate",
		 "Complex2 polar is not a Polar");

  mean_gain += polar->get_gain ();

  for (unsigned i=0; i<3; i++)
  {
    mean_boost[i] += polar->get_boost (i);
    mean_rotation[i] += 2.0 * polar->get_rotation (i);
  }
}

