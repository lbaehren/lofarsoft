/*-------------------------------------------------------------------------*
 | $Id:: templates.h 391 2007-06-13 09:25:11Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

// CASA
#include <scimath/Mathematics/FFTServer.h>
// CR-Tools
#include <Analysis/CRinvFFT.h>
#include <LopesLegacy/ccBeam.h>
#include <LopesLegacy/xBeam.h>
#include <Analysis/CRdelayPlugin.h>


namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  CRinvFFT::CRinvFFT()
    : CR::SecondStagePipeline() {
    init();
  }
  
  void CRinvFFT::init(){
    AntPosValid_p = False;
    ploAntSelValid_p = False;
    AntGainInterpInit_p = False;

    InterAntGain_p = NULL;

    Double tmpval=0.;
    DirParams_p.define("Xpos",tmpval);
    DirParams_p.define("Ypos",tmpval);
    DirParams_p.define("Curvature",tmpval);
    DirParams_p.define("Az",tmpval);
    tmpval=90.;
    DirParams_p.define("El",tmpval);    
    ExtraDelay_p = 0.;
  };
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  CRinvFFT::~CRinvFFT ()
  {
    destroy();
  }
  
  void CRinvFFT::destroy () {
    AntGainInterpInit_p = False;
    if (InterAntGain_p != NULL) {
      delete InterAntGain_p;
    }
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void CRinvFFT::summary ()
  {;}
  
  Bool CRinvFFT::setPhaseCenter(Double XC, Double YC, Bool rotate){
    try {
      if (rotate) {
	Double XCn,YCn;
	// Rotation by 15.0 degrees; remark: the exact value is 15.7 but in the
	// value used in the KRETA-evaluation is 15.0, too.
	// cos 15° = 0.965925826  ;  sin 15° = 0.258819045
	XCn = XC*0.965925826+YC*0.258819045;
	YCn = XC*-0.258819045+YC*0.965925826;
	DirParams_p.define("Xpos",XCn);
	DirParams_p.define("Ypos",YCn);
      } else {
	DirParams_p.define("Xpos",XC);
	DirParams_p.define("Ypos",YC);
      };
    } catch (AipsError x) {
      cerr << "CRinvFFT::setPhaseCenter: " << x.getMesg() << endl;
      return False;
    }; 
    return True;    
  };

  Bool CRinvFFT::setDirection(Double AZ, Double EL, Double Curvature){
    try {
      DirParams_p.define("Az",AZ);
      DirParams_p.define("El",EL);
      DirParams_p.define("Curvature",Curvature);	
    } catch (AipsError x) {
      cerr << "CRinvFFT::setDirection: " << x.getMesg() << endl;
      return False;
    }; 
    return True;    
  }

  Matrix<Double> CRinvFFT::GetAntPositions(){
    Matrix<Double> NewAntPos;
    try {
      Int i,nants=AntPositions_p.ncolumn();
      Vector<Double> ReferencePos(3,0.);
      ReferencePos(0) = DirParams_p.asDouble("Ypos");
      ReferencePos(1) = DirParams_p.asDouble("Xpos");
      NewAntPos.resize(nants,3);
      for (i=0;i<nants;i++){
	NewAntPos.row(i) = AntPositions_p.column(i)-ReferencePos;
      };
    } catch (AipsError x) {
      cerr << "CRinvFFT::GetAntPositions: " << x.getMesg() << endl;
      return Matrix<Double>();
    }; 
    return NewAntPos;    
    
  };
  
  
  // ============================================================================
  //
  //  Protected Methods
  //
  // ============================================================================


  Bool CRinvFFT::initGainInterp(DataReader *dr){
    Bool success=True;
    try {
      if (InterAntGain_p != NULL) {delete InterAntGain_p;};
      InterAntGain_p = new CalTableInterpolater<Double>;

      success = success && InterAntGain_p->doCacheInput();
      success = success && InterAntGain_p->AttatchReader(CTRead);
      success = success && InterAntGain_p->SetField("AntennaGainFaktor");
      success = success && InterAntGain_p->SetAxis("AntennaGainFaktFreq");
      success = success && InterAntGain_p->SetAxis("AntennaGainFaktAz");
      success = success && InterAntGain_p->SetAxis("AntennaGainFaktEl");
      success = success && InterAntGain_p->SetAxisValue(1,dr->frequencyValues());
     
      AntGainInterpInit_p = success;      
    } catch (AipsError x) {
      cerr << "CRinvFFT::initGainInterp: " << x.getMesg() << endl;
      return False;
    }; 
    return success;
  };

  
  Matrix<DComplex> CRinvFFT::GetShiftedFFT(DataReader *dr){
    Matrix<DComplex> FFTdata;
    try {
      Vector<Int> AntennaIDs;
      uInt i,date;
      Vector<Double> tmpvec;
      dr->headerRecord().get("Date",date);
      dr->headerRecord().get("AntennaIDs",AntennaIDs);

      // ***** getting the phase gradients
      // update the antenna positions (if needed)
      if (posCachedDate_p != dr->headerRecord().asuInt("Date")) {
	AntPosValid_p = False;
      };
      if (!AntPosValid_p){
	tmpvec.resize(3);
	AntPositions_p.resize(3,AntennaIDs.nelements());
	for (i=0;i<AntennaIDs.nelements();i++){
	  CTRead->GetData(date, AntennaIDs(i), "Position", &tmpvec);
	  AntPositions_p.column(i) = tmpvec;
	};
	tmpvec.resize(2);
	FrequencyBands_p.resize(2,AntennaIDs.nelements());
	for (i=0;i<AntennaIDs.nelements();i++){
	  CTRead->GetData(date, AntennaIDs(i), "FrequencyBand", &tmpvec);
	  FrequencyBands_p.column(i) = tmpvec;
	};
	AntPosValid_p = True;
	posCachedDate_p = dr->headerRecord().asuInt("Date");
      };
      // calculate the positions relative to the phase center
      Vector<Double> ReferencePos(3,0.);
      ReferencePos(0) = DirParams_p.asDouble("Ypos");
      ReferencePos(1) = DirParams_p.asDouble("Xpos");
      Matrix<Double> tmpAntPos(AntennaIDs.nelements(),3);
      for (i=0;i<AntennaIDs.nelements();i++){
	tmpAntPos.row(i) = AntPositions_p.column(i)-ReferencePos;
      };
      // get the direction
      tmpvec.resize(3);
      tmpvec(0) = DirParams_p.asDouble("Az");
      tmpvec(1) = DirParams_p.asDouble("El");
      tmpvec(2) = DirParams_p.asDouble("Curvature");
      // Container for the phase gradients
      Matrix<DComplex> phaseGradients;
      CR::GeometricalWeight geomWeight;
      geomWeight.showProgress(verbose); 
      geomWeight.setAntennaPositions(tmpAntPos,True,CR::CoordinateType::NorthEastHeight); 
      geomWeight.setSkyPosition(DirParams_p.asDouble("Az"),DirParams_p.asDouble("El"),
				DirParams_p.asDouble("Curvature"), CR::CoordinateType::AzElRadius, True);
      geomWeight.setFrequencies(dr->frequencyValues());
#ifdef DEBUGGING_MESSAGES      
      geomWeight.summary();
#endif
      Cube<DComplex> tmpCcube;
      tmpCcube = geomWeight.weights();
#ifdef DEBUGGING_MESSAGES      
      cout << "CRinvFFT::GetShiftedFFT: delays: " <<geomWeight.delays() << endl;
#endif
      phaseGradients = Matrix<DComplex>(geomWeight.weights().nonDegenerate());
      // ***** getting the antenna gain calibration
      // initialize the caltable interpolater (if needed)      
      if (!AntGainInterpInit_p) { //AntGainInterpInit_p is defined in FirstStagePipeline.h and reset in InitEvent()
	if (!initGainInterp(dr)){
	  cerr << "CRinvFFT::GetShiftedFFT: " << "Error while initializing the CalTableInterpolater." << endl;
	  return Matrix<DComplex>();
	};
      }
            // Set the direction in the caltable interpolater
      tmpvec.resize(1);
      tmpvec(0) = DirParams_p.asDouble("Az");
      InterAntGain_p->SetAxisValue(2,tmpvec);
      tmpvec(0) = DirParams_p.asDouble("El");
      InterAntGain_p->SetAxisValue(3,tmpvec);
      // get the interpolated gains
      Matrix<DComplex> AntGainFactors(phaseGradients.shape());
      Vector<DComplex> tmpCvec;
      Array<Double> tmparr;
      DComplex tmpCval;
      tmpvec.resize(2);
      tmpCvec.resize(dr->fftLength());
      tmparr.resize(tmpCvec.shape());
      // If the calibration for the e-field should be done,
      // then take the gain simulation into accout.
      // Otherwise set the gain to 1 + 0i
      if (DoGainCal_p)
        for (i=0;i<AntennaIDs.nelements();i++){
	  InterAntGain_p->GetValues(date, AntennaIDs(i), &tmparr);
	  //CTRead->GetData(date, AntennaIDs(i), "FrequencyBand", &tmpvec);
	  //cout << "CRinvFFT::GetShiftedFFT: tmpCvec:" << tmpCvec.shape() << " tmparr:"<<tmparr.shape()
	  //     << " phaseGradients:"<< phaseGradients.shape() << endl;
	  convertArray(tmpCvec,Vector<Double>(tmparr));
	  // compute 1.947/delta_nu/sqrt(Gain)
	  //   1.947        = constant (at least for LOPES)
	  //   delta_nu     = (FreqBand(1)-FreqBand(0))/1e6 [in MHz]
	  //   1/sqrt(Gain) = tmpCvec
	  tmpCval = 1.947*1e6/(FrequencyBands_p(1,i)-FrequencyBands_p(0,i));
	  AntGainFactors.column(i) = tmpCvec*tmpCval;
        }
      else
        AntGainFactors.set(DComplex (1.,0.));
      //cout << "CRinvFFT::GetShiftedFFT: AntGainFactors:" << AntGainFactors.shape()
      //     << " phaseGradients:"<< phaseGradients.shape() << endl;
      FFTdata = phaseGradients*AntGainFactors*GetData(dr);

// Dummy until the GeometricalWeight class works as it is supposed to...
//      FFTdata = GetData(dr);

      // add extra delay to compensate for trigger etc.
      CRdelayPlugin crdel;
      crdel.parameters().define("Az",DirParams_p.asDouble("Az"));
      crdel.parameters().define("El",DirParams_p.asDouble("El"));
      crdel.parameters().define("ReferencePos",ReferencePos);
      crdel.parameters().define("frequencyValues",dr->frequencyValues());
      crdel.parameters().define("ExtraDelay",ExtraDelay_p);
      crdel.apply(FFTdata);
    } catch (AipsError x) {
      cerr << "CRinvFFT::GetShiftedFFT: " << x.getMesg() << endl;
      return Matrix<DComplex>();
    }; 
    return FFTdata;
  }

  // ============================================================================
  //
  //  Public Methods
  //
  // ============================================================================

  Matrix<Double> CRinvFFT::GetTimeSeries(DataReader *dr, Vector<Bool> antennaSelection,
					 String Polarization){
    Matrix<Double> timeSeries;
    try {
      Matrix<DComplex> FFTData;
      FFTData = GetShiftedFFT(dr);
      uInt i,j,nants=FFTData.ncolumn(),nselants,blocksize=dr->blocksize();
      if (antennaSelection.nelements() != nants){
	antennaSelection = Vector<Bool>(nants,True);
      };
      // Select Antennas according to Polarization
      if (Polarization != "ANY"){
	uInt date;
	dr->headerRecord().get("Date",date);
	if (!ploAntSelValid_p || 
	    (ploAntSelPol_p != Polarization) ||
	    (ploAntSelDate_p != date)) {
	  ploAntSel_p.resize(nants);
	  ploAntSel_p = True;
	  Vector<Int> AntennaIDs;
	  String tempstring;
	  dr->headerRecord().get("AntennaIDs",AntennaIDs);
#ifdef DEBUGGING_MESSAGES      
	  cout << "CRinvFFT::GetTimeSeries: Polarization check, flagging Antennas: ";
#endif
	  for (i=0;i<nants;i++){
	    CTRead->GetData(date, AntennaIDs(i), "Polarization", &tempstring);
	    if (tempstring != Polarization) {
#ifdef DEBUGGING_MESSAGES      
	      cout << AntennaIDs(i) << " ";
#endif
	      ploAntSel_p(i) = False;
	    };
	  };//for
#ifdef DEBUGGING_MESSAGES      
	  cout << endl ;
#endif
	  ploAntSelPol_p = Polarization;
	  ploAntSelDate_p = date;
	  ploAntSelValid_p = True;
	};// if (!ploAntSelValid_p ...
	// Apply polarization-based selection
	antennaSelection = antennaSelection && ploAntSel_p;
      };
      nselants=ntrue(antennaSelection);
      if (nselants == 0) {
	cerr << "CRinvFFT::GetTimeSeries: " << "No antennas selected/all antennas flagged!" << endl;
      };
      timeSeries.resize(blocksize,nselants);
      j=0;
      for (i=0;i<nants;i++){
	if (antennaSelection(i)){
	  timeSeries.column(j) = dr->invfft(FFTData.column(i));
	  j++;
	};
      };      
    } catch (AipsError x) {
      cerr << "CRinvFFT::GetTimeSeries: " << x.getMesg() << endl;
      return Matrix<Double>();
    }; 
    return timeSeries;
  }

  Vector<Double> CRinvFFT::GetCCBeam(DataReader *dr, Vector<Bool> antennaSelection,
				     String Polarization){
    Vector<Double> ccBeamData;
    try {
      Matrix<Double> TimeSeries;
      TimeSeries = GetTimeSeries(dr, antennaSelection, Polarization);
      LOPES::ccBeam<Double,DComplex> cc;
      ccBeamData = cc.ccbeam(TimeSeries);
    } catch (AipsError x) {
      cerr << "CRinvFFT::GetCCBeam: " << x.getMesg() << endl;
      return Vector<Double>();
    }; 
    return ccBeamData;
  }

  Vector<Double> CRinvFFT::GetXBeam(DataReader *dr, Vector<Bool> antennaSelection,
				    String Polarization){
    Vector<Double> xBeamData;
    try {
      Matrix<Double> TimeSeries;
      TimeSeries = GetTimeSeries(dr, antennaSelection, Polarization);
      LOPES::xBeam<Double,DComplex> xx;
      xBeamData = xx.xbeam(TimeSeries);
    } catch (AipsError x) {
      cerr << "CRinvFFT::GetXBeam: " << x.getMesg() << endl;
      return Vector<Double>();
    }; 
    return xBeamData;
  }

  Vector<Double> CRinvFFT::GetPBeam(DataReader *dr, Vector<Bool> antennaSelection,
				    String Polarization){
    Vector<Double> pBeamData;
    try {
      Matrix<Double> TimeSeries;
      TimeSeries = GetTimeSeries(dr, antennaSelection, Polarization);
      Int i,nants=TimeSeries.ncolumn();

      pBeamData = square(TimeSeries.column(nants-1));
      for (i=0; i<(nants-1); i++){
	pBeamData += square(TimeSeries.column(i));
      };
      pBeamData /= (Double)nants;
      pBeamData = sqrt(pBeamData);

    } catch (AipsError x) {
      cerr << "CRinvFFT::GetXBeam: " << x.getMesg() << endl;
      return Vector<Double>();
    }; 
    return pBeamData;
  }
  
  Bool CRinvFFT::GetTCXP(DataReader *dr,
			 Matrix<Double> & TimeSeries, 
			 Vector<Double> & ccBeamData,
			 Vector<Double> & xBeamData, 
			 Vector<Double> & pBeamData,
			 Vector<Bool> antennaSelection,
			 String Polarization) {
    try {
      TimeSeries = GetTimeSeries(dr, antennaSelection, Polarization);
      LOPES::ccBeam<Double,DComplex> cc;
      ccBeamData = cc.ccbeam(TimeSeries);
      LOPES::xBeam<Double,DComplex> xx;
      xBeamData = xx.xbeam(TimeSeries);

      Int i,nants=TimeSeries.ncolumn();
      pBeamData = square(TimeSeries.column(nants-1));
      for (i=0; i<(nants-1); i++){
	pBeamData += square(TimeSeries.column(i));
      };
      pBeamData /= (Double)nants;
      pBeamData = sqrt(pBeamData);

    } catch (AipsError x) {
      cerr << "CRinvFFT::GetTCX: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }

} // Namespace CR -- end
