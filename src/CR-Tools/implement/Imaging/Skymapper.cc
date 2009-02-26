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
			std::string const &filename)
    : filename_p(filename)
  {
    init (skymapCoord,
	  antPositions);
  }

  //_____________________________________________________________________________
  //                                                                    Skymapper
  
  Skymapper::Skymapper (SkymapCoordinate const &skymapCoord,
			Vector<MVPosition> const &antPositions,
			std::string const &filename)
    : filename_p(filename)
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
    /* Adjust array sizes */
    bufferStart_p.resize(other.bufferStart_p.nelements());
    bufferStride_p.resize(other.bufferStride_p.nelements());
    bufferStep_p.resize(other.bufferStep_p.nelements());

    /* Copy values of internal variables */
    beamformer_p         = other.beamformer_p;
    coord_p              = other.coord_p;
    filename_p           = other.filename_p;
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
    
    init (coord,
	  antPositions);
  }
  
  //_____________________________________________________________________________
  //                                                                         init

  void Skymapper::init (SkymapCoordinate const &skymapCoord,
			Matrix<double> const &antPositions)
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
    /*
     *  Configure the Beamformer.
     */

    /* Get sky positions and frequency values. */
    Matrix<double> skyPos = coord_p.spatialCoordinate().positionValues();
    Vector<double> freq   = coord_p.timeFreqCoordinate().frequencyValues();
    /* Update internal settings of the Beamformer. */
    beamformer_p.setSkymapType(coord_p.skymapQuantity().type());
    beamformer_p.setSkyPositions(skyPos);
    beamformer_p.setFrequencies(freq);
    
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
    
    bufferArray_p.resize(bufferShape);

    /*
      With the image data written into an AIPS++ PagedImage, we need to create
      and initialize one first, before we can start inserting the computed 
      image data.
    */
    casa::CoordinateSystem csys = coord_p.coordinateSystem();
    casa::IPosition shape       = coord_p.shape();
    casa::TiledShape tile (shape);

    /* Create paged image on disk */
#ifdef HAVE_HDF5
    image_p = new casa::HDF5Image<double> (tile,
					   csys,
					   filename_p);
#else
    image_p = new casa::PagedImage<double> (tile,
					    csys,
					    filename_p);
#endif
    
    /*
     *  Provide some minimal feedback about the image file created on disk.
     */

    if (!image_p->ok()) {
      std::cerr << "[Skymapper::initSkymapper] Image is not ok!" << endl;
    }

    if (!image_p->isWritable()) {
      std::cerr << "[Skymapper::initSkymapper] Image is not writable!" << endl;
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
      std::cout << "[Skymapper::processData]" << endl;
      std::cout << "-- nof. processed blocks = " << nofProcessedBlocks_p << endl;
      std::cout << "-- buffer start position = " << bufferStart_p        << endl;
      std::cout << "-- buffer stride         = " << bufferStride_p       << endl;
      std::cout << "-- buffer step           = " << bufferStep_p         << endl;
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
