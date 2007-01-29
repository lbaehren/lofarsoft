/***************************************************************************
 *   Copyright (C) 2004,2005                                               *
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

/* $Id: dynamicspectrum.cc,v 1.14 2006/10/31 18:37:08 bahren Exp $ */

/*!
  \file dynamicspectrum.cc
*/

#include <dynamicspectrum.h>
#include <lopes/Utilities/ParameterFromRecord.h>

// --- global variables --------------------------------------------------------

ClientData *clientData = new ClientData ();

// --- Main routine ------------------------------------------------------------

int main (int argc, char **argv)
{  

  GlishSysEventSource glishStream(argc, argv);
  
  glishStream.addTarget(initBeamformer_event, "initBeamformer");
  glishStream.addTarget(computeSpectra_event, "computeSpectra");
  glishStream.addTarget(processDatablock_event, "processDatablock");
  glishStream.addTarget(crosscorrDynspec, "crosscorrDynspec");
  glishStream.addTarget(cmsFrequency, "cmsFrequency");
  glishStream.addTarget(paramExtraction_event, "paramExtraction");
  glishStream.loop();

}

// ---------------------------------------------------------------- logStatistics

void logStatistics (Vector<Double>& dynspec)
{
  Time logTime;
  ofstream logfile;
  String logfileName ("DynamicSpectrum.log");

  logfile.open(logfileName.c_str(),ios::app);
  logfile << clientData->nBlock << "\t" << min(dynspec) << "\t" << max(dynspec);
  logfile << "\t" << mean(dynspec) << "\t" << variance(dynspec) << endl;
  logfile.close();
}

// ----------------------------------------------------------------- AntennaPower

void calcAntennaPower (Vector<Double>& power,
		       const Matrix<DComplex>& fft)
{
  IPosition shape (fft.shape());
  Int ant(0);
  Int freq (0);

  power.resize(shape(1));
  power = 0.0;

  try {
    for (ant=0; ant<shape(1); ant++) {
      // sum over all frequency channels for this antenna
      // --- (a) --- explicit looping over frequency channels ---
      for (freq=0; freq<shape(0); freq++) {
	power(ant) += abs(fft(freq,ant));
      }
      // --- (b) --- use array mathematics ---
//       power(ant) = real(abs(fft.column(ant)));
    }
  } catch (AipsError x) {
    cerr << "[dynamicspectrum::AntennaPower] " << x.getMesg() << endl;
  }
}

// ---------------------------------------------------------- adjustBeamDirection

Vector<Double> adjustBeamDirection (ObservationData& obsData,
				    const String& refcode,
				    const Vector<Double>& direction)
{
  Vector<Double> beamDirection (direction.shape());
  try {
    MVDirection MVDirectionFROM;
    Vector<Quantity> QDirectionTO(2);
    //
    MDirection::Convert conv = obsData.conversionEngine ("AZEL",
							 refcode);
    //
    MVDirectionFROM = MVDirection (Quantity(direction(0),"deg"),
				   Quantity(direction(1),"deg"));
    //
    QDirectionTO = conv(MVDirectionFROM).getValue().getRecordValue();
    //
    beamDirection(0) = QDirectionTO(0).getValue("deg");
    beamDirection(1) = QDirectionTO(1).getValue("deg"); 
  } catch (AipsError x) {
    cerr << "[DynamicSpectrum::beamDirection] " << x.getMesg() << endl;
  }

  return beamDirection;
}

// ---------------------------------------------------------------- calcMeanPower

void calcMeanPower (Vector<Double>& spectrum,
		    const Matrix<DComplex>& dataFx) 
{
  IPosition shape (dataFx.shape());
  Vector<Double> fftPower;

  // -----------------------------------------------------------------
  // Convert raw/calibrated FFT data to power

  for (int ant=0; ant<shape(1); ant++) {
    fftPower = clientData->bf.FFT2Power(dataFx.column(ant));
    spectrum += fftPower;
  }

  // -----------------------------------------------------------------
  // [2] Convert power to noise temperature
  Vector<Double> noiseT = clientData->bf.Power2NoiseT(spectrum);

  // -----------------------------------------------------------------
  // [3] Nomalize with number of array elements
  Vector<Double> normalization(shape(0)); 
  normalization = 1.0/shape(1);
  spectrum = noiseT/normalization;

}

// --------------------------------------------------------- beam2DynamicSpectrum

