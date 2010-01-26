//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/EvenlySpaced.h,v $
   $Revision: 1.2 $
   $Date: 2007/10/16 23:21:22 $
   $Author: nopeer $ */

#ifndef __Pulsar_EvenlySpaced_h
#define __Pulsar_EvenlySpaced_h

#include "Pulsar/Divided.h"

namespace Pulsar {

  template<class C>
  class Integrate<C>::EvenlySpaced : public Integrate<C>::Divided 
  {

    //! Initialize ranges for the specified parameters
    void initialize (Integrate*, C*);

    //! Return the number of output frequency channel ranges
    unsigned get_nrange () { return output_ndivide; }

    //! Return the frequency channels indeces for the specified range
    void get_range (unsigned irange, unsigned& start, unsigned& stop);

  protected:

    //! The number of elements in the container
    unsigned input_size;
    //! The number of ranges into which the elements will be divided
    unsigned output_ndivide;
    //! The number of elements to be integrated
    unsigned output_nintegrate;

  };

}

template<class C>
void Pulsar::Integrate<C>::EvenlySpaced::initialize (Integrate*, C* container)
{
  input_size = get_size (container);
  this->divide (input_size, output_ndivide, output_nintegrate);
}

template<class C>
void Pulsar::Integrate<C>::EvenlySpaced::get_range (unsigned irange, 
						    unsigned& start, 
						    unsigned& stop)
{
  if (irange >= output_ndivide)
    throw Error (InvalidParam, 
		 "Pulsar::Integrate::EvenlySpaced::get_range",
		 "irange=%u >= output_ndivide=%u", irange, output_ndivide);

  start = irange * output_nintegrate;
  stop = start + output_nintegrate;
  
  if (stop > input_size)
    stop = input_size;

  if (C::verbose > 2)
    std::cerr << "Pulsar::Integrate::EvenlySpaced::get_range\n "
      " irange=" << irange << " output_nintegrate=" << output_nintegrate <<
      " input_size=" << input_size <<
      " start=" << start << " stop=" << stop << std::endl;
  
}



#endif
