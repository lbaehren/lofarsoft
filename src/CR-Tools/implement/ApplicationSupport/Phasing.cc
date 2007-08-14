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

#include <ApplicationSupport/Phasing.h>
#include <Math/VectorNorms.h>

Double Phasing::PI_p = 3.1415926535897932384626433832795;

// =============================================================================
//
//  Phase gradients
//
// =============================================================================

// ---------------------------------------------------------------- phaseGradient

// Phase gradients for a single antenna; all directions & frequencies
Matrix<DComplex> Phasing::phaseGradient (const Vector<Double>& frequencies,
					const Matrix<Double>& directions,
					const Vector<Double>& antpos) 
{
  IPosition shapeDirections = directions.shape();
  IPosition shapeFreq = frequencies.shape();
  Matrix<DComplex> gradients(shapeFreq(0),shapeDirections(0));
  Double delay,phi;

  for (int dir=0; dir<shapeDirections(0); dir++) {
    Vector<Double> direction = directions.row(dir);
    // -> Phasing::geometricalDelay(Vector,Vector)
    delay = 2*PI_p*Phasing::geometricalDelay(direction,antpos);
    for (int freq=0; freq<shapeFreq(0); freq++) {
      phi = frequencies(freq)*delay;
      gradients(freq,dir) = DComplex(cos(phi),sin(phi));
    }
  }

  return gradients;
}

// ---------------------------------------------------------------- phaseGradient

/* Phase gradients for a single direction; all frequencies & antennae. */
Matrix<DComplex> Phasing::phaseGradient (const Vector<Double>& frequencies,
					const Vector<Double>& direction,
					const Matrix<Double>& antennaPositions)
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
Vector<DComplex> Phasing::phaseGradient (const Double frequency,
					const Vector<Double>& direction,
					const Matrix<Double>& antennaPositions)
{
  IPosition shapeDirection = direction.shape();
  IPosition shapeAntpos = antennaPositions.shape();
  Vector<DComplex> gradients(shapeAntpos(0));

  for (int ant=0; ant<shapeAntpos(0); ant++) {
    Vector<Double> antpos = antennaPositions.row(ant);
    gradients(ant) = Phasing::phaseGradient (frequency,direction,antpos);
  }
  
  return gradients;
  
}

// ---------------------------------------------------------------- phaseGradient

/* Phase gradients for a single frequency, direction and antenna. */
DComplex Phasing::phaseGradient (const Double frequency,
				const Vector<Double>& azel,
				const Vector<Double>& antpos)
{
  Double phi;
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

Vector<Double> Phasing::geometricalDelay (const Vector<Double>& azel,
					  const Matrix<Double>& antennaPositions)
{
  Bool nearField = False;
  IPosition shape = antennaPositions.shape();
  Vector<Double> delays(shape(0));

  if (azel.nelements() == 3) nearField = True;

  for (int ant=0; ant<shape(0); ant++) {
    Vector<Double> antennaPosition = antennaPositions.row(ant);
    delays(ant) = Phasing::geometricalDelay (azel,antennaPosition,nearField);
  }

  return delays;
}

// ------------------------------------------------------------- geometricalDelay

Double Phasing::geometricalDelay (const Vector<Double>& azel,
				  const Vector<Double>& antennaPosition)
{
  Bool nearField (False);

  if (azel.nelements() == 3) {
    nearField = True;
  }

  return Phasing::geometricalDelay (azel,antennaPosition,nearField);
}

// ------------------------------------------------------------- geometricalDelay

Double Phasing::geometricalDelay (const Vector<Double>& azel,
				  const Vector<Double>& baseline,
				  Bool nearField=False)
{
  const Double c = QC::c.getValue();
  Vector<Double> rho = CR::azel2cartesian (azel);
  Double delay;
  
  if (nearField && azel.nelements() == 3) {
    Vector<Double> diff = rho-baseline;
    delay = (CR::L2Norm(diff)-CR::L2Norm(rho))/c;
  } else {
    Vector<Double> product = rho*baseline;
    delay = -sum(product)/c;
  }

  // Debugging output
//  cout << "[Phasing] : " << azel << "\t" << baseline << "\t" << delay << endl;

  /* Check if this sign inversion is indeed correct; it is present in 
     'angle2delay' (map.g), but not in 'calcdelay' (phasing.g). */

  return delay;
}
