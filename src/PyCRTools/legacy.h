#ifndef __PYCRTOOLS_LEGACY_H__
#define __PYCRTOOLS_LEGACY_H__

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <casa/Arrays.h>
#include <casa/Vector.h>
#include <casa/Exceptions/Error.h>
#include <measures/Measures.h>
#include <measures/Measures/MDirection.h>
#include <coordinates/Coordinates/Projection.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>

namespace CR {

  const double lightspeed = 299792458.0;

  class CoordinateType {

  public:

    //! Types of (spatial) coordinates
    enum Types {
      //! Azimuth-Elevation-Height, \f$ \vec x = (Az,El,H) \f$
      AzElHeight,
      //! Azimuth-Elevation-Radius, \f$ \vec x = (Az,El,R) \f$
      AzElRadius,
      //! Cartesian coordinates, \f$ \vec x = (x,y,z) \f$
      Cartesian,
      //! Cylindrical coordinates, \f$ \vec x = (r,\phi,h) \f$
      Cylindrical,
      //! Direction on the sky, \f$ \vec x = (Lon,Lat) \f$
      Direction,
      //! Direction on the sky with radial distance, \f$ \vec x = (Lon,Lat,R) \f$
      DirectionRadius,
      //! Frquency
      Frequency,
      //! Longitude-Latitude-Radius
      LongLatRadius,
      //! North-East-Height
      NorthEastHeight,
      //! Spherical coordinates, \f$ \vec x = (r,\phi,\theta) \f$
      Spherical,
      //! Time
      Time
    };
  };

  //! Convert std::vector<T> to casac::Vector<T>
  template <class T, class S>
    void convertVector (casa::Vector<T> &to,
			std::vector<S> const &from)
  {
    unsigned int nelem = from.size();
    // adjust the shape of the returned casa::Vector<T>
    to.resize(nelem);
    
    for (unsigned int n(0); n<nelem; ++n) {
      to(n) = (T)from[n];
    }
  }
  
  //! Convert std::vector<T> to casac::Vector<T>
  template <class T, class S>
    void convertVector (std::vector<T> & to,
			const casa::Vector<S> from)
  {
    unsigned int nelem = from.size();
    // adjust the shape of the returned casa::Vector<T>
    to.resize(nelem);
    
    for (unsigned int n(0); n<nelem; ++n) {
      to(n) = (T)from[n];
    }
  }

  // === Conversion between angle representations ===============================

  //! Convert radian to degrees
  inline double deg2rad (double const &deg) {
    return deg*M_PI/180.0;
  }
  
  //! Convert radian to degrees
  inline void deg2rad (double &rad,
		       double const &deg) {
    rad = deg2rad (deg);
  }
  
  //! Convert radian to degrees
  inline vector<double> deg2rad (vector<double> const &deg) {
    vector<double> rad (deg.size());
    for (unsigned int n(0); n<deg.size(); n++) {
      rad[n] = deg2rad(deg[n]);
    }
    return rad;
  }

  //! Convert radian to degrees
  inline casa::Vector<double> deg2rad (const casa::Vector<double>& deg) {
    casa::Vector<double> rad = deg*casa::C::pi/180.0;
    return rad;
  }
  
  /*!
    \brief Convert angular components from angles to degrees
    
    \param in   -- Input vector with angular components in degrees.
    \return out -- Output vector with angular components in radian.
  */
  casa::Vector<double> deg2rad (casa::Vector<double> const &in,
				CR::CoordinateType::Types const &coordType);
  
  /*!
    \brief Convert radian to degrees
    
    \param rad  -- Angle in radian
    \return deg -- Angle in degrees
  */
  inline double rad2deg (double const &rad) {
    return rad*180/M_PI;
  }

  /*!
    \brief Convert radian to degrees
    
    \retval deg -- Angle in degrees
    \param rad  -- Angle in radian
  */
  inline void rad2deg (double &deg,
		       double const &rad) {
    deg = rad2deg (rad);
  }
  
  /*!
    \brief Convert radian to degrees

    \param rad  -- Vector with angles given in radian
    \return deg -- Vector with angles given in degree
  */
  inline vector<double> rad2deg (vector<double> const &rad) {
    vector<double> deg (rad.size());
    for (unsigned int n(0); n<rad.size(); n++) {
      deg[n] = rad2deg(rad[n]);
    }
    return deg;
  }

