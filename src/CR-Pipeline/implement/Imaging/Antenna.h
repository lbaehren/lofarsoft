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

#ifndef ANTENNA_H
#define ANTENNA_H

// Standard library header files
#include <string>
#include <vector>

#include <Imaging/Feed.h>

using std::vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class Antenna

    \ingroup CR
    \ingroup Imaging
    
    \brief Brief description for class Antenna
    
    \author Lars B&auml;hren

    \date 2007/06/12

    \test tAntenna.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>Feed -- Properties of an antenna feed
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class Antenna {

    //! Identifier of the antenna
    int ID_p;
    //! Feeds of the antenna
    vector<Feed> feeds_p;
    //! Position w.r.t. the array reference position, [m]
    vector<double> position_p;
    //! Orientation w.r.t. the array referene frame, [rad]
    vector<double> orientation_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    Antenna ();
    
    /*!
      \brief Default constructor

      \param ID              -- Identifier for the antenna
      \param feeds           -- A set of Feed objects, describing the properties
                                of the antenna feeds.
      \param position        -- Positional offset \f$ (x,y,z) \f$ of the antenna
                                w.r.t. to the reference frame of the antenna
				array or station.
      \param orientation     -- Angles \f$ (\alpha_1,\alpha_2,\alpha_3) \f$ for
                                the rotation of the feed w.r.t. to the reference
				frame of the antenna.
      \param anglesInDegrees -- Are the rotation angles given in degree? Per
                                default values are expected in radian.
    */
    Antenna (int const &ID,
	     vector<Feed> const &feeds,
	     vector<double> const &position,
	     vector<double> const &orientation,
	     bool const &anglesInDegrees);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another Antenna object from which to create this new
      one.
    */
    Antenna (Antenna const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~Antenna ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another Antenna object from which to make a copy.
    */
    Antenna& operator= (Antenna const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the ID of the antenna

      \return ID -- The ID of the antenna
    */
    inline int ID () {
      return ID_p;
    }

    /*!
      \brief Set the ID of the antenna

      \param ID -- The ID of the antenna
    */
    inline void setID (int const &ID) {
      ID_p = ID;
    }
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, Antenna.
    */
    std::string className () const {
      return "Antenna";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods
    
    
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (Antenna const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* ANTENNA_H */
  
