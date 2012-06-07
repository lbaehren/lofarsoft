//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/RandomPolar.h,v $
   $Revision: 1.2 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __MEAL_RandomPolar_H
#define __MEAL_RandomPolar_H

namespace MEAL {

  class Polar;

  //! Generates a random Polar transformation within limits
  class RandomPolar {

  public:

    //! Default constructor
    RandomPolar();

    //! Get a random Polar transformation
    void get (Polar*);

    float min_gain;
    float max_gain;

    float max_abs_rotation;
    float max_abs_boost;

  };

}

#endif
