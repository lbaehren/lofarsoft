//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/EvenlyWeighted.h,v $
   $Revision: 1.2 $
   $Date: 2009/11/29 12:13:49 $
   $Author: straten $ */

#ifndef __Pulsar_EvenlyWeighted_h
#define __Pulsar_EvenlyWeighted_h

#include "Pulsar/Divided.h"
#include "debug.h"

namespace Pulsar {

  template<class C>
  class Integrate<C>::EvenlyWeighted : public Integrate<C>::Divided
  {

    //! Initialize ranges for the specified parameters
    void initialize (Integrate*, C*);

    //! Return the number of index ranges into which the container is divided
    unsigned get_nrange () { return stop_indeces.size(); }

    void get_range (unsigned irange, unsigned& start, unsigned& stop);

    //! Get the weight assigned to the element
    virtual double get_weight (const C*, unsigned ielement) = 0;

  protected:

    //! The input frequency channel index at the end of each range
    std::vector<unsigned> stop_indeces;

  };

}


template<class C>
void Pulsar::Integrate<C>::EvenlyWeighted::initialize (Integrate*, 
						       C* container)
{
  unsigned container_size = get_size( container );

  unsigned good_elements = 0;

  // count the number of good channels
  for (unsigned ielement=0; ielement < container_size; ielement++)
    if (get_weight(container,ielement) != 0)
      good_elements ++;

  DEBUG("EvenlyWeighted::initialize good_elements=" << good_elements);

  // divide them up
  unsigned output_elements = 0;
  unsigned spacing = 0;
  this->divide (good_elements, output_elements, spacing);

  stop_indeces.resize (output_elements);

  // and count off 'spacing' good channels per range
  unsigned ielement = 0;
  for (unsigned irange=0; irange < output_elements; irange++)
  {
    for (good_elements=0;
	 good_elements < spacing && ielement < container_size; ielement++)
      if (get_weight(container,ielement) != 0)
	good_elements ++;

    DEBUG("  stop[" << irange << "] = " << ielement);

    stop_indeces[irange] = ielement;

  }

}

template<class C>
void Pulsar::Integrate<C>::EvenlyWeighted::get_range (unsigned irange, 
						      unsigned& start, 
						      unsigned& stop)
{
  if (irange >= stop_indeces.size())
    throw Error (InvalidParam, 
		 "Pulsar::Integrate::Divide::EvenlyWeighted::get_range",
		 "irange=%u >= output_elements=%u", irange, stop_indeces.size());

  if (irange > 0)
    start = stop_indeces[irange-1];
  else
    start = 0;

  stop = stop_indeces[irange];
}



#endif