void beam2DynamicSpectrum (Matrix<DComplex>& dynamicSpectrum,
			   Matrix<Double>& antennaPower,
			   DataReader *dr,
			   const Int& nofBlocks)
{
  Matrix<DComplex> data (dr->nofSelectedChannels(),dr->nofSelectedAntennas());
  Vector<Double> power;
  Matrix<DComplex> beam;

//   cout << "[beam2DynamicSpectrum]" << endl;
//   cout << " - shape(dynamicSpectrum) : " << dynamicSpectrum.shape() << endl;
//   cout << " - shape(antennaPower)    : " << antennaPower.shape() << endl;
//   cout << " - shape(phase gradients) : " << clientData->bf.getGradientsShape()
//        << endl;

  LOPES::ProgressBar bar (nofBlocks,70);
  bar.showTime(false);
  
  for (Int numBlock(0); numBlock<nofBlocks; numBlock++) {
    // Read data from disk
    data = dr->calfft();
    // Compute the beamformed spectrum
//     clientData->bf.addAntennas (beam,data);
    // Compute total power per antenna
    calcAntennaPower (power, data);
    // Fill beamed spectrum into dynamic spectrum
    dynamicSpectrum.column(numBlock) = beam.column(0);
    // Store antenna power levels
    antennaPower.column(numBlock) = power;
    // Increment block pointer
    try {
      dr->nextBlock();
    } catch (AipsError x) {
      cerr << "[DynamicSpectrum::beam2DynamicSpectrum]" << x.getMesg() << endl;
    }
    //
    bar.update(numBlock+1);
  }
}

// -------------------------------------------------------- power2DynamicSpectrum

void power2DynamicSpectrum (Matrix<DComplex>& dynamicSpectrum,
			    Matrix<Double>& antennaPower,
			    DataReader *dr)
{
  Int nofAntennas (dr->nofSelectedAntennas());
  Int nofFrequencies (dr->nofSelectedChannels());
  Int nofBlocks (0);
  Int nant (0);
  Matrix<DComplex> data;
  Vector<DComplex> sum;
  Vector<Double> power;

  cout << "[power2DynamicSpectrum]" << endl;

  // Get the shape of the arrays passed as function parameters

  dynamicSpectrum.shape(nofFrequencies,nofBlocks);
//   antennaPower.shape(nofAntennas,nofBlocks);
  sum.resize (nofFrequencies);
  
  // -- Feedback on parameter settings -------------------------------

  std::cout << " - DataReader settings :" << std::endl;
  std::cout << " -- nof. selected antennas = " << dr->nofSelectedAntennas()
	    << std::endl;
  std::cout << " -- nof. selected channels = " << dr->nofSelectedChannels()
	    << std::endl;

  std::cout << " - dynamicspectrum settings :" << std::endl;
  std::cout << " -- nof. blocks            = " << nofBlocks      << std::endl;
  std::cout << " -- shape(fft data)        = " << data.shape() << std::endl;
  std::cout << " -- shape(dynamicSpectrum) = " << dynamicSpectrum.shape() << std::endl;
  std::cout << " -- shape(antennaPower)    = " << antennaPower.shape() << std::endl;
  std::cout << " -- shape(sum)             = " << sum.shape() << std::endl;
  
  LOPES::ProgressBar bar (nofBlocks,70);
  bar.showTime(false);
  
  for (Int numBlock(0); numBlock<nofBlocks; numBlock++) {
    // (a) Total power per antenna
    data = dr->calfft();
    calcAntennaPower (power, data);
    antennaPower.column(numBlock) = power;
    // (b) Dynamic spectrum
    data *= conj(data);
    sum = 0.0;
    try{
      for (nant=0; nant<nofAntennas; nant++) {
	sum += data.column(nant);
      }
    } catch (AipsError x) {
      std::cerr << x.getMesg() << std::endl;
      std::cerr << " shape(data) = " << data.shape() << std::endl;
      std::cerr << " shape(sum)  = " << sum.shape()  << std::endl;
    }
    dynamicSpectrum.column(numBlock) = sum/nofAntennas;
    dr->nextBlock();
    //
    bar.update(numBlock+1);
  }
}

// --------------------------------------------------------- DynamicSpectrum2FITS

