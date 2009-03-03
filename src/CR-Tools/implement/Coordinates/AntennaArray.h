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

#ifndef ANTENNAARRAY_H
#define ANTENNAARRAY_H

// Standard library header files
#include <iostream>
#include <string>

// casacore header files
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/Quanta/MVPosition.h>
#include <measures/Measures/MEpoch.h>>
#include <measures/Measures/MPosition.h>>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class AntennaArray
    
    \ingroup CR_Coordinates
    
    \brief Container for an array of antennas (e.g. a station of antennas)
    
    \author Lars B&auml;hren

    \date 2009/02/27

    \test tAntennaArray.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>Some familiarity with the casacore Measures module
      <li>The ObservationFrame class might serve as a model, even though we try
      to be more general at this point.
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class AntennaArray {

    //! Name/Identifier of the antenna array
    std::string name_p;
    //! Epoch (required for reference frame transformations)
    casa::MEpoch epoch_p;
    //! Geographic position of the antenna array center
    casa::MPosition location_p;
    //! Positions of the individual antennas
    casa::Vector<casa::MPosition> positions_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    AntennaArray ();
    
    /*!
      \brief Argumented constructor

      \param name     -- Name/Identifier of the antenna array
      \param epoch    -- Epoch (required for reference frame transformations)
      \param location -- Geographic position of the antenna array center
    */
    AntennaArray (std::string const &name,
		  casa::MEpoch const &epoch,
		  casa::MPosition const &location);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another AntennaArray object from which to create this new
             one.
    */
    AntennaArray (AntennaArray const &other);
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    ~AntennaArray ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another AntennaArray object from which to make a copy.
    */
    AntennaArray& operator= (AntennaArray const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the name/identifier of the antenna array

      \return name -- The name/identifier of the antenna array
    */
    inline std::string name () const {
      return name_p;
    }

    /*!
      \brief Set the name/identifier of the antenna array

      \param name -- The name/identifier of the antenna array
    */
    inline void setName (std::string const &name) {
      name_p = name;
    }

    /*!
      \brief Get the epoch

      \return epoch -- The epoch (time) for which the antenna array positions are
              valid.
    */
    inline casa::MEpoch epoch () const {
      return epoch_p;
    }
    
    /*!
      \brief Set the epoch

      \param epoch -- The epoch (time) for which the antenna array positions are
             valid.
    */
    inline void setEpoch (casa::MEpoch const &epoch) {
      epoch_p = epoch;
    }
    
    /*!
      \brief Get the geographic position of the antenna array center

      \return location -- Geographic position of the antenna array center
    */
    inline casa::MPosition location () const {
      return location_p;
    }

    /*!
      \brief Set the geographic position of the antenna array center

      \param location -- Geographic position of the antenna array center
    */
    inline void setLocation (casa::MPosition const &location) {
      location_p = location;
    }

    /*!
      \brief Set the positions of the individual antennas in the array

      \param positions -- The positions of the individual antennas in the array
      \param relative  -- Are the antenna positions relative w.r.t. the array
             center position? Set <tt>relative=false</tt> in case absolute 
	     geographic positions are provided for each individual receiving
	     element.

      \return status -- Status of the operation; returns \e false in case an
              error was encountered.
    */
    bool setPositions (casa::Matrix<double> const &positions,
		       bool const &relative);

    /*!
      \brief Set the positions of the individual antennas in the array

      \param positions -- The positions of the individual antennas in the array
      \param relative  -- Are the antenna positions relative w.r.t. the array
             center position? Set <tt>relative=false</tt> in case absolute 
	     geographic positions are provided for each individual receiving
	     element.

      \return status -- Status of the operation; returns \e false in case an
              error was encountered.
    */
    bool setPositions (casa::Vector<casa::MVPosition> const &positions,
		       bool const &relative);
    
    /*!
      \brief Set the positions of the individual antennas in the array

      \param positions -- The positions of the individual antennas in the array
      \param relative  -- Are the antenna positions relative w.r.t. the array
             center position? Set <tt>relative=false</tt> in case absolute 
	     geographic positions are provided for each individual receiving
	     element.

      \return status -- Status of the operation; returns \e false in case an
              error was encountered.
    */
    bool setPositions (casa::Vector<casa::MPosition> const &positions,
		       bool const &relative);
    
    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, AntennaArray.
    */
    inline std::string className () const {
      return "AntennaArray";
    }

    //! Provide a summary of the internal status
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);    

  private:

    /*!
      \brief Initialize internal settings

      \param name     -- Name/Identifier of the antenna array
      \param epoch    -- Epoch (required for reference frame transformations)
      \param location -- Geographic position of the antenna array center
    */
    void init (std::string const &name,
	       casa::MEpoch const &epoch,
	       casa::MPosition const &location);
    
    //! Unconditional copying
    void copy (AntennaArray const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  }; // Class AntennaArray -- end
  
} // Namespace CR -- end

#endif /* ANTENNAARRAY_H */
  
