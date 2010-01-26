//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/BackendFeed.h,v $
   $Revision: 1.3 $
   $Date: 2009/06/13 05:11:05 $
   $Author: straten $ */

#ifndef __CalibrationBackendFeed_H
#define __CalibrationBackendFeed_H

#include "MEAL/ProductRule.h"
#include "MEAL/ChainRule.h"
#include "MEAL/Complex2.h"
#include "MEAL/ScalarParameter.h"

#include "Estimate.h"
#include "Stokes.h"

namespace Calibration {

  class SingleAxis;
  class Feed;

  //! Physical parameterization of the instrumental response

  /*! Abstract base class of instrumental response parameterizations
   that separate the backend and frontend transformations. */

  class BackendFeed : public MEAL::ProductRule<MEAL::Complex2>
  {

  public:

    //! Default Constructor
    BackendFeed ();

    //! Copy Constructor
    BackendFeed (const BackendFeed& s);

    //! Assignment Operator
    const BackendFeed& operator = (const BackendFeed& s);

    //! Destructor
    ~BackendFeed ();

    //! Set cyclical limits on the model parameters
    virtual void set_cyclic (bool flag = true);

    //! Fix the orientation of the frontend
    virtual void set_constant_orientation (bool flag = true) = 0;
    
    //! Get the instrumental gain, \f$ G \f$, in calibrator flux units
    Estimate<double> get_gain () const;

    //! Get the differential gain, \f$ \gamma \f$, in hyperbolic radians
    Estimate<double> get_diff_gain () const;
    
    //! Get the differential phase, \f$ \phi \f$, in radians
    Estimate<double> get_diff_phase () const;

    //! Set the instrumental gain, \f$ G \f$, in calibrator flux units
    void set_gain (const Estimate<double>& gain);

    //! Set the differential gain, \f$ \gamma \f$, in hyperbolic radians
    void set_diff_gain (const Estimate<double>& gamma);
    
    //! Set the differential phase, \f$ \phi \f$, in radians
    void set_diff_phase (const Estimate<double>& phi);

    //! Provide access to the SingleAxis model
    const SingleAxis* get_backend () const;
    SingleAxis* get_backend ();

    //! Provide access to the Feed model
    virtual const MEAL::Complex2* get_frontend () const = 0;
    virtual MEAL::Complex2* get_frontend ();

    //! Set the instrumental gain variation
    void set_gain (MEAL::Scalar*);

    //! Set the differential gain variation
    void set_diff_gain (MEAL::Scalar*);
    
    //! Set the differential phase variation
    void set_diff_phase (MEAL::Scalar*);

    //! Get the instrumental gain variation
    const MEAL::Scalar* get_gain_variation () const;

    //! Get the differential gain variation
    const MEAL::Scalar* get_diff_gain_variation () const;
    
    //! Get the differential phase variation
    const MEAL::Scalar* get_diff_phase_variation () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! SingleAxis model: \f$G\f$, \f$\gamma\f$, and \f$\varphi\f$
    Reference::To<SingleAxis> backend;

    //! ChainRule used to model Backend parameter variations
    Reference::To< MEAL::ChainRule<MEAL::Complex2> > backend_chain;

    //! Scalar function used to model gain variation
    Reference::To<MEAL::Scalar> gain_variation;

    //! Scalar function used to model differential gain variation
    Reference::To<MEAL::Scalar> diff_gain_variation;

    //! Scalar function used to model differential phase variation
    Reference::To<MEAL::Scalar> diff_phase_variation;

  private:

    //! Initialize function used by constructors
    void init ();

  };

}

#endif

