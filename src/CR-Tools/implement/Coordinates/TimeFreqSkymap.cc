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

#include <Coordinates/TimeFreqSkymap.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  // ------------------------------------------------------------- TimeFreqSkymap
  
  TimeFreqSkymap::TimeFreqSkymap ()
    : TimeFreq(),
      beamDomain_p (CoordinateDomain(CoordinateDomain::Frequency))
  {
    setBlocksPerFrame (1);
    setNofFrames (1);
  }
  
  // ------------------------------------------------------------- TimeFreqSkymap
  
  TimeFreqSkymap::TimeFreqSkymap (uint const &blocksPerFrame,
				  uint const &nofFrames)
    : TimeFreq(),
      beamDomain_p (CoordinateDomain(CoordinateDomain::Frequency))
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
	       nyquistZone),
      beamDomain_p (CoordinateDomain(CoordinateDomain::Frequency))
  {
    setBlocksPerFrame (blocksPerFrame);
    setNofFrames (nofFrames);
  }
  
  // ------------------------------------------------------------- TimeFreqSkymap
  
  TimeFreqSkymap::TimeFreqSkymap (TimeFreqSkymap const &other)
    : TimeFreq(other),
      beamDomain_p (CoordinateDomain(CoordinateDomain::Frequency))
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

    beamDomain_p     = other.beamDomain_p;
    blocksPerFrame_p = other.blocksPerFrame_p;
    nofFrames_p      = other.nofFrames_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  // -------------------------------------------------------------- setBeamDomain

  bool TimeFreqSkymap::setBeamDomain (CR::CoordinateDomain::Types const &type)
  {
    return setBeamDomain (CoordinateDomain(type));
  }
  
  // -------------------------------------------------------------- setBeamDomain

  bool TimeFreqSkymap::setBeamDomain (CR::CoordinateDomain const &domain)
  {
    bool status (true);

    switch (domain.type()) {
    case CoordinateDomain::Time:
      break;
    case CoordinateDomain::Frequency:
      break;
    default:
      break;
    }
    
    return status;
  }
  
  // ------------------------------------------------------------------ axisShape
  
#ifdef HAVE_CASA
  casa::IPosition TimeFreqSkymap::axisShape () const
  {
    casa::IPosition shape(2);
    
    switch (beamDomain_p.type()) {
    case CoordinateDomain::Time:
      shape(0) = blocksize_p*nofFrames_p;
      shape(1) = 1;
      break;
    case CoordinateDomain::Frequency: {
      shape(0) = nofFrames_p;
      shape(1) = fftLength_p;
    }
      break;
    default:
      std::cerr << "[TimeFreqSkymap::axisShape] Invalid domain!" << std::endl;
      break;
    };
    
    return shape;
  }
#else 
  vector<int> TimeFreqSkymap::axisShape () const
  {
    vector<int> shape(2);

    return shape;
  }
#endif

  // -------------------------------------------------------------------- summary
  
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
