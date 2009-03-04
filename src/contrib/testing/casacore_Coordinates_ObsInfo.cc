/*-------------------------------------------------------------------------*
 | $Id:: tSkymapper.cc 2252 2009-01-22 16:34:17Z baehren                 $ |
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

/*!
  \file casacore_Coordinates_ObsInfo.cc
  
  \ingroup contrib
  
  \brief A number of tests and exercises for working with casa::ObsInfo objects
  
  \author Lars B&auml;hren

  \date 04 Mar 2009
*/

#include <iostream>
#include <string>

#include <coordinates/Coordinates/ObsInfo.h>

/* Namespace usage */
using std::cout;
using std::endl;
using casa::ObsInfo;

//_______________________________________________________________________________
//                                                                   show_ObsInfo

void show_ObsInfo (ObsInfo const &obs)
{
  cout << "[ObsInfo] Summary of object" << endl;
  /* Object's internal settings */
  cout << "-- Telescope        = " << obs.telescope()      << endl;
  cout << "-- Observer         = " << obs.observer()       << endl;
  cout << "-- Observation date = " << obs.obsDate()        << endl;
  cout << "-- Pointing center  = " << obs.pointingCenter() << endl;
}

//_______________________________________________________________________________
//                                                                           main

int main ()
{
  int nofFailedTests (0);

  cout << "[1] Testing static methods of ObsInfo ..." << endl;
  try {
    cout << "-- Default telescope = " << ObsInfo::defaultTelescope() << endl;
    cout << "-- Default observer  = " << ObsInfo::defaultObserver()  << endl;
    cout << "-- Default obs. date = " << ObsInfo::defaultObsDate()   << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Default constructor of ObsInfo object ..." << endl;
  try {
    ObsInfo obs;
    show_ObsInfo (obs);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Set name of telescope ..." << endl;
  try {
    ObsInfo obs;
    //
    obs.setTelescope ("VLA");
    show_ObsInfo (obs);
    //
    obs.setTelescope ("WSRT");
    show_ObsInfo (obs);
    //
    obs.setTelescope ("LOFAR");
    show_ObsInfo (obs);
    //
    obs.setTelescope ("LOPES");
    show_ObsInfo (obs);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}
