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

namespace LOFAR { // Namespace LOFAR -- begin
  
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
      name = "PROJECT";
      break;
    case OBSERVATION_ID:
      name = "OBSERVATION_ID";
      break;
    case OBSERVATION_MODE:
      name = "OBSERVATION_MODE";
      break;
      // --- Attributes for an individual LOFAR station --------------
    case STATION_ID:
      name = "STATION_ID";
      break;
      // --- Attributes for an individual signal channel (dipole) ----
    case RSP_ID:
      name = "RSP_ID";
      break;
    case RCU_ID:
      name = "RCU_ID";
      break;
    case TIME:
      name = "TIME";
      break;
    case SAMPLE_NUMBER:
      name = "SAMPLE_NUMBER";
      break;
    case SAMPLE_FREQUENCY:
      name = "SAMPLE_FREQUENCY";
      break;
    case NYQUIST_ZONE:
      name = "NYQUIST_ZONE";
      break;
    case SAMPLES_PER_FRAME:
      name = "SAMPLES_PER_FRAME";
      break;
    case DATA_LENGTH:
      name = "DATA_LENGTH";
      break;
    case FEED:
      name = "FEED";
      break;
    case ANTENNA_POSITION:
      name = "ANTENNA_POSITION";
      break;
    case ANTENNA_ORIENTATION:
      name = "ANTENNA_ORIENTATION";
      break;
      // --- Trigger algorithm ---------------------------------------
    case TRIGGER_TYPE:
      name = "TRIGGER_TYPE";
      break;
    case TRIGGER_OFFSET:
      name = "TRIGGER_OFFSET";
      break;
    case TRIGGERED_ANTENNAS:
      name = "TRIGGERED_ANTENNAS";
      break;
    }
    
    return name;
  }
  
  
} // Namespace LOFAR -- end
