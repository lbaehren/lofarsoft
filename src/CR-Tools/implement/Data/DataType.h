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

#ifndef DATATYPE_H
#define DATATYPE_H

// Standard library header files
#include <iostream>
#include <string>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class DataType
    
    \ingroup CR_Data
    
    \brief Brief description for class DataType
    
    \author Lars B&auml;hren

    \date 2009/03/24

    \test tDataType.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>Some basic knowledge about the data types available through \e casacore,
      \e CFITSIO and the \e DAL.
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>

    <ol>
      <li>Register casacore MeasurementSet:
      \code
      // Create the object
      DataType t (DataType::CASA_MS);
      // Provide a summary of the previously created object
      t.summary();
      \endcode
      <li>Use the data type as a switch to decide on how to write out data:
      \code
    switch (imageType_p) {
    case DataType::HDF5:
      image_p = new casa::HDF5Image<float> (tile,
					    csys,
					    filename_p);
      break;
    case DataType::CASA_IMAGE:
      image_p = new casa::PagedImage<float> (tile,
					     csys,
					     filename_p);
      break;
    default:
      status = false;
      break;
    };
      \endcode
    </ol>
    
  */  
  class DataType {
    
  public:

    //! Enumeration listing the possible data types
    enum Types {
      //! casacore PagedImage
      CASA_IMAGE,
      //! casacore MeasurementSet
      CASA_MS,
      //! FITS image
      FITS,
      //! HDF5 file; this can be a number of things, depending on the context.
      HDF5,
      //! LOFAR beam-formed data, stored in an HDF5 file
      LOFAR_BF,
      //! LOFAR TBB data, stored in an HDF5 file
      LOFAR_TBB,
      //! TIM40 data from LOPES
      TIM40
    };
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Constructor

      \param dataType -- The identifier for the type of the data
    */
    DataType (DataType::Types const &dataType);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another DataType object from which to create this new
             one.
    */
    DataType (DataType const &other);
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    ~DataType ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another DataType object from which to make a copy.
    */
    DataType& operator= (DataType const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the type identifier

      \return type -- The type identifier of the data
    */
    inline DataType::Types type () const {
      return type_p;
    }

    /*!
      \brief Set the type of the data

      \param dataType -- The identifier for the type of the data

      \return status -- Status of the operation; returns \e false in case an
              error was encountered (e.g. an invalid type was provided)
    */
    bool setType (DataType::Types const &dataType);
    
    /*!
      \brief Get the name of the data type
      
      \return name -- The name of the data type.
    */
    inline std::string name ()
    {
      std::string itsName;

      switch (type_p) {
      case DataType::CASA_IMAGE:
	itsName = "CASA_IMAGE";
	break;
      case DataType::CASA_MS:
	itsName = "CASA_MS";
	break;
      case DataType::FITS:
	itsName = "FITS";
	break;
      case DataType::HDF5:
	itsName = "HDF5";
	break;
      case DataType::LOFAR_BF:
	itsName = "LOFAR_BF";
	break;
      case DataType::LOFAR_TBB:
	itsName = "LOFAR_TBB";
	break;
      case DataType::TIM40:
	itsName = "TIM40";
	break;
      };
      
      return itsName;
    }
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, DataType.
    */
    inline std::string className () const {
      return "DataType";
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

    //___________________________________________________________________________
    //                                                             Public methods
    
    
    
  private:

    //___________________________________________________________________________
    //                                                          Private variables
    
    //! Type of data
    DataType::Types type_p;
    
    //___________________________________________________________________________
    //                                                            Private methods
    
    //! Unconditional copying
    void copy (DataType const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  }; // Class DataType -- end
  
} // Namespace CR -- end

#endif /* DATATYPE_H */
  
