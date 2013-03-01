//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 - 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/SourceEstimate.h,v $
   $Revision: 1.27 $
   $Date: 2009/10/02 03:38:29 $
   $Author: straten $ */

#ifndef __Pulsar_SourceEstimate_H
#define __Pulsar_SourceEstimate_H

#include "Pulsar/MeanCoherency.h"
#include "Pulsar/ReceptionModel.h"
#include "MEAL/Coherency.h"

#include "Types.h"
#include "MJD.h"

namespace Pulsar
{
  class SourceEstimate : public Reference::Able
  {
  public:

    //! Construct with the specified bin from Archive
    SourceEstimate (int ibin = -1);

    //! Create the source and add it to the measurement equation
    void create_source (Calibration::ReceptionModel* equation);

    //! Update source with the mean
    void update_source();

    //! Model of Stokes parameters as a function of frequency
    Reference::To<MEAL::Coherency> source;

    //! Best guess of Stokes parameters
    Calibration::MeanCoherency source_guess;

    //! Validity flag for this estimate
    bool valid;

    //! Phase bin from which pulsar polarization is derived
    int phase_bin;

    //! The index of the source in the model
    unsigned input_index;

    //! Count attempts to add data for this state
    unsigned add_data_attempts;

    //! Count failures to add data for this state
    unsigned add_data_failures;

    unsigned get_input_index() const { return input_index; }

    //! Return true if the last/top source estimate has data to constrain it
    bool is_constrained () const;

    //! Somewhat kludgey storage of multiple source states
    class Multiple
    {
    public:
      Reference::To<MEAL::Coherency> source;
      unsigned input_index;
    };

    std::vector<Multiple> multiples;
  };

  class SourceObservation
  {
  public:

    //! Source code
    Signal::Source source; 

    //! Epoch of the observation
    MJD epoch;

    //! Frequency channel
    unsigned ichan;

    //! The observed Stokes parameters
    Stokes< Estimate<double> > observation;

    //! The baseline
    Stokes< Estimate<double> > baseline;

  };

}

#endif
