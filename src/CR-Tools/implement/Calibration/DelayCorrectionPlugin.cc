/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
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

#include <Calibration/DelayCorrectionPlugin.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  DelayCorrectionPlugin::DelayCorrectionPlugin () {
    parameters_p.define("frequencyValues",  Vector<Double>());
    parameters_p.define("delays",           Vector<Double>());
    parameters_p.define("sampleRate",       Double(80e6) );
  }
  
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  DelayCorrectionPlugin::~DelayCorrectionPlugin () {
    ;
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void DelayCorrectionPlugin::summary (std::ostream &os)
  {;}
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
    
  Bool DelayCorrectionPlugin::calcWeights(const Matrix<DComplex> &data){
    try {
      // get values out of the parameters record.
      Vector<DComplex> cFreqVals = parameters_p.toArrayDComplex("frequencyValues");
      Vector<Double> delays      = parameters_p.asArrayDouble("delays"); 
      Double samplerate          = parameters_p.asDouble("sampleRate"); 

      uInt antInd,nAntennas = data.ncolumn();

      // check shapes of the different input data
      if (cFreqVals.nelements() != data.nrow()){
	cerr << "DelayCorrectionPlugin::calcWeights: " << "cFreqVals.nelements() != data.nrow()" << endl;
	cerr << "                                    " << cFreqVals.nelements() << " != " << data.nrow() << endl;
	return False;
      };
      if (delays.nelements() != nAntennas) {
	cerr << "DelayCorrectionPlugin::calcWeights: " << "delays.nelements() != nAntennas" << endl;
	return False;
      };

      //calculate the phase gradients (i.e. the new wheigths) from the correction delays
      DComplex tmpcomp(0.,1.);
      weights_p.resize(data.shape());
      tmpcomp *= -2.*PI/samplerate;
      for (antInd=0; antInd<nAntennas; antInd++){
	weights_p.column(antInd) = exp(tmpcomp*delays(antInd)*cFreqVals);
      };
    } catch (AipsError x) {
      cerr << "DelayCorrectionPlugin::calcWeights: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };


  // ============================================================================
  //
  //  "Service" Methods, not used in here but useful for dericved classes
  //
  // ============================================================================
  

  Vector<uInt> DelayCorrectionPlugin::getPeakPos(Vector<DComplex> spectrum, Vector<Double> frequencyValues,
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
	    cerr << "DelayCorrectionPlugin::getPeakPos: " << "(freq >= topfreq)" << endl;
	    return Vector<uInt>();
	  };
	};
        // if this frequency is already greater or equal than the end of the frequency range,
        // then it is automaticaly taken
	maxind = freq;
	maxamp = fabs(spectrum(freq));
	//cout << "getPeakPos:"<<frequencyValues(freq) <<" "<< frequencyRanges(peak,1) <<" "<<freq<<endl;
	while (frequencyValues(freq) < frequencyRanges(peak,1)) {
	  freq++;
	  if (freq >= topfreq) {
	    cerr << "DelayCorrectionPlugin::getPeakPos: " << "(freq >= topfreq)" << endl;
	    return Vector<uInt>();
	  };
	  if (fabs(spectrum(freq)) > maxamp) {
	    maxind = freq;
	    maxamp = fabs(spectrum(freq));
	    //cout << "getPeakPos " << peak <<" "<< maxamp <<" "<<freq <<" "<<frequencyValues(freq)<<endl;
	  };
	}; // while
	indices(peak) = maxind;
      }; // for
    } catch (AipsError x) {
      cerr << "DelayCorrectionPlugin::getPeakPos: " << x.getMesg() << endl;
      return Vector<uInt>();;
    }; 
    return indices;
  };

  Bool DelayCorrectionPlugin::reducePhases(Array<Double> & phases){
    try {
      Vector<Double> phasesvector(phases.reform(IPosition(1,phases.nelements())));
      for (uInt i=0; i<phases.nelements(); i++){  // not fast, (maybe) not elegant, but works!
	while (phasesvector(i) > 180.) { phasesvector(i) -= 360.; };
	while (phasesvector(i) < -180.) { phasesvector(i) += 360.; };
      };
    } catch (AipsError x) {
      cerr << "DelayCorrectionPlugin::reducePhases: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };

  Matrix<Double> DelayCorrectionPlugin::phase2delay(Matrix<Double> phases, 
						  Vector<Double> frequencies, Double samplerate){
    Matrix<Double> delays(phases.shape());
    try {
      if (phases.nrow() != frequencies.nelements()) {
	cerr << "DelayCorrectionPlugin::phase2delay: " << "phases.nrow() != frequencies.nelements()" << endl;
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
      cerr << "DelayCorrectionPlugin::phase2delay: " << x.getMesg() << endl;
      return Matrix<Double>();
    }; 
    return delays;
  };

  Matrix<Double> DelayCorrectionPlugin::delay2phase(Matrix<Double> delays, 
						  Vector<Double> frequencies, Double samplerate){
    Matrix<Double> phases(delays.shape());
    try {
      if (delays.nrow() != frequencies.nelements()) {
	cerr << "DelayCorrectionPlugin::delay2phase: " << "delays.nrow() != frequencies.nelements()" << endl;
	return Matrix<Double>();
      };
      Double convfactor;
      convfactor = 2.*PI*180./PI/samplerate;
      for (uInt i=0; i<delays.ncolumn(); i++){
	phases.column(i) = delays.column(i)*frequencies*convfactor;
      };
      reducePhases(phases);
    } catch (AipsError x) {
      cerr << "DelayCorrectionPlugin::delay2phase: " << x.getMesg() << endl;
      return Matrix<Double>();
    }; 
    return phases;
  };

  Vector<Double> DelayCorrectionPlugin::delay2phase(Double delay, 
						  Vector<Double> frequencies, Double samplerate){
    Vector<Double> phases(frequencies.shape());
    try {
      Double convfactor;
      convfactor = 2.*PI*180./PI/samplerate;
      phases = delay*frequencies*convfactor;
      reducePhases(phases);
    } catch (AipsError x) {
      cerr << "DelayCorrectionPlugin::delay2phase: " << x.getMesg() << endl;
      return Vector<Double>();
    }; 
    return phases;
  };
  

} // Namespace CR -- end
