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

/* $Id: tAxisIterator.cc,v 1.1 2006/07/05 16:01:08 bahren Exp $*/

#include <lopes/Coordinates/AxisIterator.h>

/*!
  \file tAxisIterator.cc

  \brief A collection of test routines for AxisIterator
 
  \author Lars Bahren
 
  \date 2006/03/15
*/

void show_axes (AxisIterator& axis)
{
  std::cout.precision(12);
  cout << " - block : " << axis.block() << endl;
  cout << " - Pos   : " << axis.axisValues(AxisCoordinates::Pos) << endl;
  cout << " - Time  : " << axis.axisValues(AxisCoordinates::Time) << endl;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test direct computation of position from basic parameters
*/
void computePosition () 
{
  int blocksize (20);
  int nofBlocks (10);
  int block (0);
  int sample (0);
  int pos (0);
  int presync (0);

  for (block=1; block<=nofBlocks; block++) {
    cout << block << " : [";
    for (sample=0; sample<blocksize; sample++) {
      pos = ((block-1)*blocksize) + sample;
      cout << " " << pos;
    }
    cout << " ]" << endl;
  }
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new AxisIterator object

  \return nofFailedTests -- The number of failed tests.
*/
Int test_AxisIterator ()
{
  Int nofFailedTests (0);
  int block (2);
  Int blocksize (128);
  Int offset (0);
  Int presync (32768);
  Double sampleFrequency (80e06);
  Vector<Double> frequencyRange(2);
  
  frequencyRange(0) = 40e06;
  frequencyRange(1) = 80e06;
  
  cout << "\n[test_AxisIterator]\n" << endl;

  cout << "[1] Testing default constructor ..." << endl;
  {
    AxisIterator axis;
    //
    cout << " - block number      : " << axis.block() << endl;
    cout << " - blocksize         : " << axis.blocksize() << endl;
    cout << " - offset            : " << axis.offset() << endl;
    cout << " - presync           : " << axis.presync() << endl;
    cout << " - sample frequency  : " << axis.sampleFrequency() << endl;
  }
  
  cout << "[2] Testing argumented constructor ..." << endl;
  {
    AxisIterator axis (block);
    //
    cout << " - block number      : " << axis.block() << endl;
    cout << " - blocksize         : " << axis.blocksize() << endl;
    cout << " - offset            : " << axis.offset() << endl;
    cout << " - presync           : " << axis.presync() << endl;
    cout << " - sample frequency  : " << axis.sampleFrequency() << endl;
  }
  
  cout << "[3] Testing argumented constructor ..." << endl;
  {
    AxisIterator axis (blocksize,
		       offset,
		       presync,
		       sampleFrequency,
		       frequencyRange);
    show_axes (axis);
  }
  
  cout << "[4] Testing argumented constructor ..." << endl;
  {
    AxisCoordinates axis (blocksize,
			  offset,
			  presync,
			  sampleFrequency,
			  frequencyRange);
    AxisIterator iterator (block,
			   axis);
    
    show_axes (iterator);
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

Int test_Block ()
{
  Int nofFailedTests (0);
  Int blocksize (128);
  Int offset (0);
  Int presync (32768);
  Double sampleFrequency (80e06);
  Vector<Double> frequencyRange(2);
  
  frequencyRange(0) = 40e06;
  frequencyRange(1) = 80e06;
  
  cout << "\n[test_AxisIterator]\n" << endl;

  {
    AxisIterator axis (blocksize,
		       offset,
		       presync,
		       sampleFrequency,
		       frequencyRange);
    
    show_axes (axis);
    axis.nextBlock();
    show_axes (axis);

  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  Int nofFailedTests (0);

  {
    computePosition ();
  }

  // Test for the constructor(s)
  {
    nofFailedTests += test_AxisIterator ();
  }

  {
    nofFailedTests += test_Block ();
  }

  return nofFailedTests;
}
