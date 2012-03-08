//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/RuleMath.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __MEAL_RuleMath_H
#define __MEAL_RuleMath_H

#include "MEAL/SumRule.h"
#include "MEAL/NegationRule.h"
#include "MEAL/ProductRule.h"
#include "MEAL/InverseRule.h"

namespace MEAL {

  template<class Type> SumRule<Type>* Sum (Type* a, Type* b)
  {
    SumRule<Type>* sum = new SumRule<Type>;
    *sum += a;
    *sum += b;
    
    return sum;
  }
  
  template<class Type> SumRule<Type>* Difference (Type* a, Type* b)
  {
    NegationRule<Type>* neg = new NegationRule<Type>;
    neg->set_model (b);
    
    SumRule<Type>* sum = new SumRule<Type>;
    *sum += a;
    *sum += neg;
    
    return sum;
  }

  template<class Type> NegationRule<Type>* Negation (Type* a)
  {
    NegationRule<Type>* neg = new NegationRule<Type>;
    neg->set_model (a);
    return neg;
  }
  
  template<class Type> ProductRule<Type>* Product (Type* a, Type* b)
  {
    ProductRule<Type>* product = new ProductRule<Type>;
    *product *= a;
    *product *= b;
    
    return product;
  }
  
  template<class Type> ProductRule<Type>* Quotient (Type* a, Type* b)
  {
    InverseRule<Type>* inverse = new InverseRule<Type>;
    inverse->set_model (b);
    
    ProductRule<Type>* product = new ProductRule<Type>;
    *product *= a;
    *product *= inverse;
    
    return product;
  }
  
}

#endif

