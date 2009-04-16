/*-------------------------------------------------------------------------*
 | $Id$ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#include <casa/BasicMath/Random.h>
#include <coordinates/Coordinates/ObsInfo.h>

#include <Coordinates/CoordinateType.h>
#include <Coordinates/SpatialCoordinate.h>
#include <Coordinates/TimeFreqCoordinate.h>
#include <Coordinates/SkymapCoordinate.h>
#include <Data/ObservationData.h>
#include <Imaging/Beamformer.h>
#include <Math/VectorConversion.h>

using casa::DComplex;
using casa::Matrix;
using casa::Vector;

using CR::ObservationData;

/*!
  \file create_data.h

  \ingroup CR_Imaging

  \brief A collection of routines to support the various test programs
 
  \author Lars B&auml;hren
 
  \date 2007/07/12

  <h3>Synopsis</h3>

  <ul>
    <li>Input parameters to classes/objects
    <li>Test/Simulated data for processing
    <li>Diagnostic output of parameter and (processed) data
  </ul>
*/

// ==============================================================================
//
//  Create parameters
//
// ==============================================================================

// -----------------------------------------------------------------------------

/*!
  \brief Create some antenna positions for the Beamformer

  \param nofAntennas -- The number of antennas
  \param antennaIndexFirst -- Is the antenna index the first index of the array?
         If <tt>true</tt>, then the returned array is of shape [nofAntennas,3],
	 otherwise -- if <tt>antennaIndexFirst=false</tt> -- the returned array
	 if of shape [3,nofAntennas].
  \param coordType -- CR::CoordinateType::Types as which the antenna positions are
         returned. If a coordinate type other but Cartesian is chosen, the angular
	 components will be given in radian (and not in degrees).

  \return positions -- [antenna,coordinate] Antenna positions, \f$ \vec x \f$
*/
Matrix<double>
get_antennaPositions (uint const &nofAntennas=4,
		      bool const &antennaIndexFirst=true,
		      CR::CoordinateType::Types const &coordType=CR::CoordinateType::Cartesian)
{
  uint nofAxes (3);
  double incr (100);
  Matrix<double> antPositions (nofAntennas,nofAxes,0.0);

  switch (coordType) {
  case CR::CoordinateType::Cartesian:
    if (nofAntennas == 4) {
      // [100,0,0]
      antPositions(0,0) = incr;
      // [0,100,0]
      antPositions(1,1) = incr;
      // [-100,0,0]
      antPositions(2,0) = -incr;
      // [0,-100,0]
      antPositions(3,1) = -incr;
    } else {
      for (uint n(0); n<nofAntennas; n++) {
	antPositions(n,0) = antPositions(n,1) = n*incr;
      }
    }
    break;
  case CR::CoordinateType::Spherical:
    // Set radial and zenith angle component
    antPositions.column(0) = incr;
    antPositions.column(2) = 0.0;
    // Set the stepping width along the circle
    incr = 360/nofAntennas;
    for (uint n(0); n<nofAntennas; n++) {
      antPositions(n,1) = n*incr;
    }
    break; 
  case CR::CoordinateType::Time:
  case CR::CoordinateType::Frequency:
  case CR::CoordinateType::Direction:
  case CR::CoordinateType::DirectionRadius:
  case CR::CoordinateType::AzElHeight:
  case CR::CoordinateType::AzElRadius:
  case CR::CoordinateType::LongLatRadius:
  case CR::CoordinateType::Cylindrical:
  case CR::CoordinateType::NorthEastHeight:
    std::cout << "-- No special settings for this coordinates." << std::endl;
    break;
  }
  
  if (antennaIndexFirst) {
    return antPositions;
  } else {
    return transpose(antPositions);
  }
  
}

// -----------------------------------------------------------------------------

