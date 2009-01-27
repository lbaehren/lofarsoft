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

#ifndef ANTENNAGAIN_H
#define ANTENNAGAIN_H

// Standard library header files
#include <iostream>
#include <string>
//casacore header files
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/Vector.h>
#include <casa/Quanta/MVFrequency.h>
#include <casa/Quanta/MVPosition.h>
// CR-Tools header files
#include <Imaging/GeomPhase.h>

using casa::Cube;
using casa::Vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class AntennaGain
    
    \ingroup CR_Imaging
    
    \brief Container for the complex-valued antenna gains
    
    \author Lars B&auml;hren

    \date 2009/01/27

    \test tAntennaGain.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>GeomDelay
      <li>GeomPhase
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class AntennaGain {

    //! IDs of the antennas for which the gains are provided
    Vector<int> antennas_p;
    //! Sky positions for which the antenna gains are provided
    Vector<MVPosition> skyPositions_p;
    //! Frequencies for which the antenna gains are provided
    Vector<MVFrequency> frequencies_p;
    //! Complex antenna gains as function of direction and frequency
    Cube<casa::DComplex> gains_p;
    //! Are the gain values computed for the given grid of positions and frequencies?
    bool gainsMatchGrid_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    AntennaGain ();
    
    //! Argumented constructor
    AntennaGain (Vector<int> const &antennas,
		 Vector<MVPosition> const &skyPositions,
		 Vector<MVFrequency> const &frequencies,
		 Cube<casa::DComplex> const &gains,
		 bool const gainsMatchGrid=true);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another AntennaGain object from which to create this new
             one.
    */
    AntennaGain (AntennaGain const &other);
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    ~AntennaGain ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another AntennaGain object from which to make a copy.
    */
    AntennaGain& operator= (AntennaGain const &other); 
    
    // --------------------------------------------------------------- Parameters

    inline Vector<int> antennas () const {
      return antennas_p;
    }

    /*!
      \brief Get the sky positions for which the antenna gains are provided

      \return skyPositions -- Sky positions for which the antenna gains are
              provided
    */
    inline Vector<MVPosition> skyPositions () const {
      return skyPositions_p;
    }

    /*!
      \brief Get the frequencies for which the antenna gains are provided

      \return frequencies -- Frequencies for which the antenna gains are provided
    */
    inline Vector<MVFrequency> frequencies () const {
      return frequencies_p;
    }
    
    //! Complex antenna gains as function of direction and frequency
    Cube<casa::DComplex> gains ();
    
    /*!
      \brief Set the values of the antenna gains

      \param antennas       -- 
      \param skyPositions   -- 
      \param frequencies    -- 
      \param gains          -- 
      \param gainsMatchGrid -- 
    */
    void setGains (Vector<int> const &antennas,
		   Vector<MVPosition> const &skyPositions,
		   Vector<MVFrequency> const &frequencies,
		   Cube<casa::DComplex> const &gains,
		   bool const gainsMatchGrid=true);
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, AntennaGain.
    */
    inline std::string className () const {
      return "AntennaGain";
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
    
    /*!
      \brief Shape of the array storing the antenna gain values

      \return shape -- [freq,sky] 
    */
    inline IPosition shape () {
      return IPosition (2,
			frequencies_p.nelements(),
			skyPositions_p.nelements());
    }
    
  private:
    
    //! Unconditional copying
    void copy (AntennaGain const &other);
    
    //! Unconditional deletion 
    void destroy(void);

  }; // Class AntennaGain -- end
  
} // Namespace CR -- end

#endif /* ANTENNAGAIN_H */
  
