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


#ifndef FREQUENCYNOISE_H
#define FREQUENCYNOISE_H

// AIPS++/CASA header files
#include <casa/Exceptions/Error.h>
#include <casa/namespace.h>
#include <casa/Arrays.h>
#include <casa/aips.h>

using std::cout;

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class FrequencyNoise
    
    \ingroup IO
    
    \brief Brief description for class FrequencyNoise
    
    \author Tim Huege
    
    \date 2007/09/20
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
    <li>none
    </ul>
    
    <h3>Synopsis</h3>
    
    Abstract base class for noise generation in the frequency domain.
    
  */
  
  class FrequencyNoise {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    FrequencyNoise ();
    
    // -------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    virtual ~FrequencyNoise ();
    
    // ---------------------------------------------------------------- Operators
    
    
    // --------------------------------------------------------------- Parameters
    
    
    
    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Get a vector of complex numbers specifying the noise in the frequency domain.

    
      \param frequencies -- pass a vector with the frequencies (in Hz) for which the noise is needed
    
      \param noise -- returns a vector with the noise in the frequency domain (in cgs units)
    */
    virtual void getNoise(Vector<Double> &frequencies,
		          Vector<DComplex> &noise) = 0;
        
  protected:
    static const double kB;	// Boltzmann constant in cgs units
    
  };
  
}  // Namespace CR -- end

#endif /* FREQUENCYNOISE_H */

