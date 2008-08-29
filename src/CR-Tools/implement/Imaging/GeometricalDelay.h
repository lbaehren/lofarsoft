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

#ifndef GEOMETRICALDELAY_H
#define GEOMETRICALDELAY_H

#include <crtools.h>

#include <string>

#ifdef HAVE_CASA
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/Arrays/Vector.h>
using casa::IPosition;
using casa::Matrix;
using casa::Vector;
#else
#ifdef HAVE_BLITZ
#include <blitz/array.h>
#endif
#endif

#include <Math/VectorConversion.h>
#include <Math/VectorNorms.h>

namespace CR { // NAMESPACE CR -- BEGIN
  
  /*!
    \class GeometricalDelay
    
    \ingroup CR_Imaging
    
    \brief Geometrical delay for a set of antenna and source positions
    
    \author Lars B&auml;hren

    \date 2007/01/15

    \test tGeometricalDelay.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>S. Wijnholds (2006) LOFAR station calibration
    </ul>
    
    <h3>Synopsis</h3>

    <ul>
      <li><a href="#Geometry">Geometry</a>
      <li><a href="#Array classes">Array classes</a>
      <li><a href="#Past implementation">Past implementation</a>
    </ul>

    <b><a name="Geometry">Geometry</a>.</b>
    The basic equation, for which the delays are computed, is documented in
    the GeometricalWeight class. The procedure to arrive at the geometrical
    weights passed to the Beamformer is divided into three steps, of which this
    class implements the first one.

    For a sky position \f$ \vec\rho \f$ and antenna positions \f$ \vec x_j \f$
    and \f$ \vec x_j \f$ the geometrical delay \f$ \tau_{ij} \f$ is given by
    \f[
      \tau_{ij} = \frac{1}{c} \left( |\vec \rho_j| - |\vec \rho_i| \right)
      = \frac{1}{c} \left( |\vec \rho - \vec x_j| - |\vec \rho - \vec x_i| \right)
    \f]

    Though being geometrically correct, the above expression is suitable for
    handling of baselines, rather than for the consideration of individual
    antennas (though their positions of course can be considered as a baseline
    w.r.t. to the array phase center). For an antenna located at \f$ \vec x_j \f$
    the source position \f$ \vec \rho \f$ appears at \f$ \vec \rho_j \f$ such
    the signals arrives with a delay of
    \f[ \tau_j = \frac{1}{c} \left( |\vec \rho_j| - |\vec \rho| \right) =
    \frac{1}{c} \left( |\vec \rho - \vec x_j| - |\vec \rho| \right) \f]

    <b><a name="Array classes">Array classes</a>.</b>
    The class is constructed such, that both Blitz++ and CASA arrays are
    supported. However in order to minimize conversions, only one type is
    available at a time; this is obtained by encapsulating the array type 
    specific parts (including also private variables) in preprocessor statements:
    \code
    #ifdef HAVE_CASA
    // code working with the CASA array classes
    #else 
    #ifdef HAVE_BLITZ
    // code working with the Blitz++ array classes
    #endif
    #endif
    \endcode

    <b><a name="Past implementation">Past implementation</a>.</b>
    The first implementation of the routines for thhe computation of the
    geometrical delay and the subsequent computation of beamformer weights was
    handled in Phasing:
    \code
    const Double c = QC::c.getValue();
    Vector<Double> rho = CR::azel2cartesian (azel);
    Double delay;
    
    if (nearField && azel.nelements() == 3) {
      Vector<Double> diff = rho-baseline;
      delay = (CR::L2Norm(diff)-CR::L2Norm(rho))/c;
    } else {
      Vector<Double> product = rho*baseline;
      delay = -sum(product)/c;
    }
    
    return delay;
    \endcode

    \image html GeometricalDelay.png
    
    <h3>Example(s)</h3>
    
  */  
  class GeometricalDelay {

    //! Are the buffered values in sync? 
    bool bufferInSync_p;
    
