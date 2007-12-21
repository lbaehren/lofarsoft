/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#include <Data/DataFlag.h>

namespace CR {  //  Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  template <class T> DataFlag<T>::DataFlag (const Int& num)
    : ClippingFraction<T> ()
  {
    Vector<T> flags (num, 1.0);
    //
    DataFlag<T>::setFlags (flags);
  }
  
  template <class T> DataFlag<T>::DataFlag (const Int& num,
					    const T& threshold)
    : ClippingFraction<T> (), threshold_p(threshold)
  {
    Vector<T> flags (num, 1.0);
    //
    DataFlag<T>::setFlags (flags);
  }
  
  template <class T> DataFlag<T>::DataFlag (const Int& num,
					    const T& threshold,
					    const Vector<T>& limits)
    : ClippingFraction<T> (limits), threshold_p(threshold)
  {
    Vector<T> flags (num, 1.0);
    //
    DataFlag<T>::setFlags (flags);
  }
  
  template <class T> DataFlag<T>::DataFlag (const Vector<bool>& flags)
    : ClippingFraction<T> ()
  {
    DataFlag<T>::setFlags (flags);
  }
  
  template <class T> DataFlag<T>::DataFlag (const Vector<T>& flags)
    : ClippingFraction<T> ()
  {
    DataFlag<T>::setFlags (flags);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  template <class T> DataFlag<T>::~DataFlag ()
  {}
  
  // --- Flagging threshold ------------------------------------------------------
  
  template <class T> T DataFlag<T>::threshold ()
  {
    return threshold_p;
  }
  
  template <class T> void DataFlag<T>::setThreshold (const T& threshold)
  {
    threshold_p = threshold;
  }
  
  // --- Access to the flags -----------------------------------------------------
  
  template <class T> bool DataFlag<T>::float2bool (const T& flag)
  {
    if (flag > threshold_p) {
      return True;
    } else {
      return False;
    }
  }
  
  template <class T> void DataFlag<T>::flags (bool& flag,
					      const uInt& num)
  {
    T fflag (0.0);
    //
    DataFlag<T>::flags (fflag, num);
    // 
    flag = DataFlag<T>::float2bool (fflag);
  }
  
  template <class T> void DataFlag<T>::flags (T& flag,
					      const uInt& num)
  {
    // check if the dataset identifier is valid
    if (num < flags_p.nelements()) {
      flag = flags_p(num);
    } else {
      cerr << "[DataFlag<T>::flags]" << endl;
    }
  }
  
  template <class T> void DataFlag<T>::flags (Vector<bool>& flags)
  {
    uInt nofFiles (flags_p.nelements());
    //
    flags.resize (nofFiles);
    //
    for (uInt n(0); n<nofFiles; n++) {
      flags(n) = float2bool(flags_p(n));
    }
  }
  
  template <class T> void DataFlag<T>::flags (Vector<T>& flags)
  {
    flags.resize (flags_p.shape());
    flags = flags_p;
  }
  
  template <class T> void DataFlag<T>::setFlags (const Vector<bool>& bFlags)
  {
    Int nofElements (bFlags.nelements());
    Vector<T> flags (nofElements, 1.0);
    //
    for (int n(0); n<nofElements; n++) {
      if (!bFlags(n)) {
	flags(n) = 0.0;
      }
    }
    //
    DataFlag<T>::setFlags (flags);
  }
  
  template <class T> void DataFlag<T>::setFlags (const Vector<T>& flags)
  {
    flags_p.resize (flags.nelements());
    flags_p = flags;
  }
  
  // ============================================================================
  //
  //  Template instantiations
  //
  // ============================================================================
  
  template class DataFlag<float>;
  template class DataFlag<double>;
  
}  //  Namespace CR -- end
