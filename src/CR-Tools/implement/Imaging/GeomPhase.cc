/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
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

#include <Imaging/GeomPhase.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  GeomPhase::GeomPhase ()
    : GeomDelay()
  {
    init();
  }
  
  GeomPhase::GeomPhase (GeomPhase const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  GeomPhase::~GeomPhase ()
  {
    destroy();
  }
  
  void GeomPhase::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  GeomPhase& GeomPhase::operator= (GeomPhase const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void GeomPhase::copy (GeomPhase const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void GeomPhase::summary (std::ostream &os)
  {
    os << "[GeomPhase] Summary of internal parameters." << std::endl;
  }
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                         init

  void GeomPhase::init ()
  {
    Vector<double> freq (1,0.0);
    
    init (freq,
	  false);
  }

  //_____________________________________________________________________________
  //                                                                         init

  void GeomPhase::init (Vector<double> const &frequencies,
			bool const &bufferPhases)
  {
    bufferPhases_p = bufferPhases;

    frequencies_p.resize(frequencies.shape());
    frequencies_p = frequencies;
  }
  
} // Namespace CR -- end
