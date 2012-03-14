//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/NotCached.h,v $
   $Revision: 1.5 $
   $Date: 2008/06/15 16:12:34 $
   $Author: straten $ */

#ifndef __NotCached_H
#define __NotCached_H

#include "MEAL/CalculatePolicy.h"

namespace MEAL
{
  
  //! An evaluation policy that does not cache calculations
  /*! This policy always calls the context T::calculate method */
  template<class T>
  class NotCached : public CalculatePolicy<T>
  {
  public:

    typedef typename T::Result Result;

    NotCached (T* context) : CalculatePolicy<T> (context) { }

    //! Implement the evaluate method of the Function
    Result evaluate (std::vector<Result>* grad) const try
    { 
      if (grad)
	grad->resize (this->get_context()->get_nparam());

      Result r; 
      calculate (r, grad);

      this->get_context()->set_evaluation_changed (false);
      return r;
    }
    catch (Error& error)
      {
	throw error += class_name() + "evaluate";
      }

    //! Return the name of the class for debugging
    std::string class_name() const
    { return "MEAL::NotCached[" + this->get_context()->get_name() + "]::"; }

  };

}

#endif
