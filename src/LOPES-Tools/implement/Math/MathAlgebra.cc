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

#include <Math/MathAlgebra.h>

namespace LOPES { // Namespace LOPES -- begin

  // --------------------------------------------------------------------- L1Norm

  template <class T> T L1Norm (const Vector<T>& vec)
  {
    T norm (0.0);
    norm = sqrt(sum(fabs(vec)));
    return norm;
  }

  template float L1Norm (const Vector<float>& vec);
  template double L1Norm (const Vector<double>& vec);
  
  // --------------------------------------------------------------------- L2Norm

  template <class T> T L2Norm (const Vector<T>& vec)
  {
    T norm (0.0);
    norm = sqrt(sum(pow(vec,2)));
    return norm;
  }
  
  template float L2Norm (const Vector<float>& vec);
  template double L2Norm (const Vector<double>& vec);

  // ============================================================================
  //
  //  Operations on vectors
  //
  // ============================================================================

  // ----------------------------------------------------------------- splitRange
  
  Matrix<int> splitRange (const Vector<int>& rangeValues,
			  const int nofIntervals,
			  const String method="lin")
  {
    IPosition shape (rangeValues.shape());
    Vector<int> indices(nofIntervals+1);
    Matrix<int> intervalLimits (2,nofIntervals);
    double step;
    int i(0);
    
    if (method == "log") {
      // Stepwidth in frequency; make shure we avoid division by zero!
      Vector<double> tmpRange (shape);
      //
      for (unsigned int n=0; n<shape.nelements(); n++) {
	tmpRange(n) = double(rangeValues(n));
      }
      if (rangeValues(0) == 0) {
	tmpRange += double(rangeValues(1));
      }
      step = log10(tmpRange(1)/tmpRange(0))/nofIntervals;
      // generate limits of sub-bands
      for (int i=0; i<=nofIntervals; i++) {
	indices(i) = int(rangeValues(0)*pow(10.0,step*i));
      }
    } else {
      // Stepwidth in frequency
      step = ((rangeValues(1)-rangeValues(0)+1)/(1.0*nofIntervals));
      // generate limits of sub-bands
      for (i=0; i<=nofIntervals; i++) {
	indices(i) = int(rangeValues(0)+i*step);
      }
    }

    for (i=0; i<nofIntervals; i++) {
      intervalLimits(0,i) = indices(i);
      intervalLimits(1,i) = indices(i+1)-1;
    }
    
    return intervalLimits;
  }

} // Namespace LOPES -- end
