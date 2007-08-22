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

/* $Id: skymapper.cc,v 1.15 2007/05/02 09:37:10 bahren Exp $ */

#include "skymapper.h"

IPosition imageShape;
Skymap skymap = Skymap ();        // Skymap object
Vector<Double> mapCenterCSC(2);   // map center: celestial spherical coordinates
Vector<Double> mapCenterNSC(2);   // map center: natural spherical coordinates
ofstream Logfile;                 // Output stream for the logfile
String logfile_name;              // Name of the logfile written to disk
String dataType;                  // Type of input data
Time StartTime;
Bool verboseON (False);           // Switch ON/OFF extensive logging


// =============================================================================


void decomposeIntegerNumber (Vector<Int>& factors,
			     Int IntegerNumber)
{
  Int nelem(0);
  Int factorsNumber(0);
  Vector<Int> factorsVector;
  Double reminder(0.0);

  nelem = (Int)IntegerNumber/2;
  factorsVector.resize(nelem);

  factorsVector = 0;

  for (int n=1; n<=nelem; ++n) {
    reminder = IntegerNumber%n;
    if (!reminder) {
      ++factorsNumber;
      factorsVector(n-1) = n;
    }
  }

  factors.resize(factorsNumber);
  factorsNumber = 0;
  for (int n=0; n<nelem; ++n) {
    if (factorsVector(n)) {
      factors(factorsNumber) = factorsVector(n);
      ++factorsNumber;
    }
  }

}


// =============================================================================


void signalStatistics (Matrix<DComplex>& antennaSignals)
{
  Int nant(0);
  Int nfreq(0);
  DComplex sumAntenna,sumAll;

  antennaSignals.shape(nfreq,nant);

  sumAll = DComplex(0.0,0.0);

  for (int i=0; i<nant; ++i) {
    sumAntenna = DComplex(0.0,0.0);
    for (int k=0; k<nfreq; ++k) {
      sumAntenna += antennaSignals(k,i);
    }
    sumAll += sumAntenna;
  }
  cout << "\tsum(all): " << sumAll << endl;
}


// =============================================================================


void Voltage2FFT (Matrix<DComplex>& dataFFT,
		  Matrix<Double>& dataVoltage)
{
  Int nofAntennae(0);
  Int nofSamples(0);
  Int nofChannels(0);
  Int fftsize (skymap.FFTSize());
  Int fftlen (skymap.FFTlen());
  Vector<Bool> mask = skymap.FrequencyMask();
  IPosition shapeMask (mask.shape());
  Bool applyMask (True);
  Bool verboseON (True);

  /* If the number of samples does not match the FFTSize variable we should
     issue a warning; processing most propably can be done anyway. */
  dataVoltage.shape(nofSamples,nofAntennae);
  if (nofSamples != fftsize) {
    cerr << "[skymapper::Voltage2FFT] "
      "Incosistent data: samples != fftsize" << endl;
  }
  
  /* How many frequency channels have been selected for processing; parse the
     masking array to extract this number and resize the FFT domain data
     array according to this. */
  nofChannels = 0;
  for (int n=0; n<shapeMask(0); ++n) {
    nofChannels += int(mask(n));
  }

  /* If all frequency channels are selected, there is no need to apply the
     masking array. */
  if (nofChannels == shapeMask(0) || (shapeMask(0) == 1 && mask(0) == 1)
      || shapeMask(0) != fftlen){
    applyMask = False;
    nofChannels = fftlen;
  }

  dataFFT.resize(nofChannels,nofAntennae);
  
  /* Give some basic feedback on the characteristics of the data and the
     processing parameters. */
  if (verboseON) {
    cout << "[skymapper::Voltage2FFT] " << endl;
    cout << " - Shape of the input data array : " << dataVoltage.shape() << endl;
    cout << " - Shape of the output array ... : " << dataFFT.shape() << endl;
    cout << " - Shape of the masking array .. : " << mask.shape() << endl;
    cout << " - FFT time domain size ........ : " << fftsize << endl;
    cout << " - FFT frequency domain size ... : " << fftlen << endl;
    cout << " - Selected frequency channels . : " << nofChannels << endl;
    cout << " - Apply frequency selection ... : " << applyMask << endl;
  }

  /* Finally: FFT the input data on a per antenna basis. */
  Vector<Double> vectTime(fftsize);
  Vector<DComplex> vectFreq(fftlen);
  FFTServer<Double,DComplex> server(IPosition(1,fftsize),FFTEnums::REALTOCOMPLEX);

  for (int ant=0; ant<nofAntennae; ++ant) {
    vectTime = dataVoltage.column(ant);
    // FFT from time to frequency domain
    server.fft(vectFreq,vectTime);
    // frequency channel selection
    if (applyMask) {
      for (int freq=0,k=0; freq<fftlen; ++freq) {
	if (mask(freq)) {
	  dataFFT(k,ant) = vectFreq(freq);
	  ++k;
	}
      }
    } else {
      dataFFT.column(ant) = vectFreq;
    }
  }
  
}


