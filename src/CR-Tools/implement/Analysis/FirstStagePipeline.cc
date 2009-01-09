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
    DoGainCal_p = True;
    DoDispersionCal_p = True;
    DoDelayCal_p = True;
    InterpInit = False;
    InterElGainCal_p = new CalTableInterpolater<Double>;
    InterPhaseCal_p = new CalTableInterpolater<DComplex>;
    CTRead = NULL;
    blocksize = 0;
    samplerate = 0.;
    startFreq_p = 0.;
    stopFreq_p = 1e99;
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
  

  Bool FirstStagePipeline::InitEvent(DataReader *dr, Bool setCal){
    try {
      //if (! lev->attachFile(filename) ){
      //cerr << "FirstStagePipeline::InitEvent: Failed to attach file: " << filename << endl;
      //return False;	
      //};
      String obsstring="",tmpstring;
      Bool newCTR=False;
      dr->headerRecord().get("Observatory",obsstring);
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
	cerr << "FirstStagePipeline::InitEvent: Unknown Observatory: " << obsstring << endl;
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
      if (setCal) { setCalibration(dr); };
    } catch (AipsError x) {
      cerr << "FirstStagePipeline::InitEvent: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };



  Bool FirstStagePipeline::setCalibration(DataReader *dr){
    try {
      Vector<Int> AntennaIDs;
      Int i,fftlen=dr->fftLength(),nAnt=dr->nofStreams();
      uInt date;
      dr->headerRecord().get("Date",date);
      dr->headerRecord().get("AntennaIDs",AntennaIDs);
      Matrix<DComplex> fftCal(fftlen,nAnt);
      Vector<Double> elgain;
      Vector<DComplex> phasecal,tempComplexVec1,tempComplexVec2;
      DComplex tmpcomp(0.,1.);
      tmpcomp *= 2*PI;
      Vector<Double> freqVals; 
      freqVals = dr->frequencyValues(); // work around a bug in casa(core) 
      Double delay;

      // create Vector to supress frequencies outside the desired band 
      if (static_cast<Int>(freqVals.size()) != fftlen) {	// consistency check
        cerr << "FirstStagePipeline::setCalibration: " 
             << "Lenght of FFT (" << fftlen << ") "
             << "does not equal length of frequency axis (" << freqVals.size() <<") !"
             << endl;
      }
      Vector<DComplex> band(fftlen);
      const DComplex supressionFactor(0.001,0.); // supression factor for frequencies outside the band
      const double startFreq = getStartFreq();	 // this values are just set now but later
      const double stopFreq = getStopFreq();     // something more elaborated can be used here
      for (i=0; i < fftlen; ++i) {
        if ( (freqVals(i) < startFreq) || (freqVals(i) > stopFreq) ) {
          band(i) = supressionFactor;
        } else {
          band(i) = DComplex(1.,0.);
        }
      }

      for (i=0; i<nAnt ; i++){
        // Get electrical gain if switched on (otherwise set it to 1)
        if (DoGainCal_p)
	  InterElGainCal_p->GetValues(date, AntennaIDs(i), &elgain);
        else
        {
          elgain.resize(fftlen);
          elgain.set(1);
        }
        // Get PhaseCal values if switched on (otherwise set them to 1+0i)
        if (DoDispersionCal_p)
	  InterPhaseCal_p->GetValues(date, AntennaIDs(i), &phasecal);
        else
        {
          phasecal.resize(fftlen);
          phasecal.set(DComplex (1.,0.));
        }
	tempComplexVec1.resize(phasecal.shape());
	convertArray(tempComplexVec1,elgain/amplitude(phasecal));
        // get delay if delay calibration is switched on (otherwise set delay to 0)
        if (DoDelayCal_p)
	  CTRead->GetData(date, AntennaIDs(i), "Delay", &delay);
        else
          delay = 0;
	tempComplexVec2.resize(freqVals.shape());
	convertArray(tempComplexVec2,freqVals);
	fftCal.column(i) = phasecal*tempComplexVec1*exp(tmpcomp*delay*tempComplexVec2)*band;
        // Comment added by Frank Schröder to understand, what is done:
        //
        // phasecal = PhaseCal values (e.g. filter characteristics) from CalTables
        // (the correction is relativly small: for arround 60-70 MHZ in the order of 10 degrees)
	//   Actually this is the difference of the phases to the reference antenna. (AH)
        //
        // tempComplexVec1 = electrical gain calibration values per frequency (from CalTables) 
        // (corrected by the amplitude of the phase calibration, which is ~ 1)
        //
        // tmpcomp = 2*pi*i
        // delay = delay from CalTables (= delay in seconds relative to antenna 1)
        // tempComplexVec2 = frequency axis
        // so the factor exp(tmpcomp*delay*tempComplexVec2) shifts the data by the 
        // antenna delays of the CalTables
        //
        // the total factor fftCal will be multiplied to the fft() of the voltage series
        // if the calibrated fft is requested.
      };
      dr->setFFT2calFFT(fftCal);

      // Set the Hanning Filter.
      // default is that the center half is flat
      // for LOPES thunderstorm mode the non-flat range is set explicitly to 3/4 of the
      // presync value 
      uInt abspresync=0;      
      if  (dr->className() == std::string("LopesEventIn")) {
	abspresync = abs(dr->headerRecord().asInt("presync"));
      };
      if ( (abspresync != 0) && (dr->blocksize()/abspresync > 2)){
	dr->setHanningFilter(0.5,(dr->blocksize() - 3*abspresync/2), (3*abspresync/4), (3*abspresync/4));
      } else {
	dr->setHanningFilter(0.5,fftlen);
      };

    } catch (AipsError x) {
      cerr << "FirstStagePipeline::setCal: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };


} // Namespace CR -- end