Bool DynamicSpectrum2FITS (const String& outfile,
			   const Matrix<DComplex>& dynamicSpectrum,
			   const Vector<Double>& crval,
			   const Vector<Double>& cdelt)
{
  cout << "[DynamicSpectrum] Exporting spectra to FITS" << endl;

  Bool ok (True);
  IPosition shape (dynamicSpectrum.shape());
  String filename;

  int status(0);
  int nTime(0);
  int nFreq(0);
  long fpixel (1);
  long naxis (2);
  long nelements (dynamicSpectrum.nelements());
  long naxes[2] = { shape(1), shape(0)};  // [Time,Freq]

  float pixels[shape(0)][shape(1)];       // [Freq,Time]

  // -----------------------------------------------------------------
  // (a) FITS image for the phase value

  {
    cout << " - FITS image for the phase value ..." << endl;
    
    // Set filename
    filename = "!" + outfile + "-arg.fits";
    
    // Create a new file
    fitsfile *fptr;
    fits_create_file (&fptr, filename.c_str(), &status);
    
    // Create the primary array image (16-bit short integer pixels 
    cout << " -- Creating primary image array ..." << endl;
    fits_create_img (fptr, FLOAT_IMG, naxis, naxes, &status);
    
    cout << " -- Extracting phase values from complex array ..." << endl;
    for (nTime = 0; nTime < shape(1); nTime++) {
      for (nFreq = 0; nFreq < shape(0); nFreq++) {
 	pixels[nFreq][nTime] = arg(dynamicSpectrum (nFreq,nTime));
      }
    }
    
    // Write the array of integers to the image
    cout << " -- Writing pixel values to file ..." << endl;
    fits_write_img(fptr, TFLOAT, fpixel, nelements, pixels[0], &status);
    
    // Add WCS keywords
    cout << " -- Adding WCS keywords ..." << endl;
    status = keywordsDynamicSpectrum (fptr,
				      crval,
				      cdelt);

    // Close the file
    fits_close_file(fptr, &status);

    // print out any error messages
    fits_report_error(stderr, status);
    status = 0;
  }

  // -----------------------------------------------------------------
  // (b) FITS image for the absolute value

  {
    cout << " - FITS image for the absolute value ..." << endl;
    
    // Set filename
    filename = "!" + outfile + "-abs.fits";
    
    // Create a new file
    fitsfile *fptr;
    fits_create_file (&fptr, filename.c_str(), &status);
    
    // Create the primary array image (16-bit short integer pixels 
    cout << " -- Creating primary image array ..." << endl;
    fits_create_img (fptr, FLOAT_IMG, naxis, naxes, &status);
    
    cout << " -- Extracting amplitude values from complex array ..." << endl;
    for (nTime = 0; nTime < shape(1); nTime++) {
      for (nFreq = 0; nFreq < shape(0); nFreq++) {
	pixels[nFreq][nTime] = abs(dynamicSpectrum (nFreq,nTime));
      }
    }
    
    // Write the array of integers to the image
    cout << " -- Writing pixel values to file ..." << endl;
    fits_write_img(fptr, TFLOAT, fpixel, nelements, pixels[0], &status);
    
    // Add WCS keywords
    cout << " -- Adding WCS keywords ..." << endl;
    status = keywordsDynamicSpectrum (fptr,
				      crval,
				      cdelt);    
    // Close the file
    fits_close_file(fptr, &status);
    
    // print out any error messages
    fits_report_error(stderr, status);
    status = 0;
  }
  
  // -----------------------------------------------------------------
  // (c) FITS image for the power; this can use the already extracted
  //     absolut value - it only needs to be squared.

  {
    cout << " - FITS image for the power ..." << endl;
    
    // Set filename
    filename = "!" + outfile + ".fits";

    // Create a new file
    fitsfile *fptr;
    fits_create_file (&fptr, filename.c_str(), &status);
    
    // Create the primary array image (16-bit short integer pixels 
    cout << " -- Creating primary image array ..." << endl;
    fits_create_img (fptr, FLOAT_IMG, naxis, naxes, &status);
    
    cout << " -- Computing power amplitude ..." << endl;
    for (nTime = 0; nTime < shape(1); nTime++) {
      for (nFreq = 0; nFreq < shape(0); nFreq++) {
 	pixels[nFreq][nTime] *= pixels[nFreq][nTime];
      }
    }
    
    // Write the array of integers to the image
    cout << " -- Writing pixel values to file ..." << endl;
    fits_write_img(fptr, TFLOAT, fpixel, nelements, pixels[0], &status);
    
    // Add WCS keywords
    cout << " -- Adding WCS keywords ..." << endl;
    status = keywordsDynamicSpectrum (fptr,
				      crval,
				      cdelt);

    // Close the file
    fits_close_file(fptr, &status);

    // print out any error messages
    fits_report_error(stderr, status);
  }

  if (status != 0) {  
    ok = False;
  }
  
  return ok;
}

