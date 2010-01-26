/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/MeanInstrument.h"
#include "Pulsar/Instrument.h"

void Calibration::MeanInstrument::update (MEAL::Complex2* model) const
{
  Instrument* instrument = dynamic_cast<Instrument*>(model);
  if (!instrument)
    throw Error (InvalidParam, "Calibration::MeanInstrument::update",
		 "Complex2 model is not a Instrument");

  single_axis.update (instrument->get_backend());
  feed.update (instrument->get_feed());
}

void Calibration::MeanInstrument::integrate (const MEAL::Complex2* model)
{
  const Instrument* instrument = dynamic_cast<const Instrument*>(model);
  if (!instrument)
    throw Error (InvalidParam, "Calibration::MeanInstrument::update",
		 "Complex2 model is not a Instrument");

  single_axis.integrate (instrument->get_backend());
  feed.integrate (instrument->get_feed());
}

double Calibration::MeanInstrument::chisq (const MEAL::Complex2* model) const
{
  const Instrument* instrument = dynamic_cast<const Instrument*>(model);
  if (!instrument)
    throw Error (InvalidParam, "Calibration::MeanInstrument::chisq",
		 "Complex2 model is not a Instrument");

  return 0.5 * ( single_axis.chisq (instrument->get_backend()) +
		 feed.chisq (instrument->get_feed()) );
}
