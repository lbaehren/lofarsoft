/***************************************************************************
 *   Copyright (C) 2005                                                    *
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

/* $Id$*/

#include <iostream>
#include <string>

#include <casa/BasicSL/Complex.h>

#include <IO/DataIterator.h>

using namespace std;
using namespace CR;

/*!
  \file tDataIterator.cc

  \ingroup IO

  \brief A collection of test routines for DataIterator
 
  \author Lars B&auml;hren
 
  \date 2005/11/08
*/


/*!
  \brief Test adjustment of the stepwidth

  The stepwidth can be used to adapt to different data type (e.g. int, float,
  bool).

  \return nofFailedTests -- The number of failed tests.
 */
int test_stepWidth ();

/*!
  \brief Test error catching for block counter

  \return nofFailedTests -- The number of failed tests.
 */
int test_block ();

/*!
  \brief Test computation of maximum number of readible blocks

  \return nofFailedTests -- The number of failed tests.
*/
int test_maxNofBlocks ();

// ------------------------------------------------------------ show_DataIterator

/*!
  \brief Show the values of the member data

  \param di -- DataIterator object to display.
 */
void show_DataIterator (DataIterator &di)
{
  cout << " -- dataStart : " << di.dataStart() << endl;
  cout << " -- blocksize : " << di.blocksize() << endl;
  cout << " -- stride    : " << di.stride()    << endl;
  cout << " -- shift     : " << di.shift()     << endl;
  cout << " -- block     : " << di.block()     << endl;
  cout << " -- stepWidth : " << di.stepWidth() << endl;
  cout << " -- position  : " << di.position()  << endl;
}

// ---------------------------------------------------------------- run_nextBlock

/*!
  \brief Iterate of a number of data block to monitor progression of stream position

  \param di -- DataIterator object to display.
 */
void run_nextBlock (DataIterator di)
{
  int nofBlocks (6);
  
  cout << " stride = " << di.stride() << "\t" << di.position();
  for (int n(0); n<nofBlocks; n++) {
    di.nextBlock ();
    cout << " .. " << di.position();
  }
  cout << endl;
}

// ------------------------------------------------------------ test_DataIterator

/*!
  \brief Test constructors for a new DataIterator object

  Test for <tt>T</tt> = <i>unsigned int</i>, <i>int</i>, <i>long</i>, <i>float</i>,
  <i>double</i>

  \return nofFailedTests -- The number of failed tests.
*/
int test_DataIterator ()
{
  cout << "\n[test_DataIterator]\n" << endl;

  int nofFailedTests (0);
  int blocksize (4096);
  int dataStart (1024);
  int stride (-512);
  int shift (100);

  cout << "[1] Testing default constructor" << endl;
  {
    DataIterator di;
    di.summary();
  }
  
  cout << "[2] Testing DataIterator(blocksize)" << endl;
  {
    DataIterator di (blocksize);
    di.summary();
  }
  
  cout << "[3] Testing DataIterator(blocksize,dataStart)" << endl;
  {
    DataIterator di (blocksize,dataStart);
    di.summary();
  }
  
  cout << "[4] Testing DataIterator(blocksize,dataStart,stride,shift)" << endl;
  {
    DataIterator di (blocksize,
		     dataStart,
		     stride,
		     shift);
    di.summary();
  }
  
  cout << "[5] Testing DataIterator(other)" << endl;
  {
    DataIterator di1 (blocksize,
		      dataStart,
		      stride,
		      shift);
    //
    DataIterator di2 (di1);
    di2.summary();
  }
  
  return nofFailedTests;
}

// --------------------------------------------------------------- test_stepWidth

