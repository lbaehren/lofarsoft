/***************************************************************************
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

/* $Id: tAxisCoordinates.cc,v 1.4 2006/08/24 13:55:12 bahren Exp $*/

#include <iostream>
#include <Coordinates/AxisCoordinates.h>

/*!
  \file tAxisCoordinates.cc

  \ingroup Coordinates

  \brief A collection of test routines for AxisCoordinates
 
  \author Lars B&auml;hren
 
  \date 2006/03/13
*/

using casa::Double;
using casa::Int;
using casa::Vector;

using CR::AxisCoordinates;

// -----------------------------------------------------------------------------

/*!
  Here's the output as obtained from the Glish version:

  \verbatim
  - data.getmeta("Blocksize")
  128 
  - data.getmeta("FFTlen")    
  65 
  - data.getmeta("FreqBin")
  625000 
  \endverbatim
*/
void show_parameters (AxisCoordinates& axis)
{
  std::cout << " - blocksize         [samples] = " << axis.blocksize() << std::endl;
  std::cout << " - offset            [samples] = " << axis.offset() << std::endl;
  std::cout << " - presync           [samples] = " << axis.presync() << std::endl;
  std::cout << " - sample frequency  [Hz     ] = " << axis.sampleFrequency() << std::endl;
  std::cout << " - frequency range   [Hz     ] = " << axis.frequencyRange() << std::endl;
  std::cout << " - FFT output length [samples] = " << axis.fftLength() << std::endl;
  std::cout << " - frequency bin     [Hz     ] = " << axis.frequencyBin() << std::endl;
}

// -----------------------------------------------------------------------------

void show_axes (AxisCoordinates& axis)
{
  std::cout.precision(12);
  std::cout << " - Pos                    = "
       << axis.axisValues(AxisCoordinates::Pos) << std::endl;
  std::cout << " - Time                   = "
       << axis.axisValues(AxisCoordinates::Time) << std::endl;
  std::cout << " - Frequency              = "
       << axis.axisValues(AxisCoordinates::Frequency) << std::endl;
  std::cout << " - Intermediate frequency = "
       << axis.axisValues(AxisCoordinates::IntermedFreq) << std::endl;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new AxisCoordinates object

  \return nofFailedTests -- The number of failed tests.
*/
Int test_AxisCoordinates ()
{
  Int nofFailedTests (0);
  Int blocksize (128);
  Int offset (0);
  Int presync (32768);
  Double sampleFrequency (80e06);
  Vector<Double> frequencyRange(2);

  frequencyRange(0) = 40e06;
  frequencyRange(1) = 80e06;

  std::cout << "\n[test_AxisCoordinates]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  {
    AxisCoordinates axis;
    //
    show_parameters (axis);
    show_axes (axis);
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  {
    AxisCoordinates axis (blocksize,
			  offset,
			  presync,
			  sampleFrequency,
			  frequencyRange);
    //
    show_parameters (axis);
    show_axes (axis);
    //
    axis.setOffset(1000);
    show_parameters (axis);
    show_axes (axis);
  }

  std::cout << "[3] Testing copy constructor" << std::endl;
  {
    AxisCoordinates axis1 (blocksize,
			   offset,
			   presync,
			   sampleFrequency,
			   frequencyRange);
    AxisCoordinates axis2 (axis1);

    std::cout << " - Parameters for object 1 ..." << std::endl;
    show_parameters (axis1);
    std::cout << " - Parameters for object 2 ..." << std::endl;
    show_parameters (axis2);
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  Int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_AxisCoordinates ();
  }

  return nofFailedTests;
}