// ------------------------------------------------------ keywordsDynamicSpectrum

Int keywordsDynamicSpectrum (fitsfile *fptr,
			     const Vector<Double>& crval,
			     const Vector<Double>& cdelt)
{
  Bool verbose (False);
  Int status(0);
  float keyvalue (0);
  char *ctype[] = {"Time", "FREQ"};
  char *cunit[] = {"s", "Hz"};
  float crpix (1.0);
  char *observer = LOPES::string2char(clientData->observer);
  char *telescope = LOPES::string2char(clientData->telescope);

  if (verbose) {
    cout << " --- Telescope : " << telescope << endl;
    cout << " --- Observer  : " << observer << endl;
    cout << " --- CTYPE     : [" << ctype[0] << ", " << ctype[1] << "]" << endl;
    cout << " --- CUNIT     : [" << cunit[0] << ", " << cunit[1] << "]" << endl;
    cout << " --- CRPIX     : [" << crpix << ", " << crpix << "]" << endl;
    cout << " --- CRVAL     : " << crval << endl;
    cout << " --- CDELT     : " << cdelt << endl;
  }

  // Telescope
  fits_update_key (fptr,
		   TSTRING,
		   "TELESCOP",
		   telescope,
		   "",
		   &status);
  // Instrument
  fits_update_key (fptr,
		   TSTRING,
		   "INSTRUME",
		   telescope,
		   "",
		   &status);
  // Observer
  fits_update_key (fptr,
		   TSTRING,
		   "OBSERVER",
		   observer,
		   "",
		   &status);
  
  // Coordinate type
  ffukys (fptr, "CTYPE1", ctype[0], "Frequency axis", &status);
  ffukys (fptr, "CTYPE2", ctype[1], "Time axis", &status);

   // Coordinate axis units

  ffukys (fptr, "CUNIT1", cunit[0], "Frequency axis units", &status);
  ffukys (fptr, "CUNIT2", cunit[1], "Time axis units", &status);

  // Reference pixel

  fits_update_key (fptr,
		   TFLOAT,
		   "CRPIX1",
		   &crpix,
		   "Reference pixel Frequency",
		   &status);
  fits_update_key (fptr,
		   TFLOAT,
		   "CRPIX2",
		   &crpix,
		   "Reference pixel Time",
		   &status);

  // Reference value

  keyvalue = crval(0);
  fits_update_key (fptr,
		   TFLOAT,
		   "CRVAL1",
		   &keyvalue,
		   "Reference value Frequency",
		   &status);
  keyvalue = crval(1);
  fits_update_key (fptr,
		   TFLOAT,
		   "CRVAL2",
		   &keyvalue,
		   "Reference value Time",
		   &status);

  // Coordinate increment

  keyvalue = cdelt(0);
  fits_update_key (fptr,
		   TFLOAT,
		   "CDELT1",
		   &keyvalue,
		   "Coordinate increment Frequency",
		   &status);
  keyvalue = cdelt(1);
  fits_update_key (fptr,
		   TFLOAT,
		   "CDELT2",
		   &keyvalue,
		   "Coordinate increment Time",
		   &status);

  return status;
}

// --------------------------------------------------------- initBeamformer_event

