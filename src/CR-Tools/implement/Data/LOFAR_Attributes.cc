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

#include <Data/LOFAR_Attributes.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  std::string attribute_name (Attributes const &num)
  {
    std::string name;
    
    switch (num) {
      // --- General attributes for telescope and observation --------
    case TELESCOPE:
      name = "TELESCOPE";
      break;
    case OBSERVER:
      name = "OBSERVER";
      break;
    case PROJECT:
      return "PROJECT";
      break;
    case OBSERVATION_ID:
      return "OBSERVATION_ID";
      break;
    case OBSERVATION_MODE:
      return "OBSERVATION_MODE";
      break;
    case TIME:
      return "TIME";
      break;
      // --- Attributes for an individual LOFAR station --------------
    case STATION_ID:
      return "STATION_ID";
      break;
      // --- Attributes for an individual signal channel (dipole) ----
    case RSP_ID:
      return "RSP_ID";
      break;
    case RCU_ID:
      return "RCU_ID";
      break;
    }

    return name;
  }


} // Namespace CR -- end
