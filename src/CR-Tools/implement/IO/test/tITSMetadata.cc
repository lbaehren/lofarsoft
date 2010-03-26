/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

/* $Id$*/

#include <IO/ITSMetadata.h>

/*!
  \file tITSMetadata.cc

  \ingroup CR
  \ingroup IO
  
  \brief A collection of test routines for ITSMetadata
 
  \author Lars B&auml;hren
 
  \date 2006/05/17

  <h3>Usage</h3>

  The name of the metafile, on which the test program is run, can be provided
  as command-line parameter, e.g.
  \verbatim
  tITSMetadata /data/ITS/2004.08/2004.08.24-14:28:29.00-Thunderstorm/experiment.meta
  tITSMetadata /data/ITS/2004.11/2004.11.02-11:30:14.04-24h/experiment.meta
  tITSMetadata /data/ITS/2005.03/monitoring_20050304_001/experiment.meta
  \endverbatim
  If no metafile is specified at program call, the example ITS data set is used 
  (<tt>/data/ITS/exampledata.its/experiment.meta</tt>).
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new tITSMetadata object

  \return nofFailedTests -- The number of failed tests.
*/
Int test_ITSMetadata (const String& metafile)
{
  cout << "\n[test_ITSMetadata]\n" << endl;

  Int nofFailedTests (0);

  // Test for the default constructor
  cout << "[1] Test for the default constructor" << endl;
  try {
    ITSMetadata meta;
    //
    meta.summary();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  // Test for argumented constructor (correct path)
//   cout << "[2] Test for argumented constructor (correct path)" << endl;
//   try {
//     ITSMetadata meta (metafile);
//     //
//     meta.summary();
//   } catch (AipsError x) {
//     cerr << x.getMesg() << endl;
//     nofFailedTests++;
//   }
  
  // Test for argumented constructor (incorrect path)
  cout << "[3] Test for argumented constructor (incorrect path)" << endl;
  try {
    String newMetafile (metafile+"log");
    ITSMetadata meta (newMetafile);
    //
    meta.summary();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  // Test for copy constructor
  cout << "[4] Test for copy constructor" << endl;
  try {
    ITSMetadata meta1 (metafile);
    ITSMetadata meta2 (meta1);
    //
    cout << "+++ original object +++" << endl;
    meta1.summary();
    cout << "+++ object obtained via copy ooperation +++" << endl;
    meta2.summary();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

Int test_metafile (const String& metafile)
{
  cout << "\n[test_metafile]\n" << endl;

  Int nofFailedTests (0);

  // Test for the default constructor
  cout << "[1] Assign filename after using default constructor" << endl;
  try {
    ITSMetadata meta;
    //
    meta.setMetafile (metafile);
    //
    meta.summary();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                           main

int main (int argc,
          char *argv[])
{
  int nofFailedTests (0);
  bool haveDataset (true);
  std::string filename;

  //________________________________________________________
  // Process parameters from the command line
  
  if (argc < 2) {
    haveDataset = false;
  } else {
    filename    = argv[1];
    haveDataset = true;
  }

  //________________________________________________________
  // Run the tests

  if (haveDataset) {
    nofFailedTests += test_ITSMetadata (filename);
    nofFailedTests += test_metafile (filename);
  } else {
    nofFailedTests = 1;
  }

  return nofFailedTests;
}
