//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ComplexRVMFit.h,v $
   $Revision: 1.7 $
   $Date: 2009/06/15 00:58:56 $
   $Author: straten $ */

#ifndef __Pulsar_ComplexRVMFit_h
#define __Pulsar_ComplexRVMFit_h

#include "MEAL/Axis.h"
#include "Estimate.h"

#include <complex>

// forward declarations
namespace MEAL
{
  class ComplexRVM;
}

typedef std::pair<double,double> range;

namespace Pulsar
{
  class PolnProfile;

  //! Fit rotating vector model to Stokes Q and U profiles
  class ComplexRVMFit : public Reference::Able
  {
    
  public:

    //! Default constructo
    ComplexRVMFit ();

    //! Add a range of pulse phase containing an orthogonally polarized mode
    /*! Must be called before set_observation */
    void add_opm (range radians);

    //! Return true if pulse phase falls within an OPM range
    bool is_opm (double phase_in_radians) const;

    //! Set the data to which model will be fit
    void set_observation (const PolnProfile*);
    //! Get the data to which model will be fit
    const PolnProfile* get_observation () const;

    //! Set the threshold below which data are ignored
    void set_threshold (float sigma);
    //! Get the threshold below which data are ignored
    float get_threshold () const;
    
    //! Get the model to be fit to the data
    MEAL::ComplexRVM* get_model ();

    //! Fit data to the model
    void solve ();

    void set_chisq_map (bool flag) { chisq_map = flag; }

    void global_search (unsigned nstep);

    float get_chisq () const { return chisq; }
    unsigned get_nfree () const { return nfree; }

  protected:

    void check_parameters ();

    Reference::To<const PolnProfile> data;
    Reference::To<MEAL::ComplexRVM> model;
    float threshold;

    MEAL::Axis<unsigned> state;
    std::vector< MEAL::Axis<unsigned>::Value > data_x;
    std::vector< std::complex< Estimate<double> > > data_y;

    // orthogonally polarized mode phase ranges
    std::vector<range> opm;

    float peak_phase;
    float peak_pa;

    float chisq;
    unsigned nfree;
    bool chisq_map;
  };
}

#endif

