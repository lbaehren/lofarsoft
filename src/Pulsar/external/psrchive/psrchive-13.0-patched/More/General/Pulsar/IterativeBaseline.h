//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/IterativeBaseline.h,v $
   $Revision: 1.10 $
   $Date: 2007/11/02 04:25:11 $
   $Author: straten $ */

#ifndef __Pulsar_IterativeBaseline_h
#define __Pulsar_IterativeBaseline_h

#include "Pulsar/BaselineEstimator.h"

namespace Pulsar {

  class PhaseWeightSmooth;

  //! Finds a Profile baseline using an interative bounding method
  class IterativeBaseline : public BaselineEstimator {

  public:

    //! Default constructor
    IterativeBaseline ();

    //! Destructor
    ~IterativeBaseline ();

    //! Set the Profile from which baseline PhaseWeight will be computed
    void set_Profile (const Profile* profile);

    //! Set the BaselineEstimator used to find the initial baseline
    void set_initial_baseline (BaselineEstimator*);
    BaselineEstimator* get_initial_baseline () const;

    //! Set the threshold below which samples are included in the baseline
    virtual void set_threshold (float sigma);

    //! Set the maximum number of iterations
    void set_max_iterations (unsigned iterations);

  protected:

    //! Calculate the PhaseWeight
    void calculate (PhaseWeight* weight);

    //! Derived classes must define the bounds
    virtual void get_bounds (PhaseWeight*, float& lower, float& upper) = 0;

    virtual void postprocess (PhaseWeight* weight, const Profile* profile);
    
    //! The threshold below which samples are included in the baseline
    float threshold;

    //! The maximum number of iterations
    unsigned max_iterations;

    //! The BaselineEstimator used to find the initial baseline
    Reference::To<BaselineEstimator> initial_baseline;

    //! Provide access to derived classes
    bool get_initial_bounds () const { return initial_bounds; }

  private:

    //! Flag set true on the first call to get_bounds
    bool initial_bounds;

  };

}

#endif
