/*-------------------------------------------------------------------------*
 | $Id:: checkNoiseInfluence.cc 3949 2010-01-08 15:22:30Z schroeder       $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                    *
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

#include <Analysis/checkNoiseInfluence.h>

#ifdef HAVE_STARTOOLS

// include files for root histograms
#include<TH1D.h>

namespace CR { // Namespace CR -- begin

  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  checkNoiseInfluence::checkNoiseInfluence ()
    : analyseLOPESevent2(),
    pulsePattern( Vector<Double>() ),
    pulseHeight(0.),
    pulseTime(0.),
    startTime(-150e-6),
    noiseMethod(4),
    noiseIntervalLength(10e-6),
    noiseIntervalGap(5e-6),
    NnoiseIntervals(20),
    upsampledNoise( Matrix<Double>() ),
    noiseTimeAxis( Vector<Double>() ),
    noiseHeight(vector< vector<double> >())
  {}

  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================

  checkNoiseInfluence::~checkNoiseInfluence ()
  {}

  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  void checkNoiseInfluence::loadPulsePattern(const string& evname,
                                             const int& antenna, 
                                             const double& pulseStart)
  {
    try {   
      double pulseStop = pulseStart + noiseIntervalLength;
      cout << "\nLoading pulse pattern from file '" << evname
           << "', antenna " << antenna
           << "\n,at time range from " << pulseStart << " s to " << pulseStop << " s."
           << endl;                 

      // first reset potentially existing old pipeline
      pipeline.init();
      upsamplePipe.init();

      // Generate the Data Reader
      if (! lev_p->attachFile(evname) ) {
        cerr << "checkNoiseInfluence::loadPulsePattern: " << "Failed to attach file: " << evname << endl;
        return;
      }

      //  do all corrections which have an effect on the pulse shape
      pipeline_p->doGainCal(true);
      pipeline_p->doDispersionCal(true);
      pipeline_p->doDelayCal(false);
      pipeline_p->doFlagNotActiveAnts(false);

      // initialize the Data Reader
      if (! pipeline_p->InitEvent(lev_p)) {
        cerr << "checkNoiseInfluence::loadPulsePattern: " << "Failed to initialize the DataReader!" << endl;
        return;
      }

      //  Enable/disable calibration steps for the SecondStagePipeline
      pipeline_p->doPhaseCal(false);
      pipeline_p->doRFImitigation(true);

      // Generate the antenna selection
      Vector <Bool> AntennaSelection = pipeline_p->GetAntennaMask(lev_p).copy();
      AntennaSelection.set(false);
      if (antenna > static_cast<int>(AntennaSelection.nelements())) {
        cerr << "checkNoiseInfluence::loadPulsePattern: " << "Antenna number out of range." << endl;
        return;
      }
      AntennaSelection(antenna-1)=true;

      // initialize Complete Pipeline
      CompleteBeamPipe_p = static_cast<CompletePipeline*>(pipeline_p);
      setPlotInterval(pulseStart,pulseStop);
      CompleteBeamPipe_p->setPlotInterval(pulseStart,pulseStop);
      CompleteBeamPipe_p->setCalibrationMode(true);

      // Plot chosen pulse pattern
      CompleteBeamPipe_p->plotAllAntennas("pulsePattern", lev_p, AntennaSelection, true,
                                          getUpsamplingExponent(),false, true);
      // calculate the maxima (calibPulses is defined in analyseLOPESevent2)
      calibPulses = CompleteBeamPipe_p->calculateMaxima(lev_p, AntennaSelection, getUpsamplingExponent(), false,
                                                        1e99, noiseMethod, pulseStart - 10.5e-6, pulseStart - 0.5e-6);
      
      // load the pulse pattern: calculate time range of pulse and get up-sampled trace in this range
      Vector<Double> timeValues =
        CompleteBeamPipe_p->getUpsampledTimeAxis(lev_p, getUpsamplingExponent());
      Slice range = CompleteBeamPipe_p->calculatePlotRange(timeValues);
      pulsePattern =
        CompleteBeamPipe_p->getUpsampledFieldstrength(lev_p, getUpsamplingExponent(), AntennaSelection).column(antenna-1)(range).copy();
        
      // store properties of test pulse  
      testPulseProperties = calibPulses.begin()->second;
      pulseHeight = testPulseProperties.envelopeMaximum;
      // substract start time of pulse window to get relative pulse time in pulse pattern (in ns)
      testPulseProperties.maximumTime -= pulseStart*1e9;
      testPulseProperties.envelopeTime -= pulseStart*1e9;
      testPulseProperties.minimumTime -= pulseStart*1e9;
      testPulseProperties.halfheightTime -= pulseStart*1e9;
      pulseTime = testPulseProperties.envelopeTime*1e-9; // in s
      cout << "Height of test pulse = " << pulseHeight << " \t Time of testpulse = " << pulseTime << " s" << endl;   
    } catch (AipsError x) {
      cerr << "checkNoiseInfluence::loadPulsePattern: " << x.getMesg() << endl;
    }
  }
  
  void checkNoiseInfluence::loadNoiseEvent(const string& evname)
  {
    try {   
      cout << "\nLoading noise from file: " << evname << endl;      

      // first reset potentially existing old pipeline
      pipeline.init();
      upsamplePipe.init();

      // Generate the Data Reader
      if (! lev_p->attachFile(evname) ) {
        cerr << "checkNoiseInfluence::loadNoiseEvent: " << "Failed to attach file: " << evname << endl;
        return;
      }

      //  do all corrections which have an effect on the pulse shape
      pipeline_p->doGainCal(true);
      pipeline_p->doDispersionCal(true);
      pipeline_p->doDelayCal(false);
      pipeline_p->doFlagNotActiveAnts(false);

      // initialize the Data Reader
      if (! pipeline_p->InitEvent(lev_p)) {
        cerr << "checkNoiseInfluence::loadNoiseEvent: " << "Failed to initialize the DataReader!" << endl;
        return;
      }

      //  Enable/disable calibration steps for the SecondStagePipeline
      pipeline_p->doPhaseCal(false);
      pipeline_p->doRFImitigation(true);

      // Generate the antenna selection (all antennas)
      Vector <Bool> AntennaSelection = pipeline_p->GetAntennaMask(lev_p).copy();
      AntennaSelection.set(true);
      
      // initialize Complete Pipeline
      CompleteBeamPipe_p = static_cast<CompletePipeline*>(pipeline_p);

      // create a plot to see, if things work
      CompleteBeamPipe_p->setPlotInterval(plotStart(),plotStop());
      CompleteBeamPipe_p->setCalibrationMode(true);

      // Plot noise
      CompleteBeamPipe_p->plotAllAntennas("noise", lev_p, AntennaSelection, false,
                                          getUpsamplingExponent(),false, false);

      // store upsampled noise
      upsampledNoise=CompleteBeamPipe_p->getUpsampledFieldstrength(lev_p, getUpsamplingExponent(), AntennaSelection);
      noiseTimeAxis =CompleteBeamPipe_p->getUpsampledTimeAxis(lev_p, getUpsamplingExponent());
                                         
      // calculate the maxima
      for (int i = 0; i < NnoiseIntervals; ++i) {
        setPlotInterval(startTime + i*(noiseIntervalLength+noiseIntervalGap), startTime + i*(noiseIntervalLength+noiseIntervalGap) + noiseIntervalLength);
        CompleteBeamPipe_p->setPlotInterval(plotStart(),plotStop());
        double noiseTime = plotStart()+pulseTime;   // noise range is normally calculated in respect to the time of the CC beam (where the pulse is expected)
        calibPulses = CompleteBeamPipe_p->calculateMaxima(lev_p, AntennaSelection, getUpsamplingExponent(), false,
                                                          noiseTime, noiseMethod, plotStart(), plotStop());
        vector<double> noiseValues; // stores the noise heights for one noise interval
        // loop through all antennas and store height of noise                                          
        for( map<int, PulseProperties>::iterator it = calibPulses.begin(); it != calibPulses.end(); ++it ) {
          noiseValues.push_back(it->second.noise);
        } 
        noiseHeight.push_back(noiseValues);
      }  
    } catch (AipsError x) {
      cerr << "checkNoiseInfluence::loadNoiseEvent: " << x.getMesg() << endl;
    }  
  }
  
  void checkNoiseInfluence::addPulseToNoise(const double& pulseSNR, const string& resultsFileName)                          
  {
    try {   
      // Generate the antenna selection (all antennas)
      Vector <Bool> AntennaSelection = pipeline_p->GetAntennaMask(lev_p).copy();
      AntennaSelection.set(true);
      
      // check if noise was loaded consistently
      if (upsampledNoise.shape() !=  
          CompleteBeamPipe_p->getUpsampledFieldstrength(lev_p, getUpsamplingExponent(), AntennaSelection).shape()) {
        cerr << "checkNoiseInfluence::addPulseToNoise: " << "Error: Load noise, first!" << endl;
        return;
      }

      // add pulse to noise, for each noise interval
      Matrix<Double> noiseAndPulse = upsampledNoise.copy();
      for (int interval = 0; interval < NnoiseIntervals; ++interval) {
        double startsample = ntrue(noiseTimeAxis < startTime + interval*(noiseIntervalLength+noiseIntervalGap));  
        //cout << "Interval samples" << interval << ": " << startsample << " to " << startsample+pulsePattern.nelements() << endl;
        // loop through all antennas and add pulse
        for (unsigned int i=0; i < upsampledNoise.ncolumn(); ++i)
          for (unsigned int j=0; j < pulsePattern.nelements(); ++j)            
            noiseAndPulse(j+startsample,i) += pulseSNR*noiseHeight[interval][i]/pulseHeight*pulsePattern(j); 
      }         
      CompleteBeamPipe_p->setUpsampledFieldStrength(noiseAndPulse);

      CompleteBeamPipe_p->setCalibrationMode(true);

      // create arrays and variables to store the information on snr and time shift
      double SNR;
      //double lgSNR;
      double envelopeTimeDiff;
      double halfheightTimeDiff;
      double maximumTimeDiff;
      double minimumTimeDiff;
      
      // prepare root histograms
      double SNRminimum = 0;
      if (pulseSNR > 3)
        SNRminimum = round(pulseSNR-3);
      TH1D *SNRHist = new TH1D("SNRHist","SNR",60,SNRminimum,round(pulseSNR+3));
      //TH1D *lgSNRHist = new TH1D("lgSNRHist","lg SNR",120,-3,3);
      TH1D *envelopeTimeDiffHist = new TH1D("envelopeTimeDiffHist","Time deviation of envelope maximum",
                                            100,-round(100/(1.5*pulseSNR+1.)),+round(100/(1.5*pulseSNR+1.)));      
      TH1D *halfheightTimeDiffHist = new TH1D("halfheightTimeDiffHist","Time deviation of crossing of half height",
                                              100,-round(1000/(3*pulseSNR+1.)),+round(1000/(3*pulseSNR+1.)));      
      TH1D *maximumTimeDiffHist = new TH1D("maximumTimeDiffHist","Time deviation of maximum",
                                           100,-round(30/(pulseSNR+1.)),+round(30/(pulseSNR+1.)));      
      TH1D *minimumTimeDiffHist = new TH1D("minimumTimeDiffHist","Time deviation of minimum",
                                            100,-round(30/(pulseSNR+1.)),+round(30/(pulseSNR+1.)));      

      unsigned int ant = 0;
      unsigned int i=0;  // counter to fill arrays
      int interval = 0; // noise interval

      // prepare rootfile for results
      TFile *rootfile=NULL;
      stringstream filename;
      filename << "resultsSNR-" << pulseSNR << ".root";
      rootfile = new TFile(filename.str().c_str(),"RECREATE","Results for SNR test pulse study");

      // check if file is open
      if (rootfile->IsZombie()) {
        cerr << "\nError: Could not create root file! \n" << endl;
        return;               // exit
      }
    
      // create tree and tree structure
      TTree *roottree = NULL;
      roottree = new TTree(filename.str().c_str(),filename.str().c_str());
      roottree->Branch("ant",&ant,"ant/i");
      roottree->Branch("noiseInterval",&interval,"noiseInterval/i");
      roottree->Branch("SNR",&SNR,"SNR/D");
      //roottree->Branch("lgSNR",&lgSNR,"lgSNR/D");
      roottree->Branch("envelopeTimeDiff",&envelopeTimeDiff,"envelopeTimeDiff/D");
      roottree->Branch("halfheightTimeDiff",&halfheightTimeDiff,"halfheightTimeDiff/D");
      roottree->Branch("maximumTimeDiff",&maximumTimeDiff,"maximumTimeDiff/D");
      roottree->Branch("minimumTimeDiff",&minimumTimeDiff,"minimumTimeDiff/D");
      
      // calculate the maxima in same intervals as noise
      for (interval = 0; interval < NnoiseIntervals; ++interval) {
        setPlotInterval(startTime + interval*(noiseIntervalLength+noiseIntervalGap),
                        startTime + interval*(noiseIntervalLength+noiseIntervalGap) + noiseIntervalLength);
        CompleteBeamPipe_p->setPlotInterval(plotStart(),plotStop());
        
        // Plot noise
        filename.str("");
        filename << "noiseAndPulse-" << interval;
        CompleteBeamPipe_p->plotAllAntennas(filename.str(), lev_p, AntennaSelection, false,
                                            getUpsamplingExponent(),false, false);
        calibPulses = CompleteBeamPipe_p->calculateMaxima(lev_p, AntennaSelection, getUpsamplingExponent(), false,
                                                          plotStart()+pulseTime, noiseMethod, plotStart(), plotStop());
        vector<double> noiseValues = noiseHeight[interval]; // get the noise heights for one noise interval
        
        // loop through all antennas and get the signal-to-noise ratio                              
        ant = 0;
        for( map<int, PulseProperties>::iterator it = calibPulses.begin(); it != calibPulses.end(); ++it ) {        
          SNR = it->second.envelopeMaximum/noiseValues[ant];
          //lgSNR = log10(it->second.envelopeMaximum/noiseValues[ant]);
          // calculate deviation of pulse time
          envelopeTimeDiff = it->second.envelopeTime - (plotStart()*1e9+testPulseProperties.envelopeTime);
          halfheightTimeDiff = it->second.halfheightTime - (plotStart()*1e9+testPulseProperties.halfheightTime);
          maximumTimeDiff = it->second.maximumTime - (plotStart()*1e9+testPulseProperties.minimumTime);
          minimumTimeDiff = it->second.minimumTime - (plotStart()*1e9+testPulseProperties.minimumTime);
          roottree->Fill(); // store data in root tree
          rootfile->Write("",TObject::kOverwrite);

          // fill arrays for histograms
          SNRHist->Fill(SNR);
          //lgSNRHist->Fill(lgSNR);
          envelopeTimeDiffHist->Fill(envelopeTimeDiff);
          halfheightTimeDiffHist->Fill(halfheightTimeDiff);
          maximumTimeDiffHist->Fill(maximumTimeDiff);
          minimumTimeDiffHist->Fill(minimumTimeDiff);
          
          ++i;
          ++ant;
        } 
      }

      // fit prepared histograms
      // SNRHist->Fit("gaus");
      // lgSNRHist->Fit("gaus");
      // envelopeTimeDiffHist->Fit("gaus");
      // halfheightTimeDiffHist->Fit("gaus");
      // maximumTimeDiffHist->Fit("gaus");
      // minimumTimeDiffHist->Fit("gaus");

      //SNRHist -> SetStats(0);
      //SNRHist -> SetTitle("");
      SNRHist -> GetXaxis()->SetTitle("signal-to-noise ratio"); 
      SNRHist -> GetYaxis()->SetTitle("entries per bin");
      SNRHist -> GetXaxis()->SetTitleSize(0.05);
      SNRHist -> GetYaxis()->SetTitleSize(0.05);

      //lgSNRHist -> SetStats(0);
      //lgSNRHist -> SetTitle("");
      //lgSNRHist -> GetXaxis()->SetTitle("lg(signal-to-noise ratio)"); 
      //lgSNRHist -> GetYaxis()->SetTitle("entries per bin");
      //lgSNRHist -> GetXaxis()->SetTitleSize(0.05);
      //lgSNRHist -> GetYaxis()->SetTitleSize(0.05);

      //envelopeTimeDiffHist -> SetStats(0);
      //envelopeTimeDiffHist -> SetTitle("");
      envelopeTimeDiffHist -> GetXaxis()->SetTitle("deviation of envelope maximum time [ns]"); 
      envelopeTimeDiffHist -> GetYaxis()->SetTitle("entries per bin");
      envelopeTimeDiffHist -> GetXaxis()->SetTitleSize(0.05);
      envelopeTimeDiffHist -> GetYaxis()->SetTitleSize(0.05);

      //halfheightTimeDiffHist -> SetStats(0);
      //halfheightTimeDiffHist -> SetTitle("");
      halfheightTimeDiffHist -> GetXaxis()->SetTitle("deviation of halfheight time [ns]"); 
      halfheightTimeDiffHist -> GetYaxis()->SetTitle("entries per bin");
      halfheightTimeDiffHist -> GetXaxis()->SetTitleSize(0.05);
      halfheightTimeDiffHist -> GetYaxis()->SetTitleSize(0.05);

      //maximumTimeDiffHist -> SetStats(0);
      //maximumTimeDiffHist -> SetTitle("");
      maximumTimeDiffHist -> GetXaxis()->SetTitle("deviation of maximum time [ns]"); 
      maximumTimeDiffHist -> GetYaxis()->SetTitle("entries per bin");
      maximumTimeDiffHist -> GetXaxis()->SetTitleSize(0.05);
      maximumTimeDiffHist -> GetYaxis()->SetTitleSize(0.05);

      //minimumTimeDiffHist -> SetStats(0);
      //minimumTimeDiffHist -> SetTitle("");
      minimumTimeDiffHist -> GetXaxis()->SetTitle("deviation of minimum time [ns]"); 
      minimumTimeDiffHist -> GetYaxis()->SetTitle("entries per bin");
      minimumTimeDiffHist -> GetXaxis()->SetTitleSize(0.05);
      minimumTimeDiffHist -> GetYaxis()->SetTitleSize(0.05);

      // prepare canvas for root histograms
      TCanvas *c1 = new TCanvas("c1","");
      c1->Range(-18.4356,-0.31111,195.528,2.19048);
      c1->SetFillColor(0);
      c1->SetBorderMode(0);
      c1->SetBorderSize(2);
      c1->SetLeftMargin(0.127);
      c1->SetRightMargin(0.076);
      c1->SetBottomMargin(0.125);
      c1->SetFrameLineWidth(2);
      c1->SetFrameBorderMode(0);

      // draw histograms and save them in files 
      c1->Clear();
      SNRHist -> Draw();
      filename.str("");
      filename << "SNR_hist-" << pulseSNR << ".eps";
      c1->Print(filename.str().c_str());

      // draw histograms and save them in files 
      //c1->Clear();
      //lgSNRHist -> Draw();
      //filename.str("");
      //filename << "lgSNR_hist-" << pulseSNR << ".eps";
      //c1->Print(filename.str().c_str());

      c1->Clear();
      envelopeTimeDiffHist -> Draw();
      filename.str("");
      filename << "EnvTime_hist-" << pulseSNR << ".eps";
      c1->Print(filename.str().c_str());
      
      c1->Clear();
      halfheightTimeDiffHist -> Draw();
      filename.str("");
      filename << "HalfHeightTime_hist-" << pulseSNR << ".eps";
      c1->Print(filename.str().c_str());

      c1->Clear();
      maximumTimeDiffHist -> Draw();
      filename.str("");
      filename << "MaxTime_hist-" << pulseSNR << ".eps";
      c1->Print(filename.str().c_str());

      c1->Clear();
      minimumTimeDiffHist -> Draw();
      filename.str("");
      filename << "MinTime_hist-" << pulseSNR << ".eps";
      c1->Print(filename.str().c_str());

      cout << "\nResults for true SNR of " << pulseSNR << " are stored in root file:" << endl;
      cout << "Mean of SNR = " << SNRHist->GetMean() << " \t RMS (std. dev.) of SNR = " << SNRHist->GetRMS() << endl;
      cout << "Mean of env. time = " << envelopeTimeDiffHist->GetMean() << " \t RMS (std. dev.) = " << envelopeTimeDiffHist->GetRMS() << endl;
      
      // close root file
      rootfile->Close();

      // append results to ASCII file
      if (resultsFileName.length() > 0) {
        cout << "\nAppending results to file: " << resultsFileName << endl;
        static ofstream resultsFile;
        resultsFile.open(resultsFileName.c_str(),ios_base::app);
        if (!resultsFile.is_open()) {
          cout << "\nERROR: could not open file: " << resultsFileName << endl;
          return;
        }
        // file format:
        // true SNR, mean of SNR, std. dev. of SNR, mean of env. time, std. dev. of env. time
        resultsFile << pulseSNR << " "
                    << SNRHist->GetMean() << " "
                    << SNRHist->GetRMS() << " "
                    << envelopeTimeDiffHist->GetMean() << " "
                    << envelopeTimeDiffHist->GetRMS() 
                    << endl;
        resultsFile.close();
      }
    } catch (AipsError x) {
      cerr << "checkNoiseInfluence::addPulseToNoise: " << x.getMesg() << endl;
    }  
  }


} // Namespace CR -- end


#endif
