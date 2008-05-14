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
  
  TVCalibrationPlugin::TVCalibrationPlugin (){

  parameters_p.define("frequencyRanges",  Array<Double>());
  parameters_p.define("referencePhases",  Array<Double>());
  parameters_p.define("frequencyValues",  Vector<Double>());
  parameters_p.define("sampleJumps",      Vector<Double>() );
  parameters_p.define("referenceAntenna", Int(0) );
  parameters_p.define("sampleRate",       Double(200e6) );
  parameters_p.define("badnessWeight",    Double(0.5) );
  parameters_p.define("badnessThreshold", Double(0.15) );
  parameters_p.define("AntennaMask",      Vector<Bool>() );
  parameters_p.define("SampleShiftsOnly", Bool(True) );
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
  
  Bool TVCalibrationPlugin::calcWeights(const Matrix<DComplex> &data){
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
	cerr << "TVCalibrationPlugin::calcWeights: " << "frequencyValues.nelements() != data.nrow()" << endl;
	cerr << "                                  " << frequencyValues.nelements() << " != " << data.nrow() << endl;
	return False;
      };
      if (referencePhases.nrow() != nFreqs) {
	cerr << "TVCalibrationPlugin::calcWeights: " << "referencePhases.nrow() != nFreqs" << endl;
	return False;
      };
      if (referencePhases.ncolumn() != nAntennas) {
	cerr << "TVCalibrationPlugin::calcWeights: " << "referencePhases.ncolumn() != nAntennas" << endl;
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
      
      Double badness,minbadness=0.,minJump,tmpbadness;
      Vector<Double> mindelays(nFreqs),jumpPhase(nFreqs),tmpphase(nFreqs),tmpdelay(nFreqs);
      //check for sample jumps
      for (antInd=0; antInd<nAntennas; antInd++){
	badness = ( max(relDelays.column(antInd))-min(relDelays.column(antInd)) ) * (1-badWgh) +
	  mean(abs(relDelays.column(antInd))) * badWgh;
	if (badness > (badnessThreshold*0.66666666)) { //Need to try out the sample jumps
	  minbadness = badness;
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
	    cout << "TVCalibrationPlugin: Corrected Ant: " << (antInd+1) 
                 << " by: " << correctionDelay(antInd) << " samples \t, badness: " << minbadness << endl;
	  };
	} else {
          // do corrections of minor shifts (<< full sample)
	  if (SampleShiftsOnly)
            correctionDelay(antInd) = 0.;
          else {
            correctionDelay(antInd) = mean(relDelays.column(antInd));
            cout << "TVCalibrationPlugin: Corrected Ant: " << (antInd+1) 
                 << " by: " << correctionDelay(antInd) << " samples \t, badness: " << minbadness << endl;
            }
	};
      };      
      parameters_p.define("AntennaMask",AntennaMask);
      //Some (unnecessary?) output
      //cout << "TVCalibrationPlugin: Correction delays:" << endl << correctionDelay  << endl;
      //cout << "TVCalibrationPlugin: Antenna Mask: " << AntennaMask << endl;

      //calculate the phase gradients (i.e. the new wheigths) from the correction delays
      DComplex tmpcomp(0.,1.);
      Vector<DComplex> cFreqVals;
      cFreqVals =  parameters_p.toArrayDComplex("frequencyValues");
      weights_p.resize(data.shape());
      tmpcomp *= -2.*PI/samplerate;
      for (antInd=0; antInd<nAntennas; antInd++){
	weights_p.column(antInd) = exp(tmpcomp*correctionDelay(antInd)*cFreqVals);
      };
    } catch (AipsError x) {
      cerr << "TVCalibrationPlugin::calcWeights: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };

  Vector<uInt> TVCalibrationPlugin::getPeakPos(Vector<DComplex> spectrum, Vector<Double> frequencyValues,
					       Matrix<Double> frequencyRanges){
    Vector<uInt> indices(frequencyRanges.nrow());
    try {
      uInt freq = 0; // index for the frequencies
      uInt topfreq = frequencyValues.nelements(); 
      uInt maxind;
      Double maxamp;
      // loop through different frequency ranges
      for (uInt peak = 0; peak < frequencyRanges.nrow(); peak++){
	if (frequencyValues(freq) > frequencyRanges(peak,0)) {freq=0;} // reset freq if needed
        // look for first frequency after the begin of the frequency range
	while (frequencyValues(freq) < frequencyRanges(peak,0)){
	  freq++;
	  if (freq >= topfreq) {
	    cerr << "TVCalibrationPlugin::getPeakPos: " << "(freq >= topfreq)" << endl;
	    return Vector<uInt>();
	  };
	};
        // if this frequency is allready greater than the end of the frequency range,
        // then take it, as there is no frequency inside the range,
        // else search for the peak in the frequency range.
        if (frequencyValues(freq) >= frequencyRanges(peak,1)) {
          indices(peak) = freq;
        } else {
	  maxind = freq;
	  maxamp = fabs(spectrum(freq));
	  //cout << "getPeakPos:"<<frequencyValues(freq) <<" "<< frequencyRanges(peak,1) <<" "<<freq<<endl;
	  while (frequencyValues(freq) < frequencyRanges(peak,1)) {
	    freq++;
	    if (freq >= topfreq) {
	      cerr << "TVCalibrationPlugin::getPeakPos: " << "(freq >= topfreq)" << endl;
	      return Vector<uInt>();
	    };
	    if (fabs(spectrum(freq)) > maxamp) {
	      maxind = freq;
	      maxamp = fabs(spectrum(freq));
	      //cout << "getPeakPos " << peak <<" "<< maxamp <<" "<<freq <<" "<<frequencyValues(freq)<<endl;
	    };
	  }; // while
	  indices(peak) = maxind;
        }; // if
      }; // for
    } catch (AipsError x) {
      cerr << "TVCalibrationPlugin::getPeakPos: " << x.getMesg() << endl;
      return Vector<uInt>();;
    }; 
    return indices;
  };


  // ============================================================================
  //  Helper Methods
  // ============================================================================
  
  Bool TVCalibrationPlugin::reducePhases(Array<Double> & phases){
    try {
      Vector<Double> phasesvector(phases.reform(IPosition(1,phases.nelements())));
      for (uInt i=0; i<phases.nelements(); i++){  // not fast, (maybe) not elegant, but works!
	while (phasesvector(i) > 180.) { phasesvector(i) -= 360.; };
	while (phasesvector(i) < -180.) { phasesvector(i) += 360.; };
      };
    } catch (AipsError x) {
      cerr << "TVCalibrationPlugin::reducePhases: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };

  Matrix<Double> TVCalibrationPlugin::phase2delay(Matrix<Double> phases, 
						  Vector<Double> frequencies, Double samplerate){
    Matrix<Double> delays(phases.shape());
    try {
      if (phases.nrow() != frequencies.nelements()) {
	cerr << "TVCalibrationPlugin::phase2delay: " << "phases.nrow() != frequencies.nelements()" << endl;
	return Matrix<Double>();
      };
      Vector<Double> invfreq= 1./frequencies;
      for (uInt i=0; i<phases.ncolumn(); i++){
	delays.column(i) = phases.column(i)*invfreq*(PI/180./(2.*PI)); //the compiler should optimize this...
      };
      if (samplerate != 1.){
	// convert delays in seconds to delays in sample times.
	delays *= samplerate;
      };
    } catch (AipsError x) {
      cerr << "TVCalibrationPlugin::phase2delay: " << x.getMesg() << endl;
      return Matrix<Double>();
    }; 
    return delays;
  };

  Matrix<Double> TVCalibrationPlugin::delay2phase(Matrix<Double> delays, 
						  Vector<Double> frequencies, Double samplerate){
    Matrix<Double> phases(delays.shape());
    try {
      if (delays.nrow() != frequencies.nelements()) {
	cerr << "TVCalibrationPlugin::delay2phase: " << "delays.nrow() != frequencies.nelements()" << endl;
	return Matrix<Double>();
      };
      Double convfactor;
      convfactor = 2.*PI*180./PI/samplerate;
      for (uInt i=0; i<delays.ncolumn(); i++){
	phases.column(i) = delays.column(i)*frequencies*convfactor;
      };
      reducePhases(phases);
    } catch (AipsError x) {
      cerr << "TVCalibrationPlugin::delay2phase: " << x.getMesg() << endl;
      return Matrix<Double>();
    }; 
    return phases;
  };

  Vector<Double> TVCalibrationPlugin::delay2phase(Double delay, 
						  Vector<Double> frequencies, Double samplerate){
    Vector<Double> phases(frequencies.shape());
    try {
      Double convfactor;
      convfactor = 2.*PI*180./PI/samplerate;
      phases = delay*frequencies*convfactor;
      reducePhases(phases);
    } catch (AipsError x) {
      cerr << "TVCalibrationPlugin::delay2phase: " << x.getMesg() << endl;
      return Vector<Double>();
    }; 
    return phases;
  };
  
} // Namespace CR -- end
