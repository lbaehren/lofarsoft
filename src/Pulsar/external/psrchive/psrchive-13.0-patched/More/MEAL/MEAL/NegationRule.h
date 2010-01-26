//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/NegationRule.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __MEAL_NegationRule_H
#define __MEAL_NegationRule_H

#include "MEAL/UnaryRule.h"

namespace MEAL {

  //! Computes the inverse of a model and its partial derivatives
  template<class T>
  class NegationRule : public UnaryRule<T>
  {

  public:

    typedef typename T::Result Result;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // Optimized implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the Result and its gradient
    void calculate (Result& result, std::vector<Result>* gradient);

  };

}

template<class T>
std::string MEAL::NegationRule<T>::get_name () const
{
  return "NegationRule<" + std::string(T::Name)+ ">";
}


template<class T>
void MEAL::NegationRule<T>::calculate (Result& result,
				       std::vector<Result>* grad)
{
  if (!this->model)
    throw Error (InvalidState, "MEAL::NegationRule::calculate",
		 "no model to evaluate");

  if (T::verbose)
    std::cerr << "MEAL::NegationRule::calculate" << std::endl;

  result = - this->model->evaluate (grad);
  
  if (T::verbose)
    std::cerr << "MEAL::NegationRule::calculate result\n"
      "   " << result << std::endl;

  if (!grad)
    return;

  for (unsigned igrad=0; igrad<grad->size(); igrad++)
    (*grad)[igrad] *= -1.0;

  if (T::verbose) {
    std::cerr << "MEAL::NegationRule::calculate gradient\n";
    for (unsigned i=0; i<grad->size(); i++)
      std::cerr << "   "
	   << i << ":" << this->get_infit(i) << "=" << (*grad)[i] << std::endl;
  }

}


#endif
