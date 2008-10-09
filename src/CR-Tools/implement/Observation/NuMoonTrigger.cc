/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Kalpana Singh (<k.singh@rug.nl>)                                      *
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

#include <Observation/NuMoonTrigger.h>

namespace  { // Namespace  -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  NuMoonTrigger::NuMoonTrigger ()
  {;}
  
 // NuMoonTrigger::NuMoonTrigger (NuMoonTrigger const &other)
//   {
//     copy (other);
//   }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  NuMoonTrigger::~NuMoonTrigger ()
  {
    destroy();
  }
  
  void NuMoonTrigger::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  NuMoonTrigger& NuMoonTrigger::operator= (NuMoonTrigger const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void NuMoonTrigger::copy (NuMoonTrigger const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void NuMoonTrigger::summary (std::ostream &os)
  {;}
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  
} // Namespace  -- end
