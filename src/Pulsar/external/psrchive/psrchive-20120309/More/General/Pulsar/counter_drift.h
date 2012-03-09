//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Haydon Knight
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_counter_drift_h
#define __Pulsar_counter_drift_h

#include "MJD.h"
#include <vector>

namespace Pulsar {

  class Archive;

  //! Rotates subints to counter a drifting pulse due to incorrect P/Pdot
  void counter_drift (Archive* archive,
		      double trial_p, double trial_pdot=0.0, MJD reference_time = MJD::zero);

  //! Same as above, but inputs are in frequency domain
  void counter_frequency_drift (Archive* archive,
				double trial_nu0, double trial_nu1,
				MJD reference_time = MJD::zero);

  //! Same as above, but takes multiple frequency derivatives
  void multi_counter_fdrift (Archive* archive,
			     std::vector<double> nus,
			     MJD reference_time = MJD::zero);

}

#endif
