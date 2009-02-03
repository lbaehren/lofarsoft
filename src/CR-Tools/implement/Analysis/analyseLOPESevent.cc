/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

#include <Analysis/analyseLOPESevent.h>

#define DEG2RAD (PI/180.)

//#define DEBUGGING_MESSAGES

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // ---------------------------------------------------------- analyseLOPESevent

  analyseLOPESevent::analyseLOPESevent(){
    init();
    clear();
  };
  
  // ---------------------------------------------------------- analyseLOPESevent

  analyseLOPESevent::analyseLOPESevent (double const &remoteStart,
					double const &remoteStop,
					double const &fitStart,
					double const &fitStop,
					double const &plotStart,
					double const &plotStop) {
    init();
    
    remoteStart_p = remoteStart;
    remoteStop_p  = remoteStop;
    fitStart_p    = fitStart;
    fitStop_p     = fitStop;
    plotStart_p   = plotStart;
    plotStop_p    = plotStop;
    
    clear();
  }

  // ---------------------------------------------------------------------- init

  void analyseLOPESevent::init() {
    pipeline_p     = NULL;
    upsamplePipe_p = NULL;
    beamPipe_p     = NULL;
    lev_p          = NULL;
    upsampler_p    = NULL;
    beamformDR_p   = NULL;
    remoteStart_p  = 1./4.;
    remoteStop_p   = 4./9.;
    fitStart_p     = -2e-6;
    fitStop_p      = -1.7e-6;
    plotStart_p    = -2.05e-6;
    plotStop_p     = -1.55e-6;
    freqStart_p    = 40e6;
    freqStop_p     = 80e6;
    filterStrength_p = 3;
    remoteRange_p.resize(2);
    Polarization_p = "ANY";
  }
  
  // ---------------------------------------------------------------------- clear

  void analyseLOPESevent::clear() {
    if (pipeline_p != NULL) {
      delete pipeline_p;
      pipeline_p = NULL;
    };
    if (upsamplePipe_p != NULL) {
      delete upsamplePipe_p;
      upsamplePipe_p = NULL;
    };
    if (lev_p != NULL) {
      delete lev_p;
      lev_p = NULL;
    };
    if (upsampler_p != NULL) {
      delete upsampler_p;
      upsampler_p = NULL;
    };
  };
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  analyseLOPESevent::~analyseLOPESevent ()
  {
    clear();
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void analyseLOPESevent::summary (std::ostream &os)
  {
    os << "[analyseLOPESevent] Summary of object properties" << std::endl;
    os << "-- remoteStart = " << remoteStart_p << std::endl;
    os << "-- remoteStop  = " << remoteStop_p  << std::endl;
  }
   
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // --------------------------------------------------------------- initPipeline
  
  Bool analyseLOPESevent::initPipeline(Record ObsRecord){
    try {
      clear();
      pipeline_p = new CRinvFFT();
      upsamplePipe_p = new CRinvFFT();
      lev_p = new LopesEventIn();
      pipeline_p->SetObsRecord(ObsRecord);
      upsamplePipe_p->SetObsRecord(ObsRecord);
      
    } catch (AipsError x) {
      cerr << "analyseLOPESevent::initPipeline: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };

  // --------------------------------------------------------------- ProcessEvent
  Record analyseLOPESevent::ProcessEvent(String evname,
					 Double Az,
					 Double El,
					 Double distance, 
					 Double XC,
					 Double YC,
					 Bool RotatePos,
					 String PlotPrefix, 
					 Bool generatePlots,
					 Vector<Int> FlaggedAntIDs, 
					 Bool verbose,
					 Bool simplexFit,
					 Double ExtraDelay,
					 int doTVcal,
					 Double UpSamplingRate,
					 String Polarization){
    Record erg;
    try {
      //Int nsamples;
      //
      //Matrix<Double> TimeSeries;
      //

      Vector<Bool> AntennaSelection;
      Double center;
      Record fiterg;
      
#ifdef DEBUGGING_MESSAGES
      if (!verbose){
	cout <<"analyseLOPESevent::ProcessEvent: compiled with DEBUGGING_MESSAGES, so switching on verbose output."<<endl;
	verbose = True;
      };
#endif
      if (! SetupEvent(evname, doTVcal, FlaggedAntIDs, AntennaSelection, 
		       UpSamplingRate, ExtraDelay, verbose)) {
	cerr << "analyseLOPESevent::ProcessEvent: " << "Error during SetupEvent()!" << endl;
	return Record();
      };
      if (! doPositionFitting(Az, El, distance, center, XC, YC, RotatePos,
			      AntennaSelection, Polarization, simplexFit, verbose, (distance<=0.) ) ){
	cerr << "analyseLOPESevent::ProcessEvent: " << "Error during doPositionFitting()!" << endl;
	return Record();
      };
      if (! GaussFitData(Az, El, distance, center, AntennaSelection, evname, erg, fiterg, 
			 Polarization, verbose) ){
	cerr << "analyseLOPESevent::ProcessEvent: " << "Error during GaussFitData()!" << endl;
	return Record();
      };
      if (generatePlots) {
	if (! doGeneratePlots(PlotPrefix, fiterg.asArrayDouble("Cgaussian"), 
			      fiterg.asArrayDouble("Xgaussian"), AntennaSelection, Polarization) ){
	  cerr << "analyseLOPESevent::ProcessEvent: " << "Error during generatePlots()!" << endl;
	};
      };
    } catch (AipsError x) {
      cerr << "analyseLOPESevent::ProcessEvent: " << x.getMesg() << endl;
      return Record();
    }; 
    return erg;
  }

  // --------------------------------------------------------------- SetupEvent  
  Bool analyseLOPESevent::SetupEvent(String evname, 
				     int doTVcal,
				     Vector<Int> FlaggedAntIDs,
				     Vector<Bool> &AntennaSelection,
				     Double UpSamplingRate,
				     Double ExtraDelay,
				     Bool verbose,
				     Bool doGainCal,
				     Bool doDispersionCal,
				     Bool doDelayCal,
				     Bool doRFImitigation,
				     Bool doFlagNotActiveAnts,
				     Bool doAutoFlagging){
    try {
      pipeline_p->setVerbosity(verbose);
      // Generate the Data Reader
      if (! lev_p->attachFile(evname) ){
	cerr << "analyseLOPESevent::SetupEvent: " << "Failed to attach file: " << evname << endl;
	return False;
      };

      //  Enable/disable calibration steps of the FirstStagePipeline (must be done before InitEvent)
      //  parameters are initialized with 'true' by default
      pipeline_p->doGainCal(doGainCal);
      pipeline_p->doDispersionCal(doDispersionCal);
      pipeline_p->doDelayCal(doDelayCal);
      pipeline_p->doFlagNotActiveAnts(doFlagNotActiveAnts);
      pipeline_p->setFreqInterval(freqStart_p,freqStop_p);

      // initialize the Data Reader
      if (! pipeline_p->InitEvent(lev_p)){
	cerr << "analyseLOPESevent::SetupEvent: " << "Failed to initialize the DataReader!" << endl;
	return False;
      };

      //  Enable/disable doing the phase calibration as requested
      switch (doTVcal){
      case 0:
	pipeline_p->doPhaseCal(False);
	break;
      case 1:
	pipeline_p->doPhaseCal(True);
	break;
      default:
	if ( lev_p->headerRecord().isDefined("EventClass") &&
	     lev_p->headerRecord().asInt("EventClass") == LopesEventIn::Simulation ){
	  if (verbose){
	    cout << "Simulation event: ";
	  };
	  pipeline_p->doPhaseCal(False);
	} else {
	  pipeline_p->doPhaseCal(True);
	};
	break;
      };

      //  Enable/disable doing the RFI mitigation
      pipeline_p->doRFImitigation(doRFImitigation);

      // Generate the antenna selection
      Int i,j,id,nants,nselants, nflagged=FlaggedAntIDs.nelements();
      AntennaSelection.resize();
      AntennaSelection = pipeline_p->GetAntennaMask(lev_p);
      nants = AntennaSelection.nelements();
      Vector<Int> AntennaIDs,selAntIDs;
      lev_p->headerRecord().get("AntennaIDs",AntennaIDs);
      if (nflagged >0){
	for (i=0; i<nflagged; i++){
	  id = FlaggedAntIDs(i);
	  for (j=0; j<nants; j++){
	    if (AntennaIDs(j) == id) {
	      AntennaSelection(j) = False;
	      id = 0;
	      break;
	    };
	  };
	  if (verbose && (id !=0)){
	    cout << "analyseLOPESevent::SetupEvent: " << "AntennaID: " << id 
		 << " not found -> no antenna flagged." << endl;
	  };
	};
      };

      // Flag antennas (due to bad signal or because marked as not active in the CalTables)
      if (doAutoFlagging) {
        flagger.calcWeights(pipeline_p->GetTimeSeries(lev_p));
        AntennaSelection = AntennaSelection && flagger.parameters().asArrayBool("AntennaMask");
      }
      nselants = ntrue(AntennaSelection);

      //Do the upsampling (if requested)	
      Vector<Double> Times;
      if (UpSamplingRate >= lev_p->sampleFrequency()) {
	if (upsampler_p == NULL){ upsampler_p = new UpSampledDR(); };
	upsampler_p->setup(lev_p, UpSamplingRate, False, pipeline_p);
	upsamplePipe_p->InitEvent(upsampler_p,False);
	upsamplePipe_p->doPhaseCal(False);
	upsamplePipe_p->doRFImitigation(False);
	upsamplePipe_p->setVerbosity(verbose);
	
	filterStrength_p = (int)floor(3*UpSamplingRate/lev_p->sampleFrequency());
	//filterStrength_p = (int)ceil(UpSamplingRate/{bandwidth});

	Double tmpdouble;
	int upBlockSize,upBlock;
	// Calculate the blocksize for the upsampled data
	// smallest power of 2 that is larger tha 3*([fit or plot range]+1 microsec)
	tmpdouble = (max(fitStop_p,plotStop_p)-min(fitStart_p,plotStart_p)+1e-6)*UpSamplingRate;
	upBlockSize = (uint)(pow(2.0,ceil(log(tmpdouble*3)/log(2.0))));
	// Calculate where to start the block 
	Times = upsampler_p->timeValues();
	upBlock = ntrue( Times < ((fitStop_p+fitStart_p)/2.))/upBlockSize;
	
	
	// Set blocksize and shift
	upsampler_p->setBlocksize(upBlockSize);
	upsampler_p->setBlock(upBlock);
	Times.resize();
	if (verbose) {
	  Times = upsampler_p->timeValues();
	  cout << "analyseLOPESevent::SetupEvent: " << "Upsampling: upBlockSize: " << upBlockSize
	       << " upBlock: " << upBlock << " filter: " << filterStrength_p << endl;
	  cout << "analyseLOPESevent::SetupEvent: " << " min(Times): " << min(Times)*1e6 
	       << " max(Times): " << max(Times)*1e6 << endl;
	};
	beamformDR_p = upsampler_p;
	beamPipe_p = upsamplePipe_p;
      } else {
	beamformDR_p = lev_p;
	beamPipe_p =  pipeline_p;
      };
      
      beamPipe_p->setExtraDelay(ExtraDelay);
      Times = beamformDR_p->timeValues();
      int nsamples = Times.nelements();      
      //initialize the fitter
      remoteRange_p(0) = (uInt)(nsamples*remoteStart_p);
      remoteRange_p(1) = (uInt)(nsamples*remoteStop_p);
      fitObject.setTimeAxis(Times);
      fitObject.setRemoteRange(remoteRange_p);
      fitObject.setFitRangeSeconds(fitStart_p,fitStop_p);

    } catch (AipsError x) {
      cerr << "analyseLOPESevent::SetupEvent: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };
  

  // ---------------------------------------------------------- doPositionFitting

  Bool analyseLOPESevent::doPositionFitting(Double &Az, Double &El, Double &distance, 
					    Double &center,
					    Double &XC, Double &YC, Bool RotatePos,
					    Vector<Bool> AntennaSelection, 
					    String Polarization,
					    Bool simplexFit,
					    Bool verbose,
					    Bool distanceSearch){
    try {    
      if (Polarization != ""){
	Polarization_p = Polarization;
      };
      // Set shower position
      if (! beamPipe_p->setPhaseCenter(XC, YC, RotatePos)){
	cerr << "analyseLOPESevent::doPositionFitting: " << "Error during setPhaseCenter()!" << endl;
	return False;
      };
      //perform the position fitting
      center=-1.8e-6;
      if (simplexFit) {
        if (distanceSearch) {
	  if (! findDistance(Az, El, distance, AntennaSelection, &center, True, verbose) ){
	    cerr << "analyseLOPESevent::doPositionFitting: " << "Error during findDistance()!" << endl;
	    return False;
	  };
	};
	if (verbose) { cout << "analyseLOPESevent::doPositionFitting: starting evaluateGrid()." << endl;};
	if (! evaluateGrid(Az, El, distance, AntennaSelection, &center) ){
	  cerr << "analyseLOPESevent::doPositionFitting: " << "Error during evaluateGrid()!" << endl;
	  return False;
	};
        if (distanceSearch) {
	  if (! findDistance(Az, El, distance, AntennaSelection, &center, False, verbose) ){
	    cerr << "analyseLOPESevent::doPositionFitting: " << "Error during findDistance()!" << endl;
	    return False;
	  };
	};
	//################################## quick hack! start!!! ############################################
// 	if (verbose) { cout << "analyseLOPESevent::doPositionFitting: starting reduced SimplexFit()." << endl;};
// 	{
// 	  Vector<Bool> NewAntennaSelection;
// 	  Double height, max1=-1000., max2=-1000.;
// 	  int i, max1pos=0, max2pos=0;
// 	  Matrix<Double> ts;
// 	  NewAntennaSelection = AntennaSelection;

// 	  beamPipe_p->setDirection(Az, El, distance);
// 	  for (i=0; i<AntennaSelection.nelements(); i++){
// 	    if (AntennaSelection(i)){
// 	      NewAntennaSelection = False;
// 	      NewAntennaSelection(i) = True;
// 	      //height = getHeight(Az, El, distance, NewAntennaSelection, NULL);
// 	      ts = beamPipe_p->GetTimeSeries(beamformDR_p, NewAntennaSelection, Polarization_p);	      
// 	      height = max(abs(ts.column(0)));
// 	      if (height >= max1) {
// 		max2 = max1;
// 		max2pos = max1pos;
// 		max1 = height;
// 		max1pos = i;
// 	      } else if (height >= max2) {
// 		max2 = height;
// 		max2pos = i;
// 	      };
// 	    };
// 	  };
// 	  NewAntennaSelection = AntennaSelection;
// 	  NewAntennaSelection(max1pos) = NewAntennaSelection(max2pos) = False;
// 	  if (verbose) {cout<<"analyseLOPESevent::doPositionFitting: old Selection"<<AntennaSelection<<endl;};
// 	  if (verbose) {cout<<"analyseLOPESevent::doPositionFitting: new Selection"<<NewAntennaSelection<<endl;};
// 	  if (! SimplexFit(Az, El, distance, center, NewAntennaSelection) ){
// 	    cerr << "analyseLOPESevent::doPositionFitting: " << "Error during reducedSimplexFit()!" << endl;
// 	  };	  
// 	};
	//################################## quick hack! end!!! ############################################
	if (verbose) { cout << "analyseLOPESevent::doPositionFitting: starting SimplexFit()." << endl;};

	if (distanceSearch) {
	  if (! SimplexFit(Az, El, distance, center, AntennaSelection, 2000.) ){
  	    cerr << "analyseLOPESevent::doPositionFitting: " << "Error during SimplexFit()!" << endl;
	    return False;
	  };
	} else {
	  if (! SimplexFit(Az, El, distance, center, AntennaSelection, 2000.) ){
	    cerr << "analyseLOPESevent::doPositionFitting: " << "Error during SimplexFit()!" << endl;
	    return False;
	  };
	};
	beamPipe_p->setVerbosity(verbose);
      };
    } catch (AipsError x) {
      cerr << "analyseLOPESevent::doPositionFitting: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };
        

  // --------------------------------------------------------------- GaussFitData
  Bool analyseLOPESevent::GaussFitData(Double &Az, Double &El, Double &distance, Double &center, 
				       Vector<Bool> AntennaSelection, String evname, 
				       Record &erg, Record &fiterg,
				       String Polarization, Bool verbose){
    try {      
      Vector<Double> ccBeam, xBeam,pBeam;
      Matrix<Double> TimeSeries;
      
      if (Polarization != ""){
	Polarization_p = Polarization;
      };
      // Get the beam-formed data
      if (! beamPipe_p->setDirection(Az, El, distance)){
	cerr << "analyseLOPESevent::GaussFitData: " << "Error during setDirection()!" << endl;
	return False;
      };
      if (! beamPipe_p->GetTCXP(beamformDR_p, TimeSeries, ccBeam, xBeam, pBeam, 
				AntennaSelection, Polarization_p)){
	cerr << "analyseLOPESevent::GaussFitData: " << "Error during GetTCXP()!" << endl;
	return False;
      };
      // smooth the data
      StatisticsFilter<Double> mf(filterStrength_p,FilterType::MEAN);
      ccBeam = mf.filter(ccBeam);
      xBeam = mf.filter(xBeam);
      pBeam = mf.filter(pBeam);

      Slice remoteRegion(remoteRange_p(0),(remoteRange_p(1)-remoteRange_p(0)));
      ccBeam = ccBeam - mean(ccBeam(remoteRegion));
      xBeam = xBeam - mean(xBeam(remoteRegion));
      pBeam = pBeam - mean(pBeam(remoteRegion));

      // do the fitting
      fiterg = fitObject.Fitgauss(xBeam, ccBeam, True, center);
      if ( !fiterg.isDefined("Xconverged") || !fiterg.isDefined("CCconverged") ){
	cerr << "analyseLOPESevent::GaussFitData: " << "Error during fit!" << endl;
	return False;	
      };

      if (verbose) {
	cout << "Event \"" << evname << "\" fit results:" << endl;
	if (fiterg.asBool("Xconverged")){
	  cout << " Fit to X-Beam has converged:" << endl;
	} else {
	  cout << " Fit to X-Beam has _not_ converged:" << endl;
	};
	cout << "  XHeight: " << fiterg.asDouble("Xheight") << " +/- " 
	     <<fiterg.asDouble("Xheight_error") << endl;
	cout << "  XWidth : " << fiterg.asDouble("Xwidth") << " +/- "
	     <<fiterg.asDouble("Xwidth_error") << endl;
	cout << "  XCenter: " << fiterg.asDouble("Xcenter") << " +/- "
	     <<fiterg.asDouble("Xcenter_error") << endl;
	if (fiterg.asBool("CCconverged")){
	  cout << " Fit to CC-Beam has converged:" << endl;
	} else {
	  cout << " Fit to CC-Beam has _not_ converged:" << endl;
	};
	cout << "  CCHeight: " << fiterg.asDouble("CCheight") << " +/- " 
	     <<fiterg.asDouble("CCheight_error") << endl;
	cout << "  CCWidth : " << fiterg.asDouble("CCwidth") << " +/- "
	     <<fiterg.asDouble("CCwidth_error") << endl;
	cout << "  CCCenter: " << fiterg.asDouble("CCcenter") << " +/- "
	     <<fiterg.asDouble("CCcenter_error") << endl;
      };
      
      //copy fit results to record
      erg.mergeField(fiterg,"Xconverged", RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xheight",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xwidth",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xcenter",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xheight_error",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xwidth_error",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xcenter_error",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCconverged",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCheight",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCwidth",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCcenter",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCheight_error",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCwidth_error",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCcenter_error",RecordInterface::OverwriteDuplicates);

      // calculate other stuff
      erg.define("rmsC",stddev(ccBeam(remoteRegion)));
      erg.define("rmsX",stddev(xBeam(remoteRegion)));
      Matrix<Double> AntPos; 
      AntPos=beamPipe_p->GetAntPositions();
      AntPos = toShower(AntPos, Az, El);
      int nants = AntennaSelection.nelements();
      int nselants = ntrue(AntennaSelection);
      Vector<Double> distances(nants);
      for (int i=0; i<nants; i++) {
	distances(i) = sqrt( square(AntPos.row(i)(0)) + square(AntPos.row(i)(1)) );
      };
      erg.define("distances",distances);
      Vector<Double> tmpvec;
      tmpvec.resize(nselants);
      tmpvec = distances(AntennaSelection).getCompressedArray() ;
      erg.define("meandist",mean(tmpvec));
      erg.define("Azimuth",Az);
      erg.define("Elevation",El);
      erg.define("Distance",distance);

    } catch (AipsError x) {
      cerr << "analyseLOPESevent::GaussFitData: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }

  // --------------------------------------------------------------- doGeneratePlots
  Bool analyseLOPESevent::doGeneratePlots(String PlotPrefix, Vector<Double> ccgauss, 
					  Vector<Double> xgauss, Vector<Bool> AntennaSelection,
					  String Polarization){
    try {
      if (Polarization != ""){
	Polarization_p = Polarization;
      };
      Int nsamples,i,j,nants,nselants;
      Vector<Double> Times, ccBeam, xBeam, pBeam, tmpvec;
      Matrix<Double> TimeSeries;
      
      Times = beamformDR_p->timeValues();
      nsamples = Times.nelements();
      nants = AntennaSelection.nelements();
      if (! beamPipe_p->GetTCXP(beamformDR_p, TimeSeries, ccBeam, xBeam, pBeam, 
				AntennaSelection, Polarization_p)){
	cerr << "analyseLOPESevent::generatePlots: " << "Error during GetTCXP()!" << endl;
	return False;
      };
      nselants = TimeSeries.ncolumn();
      StatisticsFilter<Double> mf(filterStrength_p,FilterType::MEAN);
      ccBeam = mf.filter(ccBeam);
      xBeam = mf.filter(xBeam);
      pBeam = mf.filter(pBeam);
      Slice remoteRegion(remoteRange_p(0),(remoteRange_p(1)-remoteRange_p(0)));
      ccBeam = ccBeam - mean(ccBeam(remoteRegion));
      xBeam = xBeam - mean(xBeam(remoteRegion));
      pBeam = pBeam - mean(pBeam(remoteRegion));
      i = ntrue(Times<plotStart_p);
      j = ntrue(Times<plotStop_p);
      Slice plotRegion(i,(j-i));
      Matrix<Double> parts((j-i),nselants);
      Vector<Double> xarr, yarr, empty, gauss;
      String plotname;
      SimplePlot plotter;
      xarr = Times(plotRegion) * 1e6;
      //plot the CC-beam
      plotname = PlotPrefix;
      plotname += "-CC.ps";
      yarr = ccBeam(plotRegion) * 1e6;
      plotter.quickPlot(plotname, xarr, yarr, empty, empty,
			"Time [#gmseconds]", "CC-Beam [#gmV/m/MHz]","",9,
			True, 1, False, False, True);
      yarr = pBeam(plotRegion) *1e6;
      plotter.PlotLine(xarr,yarr,4,3);
      yarr = ccgauss(plotRegion) *1e6;
      plotter.PlotLine(xarr,yarr,8,2);
      // plot the X-Beam
      plotname = PlotPrefix;
      plotname += "-X.ps";
      yarr = xBeam(plotRegion) * 1e6;
      plotter.quickPlot(plotname, xarr, yarr, empty, empty,
			"Time [#gmseconds]", "X-Beam [#gmV/m/MHz]","",9,
			True, 1, False, False, True);
      yarr = pBeam(plotRegion) *1e6;
      plotter.PlotLine(xarr,yarr,4,3);
      yarr = xgauss(plotRegion) *1e6;
      plotter.PlotLine(xarr,yarr,8,2);
      // plot all antenna traces
      plotname = PlotPrefix;
      plotname += "-all.ps";
      Double ymax,ymin,xmax,xmin,tmpval;
      Vector<Int> AntennaIDs,selAntIDs;
      beamformDR_p->headerRecord().get("AntennaIDs",AntennaIDs);
      selAntIDs.resize(nselants);
      AntennaIDs(AntennaSelection).getCompressedArray(selAntIDs);
      tmpvec.resize(nselants);
      for (i=0; i<nselants; i++){
	  parts.column(i) = TimeSeries.column(i)(plotRegion)*1e6;
	  tmpvec(i) = max(abs(parts.column(i)));
      };
      //ymax = max(parts); ymin = min(parts);
      //ymax = 2*abs(erg.asDouble("CCheight")); ymin = -ymax;
      ymax = median(tmpvec)*1.05; ymin = -ymax;
      xmin = min(xarr); xmax = max(xarr);
      tmpval = (xmax-xmin)*0.05;
      xmin -= tmpval;
      xmax += tmpval;
      plotter.InitPlot(plotname, xmin, xmax, ymin, ymax);
      plotter.AddLabels("Time [#gmseconds]", "FieldStrength [#gmV/m/MHz]");
      Int col1,col2,col3;
      col1=col2=col3=3;
      for (i=0; i<nselants; i++){
	if (selAntIDs(i) > 70000) { 
	  plotter.PlotLine(xarr,parts.column(i),col3++,3);
	} else if (selAntIDs(i) > 40000) {
	  plotter.PlotLine(xarr,parts.column(i),col2++,2);
	} else {
	  plotter.PlotLine(xarr,parts.column(i),col1++,1);
	};
      };
    } catch (AipsError x) {
      cerr << "analyseLOPESevent::generatePlots: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }

  // ------------------------------------------------------------------- toShower

  Matrix<Double> analyseLOPESevent::toShower (const Matrix<Double> & pos,
					      Double Az,
					      Double El){
    Matrix<Double> erg;
    try {
      if (pos.ncolumn() != 3) {
	cerr << "analyseLOPESevent:toShower: " << "invalis parameters: pos.ncolumn() != 3 " << endl;
	return Matrix<Double>();
      };
      erg.resize(pos.shape());

      Double azrad,elrad;
      azrad = Az*DEG2RAD;
      elrad = El*DEG2RAD;

      Double l1, l2, l3, m1, m2, m3, n1, n2, n3;
      
      l1 = -sin(azrad);
      l2 = cos(azrad);
      l3 = 0.;

      m1 = sin(elrad)*cos(azrad);
      m2 = sin(elrad)*sin(azrad);
      m3 = -cos(elrad);

      n1 = cos(elrad)*cos(azrad);
      n2 = cos(elrad)*sin(azrad);
      n3 = sin(elrad);
      
      erg.column(0) = pos.column(0)*l1 + pos.column(1)*l2;
      erg.column(1) = pos.column(0)*m1 + pos.column(1)*m2 + pos.column(2)*m3;
      erg.column(2) = pos.column(0)*n1 + pos.column(1)*n2 + pos.column(2)*n3;

    } catch (AipsError x) {
      cerr << "analyseLOPESevent:toShower: " << x.getMesg() << endl;
      return Matrix<Double>();
    }; 
    return erg;
  };
  

  Bool analyseLOPESevent::SimplexFit (Double &Az,
				      Double &El,
				      Double &distance,
				      Double &center, 
				      Vector<Bool> AntennaSelection,
				      Double distanceStep){    
    try {
      Int i,minpos,maxpos,niteration=0,oldminpos,nsameiter;
      Record erg;
      Vector<Double> azs(4), els(4), dists(4), height(4,0.), cents(4);
      Double newaz, newel, newdist, newheight, nnewheight, newcent, nnewcent;
      Double meanaz, meanel, meandist, vecaz, vecel, vecdist;
      Bool running=True;
      
      if (beamPipe_p == NULL){
	cerr << "analyseLOPESevent:SimplexFit: " << "Error: beamPipe_p == NULL " << endl;
	return False;	
      };
      if (beamformDR_p == NULL){
	cerr << "analyseLOPESevent:SimplexFit: " << "Error: beamformDR_p == NULL " << endl;
	return False;	
      };
      beamPipe_p->setVerbosity(False);

      // set start values
      azs = Az; els = El; dists = distance;
      azs(1) += 1; els(2) += 1; 
      azs(3) -= 1; els(3) -= 1;      
      dists(2) -= distanceStep;dists(1) += distanceStep;dists(3) += distanceStep;
      cents = center;
      for (i=0; i<4; i++){
	height(i) = getHeight(azs(i), els(i), dists(i), AntennaSelection, &cents(i));
      };
      minpos=0; nsameiter=0;oldminpos=0;
      while(running){ // the big loop
	// find worst point
	for (i=0; i<4; i++){
	  if ((i!=minpos) && (height(i)<height(minpos))){
	    minpos = i;
	  };
	};
	// find the vector from minpoint to centerpoint
	meanaz = meanel = meandist = 0.;
	for (i=0; i<4; i++){
	  if (i!=minpos){
	    meanaz += azs(i); meanel += els(i); meandist += dists(i);
	  };
	};
	meanaz /= 3.; meanel /= 3.; meandist /= 3.;
	vecaz = meanaz-azs(minpos); vecel = meanel-els(minpos); vecdist = meandist-dists(minpos);
	if (minpos == oldminpos){
	  nsameiter++;
	} else {
	  nsameiter=0;
	  oldminpos = minpos;
	};
	if ( (nsameiter==8) || ((abs(vecaz)<0.03) && (abs(vecel)<0.03) && (abs(vecdist)<1))) {
	  // restart from close to best position
	  maxpos=0;
	  for (i=1; i<4; i++){
	    if (height(i)>height(maxpos)){
	      maxpos = i;
	    };
	  };
	  azs(minpos) = azs(maxpos)+0.2;
	  els(minpos) = els(maxpos)+0.2;
	  dists(minpos) = dists(maxpos)+50;
	  cents(minpos) = cents(maxpos);
	  height(minpos) = getHeight(azs(minpos),els(minpos),dists(minpos), AntennaSelection, &cents(minpos));
	  continue;
	};
	if (nsameiter==16) {
	  // We are seriously stuck. Just give up...
	  running = False;
	}
	//new point
	newaz = azs(minpos) + 2.*vecaz;	newel = els(minpos) + 2.*vecel;
	newdist = dists(minpos) + 2.*vecdist;
	newcent = cents(minpos);
	newheight = getHeight(newaz, newel, newdist, AntennaSelection, &newcent);
	if (newheight > max(height)) { //expand the simplex
	  cout << " SimplexFit: expanding simplex"<<endl;
	  newaz = azs(minpos) + 4.*vecaz; newel = els(minpos) + 4.*vecel;
	  newdist = dists(minpos) + 4.*vecdist;
	  nnewcent = cents(minpos);
	  nnewheight = getHeight(newaz, newel, newdist, AntennaSelection, &nnewcent);
	  if (newheight > nnewheight) {
	    newaz = azs(minpos) + 2.*vecaz; newel = els(minpos) + 2.*vecel;
	    newdist = dists(minpos) + 2.*vecdist;
	  } else {
	    newheight = nnewheight;
	    newcent = nnewcent;
	  };
	} else {
	  height(minpos) = max(height);
	  if (newheight < min(height)) { 
	    cout << " SimplexFit: reducing simplex"<<endl;
	    newaz = azs(minpos) + 1.5*vecaz; newel = els(minpos) + 1.5*vecel;
	    newdist = dists(minpos) + 1.5*vecdist;
	    nnewcent = cents(minpos);
	    nnewheight = getHeight(newaz, newel, newdist, AntennaSelection, &nnewcent);
	    if (newheight > nnewheight) {
	      newaz = azs(minpos) + 2.*vecaz; newel = els(minpos) + 2.*vecel;
	      newdist = dists(minpos) + 2.*vecdist;
	    } else {
	      newheight = nnewheight;
	      newcent = nnewcent;
	    };
	  };
	};
	// save the new position
	Bool clipping=True;
	while (clipping){
	  if (newel>90.) { newel=180.-newel; continue;};
	  if (newel<0.) { newel=-newel; continue;};
	  if (newdist<0.) { newdist=-newdist; continue;};
	  clipping = False;
	};
	azs(minpos) = newaz; els(minpos) = newel; dists(minpos) = newdist; cents(minpos) = newcent;
	height(minpos)=newheight;
	// generate output
	printf("## %i ------------------------------- \n",niteration);
	printf("  Azimuth:   %6.2f; %6.2f; %6.2f; %6.2f \n",azs(0),azs(1),azs(2),azs(3));
	printf("  Elevation: %6.2f; %6.2f; %6.2f; %6.2f \n",els(0),els(1),els(2),els(3));
	printf("  Distance:  %6.1f; %6.1f; %6.1f; %6.1f \n",dists(0),dists(1),dists(2),dists(3));
	printf("  Center:    %6.3f; %6.3f; %6.3f; %6.3f \n",cents(0)*1e6,cents(1)*1e6,cents(2)*1e6,cents(3)*1e6);
	printf("  Height:    %6.3f; %6.3f; %6.3f; %6.3f \n",height(0),height(1),height(2),height(3));
	// test convergence
	meanaz = max(azs)-min(azs); meanel = max(els)-min(els);
	meanaz *= cos(mean(els)*DEG2RAD);
	meanaz = abs(meanaz*meanel);
	meandist = max(dists)-min(dists);
	if ((meanaz<0.01)&&((meandist/mean(dists))<0.025)) {
	  running = False;
	};
	niteration++;
	if (niteration > 100) {
	  running = False;
	};
      }; // end of the big loop
      // calculate mean position
      meanaz = meanel = meandist = 0.;
      for (i=0; i<4; i++){
	meanaz += azs(i); meanel += els(i); meandist += dists(i);
      };
      meanaz /= 4.; meanel /= 4.; meandist /= 4.;
      Bool clipping=True;
      while (clipping){
	if (meanaz>360.) { meanaz-=360.; continue;};
	if (meanaz<0.) { meanaz+=360.; continue;};
	if (meanel>90.) { meanel=180.-meanel; continue;};
	if (meanel<0.) { meanel=-meanel; continue;};
	if (meandist<0.) { meandist=-meandist; continue;};
	clipping = False;
      };
      Az = meanaz; El = meanel; distance = meandist; center = mean(cents);

    } catch (AipsError x) {
      cerr << "analyseLOPESevent:SimplexFit: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };

  Bool analyseLOPESevent::evaluateGrid (Double &Az,
					Double &El,
					Double &distance,
					Vector<Bool> AntennaSelection,
					Double *centerp){
    try {
      int estep,astep,arange,erange=3;
      Double maxaz=Az,maxel=El,maxheight=0.,maxcenter=-1.8e-6;
      Double az_,el_,height_,center_;
      Double stepfactor=1.;

      if (beamPipe_p == NULL){
	cerr << "analyseLOPESevent:evaluateGrid: " << "Error: beamPipe_p == NULL " << endl;
	return False;	
      };
      if (beamformDR_p == NULL){
	cerr << "analyseLOPESevent:evaluateGrid: " << "Error: beamformDR_p == NULL " << endl;
	return False;	
      };

      beamPipe_p->setVerbosity(False);

      for (estep=-erange; estep<=erange; estep++){
	el_ = El + stepfactor*estep;
	arange = erange-abs(estep);
	for (astep=-arange; astep<=arange; astep++){
	  az_ = Az+astep*stepfactor/(cos(el_*DEG2RAD)+1e-6);
	  if (el_>90.){ az_+= 180.; };
	  center_ = 2e99;
	  height_ = getHeight(az_, el_, distance, AntennaSelection, &center_);
	  if (height_ > maxheight){
	    maxaz = az_; maxel = el_; maxheight=height_; maxcenter=center_;
	  };
	};
      };
      Bool clipping=True;
      while (clipping){
	if (maxaz>360.) { maxaz-=360.; continue;};
	if (maxaz<0.) { maxaz+=360.; continue;};
	if (maxel>90.) { maxel=180.-maxel; continue;};
	if (maxel<0.) { maxel=-maxel; continue;};
	clipping = False;
      };
      Az = maxaz; El = maxel;
      if (centerp != NULL) {
	*centerp = maxcenter;
      };
    } catch (AipsError x) {
      cerr << "analyseLOPESevent:evaluateGrid: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };
  
  
  Double analyseLOPESevent::getHeight (Double az,
				       Double el,
				       Double dist, 
				       Vector<Bool> AntennaSelection,
				       Double *centerp){
    Double erg;
    try {
      Vector<Double> ccb,xb,pb;
      Matrix<Double> ts;
      Record ergrec;
      Bool clipping=True, goodfit=True;
      while (clipping){
	if (az>360.) { az-=360.; continue;};
	if (az<0.) { az+=360.; continue;};
	if (el>90.) { el=180.-el; continue;};
	if (el<0.) { el=-el; continue;};
	if (dist<0.) { dist=-dist; continue;};
	clipping = False;
      };
      beamPipe_p->setDirection(az, el, dist);
      beamPipe_p->GetTCXP(beamformDR_p, ts, ccb, xb, pb, AntennaSelection, Polarization_p);
      StatisticsFilter<Double> mf(filterStrength_p,FilterType::MEAN);
      ccb = mf.filter(ccb);
      xb = mf.filter(xb);
      if (centerp != NULL) {
	ergrec = fitObject.Fitgauss(xb, ccb, False, *centerp);
      } else {
	ergrec = fitObject.Fitgauss(xb, ccb, False);
      };
      erg = ergrec.asDouble("CCheight")*1e6;
      if (centerp != NULL) {
	*centerp = ergrec.asDouble("CCcenter");
      };
      if (!ergrec.asBool("CCconverged") || 
	  (ergrec.asDouble("CCcenter") < fitStart_p) || (ergrec.asDouble("CCcenter") > fitStop_p) ||
	  (ergrec.asDouble("CCwidth")>1e-7) || (ergrec.asDouble("CCwidth")<1e-9) ||
	  (ergrec.asDouble("CCcenter_error")>(fitStop_p-fitStart_p))||
	  (ergrec.asDouble("CCwidth_error")>1e-7)||
	  ((ergrec.asDouble("CCheight_error")/ergrec.asDouble("CCheight"))>10) ) { 
	goodfit=False;
	pb = beamformDR_p->timeValues();
	erg = mean(ccb(Slice(ntrue(pb<-1.83e-6),5)))*1e6;
	if (centerp != NULL) {
	  *centerp = -1.8e-6;
	};
      };
#ifdef DEBUGGING_MESSAGES      
      cout << "getHeight: Az:"<<az<<" El:"<<el<<" Dist:"<<dist<<" Height:"<< erg << " conv:"<<
	ergrec.asBool("Xconverged")<<ergrec.asBool("CCconverged")<<goodfit<< " Center:"<< ergrec.asDouble("CCcenter")<<endl;
#endif
    } catch (AipsError x) {
      cerr << "analyseLOPESevent:getHeight: " << x.getMesg() << endl;
      return 0.;
    }; 
    return erg;
  };


  Bool analyseLOPESevent::findDistance( Double &Az,
		      			Double &El,
				        Double &distance,
				        Vector<Bool> AntennaSelection, 
				        Double *centerp,
					Bool rough,
					Bool verbose){
    try {
      Double height_, maxheight=0., maxdist=2500., center_=-1.8e-6, maxcenter=0.;
      for(int d=2000; d<=15000; ){
        height_=getHeight(Az, El, d, AntennaSelection, &center_);
	// if the fit was not succesfull, center is set to -1.8e-6. This case has to be excluded
        // Why???? I kicked that out.
	//if (height_ > maxheight && center_ !=-1.8e-6) {
	if (height_ > maxheight) {
  	  maxheight = height_;
	  maxdist = d;
	  maxcenter = center_;
        };

        if (rough) d+=1000;
        else d+=200;
      };
      distance = maxdist;
      if (centerp != NULL) {
        *centerp = maxcenter;
      };
      if (verbose) { cout << "findDistance: best Distance: "<<distance<<endl; };
    } catch (AipsError x) {
	cerr << "analyseLOPESevent:findDistance: "<< x.getMesg() <<endl;
	return false;
    };
    return true;
  };

} // Namespace CR -- end
