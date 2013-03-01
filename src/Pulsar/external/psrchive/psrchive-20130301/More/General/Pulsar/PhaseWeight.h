//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/PhaseWeight.h,v $
   $Revision: 1.16 $
   $Date: 2008/11/15 18:20:59 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseWeight_h
#define __Pulsar_PhaseWeight_h

#include "Reference.h"
#include "Estimate.h"

namespace Pulsar {

  class Profile;

  //! Stores a weight for each Profile phase bin
  /*! This class implements a useful set of methods required to create,
    store, and manipulate a weighted set of Profile phase bins. */
  class PhaseWeight : public Reference::Able {

  public:

    //! Default constructor
    PhaseWeight ();

    //! Construct with all phase bins selected
    PhaseWeight (const Profile*);

    PhaseWeight (unsigned nbin);
    PhaseWeight (unsigned nbin, float set_all);

    //! Copy constructor
    PhaseWeight (const PhaseWeight& weight);

    //! Copy vector of float constructor
    PhaseWeight (const std::vector<float>& weight);

    //! Destructor
    ~PhaseWeight ();
    
    //! Assignment operator
    const PhaseWeight& operator = (const PhaseWeight& weight);

    //! Forms the sum of this and weight
    const PhaseWeight& operator += (const PhaseWeight& weight);

    //! Forms the product of this and weight
    const PhaseWeight& operator *= (const PhaseWeight& weight);

    //! Array operator
    float& operator [] (unsigned i) { built = false; return weight[i]; }

    //! Array operator
    const float& operator [] (unsigned i) const { return weight[i]; }

    //! Resize the weights array
    void resize (unsigned nbin) { weight.resize(nbin); }

    //! Get the number of weights in the array
    unsigned get_nbin () const { return weight.size(); }

    //! Get the reference frequency
    double get_frequency () const { return reference_frequency; }

    //! Set the reference frequency
    void set_frequency (double f) { reference_frequency = f; }

    //! Set all weights to the specified value
    void set_all (float weight);
  
    //! Retrieve the sum of all weights
    double get_weight_sum () const;

    //! Retrieve the max of all weights
    double get_weight_max () const;

    //! Retrieve the weights
    void get_weights (std::vector<float>& weights) const;

    //! Weight the Profile amplitudes by the weights
    void weight_Profile (Profile* profile) const;

    //! Set the Profile from which statistics are calculated
    void set_Profile (const Profile* profile);

    //! Get the weighted total of the amplitudes
    double get_weighted_sum () const;

    //! Get the weighted average of the Profile amplitudes
    float get_avg () const;

    //! Get the weighted rms of the Profile amplitudes
    float get_rms () const;

    //! Get the minimum amplitude with non-zero weight
    float get_min () const;

    //! Get the minimum amplitude with non-zero weight
    float get_max () const;

    //! Get the median amplitude with non-zero weight
    float get_median () const;

    //! Get the median difference between the median and non-zero amplitudes
    float get_median_difference () const;

    //! Get the weighted mean of the Profile amplitudes
    Estimate<double> get_mean () const;

    //! Get the weighted variance of the Profile amplitudes
    Estimate<double> get_variance () const;

    //! Get the statistics of the weighted phase bins (deprecated interface)
    void stats (const Profile* profile,
		double* mean, double* variance=0, 
		double* varmean=0, double* varvar=0) const;

    //! Return the base address of the weights array
    float* get_weights () { return &(weight[0]); }

    //! Return the base address of the weights array
    const float* get_weights() const { return &(weight[0]); }

    //! Text interface to methods
    class Interface;

  protected:

    //! The reference frequency
    double reference_frequency;

    //! The weights
    std::vector<float> weight;

    //! The Profile to which the weights apply
    Reference::To<const Profile> profile;

    //! Flag set when the statistics have been calculated
    bool built;

    //! The weighted mean of the Profile
    Estimate<double> mean;

    //! The weighted variance of the Profile
    Estimate<double> variance;

    //! The median of all phase bins with non-zero weight
    mutable float median;
    mutable bool median_computed;

    //! The median difference of all phase bins with non-zero weight
    mutable float median_diff;
    mutable bool median_diff_computed;

    //! working space for median computation
    mutable std::vector<float> non_zero;

    //! fill the non_zero vector
    void fill_non_zero (const char* method) const;

    //! Compute the mean and variance attributes
    void build ();

    //! Initialize attributes to default values
    void init ();

    //! Check that the profile attribute is set
    void check_Profile (const char* method) const;

    //! Check that the size of the weights vector is equal to nbin
    void check_weight (unsigned nbin, const char* method) const;

  };

}


#endif // !defined __Pulsar_PhaseWeight_h
