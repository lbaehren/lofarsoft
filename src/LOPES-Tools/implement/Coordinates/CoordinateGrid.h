/***************************************************************************
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

/* $Id: CoordinateGrid.h,v 1.3 2006/07/05 16:01:08 bahren Exp $ */

#ifndef COORDINATEGRID_H
#define COORDINATEGRID_H

#include <fstream>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>

using casa::Array;
using casa::IPosition;
using casa::Matrix;
using casa::String;
using casa::Vector;

namespace LOPES {  // Namespace LOPES -- begin
  
  /*!
    \class CoordinateGrid
    
    \ingroup Coordinates
    
    \brief Generation of N-dimension regular coordinate grids
    
    \author Lars B&auml;hren
    
    \date 2005/03/23
    
    \test tCoordinateGrid.cc
    
    <h3>Description</h3>
    
    For the i-th coordinate axis in the grid the position of the n-th grid point
    is given by
    \f[
      x_{i,n_i} = x_{i,c_i} + \delta_{i} \cdot \left( n_i -
      \left( \frac{N_i-1}{2} \right) \right)
    \f]
    where \f$ \delta_{i} \f$ is the increment and \f$ N_i \f$ the number of points
    along the i-th axis.
    
    The example below shows a 3-dimensional regular grid as it would be used as
    coordinate input to the Beamformer.
    <img src="../figures/tCoordinateGrid.png">
  */
  
  class CoordinateGrid {
    
    // Center coordinates of the grid.
    Vector<double> center_p;
    
    // Increment along each coordinate axis.
    Vector<double> increment_p;
    
    // Shape of the coordinate grid, i.e. the number of grid nodes along each
    // grid axis.
    IPosition shape_p;
    
    // Physical units associated with each grid axis.
    Vector<String> units_p;
    
    // Coordinates of the grid nodes, [points,axes]
    Matrix<double> grid_p;
    
    // Limits for the values along each grid axis.
    Matrix<double> limits_p;
    
    // Use the limits and therefore also the masking array
    bool useLimits_p;
    
    // Masking array for the grid node coordinates; applies selection via
    // the value limits.
    Vector<bool> mask_p;
    
  public:
    
    // === Construction ================================================
    
    /*!
      \brief Empty constructor
    */
    CoordinateGrid ();
    
    /*!
      \brief Argumented constructor
      
      \param shape     - Shape of the coordinate grid, i.e. number of grid nodes
                         along each coordinate axis.
      \param center    - Coordinates of the center of the grid.
      \param increment - Coordinate increment between two subsequent grid nodes
                         along a coordinate axis.
    */
    CoordinateGrid (const Vector<int>& shape,
		    const Vector<double>& center,
		    const Vector<double>& increment);
    
    /*!
      \brief Argumented constructor
      
      \param shape     - Shape of the coordinate grid, i.e. number of grid nodes
                         along each coordinate axis.
      \param center    - Coordinates of the center of the grid.
      \param increment - Coordinate increment between two subsequent grid nodes
                         along a coordinate axis.
      \param units     - Physical units associated with each coordinate axis.
    */
    CoordinateGrid (const Vector<int>& shape,
		    const Vector<double>& center,
		    const Vector<double>& increment,
		    const Vector<String>& units);
    
    // === Destruction =================================================
    
    ~CoordinateGrid();
    
    // === Grid parameters =============================================
    
    /*!
      \brief Get the shape of the coordinate grid
      
      \return shape - The grid shape as IPosition vector.
    */
    IPosition shape ();
    
    /*!
      \brief Set the shape of the coordinate grid.
      
      \param shape - The grid shape as IPosition vector.
    */
    void setShape (const IPosition& shape);
    
    /*!
      \brief Set the shape of the coordinate grid.
      
      \param shape - The grid shape as vector of integers.
    */
    void setShape (const Vector<int>& shape);
    
    /*!
      \brief Get the center coordinates of the grid
      
      \return center - Vector with the center coordinates, \f$ x_{i,\rm c} \f$,
                       of the coordinate grid
    */
    Vector<double> center ();
    
