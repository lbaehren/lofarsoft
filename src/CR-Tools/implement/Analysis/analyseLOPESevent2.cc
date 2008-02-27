/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
 *   Frank Schroeder (<mail>)                                                     *
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
    upsamplingExponent(0)
  {;}
  
  analyseLOPESevent2::analyseLOPESevent2 (analyseLOPESevent2 const &other):
    upsamplingExponent(0)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  analyseLOPESevent2::~analyseLOPESevent2 ()
  {
    destroy();
  }
  
  void analyseLOPESevent2::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  analyseLOPESevent2& analyseLOPESevent2::operator= (analyseLOPESevent2 const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void analyseLOPESevent2::copy (analyseLOPESevent2 const &other)
  {;}

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
  
  
  Bool analyseLOPESevent2::initPipeline(Record ObsRecord){
    try {
      clear();
      pipeline_p= new CompletePipeline();
      lev_p = new LopesEventIn();
      pipeline_p->SetObsRecord(ObsRecord);
      cout << "\nPipeline initialised." << endl;
      
    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::initPipeline: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }; 

  // --------------------------------------------------------------- ProcessEvent

  Record analyseLOPESevent2::ProcessEvent(String evname,
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
					  bool SinglePlots){
    Record erg;
    try {
//      ofstream latexfile;			// WARNING: causes problem in fitCR2gauss.cc line 200
      Int nsamples;
      Vector<Double> Times, ccBeam, xBeam, pBeam, tmpvec;
      Matrix<Double> TimeSeries;
      Record fiterg;
      
      pipeline_p->setVerbosity(verbose);
      // Generate the Data Reader
      if (! lev_p->attachFile(evname) ){
	cerr << "analyseLOPESevent2::ProcessEvent: " << "Failed to attach file: " << evname << endl;
	return Record();
      };
      // initialize the Data Reader
      if (! pipeline_p->InitEvent(lev_p)){
	cerr << "analyseLOPESevent2::ProcessEvent: " << "Failed to initialize the DataReader!" << endl;
	return Record();
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
	if ( lev_p->header().isDefined("EventClass") &&
	     lev_p->header().asInt("EventClass") == LopesEventIn::Simulation ){
	  if (verbose){
	    cout << "Simulation event: ";
	  };
	  pipeline_p->doPhaseCal(False);
	} else {
	  pipeline_p->doPhaseCal(True);
	};
	break;
      };
      
      
      // Generate the antenna selection
      Vector <Bool> AntennaSelection;
      Int i,j,id,nants,nselants, nflagged=FlaggedAntIDs.nelements();
      AntennaSelection = pipeline_p->GetAntennaMask(lev_p);
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

      //initialize the pipeline
      Times = lev_p->timeValues();
      nsamples = Times.nelements();
      if (! pipeline_p->setPhaseCenter(XC, YC, RotatePos)){
	cerr << "analyseLOPESevent2::ProcessEvent: " << "Error during setPhaseCenter()!" << endl;
	return Record();
      };

      //Flagg antennas
      flagger.calcWeights(pipeline_p->GetTimeSeries(lev_p));
      AntennaSelection = AntennaSelection && flagger.parameters().asArrayBool("AntennaMask");
      nselants = ntrue(AntennaSelection);


      //initialize the fitter
      Vector<uInt> remoteRange(2,0);
      remoteRange(0) = (uInt)(nsamples*remoteStart_p);
      remoteRange(1) = (uInt)(nsamples*remoteStop_p);
      fitObject.setTimeAxis(Times);
      fitObject.setRemoteRange(remoteRange);
      fitObject.setFitRangeSeconds(fitStart_p,fitStop_p);

      //perform the position fitting
      Double center;
      if (simplexFit) {
	if (verbose) { cout << "analyseLOPESevent2::ProcessEvent: starting evaluateGrid()." << endl;};
	if (! evaluateGrid(Az, El, distance, AntennaSelection, &center) ){
	  cerr << "analyseLOPESevent2::ProcessEvent: " << "Error during evaluateGrid()!" << endl;
	  return Record();
	};
	if (verbose) { cout << "analyseLOPESevent2::ProcessEvent: starting SimplexFit()." << endl;};
	//if (! SimplexFit2(Az, El, distance, center, AntennaSelection) ){
	//  cerr << "analyseLOPESevent::ProcessEvent: " << "Error during SimplexFit2()!" << endl;
	//  return Record();
	//};
	if (! SimplexFit(Az, El, distance, center, AntennaSelection) ){
	  cerr << "analyseLOPESevent2::ProcessEvent: " << "Error during SimplexFit()!" << endl;
	  return Record();
	};
	pipeline_p->setVerbosity(verbose);
      };
      
      // Get the beam-formed data
      if (! pipeline_p->setDirection(Az, El, distance)){
	cerr << "analyseLOPESevent2::ProcessEvent: " << "Error during setDirection()!" << endl;
	return Record();
      };
      if (! pipeline_p->GetTCXP(lev_p, TimeSeries, ccBeam, xBeam, pBeam, AntennaSelection)){
	cerr << "analyseLOPESevent2::ProcessEvent: " << "Error during GetTCXP()!" << endl;
	return Record();
      };
      // smooth the data
      StatisticsFilter<Double> mf(3,FilterType::MEAN);
      ccBeam = mf.filter(ccBeam);
      xBeam = mf.filter(xBeam);
      pBeam = mf.filter(pBeam);

      Slice remoteRegion(remoteRange(0),(remoteRange(1)-remoteRange(0)));
      ccBeam = ccBeam - mean(ccBeam(remoteRegion));
      xBeam = xBeam - mean(xBeam(remoteRegion));
      pBeam = pBeam - mean(pBeam(remoteRegion));

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
      Matrix<Double> AntPos=pipeline_p->GetAntPositions();
      AntPos = toShower(AntPos, Az, El);
      Vector<Double> distances(nants);

      for (i=0; i<nants; i++) {
	distances(i) = sqrt( square(AntPos.row(i)(0)) + square(AntPos.row(i)(1)) );
      };
      erg.define("distances",distances);
      tmpvec.resize(nselants);
      tmpvec = distances(AntennaSelection).getCompressedArray() ;
      erg.define("meandist",mean(tmpvec));
      erg.define("Azimuth",Az);
      erg.define("Elevation",El);
      erg.define("Distance",distance);

      // Generate the plots
      if (generatePlots)
      {
        pipeline_p->setPlotInterval(plotStart(),plotStop());
        pipeline_p->plotCCbeam(PlotPrefix + "-CC", lev_p, AntennaSelection);
        pipeline_p->plotXbeam(PlotPrefix + "-X", lev_p, AntennaSelection);
        // Plot of all antenna traces together
        pipeline_p->plotAllAntennas(PlotPrefix + "-all", lev_p, AntennaSelection, false, getUpsamplingExponent());
        // Plot of upsampled antenna traces (seperated)
        if (SinglePlots)
	  pipeline_p->plotAllAntennas(PlotPrefix, lev_p, AntennaSelection, true, getUpsamplingExponent());

        if (verbose)		// give out the names of the created plots
        {
          vector<string> plotlist = pipeline_p->getPlotList();
          std::cout <<"\nList of generated plots:\n";
          for (int i = 0; i < plotlist.size(); i++) std::cout << plotlist[i] << "\n";
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
        latexfile << "\\usepackage{psfig,a4wide}\n";
        latexfile << "\\setlength{\\textheight}{24cm}\n";
        latexfile << "\\setlength{\\topmargin}{0.0cm}\n";
        latexfile << "\\parindent=0pt\n";
        latexfile << "\\begin{document}\n";

        // get list of generated plots and loop through the list
        vector<string> plotlist = pipeline_p->getPlotList();

        for (int i = 0; i < plotlist.size(); i++)
        {
          // line break after #columns plots
          if ( (i > 0) && ((i % columns) == 0) ) latexfile << "\\newline\n";

          // add plot to latexfile
          latexfile << "\\begin{minipage}[h]{" << (0.999/columns) << "\\textwidth}\\psfig{figure="
              << plotlist[i] << ",width=\\textwidth,angle=-90}\\end{minipage}\n";
        }
        // finish the latex file and close it
        latexfile << "\\end{document}\n";
        latexfile.close();

        std::cout << "Created latex file: " << latexfilename << std::endl;

        // execute latex without creating output at the term
        string shellCommand = "latex " + latexfilename +" > /dev/null";
        system(shellCommand.c_str());

	// execute dvips to create postscript file
        shellCommand = "dvips " + filename + " > /dev/null";
        system(shellCommand.c_str());
      }
    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::summaryPlot: " << x.getMesg() << endl;
    }
  }

} // Namespace CR -- end
