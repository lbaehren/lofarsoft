/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars Bahren (bahren@astron.nl)                                        *
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
/* $Id: tDataBlockSettings.cc,v 1.4 2005/08/05 13:46:28 bahren Exp $*/

#include <IO/DataBlockSettings.h>

/*!
  \file tDataBlockSettings.cc

  \brief A collection of test routines for DataBlockSettings
 
  \author Lars B&auml;hren
 
  \date 2005/07/28
*/

using CR::DataBlockSettings;

// -----------------------------------------------------------------------------

void show_DataBlockSettings (DataBlockSettings settings)
{
  cout << " - Filesize ........ : " << settings.filesize() << endl;
  cout << " - Adjustment method : " << settings.methodName()
       << " (" << settings.methodType() << ")" << endl;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new DataBlockSettings object

  \param filesize -- Total size [samples] of the data file, for which the check
                     is performed.
*/
int test_DataBlockSettings (const int& filesize)
{
  int nofFailedTests (0);

  cout << "\n[tDataBlockSettings] Testing constructors ..." << endl;

  cout << "\n [1] DataBlockSettings()" << endl;
  {
    DataBlockSettings settings;
    //
    show_DataBlockSettings (settings);
  }
  
  cout << "\n [2] DataBlockSettings(int)" << endl;
  {
    DataBlockSettings settings (filesize);
    //
    show_DataBlockSettings (settings);
  }
  
  cout << "\n [3] DataBlockSettings(int)" << endl;
  {
    DataBlockSettings settings (filesize, DataBlockSettings::BLOCKS);
    //
    show_DataBlockSettings (settings);
  }
  
  cout << "\n [4] DataBlockSettings(int)" << endl;
  {
    DataBlockSettings settings (filesize, DataBlockSettings::SAMPLES);
    //
    show_DataBlockSettings (settings);
  }
  
  cout << "\n [5] DataBlockSettings(int)" << endl;
  {
    DataBlockSettings::method whichMethod = DataBlockSettings::SAMPLES;
    DataBlockSettings settings (filesize, whichMethod);
    //
    show_DataBlockSettings (settings);
  }
  
  return nofFailedTests;
}

// --- Test check of block settings --------------------------------------------

/*!
  \brief Test check of block settings

  \param filesize -- Total size [samples] of the data file, for which the check
                     is performed.
*/
int test_check (const int filesize)
{
  int nofFailedTests (0);

  int nofInterations (10);
  int nofBlocks (50);
  int nofSamples (65536);
  int offset (1048576);

  cout << "\n[tDataBlockSettings] Testing check of block settings ..." << endl;

  cout << "\n [1] Zero offset (ITERATIONS)" << endl;
  {
    int nInterations (nofInterations);
    int nBlocks (nofBlocks);
    int nSamples (nofSamples);
    //
    DataBlockSettings settings (filesize);
    settings.check (nInterations,nBlocks,nSamples);
    //
    cout << " - Input parameters   : \t" << nofInterations << "\t" << nofBlocks
	 << "\t" << nSamples << endl;
    cout << " - Checked parameters : \t" << nInterations << "\t" << nBlocks
	 << "\t" << nSamples << endl;
  }

  cout << "\n [2] Non-zero offset (ITERATIONS)" << endl;
  {
    int nInterations (nofInterations);
    int nBlocks (nofBlocks);
    int nSamples (nofSamples);
    //
    DataBlockSettings settings (filesize);
    settings.check (nInterations,nBlocks,nSamples,offset);
    //
    cout << " - Input parameters   : \t" << nofInterations << "\t" << nofBlocks
	 << "\t" << nSamples << endl;
    cout << " - Checked parameters : \t" << nInterations << "\t" << nBlocks
	 << "\t" << nSamples << endl;
  }

  cout << "\n [3] Non-zero offset (BLOCKS)" << endl;
  {
    int nInterations (nofInterations);
    int nBlocks (nofBlocks);
    int nSamples (nofSamples);
    //
    DataBlockSettings settings (filesize,DataBlockSettings::BLOCKS);
    settings.check (nInterations,nBlocks,nSamples,offset);
    //
    cout << " - Input parameters   : \t" << nofInterations << "\t" << nofBlocks
	 << "\t" << nSamples << endl;
    cout << " - Checked parameters : \t" << nInterations << "\t" << nBlocks
	 << "\t" << nSamples << endl;
  }

  cout << "\n [3] Non-zero offset (SAMPLES)" << endl;
  {
    int nInterations (nofInterations);
    int nBlocks (nofBlocks);
    int nSamples (nofSamples);
    //
    DataBlockSettings settings (filesize,DataBlockSettings::SAMPLES);
    settings.check (nInterations,nBlocks,nSamples,offset);
    //
    cout << " - Input parameters   : \t" << nofInterations << "\t" << nofBlocks
	 << "\t" << nSamples << endl;
    cout << " - Checked parameters : \t" << nInterations << "\t" << nBlocks
	 << "\t" << nSamples << endl;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);
  // Filesize taken from an typical LOFAR ITS data set (32 MSamples)
  int filesize (33554132);

  // Test for the constructor(s)
  {
    nofFailedTests += test_DataBlockSettings (filesize);
  }

  // Test check of block settings
  {
    nofFailedTests += test_check (filesize);
  }

  return nofFailedTests;
}