/*!
  \brief Create some sky/pointing positions for the Beamformer

  \param nofPositions -- Number of sky positions for which to generate coordinates
  \param coordType    -- CR::CoordinateType::Types as which the antenna positions are
         returned.

  \return positions -- [position,coordinate] Sky positions, \f$ \vec\rho \f$
*/
Matrix<double> get_skyPositions (uint const &nofPositions=2,
				 CR::CoordinateType::Types const &coordType=CR::CoordinateType::Cartesian)
{
  uint n (0);
  uint nofAxes (3);
  double incr (100);
  Matrix<double> positions (nofPositions,nofAxes,0.0);

  switch (coordType) {
  case CR::CoordinateType::Cartesian:
    for (n=0; n<nofPositions; n++) {
      positions(n,0) = positions(n,1) = positions(n,2) = n*incr;
    }
    break;
  case CR::CoordinateType::Spherical:
    positions.column(1) = positions.column(2) = CR::deg2rad(45);
    for (n=0; n<nofPositions; n++) {
      positions(n,0) = n*incr;
    }
    break;
  case CR::CoordinateType::Time:
  case CR::CoordinateType::Frequency:
  case CR::CoordinateType::Direction:
  case CR::CoordinateType::DirectionRadius:
  case CR::CoordinateType::AzElHeight:
  case CR::CoordinateType::AzElRadius:
  case CR::CoordinateType::LongLatRadius:
  case CR::CoordinateType::Cylindrical:
  case CR::CoordinateType::NorthEastHeight:
    std::cout << "-- No special settings for this coordinates." << std::endl;
    break;
  }

  return positions;
}

// -----------------------------------------------------------------------------

/*!
  \brief Create some sky/pointing positions

  \param start     -- 
  \param increment -- 
  \param nofSteps  -- 

  \return positions -- [position,coordinate] Sky positions, \f$ \vec\rho \f$
*/
Matrix<double> get_skyPositions (Vector<double> const &start,
				 Vector<double> const &increment,
				 Vector<int> const &nofSteps)
{
  if (start.shape() == increment.shape() &&
      start.shape() == nofSteps.shape()) {
    uint nofCoordinates (start.nelements());
    uint nofPositions (1);
    uint coord(0);
    int step (0);
    // how many positions will there be?
    for (coord=0; coord<nofCoordinates; coord++) {
      nofPositions *= nofSteps(coord);
    }
    // set up the matrix holding the generated position values
    Matrix<double> positions (nofCoordinates,nofPositions);
    // generate the position values based on the input parameters
    for (coord=0; coord<nofCoordinates; coord++) {
      for (step=0; step<nofSteps(coord); step++) {
      }
    }
    return positions;
  } else {
    // throw error message
    std::cerr << "Error; inconsistent shape of parameter vectors!" << std::endl;
    std::cerr << " -- shape(start)     = " << start.shape() << std::endl;
    std::cerr << " -- shape(increment) = " << increment.shape() << std::endl;
    // return some output
    Matrix<double> positions (1,3);
    positions = 0.0;
    return positions;
  }
}

// -----------------------------------------------------------------------------

/*!
  \brief Create frequency band values for the Beamformer

  \param nofChannels -- Number of frequency channels
  \param freqMin     -- Lower limit of the frequency band, [Hz]
  \param freqMax     -- Upper limit of the frequency band, [Hz]

  \return frequencies -- Vector with the frequency values, [Hz]
*/
Vector<double> get_frequencies (uint const &nofChannels=4096,
				double const &freqMin=40e06,
				double const &freqMax=80e06)
{
  Vector<double> channels (nofChannels);
  double incr ((freqMax-freqMin)/(nofChannels+1));

  for (uint k(0); k<nofChannels; k++) {
    channels(k) = freqMin+k*incr;
  }

  return channels;
}

// -----------------------------------------------------------------------------

