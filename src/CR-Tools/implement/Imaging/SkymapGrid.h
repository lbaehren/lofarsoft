/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

#ifndef SKYMAPGRID_H
#define SKYMAPGRID_H

#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/OS/Time.h>
#include <casa/Quanta/MVTime.h>
#include <coordinates/Coordinates/ObsInfo.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <measures/Measures.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasRef.h>
#include <measures/Measures/MeasTable.h>

#include <Observation/ObservationData.h>

#include <casa/namespace.h>

namespace CR {  // Namespace CR -- begin

/*!
  \class SkymapGrid

  \ingroup Imaging

  \brief (Regular) Coordinate grids for the creation of a Skymap.

  \author Lars B&auml;hren

  \test tSkymapGrid.cc

  \attention This class is soon -- in combination with ObservationData -- 
  to replace ObservationFrame, CoordinatesWCS and SkymapGrids.

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>"Representations of world coordinates in FITS", Greisen, E.W., &
    Calabretta, M.R. 2002, A&A, 395, 1061 (paper I)
    <li>"Representations of celestial coordinates in FITS", Calabretta, M.R., &
    Greisen, E.W. 2002, A&A, 395, 1077 (paper II)

    <li>[AIPS++]
        <a href="http://aips2.nrao.edu/docs/measures/implement/Measures.html">Measures</a>
	-- Physical quantities within a certain reference frame.
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/measures/implement/Measures/MDirection.html">MDirection</a>
         -- A Measure: astronomical direction.
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/trial/implement/Coordinates/ObsInfo.html">ObsInfo</a>
        -- Store miscellaneous information related to an observation.
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/coordinates/implement/Coordinates/Projection.html">Projection</a>
        -- Geometric parameters needed for a sky projection to a plane
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/trial/implement/Coordinates/DirectionCoordinate.html">DirectionCoordinate</a>
        -- Interconvert pixel positions and directions
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/coordinates/implement/Coordinates/CoordinateSystem.html">CoordinateSystem</a>
        -- Interconvert pixel and world coordinates

    <li>[LOPES-Tools] ObservationData -- Store information describing an
        observation.
  </ul>

  <h3>Synopsis</h3>

  <h4>(a) Coordinate system reference codes</h4>

  Listed below are the supported coordinate systems, from which the Skymap
  coordinate grid -- as used for the Beamformer -- can be derived.

  <table cellpadding="3" border="0" width="90%">
    <tr>
      <td width="15%" class="indexkey">Reference code</td>
      <td width="75%" class="indexkey">Description</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">J2000</td>
      <td>mean equator and equinox at J2000.0</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">JNAT</td>
      <td>geocentric natural frame</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">JMEAN</td>
      <td>mean equator and equinox at frame epoch</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">JTRUE</td>
      <td>true equator and equinox at frame epoch</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">APP</td>
      <td>apparent geocentric position</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">B1950</td>
      <td>mean epoch and ecliptic at B1950.0</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">BMEAN</td>
      <td>mean equator and equinox at frame epoch</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">BTRUE</td>
      <td>true equator and equinox at frame epoch</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">GALACTIC</td>
      <td>galactic coordinates</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">HADEC</td>
      <td>topocentric HA and declination</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">AZEL</td>
      <td>topocentric Azimuth and Elevation (N through E)</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">AZELSW</td>
      <td>topocentric Azimuth and Elevation (S through W)</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">AZELNE</td>
      <td>topocentric Azimuth and Elevation (N through E)</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">AZELGEO</td>
      <td>geodetic Azimuth and Elevation (N through E)</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">AZELSWGEO</td>
      <td>geodetic Azimuth and Elevation (S through W)</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">AZELNEGEO</td>
      <td>geodetic Azimuth and Elevation (N through E)</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">ECLIPTC</td>
      <td>ecliptic for J2000 equator and equinox</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">MECLIPTIC</td>
      <td>ecliptic for mean equator of date</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">TECLIPTIC</td>
      <td>ecliptic for true equator of date</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">SUPERGAL</td>
      <td>supergalactic coordinates</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">ITRF</td>
      <td>coordinates wrt ITRF Earth frame</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">TOPO</td>
      <td>apparent topocentric position</td>
    </tr>
  </table>

  <h4>(b) Customization of the skymap</h4>

  <ul type="square">
    <li>While all maps have a default orientation -- i.e. a choice for where is
    north and where is east  -- we'd like to be able to provide our own choice.
    Such choice can be implemented via adjusting the CDELT keyword of the 
    coordinate system, thus changing the the linear transformation between
    pixel and intermediate world coordinates.
  </ul>
*/

class SkymapGrid {