Bool initBeamformer_event (GlishSysEvent &event,
			   void *)
{
  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  GlishArray gtmp;

  //------------------------------------------------------------------
  /* Parse the contents of the Glish record for variables which are 
     handled locally and not by the input methods of the classes. */

  cout << "[DynamicSpectrum] Parsing contents of Glish record ..." << endl;

  String refcode;
  Vector<Double> direction;
  Matrix<Double> antennaPositions;  

  // Coordinate reference code
  if (glishRec.exists("refcode")) {
    gtmp = glishRec.get("refcode");
    gtmp.get(refcode);
  } else {
    cerr << "Missing field refcode !" << endl;
    glishBus->reply(GlishArray(False));
  }
  // Direction of the beam
  if (glishRec.exists("direction")) {
    gtmp = glishRec.get("direction");
    gtmp.get(direction);
  } else {
    cerr << "Missing field direction !" << endl;
    glishBus->reply(GlishArray(False));
  }
  // Antenna positions
  if (glishRec.exists("antennaPositions")) {
    gtmp = glishRec.get("antennaPositions");
    gtmp.get(antennaPositions);
  } else {
    cerr << "Missing field antennaPositions !" << endl;
    glishBus->reply(GlishArray(False));
  }
  // Enable/Disable producing some statistics during computation
  if (glishRec.exists("dsMethod")) {
    gtmp = glishRec.get("dsMethod");
    gtmp.get(clientData->dsMethod);
  } else {
    clientData->dsMethod = "power";
  }
  // Enable/Disable computation of differential spectra
  if (glishRec.exists("differential")) {
    gtmp = glishRec.get("differential");
    gtmp.get(clientData->differential);
  } else {
    clientData->differential = False;
  }
  // Enable/Disable producing some statistics during computation
  if (glishRec.exists("statistics")) {
    gtmp = glishRec.get("statistics");
    gtmp.get(clientData->statistics);
  } else {
    clientData->statistics = False;
  }

  clientData->nBlock = 1;

  //------------------------------------------------------------------
  // Store the received variable values
  
  try {
    cout << " - Storing Beamformer and frequency information ... " << endl;
    clientData->bf.setBeamformer(glishRec);
  } catch (AipsError x) {
    cerr << "[DynamicSpectrum::initBeamformer_event]" << x.getMesg() << endl;
  }

  // -----------------------------------------------------------------
  // If required, convert the beam direction to AZEL, for usage with 
  // the Beamformer.

  Vector<Double> beamDirection;

  if (refcode == "AZEL") {
    beamDirection = direction;
  } else {
    Quantity epoch;
    String Observatory;
    //
    if (!LOPES::QuantityFromRecord (epoch, "epoch", glishRec)) {
      cerr << "Missing field epoch!" << endl;
    }
    gtmp = glishRec.get("Observatory");
    gtmp.get(Observatory);    
    //
    ObservationData obsData (epoch,Observatory);
    //
    beamDirection = adjustBeamDirection (obsData, refcode, direction);
  }

  cout << " - Computing array with beamformer weights ... " << endl;
  Vector<Double> frequencies = clientData->bf.Frequencies (True);
  Vector<Bool> mask          = clientData->bf.FrequencyMask();

  // -----------------------------------------------------------------
  // Compute the values of the weighting factors

  clientData->bf.showProgress(True);
  clientData->bf.setPhaseGradients (frequencies,beamDirection,antennaPositions);
  clientData->bf.showProgress(False);

  //------------------------------------------------------------------
  // If we are about to compute differential dynamic spectra, we need
  // to set up the buffer array holding the previously computed
  // spectrum.

  if (clientData->differential) {
    clientData->lastSpectrum.resize(frequencies.shape());
    clientData->lastSpectrum = 0.0;
  }

  // -----------------------------------------------------------------
  // Return the AZEL coordinates of the beam position to indicate
  // completion of initialization.

  GlishRecord record;

  record.add("beamDirection",beamDirection);
  record.add("frequencies",frequencies);
  record.add("mask",mask);

  if (glishBus->replyPending()) {
    glishBus->reply(record);
  } else {
    cerr << "[DynamicSpectrum::initBeamformer_event]" << endl;
    cerr << "\tUnable to send back results; no pending reply." << endl;
  }

  return True;
}

// --------------------------------------------------------- computeSpectra_event