/*!
  \brief Get array with the complex antenna gains

  \param nofFrequencies -- Number of frequency channels
  \param nofAntennas    -- The number of antennas
  \param nofPositions   -- Number of sky positions towards which to point

  \return gains -- [freq,antenna,position] Array with the complex antenna gains
          \f$ w_{j,\rm gain} (\vec\rho,\nu) \f$ as function of sky position
	  \f$ \vec\rho \f$ and frequency \f$ \nu \f$
*/
casa::Cube<DComplex> get_antennaGains (uint const &nofFrequencies=4096,
				       uint const &nofAntennas=4,
				       uint const &nofPositions=2)
{
  casa::IPosition shape (3,nofFrequencies,nofAntennas,nofPositions);
  casa::Cube<DComplex> gains (shape);

  gains = DComplex (1,0);

  return gains;
}

// -----------------------------------------------------------------------------

/*!
  \brief Create some data for the Beamformer to process

  \param nofAntennas -- The number of antennas
  \param nofChannels -- Number of frequency channels, into which the band is
                        split.
  \param antennaIndexFirst -- How to organize the axes in the returned data 
         array; if <tt>antennaIndexFirst=false</tt> then the data are organized
	 as [freq,antenna], if <tt>antennaIndexFirst=true</tt> then we have
	 [antenna,freq].

  \return data -- Array with the created data; the organization of the array
          depends on the value of the <tt>antennaIndexFirst</tt> parameter.
*/
Matrix<DComplex> get_data (uint const &nofAntennas=4,
			   uint const &nofChannels=4096,
			   bool const &antennaIndexFirst=false)
{
  // Indices
  uint antenna (0);
  uint channel (0);
  // Array of generated data
  Matrix<DComplex> data;
  // Random number generator from CASA scimath/Mathematics module
  casa::ACG gen(10, 20);
  casa::Uniform random (&gen);

  if (antennaIndexFirst) {
    // set the shape of the array returning the data
    data.resize(nofAntennas,nofChannels,0.0);
    for (channel=0; channel<nofChannels; channel++) {
      for (antenna=0; antenna<nofAntennas; antenna++) {
	data(antenna,channel) = DComplex(random(),random());
      }
    }
  } else {
    // set the shape of the array returning the data
    data.resize(nofChannels,nofAntennas,0.0);
    for (antenna=0; antenna<nofAntennas; antenna++) {
      for (channel=0; channel<nofChannels; channel++) {
	data(channel,antenna) = DComplex(random(),random());
      }
    }
  }
  
  return data;
}

// -----------------------------------------------------------------------------

/*!
  \brief Wrapper for creation of a SkymapCoordinate object 

  \param blocksize        -- nof. samples per block of data
  \param sampleFrequency  -- Sample frequency in the A/D conversion step
  \param nyquistZone      -- Nyquist zone
  \param nofDistanceSteps -- nof. steps along the distance axis
  \param blocksPerFrame   -- nof. subsequent blocks combined into a single
         time-frame
  
  \return coord -- A new SkymapCoordinates object
*/
CR::SkymapCoordinate get_SkymapCoordinate (uint const blocksize=1024,
					   double const sampleFrequency=40e6,
					   uint const nyquistZone=1,
					   uint const nofDistanceSteps=2,
					   uint blocksPerFrame=1,
					   uint nofFrames=2,
					   std::string refcode="AZEL",
					   std::string projection="SIN")
{
  // Parameters used for object creation
  std::string telescope  = "WSRT";
  std::string observer   = "Somebody";
  casa::Quantity sampleFreq (sampleFrequency,"Hz");

  // Observation information
  casa::ObsInfo info;
  info.setTelescope(telescope);
  info.setObserver(observer);
  // Coordinate objects
  CR::TimeFreqCoordinate timeFreq (blocksize,
				   sampleFreq,
				   nyquistZone,
				   blocksPerFrame,
				   nofFrames);
  CR::SpatialCoordinate spatial (CR::CoordinateType::DirectionRadius,
				 refcode,
				 projection);
  spatial.setShape(casa::IPosition(3,20,20,nofDistanceSteps));
  CR::SkymapCoordinate coord (info,
			      spatial,
			      timeFreq);
  
  return coord;
}
