//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/JonesCloude.h,v $
   $Revision: 1.1 $
   $Date: 2010/01/20 00:51:52 $
   $Author: straten $ */

#ifndef __MEAL_JonesCloude_H
#define __MEAL_JonesCloude_H

#include "MEAL/Complex4.h"
#include "MEAL/Complex2.h"
#include "MEAL/Composite.h"

namespace MEAL {

  //! Converts a Jones matrix into a Cloude target coherency matrix
  class JonesCloude : public MEAL::Complex4
  {

  public:

    //! Default constructor
    JonesCloude (Complex2* = 0);

    //! Destructor
    ~JonesCloude () { }

    //! Set the transformation, \f$ J \f$
    virtual void set_transformation (Complex2*);

    //! Get the transformation, \f$ J \f$
    virtual Complex2* get_transformation () { return transformation; }

    std::string get_name () const;

  protected:

    //! Calculate the Cloude matrix and its gradient
    virtual void calculate (Result& result, std::vector<Result>*);

    //! The transformation, \f$ J \f$
    Project<Complex2> transformation;

    //! Composite parameter policy
    Composite composite;

  };

}

#endif

