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
      // calculate the maxima
      double noiseTime = pulseStart-1e-4;   // noise range is normally calculated in respect to the time of the CC beam
      calibPulses = CompleteBeamPipe_p->calculateMaxima(lev_p, AntennaSelection, getUpsamplingExponent(), false, noiseTime, 3);
      
      // load the pulse pattern: calculate time range of pulse and get up-sampled trace in this range
      Vector<Double> timeValues =
        CompleteBeamPipe_p->getUpsampledTimeAxis(lev_p, getUpsamplingExponent());
      Slice range= CompleteBeamPipe_p->calculatePlotRange(timeValues);
      pulsePattern =
        CompleteBeamPipe_p->getUpsampledFieldstrength(lev_p, getUpsamplingExponent(), AntennaSelection).column(antenna-1)(range).copy();
      pulseHeight = calibPulses.begin()->second.envelopeMaximum;
      
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
      // calculate the maxima
      double noiseTime = plotStart()+1e-7;   // noise range is normally calculated in respect to the time of the CC beam
      calibPulses = CompleteBeamPipe_p->calculateMaxima(lev_p, AntennaSelection, getUpsamplingExponent(), false, noiseTime, 3);
   
    } catch (AipsError x) {
      cerr << "checkNoiseInfluence::loadNoiseEvent: " << x.getMesg() << endl;
    }  
  }

} // Namespace CR -- end


#endif
