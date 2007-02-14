/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars Bahren (bahren@astron.nl)                                        *
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

/* $Id: GridCoordinate.h,v 1.2 2006/08/24 13:55:12 bahren Exp $*/

#ifndef COORDINATES_GRIDCOORDINATE_H
#define COORDINATES_GRIDCOORDINATE_H

#include <casa/aips.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Exceptions/Error.h>
#include <casa/Utilities/Assert.h>
#include <coordinates/Coordinates/LinearCoordinate.h>

using casa::assert_;
using casa::AipsError;
using casa::IPosition;
using casa::LinearCoordinate;
using casa::String;
using casa::Vector;

namespace CR {  // Namespace CR -- begin

/*!
  \class GridCoordinate

  \ingroup Coordinates

  \brief A linear coordinate, which is defined for a certain range of values.

  \author Lars B&auml;hren

  \date 2006/04/04

  \test tGridCoordinate.cc

  \todo How are we going to deal with a grid, where certain elements might are
  missing? - This might be handled using a masking array

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/coordinates/implement/Coordinates/LinearCoordinate.html">LinearCoordinate</a>
    -- Interconvert between pixel and a linear world coordinate.
  </ul>

  <h3>Synopsis</h3>

   The LinearCoordinate class ties pixel and world axes together through a general
   linear transformation.
   \verbatim
   world = (cdelt * PC * (pixel - crpix)) + crval
   \endverbatim
   where PC is an \f$ N \times N\f$ matrix; pixel, crval, crpix and world are
   length \f$ N \f$ vectors, and cdelt is an \f$ N \times N \f$ diagonal
   matrix, represented as a length \f$ N \f$ vector.

   The increment, \f$ \delta x \f$, can be derived from the range of values and
   the number of nodes:
   \f[ \delta x = \frac{x_{\rm max} - x_{\rm min}}{N-1} \f]

  <h3>Example(s)</h3>

*/

class GridCoordinate : public LinearCoordinate {

  //! Bottom-left-corner delimiting the range of the grid
  Vector<double> blc_p;
  //! Top-right-corner delimiting the range of the grid
  Vector<double> trc_p;
  //! Shape of the grid coordinates array
  IPosition shape_p;
    
 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor

    Also uses the default constructor for the parent LinearCoordinate object.
  */
  GridCoordinate ();

  /*!
    \brief Argumented constructor

    \param blc   -- Value for the Bottom-left-corner delimiting the range of
                    the grid
    \param trc   -- Value for the Top-right-corner delimiting the range of the
                    grid
    \param shape -- Number of grid nodes.
  */
  GridCoordinate (double const &blc,
		  double const &trc,
		  uint const &shape);
  
  /*!
    \brief Argumented constructor

    \param names -- World axis name, e.g. "Distance"
    \param units -- Physical units of the axis coordinates, e.g. "m"
    \param blc   -- Vector with the values for the Bottom-left-corner delimiting
                    the range of the grid
    \param trc   -- Vector with the values for the Top-right-corner delimiting
                    the range of the grid
    \param shape -- 
  */
  GridCoordinate (String const &names,
		  String const &units,
		  double const &blc,
		  double const &trc,
		  uint const &shape);
  
  /*!
    \brief Argumented constructor

    \param blc   -- Vector with the values for the Bottom-left-corner delimiting
                    the range of the grid
    \param trc   -- Vector with the values for the Top-right-corner delimiting
                    the range of the grid
    \param shape -- 
  */
  GridCoordinate (Vector<double> const &blc,
		  Vector<double> const &trc,
		  IPosition const &shape);
  
  /*!
    \brief Argumented constructor

    \param names -- World axis name, e.g. "Distance"
    \param units -- Physical units of the axis coordinates, e.g. "m"
    \param blc   -- Vector with the values for the Bottom-left-corner delimiting
                    the range of the grid
    \param trc   -- Vector with the values for the Top-right-corner delimiting
                    the range of the grid
    \param shape -- 
  */
  GridCoordinate (Vector<String> const &names,
		  Vector<String> const &units,
		  Vector<double> const &blc,
		  Vector<double> const &trc,
		  IPosition const &shape);
  
  /*!
    \brief Copy constructor

    \param other -- Another GridCoordinate object from which to create this new
                    one.
  */
  GridCoordinate (GridCoordinate const &other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~GridCoordinate ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another GridCoordinate object from which to make a copy.
  */
  GridCoordinate& operator= (const GridCoordinate&other); 

  // ----------------------------------------------------------------- Parameters

  /*!
    \brief Bottom-left-corner delimiting the range of the grid

    \return blc -- Vector with the values for the Bottom-left-corner delimiting
                   the range of the grid
  */
  Vector<double> blc () {
    return blc_p;
  }

  /*!
    \brief Set the bottom-left-corner delimiting the range of the grid

    \todo Only a dummy so far - needs to be implemented

    \param blc -- 
  */
  void setBLC (const Vector<double>& blc);

  /*!
    \brief Top-right-corner delimiting the range of the grid

    \return trc -- Vector with the values for the Top-right-corner delimiting
                   the range of the grid
  */
  Vector<double> trc () {
    return trc_p;
  }

  /*!
    \brief Set the top-right-corner delimiting the range of the grid

    \todo Only a dummy so far - needs to be implemented

    \param trc -- 
  */
  void setTRC (const Vector<double>& trc);

  /*!
    \brief Shape of the grid coordinates array

    \return shape -- Shape of the grid coordinates array
  */
  IPosition shape () {
    return shape_p;
  }

  /*!
    \brief Set the shape of the grid coordinates array

    \todo Only a dummy so far - needs to be implemented

    \param shape -- Shape of the grid coordinates array
  */
  void setShape (const IPosition& shape);

  /*!
    \brief Get the coordinates along the axis

    \return coordinates -- The set of coordinates
  */
  Vector<double> coordinates ();

  // ----------------------------------------- Methods inheireted from base class

  /*!
    \brief Set the coordinate increment
    
    \param inc -- Coordinate increment along the coordinate axes

    \return status -- Return status of the operation; return <tt>True</tt> if
                      everything went fine.
  */
  bool setIncrement (const Vector<double> &inc);

 private:

  /*!
    \brief Unconditional copying
  */
  void copy (const GridCoordinate& other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

  /*!
    \brief Recompute the increment between grid nodes

    Derive the coordinate increment from the range of values and the number of
    nodes
  */
  void setGridIncrement ();

  /*!
    \brief Recompute the number of grid nodes
   */
  void setGridNodes ();

};

}  // Namespace CR -- end

#endif /* GRIDCOORDINATE_H */
