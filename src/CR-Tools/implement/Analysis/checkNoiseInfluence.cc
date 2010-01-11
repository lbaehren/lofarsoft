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
    : analyseLOPESevent2()
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

      pipeline_p->setVerbosity(true);

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
        cerr << "checkNoiseInfluence::loadPulsePattern: " << "Antenna number out ou range." << endl;
        return;
      }
      AntennaSelection(antenna-1)=true;

      // initialize Complete Pipeline
      CompleteBeamPipe_p = static_cast<CompletePipeline*>(pipeline_p);
      CompleteBeamPipe_p->setPlotInterval(plotStart(),plotStop());
      CompleteBeamPipe_p->setCalibrationMode(true);

      // Plot the raw data, if desired
      CompleteBeamPipe_p->plotAllAntennas("pulsePattern", lev_p, AntennaSelection, true,
                                          getUpsamplingExponent(),false, true);
      // calculate the maxima
      double noiseTime = pulseStart-1e-4;   // noise range is normally calculated in respect to the time of the CC beam
      calibPulses = CompleteBeamPipe_p->calculateMaxima(lev_p, AntennaSelection, getUpsamplingExponent(), true, noiseTime);
    } catch (AipsError x) {
      cerr << "checkNoiseInfluence::loadPulsePattern: " << x.getMesg() << endl;
    }
  }

} // Namespace CR -- end


#endif
