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

#include <Imaging/SkymapOrientation.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  SkymapOrientation::SkymapOrientation (SkymapOrientation const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  SkymapOrientation::~SkymapOrientation ()
  {
    destroy();
  }
  
  void SkymapOrientation::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  SkymapOrientation& SkymapOrientation::operator= (SkymapOrientation const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void SkymapOrientation::copy (SkymapOrientation const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void SkymapOrientation::summary (std::ostream &os)
  {
    os << "[SkymapOrientation] Summary of internal parameters." << std::endl;
    os << "-- North is up   = " << northIsUp_p   << std::endl;
    os << "-- East is right = " << eastIsRight_p << std::endl;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  std::string SkymapOrientation::orientation ()
  {
    if (northIsUp_p) {
      if (eastIsRight_p) {
	return "NORTH_EAST";
      } else {
	return "NORTH_WEST";
      }
    }
    else {
      if (eastIsRight_p) {
	return "SOUTH_EAST";
      } else {
	return "SOUTH_WEST";
      }
    }
  }

} // Namespace CR -- end
