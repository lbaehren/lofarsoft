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

/* $Id: Skymapper.cc,v 1.20 2007/04/13 13:44:29 bahren Exp $*/

#include <iostream>
#include <casa/Containers/Record.h>
#include <casa/Exceptions/Error.h>
#include <Imaging/Skymapper.h>
#include <Skymap/SkymapperTools.h>
#include <Utilities/ProgressBar.h>

namespace CR {  // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // ------------------------------------------------------------------ Skymapper
  
  Skymapper::Skymapper ()
    : verbose_p (false),
      isOperational_p (false),
      nofProcessedBlocks_p (0)
  {
    coordinates_p = SkymapCoordinates ();
    filename_p    = "skymap.img";
  }
  
  // ------------------------------------------------------------------ Skymapper
  
  Skymapper::Skymapper (SkymapCoordinates const &coordinates)
    : verbose_p (false),
      isOperational_p (false),
      nofProcessedBlocks_p (0)
  {
    coordinates_p = coordinates;
  }
  
  // ------------------------------------------------------------------ Skymapper
  
  Skymapper::Skymapper (Skymapper const& other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  Skymapper::~Skymapper ()
  {
    destroy();
  }
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  // ------------------------------------------------------------------ operator=
  
  Skymapper &Skymapper::operator= (Skymapper const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  // ----------------------------------------------------------------------- copy
  
  void Skymapper::copy (Skymapper const& other)
  {
    verbose_p            = other.verbose_p;
    filename_p           = other.filename_p;
    coordinates_p        = other.coordinates_p;
    nofProcessedBlocks_p = other.nofProcessedBlocks_p;
  }
  
  // -------------------------------------------------------------------- destroy
  
  void Skymapper::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  

  // ==========================================================================
  //
  //  Processing
  //
  // ==========================================================================

  // -------------------------------------------------------------- processData
  
  bool Skymapper::processData (Matrix<DComplex> const &data)
  {
    bool status (true);

    // Check if the Skymapper is operational first; otherwise there is no need
    // to proceed beyond this point
    if (!isOperational_p) {
      return isOperational_p;
    }

    // Local variables
    IPosition shape (data.shape());
    IPosition imageShape (coordinates_p.shape());
    IPosition start  (imageShape.nelements(),0);
    IPosition stride (imageShape.nelements(),1);
    Vector<Double> distances = coordinates_p.distanceAxisValues();
    uint nofDistances = distances.nelements();
    
    for (uint distanceStep(0); distanceStep<nofDistances; distanceStep++) {
      // 1. adjust the beamformer weights
    }

    // book-keeping of the number of data blocks processed so far
    nofProcessedBlocks_p++;

    return status;
  }
  
  // ---------------------------------------------------------------- createImage
  
  Bool Skymapper::createImage ()
  {
    std::cout << "[Skymapper::createImage]" << std::endl;
    
    Bool status     (True);
    int radius      (0);
    int integration (0);
    IPosition imageShape (coordinates_p.shape());
    int nofLoops   (imageShape(2)*imageShape(3));
    CoordinateSystem csys (coordinates_p.coordinateSystem());
    Matrix<Double> antennaPositions;
    Vector<Double> frequencies;
    Matrix<DComplex> data;
  
  /*
    Adjust the settings of the embedded Skymap object
  */
  try {
    // Extract the direction axis from the coordinate system
    DirectionCoordinate axis = coordinates_p.directionAxis();
    Vector<Double> crval (axis.referenceValue());
    Vector<Double> cdelt (axis.increment());
    IPosition shape (2,imageShape(0),imageShape(1));

    MDirection direction = axis.directionType();
    String refcode       = direction.getRefString();
    String projection    = axis.projection().name();

    crval *= 1/(C::pi/180.0);
    cdelt *= 1/(C::pi/180.0);
    
    std::cout << " - Setting up the skymap grid ... "      << std::endl;
    std::cout << " -- coord. refcode    = " << refcode     << std::endl;
    std::cout << " -- coord. projection = " << projection  << std::endl;
    std::cout << " -- shape             = " << shape       << std::endl;
    std::cout << " -- reference value   = " << crval       << std::endl;
    std::cout << " -- coord. increment  = " << cdelt       << std::endl;
//     skymap_p.setSkymapGrid (csys.obsInfo(),
// 			    refcode,
// 			    projection,
// 			    shape,
// 			    crval,
// 			    cdelt);

//     skymap_p.setCoordinateSystem(csys);

    // Elevation range
    Vector<Double> elevation (2);
    elevation(0) = 00.0;
    elevation(1) = 90.0;
//     skymap_p.setElevationRange (elevation);

    // Orientation of the sky map
    Vector<String> orientation (2);
    orientation(0) = "East";
    orientation(1) = "North";
//     skymap_p.setOrientation (orientation);

//     // Default value for the distance parameter
//     skymap_p.setDistance(-1);

    // Set up the Skymap-internal function pointer to the function handling
    // the actual data processing
//     skymap_p.setSkymapQuantity (quantity_p.skymapQuantity());
  } catch (AipsError x) {
    cerr << "[Skymapper::createImage] " << x.getMesg() << endl;
    return False;
  }
  
  /*
    Create paged image on disk, based on the information assembled to far

    The choice for "PagedImage<Double>" is problematic in the sense, that by
    this preselection we exclude computation of electric field as function of
    frequency (which has pixel data of complex type).
  */

  std::cout << " - Creating paged image on disk ... " << std::flush;
  
  TiledShape shape (imageShape);
  PagedImage<Double> image (shape,
			   csys,
			   filename_p);

  std::cout << "done." << std::endl;

  /*
    This is the core loop, generating the final skymap from a set of sub-images
  */

  uint numLoop (0);
  CR::ProgressBar bar (nofLoops);
  IPosition start  (imageShape.nelements(),0);
  IPosition stride (imageShape.nelements(),1);
  Cube<Double> pixels;
  Cube<Bool> imageMask;

  std::cout << " - computing image pixels ..." << std::endl;
  bar.update(numLoop);

  try {
    for (radius=0; radius<imageShape(2); radius++) {
      // 1. store the distance parameter
//       skymap_p.setDistance(-1.0);
      // 2. update pointer to position in image array
      start(2) = radius;
      // 3. update the beamformer weights for the current shell
//       skymap_p.setPhaseGradients (frequencies,
// 				  antennaPositions);
      for (integration=0; integration<imageShape(3); integration++) {
	start(3) = integration;
	// 1. read in the data from disk
	// 	  (this->*pFunction)(data);
	// 2. process the data 
// 	skymap_p.data2skymap(data);
	// show the progress we are making on the image generation
	numLoop++;
	bar.update(numLoop);
	// retrieve the Skymap data cube ...
// 	skymap_p.skymap (pixels,
// 			 imageMask,
// 			 Int(imageShape(4)));
	// ... and insert it at [az,el,RADIUS,INTEGRATION,freq]
	image.putSlice (pixels,start,stride);
      } // ------------------------------------------------------ end integration
      // rewind DataReader::position to initial data block
    } // ------------------------------------------------------------- end radius
    std::cout << std::endl;
    std::cout << " - all image pixels computed." << std::endl;
  } catch (AipsError x) {
    cerr << "[Skymapper::createImage] " << x.getMesg() << endl;
    return False;
  }
  
  return status;
}


// ==============================================================================
//
//  Feedback 
//
// ==============================================================================

// ---------------------------------------------------------------------- summary

void Skymapper::summary ()
{
  summary (std::cout);
}

// ---------------------------------------------------------------------- summary

void Skymapper::summary (std::ostream &os)
{
  TimeFreq timeFreq        = coordinates_p.timeFreq();
  CoordinateSystem csys    = coordinates_p.coordinateSystem();
  DirectionCoordinate axis = coordinates_p.directionAxis();
  MDirection direction     = axis.directionType();
  String refcode           = direction.getRefString();
  String projection        = axis.projection().name();
  ObsInfo obsInfo          = csys.obsInfo();

  os << "[Skymapper] Summary of the internal parameters"         << std::endl;
  os << " - Observation ..........  "                            << std::endl;
  os << " -- observer             = " << obsInfo.observer()      << std::endl;
  os << " -- telescope            = " << obsInfo.telescope()     << std::endl;
  os << " -- observation date     = " << obsInfo.obsDate()       << std::endl;
  os << " - Data I/O ............   "                            << std::endl;
  os << " -- blocksize            = " << timeFreq.blocksize()       << std::endl;
  os << " -- sampling rate [Hz]   = " << timeFreq.sampleFrequency() << std::endl;
  os << " -- Nyquist zone         = " << timeFreq.nyquistZone()     << std::endl;
  os << " - Coordinates ..........  "                            << std::endl;
  os << " -- reference code       = " << refcode                 << std::endl;
  os << " -- projection           = " << projection              << std::endl;
  os << " -- nof. coordinates     = " << csys.nCoordinates()     << std::endl;
  os << " -- names                = " << csys.worldAxisNames()   << std::endl;
  os << " -- units                = " << csys.worldAxisUnits()   << std::endl;
  os << " -- ref. pixel           = " << csys.referencePixel()   << std::endl;
  os << " -- ref. value           = " << csys.referenceValue()   << std::endl;
  os << " -- coord. increment     = " << csys.increment()        << std::endl;
  os << " - Image ................  "                            << std::endl;
  os << " -- filename             = " << filename()              << std::endl;
  os << " -- imaged quantity      = " << quantity_p.quantity()   << std::endl;
//   os << " -- image type           = " << image_p.imageType()     << std::endl;
//   os << " -- is persistent?       = " << image_p.isPersistent()  << std::endl;
//   os << " -- is paged?            = " << image_p.isPaged()       << std::endl;
//   os << " -- is writable?         = " << image_p.isWritable()    << std::endl;
//   os << " -- has pixel mask?      = " << image_p.hasPixelMask()  << std::endl;
}

}  // Namespace CR -- end
