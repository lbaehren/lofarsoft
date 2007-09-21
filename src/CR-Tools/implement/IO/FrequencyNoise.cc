/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Tim Huege (<tim.huege@ik.fzk.de>)                                     *
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


#include <IO/FrequencyNoise.h>

namespace CR { // Namespace CR -- begin

const double FrequencyNoise::kB = 1.3806503e-23;	// Boltzmann constant in m^2 kg K^-1 Second^-2
//const double FrequencyNoise::impedance = 2.99792458e8*4.*M_PI*1.e-7;	// impedance of vacuum
const double FrequencyNoise::impedance = 50.0;				// impedance of LOPES antennas

  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  FrequencyNoise::FrequencyNoise ()
  {
  }
  
  // ==============================================================================
  //
  //  Destruction
  //
  // ==============================================================================
  
  FrequencyNoise::~FrequencyNoise ()
  {
  }
  
  // ==============================================================================
  //
  //  Operators
  //
  // ==============================================================================
  
  
  // ==============================================================================
  //
  //  Parameters
  //
  // ==============================================================================
  
  
  
  // ==============================================================================
  //
  //  Methods
  //
  // ==============================================================================
  

}  // Namespace CR -- end
