//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Integration.h,v $
   $Revision: 1.100 $
   $Date: 2009/11/10 03:24:35 $
   $Author: straten $ */

/*
  
  Pulsar::Integration - base class for pulsar observations

*/

#ifndef __Pulsar_Integration_h
#define __Pulsar_Integration_h

#include "Pulsar/Pulsar.h"
#include "Pulsar/Container.h"

#include "MJD.h"
#include "Types.h"
#include "Estimate.h"
#include "Jones.h"

#include <vector>

template<typename T> class Stokes;

namespace Pulsar {

  class Profile;
  class PolnProfile;
  class PhaseWeight;
  class Archive;

  //! Array of Profiles integrated over the same time interval
  /*! This class stores a two-dimensional array of pulse Profile
    instances as a function of polarization and frequency. */
  class Integration : public Container {
    
  public:
    
    //! flag controls the amount output to stderr by Integration methods
    static bool verbose;

    //! Maximum centre frequency difference in Integration::mixable
    static double match_max_frequency_difference;

    //! Default constructor
    Integration ();

    //! Copy constructor
    Integration (const Integration& subint);

    //! Assignment operator
    Integration& operator = (const Integration& subint);
    
    //! Destructor
    virtual ~Integration ();

    //! Zero all the profiles, keeping all else the same
    void zero ();

    //! Get the MJD at the start of the integration (convenience interface)
    MJD get_start_time () const;

    //! Get the MJD at the end of the integration (convenience interface)
    MJD get_end_time () const;

    //! Get the Profile centre frequency attribute of the given channel
    double get_centre_frequency (unsigned ichan) const;
    //! Set the Profile centre frequency attributes of the given channel
    void set_centre_frequency (unsigned ichan, double frequency);

    //! Get the Profile weight attribute of the given channel
    float get_weight (unsigned ichan) const;
    //! Set the Profile weight attributes of the given channel
    void set_weight (unsigned ichan, float weight);

    //! Find the transitions between high and low states in total intensity
    void find_transitions (int& hi2lo, int& lo2hi, int& buffer) const;

    //! Find the bins in which the total intensity exceeds a threshold
    void find_peak_edges (int& rise, int& fall) const;

    //! Returns the centre phase of the region with maximum total intensity
    float find_max_phase () const;

    //! Returns the centre phase of the region with minimum total intensity
    float find_min_phase (float dc = 0.10) const;

    //! Return a new PhaseWeight instance with the baseline phase bins masked
    PhaseWeight* baseline () const;

    //! Return the statistics of every profile baseline
    void baseline_stats (std::vector< std::vector< Estimate<double> > >* mean,
			 std::vector< std::vector<double> >* variance = 0) const;

    //! Remove the baseline from all profiles
    void remove_baseline (const PhaseWeight* baseline = 0);

    //! Returns the mean hi/lo and variance of the mean hi/lo of every profile
    void cal_levels (std::vector< std::vector< Estimate<double> > >& hi,
                     std::vector< std::vector< Estimate<double> > >& lo) const;

    //! Computes the weighted centre frequency of an interval of sub-chans.
    double weighted_frequency (unsigned ch_start=0, unsigned ch_end=0) const;
        
    //! Set the weight of each profile to the given number
    void uniform_weight (float new_weight = 1.0);
    
    //! Remove dispersion delays with respect to centre frequency
    void dedisperse ();

    //! Remove Faraday rotation with respect to centre frequency
    void defaraday ();

    // //////////////////////////////////////////////////////////////////
    //
    // Copying and cloning
    //
    // //////////////////////////////////////////////////////////////////

    /** @name Copying and Cloning
     *
     *  The clone and total methods return pointers to new
     *  copy-constructed instances.
     */
    //@{

    //! Return a new copy of self
    virtual Integration* clone () const = 0;

    //! Disconnect from parent archive (useful after cloning a working copy)
    void orphan ();

    //! Return an orphaned pscrunched dedispersed fscrunched clone of self
    Integration* total () const;
    
    //@}


    // //////////////////////////////////////////////////////////////////
    //
    // Dimension Attributes
    //
    // //////////////////////////////////////////////////////////////////

    /** @name Dimension Attributes
     *
     *  These pure virtual methods return the dimension attributes stored 
     *  by derived classes.  Note that the dimensions of an Integration may be 
     *  changed only through the resize method.
     */
    //@{

