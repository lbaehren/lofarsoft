/***************************************************************************
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


#include <Observation/AntennaLayout.h>

namespace CR {  // namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  AntennaLayout::AntennaLayout ()
  {;}
  
  AntennaLayout::AntennaLayout (const String& tableFilename,
				const String& telescope,
				const Quantity& epoch)
  {
    obsInfo_p.setTelescope (telescope);
    obsInfo_p.setObsDate (epoch);
  }
  
  AntennaLayout::AntennaLayout (const String& tableFilename,
				const ObsInfo& obsInfo)
    : obsInfo_p(obsInfo)
  {;}
  
  
  // ============================================================================
  //  
  //  Object parameters
  //
  // ============================================================================
  
  void AntennaLayout::setEpoch (const MEpoch& epoch) 
  {
    obsInfo_p.setObsDate (epoch);
  }
  
  void AntennaLayout::setTelescope (const String& telescope)
  {
    obsInfo_p.setTelescope (telescope);
  }
  
  // =============================================================================
  // 
  
  Vector<double> AntennaLayout::antennaPosition (const int& antenna)
  {
    Vector<double> position;
    
    return position;
  }
  
  // ============================================================================
  //  Manipulation of the antenna layout table
  // ============================================================================
  
}  //  namespace CR -- end
