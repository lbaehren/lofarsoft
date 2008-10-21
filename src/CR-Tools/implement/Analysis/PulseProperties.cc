/*-------------------------------------------------------------------------*
 | $Id:: NewClass.cc 1964 2008-09-06 17:52:38Z baehren                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
 *   Frank Schroeder (<mail>)                                                     *
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

// Don't forget to run the following commands in implement/Analysis, if something has changed:
// rm RootDict.*
// rootcint RootDict.C -c PulseProperties.h

#include <Analysis/PulseProperties.h>

// namespace declaration causes problems with root dictionary
// should be solved in ROOT 5.20
//namespace CR { // Namespace CR -- begin

  // Make class available in ROOT dictionary
  ClassImp(PulseProperties)

  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
 PulseProperties::PulseProperties ():
    antennaID(0),
    antenna(0.),
    maximum(0.),
    envelopeMaximum(0.),
    minimum(0.),
    maximumTime(0.),
    envelopeTime(0.),
    minimumTime(0.),
    halfheightTime(0.),
    fwhm(0.)
  {}
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  PulseProperties::~PulseProperties ()
  {}
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void PulseProperties::summary (std::ostream &os)
  {
    os << "[PulseProperties] Summary of internal parameters." << std::endl;
  }
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  
  

//} // Namespace CR -- end
