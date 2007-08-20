/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars Bahren (<mail>)                                                  *
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

#include <ApplicationSupport/Phasing.h>

// =============================================================================

Bool test_geometricalDelay () {

  cout << "\n[tPhasing::test_geometricalDelay]\n" << endl;

  {
    Vector<Double> posSource(2);   // source position [deg]
    Vector<Double> posAntenna(3);  // antenna position [m]
    Double delay;
    //
    posAntenna(0) = 100;
    posAntenna(1) = 100;
    posAntenna(2) = 0;
    //
    posSource = 45;

    delay = Phasing::geometricalDelay (posSource, posAntenna);
    //
    cout << " Source position [deg] : " << posSource << endl;
    cout << " Antenna position  [m] : " << posAntenna << endl;
    cout << " Geometrical delay [s] : " << delay << endl;

    posSource = 10;
    //
    delay = Phasing::geometricalDelay (posSource, posAntenna);
    //
    cout << endl;
    cout << " Source position [deg] : " << posSource << endl;
    cout << " Antenna position  [m] : " << posAntenna << endl;
    cout << " Geometrical delay [s] : " << delay << endl;
    
  }

  return True;
  
}

// =============================================================================

Bool test_phaseGradient () {

  cout << "\n[tPhasing::test_phaseGradient]" << endl;

  Double freq;
  Vector<Double> direction (2);

  // Test 1
  {
    Vector<Double> antpos(3);
    Complex w;
    //
    freq = 40e6;
    //
    direction(0) = 0;
    direction(1) = 90;
    //
    antpos(0) = 100.0;
    antpos(1) = 100.0;
    antpos(2) = 5.0;
    // 
    w = Phasing::phaseGradient (freq, direction, antpos);
    //
    cout << endl;
    cout << " - Frequency        [Hz ] : " << freq << endl;
    cout << " - Direction        [deg] : " << direction << endl;
    cout << " - Antenna position [m  ] : " << antpos << endl;
    cout << " - Weighting factor       : " << w << endl;
    //
    direction = 40;
    // 
    w = Phasing::phaseGradient (freq, direction, antpos);
    //
    cout << endl;
    cout << " - Frequency        [Hz ] : " << freq << endl;
    cout << " - Direction        [deg] : " << direction << endl;
    cout << " - Antenna position [m  ] : " << antpos << endl;
    cout << " - Weighting factor       : " << w << endl;
  }

  // Test 2
  {
    Int nofAntennas (3);
    Matrix<Double> antpos(nofAntennas,3);
    Vector<DComplex> w(nofAntennas);
    //
    antpos(0,0) = 0.0;
    antpos(0,1) = 0.0;
    antpos(0,2) = 5.0;
    antpos(1,0) = 100.0;
    antpos(1,1) = 100.0;
    antpos(1,2) = 5.0;
    antpos(2,0) = -100.0;
    antpos(2,1) = 100.0;
    antpos(2,2) = 5.0;
    //
    w = Phasing::phaseGradient (freq, direction, antpos);
    //
    cout << endl;
    cout << " - Frequency        [Hz ] : " << freq << endl;
    cout << " - Direction        [deg] : " << direction << endl;
    cout << " - Antenna positions      : " << antpos.shape() << endl;
    cout << " - Weighting factor       : " << w << endl;
  }

  return True;
}

// =============================================================================
//
//  Main routine
//
// =============================================================================

int main () {

  Bool ok (False);

  {
    ok = test_geometricalDelay ();
  }

  {
    ok = test_phaseGradient ();
  }

  return 0;

}
