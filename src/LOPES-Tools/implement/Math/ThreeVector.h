/***************************************************************************
                          threevector.h  -  description
                             -------------------
    begin                : Tue May 27 2003
    copyright            : (C) 2003 by Tim Huege
    email                : tim.huege@ik.fzk.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef THREEVECTOR_H
#define THREEVECTOR_H

#include <cmath>
#include <iostream>

using std::ostream;
using std::istream;

//! enum to specify rotation axes
enum Axis { XAxis, YAxis, ZAxis };
 
/*!
  \class ThreeVector

  \ingroup Math

  \brief A generic threedimensional vector class.

  \author Tim Huege
*/
class ThreeVector {
  
 protected:

  //! cartesian x coordinate
  double x;
  //! cartesian y coordinate
  double y;
  //! cartesian z coordinate
  double z;
  
 public: 
  
   //! constructor creating zero-vector
  ThreeVector();

  /*!
    \brief constructor with given vector coordinates

    \param px -- cartesian x coordinate
    \param py -- cartesian y coordinate
    \param pz -- cartesian z coordinate
  */
  ThreeVector(const double &px,
	      const double &py,
	      const double &pz); 
  
  //! Destructor
  ~ThreeVector();
  
  /*!
    \brief get cartesian x coordinate

    \return x -- cartesian x coordinate
  */
  const double& GetX() const;
  /*!
    \brief get cartesian y coordinate

    \return y -- cartesian y coordinate
  */
  const double& GetY() const;
  /*!
    \brief get cartesian z coordinate

    \return z -- cartesian z coordinate
  */
  const double& GetZ() const;
  
  /*!
    \brief Multiplication of vector with scalar factor

    For a vector \f$ \vec x = (x_1,x_2,x_3) \f$ and a scalar \f$ a \f$
    \f[ a \cdot \vec x = (a x_1, a x_2, a x_3) \f]

    \param factor -- Scalar factor with which to multiply
  */
  ThreeVector& operator*= (const double& factor);

  /*!
    \brief Addition of two vectors

    \param rhs -- Vector to be added
   */
  ThreeVector& operator+= (const ThreeVector& rhs);

  /*!
    \brief Substraction of two vectors

    \param rhs -- Vector to be substracted
  */
  ThreeVector& operator-= (const ThreeVector& rhs);
  /*!
    \brief Substraction of two vectors

    \param rhs -- Vector to be substracted
  */
  const ThreeVector operator- (const ThreeVector& rhs) const;
  /*!
    \brief Addition of two vectors

    \param rhs -- Vector to be added
   */
  const ThreeVector operator+ (const ThreeVector& rhs) const;
  /*!
    \brief Set vector from other

    \param other -- vector to store

    \param status -- Status of the operation; returns <tt>false</tt> if an error
                     occured.
  */
  bool operator== (const ThreeVector& rhs) const;
  
  /*!
    \brief get vector length

    \return length -- vector length
  */
  double GetLength() const;
  
  /*!
    \brief calc dot product of *this with rhs

    For two vectors \f$ \vec x = (x_1, x_2, x_3) \f$ and \f$ \vec y =
    (y_1, y_2, y_3) \f$ this function evaluates
    \f[ \vec x \cdot \vec y = \sum_k x_k y_k = x_1 y_1 + x_2 y_2 + x_3 y_3 \f]
    
    \param rhs -- Right hand side vector in the multiplication

    \return result -- dot product of <tt>*this</tt> with <tt>rhs</tt>
  */
  const double DottedWith(const ThreeVector& rhs) const;

  //! calc cross product of *this with rhs
  const ThreeVector CrossedWith(const ThreeVector& rhs) const;
  
  //! get unit vector pointing in direction of *this
  const ThreeVector GetDirection() const;
  
  //! get angle of *this to rhs
  const double GetAngleTo(const ThreeVector& rhs) const;
  
   //! rotate *this by parAngle (radians) around parAxis
  ThreeVector& Rotate(const double& parAngle,
		      const Axis parAxis);
  
