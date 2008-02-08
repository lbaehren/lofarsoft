/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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

#ifndef LOFAR_TIMESERIES_H
#define LOFAR_TIMESERIES_H

// Standard library header files
#include <iostream>
#include <string>
#include <vector>

// Header files of the HDF5 library
#include <hdf5/H5LT.h>
#include <hdf5/H5TA.h>

namespace LOFAR { // Namespace LOFAR -- begin
  
  /*!
    \class LOFAR_Timeseries
    
    \ingroup LOFAR
    
    \brief Brief description for class LOFAR_Timeseries
    
    \author Lars B&auml;hren

    \date 2008/02/06

    \test tLOFAR_Timeseries.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class LOFAR_Timeseries {

    //! Name of the data file
    std::string filename_p;
    //! File handle identifier
    hid_t fileID_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    LOFAR_Timeseries ();
    
    /*!
      \brief Default constructor

      \param filename -- Name of the data file
    */
    LOFAR_Timeseries (std::string const &filename);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another LOFAR_Timeseries object from which to create this new
      one.
    */
    LOFAR_Timeseries (LOFAR_Timeseries const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~LOFAR_Timeseries ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another LOFAR_Timeseries object from which to make a copy.
    */
    LOFAR_Timeseries& operator= (LOFAR_Timeseries const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, LOFAR_Timeseries.
    */
    std::string className () const {
      return "LOFAR_Timeseries";
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
    void copy (LOFAR_Timeseries const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace LOFAR -- end

#endif /* LOFAR_TIMESERIES_H */
  
