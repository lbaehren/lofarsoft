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

#include <IO/genTVSpec.h>
#include <scimath/Mathematics/FFTServer.h>

using CR::genTVSpec;  // Namespace usage
using namespace CR;

/*!
  \file tgenTVSpec.cc

  \ingroup IO

  \brief A collection of test routines for the genTVSpec class
 
  \author Andreas Horneffer
 
  \date 2007/10/17
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new genTVSpec object

  \return nofFailedTests -- The number of failed tests.
*/
int test_genTVSpec ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_genTVSpec]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    genTVSpec newgenTVSpec;
    //
    newgenTVSpec.summary(); 

    std::cout << "[2] Setting parameters ... \n" ;
    CalTableReader CTread("/home/horneff/lopescasa/data/LOPES/LOPES-CalTable");
    newgenTVSpec.setCalTable(&CTread);
    Vector<Int> AntIDs(3);
    AntIDs(0)=10101;  AntIDs(1)=10102;  AntIDs(2)=10201;    
    newgenTVSpec.setAntennaIDs(AntIDs);

    std::cout << "[3] Generating data ... \n" ;
    Matrix<DComplex> FFT;
    FFT = newgenTVSpec.GetTVSpectrum(1104580800);

    std::cout << "[4] Generating output file ... " << endl;;
    FILE *allout;
    allout = fopen("tgenTVSpec_allfft.tab","w");
    
    Vector<Double> Frequencies(65537);
    for (int i=0; i<65537; i++){ Frequencies(i) = i*80e6/65536; };
    Matrix<Double> absFFT = amplitude(FFT);
    Matrix<Double> phaseFFT = phase(FFT);
    Int fftlen = absFFT.nrow();
    Int nants = absFFT.ncolumn();
    Int i,j;
    for (i=0; i< fftlen; i++) {
      fprintf(allout,"\n %f ",Frequencies(i));
      for (j=0; j<nants; j++) {
	fprintf(allout,"\t %e %f ",absFFT(i,j),phaseFFT(i,j));
      };
    };
    fprintf(allout,"\n");
    fclose(allout);
    Int blocklen_p = 131072;
    absFFT.resize(blocklen_p,nants);
    Vector<DComplex> in;
    Vector<Double> out;

    FFTServer<Double,DComplex> server(IPosition(1,blocklen_p),
				      FFTEnums::REALTOCOMPLEX);

    for (j=0; j<nants; j++) {
      out.resize(blocklen_p);
      server.fft(out, FFT.column(j));
      absFFT.column(j) = out;
    };
    Double timestep,begintime;
    timestep = 1./(max(Frequencies)*2.);
    begintime = -0.5*timestep*blocklen_p; 
    out.resize(blocklen_p);
    for (i=0; i<blocklen_p; i++){
      out(i) = begintime+i*timestep;
    };

    allout = fopen("tgenTVSpec_TS.tab","w");
    for (i=0; i< blocklen_p; i++) {
      fprintf(allout,"\n %e ",out(i));
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
    nofFailedTests += test_genTVSpec ();
  }

  return nofFailedTests;
}
