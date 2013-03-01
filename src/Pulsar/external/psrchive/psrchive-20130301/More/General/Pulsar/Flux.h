//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_Flux_h
#define __Pulsar_Flux_h

#include "Pulsar/Algorithm.h"
#include "Estimate.h"

namespace Pulsar
{
  class Profile;

  //! Computes average flux of a Profile.
  /*! Virtual base class, different algorithms implemented by 
   *  descendents.  NOTE this class is very similar to the algorithms
   *  currently used by the various S/N estimators, it may be possible
   *  to merge the code.
   */
  class Flux : public Algorithm
  {   
  public:

    //! Default constructor
    Flux() {};

    //! Destructor
    ~Flux() {};

    //! Return flux
    Estimate<double> operator() (const Profile *p) { return get_flux(p); }

    //! Return flux w/ uncertainty
    //virtual Estimate<double> get_flux(const Profile *p) { return Estimate<double>(0,0); }
    virtual Estimate<double> get_flux(const Profile *p)=0;

    //! Return name of flux algorithm
    virtual std::string get_method() const { return ""; }

  protected:

  };

}


#endif



