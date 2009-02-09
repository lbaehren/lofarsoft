/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
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

#include <Imaging/SkymapQuantity.h>

namespace CR {  // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  //______________________________________________________________ SkymapQuantity
  
  SkymapQuantity::SkymapQuantity ()
  {
    setSkymapQuantity (SkymapQuantity::FREQ_POWER);
  }
  
  //______________________________________________________________ SkymapQuantity
  
  SkymapQuantity::SkymapQuantity (SkymapQuantity::Type const &quantity)
  {
    setSkymapQuantity (quantity);
  }

  //______________________________________________________________ SkymapQuantity

  SkymapQuantity::SkymapQuantity (std::string const &domain,
				  std::string const &quantity)
  {
    SkymapQuantity::Type skymapType;
    bool status = getType (skymapType,
			   domain,
			   quantity);

    if (status) {
      setSkymapQuantity (skymapType);
    } else {
      setSkymapQuantity (SkymapQuantity::FREQ_POWER);
    }
  }
  
  //______________________________________________________________ SkymapQuantity
  
  SkymapQuantity::SkymapQuantity (SkymapQuantity const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  SkymapQuantity::~SkymapQuantity ()
  {
    destroy();
  }
  
  void SkymapQuantity::destroy ()
  {;}
  
  // ==============================================================================
  //
  //  Operators
  //
  // ==============================================================================
  
  SkymapQuantity& SkymapQuantity::operator= (SkymapQuantity const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  // ------------------------------------------------------------------------- copy
  
  void SkymapQuantity::copy (SkymapQuantity const &other)
  {
    coordType_p = other.coordType_p;
    quantity_p  = other.quantity_p;
  }
  
  // ==============================================================================
  //
  //  Parameters
  //
  // ==============================================================================
  
  void SkymapQuantity::setSkymapQuantity (SkymapQuantity::Type const &quantity)
  {
    quantity_p = quantity;
    
    // update the coordinate type
    switch (quantity_p) {
    case SkymapQuantity::TIME_FIELD:
    case SkymapQuantity::TIME_POWER:
    case SkymapQuantity::TIME_CC:
    case SkymapQuantity::TIME_X:
    case SkymapQuantity::TIME_P:
      coordType_p = CR::CoordinateType (CR::CoordinateType::Time);
      break;
    case SkymapQuantity::FREQ_POWER:
    case SkymapQuantity::FREQ_FIELD:
      coordType_p = CR::CoordinateType (CR::CoordinateType::Frequency);
      break;
    }
  }

  //_____________________________________________________________________________
  //                                                                      summary
  
  void SkymapQuantity::summary (std::ostream &os)
  {
    os << "[SkymapQuantity] Summary of internal parameters." << std::endl;
    os << "-- Domain type     = " << domainType() << std::endl;
    os << "-- Domain name     = " << domainName() << std::endl;
    os << "-- Skymap quantity = " << quantity () << std::endl;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                         type

  std::string SkymapQuantity::name ()
  {
    bool status (true);
    std::string domain;
    std::string quantity;

    status = getType (domain,
		      quantity,
		      quantity_p);

    std::string quantityName = domain+"_"+quantity;

    return quantityName;
  }
  
  //_____________________________________________________________________________
  //                                                                     quantity
  
  std::string SkymapQuantity::quantity () 
  {
    std::string quantity ("UNDEFINED");
    
    switch (quantity_p) {
    case TIME_FIELD:
    case FREQ_FIELD:
      quantity = "Field";
      break;
    case TIME_POWER:
    case FREQ_POWER:
      quantity = "Power";
      break;
    case TIME_CC:
      quantity = "CC";
      break;
    case TIME_X:
      quantity = "X";
      break;
    case TIME_P:
      quantity = "P";
      break;
    }
    
    return quantity;
  }

  //_____________________________________________________________________________
  //                                                                      getType
  
  bool SkymapQuantity::getType (SkymapQuantity::Type &skymapType,
				std::string const &domain,
				std::string const &quantity)
  {
    bool status (true);

    // domain==TIME
    if (domain == "time" || domain == "Time" || domain == "TIME") {
      if (quantity == "field" || quantity == "Field" || quantity == "FIELD") {
	skymapType = TIME_FIELD;
      } else if (quantity == "power" || quantity == "Power" || quantity == "POWER") {
	skymapType = TIME_POWER;
      } else if (quantity == "cc" || quantity == "CC") {
	skymapType = TIME_CC;
      } else if (quantity == "p" || quantity == "P") {
	skymapType = TIME_P;
      } else if (quantity == "x" || quantity == "X") {
	skymapType = TIME_X;
      } else {
	status = false;
      }
    }
    // domain==FREQ
    else if (domain == "freq" || domain == "Freq" || domain == "FREQ") {
      if (quantity == "field" || quantity == "Field" || quantity == "FIELD") {
	skymapType = FREQ_FIELD;
      } else if (quantity == "power" || quantity == "Power" || quantity == "POWER") {
	skymapType = FREQ_POWER;
      } else {
	status = false;
      }
    }
    else {
      status=false;
    }
    
  /*
    Feedback in case we were unable to construct a valid SkymapQuantity::Type
    from the provided input parameters
  */
  if (status == false) {
    std::cerr << "[SkymapQuantity::getType]"
	      << " Unable to construct valid SkymapQuantity::Type from"
	      << " domain=" << domain
	      << ", quantity=" << quantity
	      << std::endl;
  }
  
  return status;
}
  
  //_____________________________________________________________________________
  //                                                                      getType

  bool SkymapQuantity::getType (std::string &domain,
				std::string &quantity,
				SkymapQuantity::Type const &skymapType)
  {
    bool status (true);
    
    switch (skymapType) {
    case TIME_FIELD:
      domain   = "TIME";
      quantity = "FIELD";
      break;
    case TIME_POWER:
      domain   = "TIME";
      quantity = "POWER";
      break;
    case TIME_CC:
      domain   = "TIME";
      quantity = "CC";
      break;
    case TIME_P:
      domain   = "TIME";
      quantity = "P";
      break;
    case TIME_X:
      domain   = "TIME";
      quantity = "X";
      break;
    case FREQ_POWER:
      domain   = "FREQ";
      quantity = "POWER";
      break;
    case FREQ_FIELD:
      domain   = "FREQ";
      quantity = "FIELD";
      break;
    default:
      status = false;
      break;
    };

    return status;
  }
  
}  // Namespace CR -- end
