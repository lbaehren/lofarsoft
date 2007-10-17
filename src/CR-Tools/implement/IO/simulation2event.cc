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

#include <IO/simulation2event.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  simulation2event::simulation2event (){
    CTRead = NULL;
  };
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  simulation2event::~simulation2event ()
  {
    destroy();
  }
  
  void simulation2event::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void simulation2event::summary (std::ostream &os)
  {;}
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  
  Bool simulation2event::generateEvent(String outfile, String const SimulationName,
				       String const AntListName, String const path, 
				       uInt date, int presync, Bool AddTV, Bool AddNoise){
    Bool success=True;
    try{
      if (CTRead == NULL){
	cerr << "simulation2event::generateEvent: " << "CalTableReader not initialized!" << endl;
	return False;
      };
      
      ImportSimulation Sim;
      Simulation2fftMatrix sim2fft;
      ApplyInstrumentEffects ApplyInstEff;
      toLOPESgrid toGrid;
      TSmatrix2Event TS2Event;
      Vector<int> AntIDs;
      Matrix<DComplex> FFT;
      int i,nAnt;
     
      success = success && Sim.openNewSimulation(SimulationName, AntListName, path);
      if (!success){
	cerr << "simulation2event::generateEvent: " << "Error in Sim.openNewSimulation()!" << endl;
	return False;
       };

      sim2fft.setCalTable(CTRead);
      ApplyInstEff.setCalTable(CTRead);
      sim2fft.setObsDate(date);
      ApplyInstEff.setObsDate(date);
      success = success && sim2fft.SimulationImport(&Sim);
      if (!success){
	cerr << "simulation2event::generateEvent: " << "Error in sim2fft.SimulationImport()!" << endl;
	return False;
       };

      AntIDs = sim2fft.getAntIDs();
      nAnt = AntIDs.nelements();
      ApplyInstEff.setAntennaIDs(AntIDs);
      ApplyInstEff.setFreqAxis(sim2fft.getFrequency());
      FFT = ApplyInstEff.ApplyEffects(sim2fft.getfft());
      if ( FFT.nelements() <=  nAnt){
	cerr << "simulation2event::generateEvent: " << "Error in ApplyInstEff.ApplyEffects()!" << endl;
	return False;
      };

      success = success && toGrid.AddFFTData(FFT,  sim2fft.getBlocklen(), sim2fft.getFrequency());
      if (!success){
	cerr << "simulation2event::generateEvent: " << "Error in toGrid.AddFFTData()!" << endl;
	return False;
      };

      if (AddTV || AddNoise){
	int blocksize=(131072*4),fftsize=blocksize/2+1;
	Vector<Double> frequency(fftsize);
	for (i=0; i<fftsize; i++){
	  frequency(i) = i*80e6/(fftsize-1.)*4.;
	};
	if (AddTV){
	  genTVSpec gTVspec;
	  gTVspec.setCalTable(CTRead);
	  gTVspec.setAntennaIDs(AntIDs);
	  gTVspec.setFreqAxis(frequency);
	  success = success && toGrid.AddFFTData(gTVspec.GetTVSpectrum(date,1e3), blocksize, frequency );
	  if (!success){
	    cerr << "simulation2event::generateEvent: " << "Error while generating TV-Spectrum!" << endl;
	    return False;
	  };
	};
	if (AddNoise){
	  FalckeGorhamGalacticNoise noiseGen;
	  Vector<DComplex> tmpnoise(fftsize);
	  ApplyInstrumentEffects ApplyInstEff2Noise;
	  Matrix<DComplex> nFFT,inFFT;
	  nFFT.resize(fftsize,nAnt); inFFT.resize(fftsize,nAnt);
	  for (i=0; i<nAnt; i++){
	    noiseGen.getNoise(frequency, tmpnoise);
	    nFFT.column(i) = tmpnoise;
	  };
	  ApplyInstEff2Noise.setCalTable(CTRead);
	  ApplyInstEff2Noise.setObsDate(date);
	  ApplyInstEff2Noise.setAntennaIDs(AntIDs);
	  ApplyInstEff2Noise.setFreqAxis(frequency);
	  inFFT = ApplyInstEff2Noise.ApplyEffects(nFFT);
	  inFFT *= DComplex(1e-2);
	  success = success && toGrid.AddFFTData(inFFT, blocksize, frequency );
	  if (!success){
	    cerr << "simulation2event::generateEvent: " << "Error while generating noise!" << endl;
	    return False;
	  };
	};
      }
           
      success = success && TS2Event.SetData(toGrid.GetData(), AntIDs, presync);
      success = success && TS2Event.SetDate(date);
      success = success && TS2Event.WriteEvent(outfile);
      if (!success){
	cerr << "simulation2event::generateEvent: " << "Error while generating the event-file!" << endl;
	return False;
      };
      

    } catch (AipsError x) {
      cerr << "simulation2event::generateEvent: " << x.getMesg() << endl;
      return False;
    }; 
    return success;        
  };

  

} // Namespace CR -- end
