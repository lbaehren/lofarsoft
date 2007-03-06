/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

#include <iostream>
#include <string>

namespace CR {

  // ============================================================================
  //
  //  class TimeFreq -- Definition
  //
  // ============================================================================
  
  class TimeFreq {
    
    //! Blocksize, [samples]
    uint blocksize_p;
    //! Sample frequency in the ADC, [Hz]
    double sampleFrequency_p;
    //! Nyquist zone,  [1]
    uint nyquistZone_p;
    
  public:
    
    /*!
      Default constructor
    */
    TimeFreq ();
    
    /*!
      \brief Argumented constructor
    */
    TimeFreq (uint const &blocksize,
	      double const &sampleFrequency,
	      uint const &nyquistZone);
    
  };
  
  // ============================================================================
  //
  //  class TimeFreq -- Implementation
  //
  // ============================================================================
  
  TimeFreq::TimeFreq ()
  {
    blocksize_p       = 1;
    sampleFrequency_p = 80e06;
    nyquistZone_p     = 1;
  }
  
  TimeFreq::TimeFreq (uint const &blocksize,
		      double const &sampleFrequency,
		      uint const &nyquistZone)
  {
    blocksize_p       = blocksize;
    sampleFrequency_p = sampleFrequency;
    nyquistZone_p     = nyquistZone;
  }
  
}  // namespace CR -- end


int main ()
{
  return 0;
}