  /*!
    \brief Convert angular position from radian to degree

    \param rad  -- Vector with angles given in radian
    \return deg -- Vector with angles given in degree
  */
  inline casa::Vector<double> rad2deg (const casa::Vector<double>& rad) {
    casa::Vector<double> deg = rad*180.0/casa::C::pi;
    return deg;
  }

  /*!
    \brief Conversion from North-East-Height to cartesian coordinates

    \retval x  -- \f$x\f$-component of the vector in cartesian coordinates
    \retval y  -- \f$y\f$-component of the vector in cartesian coordinates
    \retval z  -- \f$z\f$-component of the vector in cartesian coordinates
    \param N   -- North component of the input vector
    \param E   -- East component of the input vector
    \param H   -- Height component of the input vector
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  inline bool NorthEastHeight2Cartesian (double &x,
					 double &y,
					 double &z,
					 double const &N,
					 double const &E,
					 double const &H)
    {
      bool status (true);

      try {
	x = E;
	y = N;
	z = H;
      } catch (std::string message) {
	std::cerr << "[NorthEastHeight2Cartesian] " << message << std::endl;
	status = false;
      }
      
      return status;
    }

  //_____________________________________________________________________________
  //                                                                     Definitions 
  casa::Vector<double> deg2rad (casa::Vector<double> const &in, CR::CoordinateType::Types const &coordType);

  bool convertVector (double &xTarget, double &yTarget, double &zTarget, CR::CoordinateType::Types const &targetCoordinate, double const &xSource, double const &ySource, double const &zSource, CR::CoordinateType::Types const &sourceCoordinate, bool const &anglesInDegrees);

  bool convertVector (std::vector<double> &target, CR::CoordinateType::Types const &targetCoordinate, std::vector<double> &source, CR::CoordinateType::Types const &sourceCoordinate, bool const &anglesInDegrees);

  bool convertVector (casa::Vector<double> &target, CR::CoordinateType::Types const &targetCoordinate, casa::Vector<double> &source, CR::CoordinateType::Types const &sourceCoordinate, bool const &anglesInDegrees);

  bool AzEl2Cartesian (double &x, double &y, double &z, double const &az, double const &el, double const &r, bool const &anglesInDegrees, bool const &lastIsRadius);

  bool AzEl2Cartesian (std::vector<double> &cartesian, std::vector<double> const &azel, bool const &anglesInDegrees, bool const &lastIsRadius);

  bool AzElHeight2Cartesian (double &x, double &y, double &z, double const &az, double const &el, double const &h, bool const &anglesInDegrees);

  bool AzElHeight2Cartesian (std::vector<double> &Cartesian, std::vector<double> const &AzElHeight, bool const &anglesInDegrees);

  bool AzElHeight2Cartesian (casa::Vector<double> &Cartesian, casa::Vector<double> const &AzElHeight, bool const &anglesInDegrees);

  bool AzElRadius2Cartesian (double &x, double &y, double &z, double const &az, double const &el, double const &r, bool const &anglesInDegrees);

  bool AzElRadius2Cartesian (std::vector<double> &Cartesian, std::vector<double> const &AzElRadius, bool const &anglesInDegrees);

  bool AzElRadius2Cartesian (casa::Vector<double> &Cartesian, casa::Vector<double> const &AzElRadius, bool const &anglesInDegrees);

  bool AzElRadius2Spherical (double &r, double &phi, double &theta, double const &az, double const &el, double const &radius, bool const &anglesInDegrees);

  bool AzElRadius2Spherical (std::vector<double> &Spherical, std::vector<double> const &AzElRadius, bool const &anglesInDegrees);

  bool AzElRadius2Spherical (casa::Vector<double> &Spherical, casa::Vector<double> const &AzElRadius, bool const &anglesInDegrees);

  bool Cartesian2AzElHeight (double &az, double &el, double &h, const double &x, const double &y, const double &z, bool const &anglesInDegrees);

  bool Cartesian2AzElHeight (std::vector<double> &AzElHeight, std::vector<double> const &cartesian, bool const &anglesInDegrees);

  bool Cartesian2AzElHeight (casa::Vector<double> &AzElHeight, casa::Vector<double> const &cartesian, bool const &anglesInDegrees);

  bool Cartesian2AzElRadius (double &az, double &el, double &radius, double const &x, double const &y, double const &z, bool const &anglesInDegrees);

  bool Cartesian2AzElRadius (std::vector<double> &AzElRadius, std::vector<double> const &cartesian, bool const &anglesInDegrees);

  bool Cartesian2AzElRadius (casa::Vector<double> &AzElRadius, casa::Vector<double> const &cartesian, bool const &anglesInDegrees);

  bool Cartesian2Cylindrical (double &rho, double &phi, double &z_cyl, const double &x, const double &y, const double &z, bool const &anglesInDegrees);

  bool Cartesian2Cylindrical (std::vector<double> &cylindrical, std::vector<double> const &cartesian, bool const &anglesInDegrees);

  bool Cartesian2Cylindrical (casa::Vector<double> &cylindrical, casa::Vector<double> const &cartesian, bool const &anglesInDegrees);

  bool Cartesian2Spherical (double &r, double &phi, double &theta, const double &x, const double &y, const double &z, bool const &anglesInDegrees);

  bool Cartesian2Spherical (std::vector<double> &spherical, std::vector<double> const &cartesian, bool const &anglesInDegrees);

  bool Cartesian2Spherical (casa::Vector<double> &spherical, casa::Vector<double> const &cartesian, bool const &anglesInDegrees);

  bool Cylindrical2AzElHeight (double &az, double &el, double &h, double const &rho, double const &phi, double const &z, bool const &anglesInDegrees);

  bool Cylindrical2AzElHeight (std::vector<double> &azElHeight, std::vector<double> const &cylindrical, bool const &anglesInDegrees);

  bool Cylindrical2AzElHeight (casa::Vector<double> &azElHeight, casa::Vector<double> const &cylindrical, bool const &anglesInDegrees);

  bool Cylindrical2AzElRadius (double &az, double &el, double &r, double const &rho, double const &phi, double const &z, bool const &anglesInDegrees);

  bool Cylindrical2AzElRadius (std::vector<double> &azElRadius, std::vector<double> const &cylindrical, bool const &anglesInDegrees);

  bool Cylindrical2AzElRadius (casa::Vector<double> &azElRadius, casa::Vector<double> const &cylindrical, bool const &anglesInDegrees);

  bool Cylindrical2Cartesian (double &x, double &y, double &z, double const &rho, double const &phi, double const &z_cyl, bool const &anglesInDegrees);

  bool Cylindrical2Cartesian (std::vector<double> &cartesian, std::vector<double> const &cylindrical, bool const &anglesInDegrees);

  bool Cylindrical2Cartesian (casa::Vector<double> &cartesian, casa::Vector<double> const &cylindrical, bool const &anglesInDegrees);

  bool Cylindrical2Spherical (double &r, double &phi, double &theta, double const &rho, double const &phi_cyl, double const &z, bool const &anglesInDegrees);

  bool Cylindrical2Spherical (std::vector<double> &spherical, std::vector<double> const &cylindrical, bool const &anglesInDegrees);

  bool Spherical2Cartesian (double &x, double &y, double &z, double const &r, double const &phi, double const &theta, bool const &anglesInDegrees);

  bool Spherical2Cartesian (casa::Vector<double> &cartesian, casa::Vector<double> const &spherical, bool const &anglesInDegrees);

  bool Spherical2Cylindrical (double &rho, double &phi_cyl, double &z, double const &r, double const &phi, double const &theta, bool const &anglesInDegrees);

  bool Spherical2Cylindrical (std::vector<double> &cylindrical, std::vector<double> const &spherical, bool const &anglesInDegrees);

  bool Spherical2Cylindrical (casa::Vector<double> &cylindrical, casa::Vector<double> const &spherical, bool const &anglesInDegrees);

  void azze2xyz (double &x, double &y, double &z, double const &r, double const &az, double const &ze, bool const &anglesInDegrees);

  std::vector<double> azze2xyz (std::vector<double> const &azze, bool const &anglesInDegrees);

  std::vector<double> Spherical2Cartesian (std::vector<double> const &spherical, bool const &anglesInDegrees);

  casa::Vector<double> Spherical2Cartesian (casa::Vector<double> const &spherical, bool const &anglesInDegrees);

  casa::Vector<double> AzEl2Cartesian (const casa::Vector<double>& azel, bool const &anglesInDegrees);

  casa::Vector<double> polar2Cartesian (casa::Vector<double> const &polar);

} // Namespace CR -- end

typedef CR::CoordinateType::Types CRCoordinateType;

#endif // __PYCRTOOLS_LEGACY_H__

