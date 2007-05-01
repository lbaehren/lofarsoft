/***************************************************************************
 *   Copyright (C) 2005                                                  *
 *   Lars Bahren (<mail>)                                                     *
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

/* $Id: Algebra.cc,v 1.2 2006/10/31 18:24:08 bahren Exp $*/

#include <Math/Algebra.h>

namespace CR { // Namespace CR -- begin

  // ----------------------------------------------------------------------- sign

  template <class T>
  T sign (const T& x)
  {
    if (x<0) return static_cast<T>(-1);
    else if (x>0) return static_cast<T>(+1);
    else return static_cast<T>(0);
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
    Int nelements (vec.nelements());
    Vector<T> inv (nelements);
    Int n (0);
    Int m (nelements-1);

    for (n=0; n<nelements; n++, m--) {
      inv(m) = vec(n);
    }
    
    return inv;
  }
 
  // ----------------------------------------------------- Template instantiation

  template Int sign (const Int& x);
  template Float sign (const Float& x);
  template Double sign (const Double& x);

  template Vector<Int> sign (const Vector<Int>& x);
  template Vector<Float> sign (const Vector<Float>& x);
  template Vector<Double> sign (const Vector<Double>& x);
  
  template Vector<String> invertOrder (const Vector<String>& vec);
  template Vector<uInt> invertOrder (const Vector<uInt>& vec);
  template Vector<Int> invertOrder (const Vector<Int>& vec);
  template Vector<Float> invertOrder (const Vector<Float>& vec);
  template Vector<Double> invertOrder (const Vector<Double>& vec);
  template Vector<Complex> invertOrder (const Vector<Complex>& vec);
  template Vector<DComplex> invertOrder (const Vector<DComplex>& vec);
  
} // Namespace CR -- end
