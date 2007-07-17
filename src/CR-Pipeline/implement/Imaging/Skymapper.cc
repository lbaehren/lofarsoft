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

using std::cout;
using std::endl;

namespace CR {  // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // ------------------------------------------------------------------ Skymapper
  
  Skymapper::Skymapper ()
  {
    init (0,
	  false,
	  0,
	  "skymap.img",
	  SkymapCoordinates(),
	  Beamformer());
  }
  
  // ------------------------------------------------------------------ Skymapper
  
  Skymapper::Skymapper (SkymapCoordinates const &coordinates)
  {
    init (0,
	  false,
	  0,
	  "skymap.img",
	  coordinates,
	  Beamformer());
  }
  
  // ------------------------------------------------------------------ Skymapper
  
  Skymapper::Skymapper (SkymapCoordinates const &coordinates,
			Beamformer const &beamformer)
  {
    init (0,
	  false,
	  0,
	  "skymap.img",
	  coordinates,
	  beamformer);
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
    beamformer_p         = other.beamformer_p;
    coordinates_p        = other.coordinates_p;
    filename_p           = other.filename_p;
    nofProcessedBlocks_p = other.nofProcessedBlocks_p;
    quantity_p           = other.quantity_p;
    verbose_p            = other.verbose_p;
  }
  
  // -------------------------------------------------------------------- destroy
  
  void Skymapper::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  // ----------------------------------------------------------------------- init 
  
  void Skymapper::init (short const &verbose,
			bool const &isOperational,
			uint const &nofProcessedBlocks,
			std::string const &filename,
			SkymapCoordinates const &coordinates,
			Beamformer const &beamformer)
  {
    verbose_p            = verbose;
    isOperational_p      = isOperational;
    nofProcessedBlocks_p = nofProcessedBlocks;
    filename_p           = filename;
    
    if (!setBeamformer (beamformer)) {
      std::cerr << "[Skymapper::init] Error setting Beamformer object!"
		<< endl;
    }

    if (!setSkymapCoordinates (coordinates)) {
      std::cerr << "[Skymapper::init] Error setting SkymapCoordinates object!"
		<< endl;
    }

  }

  // -------------------------------------------------------------- setBeamformer

  bool Skymapper::setBeamformer (Beamformer const &beamformer)
  {
    bool status (true);

    try {
      beamformer_p = beamformer;
    } catch (std::string message) {
      std::cerr << "[Skymapper::setBeamformer] Error setting Beamformer object!"
		<< endl;
      std::cerr << "--> " << message << endl;
    }
    
    return status;
  }

  // -------------------------------------------------------------- setBeamformer

  bool Skymapper::setBeamformer (Matrix<double> const &antPositions,
				 Matrix<double> const &skyPositions,
				 Vector<double> const &frequencies)
  {
    bool status (true);
    
    status = beamformer_p.setAntPositions (antPositions);
    status = beamformer_p.setSkyPositions (skyPositions);
    status = beamformer_p.setFrequencies (frequencies);
    
    return status;
  }

  // ------------------------------------------------------- setSkymapCoordinates

  bool Skymapper::setSkymapCoordinates (SkymapCoordinates const &coordinates)
  {
    bool status (true);
    
    try {
      coordinates_p = coordinates;
    } catch (std::string message) {
      std::cerr << "[Skymapper::setSkymapCoordinates] "
		<< "Error setting setSkymapCoordinates object!"
		<< endl;
      std::cerr << "--> " << message << endl;
    }
    
    return status;
  }

  // ============================================================================
  //
  //  Processing
  //
  // ============================================================================

  // ----------------------------------------------------------------------- init

  bool Skymapper::init () 
  {
    bool status (true);

    /*
      For the later Beamforming we need to retrieve the coordinates of the
      poiting positions; since we keep the directions constant and iterate over
      the distance axis, we get the full position information by later
      combination.
    */
    
    {
      // Retrieve the values of the direction axes
      Matrix<double> directionValues;
      status = coordinates_p.directionAxisValues ("AZEL",
						  directionValues,
						  directionMask_p,
						  false);
      // Retrieve the values of the distance axis
      Vector<double> distances (coordinates_p.distanceAxisValues());
      // Retrieve the values of the frequency axis
      Vector<double> frequencies (coordinates_p.frequencyAxisValues());
      // Combine the values from the axes to yield the 3D positions
      bool anglesInDegrees (true);
      bool bufferDelays (false);
      bool bufferPhases (false);
      Vector<int> axisOrder (3);
      casa::indgen(axisOrder);
      status = beamformer_p.setSkyPositions(directionValues,
					    distances,
					    axisOrder,
					    CR::Spherical,
					    anglesInDegrees,
					    bufferDelays);
      status = beamformer_p.setFrequencies (frequencies,
					    bufferPhases);
      if (status && verbose_p) {
	beamformer_p.summary();
      }
    }
    
    /*
      With the image data written into an AIPS++ PagedImage, we need to create
      and initialize one first, before we can start inserting the computed 
      image data.
    */
    CoordinateSystem csys = coordinates_p.coordinateSystem();
    IPosition shape       = coordinates_p.shape();
    TiledShape tile (shape);

    try {
      image_p = new PagedImage<double> (tile,
					csys,
					filename_p);
    } catch (std::string message) {
      std::cerr << "[Skymapper::init] Failed creating the image file!" << endl;
      std::cerr << "--> " << message << endl;
      isOperational_p = false;
    }

    // check the image file created in disk
    if (image_p->ok() && image_p->isWritable()) {
      // book-keeping of operational status
      isOperational_p = true;
      // feedback to the outside world
      if (verbose_p) {
	cout << "[Skymapper::init] Image file appears ok and is writable."
		  << endl;
      }
    } else {
      isOperational_p = false;
    }
    
    return isOperational_p;
  }
  
