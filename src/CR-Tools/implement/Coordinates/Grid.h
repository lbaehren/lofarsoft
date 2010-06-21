/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                    *
 *   Lars Baehren (<mail>)                                                 *
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

#ifndef GRID_H
#define GRID_H

// Standard library header files
#include <iostream>
#include <string>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class Grid
    
    \ingroup CR
    \ingroup CR_Coordinates
    
    \brief Brief description for class Grid
    
    \author Lars Baehren

    \date 2010/06/07

    \test tGrid.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>

    The following breaks down the internally created casa objects into the
    required input parameters:
    \verbatim
    .
    |-- casa::CoordinateSystem
        |-- casa::ObInfo
        |   |-- telescope           string         = string
        |   |-- casa::MEpoch
	|   |   |-- time            double         = double
	|   |   |-- frame           string         = string
	|   `-- casa::MPosition
	|       |-- position        vector<double> = [double,double,double]
	|       `-- frame           string         = string
        `-- casa::DirectionCoordinate
	    |-- casa::Mdirection::Types
	    |   `-- reference       string         = string
	    |-- casa::Projection
	    |   `-- projection      string         = string
	    |-- crval               vector<double> = [double,double,double]
	    |-- crpix               vector<double> = [double,double,double]
	    |-- cdelt               vector<double> = [double,double,double]
    \endverbatim

    \code
    // Create observation info object storing telescope position and epoch
    casa::ObsInfo info (...);

    // Create direction coordinate storing WCS parameters (system, projection, etc.)
    casa::DirectionCoordinate dir (...);

    // Create empty coordinate system object ...
    casa::CoordinateSystem cs;
    // ... and add the previously created objects
    cs.setObsInfo (info);
    cs.addCoordinate (dir);

    casa::Vector<double> pixel;
    casa::Vector<double> world;

    cs.toWorld (world,pixel);
    \endcode
    
    <h3>Example(s)</h3>
    
  */  
  class Grid {
    
  public:
    
    // === Construction =========================================================
    
    //! Default constructor
    Grid ();
    
    //! Argumented constructor
    Grid (std::string const &telecope,
	  double const &mjd,
	  double const &posLon,
	  double const &posLat,
	  std::string const &ref,
	  std::string const &projection,
	  double const &crvalX,
	  double const &crvalY,
	  double const &crpixX,
	  double const &crpixY,
	  double const &cdeltX,
	  double const &cdeltY);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another Grid object from which to create this new
             one.
    */
    Grid (Grid const &other);
    
    // === Destruction ==========================================================

    //! Destructor
    ~Grid ();
    
    // === Operators ============================================================
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another Grid object from which to make a copy.
    */
    Grid& operator= (Grid const &other); 
    
    // === Parameter access =====================================================

    double crvalX ();
    double crvalY ();
    double crpixX ();
    double crpixY ();
    double cdeltX ();
    double cdeltY ();
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, Grid.
    */
    inline std::string className () const {
      return "Grid";
    }

    //! Provide a summary of the object's internal parameters and status
    inline void summary () {
      summary (std::cout);
    }

    //! Provide a summary of the object's internal parameters and status
    void summary (std::ostream &os);    

    // === Methods ==============================================================
    
    Grid toOther (std::string const &ref);
    
  private:
    
    //! Unconditional copying
    void copy (Grid const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  }; // Class Grid -- end
  
} // Namespace CR -- end

#endif /* GRID_H */
  
