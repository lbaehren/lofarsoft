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
			CR::Cartesian,
			skyPositions,
			CR::Cartesian,
			bufferDelays,
			true)
  {
    setFrequencies (frequencies,
		    bufferPhases);
  }

  // ----------------------------------------------------------- GeometricalPhase

  GeometricalPhase::GeometricalPhase (casa::Matrix<double> const &antPositions,
				      CR::CoordinateType const &antCoordType,
				      casa::Matrix<double> const &skyPositions,
				      CR::CoordinateType const &skyCoordType,
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
    
    // array with the computed phases, [antenna,skyPosition,frequency]
    casa::Cube<double> phases (nofChannels,
			       shape(0),
			       shape(1));

    /* Use a Slicer object to directly insert the array with the delays into
       the array holding the phases. */

    casa::IPosition end (3,1,shape(0),shape(1));
    casa::IPosition incr (3,1,1,1);
    casa::Array<double> tmp;

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
    
    /*
      We need to distinguish between casacore and CASA at this point; there are 
      a number of methods available in casacore which do not show up in CASA.
    */
    
    if (nofChannels == 1 || shape(0)*shape(1) == 1) {
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
      for (nChannel=0; nChannel<nofChannels; nChannel++) {
	// create Slicer object to address the target array
	casa::Slicer slice (casa::IPosition(3,nChannel,0,0),
			    end,
			    incr,
			    casa::Slicer::endIsLength);
	// have the Slicer object reference the target region in the output array
	tmp.reference (phases(slice).nonDegenerate());
	// calculate phases from the delays and store them
	tmp = CR::_2pi*frequencies_p(nChannel)*delays;
      }
    }

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
