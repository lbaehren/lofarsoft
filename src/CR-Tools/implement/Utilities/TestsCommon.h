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
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Quanta/Quantum.h>
#include <coordinates/Coordinates/ObsInfo.h>

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
    
    <h3>Synopsis</h3>

    Given the fact that a number of instructions/operations are used across the 
    various test programs, it makes sense to collect a common set of functions.

    <ul>
      <li>A number of general routines.
    </ul>

    \e History: This file replaces and extends upon the earlier tests_common.h and
    create_data.h
    
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
  
  /*!
    \brief Get a ObsInfo object to attach to coordinates
    
    \param telescope -- Name of the telescope
    \param observer  -- Name of the observer
    
    \return info -- A casa::ObsInfo object, containing additional information on an
            observation.
  */
  casa::ObsInfo test_ObsInfo (std::string const &telescope="LOFAR",
			      std::string const &observer="Lars Baehren");
  
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
    \brief Create frequency band values for the Beamformer

    \param blocksize -- Number of samples per block of data; this also is the input
           size to the FFT.
    \param sampleFrequency -- Sample frequency in the A/D conversion.
    \param nyquistZone     -- Nyquist zone in which the A/D conversion is performed.
    
    \return frequencies -- Vector with the frequency values, [Hz]
  */
  casa::Vector<double> test_getFrequencies (uint const &blocksize=1024,
					    double const &sampleFrequency=80e06,
					    uint const &nyquistZone=1);
  
  
  /*!
    \brief Create some data processing with the Beamformer or Skymapper
    
    \retval data -- Array with the created data; the organization of the array
                    depends on the value of the <tt>antennaIndexFirst</tt> parameter.
    \param nofAntennas -- The number of antennas
    \param nofChannels -- Number of frequency channels, into which the band is
           split.
    \param antennaIndexFirst -- How to organize the axes in the returned data 
           array; if <tt>antennaIndexFirst=false</tt> then the data are organized
	   as [freq,antenna], if <tt>antennaIndexFirst=true</tt> then we have
	   [antenna,freq].
    
  */
  void test_getData (Matrix<DComplex> &data,
		     uint const &nofAntennas=4,
		     uint const &nofChannels=4096,
		     bool const &antennaIndexFirst=false);
  
  /*!
    \brief Create some data processing with the Beamformer or Skymapper
    
    \retval data -- Array with the created data; the organization of the array
                    depends on the value of the <tt>antennaIndexFirst</tt> parameter.
  */
  void test_getData (Matrix<DComplex> &data,
		     casa::IPosition const &shape,
		     bool const &antennaIndexFirst=false);
  
  /*!
    \brief Export the internal settings of the Beamformer
    
    The following data files will be created:
    - <prefix>-antPositions.dat
    - <prefix>-skyPositions.dat
    - <prefix>-delays.dat
  */
  void test_exportGeomDelay (CR::GeomDelay &delay,
			     std::string const &prefix="geomdelay");
  
  /*!
    \brief Export the internal settings of the Beamformer
    
    The following data files will be created:
    - <prefix>-antPositions.dat
    - <prefix>-skyPositions.dat
    - <prefix>-delays.dat
  */
  void test_exportBeamformer (CR::Beamformer &bf,
			      std::string const &prefix="beamformer");
  
} // Namespace CR -- end

#endif /* TESTSCOMMON_H */

