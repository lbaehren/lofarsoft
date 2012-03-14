//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ConvertIsolated.h,v $
   $Revision: 1.3 $
   $Date: 2009/06/24 05:02:08 $
   $Author: straten $ */

#ifndef __Pulsar_ConvertIsolated_h
#define __Pulsar_ConvertIsolated_h

#include "Pulsar/PhaseWeightSmooth.h"

namespace Pulsar {

  class Profile;

  //! Converts elements without equal neighbours to another state
  class ConvertIsolated : public PhaseWeightSmooth {

  public:

    //! Default constructor
    ConvertIsolated ();

    //! Return new copy of self
    ConvertIsolated* clone () const;

    //! Set the size of the neighbourhood checked to the right or left
    void set_neighbourhood (float);
    //! Get the size of the neighbourhood checked to the right or left
    float get_neighbourhood () const;

    //! Set the fraction of equal neighbours required to the right or left
    void set_like_fraction (float);
    //! Get the fraction of equal neighbours required to the right or left
    float get_like_fraction () const;

    //! Set the value to test for isolation
    void set_test (float test) { test_value = test; }
    //! Get the value to test for isolation
    float get_test () const { return test_value; }

    //! Set the value to which isolated elements are converted
    void set_convert (float convert) { convert_value = convert; }
    //! Get the value to which isolated elements are converted
    float get_convert () const { return convert_value; }

  protected:

    float neighbourhood;
    float like_fraction;

    float test_value;
    float convert_value;

    void calculate (PhaseWeight*);

  };

}


#endif // !defined __Pulsar_BaselineWeight_h
