/***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

/* $Id: TVCalibrationPlugin.cc,v 1.3 2007/04/20 14:49:18 horneff Exp $*/

#include <Calibration/TVCalibrationPlugin.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  TVCalibrationPlugin::TVCalibrationPlugin():
    DelayCorrectionPlugin() {

  parameters_p.define("frequencyRanges",  Array<Double>());
  parameters_p.define("referencePhases",  Array<Double>());
  parameters_p.define("frequencyValues",  Vector<Double>());
  parameters_p.define("sampleJumps",      Vector<Double>() );
  parameters_p.define("referenceAntenna", Int(0) );
  parameters_p.define("sampleRate",       Double(200e6) );
  parameters_p.define("badnessWeight",    Double(0.5) );
  parameters_p.define("badnessThreshold", Double(0.15) );
  parameters_p.define("AntennaMask",      Vector<Bool>() );
  parameters_p.define("SampleShiftsOnly", Bool(False) );
  }
    
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  TVCalibrationPlugin::~TVCalibrationPlugin ()
  {;
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================ 
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Bool TVCalibrationPlugin::calcDelays(const Matrix<DComplex> &data){
    try {
      // get values out of the parameters record.
      Matrix<Double> frequencyRanges = parameters_p.asArrayDouble("frequencyRanges");
      Matrix<Double> referencePhases = parameters_p.asArrayDouble("referencePhases");
      Vector<Double> frequencyValues = parameters_p.asArrayDouble("frequencyValues");
      Vector<Double> sampleJumps     = parameters_p.asArrayDouble("sampleJumps"); 
      uInt refAnt = parameters_p.asInt("referenceAntenna");
      Double samplerate = parameters_p.asDouble("sampleRate"); 
      Double badWgh = parameters_p.asDouble("badnessWeight");
      Double badnessThreshold = parameters_p.asDouble("badnessThreshold");
      Bool SampleShiftsOnly = parameters_p.asBool("SampleShiftsOnly");

      uInt antInd,freqInd;
      uInt nAntennas = data.ncolumn();
      uInt nFreqs = frequencyRanges.nrow();
      Vector<uInt> peakPos(nFreqs);
      Vector<Double> correctionDelay(nAntennas);
      Vector<Bool> AntennaMask(nAntennas,True);

      // check shapes of the different input data
      if (frequencyValues.nelements() != data.nrow()){
	cerr << "TVCalibrationPlugin::calcDelays: " << "frequencyValues.nelements() != data.nrow()" << endl;
	cerr << "                                  " << frequencyValues.nelements() << " != " << data.nrow() << endl;
	return False;
      };
      if (referencePhases.nrow() != nFreqs) {
	cerr << "TVCalibrationPlugin::calcDelays: " << "referencePhases.nrow() != nFreqs" << endl;
	return False;
      };
      if (referencePhases.ncolumn() != nAntennas) {
	cerr << "TVCalibrationPlugin::calcDelays: " << "referencePhases.ncolumn() != nAntennas" << endl;
	return False;
      };

      //calculate the peak position (from the reference antenna...)
      peakPos = getPeakPos(data.column(refAnt), frequencyValues, frequencyRanges);
      Vector<Double> freqs(nFreqs);
      for (freqInd=0; freqInd<nFreqs; freqInd++) { freqs(freqInd) = frequencyValues(peakPos(freqInd)); };

      //calculate relative phases
      Matrix<Double> relPhases(nFreqs,nAntennas);
      Vector<Double> refAntPhases(nFreqs);
      for (freqInd=0; freqInd<nFreqs; freqInd++) {
	relPhases.row(freqInd) = phase(data.row(peakPos(freqInd)))*180./PI;
      };
      refAntPhases = relPhases.column(refAnt);
      for (freqInd=0; freqInd<nFreqs; freqInd++) {
	relPhases.row(freqInd) = relPhases.row(freqInd) - refAntPhases(freqInd);
      }; // relPhases now holds the phases relative to the phases of the reference antenna

      //Calculate the phase differences
      Matrix<Double> PhaseDiffs;
      PhaseDiffs = relPhases-referencePhases;
      reducePhases(PhaseDiffs);
      Matrix<Double> relDelays;
      relDelays = phase2delay(PhaseDiffs,freqs,samplerate);

      //Dump some data to compare to Glish results
      //      cout << "Peak Positions:" << peakPos << "Peak Frequencies" << freqs <<endl;
      //      cout << "Raw Phases:" << endl << relPhases << endl;
      //      reducePhases(relPhases);
      //      cout << "Relative Phases:" << endl << relPhases << endl;      
      //      cout << "Reference Phases:" << endl << referencePhases  << endl;
      //      cout << "Phase differences:" << endl << PhaseDiffs  << endl;
      //      cout << "relative delays:" << endl <<  relDelays << endl;
      
      Double minbadness,minJump,tmpbadness;
      Vector<Double> mindelays(nFreqs),jumpPhase(nFreqs),tmpphase(nFreqs),tmpdelay(nFreqs);
      //check for sample jumps
      for (antInd=0; antInd<nAntennas; antInd++){
	minbadness = ( max(relDelays.column(antInd))-min(relDelays.column(antInd)) ) * (1-badWgh) +
	  mean(abs(relDelays.column(antInd))) * badWgh;
	if (minbadness > (badnessThreshold*0.66666666)) { //Need to try out the sample jumps
	  mindelays = relDelays.column(antInd);
	  minJump = 0.;
	  for (uInt i=0; i<sampleJumps.nelements(); i++){
	    jumpPhase = delay2phase(sampleJumps(i),freqs,samplerate);
	    tmpphase = PhaseDiffs.column(antInd)-jumpPhase;
	    reducePhases(tmpphase);
	    tmpdelay = phase2delay(tmpphase,freqs,samplerate);
	    tmpbadness = ( max(tmpdelay)-min(tmpdelay) ) * (1-badWgh) + mean(abs(tmpdelay)) * badWgh;
	    if (minbadness>(tmpbadness+0.01)) { //have some hysteresis
	      //	      cout << "ant"<<antInd+1<<" jump:"<<minJump<<"->"<<sampleJumps(i)<<"; badness:"<<minbadness<<"->"<<tmpbadness<<endl;
	      minbadness = tmpbadness;
	      mindelays = tmpdelay;
	      minJump = sampleJumps(i);
	    };
	  };
	  if (minbadness >= badnessThreshold) { 
	    cout << "TVCalibrationPlugin: Not trusting Ant: " << (antInd+1) << " with residual badness: " << minbadness <<endl;
	    AntennaMask(antInd) = False;
	    correctionDelay(antInd) = 0.;
	  } else {
            // if requested correct only for full sample shifts
            if (SampleShiftsOnly)
	      correctionDelay(antInd) = minJump;
            else
	      correctionDelay(antInd) = mean(mindelays) + minJump;
#ifdef DEBUGGING_MESSAGES      
	    cout << "TVCalibrationPlugin: Corrected Ant: " << (antInd+1) 
                 << " by: " << correctionDelay(antInd) << " samples \t, badness: " << minbadness << endl;
#endif
	  };
	} else {
          // do corrections of minor shifts (<< full sample)
	  if (SampleShiftsOnly)
            correctionDelay(antInd) = 0.;
          else {
            correctionDelay(antInd) = mean(relDelays.column(antInd));
#ifdef DEBUGGING_MESSAGES      
            cout << "TVCalibrationPlugin: Corrected Ant: " << (antInd+1) 
                 << " by: " << correctionDelay(antInd) << " samples \t, badness: " << minbadness << endl;
#endif
	  }
	};
      };      
      parameters_p.define("AntennaMask",AntennaMask);
      //Some (unnecessary?) output
      //cout << "TVCalibrationPlugin: Correction delays:" << endl << correctionDelay  << endl;
      //cout << "TVCalibrationPlugin: Antenna Mask: " << AntennaMask << endl;

      parameters_p.define("delays", correctionDelay);

    } catch (AipsError x) {
      cerr << "TVCalibrationPlugin::calcDelays: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };

  // ============================================================================
  //  Helper Methods
  // ============================================================================
  
} // Namespace CR -- end
