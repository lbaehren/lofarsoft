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

#include <Imaging/Skymapper.h>

#include <casa/Exceptions/Error.h>

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
  
  Skymapper::Skymapper (std::string const &filename,
			Skymapper::ImageType const &imageType)
    : filename_p (filename),
      imageType_p (imageType)
  {
    init (SkymapCoordinate());
  }
  
  //_____________________________________________________________________________
  //                                                                    Skymapper
  
  Skymapper::Skymapper (SkymapCoordinate const &skymapCoord,
			std::string const &filename,
			Skymapper::ImageType const &imageType)
    : filename_p (filename),
      imageType_p (imageType)
  {
    init (skymapCoord);
  }
  
  //_____________________________________________________________________________
  //                                                                    Skymapper
  
  Skymapper::Skymapper (SkymapCoordinate const &skymapCoord,
			Matrix<double> const &antPositions,
			std::string const &filename,
			Skymapper::ImageType const &imageType)
    : filename_p (filename),
      imageType_p (imageType)
  {
    init (skymapCoord,
	  antPositions);
  }

  //_____________________________________________________________________________
  //                                                                    Skymapper
  
  Skymapper::Skymapper (SkymapCoordinate const &skymapCoord,
			Vector<MVPosition> const &antPositions,
			std::string const &filename,
			Skymapper::ImageType const &imageType)
    : filename_p (filename),
      imageType_p (imageType)
  {
    init (skymapCoord,
	  antPositions);
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
  
  //_____________________________________________________________________________
  //                                                                    operator=
  
  Skymapper &Skymapper::operator= (Skymapper const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  //_____________________________________________________________________________
  //                                                                         copy
  
  void Skymapper::copy (Skymapper const& other)
  {
    /* Adjust array sizes */
    bufferStart_p.resize(other.bufferStart_p.nelements());
    bufferStride_p.resize(other.bufferStride_p.nelements());
    bufferStep_p.resize(other.bufferStep_p.nelements());

    /* Copy values of internal variables */
    beamformer_p         = other.beamformer_p;
    coord_p              = other.coord_p;
    filename_p           = other.filename_p;
    imageType_p          = other.imageType_p;
    nofProcessedBlocks_p = other.nofProcessedBlocks_p;
    bufferStart_p        = other.bufferStart_p;
    bufferStride_p       = other.bufferStride_p;
    bufferStep_p         = other.bufferStep_p;
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                setBeamformer
  
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
  //                                                              setAntPositions

  /*!
    \param antPositions -- [antenna,3] array with the antenna positions.
    \param type         -- Coordinate type as which the antenna positions are
           provided; if necessary conversion is performed internally.
    \param anglesInDegrees -- If the coordinates of the antenna positions
           contain an angular component, is this component given in degrees?
	   If set <tt>false</tt> the angular components are considered to be
	   given in radians.
  */
  bool Skymapper::setAntPositions (Matrix<double> const &antPositions,
				   CoordinateType::Types const &type,
				   bool const &anglesInDegrees)
  {
    return beamformer_p.setAntPositions(antPositions,
					type,
					anglesInDegrees);
  }

  //_____________________________________________________________________________
  //                                                              setAntPositions
  
  /*!
    \param antPositions -- Array with the antenna positions.
  */
  bool Skymapper::setAntPositions (Vector<MVPosition> const &antPositions)
  {
    return beamformer_p.setAntPositions(antPositions);
  }
  
  //_____________________________________________________________________________
  //                                                          setSkymapCoordinate

  bool Skymapper::setSkymapCoordinate (SkymapCoordinate const &skymapCoord)
  {
    bool status (true);

    /* Store the skymap coordinates object */
    coord_p = skymapCoord;

    /* Update the Beamformer */
    CoordinateType::Types spatialType = coord_p.spatialCoordinate().type();
    Matrix<double> skyPos = coord_p.spatialCoordinate().worldAxisValues();
    Vector<double> freq   = coord_p.timeFreqCoordinate().frequencyValues();

    try {
      beamformer_p.setSkyPositions(skyPos,spatialType,false);
      beamformer_p.setFrequencies(freq);
    } catch (casa::AipsError x) {
      std::cerr << "[Skymapper::setSkymapCoordinate] Failed configuring Beamformer!\n"
		<< x.getMesg()
		<< std::endl;
    }
    
    return status;
  }

  //_____________________________________________________________________________
  //                                                         setBeamformerBuffers

  /*!
    \param bufferDelays  -- Buffer the values of the geometrical delays?
    \param bufferPhases  -- Buffer the values of the geometrical phases?
    \param bufferWeights -- Buffer the values of the geometrical weights?
  */
  bool Skymapper::setBeamformerBuffers (bool const &bufferDelays,
					bool const &bufferPhases,
					bool const &bufferWeights)
  {
    bool status (true);
    
    try {
      beamformer_p.bufferDelays(bufferDelays);
      beamformer_p.bufferPhases(bufferPhases);
      beamformer_p.bufferWeights(bufferWeights);
    } catch (casa::AipsError x) {
      std::cerr << "[Skymapper::setBeamformerBuffers]" 
		<< x.getMesg()
		<< std::endl;
      status = false;
    }
    
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
    
    init (coord,
	  antPositions);
  }
  
  //_____________________________________________________________________________
  //                                                                         init

  /*!
    \param skymapCoord  -- Coordinates to be attached to the skymap.
    \param antPositions -- Antenna positions to be fed into the Beamformer
  */
  void Skymapper::init (SkymapCoordinate const &skymapCoord,
			Matrix<double> const &antPositions)
  {
    /* Store the skymap coordinates */
    coord_p = skymapCoord;

    /* set up the Beamformer */
    beamformer_p = Beamformer();
    beamformer_p.setAntPositions(antPositions);

#ifdef DEBUGGING_MESSAGES
    cout << "[Skymapper::init]" << std::endl;
    cout << "-- antenna positions (input) = " << antPositions
	 << std::endl;
    cout << "-- antenna positions (BF)    = " << beamformer_p.antPositions()
	 << std::endl;
#endif

    /* initialize the internal settings and objects */
    initSkymapper();
  }

  //_____________________________________________________________________________
  //                                                                         init

  /*!
    \param skymapCoord  -- Coordinates to be attached to the skymap.
    \param antPositions -- Antenna positions to be fed into the Beamformer
  */
  void Skymapper::init (SkymapCoordinate const &skymapCoord,
			Vector<MVPosition> const &antPositions)
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
  //                                                                initSkymapper

  bool Skymapper::initSkymapper () 
  {
    bool status (true);

    /* Initialize variables for book-keeping */
    nofProcessedBlocks_p = 0;

    //________________________________________________________________
    // Configure the Beamformer.

    try {
      beamformer_p.setSkymapType(coord_p.skymapQuantity().type());
    } catch (casa::AipsError x) {
      std::cerr << "[Skymapper::initSkymapper] Failed configuring Beamformer!\n"
		<< x.getMesg()
		<< std::endl;
    }
    
    try {
      CoordinateType::Types spatialType = coord_p.spatialCoordinate().type();
      Matrix<double> skyPos = coord_p.spatialCoordinate().worldAxisValues();
      beamformer_p.setSkyPositions(skyPos,spatialType,false);
    } catch (casa::AipsError x) {
      std::cerr << "[Skymapper::initSkymapper] Failed setting sky positions!\n"
		<< x.getMesg()
		<< std::endl;
    }

    try {
      Vector<double> freq   = coord_p.timeFreqCoordinate().frequencyValues();
      beamformer_p.setFrequencies(freq);
    } catch (casa::AipsError x) {
      std::cerr << "[Skymapper::initSkymapper] Failed setting frequencies!\n"
		<< x.getMesg()
		<< std::endl;
    }

    //________________________________________________________________
    /*
     *  Set up the characteristics of the buffer array and the slicing
     *  information on how to insert it into the pixel array of the output
     *  image.
     */

    CoordinateType coordType    = coord_p.timeFreqCoordinate().coordType();
    uint nofAxes                = coord_p.nofAxes();
    casa::IPosition bufferShape = coord_p.shape();

    bufferStart_p.resize(nofAxes);
    bufferStride_p.resize(nofAxes);
    bufferStep_p.resize(nofAxes);

    nofProcessedBlocks_p   = 0;
    nofProcessedFrames_p   = 0;
    nofBlocksPerFrame_p    = coord_p.timeFreqCoordinate().blocksPerFrame();
    bufferStart_p          = 0;
    bufferStride_p         = 1;
    bufferStep_p           = 0;
    
    switch (coordType.type()) {   // [Long,Lat,Radius,Freq,Time]
    case CoordinateType::Time:
      {
	timeFreqAxis_p                = nofAxes-1;
	bufferShape(timeFreqAxis_p-1) = 1;
	bufferShape(timeFreqAxis_p)   = beamformer_p.shapeBeam()[0];
	bufferStep_p(timeFreqAxis_p)  = coord_p.timeFreqCoordinate().blocksize();
      }
      break;
    case CoordinateType::Frequency:
      {
	timeFreqAxis_p                 = nofAxes-2;
	bufferShape(timeFreqAxis_p+1)  = 1;                             // Time
	bufferStep_p(timeFreqAxis_p+1) = 1;                             // Time
	bufferShape(timeFreqAxis_p)    = beamformer_p.shapeBeam()[0];   // Frequency
      }
      break;
    default:
      std::cerr << "-- Unsopported coordinate type!"
		<< std::endl;
      break;
    }

    try {    
      bufferArray_p.resize(bufferShape);
      bufferArray_p = 0;
    } catch (casa::AipsError x) {
      std::cerr << "[Skymapper::initSkymapper] Failed initializing buffer array!\n"
		<< x.getMesg()
		<< std::endl;
    }
    
    //________________________________________________________________
    /*
      With the image data written into an AIPS++ PagedImage, we need to create
      and initialize one first, before we can start inserting the computed 
      image data.
    */

    casa::CoordinateSystem csys = coord_p.coordinateSystem();
    casa::IPosition shape       = coord_p.shape();
    casa::TiledShape tile (shape);
    
    /* Create paged image on disk */
    try {
      switch (imageType_p) {
      case Skymapper::HDF5Image:
	image_p = new casa::HDF5Image<float> (tile,
					      csys,
					      filename_p);
	break;
      case Skymapper::PagedImage:
	image_p = new casa::PagedImage<float> (tile,
					       csys,
					       filename_p);
	break;
      default:
	status = false;
	break;
      };
    } catch (casa::AipsError x) {
      std::cerr << "[Skymapper::initSkymapper] Failed creating image on disk!\n"
		<< x.getMesg()
		<< std::endl;
    }
    
    //________________________________________________________________
    /*
      Provide some minimal feedback about the image file created on disk.
     */
    
    if (!image_p->ok()) {
      std::cerr << "[Skymapper::initSkymapper] Image is not ok!" << endl;
      status = false;
    }
    
    if (!image_p->isWritable()) {
      std::cerr << "[Skymapper::initSkymapper] Image is not writable!" << endl;
      status = false;
    }
    
    return status;
  }

  //_____________________________________________________________________________
  //                                                                  processData
  
  bool Skymapper::processData (Matrix<DComplex> const &data)
  {
    bool status (true);
    
    casa::IPosition shape (coord_p.shape());
    casa::IPosition shapeBeam (beamformer_p.shapeBeam());
    casa::IPosition pos (bufferArray_p.shape());
    Matrix<double> beam (shapeBeam);
    int npos (0);

    //________________________________________________________________
    /*
     *  Forward the input data to the Beamformer for processing
     */
    status = beamformer_p.processData (beam,
				       data);
    
#ifdef DEBUGGING_MESSAGES
    std::cout << "[Skymapper::processData]" << std::endl;
    std::cout << "-- Input data [freq,ant]: " << std::endl;
    std::cout << "-- data      [,1]   = " << data.column(1)         << std::endl;
    std::cout << "-- data      [1,]   = " << data.row(1)            << std::endl;
    std::cout << "-- Beamformed data [freq,sky]:" << std::endl;
    std::cout << "-- beam      [,1]   = " << beam.column(1)         << std::endl;
    std::cout << "-- beam      [1,]   = " << beam.row(1)            << std::endl;
#endif

    nofProcessedBlocks_p++;

    //________________________________________________________________
    /*
     *  Map the contents of the array returning the Beamformer results
     *  onto the internal buffer array.
     */
    if (status) {
      pos = 0;
      for (pos(0)=0; pos(0)<shape(0); pos(0)++) {      // Longitude
	for (pos(1)=0; pos(1)<shape(1); pos(1)++) {    // Latitude
	  for (pos(2)=0; pos(2)<shape(2); pos(2)++) {  // Radius
	    for (pos(timeFreqAxis_p)=0; pos(timeFreqAxis_p)<shape(timeFreqAxis_p); pos(timeFreqAxis_p)++) {
	      bufferArray_p(pos) += beam(pos(timeFreqAxis_p),
					 npos);
	    }            // END -- Freq/Time
	    npos++;
	  }              // END -- Radius
	}                // END -- Latitude
      }                  // END -- Longitude

    } else {
      cerr << "[Skymapper::processData] No processing due to Beamformer error"
	   << endl;
    }
    
    //________________________________________________________________
    /*
     *  Book-keeping of the number of processed blocks so far; this is
     *  accounted for independent of whether the beamforming stage has
     *  been successful or not, in order to keep navigation within the
     *  image pixel array consistent.
     */
    if (nofProcessedBlocks_p == nofBlocksPerFrame_p) {
#ifdef DEBUGGING_MESSAGES
      cout << "[Skymapper::processData] Finished processing blocks for timeframe"
	   << endl;
      cout << "-- nof. processed blocks = " << nofProcessedBlocks_p << endl;
      cout << "-- buffer start position = " << bufferStart_p        << endl;
      cout << "-- buffer stride         = " << bufferStride_p       << endl;
      cout << "-- buffer step           = " << bufferStep_p         << endl;
#endif
      /* Write the buffered data to the output image. */
      image_p->putSlice (bufferArray_p,
			 bufferStart_p,
			 bufferStride_p);
      /* Reset internal variables. */
      bufferArray_p         = 0;
      nofProcessedBlocks_p  = 0;
      nofProcessedFrames_p += 1;
      bufferStart_p        += bufferStep_p;
    }
    
    return status;
  }
  
  //_____________________________________________________________________________
  //                                                                  processData
  
  bool Skymapper::processData (DataReader *dr)
  {
    bool status (true);
    
    /*
     * Check the settings within the DataReader object; most important of all,
     * the blocksize and the number of antennas must be correct, because otherwise
     * matrix operations will fail.
     */
    if (coord_p.timeFreqCoordinate().blocksize() != dr->blocksize()) {
	std::cerr << "[Skymapper::processData] "
		  << " Mismatch discovered in value of blocksize!"
		  << endl;
	std::cerr << "-- Skymap coordinates : "
		  << coord_p.timeFreqCoordinate().blocksize()
		  << endl;
	std::cerr << "-- DataReader         : " 
		  << dr->blocksize() 
		  << endl;
	return false;
    }
    
    /*
     * Loop over the number of input data blocks which are to be processed and 
     * written to the output image
     */
    uint nofBlocks = coord_p.timeFreqCoordinate().nofBlocks();
    Matrix<casa::DComplex> data (dr->fftLength(),
				 dr->nofSelectedAntennas());
    
    for (uint block(0); block<nofBlocks; block++) {
      // retrieve block of data
      data = dr->calfft();
      // forward the data for processing
      status = processData (data);
      // go to the next block of data
      dr->nextBlock();
    }
    
    return status;
  }
  
  //_____________________________________________________________________________
  //                                                                      summary
  
  void Skymapper::summary (std::ostream &os)
  {
    TimeFreq timeFreq           = coord_p.timeFreqCoordinate();
    casa::CoordinateSystem csys = coord_p.coordinateSystem();
    ObsInfo obsInfo             = csys.obsInfo();
    
    os << "[Skymapper] Summary of the internal parameters"             << endl;
    os << " ::: Observation / Data :::"                                      << endl;
    os << " --> Observation date         = " << obsInfo.obsDate()          << endl;
    os << " --> Telescope                = " << obsInfo.telescope()        << endl;
    os << " --> Observer                 = " << obsInfo.observer()         << endl;
    os << " --> nof. antennas            = " << beamformer_p.nofAntPositions() << endl;
    os << " --> Sampling rate       [Hz] = " << timeFreq.sampleFrequency() << endl;
    os << " --> Nyquist zone             = " << timeFreq.nyquistZone()     << endl;
    os << " --> Blocksize      [samples] = " << timeFreq.blocksize()       << endl;
    os << " --> FFT length    [channels] = " << timeFreq.fftLength()       << endl;
    os << " ::: Coordinates :::"                                << endl;
    os << " --> Skymap quantity          = " << coord_p.skymapQuantity().name()
       << endl;
    os << " --> Domain of image quantity = " << coord_p.skymapQuantity().domainName()
       << endl;
    os << " --> nof. coordinates         = " << csys.nCoordinates()        << endl;
    os << " --> World axis names ....... = " << csys.worldAxisNames()      << endl;
    os << " --> World axis units ....... = " << csys.worldAxisUnits()      << endl;
    os << " --> Reference pixel  (CRPIX) = " << csys.referencePixel()      << endl;
    os << " --> Reference value  (CRVAL) = " << csys.referenceValue()      << endl;
    os << " --> coord. increment (CDELT) = " << csys.increment()           << endl;
    os << " ::: Image :::"                               << endl;
    os << " --> Filename                 = " << filename()                 << endl;
    os << " --> Shape of pixel array     = " << coord_p.shape()            << endl;
    os << " --> Shape of beam array      = " << beamformer_p.shapeBeam()   << endl;
    os << " --> Shape of buffer array    = " << bufferArray_p.shape()      << endl;
    os << " --> buffer start position    = " << bufferStart_p              << endl;
    os << " --> buffer end position      = " << bufferStride_p             << endl;
    os << " --> buffer step size         = " << bufferStep_p               << endl;
    os << " --> index of the target axis = " << timeFreqAxis_p             << endl;
  }
  
}  // Namespace CR -- end
