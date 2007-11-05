/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#include <Imaging/GeometricalDelay.h>
#include <Math/Constants.h>

using std::cerr;
using std::cout;
using std::endl;

namespace CR { // NAMESPACE CR -- BEGIN
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  // ----------------------------------------------------------- GeometricalDelay
  
#ifdef HAVE_CASA
  GeometricalDelay::GeometricalDelay ()
  {
    Matrix<double> antennaPositions (1,3);
    Matrix<double> skyPositions (1,3);
    bool antennaIndexFirst (true);
    bool bufferDelays (false);

    antennaPositions  = 0.0;

    skyPositions      = 0.0;
    skyPositions(0,2) = 1.0;

    init (antennaPositions,
	  CR::Cartesian,
	  skyPositions,
	  CR::Cartesian,
	  bufferDelays,
	  antennaIndexFirst);
  }
#else 
#ifdef HAVE_BLITZ
  GeometricalDelay::GeometricalDelay ()
  {
    blitz::Array<double,2> antPositions (1,3);
    blitz::Array<double,2> skyPositions (1,3);
    bool antennaIndexFirst (true);
    bool bufferDelays (false);

    antPositions = 0.0, 0.0, 0.0;
    skyPositions = 0.0, 0.0, 1.0;
    
    init (antPositions,
	  CR::Cartesian,
	  skyPositions,
	  CR::Cartesian,
	  bufferDelays,
	  antennaIndexFirst);  
  }
#endif
#endif
  
  // ----------------------------------------------------------- GeometricalDelay
  
#ifdef HAVE_CASA
  GeometricalDelay::GeometricalDelay (Matrix<double> const &antPositions,
				      Matrix<double> const &skyPositions)
  {
    bool antennaIndexFirst (true);
    bool bufferDelays (false);

    init (antPositions,
	  CR::Cartesian,
	  skyPositions,
	  CR::Cartesian,
	  bufferDelays,
	  antennaIndexFirst);
  }
#else 
#ifdef HAVE_BLITZ
  GeometricalDelay::GeometricalDelay (blitz::Array<double,2> const &antPositions,
				      blitz::Array<double,2> const &skyPositions)
  {
    bool antennaIndexFirst (true);
    bool bufferDelays (false);
    
    init (antPositions,
	  CR::Cartesian,
	  skyPositions,
	  CR::Cartesian,
	  bufferDelays,
	  antennaIndexFirst);
  }
#endif
#endif

  // ----------------------------------------------------------- GeometricalDelay
  
#ifdef HAVE_CASA
  GeometricalDelay::GeometricalDelay (Matrix<double> const &antPositions,
				      CR::CoordinateType const &antCoordType,
				      Matrix<double> const &skyPositions,
				      CR::CoordinateType const &skyCoordType)
  {
    bool antennaIndexFirst (true);
    bool bufferDelays (false);
    
    init (antPositions,
	  antCoordType,
	  skyPositions,
	  skyCoordType,
	  bufferDelays,
	  antennaIndexFirst);
  }
#else 
#ifdef HAVE_BLITZ
  GeometricalDelay::GeometricalDelay (blitz::Array<double,2> const &antPositions,
				      CR::CoordinateType const &antCoordType,
				      blitz::Array<double,2> const &skyPositions,
				      CR::CoordinateType const &skyCoordType)
  {
    bool antennaIndexFirst (true);
    bool bufferDelays (false);
    
    init (antPositions,
	  antCoordType,
	  skyPositions,
	  skyCoordType,
	  bufferDelays,
	  antennaIndexFirst);
  }
#endif
#endif
  
