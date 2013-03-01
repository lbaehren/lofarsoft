//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_CalibratorInfo_h
#define __Pulsar_CalibratorInfo_h

#include "Pulsar/MultiFrequency.h"
#include "Pulsar/CalibratorParameter.h"

namespace Pulsar {

  //! Plots every class of model parameters from Calibrator::Info
  /*! 
    CalibratorInfo creates and manages a CalibratorParameter instance
    for each class of model parameters and divides the plot viewport
    into sub-panels as needed.
  */
  class CalibratorInfo : public MultiFrequency
  {
  public:

    //! Default constructor
    CalibratorInfo ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the CalibratorInfo class
    class Interface : public TextInterface::To<CalibratorInfo> {
    public:
      Interface (CalibratorInfo* = 0);
    };

    //! Ensure that frames are properly initialized
    void prepare (const Archive*);

    void set_between_panels (float x) { between_panels = x; }
    float get_between_panels () const { return between_panels; }

    void set_panels (const std::string& x) { panels = x; }
    std::string get_panels () const { return panels; }

    void set_calibrator_stokes (bool x) { calibrator_stokes = x; }
    bool get_calibrator_stokes () const { return calibrator_stokes; }

    void set_reduced_chisq (bool x) { reduced_chisq = x; }
    bool get_reduced_chisq () const { return reduced_chisq; }

  protected:

    //! Separate panels for each class of calibrator parameters
    std::vector< Reference::To<CalibratorParameter> > parameter;

    //! spacing between plot panels
    float between_panels;

    //! panels to be plotted
    std::string panels;

    //! plot the calibrator Stokes parameters
    bool calibrator_stokes;

    //! plot the goodness-of-fit statistic
    bool reduced_chisq;
  };

}

#endif
