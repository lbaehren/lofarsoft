//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Profile.h,v $
   $Revision: 1.115 $
   $Date: 2009/11/29 12:13:49 $
   $Author: straten $ */

#ifndef __Pulsar_Profile_h
#define __Pulsar_Profile_h

#include "Pulsar/ProfileAmps.h"
#include "Pulsar/Config.h"

#include "Types.h"
#include "Functor.h"
#include "Estimate.h"

namespace Pulsar {

  class PhaseWeight;

  //! Any quantity recorded as a function of pulse phase
  /*! The Pulsar::Profile class implements a useful, yet minimal, set
    of functionality required to store, manipulate, and analyse pulsar
    profiles.  Note that:
    <UL>
    <LI> All methods that change the size of a Profile are protected.
    <LI> The Integration class is declared as a friend.
    </UL>
    This arrangement protects the size of each Profile object from
    becoming unsynchronized with the Integration in which it is
    contained.  The data in each Profile may still be manipulated through
    public methods.
  */
  class Profile : public ProfileAmps {

  public:

    //! The Integration class may call protected methods
    friend class Integration;

    //! flag controls the amount output to stderr by Profile methods
    static bool verbose;

    //! When true, Profile::rotate shifts bins in the phase domain
    static Option<bool> rotate_in_phase_domain;

    //! fractional phase window used to find rise and fall of running mean
    static Option<float> transition_duty_cycle;

    //! fractional phase window used in most functions
    static Option<float> default_duty_cycle;

    //! Default constructor
    Profile (unsigned nbin = 0);

    //! copy constructor
    Profile (const Profile& profile) { init(); operator = (profile); }

    //! copy constructor
    Profile (const Profile* profile) { init(); operator = (*profile); }

    //! returns a pointer to a new copy of self
    virtual Profile* clone () const;

    //! Resize the data area
    virtual void resize (unsigned nbin);

    //! sets profile equal to another profile
    const Profile& operator = (const Profile& profile);

    //! set this to the weighted average of this and that
    void average (const Profile* that);

    //! add profile to this
    void sum (const Profile* profile);

    //! subtract profile from this
    void diff (const Profile* profile);

    //! adds offset to each bin of the profile
    const Profile& operator += (float offset);

    //! subtracts offset from each bin of the profile
    const Profile& operator -= (float offset);

    //! multiplies each bin of the profile by scale
    const Profile& operator *= (float scale);

    //! multiplies each bin of the profile by scale
    void scale (double scale);

    //! offsets each bin of the profile by offset
    void offset (double offset);

    //! rotates the profile by phase (in turns)
    void rotate_phase (double phase);

    //! set all amplitudes to zero
    void zero ();
  
    //! calculate the signed sqrt of the absolute value of each bin 
    void square_root ();

    //! calculare the absolute value of each phase bin
    void absolute ();

    //! calculate the logarithm of each bin with value greater than threshold
    void logarithm (double base = 10.0, double threshold = 0.0);

    //! Returns the maximum amplitude
    float max  (int bin_start=0, int bin_end=0) const;
    //! Returns the minimum amplitude
    float min  (int bin_start=0, int bin_end=0) const;

    //! Returns the sum of all amplitudes
    double sum (int bin_start=0, int bin_end=0) const;
    //! Returns the sum of all amplitudes squared
    double sumsq (int bin_start=0, int bin_end=0) const;
    //! Returns the sum of the absolute value
    double sumfabs (int bin_start=0, int bin_end=0) const;

    //! Calculates the mean, variance, and variance of the mean
    void stats (double* mean, double* variance = 0, double* varmean = 0,
		int bin_start=0, int bin_end=0) const;

    //! Convenience interface to stats (start_bin, end_bin)
    void stats (float phase, 
		double* mean, double* variance = 0, double* varmean = 0,
		float duty_cycle = default_duty_cycle) const;

    //! Convenience interface to stats, returns only the mean
    double mean (float phase, float duty_cycle = default_duty_cycle) const;

    //! Returns the phase of the centre of the region with maximum mean
    float find_max_phase (float duty_cycle = default_duty_cycle) const;
    //! Returns the phase of the centre of the region with minimum mean
    float find_min_phase (float duty_cycle = default_duty_cycle) const;

