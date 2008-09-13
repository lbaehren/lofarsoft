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
  
  TimeFreqSkymap::TimeFreqSkymap (uint const &blocksize,
				  casa::Quantity const &sampleFrequency,
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
  
  TimeFreqSkymap::TimeFreqSkymap (TimeFreq const &timeFreq,
				  uint const &blocksPerFrame,
				  uint const &nofFrames)
    : TimeFreq(timeFreq),
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
      if (blocksPerFrame_p == 1) {
	beamDomain_p = domain;
      } else {
	std::cerr << "[TimeFreqSkymap::setBeamDomain] " 
		  << "Inconsistent combination of parameter values!" << std::endl;
	status = false;
      }
      break;
    case CoordinateDomain::Frequency:
      beamDomain_p = domain;
      break;
    default:
      std::cerr << "[TimeFreqSkymap::setBeamDomain] Invalid domain!" << std::endl;
      status = false;
      break;
    }
    
    return status;
  }

  // ---------------------------------------------------------- setBlocksPerFrame

  bool TimeFreqSkymap::setBlocksPerFrame (uint const &blocksPerFrame,
					  bool const &adjustDomain)
  {
    bool status (true);

    switch (beamDomain_p.type()) {
    case CoordinateDomain::Time:
      if (blocksPerFrame > 1 && adjustDomain) {
	blocksPerFrame_p = blocksPerFrame;
	beamDomain_p.setDomain (CoordinateDomain::Frequency);
      } else {
	blocksPerFrame_p = blocksPerFrame;
      }
      break;
    default:
      blocksPerFrame_p = blocksPerFrame;
      break;
    }
    
    return status;
  }
  
  // -------------------------------------------------------------------- summary
  
  void TimeFreqSkymap::summary (std::ostream &os)
  {
    os << "[TimeFreqSkymap] Summary of internal parameters."    << std::endl;
    os << "-- Blocksize            = " << blocksize_p           << std::endl;
    os << "-- Sample frequency     = " << sampleFrequency_p     << std::endl;
    os << "-- Nyquist zone         = " << nyquistZone_p         << std::endl;
    os << "-- Reference time       = " << referenceTime_p       << std::endl;
    os << "-- Target domain        = " << beamDomain_p.name()   << std::endl;
    os << "-- Blocks per frame     = " << blocksPerFrame_p      << std::endl;
    os << "-- nof. frames          = " << nofFrames_p           << std::endl;
    
#ifdef HAVE_CASA
    os << "-- Shape of the axes    = " << shape()               << std::endl;
    os << "-- Axis value increment = " << increment()           << std::endl;
#else
    os << "-- Shape of the axes    = [" << shape()[0] << ","
       << shape()[1] << std::endl;
    os << "-- Axis value increment = [" << increment()[0] << ","
       << increment()[1] << std::endl;
#endif
  }
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  // ---------------------------------------------------------------------- shape
  
#ifdef HAVE_CASA
  casa::IPosition TimeFreqSkymap::shape () const
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
      std::cerr << "[TimeFreqSkymap::shape] Invalid domain!" << std::endl;
      break;
    };
    
    return shape;
  }
#else 
  vector<int> TimeFreqSkymap::shape () const
  {
    vector<int> shape(2);
 
    switch (beamDomain_p.type()) {
    case CoordinateDomain::Time:
      shape[0] = blocksize_p*nofFrames_p;
      shape[1] = 1;
      break;
    case CoordinateDomain::Frequency: {
      shape[0] = nofFrames_p;
      shape[1] = fftLength_p;
    }
      break;
    default:
      std::cerr << "[TimeFreqSkymap::shape] Invalid domain!" << std::endl;
      break;
    };
    
    return shape;
  }
#endif

  // ------------------------------------------------------------------ increment
  
#ifdef HAVE_CASA
  casa::Vector<double> TimeFreqSkymap::increment () const
  {
    casa::Vector<double> vec (2);
    
    switch (beamDomain_p.type()) {
    case CoordinateDomain::Time:
      vec(0) = 1/sampleFrequency_p;
      vec(1) = sampleFrequency_p/2;
      break;
    case CoordinateDomain::Frequency:
      vec(0) = blocksize_p*blocksPerFrame_p*nofFrames_p/sampleFrequency_p;
      vec(1) = sampleFrequency_p/blocksize_p;
      break;
    default:
      std::cerr << "[TimeFreqSkymap::increment] Invalid domain!" << std::endl;
      break;
    };
    
    return vec;
  }
#else 
  vector<double> TimeFreqSkymap::increment () const
  {
    vector<double> vec (2);

    switch (beamDomain_p.type()) {
    case CoordinateDomain::Time:
      vec[0] = 1/sampleFrequency_p;
      vec[1] = sampleFrequency_p/2;
      break;
    case CoordinateDomain::Frequency:
      vec[0] = blocksize_p*blocksPerFrame_p*nofFrames_p/sampleFrequency_p;
      vec[1] = sampleFrequency_p/blocksize_p;
      break;
    default:
      std::cerr << "[TimeFreqSkymap::increment] Invalid domain!" << std::endl;
      break;
    };
    
    return vec;
  }
#endif  
  
} // Namespace CR -- end
