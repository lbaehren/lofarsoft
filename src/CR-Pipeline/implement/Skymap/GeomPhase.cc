/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars Bahren (bahren@astron.nl)                                        *
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

/* $Id: GeomPhase.cc,v 1.1 2006/10/31 21:07:59 bahren Exp $*/

#include <lopes/Beamforming/GeomPhase.h>

/*!
  \class GeomPhase
*/

namespace LOPES {
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  GeomPhase::GeomPhase ()
    : GeomDelay ()
  {;}
  
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
  
  void GeomPhase::setPhase ()
  {
    phase_p = 2*GeomDelay::pi*delay_p;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
}