  protected:
    
#ifdef HAVE_CASA
    //! [nofAntennas,3] Antenna positions for which the delay is computed
    Matrix<double> antPositions_p;
    //! [nofSkyPositions,3] Positions in the sky towards which to point
    Matrix<double> skyPositions_p;
    //! [nofAntennas,nofSkyPositions] The geometrical delay itself
    Matrix<double> delays_p;
#else
#ifdef HAVE_BLITZ
    //! [nofAntennas,3] Antenna positions for which the delay is computed
    blitz::Array<double,2> antPositions_p;
    //! [nofSkyPositions,3] Positions in the sky towards which to point
    blitz::Array<double,2> skyPositions_p;
    //! [nofAntennas,nofSkyPositions] The geometrical delay itself
    blitz::Array<double,2> delays_p;
#endif
#endif

    //! Geometrical constraint for the beamforming geometry: far-/near-field
    bool nearField_p;
    //! Book-keeping: number of antennas
    uint nofAntennas_p;
    //! Buffer the values for the geometrical delay?
    bool bufferDelays_p;
    // Show progress during computations?
    bool showProgress_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
      
      Geometrical delay for a single antenna position at the
      position of the phase center, \f$ \vec x = (0,0,0) \f$ and a single 
      source position at unit distance above the antenna, \f$ \vec \rho =
      (0,0,1) \f$
    */
    GeometricalDelay ();
    
    /*!
      \brief Argumented constructor

      If an error is encountered processing the input parameters, we revert to
      the individual default settings used with the default constructor; at least
      this way we end up with an operational object.

      \param antPositions -- [nofAntennas,3] Antenna positions for which the
                             delay is computed, \f$ (x,y,z) \f$
      \param skyPositions -- [nofSkyPositions,3] Positions in the sky towards
                             which to point, given in the same reference frame
			     as the antenna positions, \f$ (x,y,z) \f$
    */
#ifdef HAVE_CASA
    GeometricalDelay (Matrix<double> const &antPositions,
		      Matrix<double> const &skyPositions);
#else 
#ifdef HAVE_BLITZ
    GeometricalDelay (blitz::Array<double,2> const &antPositions,
		      blitz::Array<double,2> const &skyPositions);
#endif
#endif

    /*!
      \brief Argumented constructor

      If an error is encountered processing the input parameters, we revert to
      the individual default settings used with the default constructor; at least
      this way we end up with an operational object.

      \param antPositions -- [nofAntennas,3] Antenna positions for which the
                             delay is computed
      \param antCoordType -- CR::CoordinateType of the antenna position
                             coordinates; if the coordinates are non-cartesian
			     and thereby include anglular components, the values
			     must be provided in radians.
      \param skyPositions -- [nofSkyPositions,3] Positions in the sky towards
                             which to point, given in the same reference frame
			     as the antenna positions, though not necessarily
			     using the same type of coordinates (e.g. might be
			     given in spherical coordinates).
      \param skyCoordType -- CR::CoordinateType of the sky position coordinates
    */
#ifdef HAVE_CASA
    GeometricalDelay (Matrix<double> const &antPositions,
		      CR::CoordinateTypes::Type const &antCoordType,
		      Matrix<double> const &skyPositions,
		      CR::CoordinateTypes::Type const &skyCoordType);
#else 
#ifdef HAVE_BLITZ
    GeometricalDelay (blitz::Array<double,2> const &antPositions,
		      CR::CoordinateTypes::Type const &antCoordType,
		      blitz::Array<double,2> const &skyPositions,
		      CR::CoordinateTypes::Type const &skyCoordType);
#endif
#endif

