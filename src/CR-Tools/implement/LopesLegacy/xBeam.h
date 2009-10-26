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

/* $Id$*/

#ifndef LOPES_XBEAM_H
#define LOPES_XBEAM_H

// Standard library header files
#include <string>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <scimath/Mathematics.h>

#include <casa/namespace.h>

namespace LOPES { // Namespace LOPES -- begin
  
  /*!
    \class xBeam
    
    \ingroup LopesLegacy
    
    \brief Brief description for class xBeam
    
    \author Andreas Horneffer

    \date 2007/06/16

    \test txBeam.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  template <class T, class S>  class xBeam {
    
    double start;
    double stop;
    double nsigma;
    Int navg;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    xBeam ();
    
    // -------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~xBeam ();
    
    // ---------------------------------------------------------------- Operators
    
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, xBeam.
    */
    std::string className () const {
      return "xBeam";
    }
    
    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Compute the so-called 'x-beam'
      
      \param data      -- Input data (in the time domain); [sample,antenna]
      
      \return cc  -- The 'x-beam' data.
    */
    Vector<T> xbeam (const Matrix<T>& data);
    
    /*!
      \brief Computation of the 'x-beam' from a set of "time-shifted" spectra
      
      \param data -- A set of beamformed spectra, [channel,antenna].
      \param blocksize -- Blocksize in the time domain (required for the inverse
      Fourier transform).
      
      \return cc  -- The 'x-beam' data.
    */
    Vector<T> xbeam (const Matrix<S>& data,
		      Int const &blocksize);
    
    
  private:
        
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace LOPES -- end

#endif /* LOPES_XBEAM_H */
  
