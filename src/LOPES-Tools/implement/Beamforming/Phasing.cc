/***************************************************************************
 *   Copyright (C) 2004-2006                                               *
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

/* $Id */

#include <Beamforming/Phasing.h>

namespace LOPES {  // namespace LOPES -- begin
  
  double Phasing::PI_p = 3.1415926535897932384626433832795;
  
  // =============================================================================
  //
  //  Phase gradients
  //
  // =============================================================================
  
  // ---------------------------------------------------------------- phaseGradient
  
  // Phase gradients for a single antenna; all directions & frequencies
  Matrix<DComplex> Phasing::phaseGradient (const Vector<double>& frequencies,
					   const Matrix<double>& directions,
					   const Vector<double>& antpos) 
  {
    IPosition shapeDirections = directions.shape();
    IPosition shapeFreq = frequencies.shape();
    Matrix<DComplex> gradients(shapeFreq(0),shapeDirections(0));
    double delay,phi;
    int dir (0);
    int freq (0);

    for (dir=0; dir<shapeDirections(0); dir++) {
      Vector<double> direction = directions.row(dir);
      // -> Phasing::geometricalDelay(Vector,Vector)
      delay = 2*PI_p*Phasing::geometricalDelay(direction,antpos);
      for (freq=0; freq<shapeFreq(0); freq++) {
	phi = frequencies(freq)*delay;
	gradients(freq,dir) = DComplex(cos(phi),sin(phi));
      }
    }
    
    return gradients;
  }
  
  // ---------------------------------------------------------------- phaseGradient
  
  /* Phase gradients for a single direction; all frequencies & antennae. */
  Matrix<DComplex> Phasing::phaseGradient (const Vector<double>& frequencies,
					   const Vector<double>& direction,
					   const Matrix<double>& antennaPositions)
  {
    IPosition shapeDirection = direction.shape();
    IPosition shapeAntpos = antennaPositions.shape();
    IPosition shapeFreq = frequencies.shape();
    Matrix<DComplex> gradients(shapeFreq(0),shapeAntpos(0));
    Vector<DComplex> gradient(shapeAntpos(0));
    
    for (int freq=0; freq<shapeFreq(0); freq++) {
      gradient = Phasing::phaseGradient (frequencies(freq),direction,antennaPositions);
      gradients.row(freq) = gradient;
    }
    
    return gradients;
    
  }
  
  // ---------------------------------------------------------------- phaseGradient
  
  /* Phase gradients for a single frequency & direction; all antennae. */
  Vector<DComplex> Phasing::phaseGradient (const double frequency,
					   const Vector<double>& direction,
					   const Matrix<double>& antennaPositions)
  {
    IPosition shapeDirection (direction.shape());
    IPosition shapeAntpos (antennaPositions.shape());
    Vector<DComplex> gradients(shapeAntpos(0));
    
    for (int ant=0; ant<shapeAntpos(0); ant++) {
      Vector<double> antpos = antennaPositions.row(ant);
      gradients(ant) = Phasing::phaseGradient (frequency,direction,antpos);
    }
    
    return gradients;
    
  }
  
  // ---------------------------------------------------------------- phaseGradient
  
  /* Phase gradients for a single frequency, direction and antenna. */
  DComplex Phasing::phaseGradient (const double frequency,
				   const Vector<double>& azel,
				   const Vector<double>& antpos)
  {
    double phi;
    DComplex grad;
    
    // delay phase
    // sign of phase is still under debate: possible (-1) here
    phi  = 2*PI_p*frequency*Phasing::geometricalDelay(azel,antpos);
    grad = DComplex(cos(phi),sin(phi));
    
    // phase factor
    return grad;
  }
  
  // =============================================================================
  //
  //  Geometrical delays
  //
  // =============================================================================
  
  // ------------------------------------------------------------- geometricalDelay
  
  Vector<double> Phasing::geometricalDelay (const Vector<double>& azel,
					    const Matrix<double>& antennaPositions)
  {
    bool nearField = false;
    IPosition shape = antennaPositions.shape();
    Vector<double> delays(shape(0));
    
    if (azel.nelements() == 3) {
      nearField = true;
    }
    
    for (int ant=0; ant<shape(0); ant++) {
      Vector<double> antennaPosition = antennaPositions.row(ant);
      delays(ant) = Phasing::geometricalDelay (azel,antennaPosition,nearField);
    }
    
    return delays;
  }
  
  // ------------------------------------------------------------- geometricalDelay
  
  double Phasing::geometricalDelay (const Vector<double>& azel,
				    const Vector<double>& antennaPosition)
  {
    bool nearField (false);
    
    if (azel.nelements() == 3) {
      nearField = true;
    }
    
    return Phasing::geometricalDelay (azel,antennaPosition,nearField);
  }
  
  // ------------------------------------------------------------- geometricalDelay
  
  double Phasing::geometricalDelay (const Vector<double>& azel,
				    const Vector<double>& baseline,
				    bool nearField=false)
  {
    const double c = QC::c.getValue();
    Vector<double> rho = LOPES::azel2cartesian (azel);
    double delay;
    
    if (nearField && azel.nelements() == 3) {
      Vector<double> diff = rho-baseline;
      delay = (LOPES::L2Norm(diff)-LOPES::L2Norm(rho))/c;
    } else {
      Vector<double> product = rho*baseline;
      delay = -sum(product)/c;
    }
    
    // Debugging output
    //  cout << "[Phasing] : " << azel << "\t" << baseline << "\t" << delay << endl;
    
    /* Check if this sign inversion is indeed correct; it is present in 
       'angle2delay' (map.g), but not in 'calcdelay' (phasing.g). */
    
    return delay;
  }
  
}  // namespace LOPES -- end