  // ----------------------------------------------------------- GeometricalDelay
  
#ifdef HAVE_CASA
  GeometricalDelay::GeometricalDelay (Matrix<double> const &antPositions,
				      CR::CoordinateType const &antCoordType,
				      Matrix<double> const &skyPositions,
				      CR::CoordinateType const &skyCoordType,
				      bool const &bufferDelays,
				      bool const &antennaIndexFirst)
  {
    init (antPositions,
	  antCoordType,
	  skyPositions,
	  skyCoordType,
	  bufferDelays,
	  antennaIndexFirst);
  }
#else
#ifdef HAVE_BLITZ
  GeometricalDelay::GeometricalDelay (blitz::Array<double,2> const &antPositions,
				      CR::CoordinateType const &antCoordType,
				      blitz::Array<double,2> const &skyPositions,
				      CR::CoordinateType const &skyCoordType,
				      bool const &bufferDelays,
				      bool const &antennaIndexFirst)
  {
    init (antPositions,
	  antCoordType,
	  skyPositions,
	  skyCoordType,
	  bufferDelays,
	  antennaIndexFirst);
  }
#endif
#endif
  
  // ----------------------------------------------------------- GeometricalDelay
  
  GeometricalDelay::GeometricalDelay (GeometricalDelay const &other)
  {
    copy (other);
  }

  // ============================================================================
  //
  //  Initialization
  //
  // ============================================================================
  
#ifdef HAVE_CASA
  void GeometricalDelay::init (Matrix<double> const &antPositions,
			       CR::CoordinateType const &antCoordType,
			       Matrix<double> const &skyPositions,
			       CR::CoordinateType const &skyCoordType,
			       bool const &bufferDelays,
			       bool const &antennaIndexFirst)
  {
    showProgress_p = false;
    nearField_p    = true;
    
    cout << "[GeometricalDelay::init]" << endl;
    if (!setAntennaPositions (antPositions,
			      antCoordType,
			      false,
			      antennaIndexFirst)) {
      cerr << "-- There was an error setting the ant positions" << endl;
      // use defaults
      antPositions_p.resize(1,3);
      antPositions_p = 0.0;
      nofAntennas_p  = 1;
    }
    
    if (!setSkyPositions (skyPositions,false)) {
      cerr << "-- There was an error setting the sky positions" << endl;
      // use defaults
      skyPositions_p.resize(1,3);
      skyPositions_p      = 0.0;
      skyPositions_p(0,2) = 1.0;
    }
    
    // once all the input parameters have been stored we can compute the delays
    bufferDelays_p = bufferDelays;
    if (bufferDelays_p) {
      cout << "-- buffering geometrical delays..." << endl;
      setDelays();
    }
  }
#else
#ifdef HAVE_BLITZ
  void GeometricalDelay::init (blitz::Array<double,2> const &antPositions,
			       CR::CoordinateType const &antCoordType,
			       blitz::Array<double,2> const &skyPositions,
			       CR::CoordinateType const &skyCoordType,
			       bool const &bufferDelays,
			       bool const &antennaIndexFirst)
    
  {
    bool status (true);

    showProgress_p = false;
    nearField_p    = true;
    
    try {
      status = setAntennaPositions (antPositions,
				    antCoordType,
				    false,
				    antennaIndexFirst);
    } catch (std::string message) {
      std::cerr << "[GeometricalDelay::GeometricalDelay] " << message << endl;
      status = false;
    }

    if (!status) {
      cerr << "-- There was an error setting the ant positions" << endl;
      // use defaults
      antPositions_p.resize(1,3);
      antPositions_p = 0.0, 0.0, 0.0;
      nofAntennas_p  = 1;
    }
    
    if (!setSkyPositions (skyPositions,false)) {
      cerr << "-- There was an error setting the sky positions" << endl;
      // use defaults
      skyPositions_p.resize(1,3);
      skyPositions_p = 0.0, 0.0, 1.0;
    }
  
    // once all the input parameters have been stored we can compute the delays
    bufferDelays_p = bufferDelays;
    if (bufferDelays_p) {
      cout << "-- buffering geometrical delays..." << endl;
      setDelays();
    }
  }
#endif
#endif
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  GeometricalDelay::~GeometricalDelay ()
  {
    destroy();
  }
  
  void GeometricalDelay::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  // ------------------------------------------------------------------ operator=

