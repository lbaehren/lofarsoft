
#if !defined(_MATHALGEBRA_H)
#define _MATHALGEBRA_H

/* Id */

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <scimath/Mathematics/MatrixMathLA.h>
#include <casa/Arrays/Vector.h>
#include <scimath/Mathematics.h>
#include <casa/BasicSL/Complex.h>
#include <scimath/Mathematics/FFTServer.h>
#include <casa/BasicMath/Math.h>
#include <casa/Quanta.h>
#include <casa/OS/Time.h>

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
/*!
  \brief Algebraic operations on vectors and matrices

  \ingroup Math

  \author Lars B&auml;hren

  \date 2004/10
*/

  /*!
    \brief Split a range of values into a number of subsets.
    
    Divide a range of values, \f$ [x_{\rm min},x_{\rm max}] \f$, into \f$ N \f$
    subsets.
    
    \param rangeValues  - Range of values, \f$ [x_{\rm min},x_{\rm max}] \f$.
    \param nofIntervals - The number of intervals, \f$ N \f$, into which the
    range of values is subdivided.
    \param method       - Method to divide the frequency range into sub-bands:
    <i>lin</i> (linear; this also is the default,
    <i>log</i> (logarithmic).
    
    \return indices - Matrix with the indices defining the limits of the 
    sub-bands; [2,nofIntervals]
  */
  Matrix<Int> splitRange (const Vector<Int>& rangeValues,
			  const Int nofIntervals,
			  const String method);
  
} // Namespace CR -- end

#endif
