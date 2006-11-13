
#include <Math/MathGeometry.h>

namespace LOPES { // Namespace LOPES -- begin
  
  // ============================================================================
  //
  //  Units conversion
  //
  // ============================================================================
  
  // -------------------------------------------------------------------- deg2rad

  double deg2rad (double const &deg)
  {
    return deg*C::pi/180.0;
  }
  
  // -------------------------------------------------------------------- deg2rad

  Vector<double> deg2rad (const Vector<double>& deg)
  {
    Vector<double> rad = deg*C::pi/180.0;
    return rad;
  }
  
  // -------------------------------------------------------------------- rad2deg

  double rad2deg (double const &rad)
  {
    return rad*180.0/C::pi;
  }
  
  // -------------------------------------------------------------------- rad2deg

  Vector<double> rad2deg (const Vector<double>& rad)
  {
    Vector<double> deg = rad*180.0/C::pi;
    return deg;
  }
  
  
  // ============================================================================
  //
  //  Coordinate conversions
  //
  // ============================================================================

  // ------------------------------------------------------------- zeaz2cartesian
  
  Vector<double> zeaz2cartesian (double const &ze,
				 double const &az)
  {
    Vector<double> cartesian(2);
    double r = ze/90.0;
    
    cartesian(0) = r*sin(deg2rad(az));
    cartesian(1) = r*cos(deg2rad(az));
    
    return cartesian;
  }
  
  // ------------------------------------------------------------- zeaz2cartesian

  Vector<double> zeaz2cartesian (const Vector<double>& zeaz)
  {
    return zeaz2cartesian (zeaz(0),zeaz(1));
  }
  
} // Namespace LOPES -- end