  // ---------------------------------------------------------------- processData
  
  bool Skymapper::processData (Matrix<DComplex> const &data)
  {
    bool status (true);

    // Check if the Skymapper is operational first; otherwise there is no need
    // to proceed beyond this point
    if (!isOperational_p) {
      std::cerr << "[Skymapper::processData] Skymapper is not operational!"
		<< endl;
      return isOperational_p;
    }

    // forward the data to the beamformer for processing
    Matrix<double> beam;
    status = beamformer_p.processData (beam,
				       data);

    /*
      Inserted the computed pixel values into the image.
    */
//     if (status) {
      // Declare additional variables
      int timeAxisStride (coordinates_p.timeAxisStride());
      IPosition imageShape (coordinates_p.shape());
      IPosition imageStart  (imageShape.nelements(),0);
      IPosition imageStride (imageShape.nelements(),1);
      IPosition beamShape (beam.shape());
      IPosition beamStart  (beamShape.nelements(),0);
      IPosition beamStride (beamShape.nelements(),1);

      // adjust objects for array slicing
      imageStart(3)  = timeAxisStride*nofProcessedBlocks_p;

      // Provide some feedback
      cout << "[Skymapper::processData]" << endl;
      cout << "-- shape of the input data  = " << data.shape()   << endl;
      cout << "-- Shape of beamformed data = " << beam.shape()   << endl;
      cout << "-- shape of the image       = " << imageShape     << endl;
      cout << "-- Stride on the time axis  = " << timeAxisStride << endl;
      cout << "-- image array : start      = " << imageStart     << endl;
      cout << "-- image array : stride     = " << imageStride    << endl;
      cout << "-- beam array : start       = " << beamStart      << endl;
      cout << "-- beam array : stride      = " << beamStride     << endl;
      
      for (int dist(0); dist<imageShape(SkymapCoordinates::Distance); dist++) {
	//       image_p.putSlice (pixels,imageStart,imageStride);
      }
      
//     }
    
    /*
      Book-keeping of the number of processed blocks so far; this is accounted
      for independent of whether the beamforming stage has been successful or
      not, in order to keep navigation within the image pixel array consistent.
    */
    nofProcessedBlocks_p++;

    return status;
  }
  
  // ---------------------------------------------------------------- createImage
  
  Bool Skymapper::createImage ()
  {
    cout << "[Skymapper::createImage]" << endl;
    
    Bool status     (True);
    int radius      (0);
    int integration (0);
    IPosition imageShape (coordinates_p.shape());
    int nofLoops   (imageShape(2)*imageShape(3));
    CoordinateSystem csys (coordinates_p.coordinateSystem());
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
    
    cout << " - Setting up the skymap grid ... "      << endl;
    cout << " -- coord. refcode    = " << refcode     << endl;
    cout << " -- coord. projection = " << projection  << endl;
    cout << " -- shape             = " << shape       << endl;
    cout << " -- reference value   = " << crval       << endl;
    cout << " -- coord. increment  = " << cdelt       << endl;

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

  cout << " - Creating paged image on disk ... " << std::flush;
  
  TiledShape shape (imageShape);
  PagedImage<Double> image (shape,
			   csys,
			   filename_p);

  cout << "done." << endl;

  /*
    This is the core loop, generating the final skymap from a set of sub-images
  */

  uint numLoop (0);
  CR::ProgressBar bar (nofLoops);
  IPosition start  (imageShape.nelements(),0);
  IPosition stride (imageShape.nelements(),1);
  Cube<Double> pixels;
  Cube<Bool> imageMask;

  cout << " - computing image pixels ..." << endl;
  bar.update(numLoop);

  try {
    for (radius=0; radius<imageShape(2); radius++) {
      // 1. store the distance parameter
//       skymap_p.setDistance(-1.0);
      // 2. update pointer to position in image array
      start(2) = radius;
      // 3. update the beamformer weights for the current shell
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
    cout << endl;
    cout << " - all image pixels computed." << endl;
  } catch (AipsError x) {
    cerr << "[Skymapper::createImage] " << x.getMesg() << endl;
    return False;
  }
  
  return status;
  }
  
  
  // ============================================================================
  //
  //  Feedback 
  //
  // ============================================================================
  
  // -------------------------------------------------------------------- summary
  
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
    os << " - Coordinates ..........  "                               << endl;
    os << " -- reference code       = " << refcode                    << endl;
    os << " -- projection           = " << projection                 << endl;
    os << " -- nof. coordinates     = " << csys.nCoordinates()        << endl;
    os << " -- names                = " << csys.worldAxisNames()      << endl;
    os << " -- units                = " << csys.worldAxisUnits()      << endl;
    os << " -- ref. pixel           = " << csys.referencePixel()      << endl;
    os << " -- ref. value           = " << csys.referenceValue()      << endl;
    os << " -- coord. increment     = " << csys.increment()           << endl;
    os << " - Image ................  "                               << endl;
    os << " -- filename             = " << filename()                 << endl;
    os << " -- imaged quantity      = " << quantity_p.quantity()      << endl;
    os << " -- shape of pixel array = " << coordinates_p.shape()      << endl;
    //   os << " -- image type           = " << image_p.imageType()     << endl;
    //   os << " -- is persistent?       = " << image_p.isPersistent()  << endl;
    //   os << " -- is paged?            = " << image_p.isPaged()       << endl;
    //   os << " -- is writable?         = " << image_p.isWritable()    << endl;
    //   os << " -- has pixel mask?      = " << image_p.hasPixelMask()  << endl;
  }
  
}  // Namespace CR -- end
