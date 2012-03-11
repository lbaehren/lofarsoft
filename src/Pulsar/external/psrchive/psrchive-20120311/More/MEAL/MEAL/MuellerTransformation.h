//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/MuellerTransformation.h,v $
   $Revision: 1.3 $
   $Date: 2008/06/15 16:12:34 $
   $Author: straten $ */

#ifndef __MEAL_MuellerTransformation_H
#define __MEAL_MuellerTransformation_H

#include "MEAL/Transformation.h"
#include "MEAL/Complex2.h"
#include "MEAL/Real4.h"

namespace MEAL 
{
  //! A Mueller transformation, \f$\rho^\prime=M_{ik}Tr(\sigma_k\rho)\sigma_i/2\f$.

  /*! This class models the transformation of the matrix, \f$ \rho \f$ by
    a Mueller matrix \f$ M \f$.  The partial derivatives of the
    output, \f$ \rho^\prime \f$, are computed using the product rule
    and the partial derivatives of \f$ \rho \f$ and \f$ M \f$. */
  class MuellerTransformation : public Transformation<Complex2>
  {

  public:

    //! Set the transformation, \f$ M \f$
    virtual void set_transformation (Real4* xform);

    //! Get the transformation, \f$ M \f$
    virtual Real4* get_transformation ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Returns \f$ \rho^\prime \f$ and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >*);

    //! The transformation, \f$ M \f$
    Project<Real4> transformation;

  };

}

#endif

