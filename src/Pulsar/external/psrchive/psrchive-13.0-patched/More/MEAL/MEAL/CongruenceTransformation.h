//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/CongruenceTransformation.h,v $
   $Revision: 1.9 $
   $Date: 2008/06/15 16:12:34 $
   $Author: straten $ */

#ifndef __MEAL_CongruenceTransformation_H
#define __MEAL_CongruenceTransformation_H

#include "MEAL/Transformation.h"
#include "MEAL/Complex2.h"

namespace MEAL {

  //! A congruence transformation, \f$\rho^\prime=J\rho J^\dagger\f$.
  /*! This class models the transformation of the matrix, \f$ \rho \f$ by
    a congruence transformation in \f$ J \f$.  The partial derivatives of the
    output, \f$ \rho^\prime \f$, are computed using the product rule
    and the partial derivatives of \f$ \rho \f$ and \f$ J \f$. */
  class CongruenceTransformation : public Transformation<Complex2>
  {

  public:

    //! Set the transformation, \f$ J \f$
    virtual void set_transformation (Complex2* xform);

    //! Get the transformation, \f$ J \f$
    virtual Complex2* get_transformation ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Returns \f$ \rho^\prime \f$ and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >*);

    //! The transformation, \f$ J \f$
    Project<Complex2> transformation;

  };

}

#endif

