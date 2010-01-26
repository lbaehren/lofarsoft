/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/MeanCoherency.h"
#include "MEAL/Coherency.h"

void Calibration::MeanCoherency::update (MEAL::Complex2* model) const
{
  MEAL::Coherency* coherency = dynamic_cast<MEAL::Coherency*>(model);
  if (!coherency)
    throw Error (InvalidParam, "Calibration::MeanCoherency::update",
		 "Complex2 model is not a Coherency");

  coherency->set_stokes ( get_mean () );
}

void Calibration::MeanCoherency::integrate (const MEAL::Complex2* model)
{
  const MEAL::Coherency* coherency = 0;
  coherency = dynamic_cast<const MEAL::Coherency*>(model);
  if (!coherency)
    throw Error (InvalidParam, "Calibration::MeanCoherency::integrate",
		 "Complex2 model is not a Coherency");

  mean += coherency->get_stokes ();
}

void Calibration::MeanCoherency::integrate (const Stokes<Estimate<double> >& s)
{
  mean += s;
}

Stokes< Estimate<double> > Calibration::MeanCoherency::get_mean () const
{
  return Stokes< Estimate<double> > (mean);
}
