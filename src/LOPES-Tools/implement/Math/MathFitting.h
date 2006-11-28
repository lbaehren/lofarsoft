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

/* $Id: Statistics.h,v 1.5 2006/10/31 18:24:08 bahren Exp $*/

#ifndef MATHFITTING_H
#define MATHFITTING_H

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicMath/Math.h>
#include <casa/BasicSL/Complex.h>
#include <casa/OS/Time.h>
#include <casa/Quanta.h>
#include <measures/Measures.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasTable.h>
#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>

// Custom header files
#include <Utilities/ProgressBar.h>

using casa::Vector;

/*!
  \file Math::MathFitting.h

  \brief Fitting operations and methods.

  \author Lars B&auml;hren

  \date 2005/02
*/

namespace LOPES { // Namespace LOPES -- begin

  /*!
    \brief Performs a linear regression on a set of x- and y-values
    
    Given two vectors \f$x\f$ and \f$y\f$ of input values, performs a linear
    regression \f$ y = a + b x \f$, where
    \f[
    b = \frac{\sum_{i} (x_i - \langle x \rangle) \cdot
    (y_i - \langle y_i \rangle)}{\sum_i (x_i - \langle x \rangle)^2}
    \ , \qquad
    a = \langle y \rangle - b \cdot \langle x \rangle
    \ , \qquad
    \chi^2 = \sum_i (x_i - y_i)^2
    \f]
    
    \param x     - \f$x\f$-axis values to be fitted.
    \param y     - \f$y\f$-axis values to be fitted.
    \param a     -
    \param b     -
    \param chisq -
  */
  void LinearRegression (Vector<double>& x,
			 Vector<double>& y,
			 double& a,
			 double& b,
			 double& chisq);
  
} // Namespace LOPES -- end
  
#endif
  