// =============================================================================


Bool init_event (GlishSysEvent &event, void *) {
  
  cout << "[Skymapper] Initializing internal data" << endl;

  // Variables to handle data received via Glish bus
  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  GlishArray gtmp;

  // Declare variables to accept the data from the input record
  String Eventdir;
  String Filename ("Skymapper");
  String antennaFile;
  Int Integrations (0);
  Int NumberBlocks (0);
  Int firstBlock   (0);
  Double IntegrationTime;
  Vector<Int> antennaSet;
  Vector<Double> FrequencyBand;
  String mapCenterWCS;
  String refcodeSkymap;
  String projectionCategory;
  String mapGridding;
  Vector<Double> minCSC,maxCSC,incrCSC;
  Vector<Double> mapCenterCoords;
  Vector<Int> numPixels;
  
  // extract the parameters from the input data record

  cout << " - Extracting data from Glish record ..." << flush;
  try {
    gtmp = glishRec.get("eventdir");
    gtmp.get(Eventdir);
    if (glishRec.exists("Filename")) {
      gtmp = glishRec.get("Filename");
      gtmp.get(Filename);
    }
    if (glishRec.exists("minCSC")) {
      gtmp = glishRec.get("minCSC");
      gtmp.get(minCSC);
    } else glishBus->reply(GlishArray(False));
    if (glishRec.exists("maxCSC")) {
      gtmp = glishRec.get("maxCSC");
      gtmp.get(maxCSC);
    } else glishBus->reply(GlishArray(False));
    if (glishRec.exists("incrCSC")) {
      gtmp = glishRec.get("incrCSC");
      gtmp.get(incrCSC);
    } else glishBus->reply(GlishArray(False));
    gtmp = glishRec.get("mapCenterWCS");
    gtmp.get(mapCenterWCS);
    gtmp = glishRec.get("CoordinateSystem");
    gtmp.get(refcodeSkymap);
    gtmp = glishRec.get("mapCenterCoords");
    gtmp.get(mapCenterCoords);
    gtmp = glishRec.get("mapGridding");
    gtmp.get(mapGridding);
    gtmp = glishRec.get("pixels");
    gtmp.get(numPixels);
    if (glishRec.exists("antennaFile")) {
      gtmp = glishRec.get("antennaFile");
      gtmp.get(antennaFile);
    }
    gtmp = glishRec.get("antennaSet");
    gtmp.get(antennaSet);
    gtmp = glishRec.get("FrequencyBand");
    gtmp.get(FrequencyBand);
    gtmp = glishRec.get("Integrations");
    gtmp.get(Integrations);
    gtmp = glishRec.get("blocks");
    gtmp.get(NumberBlocks);
    gtmp = glishRec.get("firstBlock");
    gtmp.get(firstBlock);
    gtmp = glishRec.get("IntegrationTime");
    gtmp.get(IntegrationTime);
    gtmp = glishRec.get("dataType");
    gtmp.get(dataType);
  } catch (AipsError x) {
    cerr << "[skymapper::init_event] Error while parsing Glish record" << endl;
    cerr << x.getMesg() << endl;
  }
  cout << "ok" << endl;
  
  // -----------------------------------------------------------------
  // Create a new logfile

  try {
    LogfileName log (Filename);
    logfile_name = log.filename();
    //
    Logfile.open(logfile_name.c_str(),ios::out);
    Logfile << "[" << StartTime << "] Skymapper started." << endl;
    Logfile.close();
  } catch (AipsError x) {
    cerr << "[skymapper::init_event] Error while creating logfile." << endl;
    cerr << x.getMesg() << endl;
  }

  // -----------------------------------------------------------------
  // Set up the internal data of the Skymap.

  {
    Bool status;
    String telescope;
    String refcodeSkymap;
    String refcodeCenter;
    String projection;
    Vector<Int> pixelShape;
    Vector<Double> center;
    Vector<Double> increment;
    Vector<String> orientation;
    Vector<Double> elevation;
    Quantity epoch;
    //
    gtmp = glishRec.get("ObservatoryName");
    gtmp.get(telescope);
    //
    gtmp = glishRec.get("CoordinateSystem");
    gtmp.get(refcodeSkymap);
    gtmp = glishRec.get("mapCenterWCS");
    gtmp.get(refcodeCenter);
    gtmp = glishRec.get("Projection");
    gtmp.get(projection);
    //
    gtmp = glishRec.get("pixels");
    gtmp.get(pixelShape);
    gtmp = glishRec.get("mapCenterCoords");
    gtmp.get(center);
    gtmp = glishRec.get("incrCSC");
    gtmp.get(increment);
    gtmp = glishRec.get("orientation");
    gtmp.get(orientation);
    gtmp = glishRec.get("rangeElevation");
    gtmp.get(elevation);
    //
    status = CR::QuantityFromRecord (epoch, "epoch", glishRec);
    IPosition shape (pixelShape);
    //
    SkymapperTools::setSkymapGrid (skymap,
				   epoch,
				   telescope,
				   refcodeSkymap,
				   refcodeCenter,
				   projection,
				   shape,
				   center,
				   increment);
    //
    skymap.setElevationRange (elevation);
    skymap.setOrientation (orientation);
  }
  
  // -----------------------------------------------------------------
  // Set up the internal data of the various objects; this section
  // soon is to be replaced by the above function call.

  cout << " - Setting up skymap parameters ... " << flush;
  skymap.setSkymap(glishRec);
  cout << "ok" << endl;

  // -----------------------------------------------------------------
  // Log the settings

  Logfile.open(logfile_name.c_str(),ios::app);
  Logfile.precision(10);

  Logfile << "\n[ Skymapper : Processing setup ]\n" << endl;
  Logfile << " - Name of the event directory ........ : " << Eventdir << endl;
  Logfile << " - Name of the input file ............. : " << Filename << endl;
  Logfile << " - Type of input data ................. : " << dataType << endl;
  Logfile << " - Set of selected antennae  .......... : " << antennaSet << endl;
  Logfile << " - Selected frequency band ............ : " << FrequencyBand << endl;
  Logfile << " - Number of integrations ............. : " << Integrations << endl;
  Logfile << " - Datablocks per Integration ......... : " << NumberBlocks << endl;
  Logfile << " - First datablock processed .......... : " << firstBlock << endl;
  Logfile << " - Integration time per timeframe [sec] : " << IntegrationTime << endl;

  Logfile.close();

  // -----------------------------------------------------------------
  // Assemble all data  required to set up the coordinate system  tool
  // to be attached to the image tool and this return this information
  // to Glish.
  
  GlishRecord record;
  
  // WCS data
  try{
    Vector<Double> referenceValue;
    Vector<Double> increment;
    Matrix<Double> xform;
    Vector<Double> referencePixel;
    Vector<String> worldAxisUnits;
    Vector<Double> projectionParameters;
    // extract the parameters of the direction coordinate
    skymap.directionCoordinate (referenceValue,
				increment,
				xform,
				referencePixel,
				worldAxisUnits,
				projectionParameters);
    //
    record.add("referenceValue",referenceValue);
    record.add("increment",increment);
    record.add("xform",xform);
    record.add("referencePixel",referencePixel);
    record.add("worldAxisUnits",worldAxisUnits);
    //
    record.add("projectionParameters",projectionParameters);
  } catch (AipsError x) {
    cerr << "[skymapper::init_event] Error adding WCS data to record" << endl;
    cerr << x.getMesg() << endl;
  }
  // Frequency data
  try {
    record.add("frequencyValues",skymap.Frequencies(True));
    record.add("frequencyMask",skymap.FrequencyMask());
  } catch (AipsError x) {
    cerr << "[skymapper::init_event] Error adding frequency data to record" << endl;
    cerr << x.getMesg() << endl;
  }

  try {
    if (glishBus->replyPending()) {
      glishBus->reply(record);
    } else {
      cerr << "[skymapper::init_event]" << endl;
      cerr << "\tUnable to send back results; no pending reply." << endl;
    }
  } catch (AipsError x) {
    cerr << "[skymapper::init_event] " << x.getMesg() << endl;
  }

  return True;
}


