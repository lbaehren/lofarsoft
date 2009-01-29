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

#include <Imaging/AntennaGain.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                  AntennaGain

  AntennaGain::AntennaGain ()
  {
    antennas_p     = Vector<int>(1);
    skyPositions_p = Vector<MVPosition>(1);
    frequencies_p  = Vector<MVFrequency>(1);
    gains_p        = Cube<casa::DComplex>(1,1,1);
  }
  
  //_____________________________________________________________________________
  //                                                                  AntennaGain
  
  AntennaGain::AntennaGain (Vector<int> const &antennas,
			    Vector<MVPosition> const &skyPositions,
			    Vector<MVFrequency> const &frequencies,
			    Cube<casa::DComplex> const &gains)
  {
    setGains (antennas,
	      skyPositions,
	      frequencies,
	      gains);
  }
  
  //_____________________________________________________________________________
  //                                                                  AntennaGain

  AntennaGain::AntennaGain (AntennaGain const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  AntennaGain::~AntennaGain ()
  {
    destroy();
  }
  
  void AntennaGain::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  AntennaGain& AntennaGain::operator= (AntennaGain const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void AntennaGain::copy (AntennaGain const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                      summary
  
  void AntennaGain::summary (std::ostream &os)
  {
    os << "[AntennaGain] Summary of internal parameters." << std::endl;
    os << "-- nof. antennas           = " << antennas_p.shape()     << std::endl;
    os << "-- nof. sky positions      = " << skyPositions_p.shape() << std::endl;
    os << "-- nof. frequency channels = " << frequencies_p.shape()  << std::endl;
    os << "-- Antenna gain values     = " << gains_p.shape()        << std::endl;
  }

  //_____________________________________________________________________________
  //                                                                        gains
  
  void AntennaGain::setGains (Vector<int> const &antennas,
			      Vector<MVPosition> const &skyPositions,
			      Vector<MVFrequency> const &frequencies,
			      Cube<casa::DComplex> const &gains)
  {
    antennas_p.resize(antennas.shape());
    antennas_p = antennas;

    skyPositions_p.resize(skyPositions.shape());
    skyPositions_p = skyPositions;
    
    frequencies_p.resize(frequencies.shape());
    frequencies_p = frequencies;

    gains_p.resize(gains.shape());
    gains_p = gains;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  

} // Namespace CR -- end
