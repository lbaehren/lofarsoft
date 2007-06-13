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
    casa::Vector<double> frequencies (1);
    frequencies = 0.0;
    setFrequencies (frequencies);
  }
  
  // ----------------------------------------------------------- GeometricalPhase
  
#ifdef HAVE_CASA
  GeometricalPhase::GeometricalPhase (casa::Vector<double> const &frequencies,
				      bool const &bufferPhases)
    : GeometricalDelay (),
      bufferPhases_p(false)
  {
    setFrequencies (frequencies,bufferPhases);
  }
#else
#ifdef HAVE_BLITZ
  GeometricalPhase::GeometricalPhase (blitz::Array<double,1> const &frequencies,
				      bool const &bufferPhases)
    : GeometricalDelay (),
      bufferPhases_p(false)
  {
    setFrequencies (frequencies,bufferPhases);
  }
#endif
#endif
  
  // ----------------------------------------------------------- GeometricalPhase

#ifdef HAVE_CASA
  GeometricalPhase::GeometricalPhase (casa::Matrix<double> const &antPositions,
				      casa::Matrix<double> const &skyPositions,
				      casa::Vector<double> const &frequencies,
				      bool const &bufferDelays,
				      bool const &bufferPhases)
    : GeometricalDelay (antPositions,skyPositions,bufferDelays)
  {
    setFrequencies (frequencies,bufferPhases);
  }
#else
#ifdef HAVE_BLITZ
  GeometricalPhase::GeometricalPhase (const blitz::Array<double,2> &antPositions,
				      const blitz::Array<double,2> &skyPositions,
				      blitz::Array<double,1> const &frequencies,
				      const bool &bufferDelays,
				      bool const &bufferPhases)
    : GeometricalDelay (antPositions,skyPositions,bufferDelays)
  {
    setFrequencies (frequencies,bufferPhases);
  }
#endif
#endif
  // ----------------------------------------------------------- GeometricalPhase

  GeometricalPhase::GeometricalPhase (GeometricalDelay const &delay)
    : GeometricalDelay (delay)
  {
    casa::Vector<double> frequencies (1);
    frequencies = 0.0;
    setFrequencies (frequencies,false);
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

#ifdef HAVE_CASA
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
#else
#ifdef HAVE_BLITZ
  bool GeometricalPhase::setFrequencies (const blitz::Array<double,1> &frequencies,
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
#endif
#endif
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  // --------------------------------------------------------------------- phases

#ifdef HAVE_CASA
  casa::Cube<double> GeometricalPhase::phases ()
  {
    if (bufferPhases_p) {
      std::cout << "-- Returning buffered phases..." << std::endl;
      return phases_p;
    } else {
      std::cout << "-- Returning recomputed phases..." << std::endl;
      return calcPhases();
    }
  }
#else
#ifdef HAVE_BLITZ
  blitz::Array<double,3> GeometricalPhase::phases ()
  {
    if (bufferPhases_p) {
      std::cout << "-- Returning buffered phases..." << std::endl;
      return phases_p;
    } else {
      std::cout << "-- Returning recomputed phases..." << std::endl;
      return calcPhases();
    }
  }
#endif
#endif
  
  // ------------------------------------------------------------------ setPhases

  void GeometricalPhase::setPhases ()
  {
    if (bufferPhases_p) {
      delays_p.resize (GeometricalDelay::nofAntennaPositions(),
		       GeometricalDelay::nofSkyPositions(),
		       nofFrequencies());
      phases_p = calcPhases ();
    }
  }

  // ----------------------------------------------------------------- calcPhases

#ifdef HAVE_CASA
  casa::Cube<double> GeometricalPhase::calcPhases ()
  {
    int nofChannels (nofFrequencies());
    int nChannel (0);

    // get the geometrical delays
    casa::Matrix<double> delays = GeometricalDelay::delays();
    casa::IPosition shape       = delays.shape();
    
    // array with the computed phases
    casa::Cube<double> phases (shape(0),shape(1),nofChannels);

    // compute the phases
    for (nChannel=0; nChannel<nofChannels; nChannel++) {
      phases.xyPlane(nChannel) = CR::_2pi*frequencies_p(nChannel)*delays;
    }

    return phases;
  }
#else
#ifdef HAVE_BLITZ
  blitz::Array<double,3> GeometricalPhase::calcPhases ()
  {

    // get the geometrical delays
    blitz::Array<double,2> delays = GeometricalDelay::delays();
    
    
    // array with the computed phases
    int nofAntennas (delays.rows());
    int nofPointings (delays.cols());
    int nofChannels (nofFrequencies());
    int nChannel (0);
    blitz::Array<double,3> phases (nofAntennas,nofPointings,nofChannels);

    // compute the phases
    for (nChannel=0; nChannel<nofChannels; nChannel++) {
      phases(Range::all(),Range::all(),nChannel) = CR::_2pi*frequencies_p(nChannel)*delays;
    }

    return phases;
  }
#endif
#endif
  
  void GeometricalPhase::summary (std::ostream &os)
  {
    // first the summary of the base class
    GeometricalDelay::summary(os);
    
    // Summary of the parameters added with this object
    os << "[GeometricalPhase] Summary of object" << std::endl;
    os << "-- Frequency values : " << frequencies_p.shape() << std::endl;
  }

} // NAMESPACE CR -- END
