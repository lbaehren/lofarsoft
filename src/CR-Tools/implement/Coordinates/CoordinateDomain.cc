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

#include <Coordinates/CoordinateDomain.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  CoordinateDomain::CoordinateDomain (CoordinateDomain::Types const &type)
  {
    setDomain (type);
  }

  CoordinateDomain::CoordinateDomain (std::string const &domain)
  {
    if (!setDomain (domain)) {
      setDomain("Time");
    }
  }
  
  CoordinateDomain::CoordinateDomain (CoordinateDomain const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  CoordinateDomain::~CoordinateDomain ()
  {
    destroy();
  }
  
  void CoordinateDomain::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  CoordinateDomain& CoordinateDomain::operator= (CoordinateDomain const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void CoordinateDomain::copy (CoordinateDomain const &other)
  {
    type_p = other.type_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  // --------------------------------------------------------------------- domain

  std::string CoordinateDomain::name (CoordinateDomain::Types const &type) 
  {
    switch (type) {
    case CoordinateDomain::Time:
      return std::string ("Time");
      break;
    case CoordinateDomain::Frequency:
      return std::string ("Frequency");
      break;
    case CoordinateDomain::Space:
      return std::string ("Space");
      break;
    default:
      return std::string ("UNDEFINED");
      break;
    }
  }

  // ------------------------------------------------------------------ setDomain

  bool CoordinateDomain::setDomain (CoordinateDomain::Types const &type)
  {
    switch (type) {
    case CoordinateDomain::Time:
    case CoordinateDomain::Frequency:
    case CoordinateDomain::Space:
      type_p = type;
      return true;
      break;
    default:
      std::cerr << "[CoordinateDomain::setDomain] Unknown domain!" << std::endl;
      return false;
      break;
    }
  }

  // ------------------------------------------------------------------ setDomain

  bool CoordinateDomain::setDomain (std::string const &domain)
  {
    bool status (true);
    
    if (domain == "Time") {
      type_p = CoordinateDomain::Time;
    }
    else if (domain == "Frequency") {
      type_p = CoordinateDomain::Frequency;
    }
    else if (domain == "Space") {
      type_p = CoordinateDomain::Space;
    } 
    else {
      std::cerr << "[CoordinateDomain::setDomain] Invalid domain name!"
		<< std::endl;
      status = false;
    }
    
    return status;
  }
  
  // ------------------------------------------------------------------ isSpatial

  bool CoordinateDomain::isSpatial (CoordinateDomain::Types const &type)
  {
    switch (type) {
    case CoordinateDomain::Space:
      return true;
      break;
    default:
      return false;
      break;
    }
  }
  
  // ----------------------------------------------------------------- isTemporal

  bool CoordinateDomain::isTemporal (CoordinateDomain::Types const &type)
  {
    switch (type) {
    case CoordinateDomain::Time:
      return true;
      break;
    default:
      return false;
      break;
    }
  }  

  // ----------------------------------------------------------------- isSpectral

  bool CoordinateDomain::isSpectral (CoordinateDomain::Types const &type)
  {
    switch (type) {
    case CoordinateDomain::Frequency:
      return true;
      break;
    default:
      return false;
      break;
    }
  }
  
  // -------------------------------------------------------------------- summary

  void CoordinateDomain::summary (std::ostream &os)
  {
    os << "[CoordinateDomain] Summary of internal parameters." << std::endl;
    os << "-- Domain type     = " << type()       << std::endl;
    os << "-- Domain name     = " << name()       << std::endl;
    os << "-- Base unit       = " << baseUnit()   << std::endl;
    os << "-- Temporal domain = " << isTemporal() << std::endl;
    os << "-- Spectral domain = " << isSpectral() << std::endl;
    os << "-- Spatial domain  = " << isSpatial()  << std::endl;
  }
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  std::string CoordinateDomain::baseUnit (CoordinateDomain::Types const &type)
  {
    switch (type) {
    case CoordinateDomain::Time:
      return std::string ("s");
      break;
    case CoordinateDomain::Frequency:
      return std::string ("Hz");
      break;
    case CoordinateDomain::Space:
      return std::string ("m");
      break;
    default:
      return std::string ("UNDEFINED");
      break;
    }
  }


} // Namespace CR -- end