// =============================================================================


Bool setBeamformer (GlishSysEvent &event, void *)
{
  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  GlishArray gtmp;

  Double Distance;
  Vector<Double> FrequencyBand;
  Matrix<Double> antennaPositions;
  Vector<Int> shape;

  // -----------------------------------------------------------------
  // Extract variable values from the received Glish record

  try {
    gtmp = glishRec.get("imageShape");
    gtmp.get(shape);
    imageShape = shape;
    //
    gtmp = glishRec.get("Distance");
    gtmp.get(Distance);
    gtmp = glishRec.get("FrequencyBand");
    gtmp.get(FrequencyBand);
    gtmp = glishRec.get("AntennaPositions");
    gtmp.get(antennaPositions);
  } catch (AipsError x) {
    cerr << "[skymapper::setBeamformer]" 
      "Error extracting variables from Glish record!" << endl;
    cerr << x.getMesg() << endl;
  }

  // -----------------------------------------------------------------
  // Store possible distance information

  skymap.setDistance(Distance);
    
  // -----------------------------------------------------------------
  // Set up the array with the pre-computed weighting factors for the
  // beamforming.
  
  if (skymap.BFMethod() != "ccMatrix") {
    toLogfile("Computing weighting factors for Beamformer.");
    try {
      // get variables for computation
      Vector<Double> frequencies = skymap.Frequencies (FrequencyBand);
      // compute the weighting factors for the beamforming
      skymap.showProgress(True);
      skymap.setPhaseGradients (frequencies,
				antennaPositions);
    } catch (AipsError x) {
      cerr << "[skymapper::setBeamformer] " 
	"Error while computing phase gradients." << endl;
      cerr << x.getMesg() << endl;
      cerr << " - Frequency array   : "
	   << skymap.Frequencies(FrequencyBand).shape() << endl;
    }
  } else {
    cout << "[skymapper::setBeamformer] "
      "No precomputation of weighting factors for cc'ed data" << endl;
  }

  // logfile
  Logfile.open(logfile_name.c_str(),ios::app);
  skymap.printSkymap(Logfile);
  Logfile.close();
  
  // report that init is complete
  if (glishBus->replyPending()) {
    glishBus->reply(GlishArray(True));
  } else {
    cerr << "[skymapper::setBeamformer]" << endl;
    cerr << "\tUnable to send back results; no pending reply." << endl;
  }
  
  return True;
}

