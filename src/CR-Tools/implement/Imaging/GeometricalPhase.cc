/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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

#include <Imaging/GeometricalPhase.h>
#include <Math/Constants.h>
#include <casa/Arrays/Slicer.h>

namespace CR { // NAMESPACE CR -- BEGIN
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // ----------------------------------------------------------- GeometricalPhase

  GeometricalPhase::GeometricalPhase ()
    : GeometricalDelay (),
      bufferPhases_p(false)
  {
    casa::Vector<double> frequencies (1,0.0);
    setFrequencies (frequencies,
		    false);
  }
  
  // ----------------------------------------------------------- GeometricalPhase
  
  GeometricalPhase::GeometricalPhase (casa::Vector<double> const &frequencies,
				      bool const &bufferPhases)
    : GeometricalDelay (),
      bufferPhases_p(false)
  {
    setFrequencies (frequencies,
		    bufferPhases);
  }
  
  // ----------------------------------------------------------- GeometricalPhase

  GeometricalPhase::GeometricalPhase (casa::Matrix<double> const &antPositions,
				      casa::Matrix<double> const &skyPositions,
				      casa::Vector<double> const &frequencies,
				      bool const &bufferDelays,
				      bool const &bufferPhases)
    : GeometricalDelay (antPositions,
			CR::CoordinateType::Cartesian,
			skyPositions,
			CR::CoordinateType::Cartesian,
			bufferDelays,
			true)
  {
    setFrequencies (frequencies,
		    bufferPhases);
  }

  // ----------------------------------------------------------- GeometricalPhase

  GeometricalPhase::GeometricalPhase (casa::Matrix<double> const &antPositions,
				      CR::CoordinateType::Types const &antCoordType,
				      casa::Matrix<double> const &skyPositions,
				      CR::CoordinateType::Types const &skyCoordType,
				      casa::Vector<double> const &frequencies,
				      bool const &bufferDelays,
				      bool const &bufferPhases)
    : GeometricalDelay (antPositions,
			antCoordType,
			skyPositions,
			skyCoordType,
			bufferDelays,
			true)
  {
    setFrequencies (frequencies,
		    bufferPhases);
  }

  // ----------------------------------------------------------- GeometricalPhase
  
  GeometricalPhase::GeometricalPhase (GeometricalDelay const &delay,
				      casa::Vector<double> const &frequencies,
				      bool const &bufferPhases)
    : GeometricalDelay (delay)
  {
    setFrequencies (frequencies,
		    bufferPhases);
  }

  // ----------------------------------------------------------- GeometricalPhase

