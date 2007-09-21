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


#ifndef FALCKEGORHAMGALACTICNOISE_H
#define FALCKEGORHAMGALACTICNOISE_H

// AIPS++/CASA header files
#include <casa/Exceptions/Error.h>
#include <casa/namespace.h>
#include <casa/Arrays.h>
#include <casa/aips.h>
#include <casa/BasicMath/Random.h>
#include <cmath>
#include <ctime>
#include <IO/FrequencyNoise.h>

using std::cout;

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class FalckeGorhamGalacticNoise
    
    \ingroup IO
    
    \brief Brief description for class FalckeGorhamGalacticNoise
    
    \author Tim Huege
    
    \date 2007/09/20
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
    <li>none
    </ul>
    
    <h3>Synopsis</h3>
    
    Class for noise generation in the frequency domain according to FalckeGorham 2003 Galactic noise formula.
    
  */
  
  class FalckeGorhamGalacticNoise : public FrequencyNoise {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    FalckeGorhamGalacticNoise ();
    
    // -------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    virtual ~FalckeGorhamGalacticNoise ();
    
    // ---------------------------------------------------------------- Operators
    
    
    // --------------------------------------------------------------- Parameters
    
    
    
    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Get a vector of complex numbers specifying the FalckeGorham Galactic noise in the frequency domain.

    
      \param frequencies -- pass a vector with the equidistantly spaced (!) frequencies (in Hz) for which the noise is needed
    
      \param noise -- returns a vector with the noise in the frequency domain (in SI units, correctly normalized for FFTServer backwards Fourier transform)
    */
    virtual void getNoise(Vector<Double> &frequencies,
		          Vector<DComplex> &noise);
        
  private:

    MLCG OurRNG;
    Uniform UniformRNG;
    
  };
  
}  // Namespace CR -- end

#endif /* FALCKEGORHAMGALACTICNOISE_H */

