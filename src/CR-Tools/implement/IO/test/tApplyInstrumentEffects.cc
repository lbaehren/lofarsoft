/*-------------------------------------------------------------------------*
 | $Id                                                                     |
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
#include <scimath/Mathematics/FFTServer.h>

using CR::CalTableReader;
using CR::ImportSimulation;
using CR::Simulation2fftMatrix;
using CR::ApplyInstrumentEffects;  // Namespace usage

/*!
  \file tApplyInstrumentEffects.cc

  \ingroup IO

  \brief A collection of test routines for the ApplyInstrumentEffects class
 
  \author Andreas Horneffer
 
  \date 2007/09/21
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new ApplyInstrumentEffects object

  \return nofFailedTests -- The number of failed tests.
*/
int test_ApplyInstrumentEffects ()
{
  int nofFailedTests (0);
  
  try {
    
    
    std::cout << "\n[test_ApplyInstrumentEffects]\n" << std::endl;
    
    std::cout << "[1] Testing default constructor ..." << std::endl;
    ApplyInstrumentEffects newApplyInstrumentEffects;
    newApplyInstrumentEffects.summary(); 
    
    std::cout << "[2] Opening Simulation ... " ;
    Simulation2fftMatrix sim2fft;
    ImportSimulation Sim;
    bool ok = Sim.openNewSimulation("30deg1e17_shift",
				    "lopes30",
				    "/home/horneff/testreas");
    std::cout << " ok: " << ok << endl;

    std::cout << "[3] Generating and attaching CalTableReader object ... " << endl;;
    CalTableReader CTread("/home/horneff/lopescasa/data/LOPES/LOPES-CalTable");
    sim2fft.setCalTable(&CTread);
    newApplyInstrumentEffects.setCalTable(&CTread);
    sim2fft.setObsDate(1104580800);
    newApplyInstrumentEffects.setObsDate(1104580800);


    std::cout << "[4] Importing simulation ... " ;;     
    ok =  sim2fft.SimulationImport(&Sim);
    std::cout << " ok: " << ok << endl;

    std::cout << "[5] Applying Instrument Effects ... " ;;   
    newApplyInstrumentEffects.setAntennaIDs(sim2fft.getAntIDs());
    newApplyInstrumentEffects.setFreqAxis(sim2fft.getFrequency());
    Matrix<DComplex> FFT;
    FFT = newApplyInstrumentEffects.ApplyEffects(sim2fft.getfft());
    

    std::cout << "[5] Generating output file ... " << endl;;

    FILE *allout;
    allout = fopen("tApplyInstrumentEffects_allfft.tab","w");
    
    std::cout << "   ... retrieving the data from the object," << std::endl;
    Vector<Double> Frequencies = sim2fft.getFrequency();
    Matrix<Double> absFFT = amplitude(FFT);
    Matrix<Double> phaseFFT = phase(FFT);
    Int fftlen = absFFT.nrow();
    Int nants = absFFT.ncolumn();
    Int i,j;
    std::cout << "   ... and dumping the data to file." << std::endl;
    for (i=0; i< fftlen; i++) {
      fprintf(allout,"\n %f ",Frequencies(i));
      for (j=0; j<nants; j++) {
	fprintf(allout,"\t %e %f ",absFFT(i,j),phaseFFT(i,j));
	// fprintf(allout,"\t %f %f ",absFFT(i,j),phaseFFT(i,j)-phaseFFT(i,0));
      };
    };
    fprintf(allout,"\n");
    fclose(allout);
    Int blocklen_p = sim2fft.getBlocklen();
    absFFT.resize(blocklen_p,nants);
    Vector<DComplex> in;
    Vector<Double> out;

    FFTServer<Double,DComplex> server(IPosition(1,blocklen_p),
				      FFTEnums::REALTOCOMPLEX);

    for (j=0; j<nants; j++) {
      in = FFT.column(j);
      out.resize(blocklen_p);
      server.fft(out,in);
      absFFT.column(j) = out;
    };

    allout = fopen("tApplyInstrumentEffects_pulse.tab","w");
    for (i=0; i< blocklen_p; i++) {
      fprintf(allout,"\n %i ",i);
      for (j=0; j<nants; j++) {
	fprintf(allout,"\t %e ",absFFT(i,j));
      };
    };
    fprintf(allout,"\n");
    fclose(allout);



  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_ApplyInstrumentEffects ();
  }

  return nofFailedTests;
}
