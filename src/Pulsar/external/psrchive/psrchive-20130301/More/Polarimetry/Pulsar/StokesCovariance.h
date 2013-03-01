//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/StokesCovariance.h,v $
   $Revision: 1.2 $
   $Date: 2009/06/08 19:12:58 $
   $Author: straten $ */

#ifndef __Pulsar_StokesCovariance_h
#define __Pulsar_StokesCovariance_h

#include "Pulsar/Container.h"

#include "Stokes.h"
#include "Jones.h"
#include "Matrix.h"

namespace Pulsar {

  class FourthMoments;
  class Profile;

  //! Phase-resolved four-dimensional covariance matrix of Stokes parameters
  class StokesCovariance : public Container {
    
  public:

    //! The number of unique fourth moments in a Stokes covariance matrix
    static const unsigned nmoment;

    //! Default constructor
    StokesCovariance ();

    //! Construct with the specified number of phase bins
    StokesCovariance (FourthMoments*);
    
    //! Destructor
    ~StokesCovariance();

    //! Clone operator
    virtual StokesCovariance* clone () const;

    //! Set the number of bins
    void resize (unsigned nbin);

    //! Get the number of bins
    unsigned get_nbin () const;

    //! Get the specifed constant profile
    const Profile* get_Profile (unsigned i) const;

    //! Get the specified profile
    Profile* get_Profile (unsigned i);

    //! Get the Stokes covariance for the specified bin
    Matrix<4,4,double> get_covariance (unsigned ibin) const;

    //! Set the Stokes covariance for the specified bin
    void set_covariance (unsigned ibin, const Matrix<4,4,double>&);

    //! multiplies each bin of each profile by scale
    void scale (double scale);

    //! Rotate the phase of each profile
    void rotate_phase (double turns);

    //! Perform the congruence transformation on each bin of the profile
    void transform (const Jones<double>& response);

    //! Perform the Mueller transformation on each bin of the profile
    void transform (const Matrix<4,4,double>& response);

  protected:

    //! References to the covariance profiles
    Reference::To<FourthMoments> covariance;

  };

}


#endif