    /*!
      \brief Argumented constructor

      If an error is encountered processing the input parameters, we revert to
      the individual default settings used with the default constructor; at least
      this way we end up with an operational object.

      \param antPositions -- Antenna positions for which the delay is computed.
             The organization of the matrix providing the coordinates can be
	     organized in two different ways:
	     - antenna index first = [antenna,3]
	     - antenna index last  = [3,antenna]
      \param antCoordType -- CR::CoordinateTypes::Type of the antenna position
             coordinates
      \param skyPositions -- [nofSkyPositions,3] Positions in the sky towards
             which to point, given in the same reference frame as the antenna
	     positions, \f$ (x,y,z) \f$
      \param antCoordType -- CR::CoordinateTypes::Type of the sky position
             coordinates
      \param bufferDelays -- Buffer the values for the geometrical delay? If set
             <i>yes</i> the delays will be computed from the provided antenna
	     and sky positions and afterwards kept in memory; if set <i>false</i>
	     only the input parameters are stored and no further action is taken.
      \param antennaIndexFirst -- Is the antenna index first in the matrix with
             antenna positions? 
	     - <tt>antennaIndexFirst=true</tt> -> shape(antPositions)=[antenna,3]
	     - <tt>antennaIndexFirst=false</tt> -> shape(antPositions)=[3,antenna]
    */
#ifdef HAVE_CASA
    GeometricalDelay (Matrix<double> const &antPositions,
		      CR::CoordinateTypes::Type const &antCoordType,
		      Matrix<double> const &skyPositions,
		      CR::CoordinateTypes::Type const &skyCoordType,
		      bool const &bufferDelays,
		      bool const &antennaIndexFirst);
#else 
#ifdef HAVE_BLITZ
    GeometricalDelay (blitz::Array<double,2> const &antPositions,
		      CR::CoordinateTypes::Type const &antCoordType,
		      blitz::Array<double,2> const &skyPositions,
		      CR::CoordinateTypes::Type const &skyCoordType,
		      bool const &bufferDelays,
		      bool const &antennaIndexFirst);
#endif
#endif

