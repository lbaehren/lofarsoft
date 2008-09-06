/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
 *   Lars Baehren (<mail>)                                                     *
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

#include <Coordinates/TimeFreqSkymap.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  // ------------------------------------------------------------- TimeFreqSkymap
  
  TimeFreqSkymap::TimeFreqSkymap ()
    : TimeFreq()
  {
    setBlocksPerFrame (1);
    setNofFrames (1);
  }
  
  // ------------------------------------------------------------- TimeFreqSkymap
  
  TimeFreqSkymap::TimeFreqSkymap (uint const &blocksPerFrame,
				  uint const &nofFrames)
    : TimeFreq()
  {
    setBlocksPerFrame (blocksPerFrame);
    setNofFrames (nofFrames);
  }
  
  // ------------------------------------------------------------- TimeFreqSkymap
  
  TimeFreqSkymap::TimeFreqSkymap (uint const &blocksize,
				  double const &sampleFrequency,
				  uint const &nyquistZone,
				  uint const &blocksPerFrame,
				  uint const &nofFrames)
    : TimeFreq(blocksize,
	       sampleFrequency,
	       nyquistZone)
  {
    setBlocksPerFrame (blocksPerFrame);
    setNofFrames (nofFrames);
  }
  
  // ------------------------------------------------------------- TimeFreqSkymap
  
  TimeFreqSkymap::TimeFreqSkymap (TimeFreqSkymap const &other)
    : TimeFreq(other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  TimeFreqSkymap::~TimeFreqSkymap ()
  {
    destroy();
  }
  
  void TimeFreqSkymap::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  TimeFreqSkymap& TimeFreqSkymap::operator= (TimeFreqSkymap const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void TimeFreqSkymap::copy (TimeFreqSkymap const &other)
  {
    TimeFreq::operator= (other);

    blocksPerFrame_p = other.blocksPerFrame_p;
    nofFrames_p      = other.nofFrames_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void TimeFreqSkymap::summary (std::ostream &os)
  {
    os << "[TimeFreqSkymap] Summary of internal parameters." << std::endl;
    os << "-- Blocksize        = " << blocksize_p       << std::endl;
    os << "-- Sample frequency = " << sampleFrequency_p << std::endl;
    os << "-- Nyquist zone     = " << nyquistZone_p     << std::endl;
    os << "-- Reference time   = " << referenceTime_p   << std::endl;
    os << "-- Blocks per frame = " << blocksPerFrame_p  << std::endl;
    os << "-- nof. frames      = " << nofFrames_p       << std::endl;
  }
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  

} // Namespace CR -- end
