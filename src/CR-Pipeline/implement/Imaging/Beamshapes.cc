/*-------------------------------------------------------------------------*
 | $Id: Beamformer.h 445 2007-07-16 16:03:40Z baehren $ |
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

#include <Imaging/Beamshapes.h>

namespace CR { // Namespace CR -- begin
  
  double Beamshapes::test_double (const double *par,
				  const double *x)
  {
    // check if we try to point below the horizon
    if (x[3]<=0) {
      std::cerr << "Error: tried to point below the horizon!" << std::endl;
      return 0;
    }
    
    const double c=3.e8;              // speed of light
    const double theta=M_PI/2-x[3];
    const double phi=par[1]+x[2];
    const double result=(1-sqr(sin(theta))*sqr(sin(phi)))*sqr(sin(2*M_PI*x[1]*par[2]*cos(theta)/c));
    return (par[0]*sqrt(result));
    
  }
  
} // Namespace CR -- end