  GeometricalDelay& GeometricalDelay::operator= (GeometricalDelay const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  // ----------------------------------------------------------------------- copy
  
  void GeometricalDelay::copy (GeometricalDelay const &other)
  {
    antPositions_p.resize (other.antPositions_p.shape());
    antPositions_p = other.antPositions_p;
    
    skyPositions_p.resize (other.skyPositions_p.shape());
    skyPositions_p = other.skyPositions_p;

    nearField_p    = other.nearField_p;
    bufferDelays_p = other.bufferDelays_p;
    showProgress_p = other.showProgress_p;

    if (bufferDelays_p) {
      delays_p.resize(other.delays_p.shape());
      delays_p        = other.delays_p;
    }

    // book-keeping variables

    nofAntennas_p  = other.nofAntennas_p;
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  // -------------------------------------------------------- setAntennaPositions
  
#ifdef HAVE_CASA
  bool
  GeometricalDelay::setAntennaPositions (const Matrix<double> &antPositions,
					 bool const &bufferDelays)
  {
    bool status (true);
    IPosition shape (antPositions.shape());

    /* A final test of the contents of the array providing the antenna
       positions. */
    if (shape(0) == 3 || shape(1) == 3) {
      antPositions_p.resize (shape);
      antPositions_p = antPositions;
    } else {
      return false;
    }
    
    // handle optional buffering of delays
    if (bufferDelays) {
      bufferDelays_p = bufferDelays;
      setDelays();
    }
    
    return status;
  }
#else 
#ifdef HAVE_BLITZ
  bool
  GeometricalDelay::setAntennaPositions (const blitz::Array<double,2> &antPositions,
					 bool const &bufferDelays)
  {
    bool status (true);
    bool antennaIndexFirst (true);
    blitz::Array<int,1> shape (2);

    shape = antPositions.rows(),antPositions.cols();

    // storing input to internal area depends on the organisation of the array
    if (antennaIndexFirst) {
      // [antenna,coord]
      if (shape(1) == 3) {
	antPositions_p.resize (shape(0),shape(1));
	antPositions_p = antPositions;
	// book-keeping: number of antennas
	nofAntennas_p  = shape(0);
      } else {
	cerr << "Wrong number of array colums; must be 3!" << endl;
	status  = false;
      }
    } else {
      // [coord,antenna]
      if (shape(0) == 3) {
	int row (0);
	int col (0);
	antPositions_p.resize (shape(1),shape(0));
	for (row=0; row<shape(1); row++) {
	  for (col=0; col<shape(0); col++) {
	    antPositions_p(row,col) = antPositions(col,row);
	  }
	}
	// book-keeping: number of antennas
	nofAntennas_p  = shape(1);
      } else {
	cerr << "Wrong number of array rows; must be 3!" << endl;
	status  = false;
      }
    }

    // handle optional buffering of delays
    if (bufferDelays) {
      bufferDelays_p = bufferDelays;
      setDelays();
    }
    
    return status;
  }
#endif
#endif
  
  // -------------------------------------------------------- setAntennaPositions
  
#ifdef HAVE_CASA
  bool
  GeometricalDelay::setAntennaPositions (const Matrix<double> &antPositions,
					 CR::CoordinateType const &antCoordType,
					 bool const &antennaIndexFirst,
					 bool const &bufferDelays)
  {
    bool status (true);
    IPosition shape (antPositions.shape());

    /* If the ordering of the antenna coordinates array is transposed,
       we need to insert an additional conversion step. */
    if (antennaIndexFirst) {
      /* If the antenna coordinates are provided in cartesian coordinates
	 already, we can simply pass them along; otherwise we need to insert
	 a coordinate conversion step before storing the values. */
      if (antCoordType == CR::Cartesian) {
	return setAntennaPositions (antPositions,
				    bufferDelays);
      } else {
	// convert antenna positions to cartesian coordinates first
	Matrix<double> antPositionsCartesian (shape);
	for (int antenna(0); antenna<shape(0); antenna++) {
	  status = CR::convertVector (antPositionsCartesian(antenna,0),
				      antPositionsCartesian(antenna,1),
				      antPositionsCartesian(antenna,2),
				      CR::Cartesian,
				      antPositions(antenna,0),
				      antPositions(antenna,1),
				      antPositions(antenna,2),
				      antCoordType,
				      false);
	}
	// pass on the adjusted antenna positions
	return setAntennaPositions (antPositions,
				    bufferDelays);
      }
    } else {
      return setAntennaPositions (transpose(antPositions),
				  antCoordType,
				  true,
				  bufferDelays);
    }
    
    return status;
  }
#else
#ifdef HAVE_BLITZ
  bool GeometricalDelay::setAntennaPositions (const blitz::Array<double,2> &antPositions,
					      CR::CoordinateType const &antCoordType,
					      bool const &antennaIndexFirst,
					      bool const &bufferDelays)
  {
    bool status (true);

    return status;
  }
#endif
#endif
  
  // ------------------------------------------------------------ setSkyPositions
  
#ifdef HAVE_CASA
  bool GeometricalDelay::setSkyPositions (const Matrix<double> &skyPositions,
					  const bool &bufferDelays)
  {
    bool status (true);
    
    // check the shape of the array
    if (skyPositions.ncolumn() == 3) {
      // store the array
      skyPositions_p.resize (skyPositions.shape());
      skyPositions_p = skyPositions;
      // update the values of the geometrical delays
      if (bufferDelays) {
	bufferDelays_p = bufferDelays;
	setDelays();
      }
    } else {
      cerr << "Wrong number of array colums; must be 3!" << endl;
      status  = false;
    }
    
    return status;
  }
#else
#ifdef HAVE_BLITZ
  bool GeometricalDelay::setSkyPositions (const blitz::Array<double,2> &skyPositions,
					  const bool &bufferDelays)
  {
    bool status (true);

    // check the shape of the array
    if (skyPositions.cols() == 3) {
      // store the array
      skyPositions_p.resize (skyPositions.shape());
      skyPositions_p = skyPositions;
      // update the values of the geometrical delays
      if (bufferDelays) {
	bufferDelays_p = bufferDelays;
	setDelays();
      }
    } else {
      cerr << "Wrong number of array colums; must be 3!" << endl;
      status  = false;
    }
    
    return status;
  }
#endif
#endif

  // ------------------------------------------------------------ setSkyPositions
  
#ifdef HAVE_CASA
  bool GeometricalDelay::setSkyPositions (Matrix<double> const &skyPositions,
					  CR::CoordinateType const &coordType,
					  bool const &anglesInDegrees,
					  bool const &bufferDelays)
  {
    bool status (true);

    /*
      Check if the number of coordinates per point is correct
    */
    if (skyPositions.ncolumn() != 3) {
      cerr << "Wrong number of array colums; must be 3!" << endl;
      return false;
    }

    /*
      The whole point behind the "coordType" parameter is to enable providing
      the positions as something else but cartesian coordinates (e.g. 
      shells of a sphere in spherical coordinates).
    */

    int nofPositions (skyPositions.nrow());
    int n (0);
    
    switch (coordType) {
      case CR::Cartesian:
	skyPositions_p.resize(skyPositions.shape());
	skyPositions_p = skyPositions;
	break;
    case CR::Spherical:
      // adjust the size in the internal array storing the positions
      skyPositions_p.resize(nofPositions,3);
      // conversion of the 
      for (n=0; n<nofPositions; n++) {
	skyPositions_p.row(n) = Spherical2Cartesian (skyPositions.row(n),
						     anglesInDegrees);
      }
      break;
    case CR::Cylindrical:
      status = false;
      break;
    }

    // update the values of the geometrical delays
    if (bufferDelays) {
      bufferDelays_p = bufferDelays;
      setDelays();
    }
    
    return status;
  }
#else
#ifdef HAVE_BLITZ
  bool GeometricalDelay::setSkyPositions (blitz::Array<double,2> const &skyPositions,
					  CR::CoordinateType const &coordType,
					  bool const &anglesInDegrees,
					  bool const &bufferDelays)
  {
    bool status (true);
    
    /*
      Check if the number of coordinates per point is correct
    */
    if (skyPositions.cols() != 3) {
      cerr << "Wrong number of array colums; must be 3!" << endl;
      return false;
    }

    /*
      The whole point behind the "coordType" parameter is to enable providing
      the positions as something else but cartesian coordinates (e.g. 
      shells of a sphere in spherical coordinates).
    */

    int nofPositions (skyPositions.rows());
    int n (0);
    
    return status;
  }
#endif
#endif
  
  // ------------------------------------------------------------ setSkyPositions
  
#ifdef HAVE_CASA
  bool GeometricalDelay::setSkyPositions (Vector<double> const &xValues,
					  Vector<double> const &yValues,
					  Vector<double> const &zValues,
					  CR::CoordinateType const &coordType,
					  bool const &anglesInDegrees,
					  bool const &bufferDelays)
  {
    unsigned int nelem_x (xValues.nelements());
    unsigned int nelem_y (yValues.nelements());
    unsigned int nelem_z (zValues.nelements());
    unsigned int n  (0);
    unsigned int nx (0);
    unsigned int ny (0);
    unsigned int nz (0);
    
    // Matrix with the combined set of sky positions
    Matrix<double> positions (nelem_x*nelem_y*nelem_z,3);
    
    for (nx=0; nx<nelem_x; nx++) {
      for (ny=0; ny<nelem_y; ny++) {
	for (nz=0; nz<nelem_z; nz++) {
	  positions(n,0) = xValues(nx);
	  positions(n,1) = yValues(ny);
	  positions(n,2) = zValues(nz);
	  n++;
	}
      }
    }
    
    // forward the sky position values to have them stored internally
    return setSkyPositions (positions,
			    coordType,
			    anglesInDegrees,
			    bufferDelays);
  }
#else
#ifdef HAVE_BLITZ
  bool GeometricalDelay::setSkyPositions (blitz::Array<double,1> const &xValues,
					  blitz::Array<double,1> const &yValues,
					  blitz::Array<double,1> const &zValues,
					  CR::CoordinateType const &coordType,
					  bool const &anglesInDegrees,
					  bool const &bufferDelays)
  {
    int nelem_x (xValues.numElements());
    int nelem_y (yValues.numElements());
    int nelem_z (zValues.numElements());
    int n  (0);
    int nx (0);
    int ny (0);
    int nz (0);
    
    // Matrix with the combined set of sky positions
    blitz::Array<double,2> positions (nelem_x*nelem_y*nelem_z,3);
    
    for (nx=0; nx<nelem_x; nx++) {
      for (ny=0; ny<nelem_y; ny++) {
	for (nz=0; nz<nelem_z; nz++) {
	  positions(n,0) = xValues(nx);
	  positions(n,1) = yValues(ny);
	  positions(n,2) = zValues(nz);
	  n++;
	}
      }
    }
    
    // forward the sky position values to have them stored internally
    return setSkyPositions (positions,
			    coordType,
			    anglesInDegrees,
			    bufferDelays);
  }
#endif
#endif
  
  // ------------------------------------------------------------ setSkyPositions
  
#ifdef HAVE_CASA
  bool GeometricalDelay::setSkyPositions (Matrix<double> const &xyValues,
					  Vector<double> const &zValues,
					  Vector<int> const &axisOrder,
					  CR::CoordinateType const &coordType,
					  bool const &anglesInDegrees,
					  bool const &bufferDelays)
  {
    /*
      Check the vector providing the information about how the axes are ordered:
      - we need at least three indices
      - the indices must be in the range [0,2]
    */
    if (axisOrder.nelements() < 3) {
      std::cerr << "[GeometricalDelay::setSkyPositions] Ordering of axis incomplete!"
		<< std::endl;
      return false;
    } else if (min(axisOrder) < 0 || max(axisOrder) > 2) {
      std::cerr << "[GeometricalDelay::setSkyPositions] Index outside valid range!"
		<< std::endl;
      std::cerr << "\t" << axisOrder << std::endl;
      return false;
    }

    // local variables
    unsigned int nelem_xy (xyValues.nrow());
    unsigned int nelem_z  (zValues.nelements());
    unsigned int n (0);
    unsigned int nxy (0);
    unsigned int nz (0);

    // Matrix with the combined set of sky positions
    Matrix<double> positions (nelem_xy*nelem_z,3);

    for (nxy=0; nxy<nelem_xy; nxy++) {
      for (nz=0; nz<nelem_z; nz++) {
	positions(n,axisOrder(0)) = xyValues(nxy,0);
	positions(n,axisOrder(1)) = xyValues(nxy,1);
	positions(n,axisOrder(2)) = zValues(nz);
	n++;
      }
    }

    // forward the sky position values to have them stored internally
    return setSkyPositions (positions,
			    coordType,
			    anglesInDegrees,
			    bufferDelays);
  }
#else
#ifdef HAVE_BLITZ
  bool GeometricalDelay::setSkyPositions (blitz::Array<double,1> const &xyValues,
					  blitz::Array<double,1> const &zValues,
					  blitz::Array<int,1> const &axisOrder,
					  CR::CoordinateType const &coordType,
					  bool const &anglesInDegrees,
					  bool const &bufferDelays)
  {
    /*
      Check the vector providing the information about how the axes are ordered:
      - we need at least three indices
      - the indices must be in the range [0,2]
    */
    if (axisOrder.numElements() < 3) {
      std::cerr << "[GeometricalDelay::setSkyPositions] Ordering of axis incomplete!"
		<< std::endl;
      return false;
    } else if (min(axisOrder) < 0 || max(axisOrder) > 2) {
      std::cerr << "[GeometricalDelay::setSkyPositions] Index outside valid range!"
		<< std::endl;
      std::cerr << "\t" << axisOrder << std::endl;

      return false;
    }

    // local variables
    int nelem_xy (xyValues.rows());
    int nelem_z  (zValues.numElements());
    int n (0);
    int nxy (0);
    int nz (0);

    // Matrix with the combined set of sky positions
    blitz::Array<double,2> positions (nelem_xy*nelem_z,3);

    for (nxy=0; nxy<nelem_xy; nxy++) {
      for (nz=0; nz<nelem_z; nz++) {
	positions(n,axisOrder(0)) = xyValues(nxy,0);
	positions(n,axisOrder(1)) = xyValues(nxy,1);
	positions(n,axisOrder(2)) = zValues(nz);
	n++;
      }
    }

    // forward the sky position values to have them stored internally
    return setSkyPositions (positions,
			    coordType,
			    anglesInDegrees,
			    bufferDelays);
  }
#endif
#endif

  // ---------------------------------------------------------------------- delay

#ifdef HAVE_CASA
  Matrix<double> GeometricalDelay::delays ()
  {
    // if the delays are buffered internally, we just need to return the array
    if (bufferDelays_p) {
      cout << "-- returning buffered geometrical delays..." << endl;
      return delays_p;
    } else {
      return calcDelays();
    }
  }
#else
#ifdef HAVE_BLITZ
  blitz::Array<double,2> GeometricalDelay::delays ()
  {
    // if the delays are buffered internally, we just need to return the array
    if (bufferDelays_p) {
      cout << "-- returning buffered geometrical delays..." << endl;
      return delays_p;
    } else {
      return calcDelays();
    }
  }
#endif
#endif

  // ------------------------------------------------------------------ setDelays

  void GeometricalDelay::setDelays ()
  {
    if (bufferDelays_p) {
      delays_p.resize (nofAntennas_p,nofSkyPositions());
      delays_p = calcDelays();
    }
  }

  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  void GeometricalDelay::summary (std::ostream &os)
  {
    os << "[GeometricalDelay] Summary of object" << std::endl;
    os << "-- Sky positions       : " << skyPositions_p.shape() << std::endl;
    os << "-- Antenna positions   : " << antPositions_p.shape() << std::endl;
    os << "-- nof. Baselines      : " << nofBaselines()         << std::endl;
    os << "-- near field geometry : " << nearField_p            << std::endl;
    os << "-- buffer delays       : " << bufferDelays_p         << std::endl;
    
    if (bufferDelays_p) {
      os << "-- Delays            : " << delays_p.shape()     << std::endl;
    }
  }

  // ------------------------------------------------------------------ calcDelay

#ifdef HAVE_CASA
  double GeometricalDelay::calcDelay (double const &xSky,
				      double const &ySky,
				      double const &zSky,
				      double const &xAntenna,
				      double const &yAntenna,
				      double const &zAntenna)
  {
    double delay (0.0);
    Vector<double> skyPosition(3);
    Vector<double> diff (3);

    skyPosition(0) = xSky;
    skyPosition(1) = ySky;
    skyPosition(2) = zSky;

    diff(0) = xSky-xAntenna;
    diff(1) = ySky-yAntenna;
    diff(2) = zSky-zAntenna;

    /*
      At this position we now need to distinguish between considering a source
      in the near-field or in the far field. For a source in the far-field we 
      can use the simplified calculation, whereas for the near-field we need
      to carry out the calculation using the proper 3-dim geometry.
     */
    if (nearField_p) {
      delay = (CR::L2Norm(diff)-CR::L2Norm(skyPosition))/lightspeed;
    } else {
      delay = -(xSky*xAntenna+ySky*yAntenna+zSky*zAntenna)/lightspeed;
    }
      
    return delay;
  }
#else
#ifdef HAVE_BLITZ
  double GeometricalDelay::calcDelay (double const &xSky,
				      double const &ySky,
				      double const &zSky,
				      double const &xAntenna,
				      double const &yAntenna,
				      double const &zAntenna)
  {
    double delay (0.0);
    blitz::Array<double,1> skyPosition(3);
    blitz::Array<double,1> diff (3);

    skyPosition = xSky,ySky,zSky;
    diff        = xSky-xAntenna,ySky-yAntenna,zSky-zAntenna;
    
    delay = (CR::L2Norm(diff)-CR::L2Norm(skyPosition))/lightspeed;
    
    return delay;
  }
#endif
#endif

  // ----------------------------------------------------------------- calcDelays

#ifdef HAVE_CASA
  Matrix<double> GeometricalDelay::calcDelays ()
  {
    uint nAnt (0);
    int nSky (0);
    int nofSky (nofSkyPositions());
    Vector<double> skyPos (3);
    Vector<double> diff (3);
    Matrix<double> delays (nofAntennas_p,nofSky);

    // computation of the geometrical delays
    for (nSky=0; nSky<nofSky; nSky++) {
      skyPos = skyPositions_p.row(nSky);
      for (nAnt=0; nAnt<nofAntennas_p; nAnt++) {
 	diff = skyPos - antPositions_p.row(nAnt);
 	delays(nAnt,nSky) = (L2Norm(diff)-L2Norm(skyPos))/lightspeed;
      }
    }
    
    return delays;
  }
#else
#ifdef HAVE_BLITZ
  blitz::Array<double,2> GeometricalDelay::calcDelays ()
  {
    int nAnt (0);
    int nSky (0);
    int nofSky (nofSkyPositions());
    blitz::Array<double,1> skyPos (3);
    blitz::Array<double,1> diff (3);
    blitz::Array<double,2> delays (nofAntennas_p,nofSky);

    // computation of the geometrical delays
    for (nSky=0; nSky<nofSky; nSky++) {
      skyPos = skyPositions_p(nSky,blitz::Range(blitz::Range::all()));
      for (nAnt=0; nAnt<nofAntennas_p; nAnt++) {
	diff = skyPos-antPositions_p(nAnt,blitz::Range(blitz::Range::all()));
	delays(nAnt,nSky) = (CR::L2Norm(diff)-CR::L2Norm(skyPos))/lightspeed;
      }
    }
    
    return delays;
  }
#endif
#endif
  
} // NAMESPACE CR -- END
