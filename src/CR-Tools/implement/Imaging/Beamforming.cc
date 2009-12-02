/***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

#include <Imaging/Beamforming.h>

namespace CR { // Namespace CR -- begin

  // ============================================================================
  //
  //  Geometrical delays
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                             geometricalDelay

  /*!
    \brief Calculate the light travel delay, [sec].
    
    Calculate the light travel delay, \f$ \tau \f$, for a source at position
    \f$ \vec \rho \f$ and an antenna at position \f$ \vec x \f$ away from the
    phase center.

    \param azel            - Source position for which the geometric delay is 
                             computed.
    \param antennaPosition - 3D antenna position.
    \param nearField       - Use delay formula for a source in the near field?
  */
  double geometricalDelay (const casa::Vector<double>& azel,
			   const casa::Vector<double>& antPosition,
			   bool &nearField)
  {
    double delay       = 0.0;
    const Double c     = QC::c.getValue();
    Vector<Double> rho;
    
    // check if beam-position is suitable for near-field beam-forming 
    if (azel.nelements() < 3) {
      nearField = false;
      casa::Vector<double> tmp (3);
      tmp(0) = azel(0);
      tmp(1) = azel(1);
      tmp(2) = 1.0;
      CR::AzElRadius2Cartesian (rho,tmp);
    } else {
      CR::AzElRadius2Cartesian (rho,azel);
    }
    
    if (nearField && azel.nelements() == 3) {
      Vector<Double> diff = rho-antPosition;
      delay = (CR::L2Norm(diff)-CR::L2Norm(rho))/c;
    } else {
      Vector<Double> product = rho*antPosition;
      delay = -sum(product)/c;
    }
    
    return delay;
  }
  
  //_____________________________________________________________________________
  //                                                             geometricalDelay

  /*!
    Calculate the light travel delay, \f$ \tau \f$, for a source at position
    \f$ \vec \rho \f$ and an antenna at position \f$ \vec x \f$ away from the
    phase center; this method assumes delay computation for the far-field
    regime.
    
    \param direction    - Direction of the beam.
    \param antPositions - 3dim antenna position.
  */
  casa::Vector<double> geometricalDelay (const casa::Vector<double>& azel,
					 const casa::Matrix<double>& antPositions)
  {
    Bool nearField = False;
    IPosition shape = antPositions.shape();
    Vector<double> delays(shape(0));
    
    if (azel.nelements() == 3) {
      nearField = True;
    }
    
    for (int ant=0; ant<shape(0); ant++) {
      Vector<double> antennaPosition = antPositions.row(ant);
      delays(ant) = geometricalDelay (azel,antennaPosition,nearField);
    }
    
    return delays;
  }
  
} // Namespace LOPES -- end

