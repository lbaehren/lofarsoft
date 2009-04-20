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
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/ObsInfo.h>
#include <images/Images/ImageInterface.h>

// CR-Tools header files
#include <Coordinates/CoordinateType.h>
#include <Coordinates/TimeFreqCoordinate.h>
#include <Imaging/Beamformer.h>
#include <Imaging/GeomDelay.h>

using std::cout;
using std::endl;
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
      <li>Routines in support of operations common to test programs of a certain
      module (e.g. Imaging).
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
  
  /*!
    \brief Provide a summary of the image's properties
    
    Example output:
    \verbatim
    -- Image type ............. : HDF5Image
    -- Table name ............. : /Users/lars/Code/lofar/usg/build/cr/test/lofar_cr_skymap.h5
    -- Image shape ............ : [256, 256, 100, 10, 513]
    -- World axis names ....... : [Longitude, Latitude, Distance, Time, Frequency]
    -- World axis units ....... : [rad, rad, m, s, Hz]
    -- Referemce pixel ........ : [0, 0, 0, 0, 0]
    -- Reference value ........ : [0, 1.5708, 0, 0, 0]
    -- Maximum cache size ..... : 0
    -- Is the image paged? .... : 1
    -- Is the image persistent? : 1
    -- Is the image writable?   : 1
    -- Has the image a mask?    : 0
    \endverbatim
    
    \param image -- Image object derived from the ImageInterface class.
  */
  template <class T>
    void image_summary (casa::ImageInterface<T> &image)
    {
      casa::CoordinateSystem cs = image.coordinates();

      cout << "-- Image type ............. : " << image.imageType()        << endl;
      cout << "-- Table name ............. : " << image.name()             << endl;
      cout << "-- Image shape ............ : " << image.shape()            << endl;
      cout << "-- World axis names ....... : " << cs.worldAxisNames()      << endl;
      cout << "-- World axis units ....... : " << cs.worldAxisUnits()      << endl;
      cout << "-- Referemce pixel ........ : " << cs.referencePixel()      << endl;
      cout << "-- Reference value ........ : " << cs.referenceValue()      << endl;
      cout << "-- Increment .............. : " << cs.increment()           << endl;
      cout << "-- Maximum cache size ..... : " << image.maximumCacheSize() << endl;
      cout << "-- Is the image paged? .... : " << image.isPaged()      << endl;
      cout << "-- Is the image persistent? : " << image.isPersistent() << endl;
      cout << "-- Is the image writable?   : " << image.isWritable()   << endl;
      cout << "-- Has the image a mask?    : " << image.hasPixelMask() << endl;
    }
  
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
  CR::TimeFreqCoordinate test_TimeFreq (uint const &blocksize=1024,
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
    \brief Create a set of antenna positions

    \retval antPositions -- [nofAntennas,3] The 3-dimensional antenna positions.
    \param  type         -- Type of coordinates, within which the positions are
            given.
    \retval scale        -- Scaling factor for the separation of the individual
            antennas. In the case of Cartesian coordinate this is translated into
	    a regular grid with \e scale representing the separation of the grid
	    points parallel to the coordinate axes.
  */
  void test_antPositions (casa::Matrix<double> &antPositions,
			  CR::CoordinateType::Types const &type=CR::CoordinateType::Cartesian,
			  double const &scale=100);
  
  /*!
    \brief Create a set of frequency values

    \param blocksize -- Number of samples per block of data; this also is the input
           size to the FFT.
    \param sampleFrequency -- Sample frequency in the A/D conversion.
    \param nyquistZone     -- Nyquist zone in which the A/D conversion is performed.
    
    \return frequencies -- Vector with the frequency values, [Hz]
  */
  casa::Vector<double> test_frequencyValues (uint const &blocksize=1024,
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