    //! Find the bin numbers at which the mean power transits
    void find_transitions (int& highlow, int& lowhigh, int& width) const;

    //! Returns the bin number with the maximum amplitude
    int find_max_bin (int bin_start=0, int bin_end=0) const;
    //! Returns the bin number with the minimum amplitude
    int find_min_bin (int bin_start=0, int bin_end=0) const;

    typedef Functor< std::pair<int,int> (const Profile*) > Edges;
    //! The default implementation of the edge detection algorithm
    static Option<Edges> peak_edges_strategy;

    //! Find the bin numbers at which the cumulative power crosses thresholds
    void find_peak_edges (int& rise, int& fall) const;
    
    typedef Functor< PhaseWeight* (const Profile*) > Mask;
    //! The default implementation of the baseline finding algorithm
    static Option<Mask> baseline_strategy;

    //! Return a new PhaseWeight instance with the baseline phase bins masked
    PhaseWeight* baseline () const;

    //! The default implementation of the onpulse finding algorithm
    static Option<Mask> onpulse_strategy;

    typedef Functor< float (const Pulsar::Profile*) > Float;
    //! The default implementation of the snr method
    static Option<Float> snr_strategy;

    //! Returns the signal to noise ratio of the profile
    float snr () const;
    
    //! Rotates the profile to remove dispersion delay
    void dedisperse (double dm, double ref_freq, double pfold);
    
    //! Returns the shift (in turns) between profile and standard
    Estimate<double> shift (const Profile& std) const;

    /*! returns a vector representation of the array of amplitudes,
     with all zero-weighted points cleaned out */
    std::vector<float> get_weighted_amps () const;
    
    //! get the centre frequency (in MHz)
    double get_centre_frequency () const { return centrefreq; }
    //! set the centre frequency (in MHz)
    virtual void set_centre_frequency (double cfreq) { centrefreq = cfreq; }

    //! get the weight of the profile
    float get_weight () const { return weight; }
    //! set the weight of the profile
    virtual void set_weight (float);

    //! convolves this with the given profile (using fft method)
    void fft_convolve (const Profile* profile); 

    //! convolves this with the given profile in time domain
    void convolve (const Profile* profile); 

    //! cross-correlates this with the given profile in time domain
    void correlate (const Profile* profile); 

    //! some extensions may have to respond to pscrunch
    void pscrunch ();

    //! integrate neighbouring phase bins in profile
    void bscrunch (unsigned nscrunch);

    //! integrate neighbouring phase bins in profile
    void bscrunch_to_nbin (unsigned nbin);

    //! integrate neighbouring sections of the profile
    void fold (unsigned nfold);

    //! interface to model_profile used by Profile::shift
    void fftconv (const Profile& std, double& shift, float& eshift,
                  float& snrfft, float& esnrfft) const;

    // //////////////////////////////////////////////////////////////////
    //
    // Extension access
    //
    // //////////////////////////////////////////////////////////////////

    /** @name Extension Interface 
     *
     * Additional algorithms or information can be made available through
     * use of Extension classes. 
     */
    //@{
    
    //! Adds features or data to Profile instances
    class Extension;
    
    //! Return the number of extensions available
    virtual unsigned get_nextension () const;

    //! Return a pointer to the specified extension
    virtual const Extension* get_extension (unsigned iextension) const;

    //! Return a pointer to the specified extension
    virtual Extension* get_extension (unsigned iextension);

    //! Template method searches for an Extension of the specified type
    template<class ExtensionType>
    const ExtensionType* get () const;

    //! Template method searches for an Extension of the specified type
    template<class ExtensionType>
    ExtensionType* get ();

    //! Add an Extension to this instance
    /*! The derived class must ensure that only one instance of the Extension
      type is stored. */
    virtual void add_extension (Extension* extension);

  protected:

    friend class PolnProfile;
    friend class StandardSNR;

    //! does the work for convolve and correlate
    void convolve (const Profile* profile, int direction);

    //! initializes all values to null
    void init ();

    //! centre frequency of profile (in MHz)
    double centrefreq;

    //! weight of profile
    float weight;

    //! The Extensions added to this Profile instance
    mutable std::vector< Reference::To<Extension> > extension;
  };

  //! Default implementation of Profile::snr method
  double snr_phase (const Profile* profile);
}




#endif // !defined __Pulsar_Profile_h
