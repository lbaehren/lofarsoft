/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *                                                                         *
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

/* $Id: SkymapperTools.h,v 1.6 2006/08/24 13:55:12 bahren Exp $*/

#ifndef SKYMAPPERTOOLS_H
#define SKYMAPPERTOOLS_H

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Exceptions/Error.h>
#include <casa/Quanta.h>

#include <lopes/IO/DataReaderTools.h>
#include <lopes/Observation/ObservationData.h>
#include <lopes/Skymap/Skymap.h>
#include <lopes/Skymap/Skymapper.h>

#include <casa/namespace.h>

/*!
  \class SkymapperTools

  \ingroup Skymap

  \brief Collection of tools and helper functions for the Skymapper class

  \author Lars Bahren

  \date 2006/07/25

  \test tSkymapperTools.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[start filling in your text here]
  </ul>

  <h3>Synopsis</h3>

  Functions provided here can be grouped as follows:
  <ol>
    <li>Setup and manipulation of the Skymap object
    <ul>
      <li>SkymapperTools::skymapCenter
      <li>SkymapperTools::setSkymapGrid
    </ul>
    <li>Handling of coordinates and coordinate systems
    <ul>
      <li>SkymapperTools::obsinfo
      <li>SkymapperTools::MDirectionType
      <li>SkymapperTools::ProjectionType
      <li>SkymapperTools::coordinateSystem
    </ul>
  </ol>

  <h3>Example(s)</h3>

*/

class SkymapperTools {
  
 public:

  // ============================================================================
  //
  //  Setup and manipulation of the Skymap object
  //
  // ============================================================================
  
  /*!
    \brief Adjust the center position of the skymap
    
    \param epoch         -- The epoch/time of the observation.
    \param telescope     -- The name of the observatory.
    \param refcodeSkymap -- Reference code for the reference frame of the skymap.
    \param refcodeCenter -- Reference code for the reference frame, in which the
    skymap center position was defined by the user.
    \param centerInput   -- Coordinates of the user defined center position.
    
    \return centerSkymap -- Coordinates of the skymap center in the reference
    frame of the skymap.
  */
  static Vector<Double> skymapCenter (const Quantity epoch,
				      const String telescope,
				      const String refcodeSkymap,
				      const String refcodeInput,
				      const Vector<Double> &centerInput);
  
  /*!
    \brief Set the SkymapGrid object embedded in Skymap.
    
    \retval skymap -- Skymap object, for which the grid will be set

    \param epoch         -- The epoch/time of the observation.
    \param telescope     -- The name of the observatory.
    \param refcodeSkymap -- Reference code for the reference frame of the skymap.
    \param refcodeCenter -- Reference code for the reference frame, in which the
                            skymap center position was defined by the user.
    \param projection    -- WCS reference code of the spherical-to-cartesian
                            projection.
    \param shape         -- Shape of the coordinate grid, i.e. number of grid
			    nodes along each coordinate axis.
    \param center        -- Coordinates of the center of the grid, [deg].
    \param increment     -- Coordinate increment between two subsequent grid
			    nodes along a coordinate axis, [deg].
  */
  static void setSkymapGrid (Skymap &skymap,
			     const Quantity epoch,
			     const String telescope,
			     const String refcodeSkymap,
			     const String refcodeCenter,
			     const String projection,
			     const IPosition& shape,
			     const Vector<Double>& center,
			     const Vector<Double>& increment);

  // ============================================================================
  //
  //  Handling of coordinates and coordinate systems
  //
  // ============================================================================
  
  /*!
    \brief Create ObsInfo from its input parameters
    
    \param obsTime   -- Time of the observation, as <i>Time</i>
    \param telescope -- Name of the telescope
    \param observer  -- Name of the observer

    \return obsinfo -- Observation info, wrapped into a ObsInfo object
  */
  static ObsInfo obsinfo (const Time &obsTime,
			  const String &telescope="LOFAR",
			  const String &observer="LOFAR/CR KSP");

  /*!
    \brief Create ObsInfo from its input parameters
    
    \param obsTime   -- Time of the observation, as <i>Quantity</i>
    \param telescope -- Name of the telescope
    \param observer  -- Name of the observer

    \return obsinfo -- Observation info, wrapped into a ObsInfo object
  */
  static ObsInfo obsinfo (const Quantity &obsTime,
			  const String &telescope="LOFAR",
			  const String &observer="LOFAR/CR KSP");

  /*!
    \brief Create ObsInfo from its input parameters

    \param obsDate   -- Time and date of the observation, as <i>MEpoch</i>
    \param telescope -- Name of the telescope
    \param observer  -- Name of the observer

    \return obsinfo -- Observation info, wrapped into a ObsInfo object
  */
  static ObsInfo obsinfo (const MEpoch &obsDate,
			  const String &telescope="LOFAR",
			  const String &observer="LOFAR/CR KSP");

  /*!
    \brief Get MDirection type from reference code of the coordinate frame

    \param refcode -- Reference code of the coordinate frame

    \return tp -- MDirection type
  */
  static MDirection::Types MDirectionType (String const &refcode);

  /*!
    \brief Get Projection type from reference code of the projection

    \param refcode -- Reference code of the projection, e.g. <i>AIT</i>

    \return tp -- Projection type
  */
  static Projection::Type ProjectionType (String const &refcode);

  /*!
    \brief Get the coordinate system from an image

    \param imagefile      -- Image file from which to extract the
                             CoordinateSystem object
    \param obsinfo        -- Miscellaneous information related to the observation
    \param replaceObsInfo -- 

    \return csys -- Coordinate system associated with the image
  */
  static CoordinateSystem coordinateSystem (const String &imagefile,
					    const ObsInfo &obsinfo=ObsInfo(),
					    const Bool replaceObsInfo=False);

  /*!
    \brief Get the coordinate system from an image

    \param image          -- Paged image from which to extract the
                             CoordinateSystem object
    \param obsinfo        -- Miscellaneous information related to the observation
    \param replaceObsInfo -- 

    \return csys -- Coordinate system associated with the image
  */
  static CoordinateSystem coordinateSystem (const PagedImage<Float> &image,
					    const ObsInfo &obsinfo=ObsInfo(),
					    const Bool replaceObsInfo=False);
  
  /*!
    \brief Set the coordinate system from the coordinate axis parameters
    
    \param obsinfo        -- 
    \param refcode        -- Reference code identifying the coordinate reference
                             frame, e.g. <i>AZEL</i>
    \param projection     -- Reference code identifying the spherical map
                             projection, e.g. <i>AIT</i>
    \param shape          -- 
    \param crpix          -- 
    \param crval          -- 
    \param cdelt          -- 
  */
  static CoordinateSystem coordinateSystem(const ObsInfo &obsinfo,
					   const String &refcode,
					   const String &projection,
					   const IPosition &shape,
					   const Vector<Double> &crpix,
					   const Vector<Double> &crval,
					   const Vector<Double> &cdelt);

};

#endif /* SKYMAPPERTOOLS_H */
