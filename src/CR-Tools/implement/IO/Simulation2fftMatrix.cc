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

/* $Id: Simulation2fftMatrix.cc,v 1.3 2007/03/27 14:38:46 horneff Exp $*/

#include <IO/Simulation2fftMatrix.h>
#include <scimath/Mathematics/FFTServer.h>
#include <casa/Arrays/ArrayMath.h>

//Maximum number of antennas that this routine can handle.
//LOPES has a maximum of 30 antennas
#define MAXANTS 35

namespace CR {  //  Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  Simulation2fftMatrix::Simulation2fftMatrix (){
    fftlen_p = blocklen_p = 0;
    date_p = 1104580800;
    CTRead=NULL;
    has_data_p = False;
    CTIzeni=NULL;
    CTIazi=NULL;
    CTIzeniPhase=NULL;
    CTIaziPhase=NULL;
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  Simulation2fftMatrix::~Simulation2fftMatrix ()
  {;}
  
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  // ============================================================================
  //
  //  Output Methods
  //
  // ============================================================================

  // ---------------------------------------------------------------- getBlocklen
  
  Int Simulation2fftMatrix::getBlocklen(){
    return blocklen_p;
  };
  
  // --------------------------------------------------------------- getFrequency

  Vector<Double> Simulation2fftMatrix::getFrequency(){
    if (has_data_p) {
      return frequency_p;
    };
    return Vector<Double>();
  };
  
  // --------------------------------------------------------------------- getfft

  Matrix<DComplex> Simulation2fftMatrix::getfft(){
    if (has_data_p) {
      return fft_p;
    };
    return Matrix<DComplex>();
  };
  
  // ------------------------------------------------------------------ getAntIDs

  Vector<Int> Simulation2fftMatrix::getAntIDs(){
    if (has_data_p) {
      return AntIDs_p;
    };
    return Vector<Int>();
  };
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  // ----------------------------------------------------------- SimulationImport
  
  Bool Simulation2fftMatrix::SimulationImport (ImportSimulation *inputSim){
    Bool success=True;
    if (inputSim == NULL) {
      cerr << "Simulation2fftMatrix::SimulationImport: inputSim not defined! "
	   << endl;   
      return False;
    }
    try {
      Int i,nAnt=0;
      Double Az,El,TimeScale;
      const ImportAntenna* pAnt;
      Vector <Double> inEazi,inEzeni,gainZeni,gainAzi;
      Vector<DComplex> outEazi,outEzeni,gainZeniC,gainAziC;
      
      inputSim->getDirection(Az,El);
      success = success && inputSim->getNextAntenna(pAnt);
      if (!success){
	cerr << "Simulation2fftMatrix::SimulationImport:"
	     << " No (more) antennas in simulation! "
	     << endl;   
	return False;
      }
      success = success && pAnt->getTimeSeries(inEazi,inEzeni);
      blocklen_p = inEazi.nelements();
      if (blocklen_p != (Int)inEzeni.nelements()) {
	cerr << "Simulation2fftMatrix::SimulationImport:"
	     << " inEazi and inEzeni have different length"
	     << endl;   
	return False;
      }
      FFTServer<Double,DComplex> server(IPosition(1,blocklen_p),
					FFTEnums::REALTOCOMPLEX);
      server.fft(outEazi,inEazi);
      server.fft(outEzeni,inEzeni);
      fftlen_p = outEazi.nelements();
      Matrix<DComplex> tempFFT (fftlen_p, MAXANTS);
      Vector<Int> tempAntIDs(MAXANTS);
      tempAntIDs(nAnt) = atoi(pAnt->getID().c_str());
      
      TimeScale = pAnt->getSamplingTimeScale();
      success = success && CalcFreqeuncies(TimeScale);
      success = success && initCTinterpolater(Az,El);
      
      tempFFT.column(nAnt) = combinePolarizations(tempAntIDs(nAnt),
						  outEazi,
						  outEzeni);
      nAnt++;
      while (success && inputSim->getNextAntenna(pAnt)) {
	success = success && pAnt->getTimeSeries(inEazi,inEzeni);
	if ((blocklen_p != (Int)inEazi.nelements())||(blocklen_p != (Int)inEzeni.nelements())) {
	  cerr << "Simulation2fftMatrix::SimulationImport:"
	       << " inEazi and/or inEzeni does not have standard length!" 
	       << endl;
	  return False;
	};
	if ( abs((pAnt->getSamplingTimeScale()-TimeScale)/TimeScale) > 1e-10 ) {
	  cerr << "Simulation2fftMatrix::SimulationImport:"
	       << " Different SamplingTimeScale in antennas!" 
	       << endl;
	  return False;
	};
	server.fft(outEazi,inEazi);
	server.fft(outEzeni,inEzeni);
	tempAntIDs(nAnt) = atoi(pAnt->getID().c_str());
	tempFFT.column(nAnt) = combinePolarizations(tempAntIDs(nAnt),
						    outEazi,
						    outEzeni);
	nAnt++;
      };
      if (success){
	AntIDs_p.resize(nAnt);
	AntIDs_p = tempAntIDs(Slice(0,nAnt));
	fft_p.resize(fftlen_p,nAnt);
	for (i=0; i<nAnt; i++) {
	  fft_p.column(i) = tempFFT.column(i);
	};
      } else {
	cout << "Simulation2fftMatrix::SimulationImport: Not successfull!"
	     << endl;
      };
    } catch (AipsError x) {
      cerr << "Simulation2fftMatrix::SimulationImport: " << x.getMesg() << endl;
      return False;
    }; 
    has_data_p = success;
    return success;
  };

