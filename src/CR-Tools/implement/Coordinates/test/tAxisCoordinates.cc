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

// Standard library
#include <iostream>
// CASA library
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/Vector.h>
// CR-Tools
#include <Coordinates/AxisCoordinates.h>

/*!
  \file tAxisCoordinates.cc

  \ingroup Coordinates

  \brief A collection of test routines for AxisCoordinates
 
  \author Lars B&auml;hren
 
  \date 2006/03/13
*/

using std::cout;
using std::endl;

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
  cout << " - blocksize         [samples] = " << axis.blocksize()       << endl;
  cout << " - offset            [samples] = " << axis.offset()          << endl;
  cout << " - presync           [samples] = " << axis.presync()         << endl;
  cout << " - sample frequency  [Hz     ] = " << axis.sampleFrequency() << endl;
  cout << " - frequency range   [Hz     ] = " << axis.frequencyRange()  << endl;
  cout << " - FFT output length [samples] = " << axis.fftLength()       << endl;
  cout << " - frequency bin     [Hz     ] = " << axis.frequencyBin()    << endl;
}

// -----------------------------------------------------------------------------

void show_axes (AxisCoordinates& axis)
{
  cout.precision(12);
  cout << " - Pos                    = "
       << axis.axisValues(AxisCoordinates::Pos) << endl;
  cout << " - Time                   = "
       << axis.axisValues(AxisCoordinates::Time) << endl;
  cout << " - Frequency              = "
       << axis.axisValues(AxisCoordinates::Frequency) << endl;
  cout << " - Intermediate frequency = "
       << axis.axisValues(AxisCoordinates::IntermedFreq) << endl;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new AxisCoordinates object

  \return nofFailedTests -- The number of failed tests.
*/
int test_AxisCoordinates ()
{
  int nofFailedTests (0);
  int blocksize (128);
  int offset (0);
  int presync (32768);
  double sampleFrequency (80e06);
  casa::Vector<double> frequencyRange(2);

  frequencyRange(0) = 40e06;
  frequencyRange(1) = 80e06;

  cout << "\n[test_AxisCoordinates]\n" << endl;

  cout << "[1] Testing default constructor ..." << endl;
  {
    AxisCoordinates axis;
    //
    show_parameters (axis);
    show_axes (axis);
  }
  
  cout << "[2] Testing argumented constructor ..." << endl;
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

  cout << "[3] Testing copy constructor" << endl;
  {
    AxisCoordinates axis1 (blocksize,
			   offset,
			   presync,
			   sampleFrequency,
			   frequencyRange);
    AxisCoordinates axis2 (axis1);

    cout << " - Parameters for object 1 ..." << endl;
    show_parameters (axis1);
    cout << " - Parameters for object 2 ..." << endl;
    show_parameters (axis2);
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  nofFailedTests += test_AxisCoordinates ();

  return nofFailedTests;
}
