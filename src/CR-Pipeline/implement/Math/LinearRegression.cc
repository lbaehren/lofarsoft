/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/* $Id: LinearRegression.cc,v 1.3 2006/11/02 12:39:16 horneff Exp $*/

#include <Math/LinearRegression.h>

// --- Construction ------------------------------------------------------------

template <class T>
LinearRegression<T>::LinearRegression ()
{
  LinearRegression<T>::init ();
}

template <class T>
LinearRegression<T>::LinearRegression (const Vector<T>& x,
				       const Vector<T>& y)
{
  LinearRegression<T>::fit (x, y);
}

// --- Destruction -------------------------------------------------------------

template <class T>
LinearRegression<T>::~LinearRegression ()
{;}

// --- Operators ---------------------------------------------------------------

template <class T>
LinearRegression<T> &LinearRegression<T>::operator= (const LinearRegression &other)
{
  if (this != &other) {
    a_p = other.a_p;
    //
    b_p = other.b_p;
    //
    y_p.resize (other.y_p.shape());
    y_p = other.y_p;
    //
    chisq_p = other.chisq_p;
  }

  return *this;
}

// --- Parameters --------------------------------------------------------------

template <class T>
void LinearRegression<T>::init ()
{
  a_p = (T)0;
  //
  b_p = (T)0;
  //
  y_p.resize (1);
  y_p = (T)0;
  //
  chisq_p = (T)0;
}

// --- Computation -------------------------------------------------------------

template <class T>
void LinearRegression<T>::fit (const Vector<T>& x,
			       const Vector<T>& y)
{
  uInt nelements(x.nelements());

  // check of input vectors are conformant
  if (nelements != y.nelements()) {
    cerr << "[LinearRegression::fit] Mismatch in shape of input arrays!"
	 << endl;
    //
    LinearRegression<T>::init ();
  } else {
    T xMean (mean(x));
    T yMean (mean(y));
    T bxy (0); 
    T bx (0); 
    uInt i (0);
    
    // --- compute b -------------------------------------------------
    for (i=0; i<nelements; i++) {
      bx += (x(i)-xMean)*(x(i)-xMean);
      bxy += (x(i)-xMean)*(y(i)-yMean);
    }
    b_p = bxy/bx;
    
    // --- compute a -------------------------------------------------
    a_p = yMean - b_p*xMean;

    // --- Evaluate the fitted linear regression ---------------------

    y_p = LinearRegression<T>::y (x);

    // --- Compute chisq for the fit ---------------------------------
    chisq_p = 0;
    for (i=0; i<nelements; i++) {
      chisq_p += (y_p(i)-y(i))*(y_p(i)-y(i));
    }
    
  }
  
}

// --- Function evaluation -----------------------------------------------------

template <class T>
Vector<T> LinearRegression<T>::y (const Vector<T>& x)
{
  uInt nelements (x.nelements());
  Vector<T> y (nelements);
  //
  for (uInt n(0); n<nelements; n++) {
    y(n) = a_p + b_p*x(n);
  }
  //
  return y;
}


// =============================================================================
//
//  Template instantiation
//
// =============================================================================

template class LinearRegression<Float>;
template class LinearRegression<Double>;
