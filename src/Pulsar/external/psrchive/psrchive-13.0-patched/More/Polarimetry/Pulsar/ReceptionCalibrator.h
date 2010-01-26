//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReceptionCalibrator.h,v $
   $Revision: 1.91 $
   $Date: 2009/06/24 05:11:32 $
   $Author: straten $ */

#ifndef __Pulsar_ReceptionCalibrator_H
#define __Pulsar_ReceptionCalibrator_H

#include "Pulsar/SystemCalibrator.h"
#include "Pulsar/ReflectStokes.h"

#include "Pulsar/StandardData.h"

#include "MEAL/VectorRule.h"

namespace Pulsar
{

  class Archive;
  class Integration;
  class ReferenceCalibrator;
  class FluxCalibrator;
  
  //! Models the variation of Stokes parameters with parallactic angle
  /*! The ReceptionCalibrator implements a technique of single dish
    polarimetric self-calibration.  This class requires a number of
    constraints, which are provided in through the add_observation,
    add_ReferenceCalibrator, and add_FluxCalibrator methods.
  */
  class ReceptionCalibrator : public SystemCalibrator
  {
   
  public:
    
    friend class ReceptionCalibratorPlotter;
    friend class SourceInfo;
    
    //! Construct with optional first pulsar archive
    ReceptionCalibrator (Calibrator::Type*);

    //! Descructor
    ~ReceptionCalibrator ();

    //! Allow the CAL Stokes V to vary (applies only if FluxCal observed)
    bool measure_cal_V;

    //! Allow the CAL Stokes Q to vary
    bool measure_cal_Q;

    //! Enforce that Stokes I > |p|, where p=(Q,U,V)
    bool physical_coherency;

    //! Allow the gain to vary independently from observation to observation
    bool independent_gains;

    //! Print an error message if Pointing parameters are not as expected
    bool check_pointing;

    //! Produce reports of reduced chisq for each state in each channel
    bool output_report;

    //! Reflections performed on the calibrator data immediately after loading
    ReflectStokes reflections;

    //! Add the specified pulse phase bin to the set of state constraints
    void add_state (unsigned pulse_phase_bin);
    
    //! Get the number of pulsar phase bin input polarization states
    unsigned get_nstate_pulsar () const;
    
    //! Retern a new plot information interface for the specified state
    Calibrator::Info* new_info_pulsar (unsigned istate) const;

    //! Set the observation that defines the baseline and on-pulse phase bins
    void set_standard_data (const Archive* data);

    //! Get the baseline mask
    const PhaseWeight* get_baseline () const;

    //! Get the on-pulse mask
    const PhaseWeight* get_onpulse () const;

    //! Add the calibrator observation to the set of constraints
    void add_calibrator (const Archive* data);
    
    //! Add the ReferenceCalibrator observation to the set of constraints
    void add_calibrator (const ReferenceCalibrator* polncal);
    
    //! Set the first guess to a previous solution
    void set_previous (const Archive* data);

    //! Normalize each Stokes vector by the mean on-pulse invariant 
    void set_normalize_by_invariant (bool set = true);

    //! Solve equation for each frequency
    void solve ();
    
  protected:
    
    //! Standard data interface
    Reference::To<Calibration::StandardData> standard_data;

    //! The unique transformation for each observation
    MEAL::VectorRule<MEAL::Complex2>* unique;

    //! The unique transformation "axis"
    MEAL::Axis< unsigned > unique_axis;

    //! Uncalibrated estimate of calibrator polarization
    SourceEstimate flux_calibrator_estimate;

    //! Uncalibrated estimate of pulsar polarization as a function of phase
    std::vector<SourceEstimate> pulsar;
    
    //! The epochs of all loaded calibrators
    std::vector<MJD> calibrator_epochs;

    // counts the number of times that add_data failed
    unsigned add_data_fail;

    // counts the number of times that add_data is called
    unsigned add_data_call;

    //! The number of channels that may be simultaneously solved
    unsigned nthread;

    //! Normalize the Stokes parameters by the invariant interval
    bool normalize_by_invariant;

    //! Set the initial guesses and update the reference epoch
    void initialize ();

    //! Initialize the StandardModel of the specified channel
    void init_model (unsigned ichan);

    //! Check that the model is ready to receive additional constraints
    void check_ready (const char* method, bool init = true);

    //! Initialization performed using the first observation added
    void initial_observation (const Archive* data);

    void valid_mask (const SourceEstimate& src);

    //! Add Integration data to the CoherencyMeasurement vector
    /*! Data is taken from the specified frequency channel and phase bin.
      \retval bins the vector to which a new measurement will be appended
      \param estimate contains the bin number and a running mean estimate
      \param ichan the frequency channel
      \param data the Integration data
      \param variance the variance to be assigned to the measurement. 
    */
    void add_data (std::vector<Calibration::CoherencyMeasurement>& bins,
		   SourceEstimate& estimate, unsigned ichan);

    //! Prepare the calibrator estimate
    void prepare_calibrator_estimate (Signal::Source);

    void submit_calibrator_data (Calibration::CoherencyMeasurementSet&,
				 const SourceObservation&);

    void integrate_calibrator_data (const Jones< Estimate<double> >&,
				    const SourceObservation&);

    //! Ensure that the pulsar observation can be added to the data set
    void match (const Archive*);

    void add_pulsar (Calibration::CoherencyMeasurementSet&,
		     const Integration*, unsigned ichan);

    //! Prepare the measurement equations for fitting
    void solve_prepare ();

    //! Prepare to export the solution
    void export_prepare () const;

    //! Ensure that selected phase bins are represented in on-pulse mask
    void ensure_consistent_onpulse ();
   };

}

#endif