  GeometricalPhase::GeometricalPhase (GeometricalPhase const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  GeometricalPhase::~GeometricalPhase ()
  {
    destroy();
  }
  
  void GeometricalPhase::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  GeometricalPhase& GeometricalPhase::operator= (GeometricalPhase const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void GeometricalPhase::copy (GeometricalPhase const &other)
  {
    // copy the underlying base object
    GeometricalDelay::operator= (other);

    bufferPhases_p = other.bufferPhases_p;

    frequencies_p.resize (other.frequencies_p.shape());
    frequencies_p = other.frequencies_p;

    if (bufferPhases_p) {
      phases_p.resize(other.phases_p.shape());
      phases_p = other.phases_p;
    }
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  // ------------------------------------------------------------- setFrequencies

  bool GeometricalPhase::setFrequencies (const casa::Vector<double> &frequencies,
					 bool const &bufferPhases)
  {
    bool status (true);

    frequencies_p.resize (frequencies.shape());
    frequencies_p = frequencies;

    bufferPhases_p = bufferPhases;

    if (bufferPhases_p) {
      setPhases();
    }
    
    return status;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  // ---------------------------------------------------------------------- phase
  
  double GeometricalPhase::phase (int const &nFreq,
				  int const &nAntenna,
				  int const &nSky)
  {
    return CR::_2pi*frequencies_p(nFreq)*GeometricalDelay::delay(nAntenna,nSky);
  }
  
  // ---------------------------------------------------------------------- phase
  
  double GeometricalPhase::phase (double const &frequency,
				  Vector<double> const &antPosition,
				  Vector<double> const &skyPosition)
  {
    return CR::_2pi*frequency*GeometricalDelay::delay(antPosition,skyPosition);
  }
  
  // --------------------------------------------------------------------- phases

  casa::Cube<double> GeometricalPhase::phases ()
  {
    if (bufferPhases_p) {
      return phases_p;
    } else {
      return calcPhases();
    }
  }
  
  // ------------------------------------------------------------------ setPhases

  void GeometricalPhase::setPhases ()
  {
    if (bufferPhases_p) {
      delays_p.resize (nofFrequencies(),
		       GeometricalDelay::nofAntennaPositions(),
		       GeometricalDelay::nofSkyPositions());
      phases_p = calcPhases ();
    }
  }

  // ----------------------------------------------------------------- calcPhases

  casa::Cube<double> GeometricalPhase::calcPhases ()
  {
    int nofChannels (nofFrequencies());
    int nChannel (0);

    // get the geometrical delays, [nofAntennas,nofSkyPositions]
    casa::Matrix<double> delays = GeometricalDelay::delays();
    casa::IPosition shape       = delays.shape();
    
    // array with the computed phases, [freq,ant,sky]
    casa::Cube<double> phases (nofChannels,
			       shape(0),
			       shape(1));

    /* Use a Slicer object to directly insert the array with the delays into
       the array holding the phases. */

    casa::IPosition end (3,1,shape(0),shape(1));
    casa::IPosition incr (3,1,1,1);

#ifdef DEBUGGING_MESSAGES
    std::cout << "[GeometricalPhase::calcPhases]" << std::endl;
    std::cout << "-- frequency values       = " << frequencies_p  << std::endl;
    std::cout << "-- geometrical delays     = " << delays         << std::endl;
    std::cout << "-- nof frequency channels = " << nofChannels    << std::endl;
    std::cout << "-- shape of delays array  = " << shape          << std::endl;
    std::cout << "-- shape of phases array  = " << phases.shape() << std::endl;
    std::cout << "-- Slicer: end specifier  = " << end            << std::endl;
    std::cout << "-- Slicer: incr specifier = " << incr           << std::endl;
#endif
    
    /*!
      Computation by looping over the individual elements in both the array
      storing the geometrical delays and the resulting phases is the most 
      efficient method here; we rather would like to treat the geometrical
      delays as a whole per frequency channel, effectively inserting slices
      into the cube storing the values of the phases. While the cassacore 
      libraries provide methods to perform such operations, for all three
      possible plains parallel to the Cube axes, this is (current) not the
      case for the CASA libraries as required to provide the Glish support.
      To allow for the usage of the more efficient methods we here make a
      distinction between the two variants of the arrays classes and their
      methods.
    */
    
#ifdef HAVE_CASACORE
    for (nChannel=0; nChannel<nofChannels; nChannel++) {
      phases.yzPlane(nChannel) = CR::_2pi*frequencies_p(nChannel)*delays;
    }
#else 
    if (nofChannels==1 || shape(0)==1 || shape(1)==1) {
      uint freq (0);
      uint ant (0);
      uint pos (0);
      for (pos=0; pos<shape(1); pos++) {
	for (ant=0; ant<shape(0); ant++) {
	  for (freq=0; freq<nofChannels; freq++) {
	    phases(freq,ant,pos) =  CR::_2pi*frequencies_p(freq)*delays(ant,pos);
	  }
	}
      }
    } 
    else {
      // temporary array for referencing
      casa::Array<double> tmp;
      // go through the frequency channels
      for (nChannel=0; nChannel<nofChannels; nChannel++) {
	// create Slicer object to address the target array
	casa::Slicer slice (casa::IPosition(3,nChannel,0,0),
			    end,
			    incr,
			    casa::Slicer::endIsLength);
	// have the Slicer object reference the target region in the output array
	tmp.reference (phases(slice).nonDegenerate(casa::IPosition(2,1,2)));
	// calculate phases from the delays and store them
	tmp = CR::_2pi*frequencies_p(nChannel)*delays;
      }
    }
#endif

    return phases;
  }
  
  void GeometricalPhase::summary (std::ostream &os)
  {
    // first the summary of the base class
    GeometricalDelay::summary(os);
    
    // Summary of the parameters added with this object
    os << "[GeometricalPhase] Summary of object" << std::endl;
    os << "-- Frequency values  : " << frequencies_p.shape() << std::endl;
    os << "-- buffer phases     : " << bufferPhases_p        << std::endl;
  }

} // NAMESPACE CR -- END