// =============================================================================

Bool data2Map (GlishSysEvent &event, void *)
{
  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  GlishArray gtmp;

  // [1] Cross-correlated data
  if (skymap.BFMethod() == "ccMatrix") {
    // (a) additional variables for parameter extraction
    String eventdir,antennaFile;
    Vector<Bool> AntennaMask;
    // (b) extract additional parameters from Glish records
    try {
      gtmp = glishRec.get("eventdir");
      gtmp.get(eventdir);
      gtmp = glishRec.get("antennaFile");
      gtmp.get(antennaFile);
      gtmp = glishRec.get("AntennaMask");
      gtmp.get(AntennaMask);
    } catch (AipsError x) {
      cerr << "[skymapper::data2Map] Error extracting parameters." << endl;
      cerr << "\t" << x.getMesg() << endl;
    }
    // (c) start processing data
    try {
      ccMatrix2Map (eventdir, antennaFile, AntennaMask);
    } catch (AipsError x) {
      cerr << "[skymapper::data2Map] Error computing map from data." << endl;
      cerr << "\t" << x.getMesg() << endl;
    }
  }
  // [2] Time domain / Fourier domain data
  else {
    // (a) additional variables for parameter extraction
    Matrix<Double> dataVoltage;
    Matrix<DComplex> dataFFT;
    // (b) extract additional parameters from Glish records
    try {
      gtmp = glishRec.get("antennaSignals");
    } catch (AipsError x) {
      cerr << "[skymapper::data2Map] Error extracting parameters." << endl;
      cerr << "\t" << x.getMesg() << endl;
    }
    if (dataType == "Voltage") {
      gtmp.get(dataVoltage);
      // Time domain -> FFT domain
      Voltage2FFT (dataFFT,dataVoltage);
    }
    else {
      gtmp.get(dataFFT);
    }
    // (c) start processing data
    try {
      skymap.addAntennas(dataFFT);
    } catch (AipsError x) {
      cerr << "[skymapper::data2Map]" << x.getMesg() << endl;
    }
  }

  // signal Glish that we are ready to process data
  try {
    glishBus->reply(GlishArray(True));
  } catch (AipsError x) {
    cerr << "[skymapper::data2Map] Error returning result," << endl;
    cerr << "\t" << x.getMesg() << endl;
  }
  
  return True;
}