    /*!
      \brief Copy constructor
      
      \param other -- Another GeometricalDelay object from which to create this
                      new one.
    */
    GeometricalDelay (GeometricalDelay const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~GeometricalDelay ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another GeometricalDelay object from which to make a copy.
    */
    GeometricalDelay& operator= (GeometricalDelay const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Are we using geometrical settings for near-field beamforming?

      \return nearField -- If <tt>true</tt> the geometrical delays are computed
              using the proper relation for a source in the near-field; if
	      <tt>false</tt> the far-field approximation is used.
    */
    inline bool nearField () const {
      return nearField_p;
    };

    /*!
      \brief Are we going to use geometrical settings for near-field beamforming?

      \param nearField    -- If <tt>true</tt> the geometrical delays are computed
                             using the proper relation for a source in the
			     near-field; if <tt>false</tt> the far-field
			     approximation is used.
      \return bufferDelays -- Are the values for the geometrical delay buffered?
              If set <i>yes</i> the delays will be computed from the provided
	      antenna and sky positions and afterwards kept in memory; if set
	      <i>no</i> only the input parameters are stored an no further
	      action is taken.
    */
    inline void setNearField (bool const &nearField=true,
			      bool const &bufferDelays=false) {
      nearField_p    = nearField;
      bufferDelays_p = bufferDelays;
      if (bufferDelays) {
	setDelays();
      }
    }

    /*!
      \brief Are the values for the geometrical delay buffered?
      
      \return bufferDelays -- Are the values for the geometrical delay buffered?
              If set <i>yes</i> the delays will be computed from the provided
	      antenna and sky positions and afterwards kept in memory; if set
	      <i>no</i> only the input parameters are stored an no further
	      action is taken.
    */
    inline bool bufferDelays () const {
      return bufferDelays_p;
    }
    
    /*!
      \brief Enable/disable buffering of the values for the geometrical delay

      \return bufferDelays -- Are the values for the geometrical delay buffered?
              If set <i>yes</i> the delays will be computed from the provided
	      antenna and sky positions and afterwards kept in memory; if set
	      <i>no</i> only the input parameters are stored an no further
	      action is taken.
    */
    inline void bufferDelays (bool const &bufferDelays=false) {
      bufferDelays_p = bufferDelays;
      if (bufferDelays) {
	setDelays();
      }
    }

    /*!
      \brief Is the progress during the computation shown?

      \return showProgress -- If set to <tt>true</tt> the progres during the
              main computations is shown via a progress bar in the terminal.
    */
    inline bool showProgress () const {
      return showProgress_p;
    }

    /*!
      \brief Show the progress during the computation?

      \param showProgress -- If set to <tt>true</tt> the progres during the
             main computations is shown via a progress bar in the terminal
    */
    inline void showProgress (bool const &showProgress=false) {
      showProgress_p = showProgress;
    }
    
    /*!
      \brief Get the antenna positions, for which the delay is computed

      \return antPositions -- Antenna positions for which the delays are computed,
                              [nofAntennas,3]
    */
#ifdef HAVE_CASA
    inline Matrix<double> antennaPositions () const {
      return antPositions_p;
    }
#else
#ifdef HAVE_BLITZ
    inline blitz::Array<double,2> antennaPositions () const {
      return antPositions_p;
    }
#endif
#endif
    
    /*!
      \brief Set the antenna positions, for which the delay is computed
      
      Internally assigned variables:
      - antCoordType = CR::Cartesian
      - antennaIndexFirst = true

      \param antPositions -- The antenna positions, for which the delay is
                             computed, [nofAntennas,3]
      \param bufferDelays -- Buffer the values for the geometrical delay? If set
             <i>true</i> the delays will be computed from the provided antenna
	     and sky positions and afterwards kept in memory; if set <i>false</i>
	     only the input parameters are stored an no further action is taken.
    */
#ifdef HAVE_CASA
    bool setAntennaPositions (const Matrix<double> &antPositions,
			      bool const &bufferDelays=false);
#else
#ifdef HAVE_BLITZ
    bool setAntennaPositions (const blitz::Array<double,2> &antPositions,
			      const bool &bufferDelays=false);
#endif
#endif
    
    /*!
      \brief Set the antenna positions for which the delay is computed

      \param antPositions -- The antenna positions for which the delay is
             computed
      \param antennaIndexFirst -- Is the antenna index first in the matrix with
             antenna positions? 
	     - <tt>antennaIndexFirst=true</tt> -> shape(antPositions)=[antenna,3]
	     - <tt>antennaIndexFirst=false</tt> -> shape(antPositions)=[3,antenna]
      \param antCoordType -- CR::CoordinateTypes::Type of the antenna position
             coordinates
      \param anglesInDegrees -- If the coordinates are non-Cartesian, are the 
             angles given in degrees? If <tt>false</tt> the angles are considered
	     to be provided in radians.
      \param bufferDelays -- Buffer the values for the geometrical delay? If set
             <i>yes</i> the delays will be computed from the provided antenna
	     and sky positions and afterwards kept in memory; if set <i>false</i>
	     only the input parameters are stored and no further action is taken.
    */
#ifdef HAVE_CASA
    bool setAntennaPositions (const Matrix<double> &antPositions,
			      bool const &antennaIndexFirst,
			      CR::CoordinateTypes::Type const &antCoordType,
			      bool const &anglesInDegrees=false,
			      bool const &bufferDelays=false);
#else
#ifdef HAVE_BLITZ
    bool setAntennaPositions (const blitz::Array<double,2> &antPositions,
			      bool const &antennaIndexFirst,
			      CR::CoordinateTypes::Type const &antCoordType,
			      bool const &anglesInDegrees=false,
			      bool const &bufferDelays=false);
#endif
#endif
    
    /*!
      \brief Get the sky positions, for which the delay is computed

      \return skyPosition -- The positions on the sky, for which the delay is
              computed, [nofSkyPositions,3].
    */
#ifdef HAVE_CASA
    inline Matrix<double> skyPositions () const {
      return skyPositions_p;
    }
#else
#ifdef HAVE_BLITZ
    inline blitz::Array<double,2> skyPositions () const {
      return skyPositions_p;
    }
#endif
#endif

    /*!
      \brief Set the sky positions, for which the delay is computed
      
      \param skyPositions -- The source positions, for which the delay is
             computed, [nofSkyPositions,3]; coordinates must be <b>cartesian</b>
	     since no further conversion is appplied.
      \param bufferDelays  -- Buffer the values for the geometrical delay? If set
             <i>yes</i> the delays will be computed from the provided antenna and
	     sky positions and afterwards kept in memory; if set <i>no</i> only
	     the input parameters are stored an no further action is taken.
    */
#ifdef HAVE_CASA
    bool setSkyPositions (const Matrix<double> &skyPositions,
			  const bool &bufferDelays=false);
#else
#ifdef HAVE_BLITZ
    bool setSkyPositions (const blitz::Array<double,2> &skyPositions,
			  const bool &bufferDelays=false);
#endif
#endif

    /*!
      \brief Set the sky positions, for which the delay is computed
      
      \param skyPositions -- [nofSources,3], The sky positions, for which the
             delay is computed
      \param coordType    -- Coordinate type, as which the position is provided;
             if position not provided in cartesian coordinates, transformation
	     will be performed
      \param bufferDelays  -- Buffer the values for the geometrical delay? If set
             <i>yes</i> the delays will be computed from the provided antenna and
	     sky positions and afterwards kept in memory; if set <i>no</i> only
	     the input parameters are stored an no further action is taken.
    */
#ifdef HAVE_CASA
    bool setSkyPositions (Matrix<double> const &skyPositions,
			  CR::CoordinateTypes::Type const &coordType,
			  bool const &anglesInDegrees=false,
			  bool const &bufferDelays=false);
#else
#ifdef HAVE_BLITZ
    bool setSkyPositions (blitz::Array<double,2> const &skyPositions,
			  CR::CoordinateTypes::Type const &coordType,
			  bool const &anglesInDegrees=false,
			  bool const &bufferDelays=false);
#endif
#endif
    
    /*!
      \brief Set the sky positions, for which the delay is computed
      
      \param xValues         -- 
      \param yValues         -- 
      \param zValues         -- 
      \param coordType       -- Coordinate type, as which the position is
             provided; if position not provided in cartesian coordinates,
	     transformation will be performed
      \param anglesInDegrees -- 
      \param bufferDelays  -- Buffer the values for the geometrical delay? If set
             <i>yes</i> the delays will be computed from the provided antenna and
	     sky positions and afterwards kept in memory; if set <i>no</i> only
	     the input parameters are stored an no further action is taken.
      
      \return status -- Success or failure of the operation; returns <t>false</tt>
              if an error was encountered
    */
#ifdef HAVE_CASA
    bool setSkyPositions (Vector<double> const &xValues,
			  Vector<double> const &yValues,
			  Vector<double> const &zValues,
			  CR::CoordinateTypes::Type const &coordType,
			  bool const &anglesInDegrees=false,
			  bool const &bufferDelays=false);
#else
#ifdef HAVE_BLITZ
    bool setSkyPositions (blitz::Array<double,1> const &xValues,
			  blitz::Array<double,1> const &yValues,
			  blitz::Array<double,1> const &zValues,
			  CR::CoordinateTypes::Type const &coordType,
			  bool const &anglesInDegrees=false,
			  bool const &bufferDelays=false);
#endif
#endif
    
    /*!
      \brief Set the sky positions, for which the delay is computed

      A possible arrangement for the provided coordinates could be e.g.
      <tt>axisOrder=[0,1,2]</tt>:
      \f[
        \mathbf M_{xy} = \left[ \begin{array}{cc} x_1 & y_1 \\ x_2 & y_2 \\
	                 \vdots & \vdots \\ x_N & y_N \end{array} \right]
	\quad , \qquad
	\mathbf z = [ z_1, z_2, ..., z_M]^T
      \f]

      \param xyValues        -- 
      \param zValues         -- 
      \param axisOrder       -- The ordering of the coordinate axis in the
                                arrays provided beforehand (see note above).
      \param coordType       -- 
      \param anglesInDegrees -- Are the angles given in degree? If set to
                                <tt>false</tt> the angles are considered in 
				radian.
      \param bufferDelays    -- Buffer the values for the geometrical delay? If
                                set <i>true</i> the delays will be computed and
				afterwards kept in memory; if set <i>false</i>
				only the input parameters are stored an no
				further action is taken.
      
      \return status -- Success or failure of the operation; returns
                        <t>false</tt> if an error was encountered
    */
#ifdef HAVE_CASA
    bool setSkyPositions (Matrix<double> const &xyValues,
			  Vector<double> const &zValues,
			  Vector<int> const &axisOrder,
			  CR::CoordinateTypes::Type const &coordType,
			  bool const &anglesInDegrees=false,
			  bool const &bufferDelays=false);
#else
#ifdef HAVE_BLITZ
    bool setSkyPositions (blitz::Array<double,1> const &xyValues,
			  blitz::Array<double,1> const &zValues,
			  blitz::Array<int,1> const &axisOrder,
			  CR::CoordinateTypes::Type const &coordType,
			  bool const &anglesInDegrees=false,
			  bool const &bufferDelays=false);
#endif
#endif
    
    /*!
      \brief Set an individual sky positions

      Whereas of course the more general case is that we are dealing with multiple
      pointing directions, there are also cases where on want a single direction;
      this method is geared toward exactly such a case.

      \param x1 -- First component of the position coordinate
      \param x2 -- Second component of the position coordinate
      \param x3 -- Third component of the position coordinate
      \param coordType -- Coordinate type, as which the position is provided;
             if position not provided in cartesian coordinates, transformation
	     will be performed
      \param anglesInDegrees -- If the coordinates contain angular components,
             these given in degrees?
      \param bufferDelays -- Buffer the values for the geometrical delay? If set
             <i>yes</i> the delays will be computed from the provided antenna and
	     sky positions and afterwards kept in memory; if set <i>no</i> only
	     the input parameters are stored an no further action is taken.

      \return status -- Success or failure of the operation; returns
              <t>false</tt> if an error was encountered
    */
#ifdef HAVE_CASA
    bool setSkyPosition (double const &x1,
			 double const &x2,
			 double const &x3,
			 CR::CoordinateTypes::Type const &coordType,
			 bool const &anglesInDegrees=false,
			 bool const &bufferDelays=false);
#else
#ifdef HAVE_BLITZ
    bool setSkyPosition (blitz::Array<double,2> const &skyPositions,
			 CR::CoordinateTypes::Type const &coordType,
			 bool const &anglesInDegrees=false,
			 bool const &bufferDelays=false);
#endif
#endif

    /*!
      \brief Get the values of the geometrical delay

      \return delay -- [nofAntennas,nofSkyPositions] The geometrical delays for
                       for the combination of antenna and sky positions
    */
#ifdef HAVE_CASA
    Matrix<double> delays ();
#else
#ifdef HAVE_BLITZ
    blitz::Array<double,2> delays ();
#endif
#endif

    /*!
      \brief Geometrical delay for a combination of antenna and sky positions

      \param antPositions -- The antPositions, for which the delay is computed;
                             values are given as [nofAntenas,nofCoordinates] with
			     the antenna positions given in cartesian coordinates
			     \f$ (x,y,z) \f$
      \param skyPositions -- The sky positions, for which the delay is computed
      \param bufferDelays  -- Buffer the values for the geometrical delay? If set
                             <i>yes</i> the delays will be computed from the 
			     provided antenna and sky positions and afterwards
			     kept in memory; if set <i>no</i> only the input 
			     parameters are stored an no further action is taken.

      \return delay -- [nofAntennas,nofSkyPositions] The geometrical delays for
                       for the combination of antenna and sky positions
    */
#ifdef HAVE_CASA
    Matrix<double> delays (const Matrix<double> &antPositions,
			   const Matrix<double> &skyPositions,
			   const bool &bufferDelays=false)
      {
	bufferDelays_p = bufferDelays;
	antPositions_p = antPositions;
	skyPositions_p = skyPositions;
	return delays ();
      }
#else
#ifdef HAVE_BLITZ
    blitz::Array<double,2> delays (const blitz::Array<double,2> &antPositions,
				   const blitz::Array<double,2> &skyPositions,
				   const bool &bufferDelays=false)
      {
	bufferDelays_p = bufferDelays;
	antPositions_p = antPositions;
	skyPositions_p = skyPositions;
	return delays ();
      }
#endif
#endif

#ifdef HAVE_CASA
    /*!
      \brief Get the shape of the array storing the geometrical weights
      
      \return shape -- [nofAntennas,nofSkyPositions] The shape of the array
              storing the values of the geometrical weights
    */
    inline casa::IPosition shape () {
      return casa::IPosition(2,nofAntennaPositions(),nofSkyPositions());
    }
#endif
    
    /*!
      \brief Get the number of baselines
      
      \return nofBaselines -- The number of baselines, \f$ N_{\rm Baselines} \f$,
                              which can be computed from the antenna positions:
			      \f$ N_{\rm Baselines} = \frac{1}{2} (N^2-N)
			      = \frac{N}{2} (N-1) \f$
    */
#ifdef HAVE_CASA
    inline int nofBaselines () {
      return antPositions_p.nrow()*(antPositions_p.nrow()-1)/2;
    }
#else
#ifdef HAVE_BLITZ
    inline int nofBaselines () {
      return antPositions_p.rows()*(antPositions_p.rows()-1)/2;
    }
#endif
#endif

    /*!
      \brief Get the number of antenna positions

      \return nofAntennaPositions -- The number of antenna positions, for which
                                     the geometrical delay is computed.
    */
    inline uint nofAntennaPositions () const {
      return nofAntennas_p;
    }
    /*!
      \brief Get the number of antenna positions

      \return nofAntennas -- The number of antenna positions, for which the
                             geometrical delay is computed.
    */
    inline uint nofAntennas () const {
      return nofAntennas_p;
    }
    
    /*!
      \brief Get the number of (source) positions

      \return nofPositions -- The number of (source) positions, which which the
                              delays are computed; corresponds to the number of
			      elements along the first axis of the
			      <tt>positions</tt> array.
    */
#ifdef HAVE_CASA
    inline int nofSkyPositions () const {
      return skyPositions_p.nrow();
    }
#else
#ifdef HAVE_BLITZ
    inline int nofSkyPositions () const {
      return skyPositions_p.rows();
    }
#endif
#endif
    
    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Get the value of the geometrical delay

      \param indexAntenna -- Index for selection from the set of antenna
             positions.
      \param indexSky     -- Index for selection from the set of sky positions.

      \return delay -- The geometrical delay for a given combination of antenna
              and sky position
    */
#ifdef HAVE_CASA
    double delay (int const &indexAntenna,
		  int const &indexSky);
#endif
    
    /*
      \brief Compute an individual delay
    */
    double delay (double const &xAntenna,
		  double const &yAntenna,
		  double const &zAntenna,
		  double const &xSky,
		  double const &ySky,
		  double const &zSky);
    
    /*
      \brief Compute an individual delay
      
      \param skyPosition     -- 
      \param antennaPosition -- 
      
      \return delay -- Geometrical delay \f$ \tau \f$
    */
#ifdef HAVE_CASA
    double delay (casa::Vector<double> const &antennaPosition,
		  casa::Vector<double> const &skyPosition);
#endif
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, GeometricalDelay.
    */
    inline std::string className () const {
      return "GeometricalDelay";
    }
    
    /*!
      \brief Provide a summary of the objects status and contents
    */
    inline void summary () {
      summary (std::cout);
    }
    
    /*!
      \brief Provide a summary of the objects status and contents

      \param os -- Output stream to which the summary is written
    */
    void summary (std::ostream &os);
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (GeometricalDelay const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);

    /*!
      \brief Store the geometrical delays in the internal buffer
    */
    void setDelays ();

#ifdef HAVE_CASA
    /*!
      \param antPositions -- [nofAntennas,3] Antenna positions for which the
             delay is computed, \f$ (x,y,z) \f$
      \param antCoordType -- CR::CoordinateTypes::Type of the antenna position
             coordinates
      \param skyPositions -- [nofSkyPositions,3] Positions in the sky towards
             which to point, given in the same reference frame as the antenna
	     positions, though not necessarily using the same type of coordinates
	     (e.g. might be given in spherical coordinates).
      \param skyCoordType -- CR::CoordinateTypes::Type of the sky position coordinates
      \param bufferDelays -- Buffer the values for the geometrical delay? If
             set <i>true</i> the delays will be computed and afterwards kept in
	     memory; if set <i>false</i> only the input parameters are stored and
	     no further action is taken.
    */
    void init (Matrix<double> const &antPositions,
	       CR::CoordinateTypes::Type const &antCoordType,
	       Matrix<double> const &skyPositions,
	       CR::CoordinateTypes::Type const &skyCoordType,
	       bool const &bufferDelays=false,
	       bool const &antennaIndexFirst=true);
#else
#ifdef HAVE_BLITZ
    void init (blitz::Array<double,2> const &antPositions,
	       CR::CoordinateTypes::Type const &antCoordType,
	       blitz::Array<double,2> const &skyPositions,
	       CR::CoordinateTypes::Type const &skyCoordType,
	       bool const &bufferDelays=false,
	       bool const &antennaIndexFirst=true);
#endif
#endif
    
    /*!
      \brief Compute the geometrical delays

      \return delays -- The geometrical delays, [nofAntennas,nofPositions]
    */
#ifdef HAVE_CASA
    Matrix<double> calcDelays ();
#else
#ifdef HAVE_BLITZ
    blitz::Array<double,2> calcDelays ();
#endif
#endif

  };  // CLASS GEOMETRICALDELAY -- END
  
}  // NAMESPACE CR -- END

#endif /* GEOMETRICALDELAY_H */
