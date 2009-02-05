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
  
  //_____________________________________________________________________________
  //                                                                    Skymapper
  
  Skymapper::Skymapper (std::string const &filename)
    : filename_p (filename)
  {
    init (SkymapCoordinate());
  }
  
  //_____________________________________________________________________________
  //                                                                    Skymapper
  
  Skymapper::Skymapper (SkymapCoordinate const &skymapCoord,
			std::string const &filename)
    : filename_p (filename)
  {
    init (skymapCoord);
  }
  
  //_____________________________________________________________________________
  //                                                                    Skymapper
  
  Skymapper::Skymapper (SkymapCoordinate const &skymapCoord,
			Matrix<double> const &antPositions,
			SkymapQuantity const &skymapType,
			std::string const &filename)
    : filename_p(filename)
  {
    init (skymapCoord,
	  antPositions,
	  skymapType);
  }

  //_____________________________________________________________________________
  //                                                                    Skymapper
  
  Skymapper::Skymapper (SkymapCoordinate const &skymapCoord,
			Vector<MVPosition> const &antPositions,
			SkymapQuantity const &skymapType,
			std::string const &filename)
    : filename_p(filename)
  {
    init (skymapCoord,
	  antPositions,
	  skymapType);
  }
  
  //_____________________________________________________________________________
  //                                                                    Skymapper
  
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
  
  void Skymapper::destroy ()
  {;}
  
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
    coord_p              = other.coord_p;
    filename_p           = other.filename_p;
    nofProcessedBlocks_p = other.nofProcessedBlocks_p;
    verbose_p            = other.verbose_p;
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

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
  
  //_____________________________________________________________________________
  //                                                          setSkymapCoordinate

  bool Skymapper::setSkymapCoordinate (SkymapCoordinate const &skymapCoord)
  {
    bool status (true);

    /* Store the skymap coordinates object */
    coord_p = skymapCoord;

    /* Update the Beamformer */
    Matrix<double> skyPos = coord_p.spatialCoordinate().positionValues();
    Vector<double> freq   = coord_p.timeFreqCoordinate().frequencyValues();
    beamformer_p.setSkyPositions(skyPos);
    beamformer_p.setFrequencies(freq);
    
    return status;
  }

  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                         init

  void Skymapper::init (SkymapCoordinate const &coord)
  {
    Beamformer bf;
    Matrix<double> antPositions = bf.antPositions();
    SkymapQuantity skymapType   = bf.skymapType();
    
    init (coord,
	  antPositions,
	  skymapType);
  }
  
  //_____________________________________________________________________________
  //                                                                         init

  void Skymapper::init (SkymapCoordinate const &skymapCoord,
			Matrix<double> const &antPositions,
			SkymapQuantity const &skymapType)
  {
    /* Store the skymap coordinates */
    coord_p = skymapCoord;

    /* set up the Beamformer */
    beamformer_p = Beamformer();
    beamformer_p.setAntPositions(antPositions);

    /* initialize the internal settings and objects */
    initSkymapper();
  }

  //_____________________________________________________________________________
  //                                                                         init

  void Skymapper::init (SkymapCoordinate const &skymapCoord,
			Vector<MVPosition> const &antPositions,
			SkymapQuantity const &skymapType)
  {
    /* Store the skymap coordinates */
    coord_p = skymapCoord;

    /* set up the Beamformer */
    beamformer_p = Beamformer();
    beamformer_p.setAntPositions(antPositions);

    /* initialize the internal settings and objects */
    initSkymapper();
  }
  
  //_____________________________________________________________________________
  //                                                                         init

  void Skymapper::init (short const &verbose,
			uint const &nofProcessedBlocks,
			std::string const &filename,
			SkymapCoordinate const &skymapCoord)
  {
    /* Store the provided input parameters */
    verbose_p            = verbose;
    nofProcessedBlocks_p = nofProcessedBlocks;
    filename_p           = filename;

    // Store embedded objects
    
    if (!setSkymapCoordinate (skymapCoord)) {
      cerr << "[Skymapper::init] Error setting SkymapCoordinate object!"
	   << endl;
    }
    
  }
  
  //_____________________________________________________________________________
  //                                                                initSkymapper

  bool Skymapper::initSkymapper () 
  {
    bool status (true);

    /* Initialize variables for book-keeping */
    verbose_p            = 0;
    nofProcessedBlocks_p = 0;
    
    /*
      Set up the beamformer:
      - set sky positions values
      - set frequency values
    */
    try {
      Matrix<double> skyPos = coord_p.spatialCoordinate().positionValues();
      Vector<double> freq   = coord_p.timeFreqCoordinate().frequencyValues();
      beamformer_p.setSkyPositions(skyPos);
      beamformer_p.setFrequencies(freq);
    } catch (std::string message) {
      std::cerr << "[Skymapper::initSkymapper] " << message << std::endl;
    }

    /*
      Set up the stride used to go through the pixel array of the output image
    */
    CoordinateType coordType = coord_p.timeFreqCoordinate().beamCoordDomain();
    uint nofAxes             = coord_p.nofAxes();
    stride_p.resize(nofAxes);
    stride_p = 1;
    switch (coordType.type()) {
    case CoordinateType::Time:
      stride_p(nofAxes-2) = coord_p.timeFreqCoordinate().blocksize();
      break;
    case CoordinateType::Frequency:
      stride_p(nofAxes-2) = 1;
      break;
    default:
      std::cerr << "[Skymapper::initSkymapper] Unsopported coordinate type!"
		<< std::endl;
      break;
    }

    /*
      With the image data written into an AIPS++ PagedImage, we need to create
      and initialize one first, before we can start inserting the computed 
      image data.
    */
    casa::CoordinateSystem csys = coord_p.coordinateSystem();
    casa::IPosition shape       = coord_p.shape();
    casa::TiledShape tile (shape);

    /* Create paged image on disk */
    image_p = new casa::PagedImage<double> (tile,
					    csys,
					    filename_p);
    
    // check the image file created in disk
    if (image_p->ok() && image_p->isWritable()) {
      cout << "[Skymapper::initSkymapper] Image file appears ok and is writable."
	   << endl;
    }
    
    return status;
  }
  
  // ---------------------------------------------------------------- processData
  
  bool Skymapper::processData (Matrix<DComplex> const &data)
  {
    bool status (true);

    // forward the data to the beamformer for processing
    Matrix<double> beam (beamformer_p.shapeBeam());
    status = beamformer_p.processData (beam,
				       data);

    /*
      Inserted the computed pixel values into the image.
    */
    if (status) {
      // Declare additional variables
      casa::IPosition shape (coord_p.shape());
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
      start(3) = nofProcessedBlocks_p*stride_p(stride_p.nelements()-2);

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
  
  // -------------------------------------------------------------------- summary
  
  void Skymapper::summary (std::ostream &os)
  {
    TimeFreq timeFreq           = coord_p.timeFreqCoordinate();
    casa::CoordinateSystem csys = coord_p.coordinateSystem();
    ObsInfo obsInfo             = csys.obsInfo();
    
    os << "[Skymapper] Summary of the internal parameters"             << endl;
    os << " :: Observation ::"                                                 << endl;
    os << " --> Observation date         = " << obsInfo.obsDate()          << endl;
    os << " --> Telescope                = " << obsInfo.telescope()        << endl;
    os << " --> Observer                 = " << obsInfo.observer()         << endl;
    os << " --> nof. antennas            = " << beamformer_p.nofAntPositions() << endl;
    os << " --> Sampling rate       [Hz] = " << timeFreq.sampleFrequency() << endl;
    os << " --> Nyquist zone             = " << timeFreq.nyquistZone()     << endl;
    os << " :: Data I/O ::"                              << endl;
    os << " --> Blocksize      [samples] = " << timeFreq.blocksize()       << endl;
    os << " --> FFT length    [channels] = " << timeFreq.fftLength()       << endl;
    os << " :: Coordinates ::"                                << endl;
    os << " --> nof. coordinates         = " << csys.nCoordinates()        << endl;
    os << " --> World axis names ....... = " << csys.worldAxisNames()      << endl;
    os << " --> World axis units ....... = " << csys.worldAxisUnits()      << endl;
    os << " --> Reference pixel  (CRPIX) = " << csys.referencePixel()      << endl;
    os << " --> Reference value  (CRVAL) = " << csys.referenceValue()      << endl;
    os << " --> coord. increment (CDELT) = " << csys.increment()           << endl;
    os << " :: Image ::"                               << endl;
    os << " --> Filename                 = " << filename()                 << endl;
    os << " --> Shape of pixel array     = " << coord_p.shape()            << endl;
    os << " --> Shape of beam array      = " << beamformer_p.shapeBeam()   << endl;
    os << " --> Image array stride       = " << stride_p                   << endl;
    os << " --> Domain of image quantity = " << beamformer_p.domainName()  << endl;
    os << " --> Skymap quantity          = " << beamformer_p.skymapType().name() << endl;
  }
  
}  // Namespace CR -- end
