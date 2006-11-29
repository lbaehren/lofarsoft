/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars B"ahren (bahren@astron,nl)                                       *
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

/* $Id: ClippingFraction.cc,v 1.8 2006/11/02 12:38:31 horneff Exp $ */

#include <Analysis/ClippingFraction.h>

/*!
  \class ClippingFraction
*/

namespace LOPES {  // Namespace LOPES -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  template <class T>
  ClippingFraction<T>::ClippingFraction ()
    : clipFraction_p(0.01)
  {
    Vector<T> limits (2);
    limits(0) = -1;
    limits(1) = 1;
    ClippingFraction<T>::setLimits (limits);
  }
  
  template <class T>
  ClippingFraction<T>::ClippingFraction (const Vector<T>& limits)
    : clipFraction_p(0.01)
  {
    ClippingFraction::setLimits (limits);
  }
  
  template <class T>
  ClippingFraction<T>::ClippingFraction (const Vector<T>& limits,
					 const T& fraction)
    : clipFraction_p(fraction)
  {
    ClippingFraction::setLimits (limits);
  }
  
  // -----------------------------------------------------------------------------
  //
  // Destruction
  //
  // -----------------------------------------------------------------------------
  
  template <class T>
  ClippingFraction<T>::~ClippingFraction ()
  {;}
  
  // -----------------------------------------------------------------------------
  //
  // Operators
  //
  // -----------------------------------------------------------------------------
  
  template <class T>
  ClippingFraction<T> &ClippingFraction<T>::operator=(const ClippingFraction &other)
  {
    if (this != &other) {
      clipLow_p = other.clipLow_p;
      //
      clipHigh_p = other.clipHigh_p;
      //
      clipFraction_p = other.clipFraction_p;
    }
    
    return *this;
  }
  
  // -----------------------------------------------------------------------------
  //
  //  Flagger parameters
  //
  // -----------------------------------------------------------------------------
  
  template <class T> T ClippingFraction<T>::clipFraction ()
  {
    return clipFraction_p;
  }
  
  template <class T> void ClippingFraction<T>::setClipFraction (const T& fraction)
  {
    clipFraction_p = fraction;
  }
  
  template <class T> Vector<T> ClippingFraction<T>::limits ()
  {
    Vector<T> limits (2);
    //
    limits(0) = clipLow_p;
    limits(1) = clipHigh_p;
    //
    return limits;
  }
  
  
  template <class T>
  void ClippingFraction<T>::setLimits (const T& clipLow,
				       const T& clipHigh)
  {
    Vector<T> limits (2);
    //
    limits(0) = clipLow;
    limits(1) = clipHigh;
    //
    ClippingFraction<T>::setLimits (limits);
  }
  
  template <class T> void ClippingFraction<T>::setLimits (const Vector<T>& limits)
  {
    clipLow_p = min(limits);
    clipHigh_p = max(limits);
  }
  
  // -----------------------------------------------------------------------------
  //
  //  Apply flagging to a block of data
  //
  // -----------------------------------------------------------------------------
  
  template <class T> bool ClippingFraction<T>::eval (const Vector<T>& data,
						     T& fraction)
  {
    int nofData (data.nelements());
    T clipped (0);
    
    for (int n(0); n<nofData; n++) {
      if (data(n) <= clipLow_p) {
	clipped++;
      }
      else if (data(n) >= clipHigh_p) {
	clipped++;
      }
    }
    
    fraction = clipped/nofData;
    
    if (fraction > clipFraction_p) {
      return true;
    } else {
      return false;
    }
  }
  
  // ============================================================================
  //
  //  Template instantiations
  //
  // ============================================================================
  
  template class ClippingFraction<float>;
  template class ClippingFraction<double>;
  
}  // Namespace LOPES -- end
