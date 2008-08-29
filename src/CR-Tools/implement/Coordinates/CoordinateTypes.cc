/*-------------------------------------------------------------------------*
 | $Id:: NewClass.cc 1159 2007-12-21 15:40:14Z baehren                   $ |
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

#include <Coordinates/CoordinateTypes.h>

namespace CR { // Namespace CR -- begin

  void CoordinateTypes::copy (CoordinateTypes const &other)
  {
    type_p = other.type_p;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  bool hasProjection (CoordinateTypes::Type const &coord)
  {
    switch (coord) {
    case CoordinateTypes::Direction:
    case CoordinateTypes::DirectionRadius:
      return true;
      break;
    default:
      return false;
      break;
    }
  }

  // ---------------------------------------------------------------- isCartesian

  bool isCartesian (CoordinateTypes::Type const &coord)
  {
    switch (coord) {
    case CoordinateTypes::Cartesian:
      return true;
      break;
    default:
      return false;
      break;
    }
  }
  
  // ---------------------------------------------------------------- isSpherical

  bool isSpherical (CoordinateTypes::Type const &coord)
  {
    switch (coord) {
    case CoordinateTypes::Spherical:
      return true;
      break;
    default:
      return false;
      break;
    }
  }

} // Namespace CR -- end
