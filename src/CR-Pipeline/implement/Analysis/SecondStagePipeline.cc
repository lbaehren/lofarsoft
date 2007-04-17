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

/* $Id: SecondStagePipeline.cc,v 1.4 2007/04/16 17:02:02 horneff Exp $*/

#include <Analysis/SecondStagePipeline.h>

/*!
  \class SecondStagePipeline
*/

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  SecondStagePipeline::SecondStagePipeline() 
    : CR::FirstStagePipeline() {
     init();
  };
  
  void SecondStagePipeline::init(){
    cachedDate_p = 0;

    // Initialize standard values of the PhaseCalibration Plugin
    pCal_p.parameters().define("badnessWeight",0.5);
    pCal_p.parameters().define("referenceAntenna",0);
  };
  
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  SecondStagePipeline::~SecondStagePipeline ()
  {
    destroy();
  }
  
  void SecondStagePipeline::destroy ()
  {;}
  
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

  Matrix<DComplex> SecondStagePipeline::GetData(DataReader *dr){
    try {
      if (cachedDate_p != dr->header().asuInt("Date")) {
	if (!updateCache(dr)) {
	  cerr << "SecondStagePipeline::GetData: " << "updateCache failed!" << endl;
	  return Matrix<DComplex>();
	};
      };
    } catch (AipsError x) {
      cerr << "SecondStagePipeline::GetData: " << x.getMesg() << endl;
      return Matrix<DComplex>();
    }; 
    return CachedData_p;
  }
  
  Bool SecondStagePipeline::updateCache(DataReader *dr){
    try {
      Matrix<DComplex> data;
      data = dr->calfft();

      InitPhaseCal(dr);
      // Do the phase calibration
      pCal_p.calcWeights(dr->fft());
      pCal_p.apply(data,False);
      pCal_p.parameters().get("AntennaMask",AntennaMask_p);

      rfiM_p.parameters().define("dataBlockSize",(Int)dr->blocksize());
      // Do the RFI mitigation
      rfiM_p.apply(data,True);

      CachedData_p.reference(data);
    } catch (AipsError x) {
      cerr << "SecondStagePipeline::updateCache: " << x.getMesg() << endl;
      return False;
    }; 
    return True;

  };
 
  Bool SecondStagePipeline::InitPhaseCal(DataReader *dr){
    try {
      Vector<Int> AntennaIDs;
      uInt date;
      dr->header().get("Date",date);
      dr->header().get("AntennaIDs",AntennaIDs);
      Vector<Double> tmpvec;
      //      Vector<Int> tmpIntvec;
      Matrix<Double> tmpmat;
      Int i,numAntennas=AntennaIDs.nelements();

      tmpvec= dr->frequencyValues();
      pCal_p.parameters().define("frequencyValues",tmpvec);
      pCal_p.parameters().define("sampleRate",dr->sampleFrequency());
      CTRead->GetData(date, AntennaIDs(0), "SampleJumps", &tmpvec);
      pCal_p.parameters().define("sampleJumps",tmpvec);
      CTRead->GetData(date, AntennaIDs(0), "PhaseRefFreqs", &tmpmat);
      if (tmpmat.ncolumn()!=2) {
	cerr << "PhaseRefFreqs: tmpmat.ncolumn()!=2 !!!" << endl;
      };
      pCal_p.parameters().define("frequencyRanges",tmpmat);
      Int nFreqranges = tmpmat.nrow();
      tmpmat.resize(nFreqranges,numAntennas);
      for (i=0; i<numAntennas; i++) {
	CTRead->GetData(date, AntennaIDs(i), "PhaseRefPhases", &tmpvec);
	tmpmat.column(i) = tmpvec;
      };
      pCal_p.parameters().define("referencePhases",tmpmat);
      
    } catch (AipsError x) {
      cerr << "SecondStagePipeline::InitPhaseCal: " << x.getMesg() << endl;
      return False;
    }; 
    return True;

  };
  /*
  Bool SecondStagePipeline::InitRFIMitigation(DataReader *dr){
    try {
      
    } catch (AipsError x) {
      cerr << "SecondStagePipeline::InitRFIMitigation: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };
  */



} // Namespace CR -- end
