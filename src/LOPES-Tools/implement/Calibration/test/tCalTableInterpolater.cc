/***************************************************************************
 *   Copyright (C) 2006                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

/* $Id: tCalTableInterpolater.cc,v 1.4 2006/11/10 16:53:36 horneff Exp $*/

#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <lopes/Calibration/CalTableInterpolater.h>

/*!
  \file tCalTableInterpolater.cc

  \ingroup Calibration

  \brief A collection of test routines for CalTableInterpolater
 
  \author Andreas Horneffer
 
  \date 2006/10/13
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new CalTableInterpolater object

  \return nofFailedTests -- The number of failed tests.
*/
int test_CalTableInterpolaterDComplexMat ()
{
  int nFT(0);
  
  cout << "\n[test_CalTableInterpolater]\n" << endl;

  cout << "[1] Testing default constructor ..." << endl;
  try {
    CalTableInterpolater<DComplex> interpol;
    CalTableReader reader("testTable"); 
    if (!reader.isAttached()){
      cout << "Table \"testTable\" does not exist! Run \"tCalTables\" first!" << endl;
      return 1;
    }
    nFT += !interpol.AttatchReader(&reader);
    cout << "AttatchReader: " << nFT << endl;
    nFT += !interpol.SetField("Gain");
    cout << "SetField: " << nFT << endl;
    nFT += !interpol.SetAxis("GainFreqAx");
    cout << "SetAxis: " << nFT << endl;
    nFT += !interpol.SetAxis("GainPosAx");
    cout << "SetAxis: " << nFT << endl;
    Vector<Double> Freqs(5),Pos(2);
    Array<DComplex> Gains;
    Int i;
    for (i=0; i<=5; i++) {
      Freqs(i) = 1.+i/2.;
    };
    Pos(0) = 15.;Pos(1) = 25.;
    nFT += !interpol.SetAxisValue(1,Freqs);
    cout << "SetAxisValue: " << nFT << endl;
    nFT += !interpol.SetAxisValue(2,Pos);
    cout << "SetAxisValue2: " << nFT << endl;
    nFT += !interpol.GetValues(1001,1,&Gains);
    cout << "GetValues: " << nFT << endl;
    nFT += !interpol.GetValues(1001,1,&Gains);
    cout << "Interpolated gains:" << Gains << endl;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nFT++;
  }
  
  return nFT;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_CalTableInterpolaterDComplexMat();
  }
  cout << "Tests done: " << nofFailedTests  << endl;
  return nofFailedTests;
}
