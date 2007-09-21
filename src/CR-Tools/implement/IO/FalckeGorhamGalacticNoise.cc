/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Tim Huege (<tim.huege@ik.fzk.de>)                                     *
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


#include <IO/FalckeGorhamGalacticNoise.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  FalckeGorhamGalacticNoise::FalckeGorhamGalacticNoise ()
  : OurRNG(time(0),time(0)+1),	// seeds
    UniformRNG(&OurRNG,0.0, 2*M_PI)
  {
  }
  
  // ==============================================================================
  //
  //  Destruction
  //
  // ==============================================================================
  
  FalckeGorhamGalacticNoise::~FalckeGorhamGalacticNoise ()
  {
  }
  
  // ==============================================================================
  //
  //  Operators
  //
  // ==============================================================================
  
  
  // ==============================================================================
  //
  //  Parameters
  //
  // ==============================================================================
  
  
  
  // ==============================================================================
  //
  //  Methods
  //
  // ==============================================================================
  

  void FalckeGorhamGalacticNoise::getNoise(Vector<Double> &frequencies, Vector<DComplex> &noise)
  {
    noise.resize(frequencies.nelements()); // delete potential entries in result vector
    Double bandwidth = frequencies(frequencies.nelements()-1)-frequencies(0);
    for (long i=0; i<frequencies.nelements(); ++i)
    {
      Double T = 0.0;
      if (frequencies(i) > 0.0)					// set DC term or negative frequencies to zero
      { T = 32.*pow(frequencies(i)/408.e6,-2.5); } 		// Falcke Gorham 2003

//      T = 2500;						// 2500 Kelvin, for testing purposes      

      Double abs = bandwidth*sqrt(0.5*kB*T*impedance);
      Double arg = UniformRNG();				// random phase between 0.0 and 2*Pi
      noise(i)=abs*DComplex(cos(arg),sin(arg));
    }
  }



}  // Namespace CR -- end
