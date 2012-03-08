//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_ProfilePCA_h
#define __Pulsar_ProfilePCA_h

#include "Pulsar/Algorithm.h"

namespace Pulsar
{
  class Profile;
  class Integration;

  //! Examines profile shape variation with principal components analysis
  class ProfilePCA : public Algorithm
  {   
  public:

    //! Default constructor
    ProfilePCA ();

    //! Destructor
    ~ProfilePCA();

    //! Set the number of harmonics to use for PCA.
    void set_nharm_pca(unsigned nharm);

    //! Set the number of harmonics store in cov matrix.
    void set_nharm_cov(unsigned nharm);

    //! Get the number of harmonics currently in use.
    unsigned get_nharm_pca();

    //! Get the number of harmonics currently stored.
    unsigned get_nharm_cov();

    //! Reset sums to zero
    void reset();

    //! Adds the given Profile to the analysis.
    void add_Profile (const Profile* p);

    //! Adds all Profiles in the given Integration to the analysis.
    void add_Integration (const Integration* i);

    //! Compute principal components.
    void compute();

    //! Return i,j cov matrix entry
    double get_cov_value(unsigned i, unsigned j);

    //! Return i-th largest PC value (eigenvalue).
    double get_pc_value(unsigned i);

    //! Return i-th largest PC vector (eigenvector) as Profile.
    Profile* get_pc_vector(int i, unsigned nbin);

    //! Decompose given Profile using first n_pc PCs
    std::vector<double> decompose(const Profile *p, unsigned n_pc);

    //! Unload the PCA results to a file.
    void unload(const std::string& filename);

    //! Load PCA results from a file.
    static ProfilePCA *load(const std::string& filename);

  protected:

    //! Number of harmonics to store
    unsigned nharm_cov;

    //! Number of harmonics to store
    unsigned nharm_pca;

    //! Current covariance matrix
    double *cov;

    //! Current mean profile
    double *mean;

    //! Total number of profiles added
    unsigned nprof;

    //! Sum of weights
    double wt_sum;

    //! Sum of squared weights
    double wt2_sum;

    //! Principal component values (eigenvalues)
    std::vector<double> pc_values;

    //! Principal component vectors (eigenvectors)
    double *pc_vectors;

  };

}


#endif



