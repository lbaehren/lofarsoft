//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolnProfile.h,v $
   $Revision: 1.45 $
   $Date: 2009/10/09 10:20:10 $
   $Author: straten $ */

#ifndef __Pulsar_PolnProfile_h
#define __Pulsar_PolnProfile_h

#include "Pulsar/Profile.h"

#include "Stokes.h"
#include "Jones.h"
#include "Matrix.h"
#include "Quaternion.h"

namespace Pulsar {

  class StokesCovariance;
  class BaselineEstimator;

  //! Polarimetric pulse profile transformations and derivations
  /*! This class uses references to Profile objects in order to manipulate
    externally stored polarimetric profiles. */
  class PolnProfile : public Container {
    
  public:

    //! When set, PolnProfile::transform will normalize the Profile::weight
    static bool normalize_weight_by_absolute_gain;

    //! Default constructor
    PolnProfile ();

    //! Construct with the specified number of phase bins
    PolnProfile (unsigned nbin);

    //! Construct from four externally-managed Profile objects
    PolnProfile (Signal::Basis basis, Signal::State state,
		 Profile* p0, Profile* p1, Profile* p2, Profile* p3);
    
    //! Destructor
    virtual ~PolnProfile();

    //! Clone operator
    virtual PolnProfile* clone () const;

    //! Set the number of bins
    void resize (unsigned nbin);

    //! Get the number of bins
    unsigned get_nbin () const;

    //! Get the specifed constant profile
    const Profile* get_Profile (unsigned ipol) const;

    //! Get the specified profile
    Profile* get_Profile (unsigned ipol);

    //! Returns a const pointer to the start of the array of amplitudes
    const float* get_amps (unsigned ipol) const;

    //! Returns a pointer to the start of the array of amplitudes
    float* get_amps (unsigned ipol);

    //! Set the amplitudes of the specified polarization
    void set_amps (unsigned ipol, float* amps);

    //! Set the baseline used by some methods
    void set_baseline (PhaseWeight*);

    //! Get the baseline (if not the one set above, then the default)
    PhaseWeight* get_baseline () const;

    //! Add that to this
    void sum (const PolnProfile* that);

    //! Subtract that from this
    void diff (const PolnProfile* that);

    //! Get the Basis of the poln profile
    Signal::Basis get_basis () const { return basis; }

    //! Get the State of the poln profile
    Signal::State get_state () const { return state; }

    //! Get the Stokes 4-vector for the specified bin
    Stokes<float> get_Stokes (unsigned ibin) const;

    //! Set the Stokes 4-vector for the specified bin
    void set_Stokes (unsigned ibin, const Stokes<float>&);

    //! Get the coherency matrix for the specified bin
    Jones<double> get_coherence (unsigned ibin) const;

    //! Set the coherency matrix for the specified bin
    void set_coherence (unsigned ibin, const Jones<double>&);

    //! Get the pseudo-Stokes 4-vector for the specified bin
    Quaternion<float,Hermitian> get_pseudoStokes (unsigned ibin) const;

    //! Set the pseudo-Stokes 4-vector for the specified bin
    void set_pseudoStokes (unsigned ibin, const Quaternion<float,Hermitian>&);

    //! Returns the sum of all amplitudes
    double sum (int bin_start=0, int bin_end=0) const;

    //! Returns the sum of all amplitudes squared
    double sumsq (int bin_start=0, int bin_end=0) const;

    //! multiplies each bin of each profile by scale
    void scale (double scale);

    //! Rotate the phase of each profile
    void rotate_phase (double turns);

    //! Perform the congruence transformation on each bin of the profile
    void transform (const Jones<double>& response);

    //! Perform the Mueller transformation on each bin of the profile
    void transform (const Matrix<4,4,double>& response);

    //! Convert to the specified state
    void convert_state (Signal::State state);

    //! Convert the Stokes parameters to the specified basis
    void convert_basis (Signal::Basis basis);

    //! Return the invariant interval
    void invint (Profile* invint, bool second = true) const;

    //! Return the invariant autoconvolution
    void invconv (Profile* invconv) const;

    //! Return the linear polarization profile
    void get_linear (Profile* linear) const;

    //! Return the absolute circular polarization profile
    void get_circular (Profile* circular) const;

    //! Return the total polarization profile
    void get_polarized (Profile* polarized) const;

    //! Return the orientation and its estimated error for each pulse phase
    void get_orientation (std::vector< Estimate<double> >&, float sigma) const;

    //! Return the ellipticity and its estimated error for each pulse phase
    void get_ellipticity (std::vector< Estimate<double> >&, float sigma) const;

    void get_linear (std::vector< std::complex< Estimate<double> > >& L,
		     float sigma) const;

    const StokesCovariance* get_covariance () const;

  protected:

    //! The basis in which the radiation is measured
    Signal::Basis basis;

    //! The state of the polarimetric profiles
    Signal::State state;

    //! References to the polarimetric profiles
    Reference::To<Profile> profile[4];

    //! The baseline set in set_baseline
    Reference::To<PhaseWeight> baseline;

    //! The baseline derived from the data
    mutable Reference::To<PhaseWeight> my_baseline;

    //! The phase-resolved four-dimensional Stokes covariance matrix
    Reference::To<StokesCovariance> covariance;

    //! Throw an exception if want_state != state or want_ibin >= nbin
    void check (const char* method, 
		Signal::State want_state,
		unsigned want_ibin) const;

    //! Efficiently forms the inplace sum and difference of two profiles
    void sum_difference (Profile* sum, Profile* difference);

    //! Set everthing to null values
    void init ();

    //! Does the work for get_polarized and get_linear
    void get_rss( Profile* rss, unsigned jpol, unsigned kpol,
		  BaselineEstimator* baseline_estimator = 0 ) const;

  };

}


#endif