  //! Switch between verbose/non-verbose mode
  Bool verbose_p;

  //! Reference code for the celestial coordiante frame
  String refcode_p;

  //! WCS reference code for the spherical-to-cartesian projection
  String projection_p;

  //! Orientation of the skymap
  Bool eastIsLeft_p;
  Bool northIsUp_p;

  IPosition shape_p;
  Vector<Double> center_p;
  Vector<Double> increment_p;

  //! Go through the initialization steps? 
  Bool init_p;

  //! Coordinate system tool
  CoordinateSystem csys_p;

  //! Coordinate grid positions, [deg], in the local reference frame (AZEL).
  Matrix<Double> azel_p;

  Vector<Double> elevation_p;

  // Masking array, marking wether or not a grid node position is visible
  // in the local -- i.e. AZEL -- reference frame.
  Vector<Bool> mask_p;

 public:

  // === Construction / Destruction ==================================

  /*!
    \brief Default constructor

    This will set up a \f$ 120 \times 120 \f$ grid of AZEL coordinates
    in a stereographic projection (STG); the angular resolution is set to
    \f$ 2^{\circ} \f$, thus the map will cover the full hemisphere above
    the observation location.
   */
  SkymapGrid ();

  /*!
    \brief Argumented constructor

    \param obsInfo - Epoch and location of the observation.
   */
  SkymapGrid (const ObsInfo& obsInfo);
  
  /*!
    \brief Argumented constructor

    \param refcode    - Reference code of the celestial coordinate system.
    \param projection - WCS reference code of the spherical-to-cartesian
                        projection.
    \param shape      - Shape of the coordinate grid, i.e. number of grid
                        nodes along each coordinate axis.
    \param center     - Coordinates of the center of the grid, [deg].
    \param increment  - Coordinate increment between two subsequent grid
                        nodes along a coordinate axis, [deg].
   */
  SkymapGrid (const String refcode,
	      const String projection,
	      const IPosition& shape,
	      const Vector<Double>& center,
	      const Vector<Double>& increment);
  
  /*!
    \brief Argumented constructor

    \param obsInfo    - Epoch and location of the observation.
    \param refcode    - Reference code of the celestial coordinate system.
    \param projection - WCS reference code of the spherical-to-cartesian
                        projection.
    \param shape      - Shape of the coordinate grid, i.e. number of grid
                        nodes along each coordinate axis.
    \param center     - Coordinates of the center of the grid, [deg].
    \param increment  - Coordinate increment between two subsequent grid
                        nodes along a coordinate axis, [deg].
   */
  SkymapGrid (const ObsInfo& obsInfo,
	      const String refcode,
	      const String projection,
	      const IPosition& shape,
	      const Vector<Double>& center,
	      const Vector<Double>& increment);

  ~SkymapGrid ();

  // --- Operators ---------------------------------------------------

  SkymapGrid &operator= (const SkymapGrid& other);

  // --- WCS settings ------------------------------------------------

  /*!
    \brief Get the object encapsulating various observation information

    \return obsInfo -- Object holding a number of observation related 
                       information, such as observatory, observer and
		       epoch.
  */
  inline ObsInfo observationInfo () {
    return csys_p.obsInfo();
  }
  
  void setObservationInfo (const ObsInfo& obsInfo);

  /*!
    \return referencecode - Reference code of the celestial coordinate system.
  */
  String referencecode () {
    return refcode_p;
  }
  
  /*!
    \param referencecode - Reference code of the celestial coordinate system.
  */
  void setReferencecode (const String referencecode);

  /*!
    \brief Get the reference code of the spherical-to-cartesian projection

    \return projection -- The reference code of the spherical-to-cartesian
    projection
  */
  String projection () {
    return projection_p;
  }
  
  /*!
    \brief Set the spherical-to-cartesian projection

    \param projection -- The reference code of the spherical-to-cartesian
                          projection
  */
  void setProjection (const String projection);

  /*!
    \brief Get the DirectionCoordinate of the CoordinateSystem
    
    \return dc -- The DirectionCoordinate of the CoordinateSystem
  */
  DirectionCoordinate directionCoordinate ();

