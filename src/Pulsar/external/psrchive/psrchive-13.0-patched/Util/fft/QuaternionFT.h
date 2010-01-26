//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/fft/QuaternionFT.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __QuaternionFT_H
#define __QuaternionFT_H

#include "Quaternion.h"
#include "Matrix.h"

class QuaternionFT {

public:

  enum Direction { Forward, Backward };

  //! Verbosity flag
  static bool verbose;

  //! Null constructor
  QuaternionFT ();

  //! Construct with spherical coordinates of imaginary axis
  QuaternionFT (double theta, double phi);

  //! Set the imaginary axis in spherical coordinates
  void set_axis (double theta, double phi);

  //! Set the imaginary axis
  void set_axis (Vector<3,float> axis);

  //! Experimental: set the power of the exponential used in ft
  void set_power (double power);
  //! And the "fractional extent" of the sampled input function,
  //! ie length/center (== fractional bandwidth for GFR)
  void set_extent (double extent);

  //! Get the imaginary axis
  Vector<3,float> get_axis () const;

  //! Set the direction of the Quaternion Fourier Transform
  void set_direction (Direction direction);

  //! Perform a fast quaternion fourier transform
  void fft (const std::vector<Quaternion<float> >& in,
	    std::vector<Quaternion<float> >& out);

  void fft (unsigned npt, const Quaternion<float>* in, Quaternion<float>* out);

  void fft (unsigned npt, const float* input, float* output);


  //! Perform a slow quaternion fourier transform
  void ft (const std::vector<Quaternion<float> >& in,
	   std::vector<Quaternion<float> >& out);

  void ft (unsigned npt, const Quaternion<float>* in, Quaternion<float>* out);

  void ft (unsigned npt, const float* input, float* output);

protected:

  //! Basis vectors of imaginary axis
  Quaternion<float> mu[3];

  //! Transformation matrix for converting imaginary 3-vectors into basis
  Matrix<3,3,float> xform;

  //! Direction of transform
  float sign;

  //! Set xform, based on mu
  void set_xform ();

  //! Experimental: power of the exponent used in ft
  double power, extent;

};

#endif