// =============================================================================


void ccMatrix2Map (const String dirname,
		   const String antennafile, 
		   Vector<Bool>& antennaSelection)
{
  String metafile = dirname + "/experiment.meta";

  // 1. Create an object for the cross-correlated data
  CR::DataCC cc (metafile,antennafile);

  // 2. Read in the experiment metafile.
  try {
    cc.readMetafile();
  } catch (AipsError x) {
    cerr << "[skymapper::ccMatrix2Map] Error reading metafile." << endl;
    cerr << "\t" << x.getMesg() << endl;
  }

  // 3. Read in the binary data of the cross-correlation data cube.
  try {
    cc.readDataCC();
  } catch (AipsError x) {
    cerr << "[skymapper::ccMatrix2Map] Error reading cc data." << endl;
    cerr << "\t" << x.getMesg() << endl;
  }

  // 4. Adjust the masks for selection of frequency channels and antennae
  Vector<Bool> frequencyMask = skymap.FrequencyMask();
  Cube<DComplex> ccCube = cc.ccCube(antennaSelection,frequencyMask);

  // 5. Get the frequency values, [Hz], for the frequency channels
  Vector<Double> frequencies = cc.frequencyValues(True);

  // 6. Get the antenna positions.
  Matrix<Double> antennaPositions = cc.antennaPositions ();

  // 7. Add contents of experiment metafile to logfile
  Logfile.open(logfile_name.c_str(),ios::app);
  //
  cc.metadata(Logfile);
  //
  Logfile << " - Frequency values ..... [Hz] : " << min(frequencies) << " ... "
	  << max(frequencies) << endl;
  Logfile << " - Frequency array ........... : " << frequencies.shape() << endl;
  //
  Logfile.close();

  // 8. create skymap from ccMatrix
  toLogfile ("Starting computation of skymap from cc-matrix.");
  try {
    skymap.ccMatrix(ccCube, frequencies, antennaPositions);
  } catch (AipsError x) {
    cerr << "[skymapper::ccMatrix2Map] Error in computing map from data." << endl;
    cerr << "\t" << x.getMesg() << endl;
  }
  toLogfile ("Finished computing skymap from cc-matrix.");

}

// =============================================================================

Bool returnMap (GlishSysEvent &event, void *) {
  
  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  GlishRecord record;
  GlishArray gtmp;
  
  // Declaration of variables
  String signalQuantity (skymap.signalQuantity());
  String signalDomain (skymap.signalDomain());
  Cube<Bool> skymapMask;
  Int nofChannels (0);

  // extract the parameters from the input data record
  if (glishRec.exists("nofChannels")) {
    gtmp = glishRec.get("nofChannels");
    gtmp.get(nofChannels);
  } else {
    cerr << "[skymapper::returnMap] : no valid value for nofChannels." << endl;
    glishBus->reply(GlishArray(False));
  }

  toLogfile ("Extracting skymap pixel values.");

  /* If the skymap contains the electric field in the frequency domain, the
     contents of the data cube is of type complex; ... */
  if (signalDomain == "freq" && signalQuantity == "field") {
    Cube<DComplex> skymapValues;
    skymap.skymap (skymapValues,
		   skymapMask,
		   nofChannels);
    record.add("skymapValues",skymapValues);
    record.add("skymapMask",skymapMask);    
  }
  /* ... otherwise the contents is of type real. */
  else {
    /* Extract the skymap values, append them to the Glish record and send back
       the results. */
    Cube<Double> skymapValues;
    skymap.skymap (skymapValues,
		   skymapMask,
		   nofChannels);
    record.add("skymapValues",skymapValues);
    record.add("skymapMask",skymapMask);
  }

  if (glishBus->replyPending()) {
    glishBus->reply(record);
  } else {
    cerr << "[skymapper::returnMap]" << endl;
    cerr << "\tUnable to send back results; no pending reply." << endl;
  }
  
  return True;

}

// =============================================================================