Bool computeSpectra_event (GlishSysEvent &event,
			   void *)
{
  cout << "[dynamicSpectrum::computeSpectra_event]" << endl;

  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  GlishArray gtmp;
  Bool ok (True);
  DataReader *dr = new DataReader ();

  // -----------------------------------------------------------------
  // Extract the parameters from the input data record. Most variables 
  // passed to the DataReader object are local, such that we do not
  // waste memory.

  String method;
  String filenameBase;
  Int StartBlock;
  Int nofBlocks;
  int stride (0);
  String telescope;
  Vector<Double> crval;
  Vector<Double> cdelt;

  cout << " - parsing Glish record to extract parameters ..." << endl;
  try {
    Vector<String> Files;
    
    if (glishRec.exists("method")) {
      gtmp = glishRec.get("method");
      gtmp.get(method);
    } else {
      cerr << "[DynamicSpectrum] Missing field: method" << endl;
      ok = False;
    }
    //
    if (glishRec.exists("datatype")) {
      gtmp = glishRec.get("datatype");
      gtmp.get(clientData->datatype);
    } else {
      cerr << "[DynamicSpectrum] Missing field: fileDataType" << endl;
      ok = False;
    }
    //
    if (glishRec.exists("stride")) {
      gtmp = glishRec.get("stride");
      gtmp.get(stride);
    }
    //
    if (glishRec.exists("StartBlock")) {
      gtmp = glishRec.get("StartBlock");
      gtmp.get(StartBlock);
    } else {
      cerr << "[DynamicSpectrum] Missing field: StartBlock" << endl;
      ok = False;
    }
    //
    if (glishRec.exists("nofBlocks")) {
      gtmp = glishRec.get("nofBlocks");
      gtmp.get(nofBlocks);
    } else {
      cerr << "[DynamicSpectrum] Missing field: nofBlocks" << endl;
      ok = False;
    }
    //
    if (glishRec.exists("filenameBase")) {
      gtmp = glishRec.get("filenameBase");
      gtmp.get(filenameBase);
    } else {
      cerr << "[DynamicSpectrum] Missing field: filenameBase" << endl;
      ok = False;
    }
    //
    if (glishRec.exists("telescope")) {
      gtmp = glishRec.get("telescope");
      gtmp.get(clientData->telescope);
    } else {
      cerr << "[DynamicSpectrum] Missing field: telescope" << endl;
      ok = False;
    }
    //
    if (glishRec.exists("observer")) {
      gtmp = glishRec.get("observer");
      gtmp.get(clientData->observer);
    } else {
      cerr << "[DynamicSpectrum] Missing field: observer" << endl;
      ok = False;
    }
    //
    if (glishRec.exists("crval")) {
      gtmp = glishRec.get("crval");
      gtmp.get(crval);
    } else {
      cerr << "[DynamicSpectrum] Missing field: crval" << endl;
      ok = False;
    }
    //
    if (glishRec.exists("cdelt")) {
      gtmp = glishRec.get("cdelt");
      gtmp.get(cdelt);
    } else {
      cerr << "[DynamicSpectrum] Missing field: cdelt" << endl;
      ok = False;
    }

    // -----------------------------------------------------------------
    // Create DataReader object with the data extracted from the record

    cout << " - assigning DataReader from record ..." << endl;
    dr = DataReaderTools::assignFromRecord (glishRec,true);
    
    /*
      After the data file type specific part is done, assign the parameters
      which are data file type independent.
     */
    cout << " - assigning DataReader progresion parameters ... " << endl;
    try {
      dr->setBlock(StartBlock);
//       dr->setStride(stride);
    } catch (AipsError x) {
      cerr << "[DynamicSpectrum::computeSpectra] " << x.getMesg() << endl;
    }
    
    // -----------------------------------------------------------------
    // Feedback on the extracted parameters

    if (ok) {
      cout << " - Extracted parameters" << endl;
      cout << " -- Input data filetype   : " << clientData->datatype  << endl;
      cout << " -- Blocksize [Samples]   : " << dr->blocksize()       << endl;
      cout << " -- Start data block      : " << StartBlock            << endl;
      cout << " -- Number of data blocks : " << nofBlocks             << endl;
      cout << " -- Stride between blocks : " << stride                << endl;
      cout << " -- Telescope             : " << clientData->telescope << endl;
      cout << " -- Observer              : " << clientData->observer  << endl;
    }

  } catch (AipsError x) {
    cerr << "[DynamicSpectrum::computeSpectra] " << x.getMesg() << endl;
    ok = False;
  }

  // -----------------------------------------------------------------
  // Compute

  Matrix<DComplex> dynamicSpectrum (dr->nofSelectedChannels(),nofBlocks);
//   Vector<Double> meanSpectrum (dr->nofSelectedChannels(),0.0);
  Matrix<Double> antennaPower (dr->nofSelectedAntennas(),nofBlocks);

  if (method == "beam") {
    cout << "[DynamicSpectrum] Computation dynamic spectrum for beam ..." << endl;
    beam2DynamicSpectrum (dynamicSpectrum,
			  antennaPower,
			  dr,
			  nofBlocks);
  }
  else if (method == "power") {
    cout << "[DynamicSpectrum] Computation dynamic spectrum for power ..." << endl;
    power2DynamicSpectrum (dynamicSpectrum, antennaPower, dr);
  }
  else {
    cerr << "[DynamicSpectrum] Unsupported method " << method << endl;
  }

  // -----------------------------------------------------------------
  // Export the dynamic spectrum to a FITS file
  
  ok = DynamicSpectrum2FITS (filenameBase,
			     dynamicSpectrum,
			     crval,
			     cdelt);
  
  // -----------------------------------------------------------------
  // Pass back the results

  cout << "[DynamicSpectrum] Returning computation results ..." << endl;
  cout << " - Shape of dynamic spectrum : " << dynamicSpectrum.shape() << endl;
  cout << " - Shape of antenna power    : " << antennaPower.shape() << endl;

  GlishRecord record;

  record.add("filenameBase",filenameBase);
  record.add("antennaPower",antennaPower);

  // Pass back record via Glish bus

  if (glishBus->replyPending()) {
    glishBus->reply(record);
  } else {
    cerr << "[DynamicSpectrum::computeSpectra]" << endl;
    cerr << "\tUnable to send back results; no pending reply." << endl;
    ok = False;
  }

  return ok;
}

