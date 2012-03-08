/***************************************************************************
 *  *
 *  *   Copyright (C) 2011 by Stefan Oslowski
 *  *   Licensed under the Academic Free License version 2.1
 *  *
 ****************************************************************************/

#ifndef __Pulsar__TimeDomainCovariance_h
#define __Pulsar__TimeDomainCovariance_h

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>

#include "Pulsar/ProfileCovariance.h"

namespace Pulsar
{
  class TimeDomainCovariance : public ProfileCovariance
  {
  public:
    //! Default constructor
    TimeDomainCovariance ();

    //! Destructor
    ~TimeDomainCovariance ();

    //! Choose which bins are to be used in the calculation of covariance matrix
    void choose_bins ( unsigned, unsigned );

    //! Get the name of class used
    std::string get_name () const;

    //! Adds the given Profile to the covariance matrix estimate
    void add_Profile (const Profile* );
    void add_Profile (const Profile*, float wt );

    //! Get the whole covariance matrix
    void get_covariance_matrix ( double * );

    //! Get the whole covariance matrix - gsl
    void get_covariance_matrix_gsl ( gsl_matrix * );

    //! Set the whole covariance matrix
    void set_covariance_matrix ( double * );
 
    //! Get the i, j covariance matrix element, where i is the row and j is the column
    double get_covariance_matrix_value ( unsigned, unsigned );

    //! This normalizes the covariance matrix after all profiles were added
    void finalize ();

    //! Return a text interface that can be used to configure this instance
    //virtual TextInterface::Parser* get_interface ();

    //! Get the count of profiles used to estimate the covariance matrix
    unsigned get_count ();

    //! Set the count of profiles used to estimate the covariance matrix
    void set_count ( unsigned );

    //! Get the rank of covariance matrix
    unsigned get_rank ();

    //! Set the rank of covariance matrix
    void set_rank ( unsigned );

    //! Reset the covariance matrix calculations
    void reset ();

  protected:
    //! first bin to be used
    unsigned first_bin;

    //! last bin to be used
    unsigned last_bin;

    //! Auxiliary variable for covariance matrix_calculation
    gsl_matrix *covariance_gsl;

    //! Auxiliary variable for storing the profile
    gsl_vector_view p_gsl;
  };
}

#endif
