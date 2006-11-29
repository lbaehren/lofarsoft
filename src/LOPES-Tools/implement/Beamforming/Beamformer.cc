/***************************************************************************
 *   Copyright (C) 2005                                                    *
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

/* $Id: Beamformer.cc,v 1.5 2006/10/31 18:24:08 bahren Exp $*/

// C++ Standard library
#include <iostream>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;

// custom C++ classes
#include <Beamforming/Beamformer.h>
#include <Beamforming/ccBeam.h>

namespace LOPES {  // namespace LOPES -- begin
  
  // ==============================================================================
  //
  //  Construction / Deconstruction
  //
  // ==============================================================================
  
  // ------------------------------------------------------------------- Beamformer
  
  Beamformer::Beamformer ()
    : DataFrequency()
  {
    Beamformer::initBeamformer ();
  }
  
  // ------------------------------------------------------------------- Beamformer
  
  Beamformer::Beamformer (Vector<double> frequencies,
			  Matrix<double> directions,
			  Matrix<double> posAntennas)
    : DataFrequency()
  {
    Beamformer::initBeamformer ();
    Beamformer::setPhaseGradients (frequencies,
				   directions,
				   posAntennas);
  }
  
  // ------------------------------------------------------------------- Beamformer
  
  Beamformer::Beamformer (Vector<double> frequencies,
			  Cube<double> directions,
			  Matrix<double> posAntennas)
    : DataFrequency()
  {
    Beamformer::initBeamformer ();
    Beamformer::setPhaseGradients (frequencies,
				   directions,
				   posAntennas);
  }
  
  // ------------------------------------------------------------------ ~Beamformer
  
  Beamformer::~Beamformer ()
  {
    bool deleteIt (true);
    const DComplex* ptrPhaseGradients = weights_p.getStorage (deleteIt);
    weights_p.freeStorage(ptrPhaseGradients,deleteIt);
  }
  
  // ==============================================================================
  //
  //  Initialization
  //
  // ==============================================================================
  
  // --------------------------------------------------------------- initBeamformer
  
  void Beamformer::initBeamformer ()
  {
    // default sizes
    inputShape_p.resize(1);
    // default values for the meta data
    inputShape_p   = 0;
    inputWasCube_p = false;
    
    // Beamformer weights,  [freq,pixel,antenna]
    shape_p = IPosition (3,1,1,1);
    weights_p.resize (shape_p);
    weights_p = 1.0;
    
    // verbose mode is switched off by default
    Beamformer::showProgress(false);
  }
  
  // ---------------------------------------------------------------- setBeamformer
  
  void Beamformer::setBeamformer (GlishRecord& record)
  {
    GlishArray ary;
    String str;
    
    // forward record to base class(es)
    try {
      DataFrequency::setDataFrequency (record);
    } catch (AipsError x) {
      cerr << "[Beamformer::setBeamformer] " << x.getMesg() << endl;
    }
    
    if (record.exists("bfMethod")) {
      ary = record.get("bfMethod");
      ary.get(str);
      Beamformer::BFMethod(str);
    }
    
  }
  
  // --------------------------------------------------------------------- BFMethod
  
  void Beamformer::BFMethod (String const &bfMethod)
  {
    BFMethod_p = bfMethod;
  }
  
  
  // =============================================================================
  //
  //  Metadata for the phase gradients array
  //
  // =============================================================================
  
  void Beamformer::setInputShape (const Array<double>& directions)
  {
    inputShape_p = directions.shape();
  }
  
  // ------------------------------------------------------------ getGradientsShape
  
  void Beamformer::getGradientsShape (int& nofFrequencies,
				      int& nofPixels,
				      int& nofAntennas)
  {
    nofFrequencies = shape_p(0);
    nofPixels      = shape_p(1);
    nofAntennas    = shape_p(2);
  }
  
  // ---------------------------------------------------------- setAntennaPositions
  
