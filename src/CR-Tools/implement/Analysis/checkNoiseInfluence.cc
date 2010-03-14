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
    noiseIntervalLength(10e-6),
    noiseIntervalGap(5e-6),
    NnoiseIntervals(2),
    upsampledNoise( Matrix<Double>() )
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
  //  Parameters
  //
  // ============================================================================

  void checkNoiseInfluence::summary (std::ostream &os)
  {}

  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  void checkNoiseInfluence::loadPulsePattern(const string& evname,
                                             const int& antenna, 
                                             const double& pulseStart,
                                             const double& pulseStop)
  {
    try {   
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
      CompleteBeamPipe_p->setPlotInterval(pulseStart,pulseStop);
      CompleteBeamPipe_p->setCalibrationMode(true);

      // Plot chosen pulse pattern
      CompleteBeamPipe_p->plotAllAntennas("pulsePattern", lev_p, AntennaSelection, true,
                                          getUpsamplingExponent(),false, true);
      // calculate the maxima (calibPulses is defined in analyseLOPESevent2)
      calibPulses = CompleteBeamPipe_p->calculateMaxima(lev_p, AntennaSelection, getUpsamplingExponent(), false,
                                                        1e99, 3, pulseStart - 10.5e-6, pulseStart - 0.5e-6);
      
      // load the pulse pattern: calculate time range of pulse and get up-sampled trace in this range
      Vector<Double> timeValues =
        CompleteBeamPipe_p->getUpsampledTimeAxis(lev_p, getUpsamplingExponent());
      Slice range= CompleteBeamPipe_p->calculatePlotRange(timeValues);
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
                                         
      // calculate the maxima
      for (int i = 0; i < NnoiseIntervals; ++i) {
        setPlotInterval(startTime + i*(noiseIntervalLength+noiseIntervalGap), startTime + i*(noiseIntervalLength+noiseIntervalGap) + noiseIntervalLength);
        CompleteBeamPipe_p->setPlotInterval(plotStart(),plotStop());
        double noiseTime = plotStart()+pulseTime;   // noise range is normally calculated in respect to the time of the CC beam (where the pulse is expected)
        calibPulses = CompleteBeamPipe_p->calculateMaxima(lev_p, AntennaSelection, getUpsamplingExponent(), false,
                                                          noiseTime, 3, plotStart(), plotStop());
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
  
  void checkNoiseInfluence::addPulseToNoise(const double& pulseSNR)                          
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

      CompleteBeamPipe_p->setUpsampledFieldStrength(upsampledNoise);

      // create a plot to see, if things work
      CompleteBeamPipe_p->setPlotInterval(plotStart(),plotStop());
      CompleteBeamPipe_p->setCalibrationMode(true);

      // Plot noise
      CompleteBeamPipe_p->plotAllAntennas("noise2", lev_p, AntennaSelection, false,
                                          getUpsamplingExponent(),false, false);

      // calculate the maxima in same intervals as noise
      for (int i = 0; i < NnoiseIntervals; ++i) {
        setPlotInterval(startTime + i*(noiseIntervalLength+noiseIntervalGap), startTime + i*(noiseIntervalLength+noiseIntervalGap) + noiseIntervalLength);
        CompleteBeamPipe_p->setPlotInterval(plotStart(),plotStop());
        calibPulses = CompleteBeamPipe_p->calculateMaxima(lev_p, AntennaSelection, getUpsamplingExponent(), false,
                                                          plotStart()+pulseTime, 3, plotStart(), plotStop());
        vector<double> noiseValues = noiseHeight[i]; // get the noise heights for one noise interval
        // loop through all antennas and get the signal-to-noise ratio                              
        cout << "\nNoise/Noise = " << endl;
        unsigned int j = 0;
        for( map<int, PulseProperties>::iterator it = calibPulses.begin(); it != calibPulses.end(); ++it ) {        
          cout << it->second.noise/noiseValues[j] << " " << endl;
          ++j;
        } 
        j=0;
        cout << "\nSignal/Noise = " << endl;
        for( map<int, PulseProperties>::iterator it = calibPulses.begin(); it != calibPulses.end(); ++it ) {
          cout << it->second.envelopeMaximum/noiseValues[j] << " " << endl;
          ++j;
        } 
      }  
    } catch (AipsError x) {
      cerr << "checkNoiseInfluence::addPulseToNoise: " << x.getMesg() << endl;
    }  
  }


} // Namespace CR -- end


#endif
