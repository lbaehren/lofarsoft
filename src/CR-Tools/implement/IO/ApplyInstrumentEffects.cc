/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

#include <IO/ApplyInstrumentEffects.h>

#define PI 3.1415926536

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  ApplyInstrumentEffects::ApplyInstrumentEffects(){
    init();
  }

  void ApplyInstrumentEffects::init(){
    CTRead = NULL;
    CTIElGain = NULL;
    CTIPhase = NULL;
    CTIFilter = NULL;
  }
    
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  ApplyInstrumentEffects::~ApplyInstrumentEffects ()
  {
    destroy();
  }
  
  void ApplyInstrumentEffects::destroy(){
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void ApplyInstrumentEffects::summary (std::ostream &os)
  {;}
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  
  Matrix<DComplex> ApplyInstrumentEffects::ApplyEffects(Matrix<DComplex> input){
    Matrix<DComplex> output;
    try {
      Int nAnts,nFreq,tmpind1,tmpind2,i;
      Vector<Double> tmpvec;
      Vector<DComplex> tmpCvec;
      Vector<DComplex> FilterVec,PhaseVec,GainVec,DelayVec;
      DComplex tmpComp;
      Double delay,maxdelay=0., timerange;

      nAnts = input.ncolumn();
      nFreq = input.nrow();
      if ((nAnts != AntIDs_p.nelements()) || (nFreq != frequency_p.nelements())){
	cerr << "ApplyInstrumentEffects::ApplyEffects: " << 
	  "Shape of input matrix does not match AntIDs or frequencies!!" << endl;
	return Matrix<DComplex>();
      };
      if (!InitGPFInterpolaters()){
	cerr << "ApplyInstrumentEffects::ApplyEffects: " << 
	  "Error while initializing the interpolators!!" << endl;
	return Matrix<DComplex>();	
      };
      output.resize(input.shape());
      GainVec.resize(nFreq);
      PhaseVec.resize(nFreq);
      tmpCvec.resize(nFreq);
      // Calculate the filter-effects;
      CTRead->GetData(date_p,AntIDs_p(0),"FrequencyBand",&tmpvec);
      CTIFilter->GetValues(1, 1, &FilterVec);
      tmpind1 = ntrue(frequency_p<=tmpvec(0));
      tmpind2 = ntrue(frequency_p<=tmpvec(1));
      tmpComp =  1/fabs(FilterVec(tmpind1))*0.7e-1;
      Slice tmpslice(0,tmpind1);
      FilterVec(tmpslice) = FilterVec(tmpslice)*tmpComp;
      tmpslice = Slice(tmpind1, (tmpind2-tmpind1) );
      FilterVec(tmpslice) = FilterVec(tmpslice)/fabs(FilterVec(tmpslice));
      tmpslice = Slice(tmpind2, (nFreq-tmpind2) );
      tmpComp =  1/fabs(FilterVec(tmpind2))*0.7e-1;
      FilterVec(tmpslice) = FilterVec(tmpslice)*tmpComp;
      for (i=0; i<nAnts; i++) {
	//Calculate the El-Gain Effects
	CTIElGain->GetValues(date_p, AntIDs_p(i), &tmpvec);
	convertArray(GainVec,(1./tmpvec));
	//Calculate the phase difference effects
	CTIPhase->GetValues(date_p, AntIDs_p(i), &tmpCvec);
	PhaseVec = fabs(tmpCvec)/tmpCvec;
	//Calculate the (constant) delay effects
	tmpComp = DComplex(0.,-1.);
	tmpComp *= 2*PI;
        CTRead->GetData(date_p, AntIDs_p(i), "Delay", &delay);
	if (abs(delay) > maxdelay) {maxdelay=delay;};
	convertArray(tmpCvec,frequency_p);
	DelayVec=exp(tmpComp*delay*tmpCvec);

	output.column(i) = input.column(i)*FilterVec*GainVec*PhaseVec*DelayVec;
      };      
      timerange = nFreq/(max(frequency_p)*2.); //the sampled data can span the range [-timerange:+timerange]
      if (maxdelay >= timerange) {
	cout  << "ApplyInstrumentEffects::ApplyEffects: " << "Sample timerange smaller than maxdelay!" << endl 
	      << "                                      " << "Aliasing is probable!" << endl;
      };
      
    } catch (AipsError x) {
      cerr << "ApplyInstrumentEffects::ApplyEffects: " << x.getMesg() << endl;
      return Matrix<DComplex>();
    }; 
    return output; 
  };
  
  Bool ApplyInstrumentEffects::InitGPFInterpolaters(){
    Bool success=True;
    try {
      if (CTRead == NULL) {
	cerr << "ApplyInstrumentEffects::InitGPFInterpolaters:"
	     << " CalTableReader not defined! " << endl;   
	return False;
      }
      if (CTIElGain != NULL) { delete CTIElGain; };
      if (CTIPhase  != NULL) { delete CTIFilter; };
      if (CTIFilter != NULL) { delete CTIFilter; };
      CTIElGain = new CalTableInterpolater<Double>(CTRead);
      CTIPhase = new CalTableInterpolater<DComplex>(CTRead);
      CTIFilter = new CalTableInterpolater<DComplex>(CTRead);

      success = success && CTIElGain->SetField("ElGainCal");
      success = success && CTIPhase->SetField("PhaseCal");
      success = success && CTIFilter->SetField("WWFilterS21",True); //this is a keyword

      success = success && CTIElGain->SetAxis("ElGainCalFreq");
      success = success && CTIPhase->SetAxis("PhaseCalFreq");
      success = success && CTIFilter->SetAxis("WWFilterS21Freq");

      success = success && CTIElGain->SetAxisValue(1,frequency_p);
      success = success && CTIPhase->SetAxisValue(1,frequency_p);
      success = success && CTIFilter->SetAxisValue(1,frequency_p);

    } catch (AipsError x) {
      cerr << "ApplyInstrumentEffects::InitGPFInterpolaters: " << x.getMesg() << endl;
      return False;
    }; 
    return success;    
  };

} // Namespace CR -- end
