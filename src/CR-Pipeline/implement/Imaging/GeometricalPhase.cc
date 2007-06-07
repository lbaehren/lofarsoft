/***************************************************************************
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

/* $Id: template-class.cc,v 1.10 2006/10/31 19:19:57 bahren Exp $*/

#include <Imaging/GeometricalPhase.h>

namespace CR { // NAMESPACE CR -- BEGIN
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // ----------------------------------------------------------- GeometricalPhase

  GeometricalPhase::GeometricalPhase ()
    : GeometricalDelay ()
  {;}
  
  // ----------------------------------------------------------- GeometricalPhase
  
#ifdef HAVE_CASA
  GeometricalPhase::GeometricalPhase (casa::Vector<double> const &frequencies,
				      bool const &bufferPhases)
    : GeometricalDelay ()
  {
    setFrequencies (frequencies,bufferPhases);
  }
#else
#ifdef HAVE_BLITZ
  GeometricalPhase::GeometricalPhase (blitz::Array<double,1> const &frequencies,
				      bool const &bufferPhases)
    : GeometricalDelay ()
  {
    setFrequencies (frequencies,bufferPhases);
  }
#endif
#endif
  
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
  {;}

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
    
    // buffer the phases?
    bufferPhases_p = bufferPhases;
    // store the array
    frequencies_p.resize (frequencies.shape());
    frequencies_p = frequencies;
    // update the values of the geometrical delays
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
    
    GeometricalPhase::bufferPhases (bufferPhases);

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
      return phases_p;
    } else {
      return calcPhases();
    }
  }
#else
#ifdef HAVE_BLITZ
  blitz::Array<double,3> GeometricalPhase::phases ()
  {
    if (bufferPhases_p) {
      return phases_p;
    } else {
      return calcPhases();
    }
  }
#endif
#endif
  
  // ------------------------------------------------------------------ setPhases

  void GeometricalPhase::setPhases ()
  {
    delays_p.resize (GeometricalDelay::nofAntennaPositions(),
		     GeometricalDelay::nofSkyPositions(),
		     nofFrequencies());
    phases_p = calcPhases ();
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
      phases.xyPlane(nChannel) = frequencies_p(nChannel)*delays;
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
      phases(Range::all(),Range::all(),nChannel) = frequencies_p(nChannel)*delays;
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
