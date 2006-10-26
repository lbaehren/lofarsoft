/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

/* $Id: RangeSplitter.cc,v 1.1 2006/03/28 12:01:44 bahren Exp $*/

#include <lopes/Functionals/RangeSplitter.h>
#include <casa/BasicMath/Math.h>
#include <scimath/Mathematics.h>

/*!
  \class RangeSplitter
*/

// ==============================================================================
//
//  Construction
//
// ==============================================================================

template <class T>
RangeSplitter<T>::RangeSplitter ()
{
  nofSegments_p = 1;
  method_p = "lin";
}

template <class T>
RangeSplitter<T>::RangeSplitter (const String& method)
{
  nofSegments_p = 1;
  method_p = method;
}

template <class T>
RangeSplitter<T>::RangeSplitter (const Int& nofSegments)
{
  nofSegments_p = nofSegments;
  method_p      = "lin";
}

template <class T>
RangeSplitter<T>::RangeSplitter (const Int& nofSegments,
				 const String& method)
{
  nofSegments_p = nofSegments;
  method_p = method;
}

template <class T>
RangeSplitter<T>::RangeSplitter (const RangeSplitter& other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

template <class T>
RangeSplitter<T>::~RangeSplitter ()
{
  destroy();
}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

template <class T>
RangeSplitter<T>& RangeSplitter<T>::operator= (const RangeSplitter& other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

template <class T>
void RangeSplitter<T>::copy (const RangeSplitter& other)
{;}

template <class T>
void RangeSplitter<T>::destroy ()
{;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================



// ==============================================================================
//
//  Methods
//
// ==============================================================================

template <class T>
Bool RangeSplitter<T>::split (const Vector<T>& input)
{
  T minValue (min(input));
  T maxValue (max(input));

  return splitRange (minValue,
		     maxValue);
}

template <class T>
Bool RangeSplitter<T>::split (const T& minValue,
			      const T& maxValue)
{
  return splitRange (minValue,
		     maxValue);
}

template <class T>
Bool RangeSplitter<T>::split (const Vector<T>& input,
			      const Int& nofSegments)
{
  setNofSegments (nofSegments);
  
  return split (input);
}

template <class T>
Bool RangeSplitter<T>::splitRange (const T& minValue,
				   const T& maxValue)
{
  Bool status (True);
  int i (0);
  double step (0.0);
  Vector<T> range (2);

  range(0) = minValue;
  range(1) = maxValue;

  rangeLimits_p.resize (nofSegments_p+1);

  step = Double((maxValue-minValue)/(1.0*nofSegments_p));
  
  if (method_p == "exp") {
    cout << "exponential splitting ..." << endl;
  } 
  else if (method_p == "lin") {
    for (i=0; i<=nofSegments_p; i++) {
      rangeLimits_p(i) = T(minValue+i*step);
    }
  }
  else if (method_p == "log") {
    // adjust range delimiters
    if (minValue == 0) {
      range += maxValue;
    }
    // compute stepwidth
    step = log10(1.0*range(1)/range(0))/(1.0*nofSegments_p);
    // generate limits of sub-bands
    for (i=0; i<=nofSegments_p; i++) {
      rangeLimits_p(i) = T(range(0)*pow(10.0,step*i));
    }
    // corretion of rangeLimits (undo previous shift)
    if (minValue == 0) {
      rangeLimits_p -= maxValue;
    }
  }
  
  // -- some feedback
//   cout << " - range of values : " << range << endl;
//   cout << " - nof segments    : " << nofSegments_p << endl;
//   cout << " - splitting mode  : " << method_p << endl;
//   cout << " - stepwidth       : " << step << endl;
//   cout << " - range limits    : " << rangeLimits_p << endl;

  return status;
}