  void Beamformer::setAntennaPositions (Matrix<double> const &antennaPositions,
					bool const &setWeights)
  {
    // get the shape of the array, [nofAntennas,3]
    IPosition shape (antennaPositions.shape());
    
    // Check if we really have received 3D position information
    if (shape(1) <3) {
      std::cerr << "[Beamformer::setAntennaPositions] Incomplete positions"
		<< std::endl;
      std::cerr << " - shape(antennaPositions) = " << shape << std::endl;
      return;
    }
    
    // if the shape was ok, we store the provided input data
    antennaPositions_p.resize(shape);
    antennaPositions_p = antennaPositions;
    // ... and update the shape information for the beamformer weights
    shape_p(2)         = shape(0);
    
    // update the beamformer weights?
    if (setWeights) {
    }
  }
  
  // ----------------------------------------------------------- setFrequencyValues
  
  void Beamformer::setFrequencyValues (Vector<double> const &frequencyValues,
				       bool const &setWeights)
  {
    IPosition shape (frequencyValues.shape());
    
    frequencyValues_p.resize(shape);
    frequencyValues_p = frequencyValues;
    
    shape_p(0)  = shape(0);
    
    // update the beamformer weights?
    if (setWeights) {
    }
  }
  
  // ==============================================================================
  //
  //  Phase gradient array with the weighting factors for the beamforming
  //
  // ==============================================================================
  
  // ------------------------------------------------------------ setPhaseGradients
  
  void Beamformer::setPhaseGradients (const Vector<double>& frequencies,
				      const Vector<double>& direction,
				      const Matrix<double>& antennaPositions)
  {
    Matrix<double> coords (1,direction.nelements());
    coords.row(0) = direction;
    
    // Set the input indication flag
    inputWasCube_p = false;
    
    // call the main method for setting the phase gradients
    Beamformer::setPhaseGradients (frequencies,
				   coords,
				   antennaPositions);
  }
  
  // ------------------------------------------------------------ setPhaseGradients
  
  void Beamformer::setPhaseGradients (const Vector<double>& frequencies,
				      const Matrix<double>& directions,
				      const Matrix<double>& antennaPositions)
  {
    // Set the input indication flag
    inputWasCube_p = false;
    // Store the frequency values
    try {
      setFrequencyValues (frequencies,
			  false);
    } catch (AipsError x) {
      cerr << "[Beamformer::setPhaseGradients] Failed setting frequencies."
	   << x.getMesg() << endl;
    }
    // Store the antenna positions
    try {
      setAntennaPositions (antennaPositions,
			   false);
    } catch (AipsError x) {
      cerr << "[Beamformer::setPhaseGradients] Failed setting antenna positions."
	   << x.getMesg() << endl;
    }
    
    // Call the main computation routine
    Beamformer::calcPhaseGradients (frequencies,
				    directions);
  }
  
  // ------------------------------------------------------------ setPhaseGradients
  
