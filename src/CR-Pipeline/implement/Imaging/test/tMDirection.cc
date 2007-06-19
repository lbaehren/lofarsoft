/***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#include <iostream>

#include <templates.h>
#include <Imaging/SkymapCoordinates.h>

/*!
  \file tMDirection.cc
  
  \ingroup CR
  \ingroup Imaging
  
  \brief A number of tests to work with the CASA MDirection classes
  
  \author Lars B&auml;hren
  
  \date 2007/05/30
  
  Besides some basic testing of the casa::MDirection class, we also test the
  functionality as available with casa::DirectionCoordinate and
  CR::SkymapCoordinates, as both make use of the functionality of MDirection.
  
  <h3>Prerequisite</h3>
  
  <ul type="square">
  <li><a href="http://casa.nrao.edu/docs/doxygen/group__Coordinates.html">CASA
      coordinates module</a>
  </ul>
*/

// -----------------------------------------------------------------------------

/*!
  \brief Show the contents of an MDirection object

  \param md -- MDirection object
*/
void show_MDirection (MDirection const &md)
{
  bool status(true);
  String refcode;
  MDirection::Types type;

  // retrieve the information from the object
  refcode = md.getRefString();
  status  = md.getType(type,refcode);

  cout << "-- Reference string = " << refcode << " (" << type << ")" << endl;
  cout << "-- MDirection type  = " << md.showType(type) << endl;
  cout << "-- Reference type   = " << md.type()         << endl;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the various ways in which to create an MDirection object

  \return nofFailedTests -- The number of failed tests.
*/
int test_MDirection ()
{
  cout << "\n[test_MDirection]\n" << endl;

  int nofFailedTests (0);

  cout << "[1] Default constructor..." << endl;
  try {
    MDirection md;
    show_MDirection (md);
  } catch (std::string err) {
    cerr << err << endl;
    nofFailedTests++;
  }

  cout << "[2] Argumented constructor..." << endl;
  try {
    MDirection md (MDirection::AZEL);
    show_MDirection (md);
  } catch (std::string err) {
    cerr << err << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

/*!
  \brief Creation and manipulation of casa::MDirection from CR::SkymapCoordinates
  
  \return nofFailedTests -- The number of failed tests.
*/
int test_SkymapCoordinates ()
{
  cout << "\n[test_SkymapCoordinates]\n" << endl;

  int nofFailedTests (0);

  // -----------------------------------------------------------------
  // [1] Assemble SkymapCoordinates object as used for the Skymapper

  uint blocksize (512);
  double sampleFrequency (40e6);
  uint nyquistZone (1);
  CR::TimeFreq timeFreq (blocksize,
			 sampleFrequency,
			 nyquistZone);
  CR::ObservationData obsData ("LOFAR-ITS",
			       "Lars Baehren");
  CR::SkymapCoordinates coord (timeFreq,
			       obsData);

  // -----------------------------------------------------------------
  // [2] Retrieve the coordinate system constructed for the image

  casa::CoordinateSystem csys = coord.coordinateSystem();

  {
    cout << "--> Summary of the coordinate system:" << endl;
    cout << "-- Number of coordinates    = " << csys.nCoordinates()   << endl;
    cout << "-- World axis names         = " << csys.worldAxisNames() << endl;
    cout << "-- World axis units         = " << csys.worldAxisUnits() << endl;
    cout << "-- Reference pixel  (CRPIX) = " << csys.referencePixel() << endl;
    cout << "-- Reference value  (CRVAL) = " << csys.referenceValue() << endl;
    cout << "-- Increment        (CDELT) = " << csys.increment()      << endl;
  }

  try {
    int nofSteps (10);
    Vector<double> pixel (5);
    Vector<double> world (5);

    pixel = 0.0;

    csys.toWorld(world,pixel);
    cout << "-- Coordinate conversion : " << endl;

    for (int n(0); n<nofSteps; n++) {
      pixel(0) = double(n);
      csys.toWorld(world,pixel);
      cout << "\t" << pixel << " -> " << world << endl;
    }
  } catch (std::string err) {
    cerr << err << endl;
    nofFailedTests++;
  }
    
  // -----------------------------------------------------------------
  // [3] Retrieve the coordinate handling the direction axes

  casa::DirectionCoordinate dc (coord.directionAxis());
  
  {
    Vector<Double> crpix = dc.referencePixel();
    Vector<Double> crval = dc.referenceValue();
    Vector<Double> cdelt = dc.increment();
    Matrix<Double> pc    = dc.linearTransform();
    Projection proj      = dc.projection();
    MDirection md (dc.directionType());
   
    cout << "--> Summary of the direction coordinate:" << endl;
    cout << "-- Direction type   = " << md.getRefString() << endl;
    cout << "-- Projection       = " << proj.name()    << endl;
    cout << "-- Reference pixel  = " << crpix          << endl;
    cout << "-- Reference value  = " << crval          << endl;
    cout << "-- Increment        = " << cdelt          << endl;
    cout << "-- Linear transform = " << pc.row(0) << ", " << pc.row(1) << endl;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main () 
{
  int nofFailedTests (0);

  nofFailedTests += test_MDirection();
  nofFailedTests += test_SkymapCoordinates();

  return nofFailedTests;
}