// ------------------------------------------------------- processDatablock_event

Bool processDatablock_event (GlishSysEvent &event, void *) {
  
  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  GlishArray gtmp;
  GlishRecord record;
  
  // -----------------------------------------------------------------
  // extract the parameters from the input data record

  Int NumChannels (0);
  Int nofAntennas (0);
  Matrix<Double> dataFx;
  Matrix<DComplex> dataCalFFT;

  if (glishRec.exists("dataCalFFT")) {
    gtmp = glishRec.get("dataCalFFT");
    gtmp.get(dataCalFFT);
    //
    dataCalFFT.shape (NumChannels,nofAntennas);
  } else if (glishRec.exists("dataFx")) {
    gtmp = glishRec.get("dataFx");
    gtmp.get(dataFx);
    //
    dataFx.shape (NumChannels,nofAntennas);
  } else {
    cerr << "[DynamicSpectrum::processDatablock_event]" << endl;
    cerr << "\tNo data found on input!" << endl;
    glishBus->reply(GlishArray(False));
  }

  // -----------------------------------------------------------------
  // Variables used during computation

  Matrix<DComplex> beam (NumChannels,1);
  Vector<Double> spectrum (NumChannels);
  Vector<Double> power (NumChannels);

  // -----------------------------------------------------------------
  // Process the FFT data

  if (clientData->dsMethod == "beam") {
    // [1] Beamforming
    clientData->bf.addAntennas (beam,Matrix<DComplex>(dataCalFFT));
    // [2] Compute power spectrum
    for (int chan=0; chan<NumChannels; ++chan) {
      spectrum(chan) = abs(beam(chan,0)*conj(beam(chan,0)));
    }
    // [3] Total power per antenna
    calcAntennaPower (power,dataCalFFT);
  }
  else if (clientData->dsMethod == "meanPower") {
    calcMeanPower (spectrum, dataCalFFT);
  }

  // Add the computation results to the record returned
  record.add("spectrum",spectrum);
  record.add("power",power);

  // -----------------------------------------------------------------
  // [4] Differential spectrum

  if (clientData->differential) {
    Vector<Double> diff(NumChannels);
    //
    diff = spectrum-clientData->lastSpectrum;
    //
    record.add("diffSpectrum",diff);
    clientData->lastSpectrum = spectrum;
  }

  // -----------------------------------------------------------------

  if (clientData->statistics) {
    logStatistics (spectrum);
  }

  // -----------------------------------------------------------------
  // Return the computed spectrum

  if (glishBus->replyPending()) {
    glishBus->reply(record);
  } else {
    cerr << "[DynamicSpectrum::processDatablock_event]" << endl;
    cerr << "\tUnable to send back results; no pending reply." << endl;
  }

  //------------------------------------------------------------------
  // Keep track of the block counter

  clientData->nBlock++;

  return True;
}

// ------------------------------------------------------------- crosscorrDynspec

