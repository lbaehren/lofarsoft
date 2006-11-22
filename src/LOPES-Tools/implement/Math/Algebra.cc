/***************************************************************************
 *   Copyright (C) 2005-2006                                               *
 *   Lars Bahren (bahren@astron.nl)                                        *
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

#include <Math/Algebra.h>

namespace LOPES { // Namespace LOPES -- begin

  // ----------------------------------------------------------------------- sign

  template <class T>
  T sign (const T& x)
  {
    if (x<0) {
      return static_cast<T>(-1);
    }
    else if (x>0) {
      return static_cast<T>(+1);
    }
    else {
      return static_cast<T>(0);
    }
  }
  
  // ----------------------------------------------------------------------- sign

  template <class T>
  Vector<T> sign (const Vector<T>& x)
  {
    IPosition shape (x.shape());
    Vector<T> s(shape);
    for (int n=0; n<shape(0); ++n) {
      s(n) = sign (x(n));
    }
    return s;
  }

  // ---------------------------------------------------------------- invertOrder

  template <class T>
  Vector<T> invertOrder (const Vector<T>& vec)
  {
    int nelements (vec.nelements());
    Vector<T> inv (nelements);
    int n (0);
    int m (nelements-1);

    for (n=0; n<nelements; n++, m--) {
      inv(m) = vec(n);
    }
    
    return inv;
  }
 
  // ----------------------------------------------------- Template instantiation

  template int sign (const int& x);
  template float sign (const float& x);
  template double sign (const double& x);

  template Vector<int> sign (const Vector<int>& x);
  template Vector<float> sign (const Vector<float>& x);
  template Vector<double> sign (const Vector<double>& x);
  
  template Vector<String> invertOrder (const Vector<String>& vec);
  template Vector<unsigned int> invertOrder (const Vector<unsigned int>& vec);
  template Vector<int> invertOrder (const Vector<int>& vec);
  template Vector<float> invertOrder (const Vector<float>& vec);
  template Vector<double> invertOrder (const Vector<double>& vec);
  template Vector<Complex> invertOrder (const Vector<Complex>& vec);
  template Vector<DComplex> invertOrder (const Vector<DComplex>& vec);
  
} // Namespace LOPES -- end
