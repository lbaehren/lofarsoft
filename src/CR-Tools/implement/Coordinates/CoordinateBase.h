/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
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

#ifndef COORDINATEBASE_H
#define COORDINATEBASE_H

// Standard library header files
#include <iostream>
#include <string>

// casacore header files
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <coordinates/Coordinates/CoordinateSystem.h>

using casa::String;
using casa::Vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class CoordinateBase
    
    \ingroup CR_Coordinates
    
    \brief Base class defining the interface for the derived coordinate classes
    
    \author Lars B&auml;hren

    \date 2009/04/15

    \test tCoordinateBase.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>casacore \e Coordinate class
      <li>Some familiarity with the handling world/celestial coordinates in the
      FITS standard.
    </ul>
    
    <h3>Synopsis</h3>

    Even though casacore provides a set of coordinate classes which implement the
    most commonly used coordinate types in astronomical images, there are a number
    of cases which are not covered by thoses classes.
    
    <h3>Example(s)</h3>
    
  */  
  class CoordinateBase {
    
  public:
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    virtual ~CoordinateBase ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another CoordinateBase object from which to make a copy.
    */
    CoordinateBase& operator= (CoordinateBase const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, CoordinateBase.
    */
    inline std::string className () const {
      return "CoordinateBase";
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
    virtual void summary (std::ostream &os) =0;

    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Get the value of the reference pixel

      \return refPixel -- The value of the reference pixel, as retrieved through
              the <tt>casa::Coordinate::referencePixel()<tt> function.
    */
    virtual Vector<double> referencePixel() =0;
    /*!
      \brief Set the value of the reference pixel
    */
    virtual bool setReferencePixel(Vector<double> const &refPixel) =0;

    /*!
      \brief Get the reference value

      \return refPixel -- The reference value, as retrieved through the
              <tt>casa::Coordinate::referenceValue()<tt> function.
    */
    virtual Vector<double> referenceValue() =0;
    /*!
      \brief Set the value of the reference value


      \param increment -- The value of the coordinate increment, as assigned
             through the <tt>casa::Coordinate::setReferenceValue()<tt> function.
      \param anglesInDegrees -- If the coordinate contains angular coordinates, 
             are their values given in radians or degrees?
    */
    virtual bool setReferenceValue(Vector<double> const &refValue,
				   bool const &anglesInDegrees=true) =0;
    
    /*!
     \brief Get the value of the coordinate increment
      
      \return increment -- The value of the coordinate increment, as retrieved
              through the <tt>casa::Coordinate::increment()<tt> function.
    */
    virtual Vector<double> increment() =0;
    /*!
      \brief Set the value of the coordinate increment

      \param increment -- The value of the coordinate increment, as assigned
             through the <tt>casa::Coordinate::setIncrement()<tt> function.
      \param anglesInDegrees -- If the coordinate contains angular coordinates, 
             are their values given in radians or degrees?
     */
    virtual bool setIncrement(Vector<double> const &incr,
			      bool const &anglesInDegrees=true) =0;

    /*!
      \brief Get the names of the world axes

      \return names -- The names of the world axes, as retrieved through the
              <tt>casa::Coordinate::worldAxisNames()<tt> function.
    */
    virtual Vector<String> worldAxisNames() =0;
    /*!
      \brief Set the names of the world axes

      \param names -- The names of the world axes, as to be set through the
             <tt>casa::Coordinate::setWorldAxisNames()<tt> function.
    */
    virtual bool setWorldAxisNames(Vector<String> const &names) =0;

    /*!
      \brief Get the units of the world axes

      \return units -- The units of the world axes, as retrieved through the
              <tt>casa::Coordinate::worldAxisUnits()<tt> function.
    */
    virtual Vector<String> worldAxisUnits() =0;
    /*!
      \brief Set the units of the world axes

      \return units -- The units of the world axes, as to be set through the
              <tt>casa::Coordinate::setWorldAxisUnits()<tt> function.
    */
    virtual bool setWorldAxisUnits(Vector<String> const &units) =0;
    
    /*!
      \brief Conversion from pixel to world coordinates

      \retval world -- Values in world coordinates
      \param pixel  -- Values in pixel coordinates
    */
    virtual void toWorld (Vector<double> &world,
			  const Vector<double> &pixel) =0;
    /*!
      \brief Conversion from world to pixel coordinates

      \retval pixel -- Values in pixel coordinates
      \param world  -- Values in world coordinates
    */
    virtual void toPixel (Vector<double> &pixel,
			  const Vector<double> &world) =0;
    
    /*!
      \brief Add the coordinates to a coordinate system object

      \retval csys  -- Coordinate system object collecting the individual
              coordinate
      \param append -- Append the coordinates to the existing coordinate system
             object. If <tt>append=false</tt> then any already existing
	     coordinate objects will be removed from the coordinate system first.
    */
    virtual void toCoordinateSystem (casa::CoordinateSystem &csys,
				     bool const &append=true) =0;
  private:
    
    //! Unconditional copying
    void copy (CoordinateBase const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  }; // Class CoordinateBase -- end
  
} // Namespace CR -- end

#endif /* COORDINATEBASE_H */
  