  // --------------------------------------------------------- initCTinterpolater

  Bool Simulation2fftMatrix::initCTinterpolater (Double azimuth,
						 Double elevation)
  {
    Bool success=True;
    try {
      if (CTRead == NULL) {
	cerr << "Simulation2fftMatrix::initCTinterpolater:"
	     << " CalTableReader not defined! " << endl;   
	return False;
      }
      if (CTIzeni != NULL) { delete CTIzeni; };
      if (CTIazi != NULL) { delete CTIazi; };
      if (CTIzeniPhase != NULL) { delete CTIzeniPhase; };
      if (CTIaziPhase != NULL) { delete CTIaziPhase; };
      CTIzeni = new CalTableInterpolater<Double>(CTRead);
      CTIazi = new CalTableInterpolater<Double>(CTRead);
      CTIzeniPhase = new CalTableInterpolater<Double>(CTRead);
      CTIaziPhase = new CalTableInterpolater<Double>(CTRead);
      
      success = success && CTIzeni->SetField("AntennaZeniGain");
      success = success && CTIazi->SetField("AntennaAziGain");
      success = success && CTIzeniPhase->SetField("AntennaZeniPhase");
      success = success && CTIaziPhase->SetField("AntennaAziPhase");
      
      success = success && CTIzeni->SetAxis("AntennaGainFaktFreq");
      success = success && CTIazi->SetAxis("AntennaGainFaktFreq");
      success = success && CTIzeniPhase->SetAxis("AntennaGainFaktFreq");
      success = success && CTIaziPhase->SetAxis("AntennaGainFaktFreq");
      
      success = success && CTIzeni->SetAxis("AntennaGainFaktAz");
      success = success && CTIazi->SetAxis("AntennaGainFaktAz");
      success = success && CTIzeniPhase->SetAxis("AntennaGainFaktAz");
      success = success && CTIaziPhase->SetAxis("AntennaGainFaktAz");
      
      success = success && CTIzeni->SetAxis("AntennaGainFaktEl");
      success = success && CTIazi->SetAxis("AntennaGainFaktEl");
      success = success && CTIzeniPhase->SetAxis("AntennaGainFaktEl");
      success = success && CTIaziPhase->SetAxis("AntennaGainFaktEl");
      
      success = success && CTIzeni->SetAxisValue(1,frequency_p);
      success = success && CTIazi->SetAxisValue(1,frequency_p);
      success = success && CTIzeniPhase->SetAxisValue(1,frequency_p);
      success = success && CTIaziPhase->SetAxisValue(1,frequency_p);
      
      success = success && CTIzeni->SetAxisValue(2,Vector<Double>(1,azimuth));
      success = success && CTIazi->SetAxisValue(2,Vector<Double>(1,azimuth));
      success = success && CTIzeniPhase->SetAxisValue(2,Vector<Double>(1,azimuth));
      success = success && CTIaziPhase->SetAxisValue(2,Vector<Double>(1,azimuth));
      
      success = success && CTIzeni->SetAxisValue(3,Vector<Double>(1,elevation));
      success = success && CTIazi->SetAxisValue(3,Vector<Double>(1,elevation));
      success = success && CTIzeniPhase->SetAxisValue(3,Vector<Double>(1,elevation));
      success = success && CTIaziPhase->SetAxisValue(3,Vector<Double>(1,elevation));
      
    } catch (AipsError x) {
      cerr << "Simulation2fftMatrix::initCTinterpolater: " << x.getMesg() << endl;
      return False;
    }; 
    return success;
  };
  
