/***************************************************************************
 *   Copyright (C) 2004-2006                                               *
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

/* $Id: Skymap.cc,v 1.7 2006/10/31 18:24:08 bahren Exp $ */

#include <Skymap/Skymap.h>
#include <Math/MathFFT.h>

using std::cerr;
using std::cout;
using std::endl;

namespace LOPES {  // namespace LOPES -- begin

// =============================================================================
//
// Construction / Destruction
//
// =============================================================================

// ----------------------------------------------------------------------- Skymap

Skymap::Skymap ()
  : Beamformer ()
{
  Skymap::init ();
}

// ----------------------------------------------------------------------- Skymap

Skymap::Skymap (const ObsInfo& obsInfo,
		const String refcode,
		const String projection,
		const IPosition& shape,
		const Vector<double>& center,
		const Vector<double>& increment)
{
  Skymap::setSkymapGrid (obsInfo,
			 refcode,
			 projection,
			 shape,
			 center,
			 increment);
}

// ---------------------------------------------------------------------- ~Skymap

Skymap::~Skymap ()
{
  bool deleteIt = true;
  // Skymap of complex values
  const DComplex* ptrSkymapComplex = skymapComplex_p.getStorage (deleteIt);
  skymapComplex_p.freeStorage(ptrSkymapComplex,deleteIt);
  // Skymap of real data
  const double* ptrSkymapReal = skymapReal_p.getStorage (deleteIt);
  skymapReal_p.freeStorage(ptrSkymapReal,deleteIt);
  // Mapping between internal matrix and skymap cube
  const int* ptrMappingToCube = matrix2cube_p.getStorage (deleteIt);
  matrix2cube_p.freeStorage(ptrMappingToCube,deleteIt);
}

// ==============================================================================
//
// Initialization
//
// ==============================================================================

// ------------------------------------------------------------------------- init

void Skymap::init ()
{
  nofAddedBlocks_p    = 0;
  itsSkymapShapeSet   = false;
  itsMappingToCubeSet = false;
  addBaselines_p      = false;
  crossCorrelation_p  = false;
  signalQuantity_p    = "Power";
  signalDomain_p      = "Frequency";
  quantity_p          = SkymapQuantity::FREQ_POWER;
  distance_p          = -1.0;
  // default coordinate system
  Skymap::setCoordinateSystem ();
}

// ------------------------------------------------------------- setMappingToCube

void Skymap::setMappingToCube (const Matrix<int>& matrix2cube)
{  
  matrix2cube_p.resize(matrix2cube.shape());
  //
  matrix2cube_p = matrix2cube;
  // set the flag
  itsMappingToCubeSet = true;
}

// --------------------------------------------------------------- setSkymapShape

void Skymap::setSkymapShape ()
{
  int nofFrequencies (0);
  int nofPixels (0);
  int numberAntennas (0);
  int thirdAxis (0);

  // extract the shape of the phase gradients data cube
  Beamformer::getGradientsShape (nofFrequencies,nofPixels,numberAntennas);

  /* Determine the length of the third image axis; this depends on wether the
     electric quantity displayed in the map is given in the time domain or in
     the frequency domain. */
  if (signalDomain_p == "Frequency") {
    thirdAxis = nofFrequencies;
  }
  else if (signalDomain_p == "Time") {
    thirdAxis = blocksize_p;
  }

  if (!strcmp(signalQuantity_p.c_str(),"Field")) {
    // Electric field as function of time: real
    if (signalDomain_p == "Time") {
      skymapComplex_p.resize(0,0);
      skymapReal_p.resize(thirdAxis,nofPixels);
    }
    // Electric field as function of frequency: complex
    if (signalDomain_p == "Frequency") {
      skymapComplex_p.resize(thirdAxis,nofPixels);
      skymapReal_p.resize(0,0);      
    }
  }
  // Electric power as function of time or frequency: real
  else if (signalQuantity_p == "Power") {
    skymapComplex_p.resize(0,0);
    skymapReal_p.resize(thirdAxis,nofPixels);
  }
  else {
    cerr << "Skymap::SkymapShape" << endl;
    cerr << "\tUnknown signal domain " << signalDomain_p << endl;
  }
  // set the flag
  itsSkymapShapeSet = true;
}

// =============================================================================
//
//  Set multiple data fields from record
//
// =============================================================================

// -------------------------------------------------------------------- setSkymap

void Skymap::setSkymap (GlishRecord& record)
{
  GlishArray gtmp;
  int itmp;
  double dtmp;
  String stmp;

  // Set up the base class(es) first
  Beamformer::setBeamformer (record);

  if (record.exists("Blocksize")) {
    gtmp = record.get("Blocksize");
    gtmp.get(itmp);
    setBlocksize(itmp);
  }
  if (record.exists("SignalQuantity")) {
    gtmp = record.get("SignalQuantity");
    gtmp.get(stmp);
    setSignalQuantity(stmp);
  }
  if (record.exists("SignalDomain")) {
    gtmp = record.get("SignalDomain");
    gtmp.get(stmp);
    setSignalDomain(stmp);
  }
  if (record.exists("Distance")) {
    gtmp = record.get("Distance");
    gtmp.get(dtmp);
    setDistance(dtmp);
  }
  
}


// =============================================================================
//
//  Access to the embedded SkymapGrid object
//
// =============================================================================

// ---------------------------------------------------------------- setSkymapGrid

void Skymap::setSkymapGrid (const ObsInfo& obsInfo,
			    const String refcode,
			    const String projection,
			    const IPosition& shape,
			    const Vector<double>& center,
			    const Vector<double>& increment)
{
  try {
    SkymapGrid grid (obsInfo,
		     refcode,
		     projection,
		     shape,
		     center,
		     increment);
    //
    grid_p = grid;
  } catch (AipsError x) {
    cerr << "[Skymap::setSkymapGrid] " << x.getMesg() << endl;
  } 
}

// --------------------------------------------------------------- setOrientation

void Skymap::setOrientation (const Vector<String>& directions)
{
  grid_p.setOrientation (directions);
}

// -------------------------------------------------------------- beamformingGrid

Matrix<double> Skymap::beamformingGrid ()
{
  Matrix<double> directions;

  // -----------------------------------------------------------------
  // New implementation.
  
  Matrix<double> azel (grid_p.azel(true));
  
  // Check if we use the distance information
  if (distance_p > 0) {
    IPosition shape (azel.shape());
    IPosition blc (shape);
    IPosition trc (shape);
    // Increase the number of elements along the second array axis
    shape(1) += 1;
    // Correct the array size ...
    directions.resize (shape);
    // ... fill it with the distance information
    directions = distance_p;
    // Addressing of the arrays
    blc = 0;
    trc -= 1;
    // Copy the values from SkymapGrid to the directions array
    directions(blc,trc) = azel(blc,trc);
    // Pass the data to the Beamformer
    return directions;
  } else {
    return azel;
  }

  // -----------------------------------------------------------------
  // Possible fix for the azimuth values

//     cerr << "[Skymap::setPhaseGradients] Fixing azimuth value!" << endl;
//     IPosition shape (directions.shape());
//     for (int pixel=0; pixel<shape(0); pixel++) {
//       directions(pixel,0) += 180;
//     }
  
}

// ------------------------------------------------------------------------- mask

Cube<bool> Skymap::mask (const int channels)
{
  IPosition shape (grid_p.shape());
  Vector<bool> mask (grid_p.mask());
  int numPixel (0);
  int nofValidPixels (0);
  //
  Cube<bool> maskCube (shape(0),shape(1),channels);
  maskCube = true;
  //
  for (int nx=0; nx<shape(0); nx++) {
    for (int ny=0; ny<shape(1); ny++) {
      if (mask(numPixel)) {
	nofValidPixels++;
      } else {
	for (int nfreq=0; nfreq<channels; nfreq++) {
	  maskCube(nx,ny,nfreq) = false;
	}
      }
      //
      numPixel++;
    }
  }

//   cout << "[Skymap::mask] " << nofValidPixels << "/" << numPixel
//        << " pixels valid." << endl;

  return maskCube;
}


// =============================================================================
//
//  Characterization of the electrial quantities stored in the map
//
// =============================================================================

// --------------------------------------------------------------- SignalQuantity

void Skymap::setSignalQuantity (String const &quantity)
{
  if (quantity == "field" || quantity == "Field") {
    signalQuantity_p = "Field";
  }
  else if (quantity == "power" || quantity == "Power") {
    signalQuantity_p = "Power";
  }
  else if (quantity == "cc" || quantity == "CC") {
    signalQuantity_p = "cc";
  }
  else {
    cerr << "[Skymap::setSignalQuantity]" << endl;
    cerr << "\tUnidentified electrical quantity " << quantity << endl;
    signalQuantity_p = "Power";
  }
}

// ----------------------------------------------------------------- SignalDomain

void Skymap::setSignalDomain (String const &SignalDomain)
{
  if (SignalDomain =="freq" || SignalDomain =="Frequency") {
    signalDomain_p = "Frequency";
  }
  else if (SignalDomain == "time" || SignalDomain == "Time") {
    signalDomain_p = "Time";
  }
  else {
    cerr << "Skymap::setSignalDomain" << endl;
    cerr << "\tUnidentified signal domain " << SignalDomain << endl;
    signalDomain_p = "Frequency";
  }
}

// ------------------------------------------------------------ setSkymapQuantity

void Skymap::setSkymapQuantity (SkymapQuantity::Type const &mapQuantity)
{
  quantity_p = mapQuantity;
  
  // backwards compatibility

  String domain;
  String quantity;

  switch (quantity_p) {
  case SkymapQuantity::FREQ_FIELD:
    domain   = "Frequency";
    quantity = "Field";
    pFunction = &Skymap::addAntennas;
    break;
  case SkymapQuantity::FREQ_POWER:
    domain   = "Frequency";
    quantity = "Power";
    pFunction = &Skymap::addAntennas;
    break;
  case SkymapQuantity::TIME_FIELD:
    domain   = "Time";
    quantity = "Field";
    pFunction = &Skymap::addAntennas;
    break;
  case SkymapQuantity::TIME_POWER:
    domain   = "Time";
    quantity = "Power";
    pFunction = &Skymap::addAntennas;
    break;
  case SkymapQuantity::TIME_CC:
    domain   = "Time";
    quantity = "cc";
    pFunction = &Skymap::ccBeam;
    break;
  case SkymapQuantity::TIME_X:
    domain   = "Time";
    quantity = "x";
    break;
  }
  
  setSignalDomain (domain);
  setSignalQuantity (quantity);
}

// ----------------------------------------------------------- adjustSignalDomain

void Skymap::adjustSignalDomain (Matrix<DComplex>& skymap)
{
  IPosition shape (Skymap::shape());  // [channel,pixel]
  IPosition shapeInput (skymap.shape());
  const int fftsize (DataFrequency::FFTSize());
  int FFTlen (DataFrequency::FFTlen());
  bool verboseON (false);
  bool dumpON (false);
  int sample (0);
  int pixel (0);
  int channel (0);

  /* If enabled make a dump of the input data. */ 
  if (dumpON) {
    cout << "[Skymap::adjustSignalDomain] Dumping input array of shape "
	 << shape << " ... ";
    ofstream log;
    log.open("Skymap.log",ios::out);
    for (channel=0; channel<shapeInput(0); ++channel) {
      log << channel;
      for (pixel=0; pixel<shapeInput(1); ++pixel) {
	log << "\t" << real(skymap(channel,pixel))
	    << "\t" << imag(skymap(channel,pixel)); 
      }
      log << endl;
    }
    log.close();
    cout << "Done." << endl;
  }
  
  /* If enabled, provide some basic feedback. */
  if (verboseON) {
    cout << "[Skymap::adjustSignalDomain]" << endl;
    cout << " - Shape of input skymap ...... : " << skymap.shape() << endl;
    cout << " - Shape of internal skymap ... : " << Skymap::shape() << endl;
    cout << " - Signal domain .............. : " << signalDomain_p << endl;
    cout << " - Signal quantity ............ : " << signalQuantity_p << endl;
    cout << " - Blocksize (time domain) .... : " << fftsize << endl;
    cout << " - Blocksize (frequency domain) : " << FFTlen << endl;
  }

//   // Fix ???
//   cout << "Fixing frequency axis ... " << flush;
//   for (int pixel=0; pixel<shapeInput(1); ++pixel) {
//     for (int chan=0; chan<shapeInput(0); ++chan) {
//       DComplex tmp = skymap(chan,pixel);
//       skymap(chan,pixel) = DComplex(-imag(tmp),real(tmp));
//     }
//   }
//   cout << "Done." << endl;
   
  // --- (1) Time domain ---------------------------------------------
  if (signalDomain_p == "Time") {
    // additional variables
    Vector<DComplex> vectFreq (FFTlen);
    // feedback
    if (verboseON) {
      std::cout << " - FFT back to time domain ... " << std::flush;
    }
    // loop over all pixels in the map
    for (pixel=0; pixel<shape(1); ++pixel) {
      vectFreq = skymap.column(pixel);
      // Use DComplex-DComplex backwards FFT
      Vector<double> vectTime = LOPES::FFT2Fx<double,DComplex> (vectFreq,
								fftsize,
								true,
								false);
      // store the data vector
      for (sample=0; sample<fftsize; ++sample) {
 	skymapReal_p(sample,pixel) = vectTime(sample);
      }
      // if required: store power
      if (signalQuantity_p == "Power") {
	for (sample=0; sample<fftsize; ++sample) {
	  skymapReal_p(sample,pixel) *= vectTime(sample);
	}
      }
    }
    if (verboseON) {
      cout << "Done." << endl;
    }
  }
  // --- (2) Frequency domain ----------------------------------------
  else if (signalDomain_p == "Frequency") {
    // Electric power as function of frequency
    if (signalQuantity_p == "Power") {
      for (pixel=0; pixel<shape(1); ++pixel) {
	for (int freq=0; freq<shape(0); ++freq) {
	  skymapReal_p(freq,pixel) += casa::fabs(skymap(freq,pixel)*conj(skymap(freq,pixel)));
	}
      }
    }
    // Electric field as function of frequency
    else {
      skymapComplex_p += skymap;
    }
  }
  else {
    cerr << "Skymap::adjustSignalDomain" << endl;
    cerr << "\tInvalid signal domain " << signalQuantity_p << endl;
  }

}

// =============================================================================
//
//  Handling of the coordinate system tool to be attached to the skymap
//
// =============================================================================

// ---------------------------------------------------------- setCoordinateSystem

void Skymap::setCoordinateSystem ()
{
  // Direction coordinate
  {
    DirectionCoordinate dc;
    {
      Vector<String> worldAxisNames(2);
      worldAxisNames(0) = "Longitude";
      worldAxisNames(1) = "Latitude";
      dc.setWorldAxisNames(worldAxisNames);
    }
    csys_p.addCoordinate(dc);
  }
  // Linear coordinates: Distance, Time  
  {
    Vector<double> crpix(2,1.0);
    Vector<double> crval(2,1.0);
    Vector<double> cdelt(2,1.0);
    Matrix<double> pc(2,2); pc= 0; pc.diagonal() = 1.0;
    Vector<String> worldAxisNames(2);
    Vector<String> names(2);
    Vector<String> units(2);
    //
    worldAxisNames(0) = "Distance";
    worldAxisNames(1) = "Time";
    names(0)          = "Length"; 
    names(1)          = "Time";
    units(0)          = "m";
    units(1)          = "s";
    
    LinearCoordinate lc (names, units, crval, cdelt, pc, crpix);
    lc.setWorldAxisNames(worldAxisNames);
    csys_p.addCoordinate(lc);
  }
  // Spectral axis: Frequency
  {
    SpectralCoordinate sc;
    {
      Vector<String> worldAxisNames(1);
      worldAxisNames(0) = "Frequency";
      sc.setWorldAxisNames(worldAxisNames);
    }
    csys_p.addCoordinate(sc);
  }
}

// ---------------------------------------------------------- setCoordinateSystem

void Skymap::setCoordinateSystem (CoordinateSystem const &csys)
{
  if (csys.nCoordinates() == csys_p.nCoordinates()) {
    csys_p = csys;
  } else {
    int axis (0);
    // Direction axis
    axis = csys.findCoordinate (Coordinate::DIRECTION,-1);
    DirectionCoordinate direction (csys.directionCoordinate(axis));
    axis = csys_p.findCoordinate(Coordinate::DIRECTION,-1);
    csys_p.replaceCoordinate(direction,axis);
    
    // Frequency axis
    axis = csys.findCoordinate (Coordinate::SPECTRAL,-1);
    SpectralCoordinate spectral (csys.spectralCoordinate(axis));
    axis = csys_p.findCoordinate(Coordinate::SPECTRAL,-1);
    csys_p.replaceCoordinate(spectral,axis);
  }
}

// ---------------------------------------------------------- setCoordinateSystem

void Skymap::setCoordinateSystem (const GlishRecord& record)
{
  GlishArray gtmp;
  Vector<String> stmp;
  Vector<double> dtmp;
  uint naxis (csys_p.nPixelAxes());

  if (record.exists("worldAxisNames")) {
    gtmp = record.get("worldAxisNames");
    gtmp.get(stmp);
    if (stmp.nelements() == naxis) {
      csys_p.setWorldAxisNames(stmp);
    }
  }
  if (record.exists("increment")) {
    gtmp = record.get("increment");
    gtmp.get(dtmp);
    if (dtmp.nelements() == naxis) csys_p.setIncrement(dtmp);
  }
  if (record.exists("referencePixel")) {
    gtmp = record.get("referencePixel");
    gtmp.get(dtmp);
    if (dtmp.nelements() == naxis) csys_p.setReferencePixel(dtmp);
  }
}

void Skymap::directionCoordinate (Vector<double>& referenceValue,
				  Vector<double>& increment,
				  Matrix<double>& xform,
				  Vector<double>& referencePixel,
				  Vector<String>& worldAxisUnits,
				  Vector<double>& projectionParameters)
{
  bool verbose (true);
  //
  DirectionCoordinate dc (grid_p.directionCoordinate());
  Projection projection (dc.projection());
  //
  referenceValue       = dc.referenceValue();
  increment            = dc.increment();
  xform                = dc.linearTransform();
  referencePixel       = dc.referencePixel();
  worldAxisUnits       = dc.worldAxisUnits();
  projectionParameters = projection.parameters();
  //
  Vector<double> pixel (dc.referencePixel());
  Vector<double> world (dc.referenceValue());
  //
  if (verbose) {
    cout << "[Skymap::directionCoordinate]" << endl;
    cout << " - MDirection::Type .... : " << dc.directionType() << endl;
    cout << " - referenceValue ...... : " << referenceValue << endl;
    cout << " - referencePixel ...... : " << referencePixel << endl;
    cout << " - increment ........... : " << increment << endl;
    cout << " - worldAxisUnits ...... : " << worldAxisUnits << endl;
    cout << " - Projection name ..... : " << projection.name() << endl;
    cout << " - Projection parameters : " << projectionParameters << endl;
    //
//     verbose = dc.toWorld (world,pixel);
//     cout << " - Coordinate conversion : " << pixel << " -> " << world << endl;
//     //
    world = referenceValue;
    pixel = 0.0;
    verbose = dc.toPixel (pixel,world);
    cout << "                         : " << world << " -> " << pixel << endl;
  }
}

// =============================================================================
//
//  Information on the image axis types.
//
// =============================================================================

// ---------------------------------------------------------- getRebinningIndices

void Skymap::getRebinningIndices (Matrix<int>& indexValues,
				  int channels) 
{
  int pixels (0);
  int nfreq (0);
  double bins_per_channel;
  double tmp (0.0);
  bool verboseON (false);

  /* If the skymap hold the electric field the frequency domain, the member data
     stored in a complex valued array; otherwise in a real valued array. */
  if (!strcmp(signalDomain_p.c_str(),"Frequency") &&
      !strcmp(signalQuantity_p.c_str(),"Field")) {
    skymapComplex_p.shape(nfreq,pixels);
  }
  else skymapReal_p.shape(nfreq,pixels);

  /* Guard against division by zero. */
  if (channels == 0) {
    cerr << "[Skymap::getRebinningIndices] ";
    cerr << "Number of output channels must no be zero!" << endl;
    cerr << "\t" << channels << " -> " << nfreq << endl;
    channels = nfreq;
  }

  /* For each channel we store the array index of the first and the last
     element as in the original data array. */
  indexValues.resize(channels,2);

  bins_per_channel = 1.0*(nfreq-1)/channels;
  
  for (int k=0; k<channels; ++k) {
    tmp = k*bins_per_channel;
    indexValues(k,0) = (int)floor(tmp+0.5);
    tmp = (k+1)*bins_per_channel;
    indexValues(k,1) = (int)floor(tmp+0.5);
  }

  // control output

  if (verboseON) {  
    cout << "[Skymap::getRebinningIndices]" << endl;
    cout << " - Input frequency channels  : " << nfreq << endl;
    cout << " - Output frequency channels : " << channels << endl;
    cout << " - Bins per channel ........ : " << bins_per_channel << endl;
  }

}


// =============================================================================
//
//  interface with the methods provided by the Beamformer
//
// =============================================================================

// ------------------------------------------------------------ setPhaseGradients

void Skymap::setPhaseGradients (Vector<double> &frequencies,
				Matrix<double> &antennaPositions) 
{
  Matrix<double> directions (Skymap::beamformingGrid());

//   cout << "[Skymap::setPhaseGradients]"                         << endl;
//   cout << " - frequencies       = " << frequencies.shape()      << endl;
//   cout << " - antenna positions = " << antennaPositions.shape() << endl;
//   cout << " - antenna positions = " << antennaPositions << endl;
//   cout << " - directions        = " << directions.shape()       << endl;

  try {
    Beamformer::setPhaseGradients (frequencies,
				   directions,
				   antennaPositions);
  } catch (AipsError x) {
    cerr << "[Skymap::setPhaseGradients] " << x.getMesg() << endl;
  }

  // Set the shape of SkymapComplex
  Skymap::setSkymapShape ();
}

// ------------------------------------------------------------------ addAntennas

void Skymap::addAntennas (Matrix<DComplex> const &data)
{
  Matrix<DComplex> skymap;
  
  // pass the FFT'ed antennas data to the beamformer
  Beamformer::addAntennas (skymap,
			   data);
  
  // Adjust the skymap to the selected signal domain
  Skymap::adjustSignalDomain (skymap);

  // book-keeping
  ++nofAddedBlocks_p;
}

// ----------------------------------------------------------------- addBaselines

void Skymap::addBaselines (Matrix<DComplex> const &data)
{
  // call the beamformer
  Beamformer::BFaddBaselines (skymapComplex_p,
			      data);
  ++nofAddedBlocks_p;
  // set the beamformer flag
  addBaselines_p = true;
}

// ----------------------------------------------------------------------- ccBeam

void Skymap::ccBeam (Matrix<DComplex> const &data)
{
  // call the beamformer
  Beamformer::crossCorrBeam (skymapReal_p,
			     data,
			     blocksize_p);
  
  ++nofAddedBlocks_p;
  
  // set the beamformer flag
  addBaselines_p = false;
}

// --------------------------------------------------------------------- ccMatrix

// void Skymap::ccMatrix (const Cube<DComplex>& ccCube)
// {
//   Vector<double> frequencies = DataFrequency::Frequencies(true);
//   Skymap::ccMatrix (ccCube, frequencies);
// }

// --------------------------------------------------------------------- ccMatrix

void Skymap::ccMatrix (const Cube<DComplex>& ccCube,
		       const Vector<double>& frequencies,
		       const Matrix<double>& antennaPositions)
{
  int BFmethod (1);
  Matrix<double> skymap; 

  if (BFmethod == 1) {
    Matrix<double> directions (Skymap::beamformingGrid());

    // Pass the data to the Beamformer
    Beamformer::BFccMatrix (skymap,
			    ccCube,
			    frequencies,
			    directions,
			    antennaPositions);
  }
  else if (BFmethod == 2) {
    Beamformer::BFccMatrix (skymap,
			    ccCube);
  }

  /* Beamforming method independent computations */

  skymapReal_p.resize(skymap.shape());
  skymapReal_p = skymap;
  
  // book-keeping
  ++nofAddedBlocks_p;  
}


// =============================================================================
//
//  Access to the Skymap data itself
//
// =============================================================================

// ------------------------------------------------------------------------ shape

IPosition Skymap::shape ()
{
  IPosition skymapShape;
  if (skymapComplex_p.nelements() > 1) {
    skymapShape = skymapComplex_p.shape();
  }
  else if (skymapReal_p.nelements() > 1) {
    skymapShape = skymapReal_p.shape();
  }

  return skymapShape;
}

// ------------------------------------------------------------------ clearSkymap

void Skymap::clearSkymap ()
{
  skymapComplex_p = DComplex(0.0,0.0);
  skymapReal_p    = 0.0;

  // reset the beamformer flags
  addBaselines_p = crossCorrelation_p = false;
  // reset the counter for the number of added blocks
  nofAddedBlocks_p = 0;
}


// =============================================================================
// 
//  Return the computed skymap.
// 
// =============================================================================

// -------------------------------------------------------------------- getSkymap

Matrix<DComplex> Skymap::getSkymap ()
{
  return skymapComplex_p/nofAddedBlocks_p;
}

// -------------------------------------------------------------------- getSkymap

void Skymap::getSkymap (Matrix<DComplex>& skymap,
			int channels)
{
  int nx,ny;
  // perform the rebinning of on the third image axis ...
  skymapComplex_p.shape(nx,ny);
  if (nx != channels) {
    Skymap::averageChannels (skymapComplex_p,channels);
  }
  // ... and normize by the number of processed data blocks
  skymap.resize(skymapComplex_p.shape());
  skymap = skymapComplex_p/nofAddedBlocks_p;
}

// -------------------------------------------------------------------- getSkymap

void Skymap::getSkymap (Matrix<double>& skymap)
{
  double blocks = 1.0*nofAddedBlocks_p;
  skymap.resize(skymapReal_p.shape());
  skymap = skymapReal_p/blocks;
}

// -------------------------------------------------------------------- getSkymap

void Skymap::getSkymap (Matrix<double>& skymap,
			int channels)
{
  IPosition shape (skymapComplex_p.shape());
  double blocks = 1.0*nofAddedBlocks_p;

  // perform the rebinning of on the third image axis ...
  if (shape(0) != channels) {
    Skymap::averageChannels (skymapComplex_p,
			     channels);
  }
  // ... and normize by the number of processed data blocks
  shape = skymapComplex_p.shape();
  skymap.resize(shape);
  skymap = skymapReal_p/blocks;
}

// -------------------------------------------------------------------- getSkymap

void Skymap::getSkymap (Cube<DComplex>& skymap,
			Cube<bool>& mask)
{
  int nfreq(0);
  int pixels (0);
  //
  skymapComplex_p.shape(nfreq,pixels);
  Skymap::skymap (skymap,mask,nfreq);
}

// ----------------------------------------------------------------------- skymap

void Skymap::skymap (Cube<DComplex>& skymap,
		     Cube<bool>& mask,
		     int channels)
{
  int nofPixels (0);
  int nofChannels (0);

  // -----------------------------------------------------------------
  // Get the image mask in the shape matching that of the skymap data
  // cube; since pixel array and masking array have the same shape,
  // we use the mask to resize the pixel array.

  mask = Skymap::mask (channels);

  skymap.resize(mask.shape());
  skymap = 0.0;

  // -----------------------------------------------------------------
  // Check wether of not the number of channels per image pixel is
  // modified on output; if yes, then we need to rebin first

  skymapComplex_p.shape(nofChannels,nofPixels);

  if (nofChannels != channels) {
    Matrix<DComplex> skymapTemp;
    //
    Skymap::averageChannels (skymapTemp,channels);
    //
    Skymap::skymapMatrix2Cube (skymap,skymapTemp);
  }
  else {
    Skymap::skymapMatrix2Cube (skymap,skymapComplex_p);
  } 

}

// ----------------------------------------------------------------------- skymap

void Skymap::skymap (Cube<double>& skymap,
		     Cube<bool>& mask)
{
  int nofChannels (0);
  int nofPixels (0);
  //
  skymapComplex_p.shape(nofChannels,
			nofPixels);
  //
  Skymap::skymap (skymap,
		  mask,
		  nofChannels);
}

// ----------------------------------------------------------------------- skymap

void Skymap::skymap (Cube<double>& skymap,
		     Cube<bool>& mask,
		     int channels)
{
  int nofPixels (0);
  int nofChannels (0);

  // -----------------------------------------------------------------
  // Get the image mask in the shape matching that of the skymap data
  // cube; since pixel array and masking array have the same shape,
  // we use the mask to resize the pixel array.

  mask = Skymap::mask (channels);

  skymap.resize(mask.shape());
  skymap = 0.0;

  // -----------------------------------------------------------------
  // Check wether of not the number of channels per image pixel is
  // modified on output; if yes, then we need to rebin first

  skymapReal_p.shape(nofChannels,nofPixels);

  if (nofChannels != channels) {
    Matrix<double> skymapTemp;
    //
    Skymap::averageChannels (skymapTemp,
			     channels);
    //
    Skymap::skymapMatrix2Cube (skymap,
			       skymapTemp);
  }
  else {
    Skymap::skymapMatrix2Cube (skymap,
			       skymapReal_p);
  } 

}

// ------------------------------------------------------------ skymapMatrix2Cube

template <class T>
void Skymap::skymapMatrix2Cube (Cube<T>& out,
				const Matrix<T>& skymapIN)
{
  bool verboseON (false);
  IPosition shape (grid_p.shape());
  Vector<bool> mask (grid_p.mask());
  int nofPixels (0);                  // number of map pixels
  int nofChannels (0);                // number of map channels

  // Get the shape of the internal skymap pixel value array
  skymapIN.shape(nofChannels,
		 nofPixels);

  // Check the number of skymap pixels masked as valid.
  if (nofPixels != grid_p.nelements(true)) {
    cerr << "[Skymap::skymapMatrix2Cube] Mismatch in number of valid pixels" << endl;
    cerr << " - Skymap pixel value array : " << nofPixels                    << endl;
    cerr << " - Pixel masking array .... : " << grid_p.nelements(true)       << endl;
  }

  /*
    Fill the pixels values into the data cibe returned; this also signifies, that
    we have to adjust the ordering of the axes.
   */
  int az      (0);
  int el      (0);
  int pixel   (0);
  int nMask   (0);
  int channel (0);

  for (az=0; az<shape(0); az++) {
    for (el=0; el<shape(1); el++) {
      //
      if (mask(nMask) == true) {
	// copy channels
	for (channel=0; channel<nofChannels; channel++) {
	  out(az,el,channel) = skymapIN(channel,pixel)/T(nofAddedBlocks_p);
	}
	// increment pixel counter
	pixel++;
      }
      // keep track of the mask
      nMask++;
      //
    }
  }
  
  if (verboseON) {
    cout << "[Skymap::skymapMatrix2Cube]" << endl;
    cout << " - Shape of input array ........ : " << skymapIN.shape() << endl;
    cout << " - Shape of output array ....... : " << out.shape() << endl;
    cout << " - Fraction of valid pixels .... : " << grid_p.nelements(true)
	 << " / " << grid_p.nelements() << endl;
    cout << " - Number of added blocks ...... : " << nofAddedBlocks_p << endl;
  }
}

// ------------------------------------------------------------------- dumpSkymap

void Skymap::dumpSkymap (std::ostream& os = std::cout)
{
  IPosition skymapShape(2);
  int pixel (0);

  if (signalDomain_p == "Frequency") {
    // get the shape of the internal data array
    skymapShape = skymapComplex_p.shape();
    // write contents of the data array to the output stream
    for (pixel=0; pixel<skymapShape(1); ++pixel) {
      os << skymapComplex_p.column(pixel) << endl;
    }
  }
  else if (signalDomain_p == "Time") {
    // get the shape of the internal data array
    skymapShape = skymapReal_p.shape();
    // write contents of the data array to the output stream
    for (pixel=0; pixel<skymapShape(1); ++pixel) {
      os << skymapReal_p.column(pixel) << endl;
    }
  }
}


// =============================================================================
// 
//  Averaging along the time/frequency axis
// 
// =============================================================================

// -------------------------------------------------------------- averageChannels

template <class T>
void Skymap::averageChannels (Vector<T>& channels,
			      int nofChannels)
{
  int nofBins (0);
  T sum (0.0);
  Matrix<int> indexValues;
  Vector<T> tmpChannels (nofChannels);

  // get the array indices defining the rebinning intervals
  Skymap::getRebinningIndices (indexValues,nofChannels);

  for (int chan=0; chan<nofChannels; ++chan) {
    sum = 0.0;
    nofBins = 0;
    for (int freq=indexValues(chan,0); freq<=indexValues(chan,1); ++freq) {
      sum += channels(freq);
      ++nofBins;
    }
    tmpChannels(chan) = sum/T(nofBins);
  }

  // rearrange arrays for output
  channels.resize(nofChannels);
  channels = tmpChannels;
  
}

// -------------------------------------------------------------- averageChannels

template <class T>
void Skymap::averageChannels (Matrix<T>& skymap,
			      int const &channels)
{
  int nofFrequencies (0);
  int nofPixels (0);
  T nofBins (0);
  T sum (0.0);
  Matrix<int> indexValues;

  // get the array indices defining the rebinning intervals
  Skymap::getRebinningIndices (indexValues,channels);

  // get the original shape of the skymap to properly resize the temporary array
  skymapReal_p.shape(nofFrequencies,nofPixels);
  skymap.resize(channels,nofPixels);

  int chan  (0);
  int pixel (0);
  int freq  (0);

  for (pixel=0; pixel<nofPixels; ++pixel) {
    // for each image pixel loop through the returned number of channels
    for (chan=0; chan<channels; ++chan) {
      sum = 0.0;
      nofBins = T(0);
      for (freq=indexValues(chan,0); freq<=indexValues(chan,1); ++freq) {
	sum += skymapReal_p(freq,pixel);
	nofBins+=T(1);
      }
      skymap(chan,pixel) = sum/nofBins;
    }
  }

}


// =============================================================================
//
//  Output of member data
//
// =============================================================================

void Skymap::printSkymap (std::ostream& s = std::cout)
{
  Time logTime;
  s << "\n[" << logTime << "] [Skymap]\n" << endl;
  s << " - Electrical quantity ................ : " << signalQuantity_p << endl;
  s << " - Domain of the electrical quantity .. : " << signalDomain_p << endl;
  s << " - Samples per block of data .......... : " << blocksize_p << endl;
  s << " - Number of datablocks per timeframe . : " << nofAddedBlocks_p
    << endl;
  s << " - Shape of the real value data cube .. : " << skymapReal_p.shape()
    << endl;
  s << " - Shape of the complex value data cube : " << skymapComplex_p.shape()
    << endl;
  s << " - Image world axis names ............. : " << csys_p.worldAxisNames()
    << endl;
  s << " - Number of image world axes ......... : " << csys_p.nWorldAxes()
    << endl;
  s << " - Number of image pixel axes ......... : " << csys_p.nPixelAxes()
    << endl;
  s << " - Image coordinate increment ......... : " << csys_p.increment()
    << endl;

  // print the contents of the embedded SkymapGrid object
  grid_p.show(s);

  // print the contents of the base classes
  Beamformer::printBeamformer(s);

}


// =============================================================================
//
//  Template instantiations
//
// =============================================================================

template void Skymap::averageChannels (Vector<float>& channels,
				       int nofChannels);
template void Skymap::averageChannels (Vector<double>& channels,
				       int nofChannels);
template void Skymap::averageChannels (Vector<Complex>& channels,
				       int nofChannels);
template void Skymap::averageChannels (Vector<DComplex>& channels,
				       int nofChannels);

template void Skymap::averageChannels (Matrix<float> &skymap,
				       int const &channels);
template void Skymap::averageChannels (Matrix<double> &skymap,
				       int const &channels);
template void Skymap::averageChannels (Matrix<Complex> &skymap,
				       int const &channels);
template void Skymap::averageChannels (Matrix<DComplex> &skymap,
				       int const &channels);

template void Skymap::skymapMatrix2Cube (Cube<float> &out,
					 const Matrix<float>& skymapIN);
template void Skymap::skymapMatrix2Cube (Cube<double> &out,
					 const Matrix<double>& skymapIN);
template void Skymap::skymapMatrix2Cube (Cube<Complex> &out,
					 const Matrix<Complex>& skymapIN);
template void Skymap::skymapMatrix2Cube (Cube<DComplex> &out,
					 const Matrix<DComplex>& skymapIN);

}  // namespace LOPES -- end