Bool clearMap (GlishSysEvent &event, void *) {
  
  GlishSysEventSource *glishBus = event.glishSource();
//   GlishRecord glishRec = event.val();
  GlishArray gtmp;

  toLogfile ("Clearing internal data array storing skymap values.");

  skymap.clearSkymap();

  // send message back over the Glish bus that the map has been cleared
  if (glishBus->replyPending()) {
    glishBus->reply(GlishArray(True));
  } else {
    cerr << "[skymapper::clearMap]" << endl;
    cerr << "\tUnable to send back results; no pending reply." << endl;    
  }

  return True;

}

// =============================================================================

void toLogfile (String message)
{
  Time logTime;
  
  Logfile.open(logfile_name.c_str(),ios::app);
  Logfile << "\n[" << logTime << "] " << message << endl;
  Logfile.close();
}

// =============================================================================

Bool addtoLogfile (GlishSysEvent &event,
		   void *)
{
  GlishRecord glishRec = event.val();

  GlishArray gtmp = glishRec.get("message");
  String message;

  gtmp.get(message);

  toLogfile(message);
  
  return True;
}

// =============================================================================

Bool dataBlocksizes (GlishSysEvent &event, void *)
{
  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  GlishArray gtmp;

  Int Filesize;
  Vector<Int> Blocksizes;

  // extract variable values from Glish record
  gtmp = glishRec.get("Filesize");
  gtmp.get(Filesize);  

  // start computation
  decomposeIntegerNumber (Blocksizes,Filesize);

  // return result
  GlishRecord record;
  record.add("Blocksizes",Blocksizes);
  
  if (glishBus->replyPending()) {
    glishBus->reply(record);
  } else {
    cerr << "Skymapper::dataBlocksizes" << endl;
    cerr << "\tUnable to send back results; no pending reply." << endl;
  }
  
  return True;
  
}

// =============================================================================

Bool getSettingsCC (GlishSysEvent &event,
		    void *)
{
  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  GlishArray gtmp;

  String metafile,antennafile,triggertime;
  Vector<String> triggertimes;

  // extract variable values from Glish record
  gtmp = glishRec.get("metafile");
  gtmp.get(metafile);  
  gtmp = glishRec.get("antennafile");
  gtmp.get(antennafile);

  // Create a cc-datacube object
  CR::DataCC cc (metafile,antennafile);
  cc.readMetafile();

  // Extract experiment metadata
  
  Vector<Int> antennas = cc.antennas(1);
  Matrix<Double> antennaPositions = cc.antennaPositions();

  Int FFTSize = cc.fftsize();
  Vector<Double> frequencies = cc.frequencyValues();

  triggertimes = cc.triggertime();
  triggertime = CR::replaceString(CR::replaceString(CR::replaceString (triggertimes(0),"-","/"),"T","/"),"Z","");
  
  // return result
  GlishRecord record;
  record.add("FFTSize",FFTSize);
  record.add("frequencies",frequencies);
  record.add("antennas",antennas);
  record.add("triggertime",triggertime);
  
  if (glishBus->replyPending()) {
    glishBus->reply(record);
  } else {
    cerr << "[skymapper::parseMetafile]" << endl;
    cerr << "\tUnable to send back results; no pending reply." << endl;
  }
  
  return True;
}

// =============================================================================

Bool setDataReader (GlishSysEvent &event,
		    void *)
{
  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  
  DataReader *dr;
  dr = DataReaderTools::assignFromRecord (glishRec,true);
  
  if (glishBus->replyPending()) {
    glishBus->reply(GlishArray(True));
  } else {
    cerr << "[skymapper::setDataReader] No pending reply." << endl;
  }

  return True;
}

// =============================================================================
// 
//  Main routine
// 
// =============================================================================

int main(int argc, char **argv)
{  

  GlishSysEventSource glishStream(argc, argv);
  
  glishStream.addTarget(init_event, "initSkymapper");
  glishStream.addTarget(setBeamformer,"setBeamformer");
  glishStream.addTarget(data2Map, "data2Map");
  glishStream.addTarget(returnMap, "returnMap");
  glishStream.addTarget(clearMap, "clearMap");
  glishStream.addTarget(addtoLogfile, "addtoLogfile");
  glishStream.addTarget(dataBlocksizes, "dataBlocksizes");
  //
  glishStream.addTarget(getSettingsCC, "getSettingsCC");
  //
  glishStream.addTarget(setDataReader, "setDataReader");
  
  glishStream.loop();

  return 0;

}
