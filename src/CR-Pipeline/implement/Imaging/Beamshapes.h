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

#ifndef BEAMSHAPES_H
#define BEAMSHAPES_H

// Standard library header files
#include <iostream>
#include <string>
#include <cmath>
#include <complex>

using std::complex;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class Beamshapes

    \ingroup Imaging
    
    \brief Analystic expressions for LOFAR-CS1 beamshapes
    
    \author Lars B&auml;hren

    \date 2007/08/02

    \test tBeamshapes.cc
    
    <h3>Prerequisite</h3>
    
    <h3>Synopsis</h3>

    This file collects code developed by Sarod Yatawatta, intended to describe
    the beamshape of individual LOFAR dipoles and therewith later on the proper
    synthesized beam of a station.
  */  
  class Beamshapes {

  public:

    static double sqr (double x){
      return x*x;
    }

    static complex<double> test_complex (const complex<double> *par,
					 const complex<double> *x) {
      return 0;
    }    
    
    /*!
      \brief Description still missing

       Evaluation of the equation:
       \f[ (1-\sin(\theta)^2 \sin(\phi)^2) \sin(2*\pi*f/c*h_0*\cos(\theta))^2 \f]
       where 
       <li>c: speed of light,
       <li> f : frequency h_0: height from ground
       <li>th: pi/2-elevation
       <li>phi: phi_0+azimuth, phi_0: dipole orientation
       <li>parms: scale, phi_0, h_0
       <li>axes: time,freq, az, el

      \param par -- 
      \param x   -- 

      \return result -- 
    */
    static double test_double (const double *par,
			       const double *x);
    
  };
  
} // Namespace CR -- end

#endif /* BEAMFORMER_H */
  
