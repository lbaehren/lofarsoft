/*-------------------------------------------------------------------------*
 | $Id:: NewClass.h 1378 2008-04-01 16:47:06Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#ifndef SPATIALCOORDINATE_H
#define SPATIALCOORDINATE_H

#include <crtools.h>

// Standard library header files
#include <iostream>
#include <string>
#include <vector>

#include <coordinates/Coordinates/Coordinate.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/TabularCoordinate.h>

#include <Coordinates/CoordinateTypes.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class SpatialCoordinate
    
    \ingroup CR_Coordinates
    
    \brief Container to combine other coordinates into a spatial (3D) coordinate
    
    \author Lars B&auml;hren

    \date 2008/08/28

    \test tSpatialCoordinate.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>casacore <a href="http://www.astron.nl/casacore/doc/html/classcasa_1_1Coordinate.html">Coordinate</a> class
    </ul>
    
    <h3>Synopsis</h3>
    
    In order to be consistent we extract the celestial coordinates required for
    beamforming directly from the same coordinate system object which is going to
    be attached to the generated image. Since the Beamformer is expected to
    support both imaging in the near field -- as required for e.g. the radio
    emission from cosmic rays induced air showers -- as well as in the far field
    -- thereby enabling the calibration on astronomical sources -- we run into 
    an implementation problem for the spatial coordinates of the image: none of
    forseeable cases can be covered using a single of the existing Coordinate
    classes of the <i>casacore</i> library.
    <ol>
      <li>For imaging in the far-field and/or using a spherical map projection
      (such as SIN, STG, CAR, ...), the direction information for the angular
      coordinates (e.g. AZEL, J2000) is contained within a casa::DirectionCoordinate
      object.
      <li>For the imaging on in the near field without the usage of a spherical
      map projection the complete coordinate information can be encoded within a
      single casa::LinearCoordinate object.
    </ol>
      
    <table border="0">
      <tr>
        <td class="indexkey">Coordinate types</td>
	<td class="indexkey">Axis names</td>
	<td class="indexkey">Axis units</td>
	<td class="indexkey">Projection</td>
	<td class="indexkey">Description</td>
      </tr>
      <tr>
        <td>[Direction,Linear]</td>
	<td>[Longitude, Latitude,Length]</td>
	<td>[rad, rad,m]</td>
	<td>yes</td>
	<td>Direction on the sky with radial distance</td>
      </tr>
      <tr>
        <td>[Linear]</td>
	<td>[Distance, Distance, Distance]</td>
	<td>[m, m, m]</td>
	<td>no</td>
	<td>Cartesian coordinates</td>
      </tr>
      <tr>
        <td>[Linear]</td>
	<td>[Longitude, Latitude, Length]</td>
	<td>[deg, deg, m]</td>
	<td>no</td>
	<td>Spherical coordinates</td>
      </tr>
    </table>
    
    <h3>Example(s)</h3>
    
  */  
  class SpatialCoordinate {

    /*! Type of spatial coordinate */
    CoordinateTypes::Type type_p;
    /*! Coordinate objects forming the spatial coordinate */
    std::vector<casa::Coordinate> coord_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    SpatialCoordinate ();
    
    /*!
      \brief Argumented constructor
      
      \param direction -- 
      \param linear    -- 
    */
    SpatialCoordinate (casa::DirectionCoordinate const &direction,
		       casa::LinearCoordinate const &linear);
    
    /*!
      \brief Argumented constructor
      
      \param linear    -- 
    */
    SpatialCoordinate (casa::LinearCoordinate const &linear);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another SpatialCoordinate object from which to create this new
      one.
    */
    SpatialCoordinate (SpatialCoordinate const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~SpatialCoordinate ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another SpatialCoordinate object from which to make a copy.
    */
    SpatialCoordinate& operator= (SpatialCoordinate const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, SpatialCoordinate.
    */
    std::string className () const {
      return "SpatialCoordinate";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods
    
    inline int nofCoordinates () const {
      return coord_p.size();
    }
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (SpatialCoordinate const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* SPATIALCOORDINATE_H */
  
