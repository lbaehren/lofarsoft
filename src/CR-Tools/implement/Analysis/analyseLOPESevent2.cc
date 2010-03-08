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

#include "TROOT.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TPaveStats.h"
#include "TAxis.h"

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
      minBeamformingAntennas(4),
      upsamplingExponent(0),
      ccWindowWidth_p(0.045e-6),
      spectrumStart_p(40e6),
      spectrumStop_p(80e6),
      rawPulses( map<int,PulseProperties>() ),
      calibPulses( map<int,PulseProperties>() ),
      lateralSNRcut(1.0),
      lateralTimeCut(25e-9),
      coreError(0.),
      zenithError(0.),
      azimuthError(0.)
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

  void analyseLOPESevent2::clear() {
    rawPulses = map<int,PulseProperties>();
    calibPulses = map<int,PulseProperties>();

    // from analyseLOPESevent::clear():
    if (lev_p != NULL) {
      delete lev_p;
      lev_p = NULL;
    }
    if (upsampler_p != NULL) {
      delete upsampler_p;
      upsampler_p = NULL;
    }
    
    pipeline.init();
    upsamplePipe.init();

    // To use the methods of analyseLOPESevent, a pointer to the pipeline is needed.
    pipeline_p = &pipeline;
    upsamplePipe_p = &upsamplePipe;
  }

  Bool analyseLOPESevent2::initPipeline(Record ObsRecord){
    try {
      clear();

      // use pointers if code from analyseLOPESevent.cc is reused
      lev_p = new LopesEventIn();
      pipeline_p->SetObsRecord(ObsRecord);
      upsamplePipe_p->SetObsRecord(ObsRecord);

      cout << "\nanalyseLOPESevent2: Pipeline initialised." << endl;
    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::initPipeline: " << x.getMesg() << endl;
      return False;
    }
    return True;
  }

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
                                         Bool generateSpectra,
                                         Vector<Int> FlaggedAntIDs, 
                                         Bool verbose,
                                         Bool simplexFit,
                                         Double ExtraDelay,
                                         int doTVcal,
                                         bool doGainCal,
                                         bool doDispersionCal,
                                         bool doDelayCal,
                                         bool doRFImitigation,
                                         bool doFlagNotActiveAnts,
                                         bool doAutoFlagging,
                                         Double UpSamplingRate,
                                         String Polarization,
                                         bool SinglePlots,
                                         bool PlotRawData,
                                         bool CalculateMaxima,
                                         bool listCalcMaxima,
                                         bool printShowerCoordinates,
                                         bool ignoreDistance,
                                         double randomDelay,
                                         unsigned int randomSeed) {
    Record erg;
    try {
      // ofstream latexfile;  // WARNING: causes problem in fitCR2gauss.cc line 200, left here for future tests
      Vector <Bool> AntennaSelection;
      Record fiterg;
      Double center;                // position of the cc-beam (set in doPositionFitting)

      // define default values for return record, to prevent crash of call_pipeline
      // in the case of any error during RunPipeline
      erg.define("goodReconstructed",false);  // will be set to true at the end of the reconstruction
      erg.define("Azimuth",double(0));
      erg.define("Elevation",double(0));
      erg.define("Distance",double(0));
      erg.define("CCheight",double(0));
      erg.define("CCwidth",double(0));
      erg.define("CCheight_error",double(0));
      erg.define("CCconverged",false);
      erg.define("Xheight",double(0));
      erg.define("Xheight_error",double(0));
      erg.define("Xconverged",false);
      erg.define("rmsCCbeam",double(0));
      erg.define("rmsXbeam",double(0));
      erg.define("rmsPbeam",double(0));
      erg.define("meandist",double(0));
      erg.define("Date",uInt(0));
      erg.define("NCCbeamAntennas",uInt(0));


      // store a copy of the input antenna selection for later use
      InputFlaggedAntIDs = FlaggedAntIDs.copy();

      // initialize the pipeline (flag not active antennas and antennas with bad signals)
      if (! SetupEvent(evname, doTVcal, FlaggedAntIDs, AntennaSelection, 
                       UpSamplingRate, ExtraDelay, verbose, doGainCal, doDispersionCal, doDelayCal, doRFImitigation,
                       doFlagNotActiveAnts, doAutoFlagging)) {
        cerr << "analyseLOPESevent2::RunPipeline: " << "Error during SetupEvent()!" << endl;
        return erg;
      }

      // Check if any antennas are left unflagged (otherwise quit)
      if ( ntrue(AntennaSelection) == 0) {
        cerr << "analyseLOPESevent2::RunPipeline: " << "All antennas flagged!" << endl;
        return erg;
      }

      // storte GT in return record
      erg.mergeField(lev_p->headerRecord(),"Date", RecordInterface::OverwriteDuplicates);

      // set Complete Pipeline and print output, which case is set!
      CompleteBeamPipe_p = static_cast<CompletePipeline*>(beamPipe_p);
      if (verbose) {
        if (CompleteBeamPipe_p == pipeline_p)
          cout << "analyseLOPESevent2::RunPipeline: " << "Proceed without (new) upsampling." << endl;
        else
          cout << "analyseLOPESevent2::RunPipeline: " << "(New) upsampling enabled." << endl;
      }

      // initialize Complete Pipeline
      CompleteBeamPipe_p->setPlotInterval(plotStart(),plotStop());
      CompleteBeamPipe_p->setCCWindowWidth(getCCWindowWidth());
      CompleteBeamPipe_p->setSpectrumInterval(getSpectrumStart(),getSpectrumStop());
      CompleteBeamPipe_p->setPolarization(Polarization);
      CompleteBeamPipe_p->setCalibrationMode(false);

      // create antenna selection of correct polarization (for Steffen's lateral output)
      PolarizationAntennaSelection = AntennaSelection.copy();
      PolarizationAntennaSelection.set(true);
      CompleteBeamPipe_p->deselectectPolarization(beamformDR_p,PolarizationAntennaSelection);

      // flag antennas with wrong polarization for normal analysis
      CompleteBeamPipe_p->deselectectPolarization(beamformDR_p,AntennaSelection);

      // Plot the raw data, if desired
      if (PlotRawData) {
        // plot upsampled raw data: either in seperated plots seperated or all traces in one plot
        if (SinglePlots)
          CompleteBeamPipe_p->plotAllAntennas(PlotPrefix+ "-raw", beamformDR_p, AntennaSelection, true,
                                              getUpsamplingExponent(),true);
        else
          CompleteBeamPipe_p->plotAllAntennas(PlotPrefix + "-raw", beamformDR_p, AntennaSelection, false,
                                              getUpsamplingExponent(), true);

        // calculate the maxima
        if (CalculateMaxima)
          rawPulses = CompleteBeamPipe_p->calculateMaxima(beamformDR_p, AntennaSelection, getUpsamplingExponent(), true);
      }

      // add random delay for timing uncertainty studies
      if (randomDelay > 0.)
        CompleteBeamPipe_p->addRandomDelay(beamformDR_p,randomDelay, randomSeed);

      // Check if there are enough antennas left unflagged (otherwise quit)
      // beam forming is not useful with only 3 or less antennas.
      if ( ntrue(AntennaSelection) < minBeamformingAntennas) {
        cerr << "analyseLOPESevent2::RunPipeline: " 
             << "Bad event: only "
             << ntrue(AntennaSelection)
             << " antenna(s) left unflagged (at least "
             << minBeamformingAntennas
             << " required)."
             << endl;
        return erg;
      }
      // store number of antennas used for CC beam
      erg.define("NCCbeamAntennas",uInt(ntrue(AntennaSelection)));


      //perform the position fitting (if simplexFit = false, then only the PhaseCenter is set)
      if (! doPositionFitting(Az, El, distance, center, XC, YC, RotatePos,
                              AntennaSelection, Polarization, simplexFit, verbose, ignoreDistance) ){
        cerr << "analyseLOPESevent2::RunPipeline: " << "Error during doPositionFitting()!" << endl;
        return erg;
      }

      // make gauss fit to CC-beam
      if (! GaussFitData(Az, El, distance, center, AntennaSelection, evname, erg, fiterg, 
                         Polarization, verbose) ){
        cerr << "analyseLOPESevent2::RunPipeline: " << "Error during GaussFitData()!" << endl;
        return erg;
      }

      // Generate plots
      if (generatePlots) {
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

        // calculate the maxima (only if CC-beam was reconstructed successfully):
        // calculate noise as mean of local maxima of the envelope in time range of -10.5 to -0.5 µs before CC center)
        if (fiterg.asBool("CCconverged")) {
          if (CalculateMaxima)
            calibPulses = CompleteBeamPipe_p->calculateMaxima(beamformDR_p, AntennaSelection, getUpsamplingExponent(),
                                                            false, fiterg.asDouble("CCcenter"),
                                                            3, fiterg.asDouble("CCcenter") - 10.5e-6, fiterg.asDouble("CCcenter") - 0.5e-6);
          // user friendly list of calculated maxima
          if (listCalcMaxima)
            CompleteBeamPipe_p->listCalcMaxima(beamformDR_p, AntennaSelection, getUpsamplingExponent(),fiterg.asDouble("CCcenter"));
        }    
      }

      // Generate spectra
      if (generateSpectra) {
        if (SinglePlots)
          CompleteBeamPipe_p->plotSpectra(PlotPrefix+"-spec", beamformDR_p, AntennaSelection, true);
        else
          CompleteBeamPipe_p->plotSpectra(PlotPrefix + "-spec-all", beamformDR_p, AntennaSelection, false);
      }

      // output of antnenna to core distances in shower coordinates, if requested
      if (printShowerCoordinates) printAntennaDistances(erg.asArrayDouble("distances"),
                                                         toShower(beamPipe_p->GetAntPositions(), Az, El),
                                                         El, Az, XC, YC, beamformDR_p->headerRecord().asInt("Date"));
      storeShowerCoordinates(erg);        // store shower coordinates in every case

      // give out the names of the created plots
      if (verbose) {
        vector<string> plotlist = CompleteBeamPipe_p->getPlotList();
        cout <<"\nList of generated plots:\n";
        for (unsigned int i = 0; i < plotlist.size(); i++) 
          cout << plotlist[i] << "\n";
        cout << endl;
      }


      // calculate the rms values of p-, cc- and xbeam in the remote region
      if (remoteRange_p(1) != 0 ) {
        Slice remoteRegion( remoteRange_p(0),(remoteRange_p(1) - remoteRange_p(0)) );
        Vector<Double> ccbeam, xbeam, pbeam;

        // get the beam data
        ccbeam = CompleteBeamPipe_p->GetCCBeam(beamformDR_p, AntennaSelection, Polarization).copy();
        xbeam = CompleteBeamPipe_p->GetXBeam(beamformDR_p, AntennaSelection, Polarization).copy();
        pbeam = CompleteBeamPipe_p->GetPBeam(beamformDR_p, AntennaSelection, Polarization).copy();

        // smooth the data
        if (filterStrength_p > 0) {
            StatisticsFilter<Double> mf(filterStrength_p,FilterType::MEAN);
          ccbeam = mf.filter(ccbeam);
          pbeam = mf.filter(pbeam);
          xbeam = mf.filter(xbeam);
        }

        // removing offset
        double ccBeamOffset = mean(ccbeam(remoteRegion));
        double pBeamOffset  = mean(pbeam(remoteRegion));
        double xBeamOffset  = mean(xbeam(remoteRegion));
        ccbeam -= ccBeamOffset;
        pbeam  -= pBeamOffset;
        xbeam  -= xBeamOffset;

        // calculating rms values
        erg.define("rmsCCbeam",rms(ccbeam(remoteRegion)));
        erg.define("rmsPbeam",rms(pbeam(remoteRegion)));
        erg.define("rmsXbeam",rms(xbeam(remoteRegion)));
      }

      // print resutls of reconstruction
      cout << "\nRestuls of reconstruction:\n"
           << "Azimuth = " << erg.asDouble("Azimuth") << " degree \t"
           << "Elevation = " << erg.asDouble("Elevation") << " degree\n"
           << "Radius of curvature (of CC-beam) = " << erg.asDouble("Distance") << " m\n"
           << "CC-beam = " << erg.asDouble("CCheight")*1e6 << " µV/m/MHz \t"
           << " X-beam = " << erg.asDouble("Xheight")*1e6  << " µV/m/MHz \n"
           << endl;
 
     if ( (fiterg.asBool("CCconverged")) && (fiterg.asBool("Xconverged")) )
       erg.define("goodReconstructed",true);  // assume that everything was fine, then this position is reached.
    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::RunPipeline: " << x.getMesg() << endl;
      return Record();
    }
    return erg;
  }


  // -------------------------------------- Process Calibration Event

  Record analyseLOPESevent2::CalibrationPipeline (const string& evname,
                                                  string PlotPrefix, 
                                                  Bool generatePlots,
                                                  Bool generateSpectra,
                                                  Vector<Int> FlaggedAntIDs, 
                                                  Bool verbose,
                                                  bool doGainCal,
                                                  bool doDispersionCal,
                                                  bool doDelayCal,
                                                  bool doRFImitigation,
                                                  bool SinglePlots,
                                                  bool PlotRawData,
                                                  bool CalculateMaxima)
  {
    Record erg;
    try {
      // ofstream latexfile;  // WARNING: causes problem in fitCR2gauss.cc line 200, left here for future tests
      Vector <Bool> AntennaSelection;

      erg.define("goodReconstructed",false);  // will be set to true at the end of the reconstruction

      // store a copy of the input antenna selection for later use
      InputFlaggedAntIDs = FlaggedAntIDs.copy();

      pipeline_p->setVerbosity(verbose);
      // Generate the Data Reader
      if (! lev_p->attachFile(evname) ) {
        cerr << "analyseLOPESevent::SetupEvent: " << "Failed to attach file: " << evname << endl;
        return Record();
      }

      //  Enable/disable calibration steps of the FirstStagePipeline (must be done before InitEvent)
      //  parameters are initialized with 'true' by default
      pipeline_p->doGainCal(doGainCal);
      pipeline_p->doDispersionCal(doDispersionCal);
      pipeline_p->doDelayCal(doDelayCal);
      pipeline_p->doFlagNotActiveAnts(false);                // use all antennas in calibration mode
      pipeline_p->setFreqInterval(getFreqStart(),getFreqStop());

      // initialize the Data Reader
      if (! pipeline_p->InitEvent(lev_p)) {
        cerr << "analyseLOPESevent2::CalibrationPipeline: " << "Failed to initialize the DataReader!" << endl;
        return Record();
      }

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
          }
        }
      }

      // storte GT in return record
      erg.mergeField(lev_p->headerRecord(),"Date", RecordInterface::OverwriteDuplicates);

      // initialize Complete Pipeline
      CompleteBeamPipe_p = static_cast<CompletePipeline*>(pipeline_p);
      CompleteBeamPipe_p->setPlotInterval(plotStart(),plotStop());
      CompleteBeamPipe_p->setSpectrumInterval(getSpectrumStart(),getSpectrumStop());
      CompleteBeamPipe_p->setCalibrationMode(true);

      // Plot the raw data, if desired
      if (PlotRawData)
      {
        // plot upsampled raw data: either in seperated plots seperated or all traces in one plot
        if (SinglePlots)
          CompleteBeamPipe_p->plotAllAntennas(PlotPrefix+ "-raw", lev_p, AntennaSelection, true,
                                              getUpsamplingExponent(),true, true);
        else
          CompleteBeamPipe_p->plotAllAntennas(PlotPrefix + "-raw", lev_p, AntennaSelection, false,
                                              getUpsamplingExponent(), true, false);

        // calculate the maxima
        if (CalculateMaxima)
          rawPulses = CompleteBeamPipe_p->calculateMaxima(lev_p, AntennaSelection, getUpsamplingExponent(), true);
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
          calibPulses = CompleteBeamPipe_p->calculateMaxima(lev_p, AntennaSelection, getUpsamplingExponent(), false);
      }

      // Generate spectra
      if (generateSpectra) {
        if (SinglePlots)
          CompleteBeamPipe_p->plotSpectra(PlotPrefix+"-spec", lev_p, AntennaSelection, true);
        else
          CompleteBeamPipe_p->plotSpectra(PlotPrefix + "-spec-all", lev_p, AntennaSelection, false);
      }

      // give out the names of the created plots
      if (verbose) {
        vector<string> plotlist = CompleteBeamPipe_p->getPlotList();
        cout <<"\nList of generated plots:\n";
        for (unsigned int i = 0; i < plotlist.size(); i++) 
          cout << plotlist[i] << "\n";
        cout << endl;
      }

      erg.define("goodReconstructed",true);  // assume that everything was fine, then this position is reached.

    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::CalibrationPipeline: " << x.getMesg() << endl;
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
    try {
      // check if size is consistent
      if ( AntPos.nrow() != distances.size()) {
        cerr << "analyseLOPESevent2::printAntennaDistances: " 
             << "ERROR: number of distances and antenna positions are different!" <<  endl;
        return;                // exit function without showing (confusing) results
      }

      // print header
      std::cout << "GT "<< date << " " << Az << " " << El << " " << Xc << " " <<Yc <<std::endl;
      std::cout << "An  dist_x    dist_y    dist"<<std::endl;

      // loop through all antennas in print the distances (should be in shower coordinates)
      for (unsigned int i=0 ; i < distances.size(); i++) {
        std::cout << std::setw(2) << i+1 << " " << std::setw(8) << AntPos.row(i)(0) << "  ";
        std::cout << std::setw(8) << AntPos.row(i)(1) << "  " << std::setw(8) <<distances(i)<<std::endl;
      }
    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::printAntennaDistances: " << x.getMesg() << endl;
    }
  }



 void analyseLOPESevent2::storeShowerCoordinates (const Record& erg)
  {
    try {
      // get AntennaIDs to store pulse parameters in corresponding map
      Vector<int> antennaIDs;
      beamformDR_p->headerRecord().get("AntennaIDs",antennaIDs);

      // get antenna positions and distances in shower coordinates
      Vector <double> distances = erg.asArrayDouble("distances");
      // get antenna positions (shifted, so that core position is at 0)
      Matrix <double> antPos = beamPipe_p->GetAntPositions(beamformDR_p);
      // get antenna positions in shower coordinates
      Matrix <double> antPosS = toShower(beamPipe_p->GetAntPositions(beamformDR_p), erg.asDouble("Azimuth"), erg.asDouble("Elevation"));

      // for error calculation
      // errors of GPS measurement (< 1-2 cm) is neglected
      // possible shift between KASCADE and LOPES coordinate system ~20 cm is also negelected
      // calcualte squared errors for x, y, and z in shower coordinates
      // formulas: error propagation of transformation to shower coordinates
      double sinEl = sin(erg.asDouble("Elevation")*PI/180.);
      double cosEl = cos(erg.asDouble("Elevation")*PI/180.);
      double sinAz = sin(erg.asDouble("Azimuth")*PI/180.);
      double cosAz = cos(erg.asDouble("Azimuth")*PI/180.);

      // loop through antenna IDs and write distances in pulseProperties if element exists
      for (unsigned int i=0 ; i < antennaIDs.size(); i++)
        if (calibPulses.find(antennaIDs(i)) != calibPulses.end()) {
          // substitution for better readablitiy
          double x = antPos.row(i)(0);
          double y = antPos.row(i)(1);
          // double z = antPos.row(i)(2); // not used
          double xS = antPosS.row(i)(0);
          double yS = antPosS.row(i)(1);
          double zS = antPosS.row(i)(2);
          calibPulses[antennaIDs(i)].distX    = xS;
          calibPulses[antennaIDs(i)].distY    = yS;
          calibPulses[antennaIDs(i)].distZ    = zS;
          calibPulses[antennaIDs(i)].dist     = distances(i);  // already calculated: dist = sqrt(xS^2+yS^2), no zS!
          // calcualte errors
          calibPulses[antennaIDs(i)].distXerr = sqrt(  pow(coreError, 2)
                                                     + pow(( x*cosAz + y*sinAz)*azimuthError, 2)); 
          calibPulses[antennaIDs(i)].distYerr = sqrt(  pow(coreError*sinEl, 2)
                                                     + pow((-x*sinAz + y*cosAz)*azimuthError*sinEl, 2)
                                                     + pow(( x*cosAz + y*sinAz)*zenithError *cosEl, 2)); 
          // The error of z due to the core uncertainty should affect all z-values exactly in the same direction.
          // That means a wrong core position should either decrease or increase all z-values the same way.
          // Thus, it should not be considered as statistical error in any fit.
          // Remark: for x and y this is also true, but the lateral distance R can increase or decrease
          //         for different antennas if the core is wrong. Hence, for x and y the core uncertainty
          //         cannot be negelected (maybe it can be paritially neglected, but I have not investigated this).
          calibPulses[antennaIDs(i)].distZerr = sqrt(  // pow(coreError*cosEl, 2)
                                                     + pow((-x*sinAz + y*cosAz)*azimuthError*cosEl, 2)
                                                     + pow(( x*cosAz + y*sinAz)*zenithError *sinEl, 2)); 
                                                    
          calibPulses[antennaIDs(i)].disterr  = sqrt(  pow(xS*calibPulses[antennaIDs(i)].distXerr,2) 
                                                     + pow(yS*calibPulses[antennaIDs(i)].distYerr,2))
                                                / calibPulses[antennaIDs(i)].dist;
          // for debug output                                                
          /* cout << calibPulses[antennaIDs(i)].distX << " +/-" << calibPulses[antennaIDs(i)].distXerr << "\t "
               << calibPulses[antennaIDs(i)].distY << " +/-" << calibPulses[antennaIDs(i)].distYerr << "\t "
               << calibPulses[antennaIDs(i)].distZ << " +/-" << calibPulses[antennaIDs(i)].distZerr << "\t "
               << calibPulses[antennaIDs(i)].dist  << " = "
               << sqrt(xS*xS+yS*yS) << " +/-" << calibPulses[antennaIDs(i)].disterr  << "\t " << endl;  */                                               
        }

    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::storeShowerCoordinates: " << x.getMesg() << endl;
    }
  }


  Vector<int> analyseLOPESevent2::getAntennaIDs (void)
  {
    try {
      // get AntennaIDs if there is an event
      if (beamformDR_p != NULL) {
        Vector<int> antennaIDs;
        beamformDR_p->headerRecord().get("AntennaIDs",antennaIDs);
        return antennaIDs;
      }
    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::getAntennaIDs: " << x.getMesg() << endl;
    }
    return Vector<int>();
  }


  Matrix<Double> analyseLOPESevent2::getAntennaPositions(void)
  {
    try {
      // get antenna positions: not shifted for reference (core?) position
      if ((beamPipe_p != NULL) && (beamformDR_p != NULL)) {
        return beamPipe_p->GetAntPositions(beamformDR_p, false).copy();
      }
    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::getAntennaPositions: " << x.getMesg() << endl;
    }
    return Matrix<Double>();
  }


  void analyseLOPESevent2::summaryPlot(string filename,
                                       unsigned int columns)
  {
    try {
      // check if anything should be done (means columns must be > 0)
      if (columns <= 0) return;

      // create latexfilename
      string latexfilename = filename + ".tex";

      // open latexfile
      ofstream latexfile;
      latexfile.open(latexfilename.c_str(), ofstream::out);

      // check if file could be opened
      if (!(latexfile.is_open())) {
        std::cerr << "Failed to write to file \"" << latexfilename <<"\"." << std::endl;
      } else {
        // Create header for latex file
        latexfile << "\\documentclass[a4paper]{article}\n";
        latexfile << "\\usepackage{graphicx,a4wide}\n";
        latexfile << "\\setlength{\\textheight}{24cm}\n";
        latexfile << "\\setlength{\\topmargin}{0.0cm}\n";
        latexfile << "\\parindent=0pt\n";
        latexfile << "\\begin{document}\n";

        // get list of generated plots and loop through the list
        vector<string> plotlist = CompleteBeamPipe_p->getPlotList();

        for (unsigned int i = 0; i < plotlist.size(); i++) {
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
        if (system(shellCommand.c_str()) != 0)
          cout << "\nWARNING: Possible error while executing shell command:\n" << shellCommand << endl;

        // execute dvips to create postscript file
        shellCommand = "dvips " + filename + " 2> /dev/null"; // don't show the output to stderr
        if (system(shellCommand.c_str()) != 0)
          cout << "\nWARNING: Possible error while executing shell command:\n" << shellCommand << endl;

        // delete temporary files
        shellCommand = "rm -f " + filename + ".aux "
                                + filename + ".dvi "
                                + filename + ".tex "
                                + filename + ".log ";
        if (system(shellCommand.c_str()) != 0)
          cout << "\nWARNING: Possible error while executing shell command:\n" << shellCommand << endl;


        cout << "Created postscript summary: " << filename << ".ps" << endl;
      }
    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::summaryPlot: " << x.getMesg() << endl;
    }
  }

 void analyseLOPESevent2::createLateralOutput (const string& filePrefix, 
                                               const Record& erg,
                                               const double& Xc,
                                               const double& Yc)
 {
    try {
      unsigned int GT = erg.asuInt("Date");

      // get AntennaIDs to loop through pulse parameters
      Vector<int> antennaIDs;
      beamformDR_p->headerRecord().get("AntennaIDs",antennaIDs);

      // create filename
      stringstream filenameStream;
      filenameStream << filePrefix << GT << ".dat";
      string filename = filenameStream.str();

      // loop through antenna and verify that there are at least 2 pulse informations
      unsigned int NpulseInformation = 0;
      for (unsigned int i=0 ; i < antennaIDs.size(); ++i)
        if (calibPulses.find(antennaIDs(i)) != calibPulses.end())
          ++NpulseInformation;
      if (NpulseInformation < minBeamformingAntennas) {
        cerr << "analyseLOPESevent2::createLateralOutput: " 
             << "Event does not contain at least "
             << minBeamformingAntennas
             << " good pulses!"
             << endl;
        return;
      }

      // open file
      ofstream lateralfile;
      lateralfile.open(filename.c_str(), ofstream::out);

      cout << "\nWriting output for lateral distribution into file: " << filename << endl;

      // get antenna positions and distances in shower coordinates
      Vector <double> distances = erg.asArrayDouble("distances");
      Matrix <double> antPos = toShower(beamPipe_p->GetAntPositions(), erg.asDouble("Azimuth"), erg.asDouble("Elevation"));

      // loop through antenna and create output for every antenna with existing pulse information
      for (unsigned int i=0 ; i < antennaIDs.size(); ++i)
        if (calibPulses.find(antennaIDs(i)) != calibPulses.end()) {
          lateralfile << i + 1 << "  "
                      << antPos.row(i)(0) << "  "
                      << antPos.row(i)(1) << "  "
                      << distances(i) << "  "
                      << i + 1 << "  "
                      << calibPulses[antennaIDs(i)].envelopeMaximum << "  "
                      << calibPulses[antennaIDs(i)].noise << "  "
                      << calibPulses[antennaIDs(i)].envelopeTime << endl;
        } else {
          // if the antenna was not flagged manually and has the correct polarization
          // then wirte -1 in the output file
          // break if antennaID was flagged
          bool manuallyFlagged = false;
          for (unsigned int j=0 ; j < InputFlaggedAntIDs.size(); ++j)
            if (antennaIDs(i) == InputFlaggedAntIDs(j))
              manuallyFlagged = true;

          if ( !(manuallyFlagged) && PolarizationAntennaSelection(i) ) {
            lateralfile << i + 1 << "  "
                        << "-1" << "  "
                        << "-1" << "  "
                        << "-1" << "  "
                        << i + 1 << "  "
                        << "-1" << "  "
                        << "-1" << "  "
                        << "-1" << endl;
          }
        }

     // final line
     lateralfile << "# " << GT
                 << " "  << erg.asDouble("Azimuth")
                 << " "  << erg.asDouble("Elevation")
                 << " "  << erg.asDouble("Distance")
                 << " "  << Xc
                 << " "  << Yc << endl;

     // close file
     lateralfile.close();
    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::createLateralOutput: " << x.getMesg() << endl;
    }
  }

 void analyseLOPESevent2::fitLateralDistribution (const string& filePrefix,
                                                  Record& erg,
                                                  const double& Xc,
                                                  const double& Yc,
                                                  bool fitPowerLaw)
 {
    try {
      cout << "\nFitting lateral distribution..." << endl;
      
      // for investigation of suspicious antenna (mark it in green)
      int greenAnt = 0;  // set it to zero if now antenna should be green
      int greenValue = -1; // is set to array index which contains point information

      // get shower properties
      unsigned int GT = erg.asuInt("Date");
      double az = erg.asDouble("Azimuth");
      double el = erg.asDouble("Elevation");
      
      // for debugging: mark antenna 4 for the dual polarization setup in green
      // TODO: Remove this lines if the case of antenna 4 is clear
      if (GT > 1165574694)
        greenAnt = 4;

      // get AntennaIDs to loop through pulse parameters
      Vector<int> antennaIDs;
      beamformDR_p->headerRecord().get("AntennaIDs",antennaIDs);

      // get antenna positions and distances in shower coordinates
      Vector <double> distances = erg.asArrayDouble("distances");

      // create arrays for plotting and fitting
      unsigned int Nant = calibPulses.size();
      Double_t fieldStr[Nant],distance[Nant],distanceClean[Nant],fieldStrClean[Nant];
      Double_t fieldStrEr[Nant],distanceEr[Nant],distanceCleanEr[Nant],fieldStrCleanEr[Nant];
      Double_t noiseBgr[Nant], timePos[Nant];
      int antennaNumber[Nant];
      int antID[Nant], antIDclean[Nant];

      // loop through antennas and fill the arrays
      unsigned int ant = 0;        // counting antennas with pulse information
      for (unsigned int i=0 ; i < antennaIDs.size(); ++i)
        if (calibPulses.find(antennaIDs(i)) != calibPulses.end()) {
           distance[ant] = calibPulses[antennaIDs(i)].dist;
           distanceEr[ant] = calibPulses[antennaIDs(i)].disterr;
           fieldStr[ant] = calibPulses[antennaIDs(i)].envelopeMaximum;
           noiseBgr[ant] = calibPulses[antennaIDs(i)].noise;
           timePos[ant] = calibPulses[antennaIDs(i)].envelopeTime;
           antennaNumber[ant] = i+1;
           antID[ant] = antennaIDs(i);
           // look if this antenna shall be marked in green in the plot
           if (antennaNumber[ant] == greenAnt) {
             greenValue = ant;
           }  
           ++ant;
        }
      // consistancy check
      if (ant != Nant)
        cerr << "analyseLOPESevent2::fitLateralDistribution: Nant != number of antenna values\n" << endl; 

      Double_t fieldMax=0;
      Double_t fieldMin=0;
      Double_t maxdist=0;

      // for cuts (counters)
      unsigned int CutCloseToCore = 0;
      unsigned int CutSmallSignal = 0;
      unsigned int CutBadTiming = 0;
      unsigned int CutSNR = 0;
      double ccCenter = erg.asDouble("CCcenter");
      double xCenter = erg.asDouble("Xcenter");

      // calculate errors and count number of clean (good) values
      unsigned int clean = 0;
      cout << "\nApplying quality cuts..." << endl;
      for (unsigned int i = 0; i < Nant; ++i) {
        /* error of field strength = 19% + noise */
        fieldStrEr[i]=fieldStr[i]*0.19+noiseBgr[i];
        //distanceEr[i]=15;                // should probably be calculated instead

        /* get largest distance and min and max field strength */
        if ( distance[i] > maxdist)
          maxdist=distance[i];
        if (fieldStr[i] > fieldMax)
          fieldMax=fieldStr[i];
        if (fieldStr[i] < fieldMin)
          fieldMin=fieldStr[i];

        // apply Steffen's cuts
        /*
        if (distance[i]<15) {
          CutCloseToCore++;
          cout << "analyseLOPESevent2::fitLateralDistribution: Antenna cut because close to core!" << endl;
          calibPulses[antID[i]].lateralCut = true;
          continue;
        }
      */
        /* Removed this cut as SNR cut should be better
        if (fieldStr[i]<1.5) {
          CutSmallSignal++;
          cout << "analyseLOPESevent2::fitLateralDistribution: Antenna cut because signal to small!" << endl;
          calibPulses[antID[i]].lateralCut = true;
          continue;
        }
      */

        /* New Cuts */
        // pulse time correct? (default: pulse at cc-beam center +/- 25ns)
        if (abs(timePos[i]*1e-9 - ccCenter) > lateralTimeCut) {
          CutBadTiming++;
          cout << "analyseLOPESevent2::fitLateralDistribution: Antenna " << antennaNumber[i] << " cut because of bad timing: "
               << "CCcenter = " << ccCenter*1e9 << " , pulse time = " << timePos[i]
               << endl;
          calibPulses[antID[i]].lateralCut = true;
          continue;
        }
        // SNR >= 1 ?
        if ( abs(fieldStr[i]/noiseBgr[i]) < lateralSNRcut) {
          CutSNR++;
          cout << "analyseLOPESevent2::fitLateralDistribution: Antenna " << antennaNumber[i] 
               << " cut because SNR lower than " << lateralSNRcut
               << endl;
          calibPulses[antID[i]].lateralCut = true;
          continue;
        }

        // store value as good value if it passed all the cuts
        distanceClean[clean]  = distance[i];
        distanceCleanEr[clean]= distanceEr[i];
        fieldStrClean[clean]  = fieldStr[i];
        fieldStrCleanEr[clean]= fieldStrEr[i];
        antIDclean[clean]     = antID[i];
        clean++;
      }

      // store number of cut antennas
      erg.define("CutCloseToCore",CutCloseToCore);
      erg.define("CutSmallSignal",CutSmallSignal);
      erg.define("CutBadTiming",CutBadTiming);
      erg.define("CutSNR",CutSNR);

      // check if CC-beam was reconstructed (position inside of fit range)
      // oterwise don't consider any antenna as good
      if (!erg.asBool("CCconverged") ) {
        clean = 0;
        cout << "analyseLOPESevent2::fitLateralDistribution: Error: CC-beam did not converge." << endl; 
      }
      if ( (ccCenter < fitStart()) || (ccCenter > fitStop()) ) {
        clean = 0;
        cout << "analyseLOPESevent2::fitLateralDistribution: Error: CC-beam-center out of range (maybe fit did not converge)." << endl; 
      }
      // for consistency check if X-beam and CC-beam converged to the same peak
      if ( abs(xCenter -ccCenter) > getCCWindowWidth()/2.  ) {
        clean = 0;
        cout << "analyseLOPESevent2::fitLateralDistribution: Error: X-beam and CC-beam converged to different peaks!" << endl; 
      }

      cout << "\nAntennas in the Plot: " << ant << endl;
      cout << "Entries for fit: " << clean << endl;

      // calculate mean distance of the remaining antennas
      double latMeanDist = 0;
      if (clean > 0)
        latMeanDist = Mean(clean, distanceClean);
      cout << "Mean distance of antennas surviving the cuts: " << latMeanDist << " m." << endl;
      erg.define("latMeanDist",latMeanDist);

      TGraphErrors *latPro = new TGraphErrors (ant, distance,fieldStr,distanceEr,fieldStrEr);
      TGraphErrors *latProClean = new TGraphErrors (clean, distanceClean,fieldStrClean,distanceCleanEr,fieldStrCleanEr);
      latPro->SetFillColor(1);
      latPro->SetLineColor(2);
      latPro->SetMarkerColor(2);
      latPro->SetMarkerStyle(20);
      latPro->SetMarkerSize(1.1);
      stringstream label;
      label << "Lateral distribution - " << GT;
      latPro->SetTitle(label.str().c_str());
      latPro->GetXaxis()->SetTitle("distance R [m]"); 
      latPro->GetYaxis()->SetTitle("field strength #epsilon [#muV/m/MHz]");
      latPro->GetXaxis()->SetTitleSize(0.05);
      latPro->GetYaxis()->SetTitleSize(0.05);
      latPro->GetYaxis()->SetRange(0,100);

      latProClean->SetLineColor(4);
      latProClean->SetMarkerColor(4);
      latProClean->SetMarkerStyle(20);
      latProClean->SetMarkerSize(1.1);


      /* Canvas and Plotting */
      TCanvas *c1 = new TCanvas("c1","lateral distribution");
      c1->Range(-18.4356,-0.31111,195.528,2.19048);
      c1->SetFillColor(0);
      c1->SetBorderMode(0);
      c1->SetBorderSize(2);
      c1->SetLeftMargin(0.127768);
      c1->SetRightMargin(0.0715503);
      c1->SetTopMargin(0.0761421);
      c1->SetBottomMargin(0.124365);
      c1->SetFrameLineWidth(2);
      c1->SetFrameBorderMode(0);

      /* lateral plot */
      c1->SetLogy();
      latPro->SetMinimum(1);
      latPro->SetMaximum((fieldMax*3));
      latPro->SetTitle("");
      //    latPro->SetTitle("Lateral 1160026193");
      latPro->SetFillColor(0);
      latPro->GetYaxis()->SetRangeUser(1,fieldMax);
      latPro->GetXaxis()->SetRangeUser(0,(maxdist*1.10));
      latPro->Draw("AP");
      latProClean->Draw("same p");

      // mark point of one antenna in green (for debugging)
      if (greenValue > 0) {
        TGraph *latProGreen = new TGraph(1, &distance[greenValue], &fieldStr[greenValue]);
        latProGreen->SetMarkerColor(3);
        latProGreen->SetMarkerStyle(20);
        latProGreen->SetMarkerSize(1.1);
        latProGreen->Draw("same p");
      }  


      /* statistic box of fit */
      TPaveStats *ptstats = new TPaveStats(0.62,0.84,0.98,0.99,"brNDC");
      ptstats->SetName("stats");
      ptstats->SetBorderSize(2);
      ptstats->SetTextAlign(12);
      ptstats->SetFillColor(0);
      ptstats->SetOptStat(0);
      ptstats->SetOptFit(111);
      ptstats->Draw();
      latProClean->GetListOfFunctions()->Add(ptstats);
      ptstats->SetParent(latProClean->GetListOfFunctions());

      /* statistic box of EAS parameter */
      TPaveStats *easstats = new TPaveStats(0.45,0.84,0.62,0.99,"brNDC");
      easstats->SetBorderSize(2);
      easstats->SetTextAlign(12);
      easstats->SetFillColor(0);
      easstats->SetOptStat(0);
      easstats->SetOptFit(0);
      easstats->SetName("stats");

      // create labels for the plot
      label.str("");
      label << "GT " << GT;
      TText *text = easstats->AddText(label.str().c_str());
      text->SetTextSize(0.04);
      // label.str("");
      // label << "E_{g}  = " << energy;
      // text = easstats->AddText(label.str().c_str());
      label.str("");
      label << "#phi   = ";
      label.setf(ios::showpoint);
      label.precision(4);
      label.width(5);
      label << az << "^{o}";
      text = easstats->AddText(label.str().c_str());
      label.str("");
      label << "#theta = ";
      label.setf(ios::showpoint);
      label.precision(4);
      label.width(5);
      label << 90-el << "^{o}";
      text = easstats->AddText(label.str().c_str());
      easstats->Draw();

      /* FIT */
      // do fit only if there are at least 3 good antennas left (otherwise set parameters to 0)!
      if (clean >= 3) {
        // store number of antennas used for fit
        erg.define("NlateralAntennas",clean);

        // fit exponential
        TF1 *fitfuncExp;
        fitfuncExp=new TF1("fitfuncExp","[0]*exp(-x/[1])",50,190);
        fitfuncExp->SetParName(0,"#epsilon_{0}");
        fitfuncExp->SetParName(1,"R_{0}");
        fitfuncExp->SetParameter(1,200);
        fitfuncExp->GetXaxis()->SetRange(20,300);
        fitfuncExp->SetFillStyle(0);
        fitfuncExp->SetLineWidth(2);

        cout << "------------------------------"<<endl;
        latProClean->Fit(fitfuncExp, "");
        ptstats->Draw();

        // write fit results to record with other results
        erg.define("eps",fitfuncExp->GetParameter(0));
        erg.define("R_0",fitfuncExp->GetParameter(1));
        erg.define("sigeps",fitfuncExp->GetParError(0));
        erg.define("sigR_0",fitfuncExp->GetParError(1));
        erg.define("chi2NDF",fitfuncExp->GetChisquare()/double(fitfuncExp->GetNDF()));
        cout << "Result of exponential fit eps * e^(-x/R_0):\n"
             << "eps    = " << fitfuncExp->GetParameter(0) << "\t +/- " << fitfuncExp->GetParError(0) << "\t µV/m/MHz\n"
             << "R_0    = " << fitfuncExp->GetParameter(1) << "\t +/- " << fitfuncExp->GetParError(1) << "\t m\n"
             << "Chi^2  = " << fitfuncExp->GetChisquare() << "\t NDF " << fitfuncExp->GetNDF() << "\n"
             << endl;

        // write plot to file
        stringstream plotNameStream;
        plotNameStream << filePrefix << GT << ".eps";
        cout << "\nCreating plot: " << plotNameStream.str() << endl;
        c1->Print(plotNameStream.str().c_str());

        // calculate residuals 
        double eps = fitfuncExp->GetParameter(0);
        double r_0 = fitfuncExp->GetParameter(1);
        for (unsigned int i=0 ; i < clean; ++i) {
          double calculatedExp = eps * exp(-distanceClean[i]/r_0);
          double deviationExp  = 1. - fieldStrClean[i]/calculatedExp;
 
          calibPulses[antIDclean[i]].lateralExpHeight = calculatedExp;
          calibPulses[antIDclean[i]].lateralExpDeviation = deviationExp;
            /* debug output 
            cout << "Residuals for Ant " << antIDclean[i] << ": "
                 << "Exp: " << deviationExp*100 << "  \% \t"
                 << endl;
            */
        }

        // fit power law
        if (fitPowerLaw) {
          TF1 *fitfuncPow;
          fitfuncPow=new TF1("fitfuncPow","[0]*x^[1]");
          fitfuncPow->SetParName(0,"#epsilon_{0}");
          fitfuncPow->SetParName(1,"k");
          fitfuncPow->GetXaxis()->SetRange(20,300);
          fitfuncPow->SetFillStyle(0);
          fitfuncPow->SetLineWidth(2);
          //fitfuncPow->SetLineColor(3);

          cout << "------------------------------"<<endl;
          latProClean->Fit(fitfuncPow, "");
          ptstats->Draw();

          // write fit results to record with other results
          erg.define("epsPow",fitfuncPow->GetParameter(0));
          erg.define("kPow",fitfuncPow->GetParameter(1));
          erg.define("sigepsPow",fitfuncPow->GetParError(0));
          erg.define("sigkPow",fitfuncPow->GetParError(1));
          erg.define("chi2NDFPow",fitfuncPow->GetChisquare()/double(fitfuncPow->GetNDF()));
          cout << "Result of power law fit epsPow * x ^ kPow:\n"
               << "epsPow = " << fitfuncPow->GetParameter(0) << "\t +/- " << fitfuncPow->GetParError(0) << "\t µV/m/MHz\n"
               << "kPow   = " << fitfuncPow->GetParameter(1) << "\t +/- " << fitfuncPow->GetParError(1) << "\t m\n"
               << "Chi^2  = " << fitfuncPow->GetChisquare() << "\t NDF " << fitfuncPow->GetNDF() << "\n"
               << endl;

          // write plot to file
          plotNameStream.str("");
          plotNameStream << filePrefix << GT << "_pow.eps";
          cout << "\nCreating plot: " << plotNameStream.str() << endl;
          c1->Print(plotNameStream.str().c_str());

          // calculate residuals 
          double epsPow = fitfuncPow->GetParameter(0);
          double kPow = fitfuncPow->GetParameter(1);
          for (unsigned int i=0 ; i < clean; ++i) {
            double calculatedPow = epsPow * pow(distanceClean[i], kPow);
            double deviationPow  = 1. - fieldStrClean[i]/calculatedPow;

            calibPulses[antIDclean[i]].lateralPowHeight = calculatedPow;
            calibPulses[antIDclean[i]].lateralPowDeviation = deviationPow;
            /* debug output 
            cout << "Residuals for Ant " << antIDclean[i] << ": "
                 << "Exp: " << deviationExp*100 << "  \% \t"
                 << "Pow: " << deviationPow*100 << "  \%"
                 << endl;
            */
          }  
        }
      } else {
        erg.define("eps",0.);
        erg.define("R_0",0.);
        erg.define("sigeps",0.);
        erg.define("sigR_0",0.);
        erg.define("chi2NDF",0.);
        erg.define("epsPow",0.);
        erg.define("kPow",0.);
        erg.define("sigepsPow",0.);
        erg.define("sigkPow",0.);
        erg.define("chi2NDFPow",0.);
        erg.define("NlateralAntennas",0);
        cout << "No fit was done, because less than 3 antennas are 'good':\n"
             << "eps = " << 0 << "\t +/- " << 0 << "\t µV/m/MHz\n"
             << "R_0 = " << 0 << "\t +/- " << 0 << "\t m\n"
             << endl;
      }

    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::fitLateralDistribution: " << x.getMesg() << endl;
    }
  }


  // Function is based on fitLateralDistribution from AnalyseLopesEvent2
  // [added: mfranc]
  void analyseLOPESevent2::lateralTimeDistribution( const string& filePrefix,
                                                    Record& erg,
                                                    const double& Xc,
                                                    const double& Yc)
  {
    try {
      cout << "\nPlotting time vs distance" << endl;

      // Get shower properties
      unsigned int GT       = erg.asuInt  ("Date");
      double       az       = erg.asDouble("Azimuth");
      double       el       = erg.asDouble("Elevation");
      double       ccCenter = erg.asDouble("CCcenter");
      double       xCenter  = erg.asDouble("Xcenter");

      // get AntennaIDs to loop through pulse parameters
      Vector<int> antennaIDs;
      beamformDR_p->headerRecord().get("AntennaIDs",antennaIDs);

      // get antenna positions and distances in shower coordinates
      Vector <double> distances = erg.asArrayDouble("distances");

      // get antenna positions: shifted for core position and in shower coordinates
      Matrix <double> antPos = toShower(beamPipe_p->GetAntPositions(), erg.asDouble("Azimuth"), erg.asDouble("Elevation"));

      // create arrays for plotting and fitting
      unsigned int Nant = calibPulses.size();
      Double_t timeVal  [Nant], distance  [Nant], distanceClean  [Nant], timeValClean  [Nant];
      Double_t timeValEr[Nant], distanceEr[Nant], distanceCleanEr[Nant], timeValCleanEr[Nant];
      Double_t noiseBgr [Nant], fieldStr  [Nant];
      int antennaNumber [Nant];
      int antID         [Nant], antIDclean[Nant];

      // loop through antennas and fill the arrays
      unsigned int ant = 0;  // counting antennas with pulse information
      for (unsigned int i=0 ; i < antennaIDs.size(); ++i)
        if (calibPulses.find(antennaIDs(i)) != calibPulses.end()) {
           // Pulse time relative to shower plane and relative to CC time
           // = geom. delay from beam forming (as it is substracted during analysis) + 
           // time of pulse in trace (- CC beam time, to get an average of 0).
           timeVal      [ant] = calibPulses[antennaIDs(i)].distZ / lightspeed * 1e9,    // defined in Math/Constants.h
           timeVal      [ant] += calibPulses[antennaIDs(i)].geomDelay;
           timeVal      [ant] += calibPulses[antennaIDs(i)].envelopeTime - ccCenter*1e9;
           // pulse time error: Error from shower plane (geometry) + time calibration (2 ns)
           timeValEr    [ant] = sqrt(  pow(calibPulses[antennaIDs(i)].distZerr / lightspeed * 1e9,2)
                                     + pow(2.,2));
           distance     [ant] = calibPulses[antennaIDs(i)].dist;
           distanceEr   [ant] = calibPulses[antennaIDs(i)].disterr;
           fieldStr     [ant] = calibPulses[antennaIDs(i)].envelopeMaximum;
           noiseBgr     [ant] = calibPulses[antennaIDs(i)].noise;
           antennaNumber[ant] = i+1;
           antID        [ant] = antennaIDs(i);
           ++ant;
        }

      // consistancy check
      if (ant != Nant)
        cerr << "analyseLOPESevent2::fitTimeDistribution: Nant != number of antenna values\n" << endl; 

      Double_t timeMax=0;
      Double_t timeMin=0;
      Double_t maxdist=0;
      Double_t mindist=0;

      timeMin = *min_element(timeVal, timeVal + Nant);
      timeMax = *max_element(timeVal, timeVal + Nant);
      mindist = *min_element(distance, distance + Nant);
      maxdist = *max_element(distance, distance + Nant);

      // for cuts (counters)
      // unsigned int CutBadTiming   = 0;
      unsigned int CutSNR         = 0;

      // calculate errors and count number of clean (good) values
      unsigned int clean = 0;
      cout << "\nApplying quality cuts..." << endl;
      for (unsigned int i = 0; i < Nant; ++i) {
        /* Cuts taken from fitLateralDitribution */
        // pulse time correct? (default: pulse at cc-beam center +/- 25ns)
        /*if (abs(timeVal[i]*1e-9 - ccCenter) > lateralTimeCut) {
          CutBadTiming++;
          cout << "analyseLOPESevent2::fitTimeDistribution: Antenna " << antennaNumber[i] << " cut because of bad timing: "
               << "CCcenter = " << ccCenter*1e9 << " , pulse time = " << timeVal[i]
               << endl;
          continue;
        }*/
        // SNR >= 1 ?
        if ( abs(fieldStr[i]/noiseBgr[i]) < lateralSNRcut) {
          CutSNR++;
          cout << "analyseLOPESevent2::fitTimeDistribution: Antenna " << antennaNumber[i] 
               << " cut because SNR lower than " << lateralSNRcut
               << endl;
          continue;
        }

        // store value as good value if it passed all the cuts
        distanceClean[clean]   = distance[i];
        distanceCleanEr[clean] = distanceEr[i];
        timeValClean[clean]    = timeVal[i];
        timeValCleanEr[clean]  = timeValEr[i];
        antIDclean[clean]      = antID[i];
        clean++;
      }

      // check if CC-beam was reconstructed (position inside of fit range)
      // oterwise don't consider any antenna as good
      if (!erg.asBool("CCconverged") ) {
        clean = 0;
        cout << "analyseLOPESevent2::fitLateralDistribution: Error: CC-beam did not converge." << endl; 
      }
      if ( (ccCenter < fitStart()) || (ccCenter > fitStop()) ) {
        clean = 0;
        cout << "analyseLOPESevent2::fitLateralDistribution: Error: CC-beam-center out of range (maybe fit did not converge)." << endl; 
      }
      // for consistency check if X-beam and CC-beam converged to the same peak
      if ( abs(xCenter -ccCenter) > getCCWindowWidth()/2.  ) {
        clean = 0;
        cout << "analyseLOPESevent2::fitLateralDistribution: Error: X-beam and CC-beam converged to different peaks!" << endl; 
      }

      cout << "\nAntennas in the Plot: " << ant << endl;
      cout << "Entries for fit: " << clean << endl;

      // calculate mean distance of the remaining antennas
      double latMeanDist = 0;
      if (clean > 0)
        latMeanDist = Mean(clean, distanceClean);
      cout << "Mean distance of antennas surviving the cuts: " << latMeanDist << " m." << endl;
      // erg.define("latMeanDist",latMeanDist);
      // TODO: Define it differently or use the values which were previously calculated    

      TGraphErrors *latPro      = new TGraphErrors (ant, distance, timeVal, distanceEr, timeValEr);
      double offsetY = 100;
      double offsetX = 50;
      TGraphErrors *latProClean = new TGraphErrors (clean, distanceClean,timeValClean,distanceCleanEr,timeValCleanEr);
      latPro->SetFillColor(1);
      latPro->SetLineColor(2);
      latPro->SetMarkerColor(2);
      latPro->SetMarkerStyle(20);
      latPro->SetMarkerSize(1.1);
      stringstream label;
      label << "Time vs distance distribution - " << GT;
      latPro->SetTitle(label.str().c_str());
      latPro->GetXaxis()->SetTitle("distance R [m]"); 
      latPro->GetYaxis()->SetTitle("time T [ns]");
      latPro->GetXaxis()->SetTitleSize(0.05);
      latPro->GetYaxis()->SetTitleSize(0.05);
      latPro->GetYaxis()->SetRangeUser(timeMin - offsetY, timeMax + offsetY);
      latPro->GetXaxis()->SetRangeUser(mindist - offsetX, maxdist + offsetX);

      latProClean->SetLineColor(4);
      latProClean->SetMarkerColor(4);
      latProClean->SetMarkerStyle(20);
      latProClean->SetMarkerSize(1.1);
      // TODO: change the names of latPro and latProClean

      /* Canvas and Plotting */
      TCanvas *c1 = new TCanvas("c1","time vs distance distribution");
      c1->Range(-18.4356,-0.31111,195.528,2.19048);
      c1->SetFillColor(0);
      c1->SetBorderMode(0);
      c1->SetBorderSize(2);
      c1->SetLeftMargin(0.127768);
      c1->SetRightMargin(0.0715503);
      c1->SetTopMargin(0.0761421);
      c1->SetBottomMargin(0.124365);
      c1->SetFrameLineWidth(2);
      c1->SetFrameBorderMode(0);

      /* time vs distance plot */
      latPro->SetFillColor(0);
      latPro->Draw("AP");
      latProClean->Draw("same p");

      /* statistic box of fit */
      TPaveStats *ptstats = new TPaveStats(0.62,0.84,0.98,0.99,"brNDC");
      ptstats->SetName("stats");
      ptstats->SetBorderSize(2);
      ptstats->SetTextAlign(12);
      ptstats->SetFillColor(0);
      ptstats->SetOptStat(0);
      ptstats->SetOptFit(111);
      ptstats->Draw();
      latProClean->GetListOfFunctions()->Add(ptstats);
      ptstats->SetParent(latProClean->GetListOfFunctions());

      /* statistic box of EAS parameter */
      TPaveStats *easstats = new TPaveStats(0.45,0.84,0.62,0.99,"brNDC");
      easstats->SetBorderSize(2);
      easstats->SetTextAlign(12);
      easstats->SetFillColor(0);
      easstats->SetOptStat(0);
      easstats->SetOptFit(0);
      easstats->SetName("stats");

      /* create labels for the plot */
      label.str("");
      label << "GT " << GT;
      TText *text = easstats->AddText(label.str().c_str());
      text->SetTextSize(0.04);
      label.str("");
      label << "#phi   = ";
      label.setf(ios::showpoint);
      label.precision(4);
      label.width(5);
      label << az << "^{o}";
      text = easstats->AddText(label.str().c_str());
      label.str("");
      label << "#theta = ";
      label.setf(ios::showpoint);
      label.precision(4);
      label.width(5);
      label << 90-el << "^{o}";
      text = easstats->AddText(label.str().c_str());
      easstats->Draw();

      /* FIT */
      if (clean >= 3) {
        // fit polynomial
        TF1 *fitFunc;
        fitFunc=new TF1("fitFunc","[0]+[1]*(sqrt([2]**2+x**2)-[2])",50,190);
        fitFunc->SetParName(0,"offset");
        fitFunc->SetParName(1,"1/c");
        fitFunc->SetParName(2,"R_curv");
        fitFunc->SetParameter(0,0);
        fitFunc->FixParameter(1,1e9/lightspeed);
        fitFunc->SetParameter(2,1000);
        fitFunc->GetXaxis()->SetRange(20,300);
        fitFunc->SetFillStyle(0);
        fitFunc->SetLineWidth(2);

        cout << "------------------------------"<<endl;
        latProClean->Fit(fitFunc, "");
        ptstats->Draw();

        // write fit results to record with other results
        erg.define("FTD_offset",fitFunc->GetParameter(0));
        erg.define("FTD_R_curv",fitFunc->GetParameter(2));
        erg.define("FTD_sigoffset",fitFunc->GetParError(0));
        erg.define("FTD_sigR_curv",fitFunc->GetParError(2));
        erg.define("FTD_chi2NDF",fitFunc->GetChisquare()/double(fitFunc->GetNDF()));
        cout << "Results of fit"
             << "offset = " << fitFunc->GetParameter(0) << "\t +/- " << fitFunc->GetParError(0) << "\t ns\n"
             << "R_curv = " << fitFunc->GetParameter(2) << "\t +/- " << fitFunc->GetParError(2) << "\t m\n"
             << "Chi^2  = " << fitFunc->GetChisquare() << "\t NDF " << fitFunc->GetNDF() << "\n"
             << "Curvature radius of CC-beam (to compare) = " << erg.asDouble("Distance") << " m\n"
             << endl;

        // write plot to file
        stringstream plotNameStream;
        plotNameStream << filePrefix << GT <<".eps";
        cout << "\nCreating plot: " << plotNameStream.str() << endl;
        c1->Print(plotNameStream.str().c_str());
      } else {
        cout<<"No fit was done, because less than 3 antennas are 'good':\n";
      }
    } catch (AipsError x) {
      cerr << "analyseLOPESevent2::lateralTimeDistribution: " << x.getMesg() << endl;
    }
  }
//---------------------------------------------------------------------------------------------------


 bool analyseLOPESevent2::simpleSkymap (const string& filename,
                                        const int& noBlocks,
                                        const unsigned int& startBlock,
                                        const unsigned int& endBlock,
                                        const double& az_start,
                                        const double& az_stop,
                                        const double& az_d,
                                        const double& ze_start,
                                        const double& ze_stop,
                                        const double& ze_d,
                                        Vector<bool> AntennaSelection,
                                        const double& distance,
                                        const string& beamtype,
                                        const double& hanning)
 {
   bool status=1;
   try {
     cout<<"Generating simple skymap..."<<endl;
     CompleteBeamPipe_p = static_cast<CompletePipeline*>(beamPipe_p);
     CompleteBeamPipe_p->InitEvent(beamformDR_p);
     CompleteBeamPipe_p->setPhaseCenter(0,0);
     int newblocksize = lev_p->blocksize() / noBlocks;
     beamformDR_p->setBlocksize(newblocksize);
     beamformDR_p->setHanningFilter(hanning);
     cout<<"blocksize: "<<beamformDR_p->blocksize()<<endl;
     string fname = filename;
     fname.erase(fname.find_last_of("."), 6);
     fname.append("-skymap");
     beamformDR_p->setBlock(startBlock);

     while( beamformDR_p->block() <= endBlock) {
       cout<<"Processing block "<< beamformDR_p->block()<<" ["<< beamformDR_p->block()-startBlock+1 << "/"<<endBlock-startBlock+1<<"]"<<endl;
       // The following statement does not work any more
       // since TimeFreq.cc::timeValues has been changed to demand a reference instead to return a vector
       // cout<<"startTime: " << beamformDR_p->timeValues()[0]<<endl;
       stringstream str;
       str.clear();
       str<< beamformDR_p->block();
       string buf = str.str();
       if(buf.length()==1) buf="000"+buf;
       if(buf.length()==2) buf="00"+buf;
       if(buf.length()==3) buf="0"+buf;
       string ofname = fname+"-"+buf+".dat";
       string psfname = fname+"-"+buf+".eps";
       ofstream fout(ofname.c_str());

       Vector<Double> power;
       double max=0, min=0, pix;

       // loop over all pixels
       for(float it_ze=ze_start; it_ze<=ze_stop; it_ze+=ze_d) {
         float it_az=az_start;
         while( it_az<=az_stop-az_d) {
           cout<<'\xd';
           cout<<"["<<it_ze<<"]["<<it_az<<"]     ";
           CompleteBeamPipe_p->setDirection(it_az, 90 - it_ze, distance);
           if(beamtype.compare("CC")==0)
             power = CompleteBeamPipe_p->GetCCBeam(beamformDR_p, AntennaSelection, "ANY").copy();
           else
             if (beamtype.compare("P")==0)
               power = CompleteBeamPipe_p->GetPBeam(beamformDR_p, AntennaSelection, "ANY").copy();
             else
               if (beamtype.compare("X")==0)
                 power = CompleteBeamPipe_p->GetXBeam(beamformDR_p, AntennaSelection, "ANY").copy();
               else {
                 cerr<<"Unknown beamtype: "<<beamtype<<endl;
                 return 0 ;
              }
           // choose mean of beam value in block interval (not maximum)
           // maximum would be better if there is a beam
           // mean is usefull for noise, to avoid random coherence
           pix=mean(power);
           fout<<it_az<<"  "<<it_ze<<"   "<<pix<<endl;
           it_az += az_d;
           if(pix>max)
             max=pix;
           if(pix<min)
             min=pix;
         }
       }
       fout.close();
       cout<<'\xd'<<"frame finished...   \nWriting ps-file..."<<endl;
       ifstream fin(ofname.c_str());

       //generate the postscript file...
       ofstream psout(psfname.c_str());
       // writing ps-header...
       psout<<"%!PS-Adobe-2.0 EPSF-2.0\n"
           <<"%%DocumentFonts: Courier-Bold Helvetica-Bold\n"
           <<"%%BoundingBox: -320 -340 360 310\n"
           <<"%%Title: "<< psfname<<endl
           <<"%%EndComments\n"
           <<"gsave\n"
           <<"/PSDict 200 dict def\n"
           <<"PSDict begin\n"
           <<"/a {arc} def\n"
           <<"/s {stroke} def\n"
           <<"/l {lineto} def\n"
           <<"/m {moveto} def\n"
           <<"/n {newpath} def\n"
           <<"/rgb {setrgbcolor} def\n"
           <<"/c {closepath} def\n"
           <<"/f {fill} def\n"
           <<"/gs {gsave} def\n"
           <<"/gr {grestore} def\n"
           <<"/cf {closepath fill} def\n"
           <<"/lw {setlinewidth} def\n"
           <<"end\n"
           <<"PSDict begin\n"
           <<"3 3 scale\n"
           <<"0.1 lw \n\n";

      double r=1,g=1,b=1;
      float az, ze, ax, ay, x1, y1, x2, y2, x3, y3, x4, y4;

      while(fin>>az>>ze>>pix) {
        psout.precision(3);
        if(pix >=0) {
            if(pix <= 0.1 && pix >= 1.e-8) {
                r=1.-(8.+log10(pix))/7.;
                g=1.-(8.+log10(pix))/7.;
                b=1.-0.2*(8.+log10(pix))/7.;
            } else {
              if(pix> 0.1) {
                r=0;
                g=0;
                b=0.8;
              }
              if(pix< 1.e-8) {
                r=1;
                g=1;
                b=1;
              }
            }
        } else {
            if(pix >= -0.1 && pix <= -1.e-8) {
                r=1.-0.2*(8.+log10(-1.*pix))/7.;
                g=1.-(8.+log10(-1.*pix))/7.;
                b=1.-(8.+log10(-1.*pix))/7.;
            } else {
              if(pix< -0.1) {
                r=0.8;
                g=0;
                b=0.;
              }
              if(pix> -1.e-8) {
                r=1;
                g=1;
                b=1;
              }
            }
        }


           psout<<r<<" "<<g<<" "<<b<<" rgb ";
           ax=-1.*ze*sin(az/180.*PI);
           ay= ze*cos(az/180.*PI);
           x1= ax+0.5*ze_d*sin(az/180.* PI) - (ze-ze_d/2.)*sin(az_d/2./180.* PI)*cos(az/180.* PI);
           y1= ay-0.5*ze_d*cos(az/180.* PI) - (ze-ze_d/2.)*sin(az_d/2./180.* PI)*sin(az/180.* PI);
           x2= ax+0.5*ze_d*sin(az/180.* PI) + (ze-ze_d/2.)*sin(az_d/2./180.* PI)*cos(az/180.* PI);
           y2= ay-0.5*ze_d*cos(az/180.* PI) + (ze-ze_d/2.)*sin(az_d/2./180.* PI)*sin(az/180.* PI);
           x3= ax-0.5*ze_d*sin(az/180.* PI) + (ze+ze_d/2.)*sin(az_d/2./180.* PI)*cos(az/180.* PI);
           y3= ay+0.5*ze_d*cos(az/180.* PI) + (ze+ze_d/2.)*sin(az_d/2./180.* PI)*sin(az/180.* PI);
           x4= ax-0.5*ze_d*sin(az/180.* PI) - (ze+ze_d/2.)*sin(az_d/2./180.* PI)*cos(az/180.* PI);
           y4= ay+0.5*ze_d*cos(az/180.* PI) - (ze+ze_d/2.)*sin(az_d/2./180.* PI)*sin(az/180.* PI);
           psout<<"n "<< x1<<" "<<y1<<" m "<< x2<<" "<<y2<<" l "<<x3<< " "<<y3<< " l "<< x4<<" "<<y4<<" l c gs f gr s\n";
      }

      // writing footer...
     psout<<"0.000 0.000 0.800 rgb n 80 -55 m 85 -55 l 85 -50 l 80 -50 l 80 -55 l c gs f gr s\n"
          <<"0.250 0.250 0.850 rgb n 80 -60 m 85 -60 l 85 -55 l 80 -55 l 80 -60 l c gs f gr s\n"
          <<"0.500 0.500 0.900 rgb n 80 -65 m 85 -65 l 85 -60 l 80 -60 l 80 -65 l c gs f gr s\n"
          <<"0.750 0.750 0.950 rgb n 80 -70 m 85 -70 l 85 -65 l 80 -65 l 80 -70 l c gs f gr s\n"
          <<"1.000 1.000 1.000 rgb n 80 -75 m 85 -75 l 85 -70 l 80 -70 l 80 -75 l c gs f gr s\n"
          <<"0.950 0.750 0.750 rgb n 80 -80 m 85 -80 l 85 -75 l 80 -75 l 80 -80 l c gs f gr s\n"
          <<"0.900 0.500 0.500 rgb n 80 -85 m 85 -85 l 85 -80 l 80 -80 l 80 -85 l c gs f gr s\n"
          <<"0.850 0.250 0.250 rgb n 80 -90 m 85 -90 l 85 -85 l 80 -85 l 80 -90 l c gs f gr s\n"
          <<"0.800 0.000 0.000 rgb n 80 -95 m 85 -95 l 85 -90 l 80 -90 l 80 -95 l c gs f gr s\n";






      psout.precision(2);
      psout<<"0.0 0.0 0.0 rgb n 80 -50 m 85 -50 l 85 -95 l 80 -95 l 80 -50 l c s\n"
           <<"0.0 0.0 0.0 rgb /Helvetica 5 selectfont\n"
           <<"n 87 -47 m (CC-Beam: ) show\n"
           <<"n 87 -55 m (1.e-1) show c n 87 -60 m (1.e-3) show c n 87 -65 m (1.e-5) show c\n"
           <<"n 87 -70 m (1.e-7) show c n 87 -75 m (0.0) show c n 87 -80 m (-1.e-7) show c\n"
           <<"n 87 -85 m (-1.e-5) show c n 87 -90 m (-1.e-3) show c n 87 -95 m (-1.e-1) show c\n";


      psout<<"n -97 -110 m\n"
           <<"0.0 0.0 0.0 rgb\n"
           <<"/Helvetica 6 selectfont\n"
           <<"("<<filename<<" - frame "<<buf<<"/"<< noBlocks<<") show c\n"
           <<"n -2 92 m (N) show c\n"
           <<"n -98 -3 m (E) show c\n"
           <<"n -2 -98 m (S) show c\n"
           <<"n 93 -3 m (W) show c\n"
           <<"0.3 0.3 0.3 rgb 0.1 lw\n"
           <<"n 0 0 "<< 90+0.5*ze_d <<" 0 360 a s\n"
           <<"0 0 60 0 360 a s\n"
           <<"0 0 30 0 360 a s\n"
           <<-90-0.5*ze_d<<" 0 m  "<<90+0.5*ze_d<<" 0 l s\n"
           <<"0 "<<-90-0.5*ze_d<<" m 0 "<< 90+0.5*ze_d<<" l s\n"
           <<"grestore\n"
           <<"end\n";
       psout.close();
       cout<<"finished writing of ps file...\n";


       beamformDR_p->nextBlock();
       CompleteBeamPipe_p->resetCachedFlag();
       }

   } catch (AipsError x) {
     cerr << "analyseLOPESevent2::simpleSkymap: " << x.getMesg() << endl;
     status=0;
     }
   return status;
 }


} // Namespace CR -- end


#endif
