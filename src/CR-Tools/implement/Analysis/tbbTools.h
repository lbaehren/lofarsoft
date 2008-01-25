/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
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

#ifndef TBBTOOLS_H
#define TBBTOOLS_H

// Standard library header files
#include <iostream>
#include <string>

#include <casa/Arrays/Vector.h>

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class tbbTools
    
    \ingroup Analysis
    
    \brief Collection of routines to test the TBBs
    
    \author Andreas Horneffer

    \date 2008/01/25

    \test ttbbTools.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class tbbTools {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    tbbTools ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~tbbTools ();
    
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, tbbTools.
    */
    std::string className () const {
      return "tbbTools";
    }


    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Perform the simplified (mean-based) version of the FPGA trigger
      
      \param inary[in]   - input data that is to be searched for pulses
      \param level[in]   - trigger parameter, threshold level
      \param start[in]   - trigger parameter, start level
      \param stop[in]    - trigger parameter, stop level
      \param window[in]  - trigger parameter, window size index
      \param index[out]  - indices of the first sample of the found pulses
      \param sum[out]    - sum of the samples in the pulse
      \param width[out]  - width of the pulse
      \param peak[out]   - peak value inside the pulse
      
      \return ok -- True if successfull
    */
    Bool meanFPGAtrigger(Vector<Double>, int level, int start, int stop, int window,
			 Vector<Int> &index, Vector<Int> &sum, Vector<Int> &width, 
			 Vector<Int> &peak);

    
  private:
        
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* TBBTOOLS_H */
  
