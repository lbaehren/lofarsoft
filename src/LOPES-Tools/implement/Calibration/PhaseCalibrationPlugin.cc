/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Kalpana Singh (<k.singh@astro.ru.nl>)                                 *
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

/* $Id: PhaseCalibrationPlugin.cc,v 1.2 2006/11/08 16:06:41 bahren Exp $*/

#include <lopes/Calibration/PhaseCalibrationPlugin.h>

/*!
  \class PhaseCalibrationPlugin
*/

namespace LOPES { // Namespace LOPES -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  PhaseCalibrationPlugin::PhaseCalibrationPlugin ()
  {;}
  
  PhaseCalibrationPlugin::PhaseCalibrationPlugin (PhaseCalibrationPlugin const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  PhaseCalibrationPlugin::~PhaseCalibrationPlugin ()
  {
    destroy();
  }
  
  void PhaseCalibrationPlugin::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  PhaseCalibrationPlugin& PhaseCalibrationPlugin::operator= (PhaseCalibrationPlugin const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void PhaseCalibrationPlugin::copy (PhaseCalibrationPlugin const &other)
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
  
  

} // Namespace LOPES -- end
