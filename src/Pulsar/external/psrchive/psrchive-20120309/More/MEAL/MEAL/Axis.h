//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Axis.h,v $
   $Revision: 1.7 $
   $Date: 2007/11/08 20:55:07 $
   $Author: straten $ */

#ifndef __MEAL_Axis_Header
#define __MEAL_Axis_Header

#include "MEAL/Argument.h"
#include "Callback.h"

#include <iostream>

namespace MEAL {

  //! An argument of a specified type
  /*! This template class implements the Argument/Value interface by
    enabling methods to be connected and called by Value instances. */

  template<class Type> 
  class Axis : public Argument {

  public:

    //! A value of the independent variable represented by Axis
    /*! This class implements the apply method of the Argument::Value
    base class by calling the send method of its Axis. */
    class Value : public Argument::Value {

    public:

      friend class Axis;

      //! Construct with value
      Value (const Type& _value) { value = _value; }
      
      //! Set the value of this instance
      void set_value (const Type& _value) { value = _value; }
      
      //! Get the value of this instance
      Type get_value () const { return value; }

      //! Get the value of this instance by implicit cast
      operator Type () const { return value; }
 
      // ///////////////////////////////////////////////////////////////////
      //
      // Argument::Value implementation
      //
      // ///////////////////////////////////////////////////////////////////
      
      //! Apply the value 
      void apply () const 
      { 
	if (verbose) std::cerr << "MEAL::Axis<Type>::Value::apply value="
			       << value << std::endl;
	axis->set_value (value);
      }

    protected:
      
      //! Construct with value and Axis
      Value (const Type& _value, Axis* _axis) { value = _value; axis = _axis; }

      //! The value of this instance
      Type value;

      //! The Axis on which this value applies
      Reference::To<Axis> axis;

    };

    //! Return a new instance of the Value of this argument
    Value* new_Value (const Type& value) { return new Value(value, this); }

    //! Return an instance of the Value of this argument
    Value get_Value (const Type& value) { return Value(value, this); }

    //! Set the value of the argument represented by this axis
    void set_value (const Type& value)
    { signal.send (value); current_value = value; }

    //! Get the value of the argument represented by this axis
    Type get_value () const { return current_value; }

    //! The mechanism by which methods are connected
    Callback<Type> signal;

  protected:

    Type current_value;

  };

}

#endif

