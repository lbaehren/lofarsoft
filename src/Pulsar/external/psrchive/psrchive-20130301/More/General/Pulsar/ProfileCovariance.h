/***************************************************************************
 *  *
 *  *   Copyright (C) 2011 by Stefan Oslowski
 *  *   Licensed under the Academic Free License version 2.1
 *  *
 ****************************************************************************/

#ifndef __Pulsar_ProfileCovariance_h
#define __Pulsar_ProfileCovariance_h

#include "Pulsar/Profile.h"
#include "Pulsar/Algorithm.h"
#include "Warning.h"
#include "string.h" // for memcpy

namespace Pulsar
{
  /*! This pure virtual base class of ProfileCovariance defines the
   * interface by which various routines for calculating the covariance
   * matrix of the given profile can be accessed */
  class ProfileCovariance : public Algorithm
  {
  public:

    //! Return a copy constructed from instance of self
    // virtual ProfileCovariance* clone () const = 0;

    //! Get the name of class used
    virtual std::string get_name () const = 0;

    //! Adds the given Profile to the covariance matrix estimate
    virtual void add_Profile (const Profile* ) = 0;

    //! Get the whole covariance matrix
    virtual void get_covariance_matrix ( double * ) = 0;

    //! Set the whole covariance matrix
    virtual void set_covariance_matrix ( double * ) = 0;

    //! Get the i, j covariance matrix element, where i is the row and j is the column
    virtual double get_covariance_matrix_value ( unsigned, unsigned ) = 0;

    //! Normalize the covariance matrix after all the profiles were added
    virtual void finalize () = 0;

    //! Return a text interface that can be used to configure this instance
    //virtual TextInterface::Parser* get_interface () = 0;

    //! Get the count of profiles used to estimate the covariance matrix
    virtual unsigned get_count () = 0;

    //! Set the count of profiles used to estimate the covariance matrix
    virtual void set_count ( unsigned ) = 0;

    //! Get the rank of covariance matrix
    virtual unsigned get_rank () = 0;

    //! Set the rank of covariance matrix
    virtual void set_rank ( unsigned ) = 0;

    //! Reset the covariance matrix calculations
    virtual void reset () = 0;

  protected:

    //! Has the covariance matrix
    bool finalized;

    //! The calculated covariance matrix
    double *covariance_matrix;

    //! Rank of the covariance matrix
    unsigned rank;

    //! The count of profiles used to estimate the covariance matrix
    unsigned count;

    //! Sum of weights
    double wt_sum;

    //! Sum of squard weights
    double wt_sum2;

    //! Promote an array of floats to doubles. Necessary for using the gsl's eigensystems
    struct CastToDouble
    {
      double operator () (float value) const { return static_cast<double>(value);}
    };

    //! double version of profile
    double *p_damps;
  };
}

#endif
