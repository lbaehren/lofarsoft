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
// #include <Skymap/SkymapperTools.h>
#include <Utilities/ProgressBar.h>

using std::cerr;
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
	  SkymapCoordinate(),
	  Beamformer());
  }
  
  // ------------------------------------------------------------------ Skymapper
  
  Skymapper::Skymapper (SkymapCoordinate const &skymapCoord)
  {
    init (0,
	  false,
	  0,
	  "skymap.img",
	  skymapCoord,
	  Beamformer());
  }
  
  // ------------------------------------------------------------------ Skymapper
  
  Skymapper::Skymapper (SkymapCoordinate const &skymapCoord,
			Matrix<double> const &antPositions)
  {
    Beamformer beamformer;
    beamformer.setAntennaPositions (antPositions);
    //
    init (0,
	  false,
	  0,
	  "skymap.img",
	  skymapCoord,
	  beamformer);
  }
  
  // ------------------------------------------------------------------ Skymapper
  
  Skymapper::Skymapper (SkymapCoordinate const &skymapCoord,
			Beamformer const &beamformer)
  {
    init (0,
	  false,
	  0,
	  "skymap.img",
	  skymapCoord,
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
			SkymapCoordinate const &skymapCoord,
			Beamformer const &beamformer)
  {
    // Store atomic parameters

    verbose_p            = verbose;
    isOperational_p      = isOperational;
    nofProcessedBlocks_p = nofProcessedBlocks;
    filename_p           = filename;

    // Store embedded objects
    
    if (!setBeamformer (beamformer)) {
      cerr << "[Skymapper::init] Error setting Beamformer object!"
	   << endl;
    }
    
    if (!setSkymapCoordinate (skymapCoord)) {
      cerr << "[Skymapper::init] Error setting SkymapCoordinate object!"
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
      cerr << "[Skymapper::setBeamformer] Error setting Beamformer object!"
	   << endl;
      cerr << "--> " << message << endl;
    }
    
    return status;
  }
  
  // -------------------------------------------------------------- setBeamformer
  
  bool Skymapper::setAntennaPositions (Matrix<double> const &antPositions)
  {
    return beamformer_p.setAntennaPositions (antPositions);
  }

  // ------------------------------------------------------- setSkymapCoordinates

  bool Skymapper::setSkymapCoordinate (SkymapCoordinate const &skymapCoord)
  {
    bool status (true);
    
    try {
      coordinates_p = skymapCoord;
    } catch (std::string message) {
      cerr << "[Skymapper::setSkymapCoordinate] "
		<< "Error setting setSkymapCoordinate object!"
		<< endl;
      cerr << "--> " << message << endl;
    }
    
    return status;
  }

  // ============================================================================
  //
  //  Processing
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                               timeAxisStride

  int Skymapper::timeAxisStride ()
  {
    int stride (1);
    
    switch (quantity_p.type()) {
    case SkymapQuantity::TIME_FIELD:
    case SkymapQuantity::TIME_POWER:
    case SkymapQuantity::TIME_CC:
    case SkymapQuantity::TIME_P:
    case SkymapQuantity::TIME_X:
      stride = coordinates_p.timeFreqCoordinate().blocksize();
      break;
    case SkymapQuantity::FREQ_POWER:
    case SkymapQuantity::FREQ_FIELD:
      stride = 1;
      break;
    }

    return stride;
  }
  
  // -------------------------------------------------------------- initSkymapper

  bool Skymapper::initSkymapper () 
  {
    bool status (true);

    /*
      For the later Beamforming we need to retrieve the coordinates of the
      pointing positions; since we keep the directions constant and iterate over
      the distance axis, we get the full position information by later
      combination.
    */
    try {
      // Retrieve the values of the direction axes
      Matrix<double> skyPositions = coordinates_p.spatialCoordinate().worldAxisValues();

      // Combine the values from the axes to yield the 3D positions
      bool anglesInDegrees (true);
      bool bufferDelays (false);
      Vector<int> axisOrder (3);
      casa::indgen(axisOrder);
//       status = beamformer_p.setSkyPositions(skyPositions,
// 					    axisOrder,
// 					    CR::CoordinateType::Spherical,
// 					    anglesInDegrees,
// 					    bufferDelays);
    } catch (std::string message) {
      cerr << "[Skymapper::initSkymapper] Failed assigning beam directions!"
	   << endl;
      cerr << message << endl;
    }

    /*
      In order to set the beamforming weights we require the frequency values
    */
    try {
      Vector<double> frequencies (coordinates_p.timeFreqCoordinate().timeValues());
      status = beamformer_p.setFrequencies (frequencies);

    } catch (std::string message) {
      cerr << "[Skymapper::initSkymapper] Failed assigning frequency values!"
	   << endl;
      cerr << message << endl;
    }
    
    /*
      With the image data written into an AIPS++ PagedImage, we need to create
      and initialize one first, before we can start inserting the computed 
      image data.
    */
    casa::CoordinateSystem csys = coordinates_p.coordinateSystem();
    casa::IPosition shape       = coordinates_p.shape();
    casa::TiledShape tile (shape);

    try {
      image_p = new casa::PagedImage<double> (tile,
					      csys,
					      filename_p);
    } catch (std::string message) {
      cerr << "[Skymapper::initSkymapper] Failed creating the image file!" << endl;
      cerr << "--> " << message << endl;
      isOperational_p = false;
    }

    // check the image file created in disk
    if (image_p->ok() && image_p->isWritable()) {
      // book-keeping of operational status
      isOperational_p = true;
      // feedback to the outside world
      if (verbose_p) {
	cout << "[Skymapper::initSkymapper] Image file appears ok and is writable."
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
      cerr << "[Skymapper::processData] Skymapper is not operational!"
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
    if (status) {
      // Declare additional variables
      casa::IPosition shape (coordinates_p.shape());
      casa::IPosition start  (shape.nelements(),0);
      casa::IPosition stride (shape.nelements(),1);

      // Set up temporary array used to insert the output from the Beamformer
      // into the pixel array of the PagedImage; this approach is not too 
      // efficient, so a better method needs to be found.
      
      casa::Array<double> tmp (IPosition(5,
					 shape(0),
					 shape(1),
					 shape(2),
					 1,
					 1)
			       );
      
      // Adjust the slicing operators
      start(3) = nofProcessedBlocks_p*timeAxisStride();

      // Progress bar
      ProgressBar bar (shape(0),">");

      /*
	Insert the previously computed pixel values into the pixel array of
	the already existing image.
      */
      
      // -----------------------------------------
      // insert one spectrum at a time

//       for (start(0)=0; start(0)<shape(0); start(0)++) {
// 	for (start(1)=0; start(1)<shape(1); start(1)++) {
// 	  for (start(2)=0; start(2)<shape(2); start(2)++) {
//  	    image_p->putSlice (beam.row(coord),start,stride);
// 	    // increment counter
// 	    coord++;
// 	  }  // -- end loop: start(2)
// 	}  // -- end loop: start(1)
// 	bar.update(start(0));
//       }  // -- end loop: start(0)
      
      // -----------------------------------------
      // insert radius-frequency plain at a time

      IPosition beamSliceStart (2,0);
      IPosition beamSliceEnd (2,shape(4)-1);

      for (start(0)=0; start(0)<shape(0); start(0)++) {      // Longitude
	for (start(1)=0; start(1)<shape(1); start(1)++) {    // Latitude
	  for (start(2)=0; start(2)<shape(2); start(2)++) {  // Radius
	    std::cout << "[" << shape(0)*shape(1)*shape(2) << " 0]" << std::endl;
	  }
	}
      }

      // -----------------------------------------

    } else {
      cerr << "[Skymapper::processData] No processing due to Beamformer error"
	   << endl;
    }
    
    /*
      Book-keeping of the number of processed blocks so far; this is accounted
      for independent of whether the beamforming stage has been successful or
      not, in order to keep navigation within the image pixel array consistent.
    */
    nofProcessedBlocks_p++;

    return status;
  }

  // -------------------------------------------------------- write_beam_to_image

  bool Skymapper::write_beam_to_image (Matrix<double> const &beam)
  {
    bool status (true);

    

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
    TimeFreq timeFreq           = coordinates_p.timeFreqCoordinate();
    casa::CoordinateSystem csys = coordinates_p.coordinateSystem();
    ObsInfo obsInfo             = csys.obsInfo();
    
    os << "[Skymapper] Summary of the internal parameters"             << endl;
    os << " - Observation ..............  "                               << endl;
    os << " -- observer                 = " << obsInfo.observer()         << endl;
    os << " -- telescope                = " << obsInfo.telescope()        << endl;
    os << " -- observation date         = " << obsInfo.obsDate()          << endl;
    os << " - Data I/O .................   "                              << endl;
    os << " -- blocksize      [samples] = " << timeFreq.blocksize()       << endl;
    os << " -- FFT length    [channels] = " << timeFreq.blocksize()       << endl;
    os << " -- sampling rate       [Hz] = " << timeFreq.sampleFrequency() << endl;
    os << " -- Nyquist zone             = " << timeFreq.nyquistZone()     << endl;
    os << " -- nof. antennas            = " << beamformer_p.nofAntennas() << endl;
    os << " - Coordinates .............  "                                << endl;
//     os << " -- reference code           = " << refcode                    << endl;
//     os << " -- projection               = " << projection                 << endl;
    os << " -- nof. coordinates         = " << csys.nCoordinates()        << endl;
    os << " -- names                    = " << csys.worldAxisNames()      << endl;
    os << " -- units                    = " << csys.worldAxisUnits()      << endl;
    os << " -- ref. pixel       (CRPIX) = " << csys.referencePixel()      << endl;
    os << " -- ref. value       (CRVAL) = " << csys.referenceValue()      << endl;
    os << " -- coord. increment (CDELT) = " << csys.increment()           << endl;
    os << " - Image ....................  "                               << endl;
    os << " -- filename                 = " << filename()                 << endl;
    os << " -- imaged quantity          = " << quantity_p.quantity()      << endl;
    os << " -- shape of pixel array     = " << coordinates_p.shape()      << endl;
//     os << " -- image type               = " << image_p.imageType()        << endl;
//     os << " -- is persistent?           = " << image_p.isPersistent()     << endl;
//     os << " -- is paged?                = " << image_p.isPaged()          << endl;
//     os << " -- is writable?             = " << image_p.isWritable()       << endl;
//     os << " -- has pixel mask?          = " << image_p.hasPixelMask()     << endl;
  }
  
}  // Namespace CR -- end
