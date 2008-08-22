/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
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

#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace CR {  // Namespace CR -- begin
  
  /*!
    \brief A collection of numerical constants for common usage
    
    \ingroup CR_Math
    
    <h3>References</h3>
    
    <ul>
      <li><tt>casa/BasicSL/Constants.h</tt> -- Mathematical and physical constants
      implemented with the CASA libraries
      <li>Abramowitz & Stegun (1964) Handbook of Mathematical Functions, Dover
      Publications Inc., New York (9th printing, 1972)
    </ul>
  */

  //=============================================================================
  //
  //  Mathematical constants
  //
  //=============================================================================
  
  /*
    Irrationals
  */
  //! Square-root of 2, \f$ \sqrt{2} \f$
  const double sqrt2    = 1.4142135623730950488;
  //! Square-root of 3, \f$ \sqrt{3} \f$
  const double sqrt3    = 1.7320508075688772935;
  //! One over square-root of 2, \f$ 1/\sqrt{2} \f$
  const double _1_sqrt2	= 0.70710678118654752440;
  //! One over square-root of 3, \f$ 1/\sqrt{3} \f$
  const double _1_sqrt3 = 0.57735026918962576451;
  
  // PI and functions thereof:
  //! \f$ \pi \f$
  const double pi   = 3.1415926535897932384626433832795;
  const double _2pi = 2*pi;
  
  // Angular measure:
  const double radian  = 1.0;
  //! Circumference of a circle with unit radius, \f$ 2 \pi \f$
  const double circle  = 6.2831853071795864769252867;
  //! Conversion factor from degree to radian, \f$ \pi/180^\circ \f$
  const double degree  = 0.0174532925199432957692369;
  const double arcmin  = 0.000290888208665721596153948459;
  const double arcsec  = 0.00000484813681109535993589914098765;
  
  //=============================================================================
  //
  //  Astronomical/Physical constants
  //
  //=============================================================================
  
  //! Vacuum velocity of light, [m/s]
  const double lightspeed = 299792458.0;

}  // Namespace CR -- end

#endif
