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
#include <images/Images/ImageFITSConverter.h>

// CR-Tools header files
#include <crtools.h>
#include <Coordinates/CoordinateType.h>
#include <Coordinates/TimeFreqCoordinate.h>
#include <Imaging/Beamformer.h>
#include <Imaging/GeomDelay.h>

// DAL header files
#include <dal/dalCommon.h>

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
    \brief Provide a summary of the properties of a casa::Coordinate object
    
    \param coord -- Coordinate object of which to show the properties.
  */
  void summary (casa::Coordinate &coord);
  
  /*!
    \brief Print a summary of the properties of a coordinate system object
  */
  void summary (casa::CoordinateSystem &csys);
  
  //_______________________________________________________________________________
  //                                                                     image2fits
  
  /*!
    \brief Export casacore image to FITS file

    \param image          -- Input casacore image to be exported to FITS.
    \param outfile        -- Name of the created output FITS file.
    \param degenerateLast -- If <tt>true</tt>, axes of length 1 will be written
           last to the header.
    \param allowOverwrite -- If <tt>true</tt>, allow <i>outfile</i> to be
           overwritten if it already exists.
    \param verbose        -- Be verbose during the operation.

    \return status -- <tt>true</tt> if the conversion succeeds, <tt>false</tt>
            otherwise.
  */
  template <class T>
    bool image2fits (casa::ImageInterface<T> &image,
		     std::string const &outfile="image.fits",
		     bool const &degenerateLast=true,
		     bool const &allowOverwrite=true,
		     bool const &verbose=false)
    {
      casa::String error;
      bool status          = true;
      uint memoryInMB      = 64;
      bool preferVelocity  = true;
      bool opticalVelocity = true;
      int BITPIX           = -32;
      float minPix         = 1.0;
      float maxPix         = -1.0;
      
      status = casa::ImageFITSConverter::ImageToFITS(error,
						     image,
						     outfile,
						     memoryInMB,
						     preferVelocity,
						     opticalVelocity,
						     BITPIX,
						     minPix,
						     maxPix,
						     allowOverwrite,
						     degenerateLast,
						     verbose);
      
      if (!status) {
	std::cerr << "[TestsCommon::image2fits] Error during export to FITS!"
		  << std::endl;
	std::cerr << error << std::endl;
      }
      
      return status;
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
    \brief Export coordinate values of a set of positions in space
    
    \param pos      -- [nofPositions,nofCoordinates] Array with the position values.
    \param filename -- Name of the output file to which the positions will be
           written.
    \param screenSummary -- Also provide a short summary to the screen? If \e true,
           then a small subset of the values will be written to standard output.
  */
  void test_exportPositions (Matrix<double> const &pos,
			     std::string const &filename="positions.dat",
			     bool const &screenSummary=false);
  
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

  /*!
    \brief Show internal parameters of a GeomDelay object

    \param delay -- GeomDelay object of which to display internal parameters
    \param nofSkyPositions -- nof. sky positions to be displayed
    \param nofDelays -- nof. geometrical delays for which to display the values
  */
  void test_showGeomDelay (CR::GeomDelay &delay,
			   int nofSkyPositions=4,
			   int nofDelays=4);
  
  /*!
    \brief Show internal parameters of a GeomPhase object

    \param phase -- GeomPhase object of which to display internal parameters
    \param nofSkyPositions -- nof. sky positions to be displayed
    \param nofDelays -- nof. geometrical delays for which to display the values
    \param nofPhases -- nof. geometrical phases for which to display the values
  */
  void test_showGeomPhase (CR::GeomPhase &phase,
			   int nofSkyPositions=4,
			   int nofDelays=4,
			   int nofPhases=4);
  
  /*!
    \brief Show internal parameters of a GeomWeight object

    \param weight -- GeomWeight object of which to display internal parameters
    \param nofSkyPositions -- nof. sky positions to be displayed
    \param nofDelays  -- nof. geometrical delays for which to display the values
    \param nofPhases  -- nof. geometrical phases for which to display the values
  */
  void test_showGeomWeight (CR::GeomWeight &weight,
			    int nofSkyPositions=4,
			    int nofDelays=4,
			    int nofPhases=4);
  
} // Namespace CR -- end

#endif /* TESTSCOMMON_H */

