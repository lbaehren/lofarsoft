//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_AdaptiveSmooth_h
#define __Pulsar_AdaptiveSmooth_h

#include "Pulsar/Transformation.h"

namespace Pulsar {

  class Profile;

  //! Profile smoothing algorithms that automatically pick
  //! smoothing size based on profile SNR.
  class AdaptiveSmooth : public Transformation<Profile> {

  public:

    //! Default constructor
    AdaptiveSmooth ();

    //! Destructor
    virtual ~AdaptiveSmooth ();

    //! Get the characteristic width of the smoothing window in turns
    float get_turns () const;

    //! Get maximum nonzero harmonic
    unsigned get_max_harm() const;

    //! Hold current filter to apply to more profiles
    void set_hold (bool);

    //! Get hold state
    bool get_hold() const;

    //! Smooth given Profile
    void transform(Profile *);

    //! Different types of filter
    enum Method { Wiener, Sinc_MSE, Sinc_IC, VonMises };

    //! Set filter method
    void set_method(Method m);

  protected:

    //! Current fourier-domain smoothing function
    std::vector<float> filter;

    //! Maximum nonzero harmonic
    int max_harm;

    //! Current hold state
    bool hold;

    //! Upper fraction of band to use for noise estimate
    float noise_band;

    //! Which filter method to use
    Method method;

  private:

    //! Compute filter from given Fourier-domain Profile
    void compute(const float *fprof, int nbin);

    //! Compute using general Wiener filter method
    void compute_wiener(const float *pspec, double sigma2, int nh);

    //! Compute using best sinc lowpass (ie, harmonic cutoff)
    void compute_sinc_mse(const float *pspec, double sigma2, int nh);

    //! Compute using best sinc lowpass using information criteria
    void compute_sinc_ic(const float *pspec, double sigma2, int nh);

    //! Compute using a generic filter func
    void compute_lpf(const float *pspec, double sigma2, int nh,
        float (*filter_func)(float,float));

    //! Compute using a Von Mises filter func
    void compute_vonmises_lpf(const float *pspec, double sigma2, int nh);

  };

}

#endif
