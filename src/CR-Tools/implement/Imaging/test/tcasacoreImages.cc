/*-------------------------------------------------------------------------*
 | $Id:: tSpatialCoordinate.cc 2579 2009-04-24 12:46:45Z baehren         $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#include <crtools.h>

// standard header files
#include <iostream>
#include <string>
#include <vector>

// casacore header files
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/Projection.h>
#include <images/Images/ImageFITSConverter.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#ifdef HAVE_HDF5
#include <images/Images/HDF5Image.h>
#endif
#include <measures/Measures/MDirection.h>

// CR-Tools header files
#include <Coordinates/CoordinateType.h>
#include <Coordinates/SpatialCoordinate.h>
#include <Utilities/MConversions.h>
#include <Utilities/TestsCommon.h>

// Namespace usage
using std::cerr;
using std::cout;
using std::endl;

/*!
  \file tcasacoreImages.cc

  \ingroup CR_Imaging

  \brief A collection of tests for the caaacore image classes and their methods
 
  \author Lars B&auml;hren
 
  \date 2009/04/27
  
  <h3>Prerequisite</h3>
  
  - casacore images base class <tt>images/Images/PagedImage.h</tt> and derived 
  implementations <tt>PagedImage</tt> and <tt>HDF5Image</tt>.
  - For the full list of reference types see <tt>measures/Measures/MDirection.h</tt>
  
  <h3>Synopsis</h3>
  
*/

//_______________________________________________________________________________
//                                                               test_export2fits

/*!
  \brief Test translation of WCS information during export to FITS

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_export2fits ()
{
  cout << "\n[tcasacoremages::test_export2fits]\n" << endl;

  int nofFailedTests (0);

  bool status (true);
  casa::String error;
  std::string outfile;
  std::vector<std::string> refcode;
  std::vector<std::string> projection;
  casa::CoordinateSystem csys;
  casa::IPosition shape (3,50,50,10);
  casa::TiledShape tile (shape);

  refcode.push_back("AZEL");      /* topocentric Azimuth and Elevation (N through E) */
  refcode.push_back("AZELSW");    /* topocentric Azimuth and Elevation (S through W) */
  refcode.push_back("AZELNE");    /* topocentric Azimuth and Elevation (N through E) */
  refcode.push_back("AZELGEO");   /* geodetic Azimuth and Elevation (N through E)    */
  refcode.push_back("AZELSWGEO"); /* geodetic Azimuth and Elevation (S through W)    */
  refcode.push_back("AZELNEGEO"); /* geodetic Azimuth and Elevation (N through E)    */
  refcode.push_back("B1950");     /* mean epoch and ecliptic at B1950.0              */
  refcode.push_back("ECLIPTIC");  /* ecliptic for J2000 equator and equinox          */
  refcode.push_back("ICRS");      /* International Celestial reference system        */
  refcode.push_back("J2000");     /* mean equator and equinox at J2000.0 (FK5)       */
  refcode.push_back("GALACTIC");  /* galactic coordinates                            */

  projection.push_back("AIR");    /* Airy                     */
  projection.push_back("AIT");    /* Hammer-Aitoff            */
  projection.push_back("CAR");    /* Cartesian                */
  projection.push_back("MER");    /* Mercator                 */
  projection.push_back("MOL");    /* Molweide                 */
  projection.push_back("SIN");    /* Orthographics/synthesis  */
  projection.push_back("STG");    /* Stereographic            */
  projection.push_back("TAN");    /* Gnomonic                 */

  for (uint ref(0); ref<refcode.size(); ref++) {
    for (uint proj(0); proj<projection.size(); proj++) {
      // report tested combination
      cout << "-- Testing " << refcode[ref] << " / " << projection[proj] << endl;
      // base name of the output file
      std::string filename = "image-" + refcode[ref] + "-" + projection[proj];
      /// create spatial coordinate 
      CR::SpatialCoordinate coord (CR::CoordinateType::DirectionRadius,
				   refcode[ref],
				   projection[proj]);
      coord.toCoordinateSystem (csys,false);
      csys.setObsInfo(CR::test_ObsInfo());
      // create PagedImage ...
      outfile = filename + ".img";
      casa::PagedImage<float> image_paged (tile,
					   csys,
					   outfile);
      // .. and export to FITS
      outfile = "!" + filename + ".fits";
      status = casa::ImageFITSConverter::ImageToFITS(error,
						     image_paged,
						     outfile);
      // create HDF5Image
#ifdef HAVE_HDF5
      outfile = filename + ".h5";
      casa::HDF5Image<float> image_hdf5 (tile,
					 csys,
					 outfile);
#endif
    }
  }

  return nofFailedTests;
}
//_______________________________________________________________________________
//                                                                           main

int main ()
{
  int nofFailedTests (0);

  nofFailedTests += test_export2fits ();

  return nofFailedTests;
}