  void Beamformer::setPhaseGradients (const Vector<double>& frequencies,
				      const Cube<double>& directions,
				      const Matrix<double>& antennaPositions)
  {
    IPosition coordsShape (directions.shape());
    Matrix<double> coordinates;
    
    // Set the input indication flag
    inputWasCube_p = true;
    
    // Resize the Matrix passed to the computation routine
    coordinates.resize(coordsShape(0)*coordsShape(1),
		       coordsShape(2));
    
    // Fill the values from the input cube into the matrix 
    int i,j,k;
    int n = 0;

  for (i=0; i<coordsShape(0); i++) {
      for (j=0; j<coordsShape(1); j++) {
	for (k=0; k<coordsShape(2); k++) {
	  coordinates(n,k) = directions(i,j,k);
	  ++n;
	}
      }
    }
    
    // call the main method for setting the phase gradients
    Beamformer::setPhaseGradients (frequencies,
				   coordinates,
				   antennaPositions);
  }
  
// ------------------------------------------------------------ getPhaseGradients

void Beamformer::getPhaseGradients (Vector<DComplex>& phaseGrad,
				    int pixel,
				    int antenna)
{
  // resize the returned vector based on the stored shape information
  phaseGrad.resize(shape_p(0));

  // Set up a slicer to extract data cube elements parallel to the x-axis
  IPosition start(3,0,pixel,antenna);
  IPosition end(3,shape_p(0)-1,pixel,antenna);
  IPosition stride(3,1,0,0);
  Slicer slicer (start, end, stride, Slicer::endIsLast);

  phaseGrad = weights_p(slicer);
}

// ----------------------------------------------------------- calcPhaseGradients

void Beamformer::calcPhaseGradients (const Vector<double>& frequencies,
				     const Matrix<double>& directions)
{
  uint nfreq (frequencies.nelements());
  IPosition shapeDir (directions.shape());
  IPosition shapeAntpos (antennaPositions_p.shape());

  Vector<double> antpos(3); 
  antpos = 0.0;

  // ... to properly resize the internal data array
  if (showProgress_p) {
    cout << "[Beamformer::calcPhaseGradients]" << endl;
    cout << " - Allocating memory for phase gradients array of shape ["
	 << nfreq << "," << shapeDir(0) << "," << shapeAntpos(0) << "]" << endl;
  }
  // resize the array storing the beamformer weights ...
  weights_p.resize(nfreq,shapeDir(0),shapeAntpos(0));
  // ... and buffer the shape of this array
  shape_p = weights_p.shape();
  
  ProgressBar bar = ProgressBar (shapeAntpos(0),70,"=");

  Matrix<DComplex> gradients (nfreq,shapeDir(0));

  for (int ant=0; ant<shapeAntpos(0); ++ant) {
    // extract the antenna position
    antpos = antennaPositions_p.row(ant);
    // compute the pase gradients for all frequencies and sky positions
    // -> Phasing::phaseGradient(Vector,Matrix,Vector)
    gradients = Phasing::phaseGradient(frequencies,
				       directions,
				       antpos);
//     // ---------------------------------------------------------------
//     // Debugging output
//     int pix = shapeDir(0)/2;
//     cout << "[Beamformer::calcPhaseGradients]" << endl;
//     cout << " Antenna number   : " << ant << " (" << ant+1 << ")" << endl;
//     cout << " Antenna position : " << antpos << endl;
//     cout << pix << "\t" << directions.row(pix) << "\t"
// 	 << Phasing::geometricalDelay(directions.row(pix),antpos) << "\t"
// 	 << gradients.column(pix) << endl;
//     // ---------------------------------------------------------------
    // copy the computation results to internal storage
    weights_p.xyPlane(ant) = gradients;
    // show progress
    if (showProgress_p) bar.update (ant+1);
  }
  
  if (showProgress_p) cout << endl;
}

// ----------------------------------------------------------- calcPhaseGradients

void Beamformer::calcPhaseGradients (Matrix<double>& directions,
				     Matrix<double>& antennaPositions)
{
  Vector<double> frequencies = DataFrequency::Frequencies(true);

  Beamformer::calcPhaseGradients (frequencies,
				  directions);
}

// --------------------------------------------------------------- phaseGradients

void Beamformer::phaseGradients (std::ostream& s = std::cout)
{
  int ant (0);
  int pixel (0);
  int freq (0);

  cout << "Exporting phase gradients ... " << flush;
  
  for (ant=0; ant<shape_p(2); ++ant) {
    for (pixel=0; pixel<shape_p(1); ++pixel) {
      for (freq=0; freq<shape_p(0); ++freq) {
	s << ant << "\t" << pixel << "\t" << freq << "\t" 
	  << weights_p(freq,pixel,ant) << endl;
      }
    }
  }
  
  cout << "Done." << endl;
}


// =============================================================================
//
//  Beamforming routines
//
// =============================================================================

// [1] Adding of the phase shifted antenna signals

void Beamformer::addAntennas (Matrix<DComplex>& skymap,
			      const Matrix<DComplex>& data)
{
  bool verboseON (false);
  IPosition shapeSignals (data.shape());      // [freq,ant]
  IPosition shapeGradients (weights_p.shape());  // [freq,pixel,ant]
  DComplex antennas = DComplex(shapeSignals(1),0.0);
  
  skymap.resize(shapeSignals(0),shape_p(1));
  skymap = DComplex(0.0,0.0);

  // -----------------------------------------------------------------
  // Set up IPosition object for accessing the arrays

//   IPosition skymapBLC (skymap.shape());
//   IPosition skymapTRC (skymapBLC);
//   IPosition phasegradBLC (shapeGradients);
//   IPosition phasegradTRC (shapeGradients);
//   IPosition signalsBLC (shapeSignals);
//   IPosition signalsTRC (shapeSignals);

  // -----------------------------------------------------------------
  // Provide some feedback (if enabled)

  if (verboseON) {
    cout << "[Beamformer::addAntennas]" << endl;
    cout << " Shape of skymap array     : " << skymap.shape() << endl;
    cout << " Shape of input data array : " << shapeSignals << endl;
    cout << " Shape of gradients array  : " << shapeGradients << endl;
    cout << " Number of pixels ........ : " << shape_p(1) << endl;
    cout << " Number of antennas ...... : " << shape_p(2) << endl;
  }
 
  // -----------------------------------------------------------------
  // Carry out computation

  if (showProgress_p) {
    cout << "[Beamformer::addAntennas]" << endl;
  }

  ProgressBar bar = ProgressBar (shape_p(2),70,"#");
  int ant (0);
  int pixel (0);
  int freq (0);

  for (ant=0; ant<shapeSignals(1); ++ant) {
    //
//     signalsBLC(1) = signalsTRC(1) = ant;
//     phasegradBLC(2) = phasegradTRC(2) = ant;
    //
    for (pixel=0; pixel<shape_p(1); ++pixel) {
      //
//       skymapBLC(1) = skymapTRC(1) = pixel;
//       phasegradBLC(1) = phasegradTRC(1) = pixel;
      //
      for (freq=0; freq<shapeSignals(0); ++freq) {
	skymap(freq,pixel) += data(freq,ant)*weights_p(freq,pixel,ant);
      }
//       // -------------------------------------------------------------
//       // Debugging output
//       if (pixel == shapeGradients(1)/2) {
// 	cout << "[Beamformer::addAntennas]" << endl;
// 	cout << " Antenna : " << ant << " (" << ant+1 << ")" << endl;
// 	cout << " Pixel   : " << pixel << "/" << shape_p(1) << endl;
// 	//
// 	cout << " Weights : [";
// 	for (int freq=0; freq<shapeSignals(0); ++freq) {
// 	  cout << " " << weights_p(freq,pixel,ant);
// 	}
// 	cout << " ]" << endl;
// 	// 
// 	cout << " Input   : " << data.column(ant) << endl;
// 	//
// 	cout << " Skymap  : " << skymap.column(pixel) << endl;
// 	//
// 	cout << " # ant   : " << antennas << endl;
//       }
//       // -------------------------------------------------------------
    }
    if (showProgress_p) {
      bar.update (ant+1);
    }
  }

  // mormalize by the number of antennas
  skymap /= antennas;
  
}

// --------------------------------------------------------------- BFaddBaselines

void Beamformer::BFaddBaselines (Matrix<DComplex>& skymap,
				 const Matrix<DComplex>& data)
{
  DComplex baselines = Beamformer::nofBaselines();
  Vector<DComplex> beam;
  DComplex product;

  beam.resize(shape_p(0));

  for (int pixel=0; pixel<shape_p(1); ++pixel) {
    // initialize the beam vector
    beam = DComplex(0.0,0.0);
    for (int i=0; i<shape_p(2); ++i) {
      for (int j=i+1; j<shape_p(2); ++j) {
	for (int freq=0; freq<shape_p(0); ++freq) {
	  product = data(freq,i)*weights_p(freq,pixel,i);
	  product *= conj(data(freq,j)*weights_p(freq,pixel,j));
	  beam(freq) += product;
	}
      }
    }

    // normalization by the number of baselines
    beam /= baselines;

    // copy the beam to the returned array
    for (int freq=0; freq<shape_p(0); ++freq) {
      skymap(freq,pixel) = casa::fabs(beam(freq));
    }
  }  

}

// ------------------------------------------------------------- crossCorrelation

void Beamformer::BFcrossCorrelation (Matrix<DComplex>& skymap,
				     Matrix<DComplex>& data)
{
  int nfreq,nant,ncorr;
  Vector<DComplex> dataI;

  // adjust the shape of the arrays
  data.shape(nfreq,nant);
  dataI.resize(nfreq);
  
  // for bookkeeping: get the number of cross-correlations
  ncorr = nant*(nant-1)/2; 
  
  for (int i=0; i<nant; ++i) {
    dataI = data.row(i);
    for (int j=i; j<nant; ++j) {
      Beamformer::crossCorrelation (dataI,data.row(j));
    }
  }
 
}

// ------------------------------------------------------------- crossCorrelation

void Beamformer::crossCorrelation (Vector<DComplex>& DataI,
				   Vector<DComplex> DataJ)
{
  int nk (DataI.nelements());
  Vector<DComplex> conjDataI (nk);

  conjDataI = conj(DataI);
  DataI = 0.0;

  for (int k=0; k<nk; ++k) {
    for (int m=0; m<nk; ++m) {
      DataI(k) += conjDataI(m)*DataJ(k-m);
    }
  } 
  
}
  