  // --- Grid parameters ---------------------------------------------

  /*!
    \brief Get the total number of grid nodes.

    \return nelements -- The total number of grid nodes.
   */
  Int nelements ();

  /*!
    \brief Get the total number of grid nodes.

    \param which -- Selection of which coordinates to count; if set 
           <i>True</i> only the unflagged subset of coordinates is
	   accounted for.

    \return nelements -- The total number of grid nodes.
   */
  Int nelements (const Bool which);

  /*!
    \brief Get the shape of the coordinate grid

    \return shape - The grid shape as IPosition vector.
  */
  inline IPosition shape () {
    return shape_p;
  }
  
  /*!
    \brief Set the shape of the coordinate grid.

    \param shape - The grid shape as IPosition vector.
   */
  void setShape (const IPosition& shape);

  /*!
    \brief Set the shape of the coordinate grid.

    \param shape - The grid shape as vector of integers.
   */
  void setShape (const Vector<Int>& shape);

  /*!
    \brief Get the position on which the skymap is centered.

    \return center - \f$ (\alpha,\delta)_c \f$
   */
  inline Vector<Double> center () {
    return center_p;
  }

  /*!
    \brief Set a new center position of the skymap.

    \param center - \f$ (\alpha,\delta)_c \f$
   */
  void setCenter (const Vector<Double>& center);

  /*!
    \brief Get the coordinate increment.

    \return increment - Coordinate increment, [deg].
  */
  Vector<Double> increment () {
    return increment_p;
  }

  /*!
    \brief Set the coordinate increment.

    \param increment - Coordinate increment, [deg].
  */
  void setIncrement (const Vector<Double>& increment);

  // --- Orientation of the map --------------------------------------

  /*!
    \brief Set the orientation of the skymap.

    \param directions - Vector with a description of the map orientation --
           the first element describing <i>longitude</i>. the second
	   <i>latitude</i>. Valid combinations are:
	   <tt>[East,North]</tt>,
	   <tt>[East,South]</tt>,
	   <tt>[West,North]</tt>,
	   <tt>[West,South]</tt>
  */
  void setOrientation (const Vector<String>& directions);
  
  /*!
    \brief Get the orientation of the skymap.

    \return orientation - Vector of strings, holding the
            <tt>[longitude,latitude]</tt> of the skymap.
  */
  Vector<String> orientation ();

  /*!
    \brief Is direction towards north pointing upwards?

    \return northIsUp
  */
  inline Bool isNorthUp () {
    return northIsUp_p;
  }
  
  /*!
    \brief Is direction towards north about to point upwards?

    In order to avoid unnecessary computations, we first check, if there
    really is a change w.r.t. to the stored orientation; if the axis orientation
    has changed, then we recompute the grid parameters.

    \param northIsUp
  */
  void setNorthUp (const Bool northIsUp);

  /*!
    \brief Is direction towards east pointing to the left?

    \return eastIsLeft
  */
  inline Bool isEastLeft () {
    return eastIsLeft_p;
  }

  /*!
    \brief Is direction towards east about to point to the left?

    In order to avoid unnecessary computations, we first check, if there
    really is a change w.r.t. to the stored orientation; if the axis orientation
    has changed, then we recompute the grid parameters.

    \param eastIsLeft
  */
  void setEastLeft (const Bool eastIsLeft);

  // --- Elevation range in the local (AZEL) reference frame ---------

  /*!
    \brief Get the limits for the local elevation range.

    \return elevation -- Limits of the elevation range in the local reference
                         frame (AZEL).
   */
  Vector<Double> elevationRange () const {
    return elevation_p;
  }

  /*!
    \brief Set the limits for the local elevation range.

    \param elevation -- Limits of the elevation range in the local reference
                        frame (AZEL).
   */
  void setElevationRange (const Vector<Double>& elevationRange);

  // === Grid of spherical coordinates ===============================

  /*!
    \brief Get the positions of the grid nodes

    \return grid - Coordinate grid positions, [deg], in the frame of the
            celestial coordinate system.
  */
  Matrix<Double> grid ();

  /*!
    \brief Get the positions of the grid nodes

    \param which - Selection of which coordinated to return; if set 
           <i>True</i> only the unflagged subset of coordinates is
	   returned.

    \return grid - Coordinate grid positions, [deg], in the frame of the
            celestial coordinate system.
  */
  Matrix<Double> grid (const Bool which);

