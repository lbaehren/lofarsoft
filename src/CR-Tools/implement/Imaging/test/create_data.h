/*-------------------------------------------------------------------------*
 | $Id: tBeamformer.cc 431 2007-07-10 17:19:30Z baehren                  $ |
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

#include <Imaging/SkymapCoordinates.h>

using casa::DComplex;
using casa::Matrix;
using casa::Vector;

using CR::ObservationData;
using CR::SkymapCoordinates;
using CR::TimeFreq;

/*!
  \file create_data.h

  \ingroup Imaging

  \brief A collection of routines to generate data for the various test programs
 
  \author Lars B&auml;hren
 
  \date 2007/07/12
*/

// -----------------------------------------------------------------------------

/*!
  \brief Create some antenna positions for the Beamformer

  \param nofAntennas -- The number of antennas

  \return positions -- [antenna,coordinate] Antenna positions, \f$ \vec x \f$
*/
Matrix<double> get_antennaPositions (uint const &nofAntennas=3)
{
  uint nofAxes (3);
  Matrix<double> positions (nofAntennas,nofAxes,0.0);

  for (uint n(0); n<nofAntennas; n++) {
    positions(n,0) = positions(n,1) = n*100;
  }

  return positions;
}

// -----------------------------------------------------------------------------

/*!
  \brief Create some sky/pointing positions for the Beamformer

  \param nofDirections -- Number of \f$ (\theta,\phi) \f$ directions
  \param nofDistances  -- Number of distance steps.

  \return positions -- [position,coordinate] Sky positions, \f$ \vec\rho \f$
*/
Matrix<double> get_skyPositions (uint const &nofDirections=3,
				 uint const &nofDistances=1)
{
  uint nofAxes (3);
  uint nofPositions (nofDirections*nofDistances);
  uint direction(0);
  uint distance(0);
  Matrix<double> positions (nofPositions,nofAxes,0.0);

  for (direction=0; direction<nofDirections; direction++) {
    for (distance=0; distance<nofDistances; distance++) {
    }
  }

  return positions;
}

// -----------------------------------------------------------------------------

/*!
  \brief Create frequency band values for the Beamformer

  \param freqMin     -- Lower limit of the frequency band, [Hz]
  \param freqMax     -- Upper limit of the frequency band, [Hz]
  \param nofChannels -- Number of frequency channels, into which the band is
                        split.

  \return frequencies -- 
*/
Vector<double> get_frequencies (double const &freqMin=40e06,
				double const &freqMax=80e06,
				uint const &nofChannels=4096)
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
  \brief Create some data for the Beamformer to process

  \param nofAntennas -- The number of antennas
  \param nofChannels -- Number of frequency channels, into which the band is
                        split.

  \return data -- [antenna,channel]
*/
Matrix<DComplex> get_data (uint const &nofAntennas=3,
			   uint const &nofChannels=4096)
{
  // Indices
  uint antenna (0);
  uint channel (0);
  // Array of generated data
  Matrix<DComplex> data (nofAntennas,nofChannels,0.0);
  // Random number generator from CASA scimath/Mathematics module
  casa::ACG gen(10, 20);
  casa::Uniform random (&gen);

  for (antenna=0; antenna<nofAntennas; antenna++) {
    for (channel=0; channel<nofChannels; channel++) {
      data(antenna,channel) = DComplex(random(),random());
    }
  }

/*   std::cout << "[get_data]" <<  std::endl; */
/*   std::cout << "-- nof. antennas = " << nofAntennas  << std::endl; */
/*   std::cout << "-- nof. channels = " << nofChannels  << std::endl; */
/*   std::cout << "-- shape(data)   = " << data.shape() << std::endl; */
  
  return data;
}

// -----------------------------------------------------------------------------

/*!
  \brief Wrapper for creation of a SkymapCoordinates object 
  
  \verbatim
  nof. processed blocks    = 10
  Projection               = STG
  Skymap orientation       = 1 [North,West]
  Skymap quantity          = 1 [FREQ,POWER]
  Number of coordinates    = 4
  Shape of the pixel array = [120, 120, 1, 10, 513]
  World axis names         = [Longitude, Latitude, Distance, Time, Frequency]
  World axis units         = [rad, rad, m, s, Hz]
  Reference pixel  (CRPIX) = [60, 60, 0, 0, 0]
  Reference value  (CRVAL) = [0, 1.5708, -1, 0, 0]
  Increment        (CDELT) = [-0.0349066, 0.0349066, 0, 2.5e-08, 39062.5]
  \endverbatim

  \param blocksize        -- 
  \param sampleFrequency  -- 
  \param nyquistZone      -- 
  \param nofDistanceSteps -- 
  
  \return coord -- A new SkymapCoordinates object
*/
SkymapCoordinates get_SkymapCoordinates (uint const blocksize=1024,
					 double const sampleFrequency=40e6,
					 uint const nyquistZone=1,
					 uint const nofDistanceSteps=2)
{
  TimeFreq timeFreq (blocksize,
		     sampleFrequency,
		     nyquistZone);
  ObservationData obsData ("WSRT");
  uint nofBlocks (10);
  
  // create SkymapCoordinates object
  SkymapCoordinates coord (timeFreq,
			   obsData,
			   nofBlocks,
			   SkymapCoordinates::NORTH_WEST,
			   CR::FREQ_POWER);

  // update parameters
  coord.setDistanceShape (nofDistanceSteps);
  
  return coord;
}
