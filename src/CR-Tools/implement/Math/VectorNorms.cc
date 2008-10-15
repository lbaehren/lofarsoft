/***************************************************************************
 *   Copyright (C) 2005                                                    *
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

/* $Id: Algebra.cc,v 1.2 2006/10/31 18:24:08 bahren Exp $*/

#include <cmath>
#include <Math/VectorNorms.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Support vector operations
  //
  // ============================================================================

  // ------------------------------------------------------------- scalar_product

  template <class T>
  T scalar_product (std::vector<T> const &x,
		    std::vector<T> const &y)
  {
    T product(0);

    scalar_product (product,x,y);
    
    return product;
  }

  // ------------------------------------------------------------- scalar_product

  template <class T>
  void scalar_product (T &product,
		       std::vector<T> const &x,
		       std::vector<T> const &y)
  {
    product = 0;

    if (x.size() == y.size()) {
      for (uint n(0); n<x.size(); n++) {
	product += x[n]*y[n];
      }
    } 
  }
  
  // ============================================================================
  //
  //  Vector norms
  //
  // ============================================================================

  // --------------------------------------------------------------------- L1Norm
  
  template <class T>
  T L1Norm (T const *vec,
	    unsigned int const &nelem)
  {
    T sum (0.0);
    
    for (unsigned int n(0); n<nelem; n++) {
      sum += fabs(vec[n]);
    }
    
    return sum;
  }
  
  // --------------------------------------------------------------------- L2Norm
  
  template <class T>
  T L2Norm (T const *vec,
	    unsigned int const &nelem)
  {
    T sum2 (0.0);
    
    for (unsigned int n(0); n<nelem; n++) {
      sum2 += vec[n]*vec[n];
    }
    
    return sqrt (sum2);
  }
  
#ifdef HAVE_CASA
  
  // ---------------------------------------------------------------- invertOrder

  template <class T>
  casa::Vector<T> invertOrder (casa::Vector<T> const &vec)
  {
    int nelements (vec.nelements());
    casa::Vector<T> inv (nelements);
    int n (0);
    int m (nelements-1);

    for (n=0; n<nelements; n++, m--) {
      inv(m) = vec(n);
    }
    
    return inv;
  }
 
#endif

  // ============================================================================
  //
  //  Template instantiation
  //
  // ============================================================================

  template void scalar_product (float &product,
				std::vector<float> const &x,
				std::vector<float> const &y);
  template void scalar_product (double &product,
				std::vector<double> const &x,
				std::vector<double> const &y);

  template float scalar_product (std::vector<float> const &x,
				 std::vector<float> const &y);
  template double scalar_product (std::vector<double> const &x,
				  std::vector<double> const &y);

  template float L1Norm (float const *vec, unsigned int const &nelem);
  template double L1Norm (double const *vec, unsigned int const &nelem);

  template float L2Norm (float const *vec, unsigned int const &nelem);
  template double L2Norm (double const *vec, unsigned int const &nelem);

#ifdef HAVE_CASA
  template casa::Vector<String> invertOrder (casa::Vector<String> const &vec);
  template casa::Vector<uInt> invertOrder (casa::Vector<uInt> const &vec);
  template casa::Vector<int> invertOrder (casa::Vector<int> const &vec);
  template casa::Vector<float> invertOrder (casa::Vector<float> const &vec);
  template casa::Vector<double> invertOrder (casa::Vector<double> const &vec);
  template casa::Vector<Complex> invertOrder (casa::Vector<Complex> const &vec);
  template casa::Vector<DComplex> invertOrder (casa::Vector<DComplex> const &vec);
#endif

} // Namespace CR -- end
