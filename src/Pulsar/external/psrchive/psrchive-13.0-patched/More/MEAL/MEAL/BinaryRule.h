//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/BinaryRule.h,v $
   $Revision: 1.8 $
   $Date: 2008/04/07 00:38:12 $
   $Author: straten $ */

#ifndef __MEAL_BinaryRule_H
#define __MEAL_BinaryRule_H

#include "MEAL/Projection.h"
#include "MEAL/Composite.h"

namespace MEAL {

  //! Abstract base class of binary operators
  template<class T>
  class BinaryRule : public T
  {

  public:

    //! Default constructor
    BinaryRule () : composite(this)  { }

    //! Copy constructor
    BinaryRule (const BinaryRule& rule) : composite(this) { operator=(rule); }

    //! Assignment operator
    BinaryRule& operator = (const BinaryRule& rule);

    //! Destructor
    ~BinaryRule () { }

    //! Set the first argument to the binary operation
    void set_arg1 (T* model);

    //! Set the second argument to the binary operation
    void set_arg2 (T* model);

  protected:

    //! The first argument to the binary operation
    Project<T> arg1;

    //! The first argument to the binary operation
    Project<T> arg2;

  private:

    //! Composite parameter policy
    Composite composite;

  };

}

template<class T>
MEAL::BinaryRule<T>&
MEAL::BinaryRule<T>::operator = (const BinaryRule& rule)
{
  if (this == &rule)
    return *this;

  set_arg1 (rule.arg1);
  set_arg2 (rule.arg2);

  return *this;
}


template<class T>
void MEAL::BinaryRule<T>::set_arg1 (T* model)
{
  if (arg1)
    composite.unmap (arg1);

  arg1 = model;

  if (!model)
    return;

  if (T::verbose)
    std::cerr << "MEAL::BinaryRule::set_model map new model" << std::endl;

  composite.map (arg1);
}

template<class T>
void MEAL::BinaryRule<T>::set_arg2 (T* model)
{
  if (arg2)
    composite.unmap (arg2);

  arg2 = model;

  if (!model)
    return;

  if (T::verbose)
    std::cerr << "MEAL::BinaryRule::set_model map new model" << std::endl;

  composite.map (arg2);
}


#endif
