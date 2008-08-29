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

#ifndef COORDINATETYPES_H
#define COORDINATETYPES_H

// Standard library header files
#include <iostream>
#include <string>

// CR-Tools
#include <crtools.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class CoordinateTypes
    
    \ingroup CR_Coordinates
    
    \brief Definition of the various coordinate types we are dealing with
    
    \author Lars B&auml;hren

    \date 2008/08/29

    \test tCoordinateTypes.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class CoordinateTypes {

  public:

    /*!
      \brief Types of (spatial) coordinates
    */
    enum Type {
      //! 
      Direction,
      //! Direction on the sky with radial distance, \f$ \vec x = (Lon,Lat,R) \f$
      DirectionRadius,
      //! Cartesian coordinates, \f$ \vec x = (x,y,z) \f$
      Cartesian,
      //! Spherical coordinates, \f$ \vec x = (r,\phi,\theta) \f$
      Spherical,
      //! Cylindrical coordinates, \f$ \vec x = (r,\phi,h) \f$
      Cylindrical,
      //! Azimuth-Elevation-Height, \f$ \vec x = (Az,El,H) \f$
      AzElHeight,
      //! Azimuth-Elevation-Radius, \f$ \vec x = (Az,El,R) \f$
      AzElRadius,
      //! North-East-Height
      NorthEastHeight
    };
    
    // ------------------------------------------------------------- Construction

    /*!
      \brief Argumented constructor
    */
    CoordinateTypes (CoordinateTypes::Type const &type) {
      type_p = type;
    }
    
    /*!
      \brief Copy constructor
      
      \param other -- Another CoordinateTypes object from which to create this new
      one.
    */
    CoordinateTypes (CoordinateTypes const &other);
    
    // ------------------------------------------------------------------ Methods

    /*!
      \brief Get the coordinate type

      \return type -- The coordinate type
    */
    inline CoordinateTypes::Type type () const {
      return type_p;
    }

    /*!
      \brief Set the coordinate type

      \param type -- The coordinate type
    */
    inline void setType (CoordinateTypes::Type const &type) {
      type_p = type;
    }

    /*!
      \brief Does this coordinate type contain a spherical map projection?
      
      \return hasProjection -- <tt>true</tt> if the coordinate type contains a
              spherical map projection; this is the case if the coordinate is
	      composed of at least one object of type casa::DirectionCoordinate.
    */
    inline bool hasProjection () const {
      return hasProjection (type_p);
    }
    
    /*!
      \brief Does this coordinate type contain a spherical map projection?
      
      \param coord -- Coordinate type to check
      
      \return hasProjection -- <tt>true</tt> if the coordinate type contains a
              spherical map projection; this is the case if the coordinate is
	      composed of at least one object of type casa::DirectionCoordinate.
    */
    static bool hasProjection (CoordinateTypes::Type const &coord);
    
    /*!
      \brief Is the coordinate of type Cartesian?
    
      \return isCartesian -- Returns <tt>true</tt> is the coordinate is of type
              Cartesian.
    */
    inline bool isCartesian () const {
      return isCartesian (type_p);
    }
    
    /*!
      \brief Is the coordinate of type Cartesian?
    
      \param coord -- Coordinate type to check

      \return isCartesian -- Returns <tt>true</tt> is the coordinate is of type
              Cartesian.
    */
    static bool isCartesian (CoordinateTypes::Type const &coord);
    
    /*!
      \param coord -- Coordinate type to check
    */
    static bool isSpherical (CoordinateTypes::Type const &coord);
    
  private:
    
    /*! Coordinate type */
    CoordinateTypes::Type type_p;
    
    /*!
      \brief Unconditional copying
    */
    void copy (CoordinateTypes const &other);
    
  };
  
} // Namespace CR -- end

#endif /* COORDINATETYPE_H */
  
