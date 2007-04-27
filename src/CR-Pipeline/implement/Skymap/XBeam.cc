/***************************************************************************
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

/* $Id: XBeam.cc,v 1.1 2007/03/19 12:29:47 bahren Exp $*/

#include <lopes/Skymap/XBeam.h>

// =============================================================================
//
//  Construction
//
// =============================================================================

XBeam::XBeam ()
  : method_p(method3), sigma_p (1.0), blockaverage_p(3)
{}

XBeam::XBeam (XBeam::method which,
	      const double& sigma,
	      const int& blockaverage)
  : method_p(which), sigma_p (sigma), blockaverage_p(blockaverage)
{}


// =============================================================================
//
//  Destruction
//
// =============================================================================

XBeam::~XBeam () {}


// =============================================================================
//
//  Access to the computation parameters
//
// =============================================================================

XBeam::method XBeam::methodType ()
{
  return method_p;
}

String XBeam::methodName ()
{
  return XBeam::methodName(method_p);
}

String XBeam::methodName (XBeam::method method)
{
  switch (method) {
  case method1: return String ("method1");
  case method2: return String ("method2");
  case method3: return String ("method3");
  case method4: return String ("method4");
  default: 
    std::cerr << "[XBeam::methodName] Unsupported method" << std::endl;
  }
  return String ("UNKNOWN");
}


// =============================================================================
//
//  Computation of the X-Beam
//
// =============================================================================

Vector<double> XBeam::xbeam (const Matrix<double>& antennaData)
{
  bool verbose (true);
  IPosition shape (antennaData.shape());
  Vector<double> powerAC (shape(1));
  Vector<double> powerCC (shape(1));
  int nofCC (0);
  int nofAC (0);

  for (int ant1(0); ant1<shape(0)-1; ant1++) {
    for (int ant2(ant1+1); ant2<shape(0); ant2++) {
      powerCC += antennaData.row(ant1)*antennaData.row(ant2);
      nofCC++;
    }
    powerAC += antennaData.row(ant1)*antennaData.row(ant1);
    nofAC++;
  }

  // do not forget about the last AC contribution:
  powerAC += antennaData.row(shape(0)-1)*antennaData.row(shape(0)-1);

  // normalization of the sums
  {
    Vector<double> norm (powerCC.shape());
    //
    norm = nofCC;
    powerCC /= norm;
    //
    norm = nofAC;
    powerAC /= norm;
  }

  // Get statistical properties
  double meanCC (mean(powerCC));
  double meanAC (mean(powerAC));
  double stddevCC (stddev(powerCC));
  double stddevAC (stddev(powerAC));

  if (verbose) {
    std::cout << "[XBeam::xbeam]" << std::endl;
    std::cout << " - Auto-correlation power - mean    : " << meanAC   << std::endl;
    std::cout << " - Auto-correlation power - stddev  : " << stddevAC << std::endl;
    std::cout << " - Cross-correlation power - mean   : " << meanCC   << std::endl;
    std::cout << " - Cross-correlation power - stddev : " << stddevCC << std::endl;
  }

  return powerCC;
}
