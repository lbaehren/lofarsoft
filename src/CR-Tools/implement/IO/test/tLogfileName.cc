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

/* $Id: tLogfileName.cc,v 1.2 2006/01/09 19:23:10 bahren Exp $*/

#include <templates.h>
#include <IO/LogfileName.h>

/*!
  \file tLogfileName.cc

  \brief A collection of test routines for LogfileName
 
  \author Lars Bahren
 
  \date 2006/01/09
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new LogfileName object

  \return nofFailedTests -- The number of failed tests.
*/
Int test_LogfileName ()
{
  cout << "\n[test_LogfileName]\n" << endl;

  Int nofFailedTests (0);
  String filename;

  cout << "[1] Test default constructor" << endl;
  try {
    LogfileName logfile;
    //
    filename = logfile.filename();
    cout << filename << endl;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Test argumented constructor" << endl;
  try {
    LogfileName logfile ("prefix");
    //
    filename = logfile.filename();
    cout << filename << endl;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Test argumented constructor" << endl;
  try {
    LogfileName logfile ("prefix","suffix");
    //
    filename = logfile.filename();
    cout << filename << endl;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  Int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_LogfileName ();
  }

  return nofFailedTests;
}