  /*!
    \brief Get the positions of the grid nodes

    \retval grid - Coordinate grid positions, [deg], in the frame of the
            celestial coordinate system.
    \retval mask - Boolean array, marking wether or not a grid node position is
            visible in the local -- i.e. AZEL -- reference frame.
  */
  void grid (Matrix<Double>& grid,
	     Vector<Bool>& mask);

  /*!
    \brief Get the positions of the grid nodes within a defined reference frame

    \param which - Selection of which coordinated to return; if set 
           <i>True</i> only the unflagged subset of coordinates is
	   returned.
    \retval grid - Coordinate grid positions, [deg], in the frame of the
            celestial coordinate system.
    \retval mask - Boolean array, marking wether or not a grid node position is
            visible in the local -- i.e. AZEL -- reference frame.
  */
  Matrix<Double> grid (const String refcode,
		       Matrix<Double>& grid,
		       Vector<Bool>& mask);

  // --- Grid node positions in the local reference frame ------------

  Matrix<Double> azel (const Bool which);

  // --- Masking array -----------------------------------------------

  /*!
    \brief Return the masking array 

    \return mask -- The masking array
  */
  Vector<Bool> mask () {
    return mask_p;
  }

  // --- Feedback on the stored grid parameters ----------------------

  /*!
    \brief Write overview on grid parameters to output stream.

    \param os - Output stream
   */
  void show (std::ostream& os);
  
 private:

  void init ();

  /*!
    \brief Initialize the internal data of the newly created object.

    \param refcode    - Reference code of the celestial coordinate system.
    \param projection - WCS reference code of the spherical-to-cartesian
                        projection.
    \param shape      - Shape of the coordinate grid, i.e. number of grid
                        nodes along each coordinate axis.
    \param center     - Coordinates of the center of the grid.
    \param increment  - Coordinate increment between two subsequent grid
                        nodes along a coordinate axis.
  */
  void init (const String refcode,
	     const String projection,
	     const IPosition& shape,
	     const Vector<Double>& center,
	     const Vector<Double>& increment);

  /*!
    \brief Initialize the internal data of the newly created object.

    \param obsInfo    -- Epoch and location of the observation.
    \param refcode    -- Reference code of the celestial coordinate system.
    \param projection -- WCS reference code of the spherical-to-cartesian
                         projection.
    \param shape      -- Shape of the coordinate grid, i.e. number of grid
                         nodes along each coordinate axis.
    \param center     -- Coordinates of the center of the grid.
    \param increment  -- Coordinate increment between two subsequent grid
                         nodes along a coordinate axis.
  */
  void init (const ObsInfo& obsInfo,
	     const String refcode,
	     const String projection,
	     const IPosition& shape,
	     const Vector<Double>& center,
	     const Vector<Double>& increment);

  /*!
    \brief Copy all fields from "other" to this object
   */
  void copy_other (const SkymapGrid& other);

  ObsInfo defaultObservationInfo ();

  /*!
    \brief Default elevation range of local reference frame

    \return elevation -- Default elevation range of local reference frame,
                         [0,90].
   */
  Vector<Double> defaultElevation ();

  void setCoordinateSystem ();
  
  void setOrientation ();

  Vector<Double> centerPixel();

  /*!
    \brief Adjust the coordinate increment.

    Adjust the sign of the coordinate increment based on the combination of
    map projection and map orientation.
   */
  void adjustIncrement ();

  /*!
    \brief Adjust the value of the reference pixel
  */
  void adjustReferencePixel ();
  
  /*!
    \brief Set the coordinate grids based on the stored CoordinateSystem object.
    
    Based on the previously defined CoordiateSystem object, coordinates are 
    transformed from pixel to world value; since the latter in general will
    be given in some celestial reference frame, we perform a further
    transformation to the local reference frame (AZEL) to check, wether or not
    positions are visible at the time and location of the observation.

    \todo Check for a possible +1 pixel offset; the generated skymap appears to
    be shifted by [+1,+1] -- this might be resolved looping over pixel coordinates
    [1,N] instead of [0,N-1].
  */
  void setGrid ();

  /*!
    \brief Helper routine to convert a reference code to a MDirection::Type

    \param refcode - Reference code of the coordinate system.

    \return tp - MDirection::Types object, as used for setting up a MDirection
   */
  MDirection::Types MDirectionType (const String refcode);
  
};

}  // Namespace CR -- end

#endif