  // friends
  /*!
    \param factor -- 
    \param rhs    -- 
   */
  friend const ThreeVector operator* (const double& factor,
				      const ThreeVector& rhs);
  /*!
    \param rhs    -- 
    \param factor -- 
   */
  friend const ThreeVector operator/ (const ThreeVector& rhs,
				      const double& factor);
  /*!
    \param os  -- 
    \param rhs -- 
  */
  friend ostream& operator<< (ostream& os,
			      const ThreeVector& rhs);
  /*!
    \param is  -- 
    \param rhs -- 
  */
  friend istream& operator>> (istream& is,
			      ThreeVector& rhs);
  
  //! exception class
  class Exception { };
};

// ----------------------------------------------------------------- constructors

inline
ThreeVector::ThreeVector()
  : x(0.0), y(0.0), z(0.0)
{}

inline
ThreeVector::ThreeVector(const double &px,
			 const double &py,
			 const double &pz)
  : x(px), y(py), z(pz)
{}

// ------------------------------------------------------------- access functions

inline
const double& ThreeVector::GetX() const
{
  return x;
}

inline
const double& ThreeVector::GetY() const
{
  return y;
}

inline
const double& ThreeVector::GetZ() const
{
  return z;
}

// -------------------------------------------------------------------- operators

inline
ThreeVector& ThreeVector::operator*= (const double& factor)
{
  x*=factor;
  y*=factor;
  z*=factor;
  return *this;
}

inline
ThreeVector& ThreeVector::operator+= (const ThreeVector& rhs)
{
  x+=rhs.x;
  y+=rhs.y;
  z+=rhs.z;
  return *this;
}

inline
ThreeVector& ThreeVector::operator-= (const ThreeVector& rhs)
{
  x-=rhs.x;
  y-=rhs.y;
  z-=rhs.z;
  return *this;
}

inline
const ThreeVector ThreeVector::operator- (const ThreeVector& rhs) const
{
  return ThreeVector(x - rhs.x, y - rhs.y, z - rhs.z);
}

inline
const ThreeVector ThreeVector::operator+ (const ThreeVector& rhs) const
{
  return ThreeVector(x + rhs.x, y + rhs.y, z + rhs.z);
}

inline
bool ThreeVector::operator== (const ThreeVector& rhs) const
{
  return ((rhs.x == this->x) && (rhs.y == this->y) && (rhs.z == this->z));
}

// ------------------------------------------------------------- friend operators

inline
const ThreeVector operator* (const double& factor, const ThreeVector& rhs)
{
	return ThreeVector(rhs.x*factor, rhs.y*factor, rhs.z*factor);
}

inline
const ThreeVector operator/ (const ThreeVector& rhs, const double& factor)
{
  return ThreeVector(rhs.x/factor, rhs.y/factor, rhs.z/factor);
}

// ----------------------------------------------------------------------- others

inline
const double ThreeVector::DottedWith(const ThreeVector& rhs) const
{
  return (this->x * rhs.x + this->y * rhs.y + this->z * rhs.z);
}

inline
const ThreeVector ThreeVector::CrossedWith ( const ThreeVector& rhs) const
{
  return ThreeVector( this->y * rhs.z - rhs.y * this->z,
		      this->z * rhs.x - rhs.z * this->x,
		      this->x * rhs.y - rhs.x * this->y);
}

inline
const ThreeVector ThreeVector::GetDirection() const
{
  return (1.0 / this->GetLength() * (*this));
}


inline
const double ThreeVector::GetAngleTo(const ThreeVector& rhs) const
{
  return acos(this->DottedWith(rhs)/(this->GetLength()*rhs.GetLength()));
}


inline
double ThreeVector::GetLength() const
{
  return sqrt(x*x + y*y + z*z);
}

// ---------------------------------------------------------------------- friends

inline
ostream& operator<<(ostream& os,
		    const ThreeVector& rhs)
{
  os << rhs.x << "\t" << rhs.y << "\t" << rhs.z;
  return os;
}

inline
istream& operator>>(istream& is, ThreeVector& rhs)
{
  is >> rhs.x >> rhs.y >> rhs.z;
  return is;
}



#endif