int test_stepWidth ()
{
  cout << "\n[test_stepWidth]\n" << endl;

  int nofFailedTests (0);

  DataIterator di;

  cout << "[1] Default step width" << endl;
  try {
    di.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[2] Step width for bool" << endl;
  try {
    bool step (1);
    di.setStepWidth (step);
    //
    di.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[3] Step width for int" << endl;
  try {
    int step (1);
    di.setStepWidth (step);
    //
    di.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[4] Step width for long" << endl;
  try {
    long step (1);
    di.setStepWidth (step);
    //
    di.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[5] Step width for float" << endl;
  try {
    float step (1);
    di.setStepWidth (step);
    //
    di.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[6] Step width for double" << endl;
  try {
    double step (1);
    di.setStepWidth (step);
    //
    di.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------- test_block

int test_block ()
{
  cout << "\n[test_block]\n" << endl;

  int nofFailedTests (0);
  unsigned int nofBlocks (10);
  unsigned int blocksize (1024);
  DataIterator di (blocksize);

  cout << "[1] Test advancing of block counter ..." << endl;
  try {
    for (unsigned int block(0); block<nofBlocks; block++) {
      di.setBlock(block);
      //
      cout << "\t" << block << "\t" << di.block() << "\t" << di.position() << endl;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ---------------------------------------------------------------- test_settings

/*!
  \brief Test access to the iterator settings

  \return nofFailedTests -- The number of failed tests.
 */
int test_settings ()
{
  cout << "\n[test_settings]\n" << endl;

  int nofFailedTests (0);
  //
  int blocksize (2048);
  int dataStart (1000);
  int stride (blocksize/4);
  int shift (100);

  cout << "[1] Testing access to DataIterator settings" << endl;
  {
    cout << " - Default constructor" << endl;
    DataIterator di;
    show_DataIterator (di);
    //
    cout << " - New value for 'blocksize'" << endl;
    di.setBlocksize (blocksize);
    show_DataIterator (di);
    //
    cout << " - New value for 'dataStart'" << endl;
    di.setDataStart (dataStart);
    show_DataIterator (di);
    //
    cout << " - New value for 'stride'" << endl;
    di.setStride (stride);
    show_DataIterator (di);
    //
    cout << " -  New value for 'shift'" << endl;
    di.setShift (shift);
    show_DataIterator (di);
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------ test_stride

/*!
  \brief Test the effect of the stride parameter

  The function tests different combinations of stride (-blocksize, -100, 0 100)
  and variable type (int, float, double):
  \verbatim
  [1] Testing stride for DataIterator<int>
   stride = -2048 0 .. 0 .. 0 .. 0 .. 0 .. 0 .. 0
   stride = -100  0 .. 7792 .. 15584 .. 23376 .. 31168 .. 38960 .. 46752
   stride = 0     0 .. 8192 .. 16384 .. 24576 .. 32768 .. 40960 .. 49152
   stride = 100   0 .. 8592 .. 17184 .. 25776 .. 34368 .. 42960 .. 51552
  [2] Testing stride for DataIterator<float>
   stride = -2048 0 .. 0 .. 0 .. 0 .. 0 .. 0 .. 0
   stride = -100  0 .. 7792 .. 15584 .. 23376 .. 31168 .. 38960 .. 46752
   stride = 0     0 .. 8192 .. 16384 .. 24576 .. 32768 .. 40960 .. 49152
   stride = 100   0 .. 8592 .. 17184 .. 25776 .. 34368 .. 42960 .. 51552
  [3] Testing stride for DataIterator<double>
   stride = -2048 0 .. 0 .. 0 .. 0 .. 0 .. 0 .. 0
   stride = -100  0 .. 15584 .. 31168 .. 46752 .. 62336 .. 77920 .. 93504
   stride = 0     0 .. 16384 .. 32768 .. 49152 .. 65536 .. 81920 .. 98304
   stride = 100   0 .. 17184 .. 34368 .. 51552 .. 68736 .. 85920 .. 103104
  \endverbatim

  \return nofFailedTests -- The number of failed tests.
*/
int test_stride ()
{
  cout << "\n[test_stride]\n" << endl;

  int nofFailedTests (0);
  //
  int blocksize (2048);

  cout << "[1] Testing stride for DataIterator" << endl;
  {
    DataIterator di (blocksize);
    //
    di.setStride (-blocksize);
    run_nextBlock (di);
    //
    di.setStride (-100);
    run_nextBlock (di);
    //
    di.setStride (0);
    run_nextBlock (di);
    //
    di.setStride (100);
    run_nextBlock (di);
  }

  cout << "[1] Testing stride for DataIterator" << endl;
  {
    DataIterator di (blocksize);
    di.setStepWidth (int(1));
    //
    di.setStride (-blocksize);
    run_nextBlock (di);
    //
    di.setStride (-100);
    run_nextBlock (di);
    //
    di.setStride (0);
    run_nextBlock (di);
    //
    di.setStride (100);
    run_nextBlock (di);
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------- test_shift

/*!
  \brief Test methods for introducing a shift in the starting position.

  \return nofFailedTests -- The number of failed tests.
*/
int test_shift ()
{
  cout << "\n[test_shift]\n" << endl;

  int nofFailedTests (0);
  int blocksize (1000);
  int shift (100);

  DataIterator di (blocksize);

  cout << "[1] Default value of shift" << endl;
  run_nextBlock (di);
  //
  cout << "[2] Shift of " << shift << " bytes" << endl;
  di.setShift(shift);
  run_nextBlock (di);
  //
  cout << "[2] Shift of " << shift << " samples" << endl;
  di.setShiftInSamples(shift);
  run_nextBlock (di);
  //
  cout << "[2] Shift of " << shift << " blocks" << endl;
  di.setShiftInBlocks(shift);
  run_nextBlock (di);

  return nofFailedTests;
}
  

// --------------------------------------------------------- test_multipleStreams

/*!
  \brief Test the parallel handling of multiple streams

  \return nofFailedTests -- The number of failed tests.
*/
int test_multipleStreams ()
{
  cout << "\n[test_multipleStreams]\n" << endl;

  int nofFailedTests (0);
  int nofStreams (10);
  int blocksize (1000);
  int n (0);
  
  {
    DataIterator *di = new DataIterator[nofStreams];

    for (n=0; n<nofStreams; n++) {
      cout << " - stream " << n << " : " << di[n].blocksize() << " -> ";
      di[n].setBlocksize(blocksize+n);
      cout << di[n].blocksize() << endl;
      //
      run_nextBlock (di[n]);
    }

    delete [] di;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------ test_maxNofBlocks

int test_maxNofBlocks ()
{
  cout << "\n[test_maxNofBlocks]\n" << endl;

  int nofFailedTests (0);
  int blocksize (1024);
  int dataStart (1024);
  int stride (-512);
  int shift (100);
  int filesize (10000);

  {
    DataIterator di(blocksize,dataStart,stride,shift);
    //
    cout << " - Maximum number of blocks [unsigned int] = "
	 << di.maxNofBlocks (filesize) << endl;
  }

  {
    DataIterator di(blocksize,dataStart,stride,shift);
    //
    cout << " - Maximum number of blocks          [int] = "
	 << di.maxNofBlocks (filesize) << endl;
  }

  {
    DataIterator di(blocksize,dataStart,stride,shift);
    //
    cout << " - Maximum number of blocks        [float] = "
	 << di.maxNofBlocks (filesize) << endl;
  }

  {
    DataIterator di(blocksize,dataStart,stride,shift);
    //
    cout << " - Maximum number of blocks       [double] = "
	 << di.maxNofBlocks (filesize) << endl;
  }

  return nofFailedTests;
}

// --------------------------------------------------------------- test_typeSizes

int test_typeSizes ()
{
  cout << "\n[test_maxNofBlocks]\n" << endl;

  int nofFailedTests (0);
  
  bool var_bool (true);
  int var_short (0);
  int var_int (0);
  uint var_uint (0);
  float var_float (0);
  double var_double (0);
  casa::Double var_Double (0);
  casa::Complex var_Complex (0);
  casa::DComplex var_DComplex (0);

  cout << " bool     = " << sizeof(var_bool)   << endl;
  cout << " short    = " << sizeof(var_short)  << endl;
  cout << " int      = " << sizeof(var_int)    << endl;
  cout << " uint     = " << sizeof(var_uint)   << endl;
  cout << " float    = " << sizeof(var_float)  << endl;
  cout << " double   = " << sizeof(var_double) << endl;
  cout << " Double   = " << sizeof(var_Double) << endl;
  cout << " Complex  = " << sizeof(var_Complex) << endl;
  cout << " DComplex = " << sizeof(var_DComplex) << endl;

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test the sizes of the various data types
  nofFailedTests += test_typeSizes ();
  
  // Test for the constructor(s)
  nofFailedTests += test_DataIterator ();
  // Test effect of different step widths
  nofFailedTests += test_stepWidth ();
  // Some explicit testing for the block counter
  nofFailedTests += test_block ();
  // Test access to the DataIterator settings
  nofFailedTests += test_settings ();
  // Test progression through the data volume
  nofFailedTests += test_stride ();
  // Test methods for introducing a shift in the starting position.
  nofFailedTests += test_shift ();
  // Test parallel handling of multiple streams
  nofFailedTests += test_multipleStreams ();
  // Test computation of maximum number of readible blocks
  nofFailedTests += test_maxNofBlocks ();
  
  return nofFailedTests;
}
