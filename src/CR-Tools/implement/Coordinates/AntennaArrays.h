/*-------------------------------------------------------------------------*
 | $Id:: NewClass.h 2286 2009-02-03 10:50:48Z baehren                    $ |
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

#ifndef ANTENNAARRAYS_H
#define ANTENNAARRAYS_H

// Standard library header files
#include <iostream>
#include <string>
#include <vector>

// CR-Tools header files
#include <Coordinates/AntennaArray.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class AntennaArrays
    
    \ingroup CR_Coordinates
    
    \brief Container for collecting the information on multiple antenna arrays
    
    \author Lars B&auml;hren

    \date 2009/03/03

    \test tAntennaArrays.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>CR::AntennaArray
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class AntennaArrays {

    std::vector<CR::AntennaArray> arrays_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    AntennaArrays ();
    
    /*!
      \brief Argumented constructor
      
      \param arrays -- Set of antenna arrays to be stored
    */
    AntennaArrays (std::vector<CR::AntennaArray> const &arrays);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another AntennaArrays object from which to create this new
             one.
    */
    AntennaArrays (AntennaArrays const &other);
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    ~AntennaArrays ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another AntennaArrays object from which to make a copy.
    */
    AntennaArrays& operator= (AntennaArrays const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, AntennaArrays.
    */
    inline std::string className () const {
      return "AntennaArrays";
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

    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Add an AntennaArray to the list of stored objects

      \param arr -- 

      \return status -- Status of the operation; return \e false in case an error
              was encountered.
    */
    bool addArray (CR::AntennaArray const &arr);
    
  private:
    
    //! Unconditional copying
    void copy (AntennaArrays const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  }; // Class AntennaArrays -- end
  
} // Namespace CR -- end

#endif /* ANTENNAARRAYS_H */
  
