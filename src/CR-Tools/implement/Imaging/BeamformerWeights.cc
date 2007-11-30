/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                  *
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

#include <Imaging/BeamformerWeights.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  BeamformerWeights::BeamformerWeights ()
    : GeometricalWeight ()
  {;}

  BeamformerWeights::BeamformerWeights(casa::Matrix<double> const &antPositions,
				       CR::CoordinateType const &antCoordType,
				       casa::Matrix<double> const &skyPositions,
				       CR::CoordinateType const &skyCoordType,
				       casa::Vector<double> const &frequencies,
				       bool const &bufferDelays,
				       bool const &bufferPhases,
				       bool const &bufferWeights)
    : GeometricalWeight (antPositions,
			 antCoordType,
			 skyPositions,
			 skyCoordType,
			 frequencies,
			 bufferDelays,
			 bufferPhases,
			 bufferWeights)
  {
  }
  
  BeamformerWeights::BeamformerWeights (BeamformerWeights const &other)
    : GeometricalWeight ()
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  BeamformerWeights::~BeamformerWeights ()
  {
    destroy();
  }
  
  void BeamformerWeights::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  BeamformerWeights& BeamformerWeights::operator= (BeamformerWeights const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void BeamformerWeights::copy (BeamformerWeights const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void BeamformerWeights::summary (std::ostream &os)
  {
    GeometricalWeight::summary(os);
  }
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  

} // Namespace CR -- end