  // ----------------------------------------------------------------------- ccBeam
  
  void Beamformer::crossCorrBeam (Matrix<double>& skymap,
				  const Matrix<DComplex>& data,
				  int const &blocksize)
  {
    int pixel (0);
    int freq (0);
    int antenna (0);
    ccBeam<double,DComplex> cc (blocksize);
    Matrix<DComplex> tmp (data.shape());
    
    skymap.resize (blocksize,shape_p(1));
    
    for (pixel=0; pixel<shape_p(1); pixel++) {
      // (1) multiply phase gradients to the data
      for (antenna=0; antenna<shape_p(2); antenna++) {
	for (freq=0; freq<shape_p(0); freq++) {
	  tmp (freq,antenna) = data (freq,antenna)*weights_p(freq,pixel,antenna);
	}
      }
      // (2) compute cc-beam
      skymap.column(pixel) = cc.ccbeam(tmp,
				       blocksize);
    }
  }
  
  // ------------------------------------------------------------------- BFccMatrix
  
  void Beamformer::BFccMatrix (Matrix<double>& skymap,
			       const Cube<DComplex>& ccMatrix)
  {
    int baselines (0);
    Vector<DComplex> beam;
    DComplex w;             // weighting factors
    int freq (0);
    int pixel (0);
    int i (0);
    int j (0);
    
    beam.resize(shape_p(0));
    baselines = Beamformer::nofBaselines();
    
    /* Allocate memory for the array taking up the skymap pixels and use the
       baseline number (used for normalization) as initial value. */
    skymap.resize(shape_p(0),shape_p(1));
    skymap = 1.0/baselines;
    
    cout << "[Beamformer::BFccMatrix] Generating map from cc'ed data ..." << endl;
    ProgressBar bar = ProgressBar (shape_p(1),70,"#");
    bar.showTime(true);
    
    for (pixel=0; pixel<shape_p(1); ++pixel) {
      // initialize the beam vector
      beam = DComplex(0.0,0.0);
      for (i=0; i<shape_p(2); ++i) {
	for (j=i+1; j<shape_p(2); ++j) {
	  for (freq=0; freq<shape_p(0); ++freq) {
	    w = weights_p(freq,pixel,i)*conj(weights_p(freq,pixel,j));
	    beam(freq) += w*ccMatrix(i,j,freq);
	  }
	}
      }
      bar.update (pixel+1);
      
      // copy the beam to the returned array
      for (freq=0; freq<shape_p(0); ++freq) {
	skymap(freq,pixel) *= casa::fabs(beam(freq));
      }
    }  
    
  }
  