    //! Get the number of chans
    /*! This attribute may be set only through Integration::resize */
    virtual unsigned get_nchan () const = 0;

    //! Get the number of polarization measurements
    /*! This attribute may be set only through Integration::resize */
    virtual unsigned get_npol () const = 0;

    //! Get the number of bins in each profile
    /*! This attribute may be set only through Integration::resize */
    virtual unsigned get_nbin () const = 0;
 
    //@}


    // //////////////////////////////////////////////////////////////////
    //
    // Integration Unique Attributes
    //
    // //////////////////////////////////////////////////////////////////

    /** @name Integration Unique Attributes
     *
     *  These pure virtual methods provide access to the attributes that 
     *  are unique to each Integration.
     */
    //@{

    //! Get the epoch of the rising edge of bin zero
    virtual MJD get_epoch () const = 0;
    //! Set the epoch of the rising edge of bin zero
    /*  When not dealing with single-pulse data, the epoch should
       refer to that of the rising edge of phase bin zero near the
       middle of the integration. */
    virtual void set_epoch (const MJD& mjd) = 0;

    //! Get the total time integrated (in seconds)
    virtual double get_duration() const =0;
    //! Set the total time integrated (in seconds)
    virtual void set_duration (double seconds) = 0;

    //! Get the folding or topocentric pulsar period (in seconds)
    virtual double get_folding_period() const = 0;
    //! Set the folding or topocentric pulsar period (in seconds)
    /*  The topocentric folding period of the pulsar should be equal
	to that at the epoch defined by get_epoch. */
    virtual void set_folding_period (double seconds) = 0;

    //@}


    // //////////////////////////////////////////////////////////////////
    //
    // Archive Shared Attributes
    //
    // //////////////////////////////////////////////////////////////////

    /** @name Archive Shared Attributes
     *
     *  By default, each Integration has the same observing parameters
     *  as all other Integrations in the Archive; therefore, it is not
     *  possible to set these attributes through the Integration base
     *  class interface. */
    //@{

    //! Get the centre frequency (in MHz)
    double get_centre_frequency() const;
    
    //! Get the bandwidth (in MHz)
    double get_bandwidth() const;

    //! Get the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    double get_dispersion_measure () const;
    
    //! Inter-channel dispersion delay has been removed
    bool get_dedispersed () const;

    //! Get the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    double get_rotation_measure () const;

    //! Data has been corrected for ISM faraday rotation
    bool get_faraday_corrected () const;

    //! Get the feed configuration of the receiver
    Signal::Basis get_basis () const;

    //! Get the polarimetric state of the profiles
    Signal::State get_state () const;

    //@}


    // //////////////////////////////////////////////////////////////////
    //
    // Access to data
    //
    // //////////////////////////////////////////////////////////////////

    /** @name Data Access
     *
     *  These methods provide access to the data contained in the Integration
     */
    //@{

    //! Returns a pointer to the Profile given by the specified indeces
    Profile* get_Profile (unsigned ipol, unsigned ichan);

    //! Returns a const pointer to the Profile given by the specified indeces
    const Profile* get_Profile (unsigned ipol, unsigned ichan) const;

    //! Returns a pointer to a new PolnProfile instance
    PolnProfile* new_PolnProfile (unsigned ichan);

    //! Returns a const pointer to a new PolnProfile instance
    const PolnProfile* new_PolnProfile (unsigned ichan) const;

    //! Return the Stokes 4-vector for the frequency channel and phase bin
    Stokes<float> get_Stokes (unsigned ichan, unsigned ibin) const;

    //! Returns a vector of Stokes parameters along the specified dimension
    void get_Stokes (std::vector< Stokes<float> >& S, unsigned iother,
		     Signal::Dimension abscissa = Signal::Phase ) const;

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
    
    //! Adds features or data to Integration instances
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

    //! Add an Extension to the Integration instance
    /*! The derived class must ensure that only one instance of the Extension
      type is stored.

      \return On successful addition, this method should return the
      pointer to the Extension, equal to the extension argument.  If the
      Extension is not supported, this method should return a null pointer.
    */
    virtual void add_extension (Extension* extension);

    //@}

    //! Provides access to certain protected and private methods
    class Expert;

    //! Provide access to the expert interface
    Expert* expert ();

  protected:

