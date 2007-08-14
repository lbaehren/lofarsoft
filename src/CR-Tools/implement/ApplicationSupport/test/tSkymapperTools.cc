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

/* $Id: tSkymapperTools.cc,v 1.4 2006/08/02 14:08:44 bahren Exp $*/

#include <casa/aips.h>
#include <casa/System/Aipsrc.h>
#include <casa/Exceptions/Error.h>
#include <images/Images/PagedImage.h>

#include <templates.h>
#include <ApplicationSupport/SkymapperTools.h>

/*!
  \file tSkymapperTools.cc

  \ingroup ApplicationSupport

  \brief A collection of test routines for SkymapperTools
 
  \author Lars B&auml;hren
 
  \date 2006/07/25
*/

/*!
  \verbatim
  - data.data[1].Header.Date
  2006/03/07/16:55:47.000 
  - data.head('Date') 
  2006/03/07/16:55:47.000 
  - qnt.quantity(data.head('Date'))
  [value=53801.7054, unit=d] 
  \endverbatim
*/
int test_obsinfo ()
{
  std::cout << "\n[test_obsinfo]" << std::endl;

  int nofFailedTests (0);

  cout << "\n[1] Conversion of instances of time (Time -> MVEpoch -> MEpoch) ..."
       << endl;
  {
    Time obsTime;
    MVTime mvtime (obsTime);
    Quantity qtime (mvtime.get());
    MVEpoch mvepoch (obsTime.modifiedJulianDay());
    MEpoch mepoch (mvepoch);
    
    cout << " - Time                = " << obsTime                     << endl;
    cout << " - modified Julian day = " << obsTime.modifiedJulianDay() << endl;
    cout << " - MVTime              = " << mvtime                      << endl;
    cout << " - Quantity            = " << qtime                       << endl;
    cout << " - MVEpoch             = " << mvepoch                     << endl;
    cout << " - MEpoch              = " << mepoch                      << endl;
  }

  cout << "\n[2] Conversion of instances of time (Time -> Quantity -> MEpoch) ..."
       << endl;
  {
    Time obsTime;
    MVTime mvtime (obsTime);
    Quantity qtime (mvtime.get());
    MEpoch mepoch (qtime);

    cout << " - Time                = " << obsTime                     << endl;
    cout << " - MVTime              = " << mvtime                      << endl;
    cout << " - Quantity            = " << qtime                       << endl;
    cout << " - MEpoch              = " << mepoch                      << endl;
  }

  cout << "\n[3] Assigning ObsInfo from parameters ..." << endl;
  {
    Time obsTime;
    MVTime mvtime (obsTime);
    Quantity qtime (mvtime.get());
    MEpoch mepoch (qtime);
    String telescope ("LOFAR");
    String observer ("LOFAR/CR KSP");

    ObsInfo obsinfo1 = SkymapperTools::obsinfo (obsTime, telescope, observer);
    ObsInfo obsinfo2 = SkymapperTools::obsinfo (qtime,   telescope, observer);
    ObsInfo obsinfo3 = SkymapperTools::obsinfo (mepoch,  telescope, observer);

    cout << " - Obs. date from Time     = " << obsinfo1.obsDate() << endl;
    cout << " - Obs. date from Quantity = " << obsinfo2.obsDate() << endl;
    cout << " - Obs. date from MEpoch   = " << obsinfo3.obsDate() << endl;
  }
    
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int test_coordinateSystem ()
{
  std::cout << "\n[test_coordinateSystem]" << std::endl;

  int nofFailedTests (0);

  String filename (Aipsrc::aipsRoot());

  filename += "/data/demo/Images/test_image";

  cout << "\n[0] Default constructor for CoordinateSystem ..." << endl;
  {
    CoordinateSystem csys;
    cout << " - image file           = " << filename       << endl;
    cout << " - observation info     = " << csys.obsInfo() << endl;
    cout << " - nof. coordinate axes = " << csys.nCoordinates() << endl;
  }
    
  cout << "\n[1] Extracting coordinate system from image file ..." << endl;
  {
    CoordinateSystem csys = SkymapperTools::coordinateSystem (filename);
    cout << " - image file           = " << filename       << endl;
    cout << " - observation info     = " << csys.obsInfo() << endl;
    cout << " - nof. coordinate axes = " << csys.nCoordinates() << endl;
  }
    
  cout << "\n[2] Extracting coordinate system from paged image ..." << endl;
  {
    PagedImage<Float> myimage (filename);
    CoordinateSystem csys = SkymapperTools::coordinateSystem (myimage);
    cout << " - observation info     = " << csys.obsInfo() << endl;
    cout << " - nof. coordinate axes = " << csys.nCoordinates() << endl;
  }

  cout << "\n[3] Extracting coordinate system, adjusting ObsInfo ..." << endl;
  {
    Time obsTime;
    String telescope ("LOFAR");
    String observer ("LOFAR/CR KSP");
    ObsInfo obsinfo (SkymapperTools::obsinfo (obsTime, telescope, observer));

    CoordinateSystem csys = SkymapperTools::coordinateSystem (filename,
							      obsinfo,
							      True);

    cout << " - image file           = " << filename       << endl;
    cout << " - observation info     = " << csys.obsInfo() << endl;
    cout << " - nof. coordinate axes = " << csys.nCoordinates() << endl;
  }

  cout << "\n[4] Conversion of reference codes to types ..." << endl;
  try {
    String projection ("AIT");
    String direction  ("J2000");

    cout << " - projection : "
	 << projection
	 << "\t= "
	 << SkymapperTools::ProjectionType (projection) << endl;
    cout << " - direction  : "
	 << direction
	 << "\t= "
	 << SkymapperTools::MDirectionType (direction) << endl;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
 
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  {
    nofFailedTests += test_obsinfo ();
  }

  {
    nofFailedTests += test_coordinateSystem ();
  }

  return nofFailedTests;
}
