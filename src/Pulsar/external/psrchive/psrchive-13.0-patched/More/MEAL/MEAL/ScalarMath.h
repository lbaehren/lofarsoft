//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarMath.h,v $
   $Revision: 1.10 $
   $Date: 2009/06/11 06:39:43 $
   $Author: straten $ */

#ifndef __ScalarMath_H
#define __ScalarMath_H

#include "MEAL/Scalar.h"

namespace MEAL {

  class ScalarMath;

  //! Negation operator
  const ScalarMath operator - (const ScalarMath& b);

  //! Nothing operator
  const ScalarMath operator + (const ScalarMath& b);

  //! Return a ScalarMath instance representing a+b
  const ScalarMath operator + (const ScalarMath& a, const ScalarMath& b);

  //! Return a ScalarMath instance representing a-b
  const ScalarMath operator - (const ScalarMath& a, const ScalarMath& b);

  //! Return a ScalarMath instance representing a*b
  const ScalarMath operator * (const ScalarMath& a, const ScalarMath& b);
    
  //! Return a ScalarMath instance representing a/b
  const ScalarMath operator / (const ScalarMath& a, const ScalarMath& b);

  //! Return true if the expression evaluations are equal
  bool operator == (const ScalarMath& a, const ScalarMath& b);

  //! Return true if the expression evaluations are not equal
  bool operator != (const ScalarMath& a, const ScalarMath& b);

  //! Return true if the expression evaluation of a is less than that of b
  bool operator < (const ScalarMath& a, const ScalarMath& b);

  //! Return true if the expression evaluation of a is less than that of b
  bool operator > (const ScalarMath& a, const ScalarMath& b);

  //! Comparison operator
  bool operator <= (const ScalarMath&, const ScalarMath&);

  //! Comparison operator
  bool operator >= (const ScalarMath&, const ScalarMath&);

  //! Return a ScalarMath instance representing x^y
  const ScalarMath pow (const ScalarMath& x, const ScalarMath& y);

  //! Return a ScalarMath instance representing x^.5
  const ScalarMath sqrt (const ScalarMath& x);

  //! Return a ScalarMath instance representing |x|
  const ScalarMath abs (const ScalarMath& x);

  //! Return a ScalarMath instance representing sin(x)
  const ScalarMath sin (const ScalarMath& x);

  //! Return a ScalarMath instance representing cos(x)
  const ScalarMath cos (const ScalarMath& x);

  //! Return a ScalarMath instance representing tan(x)
  const ScalarMath tan (const ScalarMath& x);

  //! Return a ScalarMath instance representing sinh(x)
  const ScalarMath sinh (const ScalarMath& x);
  
  //! Return a ScalarMath instance representing cosh(x)
  const ScalarMath cosh (const ScalarMath& x);

  //! Return a ScalarMath instance representing tanh(x)
  const ScalarMath tanh (const ScalarMath& x);

  //! Return a ScalarMath instance representing exp(x)
  const ScalarMath exp (const ScalarMath& x);

  //! Return a ScalarMath instance representing log(x)
  const ScalarMath log (const ScalarMath& x);

  //! Return a ScalarMath instance representing atanh(x)
  const ScalarMath atanh (const ScalarMath& x);

  //! Return a ScalarMath instance representing atan(x)
  const ScalarMath atan (const ScalarMath& x);

  //! Return a ScalarMath instance representing tan(y/x)
  const ScalarMath atan2 (const ScalarMath& y, const ScalarMath& x);

  //! Convenience interface to building expressions from elements
  class ScalarMath {

  public:

    //! Default construct from a scalar value
    ScalarMath (double value = 0.0);

    //! Construct from a scalar value
    ScalarMath (const Estimate<double>& value);

    //! Copy constructor
    ScalarMath (const ScalarMath& s);

    //! Construct from a Reference::To<Scalar>
    ScalarMath (const Reference::To<Scalar>& s);

    //! Construct from a pointer to Scalar
    ScalarMath (Scalar& s);

    //! Assignment operator
    const ScalarMath operator = (const ScalarMath& s);

    //! Addition operator
    const ScalarMath operator += (const ScalarMath& b);

    //! Subtraction operator
    const ScalarMath operator -= (const ScalarMath& b);
    
    //! Multiplication operator
    const ScalarMath operator *= (const ScalarMath& b);
    
    //! Division operator
    const ScalarMath operator /= (const ScalarMath& b);

    friend const ScalarMath operator - (const ScalarMath& b);
    friend const ScalarMath operator + (const ScalarMath& b);
    friend const ScalarMath operator + (const ScalarMath& a,
					const ScalarMath& b);
    friend const ScalarMath operator - (const ScalarMath& a,
					const ScalarMath& b);
    friend const ScalarMath operator * (const ScalarMath& a,
					const ScalarMath& b);
    friend const ScalarMath operator / (const ScalarMath& a,
					const ScalarMath& b);
    friend bool operator == (const ScalarMath& a, const ScalarMath& b);
    friend bool operator != (const ScalarMath& a, const ScalarMath& b);
    friend bool operator < (const ScalarMath& a, const ScalarMath& b);
    friend bool operator > (const ScalarMath& a, const ScalarMath& b);
    friend bool operator <= (const ScalarMath&, const ScalarMath&);
    friend bool operator >= (const ScalarMath&, const ScalarMath&);
    friend const ScalarMath pow (const ScalarMath& x, const ScalarMath& y);
    friend const ScalarMath sqrt (const ScalarMath& x);
    friend const ScalarMath abs (const ScalarMath& x);
    friend const ScalarMath sin (const ScalarMath& x);
    friend const ScalarMath cos (const ScalarMath& x);
    friend const ScalarMath tan (const ScalarMath& x);
    friend const ScalarMath sinh (const ScalarMath& x);
    friend const ScalarMath cosh (const ScalarMath& x);
    friend const ScalarMath tanh (const ScalarMath& x);
    friend const ScalarMath exp (const ScalarMath& x);
    friend const ScalarMath log (const ScalarMath& x);
    friend const ScalarMath atanh (const ScalarMath& x);
    friend const ScalarMath atan (const ScalarMath& x);
    friend const ScalarMath atan2 (const ScalarMath& y, const ScalarMath& x);

    //! Evaluate the expression
    double evaluate () const;

    //! Evaluate the expression and its estimated error
    Estimate<double> get_Estimate () const;

    //! Access the expression
    Scalar* get_expression () { return expression; }

  protected:

    //! The expression
    Reference::To<Scalar> expression;

  };


}

//! Useful for quickly printing the values
inline std::ostream&
operator << (std::ostream& o, const MEAL::ScalarMath& sm)
{
  return o << sm.get_Estimate();
}


#endif