    // //////////////////////////////////////////////////////////////////
    //
    // Dimension Attributes
    //
    // //////////////////////////////////////////////////////////////////

    /** @name Dimension Attributes
     *
     *  The pure virtual methods are used by the Integration class to
     *  set the dimension attributes stored by the derived classes.
     *  Note that the dimensions of an Integration should be changed
     *  only through the resize method.
     */
    //@{

    //! Set the number of pulsar phase bins
    virtual void set_nbin (unsigned nbin) = 0;

    //! Set the number of frequency channels
    virtual void set_nchan (unsigned nchan) = 0;

    //! Set the number of polarization measurements
    virtual void set_npol (unsigned npol) = 0;

    //! Set the dimensions of the data container
    virtual void resize (unsigned npol=0, unsigned nchan=0, unsigned nbin=0);

    //! Inserts Profiles from Integration into this
    virtual void insert (Integration*);

    //! Remove a channel from this Integration
    virtual void remove (unsigned ichan);

    //! Remove an inclusive channel range from this Integration
    virtual void remove (unsigned ichan_first, unsigned ichan_last);

    //@}

    friend class Archive;
    friend class Extension;
    friend class Calibrator;
    friend class Expert;

    //! Combine from into this
    void combine (const Integration* from);

    //! Test if integration may be combined with this
    bool mixable (const Integration* integ, std::string& reason) const;

    //! Copy the profiles and attributes through set_ get_ methods
    virtual void copy (const Integration* subint, bool management = true);

    //! Swap the two specified profiles
    void swap_profiles (unsigned ipol, unsigned ichan,
			unsigned jpol, unsigned jchan);

    //! Rotate each profile by time (in seconds); updates the epoch attribute
    void rotate (double time);

    //! Rotate each profile by phase; does not update the epoch attribute
    void rotate_phase (double phase);

    //! Call Profile::fold on every profile
    void fold (unsigned nfold);

    //! Call Profile::bsrunch on every profile
    void bscrunch (unsigned nscrunch);
    
    //! Call Profile::bsrunch_to_nbin on every profile
    void bscrunch_to_nbin (unsigned nbin);

    //! Integrate profiles from neighbouring chans
    void fscrunch (unsigned nscrunch = 0);

    //! Integrate profiles from single polarizations into one total intensity
    void pscrunch ();
    
    //! Transform from Stokes (I,Q,U,V) to the polarimetric invariant interval
    void invint ();
    
    //! Perform the congruence transformation on each polarimetric profile
    void transform (const Jones<float>& response);

    //! Perform frequency response on each polarimetric profile
    void transform (const std::vector< Jones<float> >& response);

    //! Convert polarimetric data to the specified state
    void convert_state (Signal::State state);

    //! All new Profile instances are created through this method
    virtual Profile* new_Profile ();

    //! The Extensions added to this Integration instance
    mutable std::vector< Reference::To<Extension> > extension;

    //! Data: npol by nchan profiles
    std::vector< std::vector< Reference::To<Profile> > > profiles;

    //! The Archive that manages this integration
    Reference::To<const Archive, false> parent;

    class Meta;

    //! The orphaned Integration's attributes
    Reference::To<Meta> orphaned;

    //! Dedispersion worker function
    void dedisperse (unsigned ichan, unsigned kchan,
                     double dm, double f0);

    //! Defaraday worker function
    void defaraday (unsigned ichan, unsigned kchan,
                    double rm, double f0);

    //! Throw exception if ipol or ichan are out of range
    void range_check (unsigned ipol, unsigned ichan) const;

    //! Expert interface
    Reference::To<Expert> expert_interface;

  private:

    //! Converts between coherency products and Stokes parameters
    void poln_convert (Signal::State out_state);

    //! Leading edge of phase bin zero = polyco predicted phase zero
    bool zero_phase_aligned;

    //! Set the number of phase bins to that of profiles[0][0]
    void update_nbin ();
  };

  template<typename UnaryProfileMethod, typename Argument>
  void foreach (Integration* integration,
		UnaryProfileMethod method, const Argument& arg)
  {
    const unsigned npol = integration->get_npol();
    const unsigned nchan = integration->get_nchan();

    for (unsigned ipol=0; ipol<npol; ipol++)
      for (unsigned ichan=0; ichan<nchan; ichan++)
	(integration->get_Profile(ipol, ichan)->*(method)) (arg);
  }
}

#endif
