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

/* $Id: tsimulation2fftMatrix.cc,v 1.2 2007/03/09 17:59:00 horneff Exp $*/

#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <IO/Simulation2fftMatrix.h>
#include <scimath/Mathematics/FFTServer.h>

using CR::Simulation2fftMatrix;

/*!
  \file tSimulation2fftMatrix.cc

  \ingroup IO

  \brief A collection of test routines for Simulation2fftMatrix
 
  \author Andreas Horneffer
 
  \date 2007/03/08
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new Simulation2fftMatrix object

  \return nofFailedTests -- The number of failed tests.
*/
int test_Simulation2fftMatrix ()
{
  int nofFailedTests (0);
  
  try {
    std::cout << "\n[test_Simulation2fftMatrix]\n" << std::endl;
    
    std::cout << "[1] Testing default constructor ..." << std::endl;
    Simulation2fftMatrix newObject;

    std::cout << "[2] Generating importSimulation object ... " ;
    importSimulation Sim;
    bool ok = Sim.openNewSimulation("30deg1e17_shift",
				    "lopes30",
				    "/home/horneff/testreas");
    std::cout << " ok: " << ok << endl;

    std::cout << "[3] Generating and attaching CalTableReader object ... " << endl;;
    CalTableReader CTread("/home/horneff/lopescasa/data/LOPES/LOPES-CalTable");
    newObject.setCalTable(&CTread);

    std::cout << "[4] Importing simulation ... " ;;     
    ok =  newObject.SimulationImport(&Sim);
    std::cout << " ok: " << ok << endl;

    std::cout << "[5] Generating output file ... " << endl;;

    FILE *allout;
    allout = fopen("tSimulation2fftMatrix_allfft.tab","w");
    
    std::cout << "   ... retrieving the data from the object," << std::endl;
    Vector<Double> Frequencies = newObject.getFrequency();
    Matrix<DComplex> FFT = newObject.getfft();
    Matrix<Double> absFFT = amplitude(FFT);
    Matrix<Double> phaseFFT = phase(FFT);
    Int fftlen = absFFT.nrow();
    Int nants = absFFT.ncolumn();
    Int i,j;
    std::cout << "   ... and dumping the data to file." << std::endl;
    for (i=0; i< fftlen; i++) {
      fprintf(allout,"\n %f ",Frequencies(i));
      for (j=0; j<nants; j++) {
	fprintf(allout,"\t %f ",absFFT(i,j));
	// fprintf(allout,"\t %f %f ",absFFT(i,j),phaseFFT(i,j)-phaseFFT(i,0));
      };
    };
    fprintf(allout,"\n");
    fclose(allout);
    Int blocklen_p = newObject.getBlocklen();
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

    allout = fopen("tSimulation2fftMatrix_pulse.tab","w");
    for (i=0; i< blocklen_p; i++) {
      fprintf(allout,"\n %f ",i);
      for (j=0; j<nants; j++) {
	fprintf(allout,"\t %f ",absFFT(i,j));
      };
    };
    fprintf(allout,"\n");
    fclose(allout);


  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
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
    nofFailedTests += test_Simulation2fftMatrix ();
  }

  return nofFailedTests;
}