    /*!
      \brief Get the increment along each coordinate axis.
      
      \return increment - Vector with the increment, \f$ \delta_{i} \f$, along
                          each coordinate axis.
    */
    Vector<double> increment ();
    
    /*!
      \brief Units of the coordinate axes.
      
      \return units - Vector with the units for each coordinate axis.
    */
    Vector<String> units ();
    
    // === Coordinate grid =============================================
    
    /*!
      \brief Get the values of the coordinate grid.
      
      \return grid - Coordinates of the grid nodes, [points,axes]
    */
    Matrix<double> grid () {
      return grid_p;
    }
    
    /*!
      \brief Export the values of the coordinate grid to an output stream.
      
      \param os - Output stream to which the coordinate grid is written.
    */
    void grid (std::ostream& os);
    
    /*!
      \brief Export the values of the coordinate grid to an array.
      
      \param grid - An Array of doubles; will be resized internally before
                    filling in the values.
      
      \retval grid - Array containing the values of the coordinate grid nodes.
    */
    void grid (Array<double>& grid);
    
    // === Masking of the grid node values =============================
    
    /*!
      \brief Get the limits for the values along each grid axis.
      
      \return limits - Matrix with the limits for each coordinate axis.
    */
    Matrix<double> limits ();
    
    /*!
      \brief Get the limits for the values along each grid axis.
      
      \param axis - Number of the coordinate axis, for which to return the 
                    limits.
      
      \return limits - Limit of values for the i-th coordinate axis.
    */
    Vector<double> limits (const int&);
    
    /*!
      \brief Set the limits for the values along a certain grid axis.
    */
    void setLimits (const Matrix<double>& limits);
    
    /*!
      \brief Set the limits for the values along a certain grid axis.
      
      \param limits - Limit of the values.
      \param axis   - The number of the axis, for which the limits are provided.
    */
    void setLimits (const Vector<double>& limits,
		    const int& axis);
    
    /*!
      \brief Are we applying value range limitation to the coordinate grid?
      
      \return useLimits - boolean, true of limits are used.
    */
    bool useLimits ();
    
    /*!
      \brief Enable/Disable value range limitation to the coordinate grid.
      
      \param useLimits - boolean, true of limits are used.
    */
    void useLimits (const bool);
    
  private:
    
    // === Default values for the grid parameters ======================

    /*!
      \brief Get the default coordinates for the center

      \return defaultCenter -- The default coordinates for the center
    */
    Vector<double> defaultCenter ();
    
    /*!
      \brief Get the default increment along the coordinate axes

      \return defaultIncrement -- The default increment along the coordinate axes
    */
    Vector<double> defaultIncrement ();

    /*!
      \brief Get the default shape of the coordinate grid

      \return defaultShape -- The default shape of the coordinate grid, i.e. the
                              lengths of the coordinate axes
     */
    Vector<int> defaultShape ();
    
    /*!
      \param nofAxes - The number of coordinate grid axes.
    */
    Vector<String> defaultUnits (const int nofAxes);
    
    Matrix<double> defaultLimits (const int);
    
    /*!
      \brief Initialize the parameters defining the coordinate grid.

      \param shape     -- 
      \param center    -- 
      \param increment --
      \param units     -- 
      
      All constructors call this function to perform required checks on the
      provided parameters (consistency of array shapes, etc.).
    */
    void initCoordinateGrid (const Vector<int>& shape,
			     const Vector<double>& center,
			     const Vector<double>& increment,
			     const Vector<String>& units);
    
    /*!
      \brief Set up the array holding the coordinates of the grid nodes.
    */
    void setGrid ();
    
    /*!
      \brief Set up the masking array for the grid nodes.
      
      Set the masking array for complete set of grid nodes, based on the grid
      layout and the limits for each coordinate axis.
    */
    void setMask ();
    
    /*!
      \brief Compute the coordinate of a point on a coordinate axis.
      
      \param numCoord - Number of the coordinate axis
      \param numPoint - Number of the point along the selected coordinate axis.
    */
    double calcCoordinate (const int numCoord, 
			   const int numPoint);
  };
  
}  // Namespace LOPES -- end

#endif


