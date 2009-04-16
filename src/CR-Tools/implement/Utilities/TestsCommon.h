/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
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

#ifndef TESTSCOMMON_H
#define TESTSCOMMON_H

// Standard library header files
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

// casacore header files
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Quanta/Quantum.h>

// CR-Tools header files
#include <Coordinates/CoordinateType.h>
#include <Coordinates/TimeFreqCoordinate.h>
#include <Imaging/Beamformer.h>
#include <Imaging/GeomDelay.h>

using casa::Quantity;

namespace CR { // Namespace CR -- begin
  
  /*!
    \file TestsCommon.h
    
    \ingroup CR_Utilities
    
    \brief A collection of common routines for test programs
    
    \author Lars B&auml;hren

    \date 2009/04/15

    \test tTestsCommon.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  

  // ============================================================================
  //
  //  General routines
  //
  // ============================================================================

  /*!
    \brief Compute runtime of an operation based on start and end time
    
    \param start -- Start of the time interval
    \param end   -- End of the time interval
    
    \return runtime -- Time-interval between the moments the two values
            <tt>start</tt> and <tt>end</tt> were extracted.
  */
  double runtime (clock_t const &start,
		  clock_t const &end);
  
  /*!
    \brief Get a list of array sidelength values for setting up array shapes
    
    \return nelem -- Vector with a list of values used to set the size of an array,
            something like <tt>[1,2,5,10,20,50,100,200,500,1000,2000,5000,10000]</tt>
  */
  std::vector<int> number_sequence ();
  
  // ============================================================================
  //
  //  Module Coordinates
  //
  // ============================================================================
  
  /*!
    /brief Get TimeFreq coordinate
    
    \param blocksize      -- Number of samples per block of data.
    \param sampleFreq     -- 
    \param nyquistZone    -- 
    \param blocksPerFrame -- 
    \param nofFrames      -- 
  */
  CR::TimeFreqCoordinate getTimeFreq (uint const &blocksize=1024,
				      Quantity const &sampleFreq=Quantity(200,"MHz"),
				      uint const &nyquistZone=1,
				      uint const &blocksPerFrame=1,
				      uint const &nofFrames=2);
    
  // ============================================================================
  //
  //  Module Imaging
  //
  // ============================================================================
  
  /*!
    \brief Export the internal settings of the Beamformer
    
    The following data files will be created:
    - <prefix>-antPositions.dat
    - <prefix>-skyPositions.dat
    - <prefix>-delays.dat
  */
  void export_GeomDelay (CR::GeomDelay &delay,
			 std::string const &prefix="geomdelay");
  
  /*!
    \brief Export the internal settings of the Beamformer
    
    The following data files will be created:
    - <prefix>-antPositions.dat
    - <prefix>-skyPositions.dat
    - <prefix>-delays.dat
  */
  void export_Beamformer (CR::Beamformer &bf,
			  std::string const &prefix="beamformer");
  
} // Namespace CR -- end

#endif /* TESTSCOMMON_H */

