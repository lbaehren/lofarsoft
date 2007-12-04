/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

/* $Id: FirstStagePipeline.cc,v 1.6 2007/06/20 15:28:11 horneff Exp $*/

#include <Analysis/FirstStagePipeline.h>

#define PI 3.1415926536

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  FirstStagePipeline::FirstStagePipeline() {
    init();
  }
  
  void FirstStagePipeline::init(){
    verbose = True; // make "debugging" output the default ;-)
    InterpInit = False;
    InterElGainCal_p = new CalTableInterpolater<Double>;
    InterPhaseCal_p = new CalTableInterpolater<DComplex>;
    CTRead = NULL;
    blocksize = 0;
    samplerate = 0.;
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  FirstStagePipeline::~FirstStagePipeline() {
    destroy();
  }
  
  void FirstStagePipeline::destroy() {
    InterpInit = False;
    delete InterElGainCal_p;
    delete InterPhaseCal_p;
    if (CTRead != NULL) { delete CTRead; };
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
  

  Bool FirstStagePipeline::Init_CalTableInter(DataReader *dr){
    Bool success=True;
    try {
      delete InterElGainCal_p;
      delete InterPhaseCal_p;
      InterElGainCal_p = new CalTableInterpolater<Double>;
      InterPhaseCal_p = new CalTableInterpolater<DComplex>;

      success = success && InterElGainCal_p->AttatchReader(CTRead);
      success = success && InterPhaseCal_p->AttatchReader(CTRead);

      success = success && InterElGainCal_p->SetField("ElGainCal");
      success = success && InterPhaseCal_p->SetField("PhaseCal");

      success = success && InterElGainCal_p->SetAxis("ElGainCalFreq");
      success = success && InterPhaseCal_p->SetAxis("PhaseCalFreq");
      
      success = success && InterElGainCal_p->SetAxisValue(1,dr->frequencyValues());
      success = success && InterPhaseCal_p->SetAxisValue(1,dr->frequencyValues());
     
    } catch (AipsError x) {
      cerr << "FirstStagePipeline::Init_CalTableInter: " << x.getMesg() << endl;
      return False;
    }; 
    return success;
  };
  

  Bool FirstStagePipeline::InitEvent(DataReader *dr){
    try {
      //if (! lev->attachFile(filename) ){
      //cerr << "FirstStagePipeline::InitEvent: Failed to attach file: " << filename << endl;
      //return False;	
      //};
      String obsstring="",tmpstring;
      Bool newCTR=False;
      dr->header().get("Observatory",obsstring);
      if (ObsToCTPath.isDefined(obsstring)){
	if (CTRead!=NULL){
	  CTRead->GetKeyword("Observatory",&tmpstring);
	  if (tmpstring != obsstring){
	    newCTR=True;
	  }; 
	} else {
	  newCTR=True;
	};
      } else {
	cerr << "FirstStagePipeline::InitEvent: Unknown Observatory: " << tmpstring << endl;
	return False;	
      };
      if (newCTR){
	if (CTRead!=NULL) delete CTRead;
	CTRead = new CalTableReader(ObsToCTPath.asString(obsstring));
      };
      if ((newCTR) || (blocksize!=dr->blocksize()) || (samplerate!=dr->sampleFrequency())){
	blocksize = dr->blocksize();
	samplerate = dr->sampleFrequency();
	SecondStageCacheValid_p = False;
	AntGainInterpInit_p = False;
	if (! Init_CalTableInter(dr) ){
	  cerr << "FirstStagePipeline::InitEvent: Failed to initialize CalTableInterpolaters " << endl;
	  return False;	
	};
      };
      Vector<Int> AntennaIDs;
      Int i,fftlen=dr->fftLength(),nAnt=dr->nofStreams();
      uInt date;
      dr->header().get("Date",date);
      dr->header().get("AntennaIDs",AntennaIDs);
      Matrix<DComplex> fftCal(fftlen,nAnt);
      Vector<Double> elgain;
      Vector<DComplex> phasecal,tempComplexVec1,tempComplexVec2;
      DComplex tmpcomp(0.,1.);
      tmpcomp *= 2*PI;
      Vector<Double> freqVals = dr->frequencyValues();
      Double delay;
      for (i=0; i<nAnt ; i++){
	InterElGainCal_p->GetValues(date, AntennaIDs(i), &elgain);
	InterPhaseCal_p->GetValues(date, AntennaIDs(i), &phasecal);
	tempComplexVec1.resize(phasecal.shape());
	convertArray(tempComplexVec1,elgain/amplitude(phasecal));
	CTRead->GetData(date, AntennaIDs(i), "Delay", &delay);
	tempComplexVec2.resize(freqVals.shape());
	convertArray(tempComplexVec2,freqVals);
	fftCal.column(i) = phasecal*tempComplexVec1*exp(tmpcomp*delay*tempComplexVec2);
      };
      dr->setFFT2calFFT(fftCal);
      dr->setHanningFilter(0.5,(fftlen/2));

    } catch (AipsError x) {
      cerr << "FirstStagePipeline::InitEvent: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };

} // Namespace CR -- end
