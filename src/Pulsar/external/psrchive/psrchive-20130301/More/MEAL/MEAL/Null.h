//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Null.h,v $
   $Revision: 1.1 $
   $Date: 2007/02/06 22:09:43 $
   $Author: straten $ */

#ifndef __MEAL_Null_H
#define __MEAL_Null_H

#include "MEAL/Function.h"
#include "MEAL/Parameters.h"

namespace MEAL {

  //! Null function holds an arbitrary number of parameters
  class Null : public Function {

  public:

    //! Default constructor
    Null (unsigned nparam = 0);

    //! Copy constructor
    Null (const Null&);

    //! Assignment operator
    Null& operator = (const Null&);

    //! Resize, setting fit=true for new parameters
    void resize (unsigned nparam);

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  private:

    //! Parameter policy
    Parameters parameters;

    //! Initialization
    void init ();

  };

}

#endif