  Vector<DComplex> Simulation2fftMatrix::combinePolarizations(Int AntID,
							      Vector<DComplex> &FFTazi, 
							      Vector<DComplex> &FFTzeni)
  {
    Bool success = True;
    try {
      Vector<DComplex> fftvals,tmpCvec;
      Vector<Double> tmpvec;
      DComplex tmpcomp(0.,1.);
      Array<Double> gainZeni,gainAzi,phaseZeni,phaseAzi;
      Vector<DComplex> gainZeniC(fftlen_p),gainAziC(fftlen_p),phaseZeniC(fftlen_p),phaseAziC(fftlen_p);
      success = success && CTIazi->GetValues(date_p, AntID, &gainAzi);
      success = success && CTIzeni->GetValues(date_p, AntID, &gainZeni);
      success = success && CTIaziPhase->GetValues(date_p, AntID, &phaseAzi);
      success = success && CTIzeniPhase->GetValues(date_p, AntID, &phaseZeni);

      convertArray(gainAziC,Vector<Double>(gainAzi));
      convertArray(gainZeniC,Vector<Double>(gainZeni));
      convertArray(phaseAziC,Vector<Double>(phaseAzi));
      convertArray(phaseZeniC,Vector<Double>(phaseZeni));
 
      if (success) {
	fftvals = FFTazi/gainAziC*exp(phaseAziC*tmpcomp)+FFTzeni/gainZeniC*exp(phaseZeniC*tmpcomp);
	// Finish the Antenna calibration with a number of "constants"
	// tmpvec = sqrt(c/(4*pi*mu0))/freq
	tmpvec = 4.35713e+06/frequency_p;
	tmpvec(0) = tmpvec(1);
	tmpCvec.resize(fftlen_p);
	convertArray(tmpCvec,tmpvec);
        fftvals *= tmpCvec;
	return fftvals;
      }      
    } catch (AipsError x) {
      cerr << "Simulation2fftMatrix::combinePolarizations: " << x.getMesg() << endl;
      return Vector<DComplex>();
    }; 
    return Vector<DComplex>();
  };
  
  
  Bool Simulation2fftMatrix::CalcFreqeuncies(Double StepSize)
  {
    try {
      Double samplingRate_p=1/StepSize;
      Double freqResolution (samplingRate_p/(2*(fftlen_p-1)));
      
      frequency_p.resize(fftlen_p);
      for (Int channel=0; channel<fftlen_p; channel++) {
	frequency_p(channel) = channel*freqResolution;
      };
    } catch (AipsError x) {
      cerr << "Simulation2fftMatrix::CalcFreqeuncies: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };
  
}  //  Namespace CR -- end
