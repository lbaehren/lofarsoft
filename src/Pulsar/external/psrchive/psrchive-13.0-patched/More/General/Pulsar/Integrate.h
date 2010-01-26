//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Integrate.h,v $
   $Revision: 1.1 $
   $Date: 2007/06/21 17:32:17 $
   $Author: straten $ */

#ifndef __Pulsar_Integrate_h
#define __Pulsar_Integrate_h

#include "Pulsar/Transformation.h"

namespace Pulsar {

  //! Profile integration algorithms
  /*!
    Before integrating the profiles
    <UL>
    <LI> If the data have not already been dedispersed, then for each
    resulting frequency channel, a weighted centre frequency will be
    calculated and dispersion delays between this reference frequency
    and the individual channel centre frequencies will be removed.

    <LI> If the data have not already been corrected for Faraday
    rotation, then for each resulting frequency channel, a weighted
    centre frequency will be calculated and Faraday rotation between
    this reference frequency and the individual channel centre
    frequencies will be corrected.
    </UL>
  */

  template<class Container>
  class Integrate : public Transformation<Container> {

  public:

    //! Policy for selecting indeces of container elements to be integrated
    class RangePolicy;

    //! Set the range division policy
    virtual void set_range_policy (RangePolicy* rp) { range_policy = rp; }

  protected:

    //! The frequency channel range selection policy
    Reference::To<RangePolicy> range_policy;

    //! Policy for setting the number of divisions or integration factor
    class Divided;

    //! Policy for dividing into evenly spaced ranges
    class EvenlySpaced;

    //! Policy for dividing into evenly weighted ranges
    class EvenlyWeighted;

  };

  /*! This pure virtual base class defines the interface to the various
    range selection policies. */
  template<class Container>
  class Integrate<Container>::RangePolicy : public Reference::Able {

  public:

    //! Initialize ranges for the specified parameters
    virtual void initialize (Integrate*, Container*) = 0;
    
    //! Return the number of ranges
    virtual unsigned get_nrange () = 0;
    
    //! Return the first and last indeces of the specified range
    virtual void get_range (unsigned irange, 
			    unsigned& istart,
			    unsigned& istop) = 0;

  };

}

#endif
