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

/* $Id$*/

#include <cmath>
#include <Math/VectorNorms.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Vector norms
  //
  // ============================================================================

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
