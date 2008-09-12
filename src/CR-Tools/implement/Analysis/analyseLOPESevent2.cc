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

#ifdef HAVE_STARTOOLS

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  analyseLOPESevent2::analyseLOPESevent2 ()
    : analyseLOPESevent(),
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
    pipeline_p         = NULL;
    upsamplePipe_p     = NULL;
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

      // use pointers if code from analyseLOPESevent.cc is reused
      lev_p = new LopesEventIn();
      pipeline_p->SetObsRecord(ObsRecord);
      upsamplePipe_p->SetObsRecord(ObsRecord);

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
					  bool doGainCal,
					  bool doDispersionCal,
					  bool doDelayCal,
					  bool doRFImitigation,
					  Double UpSamplingRate,
					  String Polarization,
					  bool SinglePlots,
					  bool PlotRawData,
  					  bool CalculateMaxima,
					  bool listCalcMaxima,
					  bool printShowerCoordinates) {
    Record erg;
    try {
      // ofstream latexfile;  // WARNING: causes problem in fitCR2gauss.cc line 200, left here for future tests
      Vector <Bool> AntennaSelection;
      Record fiterg;
      Double center;		// position of the cc-beam (set in doPositionFitting)

      // initialize the pipeline
      if (! SetupEvent(evname, doTVcal, FlaggedAntIDs, AntennaSelection, 
		       UpSamplingRate, ExtraDelay, verbose, doGainCal, doDispersionCal, doDelayCal, doRFImitigation)) {
	cerr << "analyseLOPESevent2::RunPipeline: " << "Error during SetupEvent()!" << endl;
	return Record();
      };

      // storte GT in return record
      erg.mergeField(lev_p->headerRecord(),"Date", RecordInterface::OverwriteDuplicates);

      // set Complete Pipeline and print output, which case is set!
      CompleteBeamPipe_p = static_cast<CompletePipeline*>(beamPipe_p);
      if (verbose)
        if (CompleteBeamPipe_p == pipeline_p)
 		 cout << "analyseLOPESevent2::RunPipeline: " << "Proceed without (new) upsampling." << endl;
        else
 		 cout << "analyseLOPESevent2::RunPipeline: " << "(New) upsampling enabled." << endl;

      // initialize Complete Pipeline
      CompleteBeamPipe_p->setPlotInterval(plotStart(),plotStop());
      CompleteBeamPipe_p->setCCWindowWidth(getCCWindowWidth());
      CompleteBeamPipe_p->setPolarization(Polarization);
      CompleteBeamPipe_p->setCalibrationMode(false);

      // Plot the raw data, if desired
      if (PlotRawData)
      {
	// plot upsampled raw data: either in seperated plots seperated or all traces in one plot
        if (SinglePlots)
          CompleteBeamPipe_p->plotAllAntennas(PlotPrefix+ "-raw", beamformDR_p, AntennaSelection, true,
                                              getUpsamplingExponent(),true);
        else
          CompleteBeamPipe_p->plotAllAntennas(PlotPrefix + "-raw", beamformDR_p, AntennaSelection, false,
                                              getUpsamplingExponent(), true);

	// calculate the maxima
        if (CalculateMaxima) CompleteBeamPipe_p->calculateMaxima(beamformDR_p, AntennaSelection, getUpsamplingExponent(), true);
      }

      //perform the position fitting (if simplexFit = false, then only the PhaseCenter is set)
      if (! doPositionFitting(Az, El, distance, center, XC, YC, RotatePos,
			      AntennaSelection, Polarization, simplexFit, verbose) ){
	cerr << "analyseLOPESevent2::RunPipeline: " << "Error during doPositionFitting()!" << endl;
	return Record();
      };

      // make gauss fit to CC-beam
      if (! GaussFitData(Az, El, distance, center, AntennaSelection, evname, erg, fiterg, 
			 Polarization, verbose) ){
	cerr << "analyseLOPESevent2::RunPipeline: " << "Error during GaussFitData()!" << endl;
	return Record();
      };


      // output of antnenna to core distances in shower coordinates, if requested
      if (printShowerCoordinates) printAntennaDistances(erg.asArrayDouble("distances"),
                                                         toShower(beamPipe_p->GetAntPositions(), Az, El),	
                                                         El, Az, XC, YC, beamformDR_p->headerRecord().asInt("Date"));
      // Generate plots
      if (generatePlots)
      {
        // Plot CC-beam; if fit has converged, then also plot the result of the fit
    	if (fiterg.asBool("CCconverged"))
          CompleteBeamPipe_p->plotCCbeam(PlotPrefix + "-CC", beamformDR_p, fiterg.asArrayDouble("Cgaussian"),
                                         AntennaSelection, filterStrength_p, remoteRange_p(0), remoteRange_p(1));
        else
          CompleteBeamPipe_p->plotCCbeam(PlotPrefix + "-CC", beamformDR_p, Vector<Double>(),
                                         AntennaSelection, filterStrength_p, remoteRange_p(0), remoteRange_p(1));

        // Plot X-beam; if fit has converged, then also plot the result of the fit
    	if (fiterg.asBool("Xconverged"))
          CompleteBeamPipe_p->plotXbeam(PlotPrefix + "-X", beamformDR_p, fiterg.asArrayDouble("Xgaussian"),
                                        AntennaSelection, filterStrength_p, remoteRange_p(0), remoteRange_p(1) );
        else
          CompleteBeamPipe_p->plotXbeam(PlotPrefix + "-X", beamformDR_p, Vector<Double>(),
                                        AntennaSelection, filterStrength_p, remoteRange_p(0), remoteRange_p(1));

        // Plot of all antenna traces together
        CompleteBeamPipe_p->plotAllAntennas(PlotPrefix + "-all", beamformDR_p, AntennaSelection, false,
                                            getUpsamplingExponent(),false);

	// Plot of upsampled antenna traces (seperated) 
        if (SinglePlots)
          CompleteBeamPipe_p->plotAllAntennas(PlotPrefix, beamformDR_p, AntennaSelection, true,
                                              getUpsamplingExponent(),false);

        // calculate the maxima
	if (CalculateMaxima)
          CompleteBeamPipe_p->calculateMaxima(beamformDR_p, AntennaSelection, getUpsamplingExponent(), false);
        // user friendly list of calculated maxima
        if (listCalcMaxima)
          CompleteBeamPipe_p->listCalcMaxima(beamformDR_p, AntennaSelection, getUpsamplingExponent(),fiterg.asDouble("CCcenter"));
      };

      // give out the names of the created plots
      if (verbose)		
      {
        vector<string> plotlist = CompleteBeamPipe_p->getPlotList();
        std::cout <<"\nList of generated plots:\n";
        for (unsigned int i = 0; i < plotlist.size(); i++) std::cout << plotlist[i] << "\n";
        std::cout << std::endl;
      }

    } catch (AipsError x) 
    {
      std::cerr << "analyseLOPESevent2::RunPipeline: " << x.getMesg() << std::endl;
      return Record();
    } 
    return erg;
  }


  // -------------------------------------- Process Calibration Event

  Record analyseLOPESevent2::CalibrationPipeline (const string& evname,
						  string PlotPrefix, 
						  Bool generatePlots,
						  Vector<Int> FlaggedAntIDs, 
						  Bool verbose,
						  bool doDispersionCal,
						  bool doRFImitigation,
						  bool SinglePlots,
						  bool PlotRawData,
						  bool CalculateMaxima)
  {
    Record erg;
    try {
      // ofstream latexfile;  // WARNING: causes problem in fitCR2gauss.cc line 200, left here for future tests
      Vector <Bool> AntennaSelection;


      pipeline_p->setVerbosity(verbose);
      // Generate the Data Reader
      if (! lev_p->attachFile(evname) ){
	cerr << "analyseLOPESevent::SetupEvent: " << "Failed to attach file: " << evname << endl;
	return Record();
      };

      //  Enable/disable calibration steps of the FirstStagePipeline (must be done before InitEvent)
      //  parameters are initialized with 'true' by default
      pipeline_p->doGainCal(false);
      pipeline_p->doDispersionCal(doDispersionCal);
      pipeline_p->doDelayCal(false);

      // initialize the Data Reader
      if (! pipeline_p->InitEvent(lev_p)){
	cerr << "analyseLOPESevent2::CalibrationPipeline: " << "Failed to initialize the DataReader!" << endl;
	return Record();
      };

      //  Enable/disable calibration steps for the SecondStagePipeline
      pipeline_p->doPhaseCal(false);
      pipeline_p->doRFImitigation(doRFImitigation);

      // Generate the antenna selection
      Int i,j,id,nants, nflagged=FlaggedAntIDs.nelements();
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

      // storte GT in return record
      erg.mergeField(lev_p->headerRecord(),"Date", RecordInterface::OverwriteDuplicates);

      // initialize Complete Pipeline
      CompleteBeamPipe_p = static_cast<CompletePipeline*>(pipeline_p);
      CompleteBeamPipe_p->setPlotInterval(plotStart(),plotStop());
      CompleteBeamPipe_p->setCalibrationMode(true);

      // Plot the raw data, if desired
      if (PlotRawData)
      {
	// plot upsampled raw data: either in seperated plots seperated or all traces in one plot
        if (SinglePlots)
          CompleteBeamPipe_p->plotAllAntennas(PlotPrefix+ "-raw", lev_p, AntennaSelection, true,
                                              getUpsamplingExponent(),true);
        else
          CompleteBeamPipe_p->plotAllAntennas(PlotPrefix + "-raw", lev_p, AntennaSelection, false,
                                              getUpsamplingExponent(), true);

	// calculate the maxima
        if (CalculateMaxima) CompleteBeamPipe_p->calculateMaxima(lev_p, AntennaSelection, getUpsamplingExponent(), true);
      }

      // Generate plots
      if (generatePlots)
      {
        // Plot of upsampled antenna traces (seperated with envelope or together without envelope) 
        if (SinglePlots)
          CompleteBeamPipe_p->plotAllAntennas(PlotPrefix, lev_p, AntennaSelection, true,
                                              getUpsamplingExponent(),false, true);
        else
          CompleteBeamPipe_p->plotAllAntennas(PlotPrefix + "-all", lev_p, AntennaSelection, false,
                                              getUpsamplingExponent(),false, false);

        // calculate the maxima
	if (CalculateMaxima)
          CompleteBeamPipe_p->calculateMaxima(lev_p, AntennaSelection, getUpsamplingExponent(), false);
      };

      // give out the names of the created plots
      if (verbose)		
      {
        vector<string> plotlist = CompleteBeamPipe_p->getPlotList();
        std::cout <<"\nList of generated plots:\n";
        for (unsigned int i = 0; i < plotlist.size(); i++) std::cout << plotlist[i] << "\n";
        std::cout << std::endl;
      }

    } catch (AipsError x) 
    {
      std::cerr << "analyseLOPESevent2::CalibrationPipeline: " << x.getMesg() << std::endl;
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
        string shellCommand = "latex " + latexfilename + " > /dev/null";  // don't show the output to stdout
        system(shellCommand.c_str());

	// execute dvips to create postscript file
        shellCommand = "dvips " + filename + " 2> /dev/null"; // don't show the output to stderr
        system(shellCommand.c_str());

        // delete temporary files
        shellCommand = "rm -f " + filename + ".aux "
                                + filename + ".dvi "
                                + filename + ".tex "
                                + filename + ".log ";
        system(shellCommand.c_str());


        std::cout << "Created postscript summary: " << filename << ".ps" << std::endl;
      }
    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::summaryPlot: " << x.getMesg() << endl;
    }
  }

} // Namespace CR -- end


#endif
