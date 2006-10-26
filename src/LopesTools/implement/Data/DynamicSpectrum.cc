/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

/* $Id: template-class.cc,v 1.9 2006/04/14 11:34:17 bahren Exp $*/

#include <Data/DynamicSpectrum.h>

namespace lopestools {
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  DynamicSpectrum::DynamicSpectrum ()
  {  
    init (defaultShape());
  }
  
  DynamicSpectrum::DynamicSpectrum (std::vector<int> const &shape)
  {
    init(shape);
  }
  
  DynamicSpectrum::DynamicSpectrum (std::vector<int> const &shape,
				    std::vector<double> const &crpix,
				    std::vector<double> const &crval,
				    std::vector<double> const &cdelt)
  {
}
  
  DynamicSpectrum::DynamicSpectrum (DynamicSpectrum const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  DynamicSpectrum::~DynamicSpectrum ()
  {
    destroy();
  }
  
  void DynamicSpectrum::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  DynamicSpectrum& DynamicSpectrum::operator= (DynamicSpectrum const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void DynamicSpectrum::copy (DynamicSpectrum const &other)
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
  
  void DynamicSpectrum::init (std::vector<int> const &shape)
  {
    uint nelem (shape.size());
    
    if (nelem == nofAxes) {
      // copy the pixel map shape
      shape_p.resize(nofAxes);
      shape_p = shape;
      // Coordinate axes information
      crpix_p.resize(nofAxes,1.0);
      crval_p.resize(nofAxes,1.0);
      cdelt_p.resize(nofAxes,1.0);
    } else {
      std::cerr << "[DynamicSpectrum::init] Invalid number of axes" << std::endl;
      init (defaultShape());
    }
  }
  
}
