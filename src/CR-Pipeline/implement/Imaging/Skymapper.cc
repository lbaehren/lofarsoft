/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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
#include <casa/Containers/Record.h>
#include <casa/Exceptions/Error.h>
#include <Imaging/Skymapper.h>
#include <Skymap/SkymapperTools.h>
#include <Utilities/ProgressBar.h>

using std::endl;

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

  bool Skymapper::init () 
  {
    std::cout << "[Skymapper::init]" << endl;

    std::cout << "--> collecting objects to create image ..." << endl;
    CoordinateSystem csys = coordinates_p.coordinateSystem();
    IPosition shape       = coordinates_p.shape();
    TiledShape tile (shape);
    
    std::cout << "--> creating the image file ... " << std::flush;
    try {
      image_p = new PagedImage<double> (tile,
					csys,
					filename_p);
      std::cout << "done" << endl;
    } catch (std::string message) {
      std::cout << endl;
      std::cerr << "--> Failed creating the image file!" << endl;
      std::cerr << "--> " << message << endl;
      isOperational_p = false;
    }

    // check the image file created in disk
    if (image_p->ok() && image_p->isWritable()) {
      std::cout << "--> Image file appears ok and is writable." << endl;
    } else {
      isOperational_p = false;
    }
      
    return isOperational_p;
  }
  
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
    uint nofDistances        = distances.nelements();
    
    for (uint distanceStep(0); distanceStep<nofDistances; distanceStep++) {
      
    }

    // book-keeping of the number of data blocks processed so far
    nofProcessedBlocks_p++;

    return status;
  }
  
  // ---------------------------------------------------------------- createImage
  
  Bool Skymapper::createImage ()
  {
    std::cout << "[Skymapper::createImage]" << endl;
    
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
    
    std::cout << " - Setting up the skymap grid ... "      << endl;
    std::cout << " -- coord. refcode    = " << refcode     << endl;
    std::cout << " -- coord. projection = " << projection  << endl;
    std::cout << " -- shape             = " << shape       << endl;
    std::cout << " -- reference value   = " << crval       << endl;
    std::cout << " -- coord. increment  = " << cdelt       << endl;

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

  std::cout << "done." << endl;

  /*
    This is the core loop, generating the final skymap from a set of sub-images
  */

  uint numLoop (0);
  CR::ProgressBar bar (nofLoops);
  IPosition start  (imageShape.nelements(),0);
  IPosition stride (imageShape.nelements(),1);
  Cube<Double> pixels;
  Cube<Bool> imageMask;

  std::cout << " - computing image pixels ..." << endl;
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
    std::cout << endl;
    std::cout << " - all image pixels computed." << endl;
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

  os << "[Skymapper] Summary of the internal parameters"            << endl;
  os << " - Observation ..........  "                               << endl;
  os << " -- observer             = " << obsInfo.observer()         << endl;
  os << " -- telescope            = " << obsInfo.telescope()        << endl;
  os << " -- observation date     = " << obsInfo.obsDate()          << endl;
  os << " - Data I/O ............   "                               << endl;
  os << " -- blocksize            = " << timeFreq.blocksize()       << endl;
  os << " -- sampling rate [Hz]   = " << timeFreq.sampleFrequency() << endl;
  os << " -- Nyquist zone         = " << timeFreq.nyquistZone()     << endl;
  os << " - Coordinates ..........  "                            << endl;
  os << " -- reference code       = " << refcode                 << endl;
  os << " -- projection           = " << projection              << endl;
  os << " -- nof. coordinates     = " << csys.nCoordinates()     << endl;
  os << " -- names                = " << csys.worldAxisNames()   << endl;
  os << " -- units                = " << csys.worldAxisUnits()   << endl;
  os << " -- ref. pixel           = " << csys.referencePixel()   << endl;
  os << " -- ref. value           = " << csys.referenceValue()   << endl;
  os << " -- coord. increment     = " << csys.increment()        << endl;
  os << " - Image ................  "                            << endl;
  os << " -- filename             = " << filename()              << endl;
  os << " -- imaged quantity      = " << quantity_p.quantity()   << endl;
//   os << " -- image type           = " << image_p.imageType()     << endl;
//   os << " -- is persistent?       = " << image_p.isPersistent()  << endl;
//   os << " -- is paged?            = " << image_p.isPaged()       << endl;
//   os << " -- is writable?         = " << image_p.isWritable()    << endl;
//   os << " -- has pixel mask?      = " << image_p.hasPixelMask()  << endl;
}

}  // Namespace CR -- end
