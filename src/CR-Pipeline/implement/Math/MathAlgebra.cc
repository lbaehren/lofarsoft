
#include <Math/MathAlgebra.h>
#include <scimath/Mathematics.h>

namespace CR { // Namespace CR -- begin

  // ============================================================================
  //
  //  Operations on vectors
  //
  // ============================================================================

  // ----------------------------------------------------------------- splitRange
  
  Matrix<Int> splitRange (const Vector<Int>& rangeValues,
			  const Int nofIntervals,
			  const String method="lin")
  {
    IPosition shape (rangeValues.shape());
    Vector<Int> indices(nofIntervals+1);
    Matrix<Int> intervalLimits (2,nofIntervals);
    Double step;
    int i(0);
    
    if (method == "log") {
      // Stepwidth in frequency; make shure we avoid division by zero!
      Vector<Double> tmpRange (shape);
      //
      for (uInt n=0; n<shape.nelements(); n++) {
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

} // Namespace CR -- end
