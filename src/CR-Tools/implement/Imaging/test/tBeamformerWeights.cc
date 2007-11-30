/*-------------------------------------------------------------------------*
 | $Id                                                                     |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (<mail>)                                                 *
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

#include <Imaging/GeometricalDelay.h>
#include <Imaging/GeometricalPhase.h>
#include <Imaging/GeometricalWeight.h>
#include <Imaging/BeamformerWeights.h>
#include "create_data.h"

using CR::GeometricalPhase;
using CR::GeometricalWeight;
using CR::BeamformerWeights;

/*!
  \file tBeamformerWeights.cc

  \ingroup Imaging

  \brief A collection of test routines for the BeamformerWeights class
 
  \author Lars B&auml;hren
 
  \date 2007/11/29
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new BeamformerWeights object

  \return nofFailedTests -- The number of failed tests.
*/
int test_BeamformerWeights ()
{
  std::cout << "\n[test_BeamformerWeights]\n" << std::endl;

  int nofFailedTests (0);
  casa::Matrix<double> antennaPositions = get_antennaPositions();
  casa::Matrix<double> skyPositions     = get_skyPositions();
  casa::Vector<double> frequencies      = get_frequencies();
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    BeamformerWeights w;
    //
    w.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  try {
    BeamformerWeights w (antennaPositions,
			 CR::Cartesian,
			 skyPositions,
			 CR::Cartesian,
			 frequencies);
    //
    w.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Testing construction with GeometricalPhase ..." << std::endl;
  try {
    /* create GeometricalPhase object */
    GeometricalPhase phase (antennaPositions,
			    skyPositions,
			    frequencies);
    /* create BeamformerWeights using phases */
    BeamformerWeights w (phase);
    /* summarize object internals */
    w.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[4] Testing construction with GeometricalWeight ..." << std::endl;
  try {
    /* create GeometricalWeight object */
    GeometricalWeight weight (antennaPositions,
			      skyPositions,
			      frequencies);
    /* create BeamformerWeights using weights */
    BeamformerWeights w (weight);
    /* summarize object internals */
    w.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[5] Testing copy constructor ..." << std::endl;
  try {
    BeamformerWeights w (antennaPositions,
			 CR::Cartesian,
			 skyPositions,
			 CR::Cartesian,
			 frequencies);
    //
    w.summary(); 
    //
    BeamformerWeights wCopy (w);
    //
    wCopy.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_BeamformerWeights ();
  }

  return nofFailedTests;
}
