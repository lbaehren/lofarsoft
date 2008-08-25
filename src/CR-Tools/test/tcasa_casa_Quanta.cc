/*-------------------------------------------------------------------------*
 | $Id:: NewClass.h 1159 2007-12-21 15:40:14Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#include <assert.h>
#include <casa/aips.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/ArrayError.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/Slice.h>
#include <casa/OS/Time.h>
#include <casa/Quanta/Quantum.h>
#include <casa/Quanta/MVEpoch.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Quanta/MVPosition.h>

#include "tests_common.h"

using casa::Array;
using casa::Block;
using casa::IPosition;
using casa::Vector;
using casa::Matrix;
using casa::Cube;
using casa::Slice;

using std::cout;
using std::endl;

/*!
  \file tcasa_casa_Quanta.cc

  \ingroup CR_test

  \brief A number of tests for clases in the casacore casa/Quanta module

  \author Lars B&auml;hren

  \date 2008/03/20

  <h3>Notes</h3>

  <ol>
    <li>Due to the split of the code base and the discontinued developement of
    the original CASA libraries, some of the test might work with <i>casacore</i>
    only; this needs to be handled by precompiler statements.
    <li><a href="http://www.fourmilab.ch/documents/calendar">Calendar converter</a>
  </ol>
*/


// ------------------------------------------------------------------------------

/*!
  \brief Test construction of various types of Quantum<T> objects

  \return nofFailedTests -- The number of failed tests within this function
*/
int test_Quantum ()
{
  cout << "\n[test_Quantum]\n" << endl;
  
  int nofFailedTests (0);

  try {
    casa::Quantum<int> quantInt (1,"s");
    casa::Quantum<float> quantFloat (2.,"s");
    casa::Quantum<double> quantDouble (3.,"s");
    //
    cout << "-- Quantum<int>    = " << quantInt    << endl;
    cout << "-- Quantum<float>  = " << quantFloat  << endl;
    cout << "-- Quantum<double> = " << quantDouble << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test construction of Quantity = Quantum<double> objects

  Create various quantities, e.g. like
  \code
  casa::Quantity time (0.5,"s");
  casa::Quantity length (100,"m");
  casa::Quantity freq (200.0,"MHz");
  \endcode

  \return nofFailedTests -- The number of failed tests within this function
*/
int test_Quantity ()
{
  cout << "\n[test_Quantity]\n" << endl;

  int nofFailedTests (0);

  try {
    casa::Quantity quantTime (0.5,"s");
    casa::Quantity quantLength (100,"m");
    casa::Quantity quantFreq (200.0,"MHz");
    //
    cout << "-- time      = " << quantTime   << endl;
    cout << "-- length    = " << quantLength << endl;
    cout << "-- frequency = " << quantFreq   << endl;
    //
    uint nelem (3);
    casa::String unit ("deg");
    casa::Vector<double> angles (nelem);
    angles(0) = 0;
    angles(1) = 45;
    angles(2) = 90;
    for (uint n(0); n<nelem; n++) {
      casa::Quantity deg (angles(n),unit);
      cout << "-- (" << angles(n) << "," << unit << ") -> " << deg << endl;
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test conversion of quantities
  
  \return nofFailedTests -- The number of failed tests within this function
*/
int test_conversions ()
{
  cout << "\n[test_conversions]\n" << endl;
  
  int nofFailedTests (0);

  cout << "[1] Conversion of frequencies ..." << endl;
  try {
    casa::Quantity freq (200.0,"MHz");
    // 
    cout << "-- Freq [GHz] = " << freq.get(casa::Unit("GHz")) << endl;
    cout << "-- Freq [MHz] = " << freq.get(casa::Unit("MHz")) << endl;
    cout << "-- Freq [kHz] = " << freq.get(casa::Unit("kHz")) << endl;
    cout << "-- Freq [ Hz] = " << freq.get(casa::Unit("Hz"))  << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Conversion of lenghts ..." << endl;
  try {
    casa::Quantity length (1,"m");
    // conversions
    cout << "-- Length [mm] = " << length.get(casa::Unit("mm")) << endl;
    cout << "-- Length [cm] = " << length.get(casa::Unit("cm")) << endl;
    cout << "-- Length [km] = " << length.get(casa::Unit("km")) << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Conversion of angles ..." << endl;
  try {
    casa::Quantity angle (1.0,"deg");
    //conversions
    cout << "-- Angle [   deg] = " << angle.get(casa::Unit("deg"))    << endl;
    cout << "-- Angle [arcsec] = " << angle.get(casa::Unit("arcsec")) << endl;
    cout << "-- Angle [   rad] = " << angle.get(casa::Unit("rad"))    << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Conversion of times ..." << endl;
  try {
    casa::Quantity time (1.0,"h");
    // conversions
    cout << "-- Time [  s] = " << time.get(casa::Unit("s"))   << endl;
    cout << "-- Time [min] = " << time.get(casa::Unit("min")) << endl;
    cout << "-- Time [  d] = " << time.get(casa::Unit("d"))   << endl;
    cout << "-- Time [ yr] = " << time.get(casa::Unit("yr"))  << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test operations combining different quantities
  
  \return nofFailedTests -- The number of failed tests within this function
*/
int test_operations ()
{
  cout << "\n[test_operations]\n" << endl;

  int nofFailedTests (0);

  try {
    casa::Quantity length (100.0,"m");
    casa::Quantity time (2,"s");
    //
    casa::Quantity velocity = length/time;
    //
    cout << "-- Length   = " << length << endl;
    cout << "-- Time     = " << time   << endl;
    cout << "-- Velocity = " << velocity << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test various instances to handle a moment in time

  \return nofFailedTests -- The number of failed tests within this function
*/
int test_time ()
{
  cout << "\n[test_time]\n" << endl;
  
  int nofFailedTests (0);

  try {
    time_t rawtime;
    casa::Time systemTime;
    casa::MVTime mvTime (systemTime);
    casa::MVEpoch epoch (mvTime);
    //
    rawtime = time (NULL);
    //
    cout << "-- UNIX seconds = " << rawtime << endl;
    cout << "--> Normal time = " << ctime (&rawtime);
    cout << "--> JD          = " << rawtime/86400+2440587.5 << endl;
    cout << "-- casa::Time   = " << systemTime       << endl;
    cout << "--> JD          = " << systemTime.julianDay() << endl;
    cout << "--> MJD         = " << systemTime.modifiedJulianDay() << endl;
    cout << "-- casa::MVTime = " << mvTime << endl;
    cout << "-- MVEpoch      = " << epoch            << endl;

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

int test_MVPosition ()
{
  int nofFailedTests (0);

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Main method

  \return nofFailedTests -- The number of failed tests we have encountered
*/
int main ()
{
  int nofFailedTests (0);

  nofFailedTests += test_Quantum();
  nofFailedTests += test_Quantity();
  nofFailedTests += test_conversions();
  nofFailedTests += test_operations();
  nofFailedTests += test_time();

  // Tests for the various MeasuresValue classes
  nofFailedTests += test_MVPosition ();

  return nofFailedTests;
}