Bool crosscorrDynspec (GlishSysEvent &event, void *) {
  
  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  GlishArray gtmp;
  
  //------------------------------------------------------------------
  // Declare variables to accept the data from the input record

  Matrix<Double> dynspec;
  Matrix<Double> crosscorr;

  //------------------------------------------------------------------
  // extract the parameters from the input data record

  if (glishRec.exists("dynspec")) {
    gtmp = glishRec.get("dynspec");
    gtmp.get(dynspec);
  } else {
    cerr << "[DynamicSpectrum::crosscorrFrequencies]" << endl;
    cerr << "\tWarning, unable to find field dynspec." << endl;
    glishBus->reply(GlishArray(False));
  }

  //------------------------------------------------------------------
  // Compute the cross-correlation of the frequencies

  LOPES::CrossCorrelation (crosscorr, dynspec, dynspec, -1);

  //------------------------------------------------------------------
  // Return the computed spectrum

  GlishRecord record;

  record.add("crosscorr",crosscorr);

  if (glishBus->replyPending()) {
    glishBus->reply(record);
  } else {
    cerr << "[DynamicSpectrum::crosscorrFrequencies]" << endl;
    cerr << "\tUnable to send back results; no pending reply." << endl;
  }

  return True;

}

// ----------------------------------------------------------------- cmsFrequency

Bool cmsFrequency (GlishSysEvent &event, void *)
{
  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  GlishArray gtmp;
  
  //------------------------------------------------------------------
  // Declare variables to accept the data from the input record

  Vector<Double> spectrum;
  Vector<Double> cmsFreq;
  Matrix<Double> DynSpec;

  //------------------------------------------------------------------
  // extract the parameters from the input data record

  cout << "[DynamicSpectrum::cmsFrequency] Reading input variables ... ";

  if (glishRec.exists("DynSpec")) {
    gtmp = glishRec.get("DynSpec");
    gtmp.get(DynSpec);
  } else glishBus->reply(GlishArray(False));

  cout << "Done." << endl;

  //------------------------------------------------------------------
  // Setup additional variables

  DataFrequency df (glishRec);
  Vector<Double> frequencies = df.Frequencies(True);

  IPosition shapeDynspec (DynSpec.shape());  // [nTime,nFreq]
  spectrum.resize(shapeDynspec(1));
  cmsFreq.resize(shapeDynspec(0));

  //------------------------------------------------------------------
  // Compute the 'Center of mass' equivalent in frequency space

  cout << "[DynamicSpectrum::cmsFrequency] Computing ... " << flush;

  for (int t=0; t<shapeDynspec(0); ++t) {
    spectrum = DynSpec.row(t);
    cmsFreq(t) = LOPES::WeightedCenter (frequencies,spectrum);
  }

  cout << "Done." << endl;

  //------------------------------------------------------------------
  // Return the computed spectrum

  GlishRecord record;

  record.add("cmsFreq",cmsFreq);

  if (glishBus->replyPending()) {
    glishBus->reply(record);
  } else {
    cerr << "[DynamicSpectrum::cmsFrequency]" << endl;
    cerr << "\tUnable to send back results; no pending reply." << endl;
  }

  return True;

}

// -------------------------------------------------------- paramExtraction_event

Bool paramExtraction_event (GlishSysEvent &event,
			    void *)
{
  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  Bool status (True);
  Quantity obsTime;
  MEpoch date;
  MDirection j2000;
  MDirection azel;
  MPosition observatory;

  status = LOPES::QuantityFromRecord   (obsTime,     "time",         glishRec);
  status = LOPES::MEpochFromRecord     (date,        "date",         glishRec);
  status = LOPES::MDirectionFromRecord (j2000,       "j2000",        glishRec);
  status = LOPES::MDirectionFromRecord (azel,        "azel",         glishRec);
  status = LOPES::MPositionFromRecord  (observatory, "observatory",  glishRec);

  std::cout << "[paramExtraction]" << std::endl;
  std::cout << " - obsTime      = " << obsTime     << std::endl;
  std::cout << " - date         = " << date        << std::endl;
  std::cout << " - azel         = " << azel        << std::endl;
  std::cout << " - j2000        = " << j2000       << std::endl;
  std::cout << " - observatory  = " << observatory << std::endl;

  GlishRecord record;
  record.add ("timeValue", obsTime.getValue());
  record.add ("timeUnit",  obsTime.getUnit());

  if (glishBus->replyPending()) {
    glishBus->reply(record);
  }

  return status; 
}
