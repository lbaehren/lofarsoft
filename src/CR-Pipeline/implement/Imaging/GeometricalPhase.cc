/***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Lars Baehren (<mail>)                                                     *
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
  
  GeometricalPhase::GeometricalPhase ()
    : GeometricalDelay ()
  {;}
  
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
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  

} // NAMESPACE CR -- END
