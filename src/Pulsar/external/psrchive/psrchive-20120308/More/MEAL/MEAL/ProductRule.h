//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ProductRule.h,v $
   $Revision: 1.7 $
   $Date: 2008/06/15 17:09:09 $
   $Author: straten $ */

#ifndef __MEAL_ProductRule_H
#define __MEAL_ProductRule_H

#include "MEAL/GroupRule.h"

namespace MEAL {

  //! Represents a product of models
  template<class T>
  class ProductRule : public GroupRule<T>
  {

  public:

    typedef typename T::Result Result;

    //! Add the model to the product
    void operator *= (T* model) { add_model (model); }

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const
    { return "ProductRule<" + std::string(T::Name)+ ">"; }

  protected:
    
    // ///////////////////////////////////////////////////////////////////
    //
    // GroupRule implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the identity of the multiplication group (one)
    const Result get_identity() const { return 1.0; }

    //! Multiply the total by the element
    void operate (Result& total, const Result& element)
    { total = total * element; }

    //! Neighbouring terms stay in each other's partial derivatives
    const Result partial (const Result& element) const { return element; }

  };
  
}





#endif

