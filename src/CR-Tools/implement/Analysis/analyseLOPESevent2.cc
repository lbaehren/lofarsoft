/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Frank Schroeder (<mail>)                                              *
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

#include <Analysis/analyseLOPESevent2.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  analyseLOPESevent2::analyseLOPESevent2 ():
    CompleteBeamPipe_p(NULL),
    upsamplingExponent(0),
    ccWindowWidth_p(0.045e-6)
  {;}
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  analyseLOPESevent2::~analyseLOPESevent2 ()
  {
    // Set pipeline_p back to NULL (to avoid that analyseLOPESevent deletes pipeline)
    pipeline_p = NULL;
    upsamplePipe_p = NULL;
    CompleteBeamPipe_p = NULL;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void analyseLOPESevent2::summary (std::ostream &os)
  {;}
  
 
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // --------------------------------------------------------------- initPipeline
  
  Bool analyseLOPESevent2::initPipeline(Record ObsRecord){
    try {
      clear();
      // To use the methods of analyseLOPESevent, a pointer to the pipeline is needed.
      pipeline_p = &pipeline;
      upsamplePipe_p = &upsamplePipe;
      lev_p = new LopesEventIn();
      pipeline.SetObsRecord(ObsRecord);
      upsamplePipe.SetObsRecord(ObsRecord);
      cout << "\nanalyseLOPESevent2: Pipeline initialised." << endl;
    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::initPipeline: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }; 

  // --------------------------------------------------------------- ProcessEvent

  Record analyseLOPESevent2::RunPipeline(const string& evname,
					  Double Az,
					  Double El,
					  Double distance, 
					  Double XC,
					  Double YC,
					  Bool RotatePos,
					  string PlotPrefix, 
					  Bool generatePlots,
					  Vector<Int> FlaggedAntIDs, 
					  Bool verbose,
					  Bool simplexFit,
					  Double ExtraDelay,
					  int doTVcal,
					  Double UpSamplingRate,
					  bool SinglePlots,
					  bool PlotRawData,
  					  bool CalculateMaxima,
					  bool listCalcMaxima,
					  bool printShowerCoordinates) {
    Record erg;
    try {
      string Polarization = "ANY";
      // ofstream latexfile;  // WARNING: causes problem in fitCR2gauss.cc line 200, left here for future tests
      Int nsamples;
      Vector<Double> Times, ccBeam, xBeam, pBeam, tmpvec;
      Matrix<Double> TimeSeries;
      Record fiterg;
      Double center;		// position of the cc-beam (set in doPositionFitting)


      // set parameters of the pipeline
      pipeline.setVerbosity(verbose);
      pipeline.setPlotInterval(plotStart(),plotStop());
      pipeline.setCCWindowWidth(getCCWindowWidth());
      upsamplePipe.setVerbosity(verbose);
      upsamplePipe.setPlotInterval(plotStart(),plotStop());
      upsamplePipe.setCCWindowWidth(getCCWindowWidth());

      // Generate the Data Reader
      if (! lev_p->attachFile(evname) ){
	cerr << "analyseLOPESevent2::ProcessEvent: " << "Failed to attach file: " << evname << endl;
	return Record();
      };
      // initialize the Data Reader
      if (! pipeline.InitEvent(lev_p)){
	cerr << "analyseLOPESevent2::ProcessEvent: " << "Failed to initialize the DataReader!" << endl;
	return Record();
      };

      //  Enable/disable doing the phase calibration as requested
      switch (doTVcal){
      case 0:
	pipeline.doPhaseCal(False);
	break;
      case 1:
	pipeline.doPhaseCal(True);
	break;
      default:
	if ( lev_p->header().isDefined("EventClass") &&
	     lev_p->header().asInt("EventClass") == LopesEventIn::Simulation ){
	  if (verbose){
	    cout << "Simulation event: ";
	  };
	  pipeline.doPhaseCal(False);
	} else {
	  pipeline.doPhaseCal(True);
	};
	break;
      };
      
      
      // Generate the antenna selection
      Vector <Bool> AntennaSelection;
      Int i,j,id,nants,nselants, nflagged=FlaggedAntIDs.nelements();
      AntennaSelection = pipeline.GetAntennaMask(lev_p);
      nants = AntennaSelection.nelements();
      Vector<Int> AntennaIDs,selAntIDs;
      lev_p->header().get("AntennaIDs",AntennaIDs);
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
	    cout << "analyseLOPESevent2::ProcessEvent: " << "AntennaID: " << id 
		 << " not found -> no antenna flagged." << endl;
	  };
	};
      };

      
      // Plot the raw data, if desired
      if (PlotRawData)       
      {
	// plot upsampled raw data: either in seperated plots seperated or all traces in one plot
        if (SinglePlots)
          pipeline.plotAllAntennas(PlotPrefix+ "-raw", lev_p, AntennaSelection, true, getUpsamplingExponent(),true);
        else
          pipeline.plotAllAntennas(PlotPrefix + "-raw", lev_p, AntennaSelection, false, getUpsamplingExponent(), true);
          
	// calculate the maxima
        if (CalculateMaxima) pipeline.calculateMaxima(lev_p, AntennaSelection, getUpsamplingExponent(), true);
      }

      // if there are no plots and now fits requested, stop here
      if (!generatePlots && !simplexFit)
      {
        if (verbose)		// give out the names of the created plots
        {
          vector<string> plotlist = pipeline.getPlotList();
          std::cout <<"\nList of generated plots:\n";
          for (unsigned int i = 0; i < plotlist.size(); i++) std::cout << plotlist[i] << "\n";
          std::cout << std::endl;
        }
        return Record();
      }

      //Flagg antennas
      flagger.calcWeights(pipeline.GetTimeSeries(lev_p));
      AntennaSelection = AntennaSelection && flagger.parameters().asArrayBool("AntennaMask");
      nselants = ntrue(AntennaSelection);

      //Do the upsampling (if requested)
      if (UpSamplingRate >= lev_p->sampleFrequency()) {
	if (upsampler_p == NULL){ upsampler_p = new UpSampledDR(); };
	upsampler_p->setup(lev_p, UpSamplingRate, False, pipeline_p);
	upsamplePipe.InitEvent(upsampler_p,False);
	upsamplePipe.doPhaseCal(False);
	upsamplePipe.doRFImitigation(False);
	upsamplePipe.setVerbosity(verbose);
	
	filterStrength_p = (int)ceil(3*UpSamplingRate/lev_p->sampleFrequency());

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
	  cout << "analyseLOPESevent2::RunPipeline: " << "Upsampling: upBlockSize: " << upBlockSize
	       << " upBlock: " << upBlock << endl;
	  cout << "analyseLOPESevent2::RunPipeline: " << " min(Times): " << min(Times)*1e6 
	       << " max(Times): " << max(Times)*1e6 << endl;
	};
        // Set DataReader and Pipeline object for upsampling
	beamformDR_p = upsampler_p;
	beamPipe_p = &upsamplePipe;
	CompleteBeamPipe_p = &upsamplePipe;
      } else {
        // Set DataReader and Pipeline object for standard (no upsampling)
	beamformDR_p = lev_p;
	beamPipe_p = &pipeline;
	CompleteBeamPipe_p = &pipeline;
        if (verbose) cout << "analyseLOPESevent2::RunPipeline: " << "Proceed without (new) upsampling." << endl;
      };

      //initialize the pipeline
      Times = beamformDR_p->timeValues();
      nsamples = Times.nelements();

      //initialize the fitter
      Vector<uInt> remoteRange(2,0);
      remoteRange(0) = (uInt)(nsamples*remoteStart_p);
      remoteRange(1) = (uInt)(nsamples*remoteStop_p);
      fitObject.setTimeAxis(Times);
      fitObject.setRemoteRange(remoteRange);
      fitObject.setFitRangeSeconds(fitStart_p,fitStop_p);

      //perform the position fitting (if simplexFit = false, then only the PhaseCenter is set)
      if (! doPositionFitting(Az, El, distance, center, XC, YC, RotatePos,
			      AntennaSelection, Polarization, simplexFit, verbose) ){
	cerr << "analyseLOPESevent2::RunPipeline: " << "Error during doPositionFitting()!" << endl;
	return Record();
      };

      // Get the beam-formed data
      if (! beamPipe_p->setDirection(Az, El, distance)){
	cerr << "analyseLOPESevent2::RunPipeline: " << "Error during setDirection()!" << endl;
	return Record();
      };
      if (! beamPipe_p->GetTCXP(beamformDR_p, TimeSeries, ccBeam, xBeam, pBeam, AntennaSelection)){
	cerr << "analyseLOPESevent2::RunPipeline: " << "Error during GetTCXP()!" << endl;
	return Record();
      };
      
      // smooth the data
      StatisticsFilter<Double> mf(filterStrength_p,FilterType::MEAN);
      ccBeam = mf.filter(ccBeam);
      xBeam = mf.filter(xBeam);
      pBeam = mf.filter(pBeam);

      // Calculate noise in the beamforming direction and store it for later subtraction
      Slice remoteRegion(remoteRange(0),(remoteRange(1)-remoteRange(0)));
      double ccBeamMean = mean(ccBeam(remoteRegion));
      double xBeamMean = mean(xBeam(remoteRegion));
      double pBeamMean = mean(pBeam(remoteRegion));
      ccBeam -= ccBeamMean;
      xBeam -= xBeamMean;
      pBeam -= pBeamMean;

      
      // do the fitting
      fiterg = fitObject.Fitgauss(xBeam, ccBeam, True, center);
      if ( !fiterg.isDefined("Xconverged") || !fiterg.isDefined("CCconverged") ){
	cerr << "analyseLOPESevent2::ProcessEvent: " << "Error during fit!" << endl;
	return Record();	
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
      Vector<Double> distances(nants);

      // calculate distances between antennas and shower core in shower coordinates
       for (i=0; i<nants; i++) {
	distances(i) = sqrt( square(AntPos.row(i)(0)) + square(AntPos.row(i)(1)) );
      }

      erg.define("distances",distances);
      tmpvec.resize(nselants);
      tmpvec = distances(AntennaSelection).getCompressedArray() ;
      erg.define("meandist",mean(tmpvec));
      erg.define("Azimuth",Az);
      erg.define("Elevation",El);
      erg.define("Distance",distance);

      // output of antnenna to core distances in shower coordinates, if requested
      if (printShowerCoordinates) printAntennaDistances(erg.asArrayDouble("distances"),
                                                         toShower(beamPipe_p->GetAntPositions(), Az, El),	
                                                         El, Az, XC, YC, beamformDR_p->header().asInt("Date"));

      // Generate the plots
      if (generatePlots)
      {
        // Plot CC-beam; if fit has converged, then also plot the result of the fit
    	if (fiterg.asBool("CCconverged"))
          CompleteBeamPipe_p->plotCCbeam(PlotPrefix + "-CC", beamformDR_p, fiterg.asArrayDouble("Cgaussian"), AntennaSelection, ccBeamMean, pBeamMean);
        else
          CompleteBeamPipe_p->plotCCbeam(PlotPrefix + "-CC", beamformDR_p, Vector<Double>(), AntennaSelection, ccBeamMean, pBeamMean);

        // Plot X-beam; if fit has converged, then also plot the result of the fit
    	if (fiterg.asBool("Xconverged"))
          CompleteBeamPipe_p->plotXbeam(PlotPrefix + "-X", beamformDR_p, fiterg.asArrayDouble("Xgaussian"), AntennaSelection, xBeamMean, pBeamMean);
        else
          CompleteBeamPipe_p->plotXbeam(PlotPrefix + "-X", beamformDR_p, Vector<Double>(), AntennaSelection, xBeamMean, pBeamMean);

        // Plot of all antenna traces together
        CompleteBeamPipe_p->plotAllAntennas(PlotPrefix + "-all", beamformDR_p, AntennaSelection, false, getUpsamplingExponent(),false);
        
	// Plot of upsampled antenna traces (seperated) 
        if (SinglePlots)
          CompleteBeamPipe_p->plotAllAntennas(PlotPrefix, beamformDR_p, AntennaSelection, true, getUpsamplingExponent(),false);

        // calculate the maxima
	if (CalculateMaxima) CompleteBeamPipe_p->calculateMaxima(beamformDR_p, AntennaSelection, getUpsamplingExponent(), false);
        // user friendly list of calculated maxima
        if (listCalcMaxima) CompleteBeamPipe_p->listCalcMaxima(beamformDR_p, AntennaSelection, getUpsamplingExponent(),fiterg.asDouble("CCcenter"));
        
        if (verbose)		// give out the names of the created plots
        {
          vector<string> plotlist = CompleteBeamPipe_p->getPlotList();
          std::cout <<"\nList of generated plots:\n";
          for (unsigned int i = 0; i < plotlist.size(); i++) std::cout << plotlist[i] << "\n";
          std::cout << std::endl;
        }
      };
    } catch (AipsError x) 
    {
      std::cerr << "analyseLOPESevent2::ProcessEvent: " << x.getMesg() << std::endl;
      return Record();
    } 
    return erg;
  }


  void analyseLOPESevent2::printAntennaDistances (const Vector <double>& distances,
                                                  const Matrix<double>& AntPos,
                                                  const double& Az,
                                                  const double& El,
                                                  const double& Xc,
                                                  const double& Yc,
                                                  const unsigned int date) const
  {
    try 
    {
      // check if size is consistent
      if ( AntPos.nrow() != distances.size())
      {
        cerr << "analyseLOPESevent2::printAntennaDistances: " 
             << "ERROR: number of distances and antenna positions are different!" <<  endl;
        return;		// exit function without showing (confusing) results
      }

      // print header
      std::cout << "GT "<< date << " " << Az << " " << El << " " << Xc << " " <<Yc <<std::endl;
      std::cout << "An  dist_x    dist_y    dist"<<std::endl;

      // loop through all antennas in print the distances (should be in shower coordinates)
      for (unsigned int i=0 ; i < distances.size(); i++)
      {
        std::cout << std::setw(2) << i+1 << " " << std::setw(8) << AntPos.row(i)(0) << "  ";
        std::cout << std::setw(8) << AntPos.row(i)(1) << "  " << std::setw(8) <<distances(i)<<std::endl;
      }
    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::printAntennaDistances: " << x.getMesg() << endl;
    }
  }




  void analyseLOPESevent2::summaryPlot(string filename,
                                       unsigned int columns)
  {
    try 
    {
      // check if anything should be done (means columns must be > 0)
      if (columns <= 0) return;

      // create latexfilename
      string latexfilename = filename + ".tex";

      // open latexfile
      ofstream latexfile;
      latexfile.open(latexfilename.c_str(), ofstream::out);

      // check if file could be opened
      if (!(latexfile.is_open()))
      {
        std::cerr << "Failed to write to file \"" << latexfilename <<"\"." << std::endl;
      } else 
      {
        // Create header for latex file
        latexfile << "\\documentclass[a4paper]{article}\n";
        latexfile << "\\usepackage{graphicx,a4wide}\n";
        latexfile << "\\setlength{\\textheight}{24cm}\n";
        latexfile << "\\setlength{\\topmargin}{0.0cm}\n";
        latexfile << "\\parindent=0pt\n";
        latexfile << "\\begin{document}\n";

        // get list of generated plots and loop through the list
        vector<string> plotlist = pipeline.getPlotList();

        for (unsigned int i = 0; i < plotlist.size(); i++)
        {
          // line break after #columns plots
          if ( (i > 0) && ((i % columns) == 0) ) latexfile << "\\newline\n";

          // add plot to latexfile
          latexfile << "\\begin{minipage}[h]{" << (0.999/columns) 
                    << "\\textwidth}\\includegraphics[angle=-90,width=\\textwidth]{"
                    << plotlist[i] << "}\\end{minipage}\n";	
        }
        // finish the latex file and close it
        latexfile << "\\end{document}\n";
        latexfile.close();

        // execute latex without creating output at the term
        string shellCommand = "latex " + latexfilename +" > /dev/null";  // don't show the output to stdout
        system(shellCommand.c_str());

	// execute dvips to create postscript file
        shellCommand = "dvips " + filename + " 2> /dev/null"; // don't show the output to stderr
        system(shellCommand.c_str());

        std::cout << "Created postscript summary: " << filename << ".ps" << std::endl;
      }
    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::summaryPlot: " << x.getMesg() << endl;
    }
  }

} // Namespace CR -- end
