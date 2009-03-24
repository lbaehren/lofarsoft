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

#include <Data/DataType.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  DataType::DataType (DataType::Types const &dataType)
  {
    if (!setType (dataType)) {
      std::cerr << "[DataType] Constructor called with unsupported data type!"
		<< " Using default value..."
		<< std::endl;
      setType (DataType::HDF5);
    }
  }
  
  DataType::DataType (DataType const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  DataType::~DataType ()
  {
    destroy();
  }
  
  void DataType::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  DataType& DataType::operator= (DataType const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void DataType::copy (DataType const &other)
  {
    type_p = other.type_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  bool DataType::setType (DataType::Types const &dataType)
  {
    bool status (true);

    switch (dataType) {
    case DataType::CASA_IMAGE:
    case DataType::CASA_MS:
    case DataType::FITS:
    case DataType::HDF5:
    case DataType::LOFAR_BF:
    case DataType::LOFAR_TBB:
    case DataType::TIM40:
      type_p = dataType;
      break;
    default:
      status = false;
      break;
    };

    return status;
  }

  
  void DataType::summary (std::ostream &os)
  {
    os << "[DataType] Summary of internal parameters." << std::endl;
    os << "-- Type = " << type() << std::endl;
    os << "-- Name = " << name() << std::endl;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  

} // Namespace CR -- end