  // ------------------------------------------------------------------- BFccMatrix
  
void Beamformer::BFccMatrix (Matrix<double>& skymap,
			     const Cube<DComplex>& ccCube,
			     const Vector<double>& frequencies,
			     Matrix<double>& directions,
			     const Matrix<double>& antennaPositions)
{
  IPosition ccShape (ccCube.shape());         // shape of the cc data cube
  IPosition coordsShape (directions.shape());  // shape of the directions array
  int nofPixels (0);
  int nofCoordinates (0);
  int nofBaselines (0);
  Matrix<double> azel(1,coordsShape(1));      // beam direction
  Vector<DComplex> beam;                       // individual station beam
  DComplex w;                                  // weighting factors

  // get array dimensions
  directions.shape(nofPixels,nofCoordinates);

  beam.resize(ccShape(2));
  nofBaselines = Beamformer::nofBaselines(ccShape(0));

  /* Allocate memory for the array taking up the skymap pixels and use the
     baseline number (used for normalization) as initial value. */
  skymap.resize(ccShape(2),nofPixels);
  skymap = 1.0/nofBaselines;

  // Store the antenna positions
  setAntennaPositions (antennaPositions);

  /* Provide some feedback before starting the computation */

  cout << "[Beamformer::BFccMatrix] Generating map from cc'ed data ..." << endl;
//   cout << " - Shape of the skymap ........ : " << skymap.shape() << endl;
//   cout << " - Shape of the cc data cube .. : " << ccShape << endl;
//   cout << " - Shape of the beam vector ... : " << beam.shape() << endl;
//   cout << " - Shape of the frequency array : " << frequencies.shape() << endl;
//   cout << " - Shape of the antenna array . : " << antennaPositions.shape() << endl;

  ProgressBar bar = ProgressBar (nofPixels,70,"#");
  bar.showTime(true);
  
  for (int pixel=0; pixel<nofPixels; pixel++) {
    // compute the weighting factors for a beam towards the current pixel
    azel.row(0) = directions.row(pixel);
    Beamformer::calcPhaseGradients (frequencies,azel);
    // initialize the beam vector
    beam = DComplex(0.0,0.0);
    for (int i=0; i<ccShape(0); i++) {
      for (int j=i+1; j<ccShape(1); j++) {
	for (int freq=0; freq<ccShape(2); freq++) {
	  w = weights_p(freq,0,i)*conj(weights_p(freq,0,j));
	  beam(freq) += w*ccCube(i,j,freq);
//  	  cout << freq
// 	       << "\t" << weights_p(freq,0,i)
// 	       << "\t" << conj(weights_p(freq,0,j))
// 	       << "\t" << w
// 	       << "\t" << ccCube(i,j,freq)
// 	       << "\t" << beam(freq) << endl;
	}
      }
    }
    
    // Copy the beam data into the array holdingthe skymap.
    for (int freq=0; freq<ccShape(2); freq++) {
      skymap(freq,pixel) *= casa::fabs(beam(freq));
    }
    
    if ((pixel%100)==0 || pixel == (nofPixels-1)) bar.update (pixel+1);
    
  } // end loop : pixel  

}

// ----------------------------------------------------------------- nofBaselines

int Beamformer::nofBaselines (const int nant)
{ 
  int baselines;

  /* Calculate the number of baseline combinations: 0.5*(N^2 - N), where N is the
     number of antennas. */
  baselines = int(0.5*(nant*nant-nant));

  return baselines;  
}


// =============================================================================
//
//  Output to a stream
//
// =============================================================================
  
  void Beamformer::summary()
  {
    printBeamformer (std::cout);
  }
  
  void Beamformer::printBeamformer (std::ostream &os)
  {
    os.precision(10);
    
    // output of base class(es)
    DataFrequency::show (os);
    
    os << "\n[Beamformer]\n" << endl;
    os << " - Beamforming method .................... : " << BFMethod_p
       << endl;
    os << " - Phase gradients array derived from cube : " << inputWasCube_p
       << endl;
    os << " - Shape of the input coordinates array .. : " << inputShape_p
       << endl;
    os << " - Shape of the phase gradients array .... : " << weights_p.shape()
       << endl;
    os << " - Verbose mode during computations? ..... : " << showProgress_p
       << endl;
  }
  
}  // namespace LOPES -- end
