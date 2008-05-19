/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
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

#include <Data/UpSampledDR.h>
#include <Data/LopesEventIn.h>

// Namespace usage
using CR::UpSampledDR;
using CR::LopesEventIn;

/*!
  \file tUpSampledDR.cc

  \ingroup Data

  \brief A collection of test routines for the UpSampledDR class
 
  \author Andreas Horneffer
 
  \date 2008/04/23
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new UpSampledDR object

  \return nofFailedTests -- The number of failed tests.
*/
int test_UpSampledDR ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_UpSampledDR]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    UpSampledDR newUpSampledDR;
    //

    LopesEventIn lev;

    std::cout << "[2] Testing attaching a file ..." << std::endl;
    std::cout << "                             ..." << " opening File" << std::endl;
    if (! lev.attachFile("example.event")){
      std::cout << "  Failed to attach file!" << std::endl;
      std::cout << "  (Maybe no file called \"example.event\" in the local directory. Copy one here.)" 
		<< std::endl;
      nofFailedTests++;
      return nofFailedTests;
    };
    std::cout << "                             ..." << " attaching File" << std::endl;
    if (! newUpSampledDR.setup(&lev,320e6,False)){
      std::cout << "  Failed to setup UpSampledDR!" << std::endl;
      nofFailedTests++;
      return nofFailedTests;
    };

    std::cout << "[3] Setting new blocksizes ..." << std::endl;
    lev.setHanningFilter(0.5,16384);
    newUpSampledDR.setBlocksize(2048);
    newUpSampledDR.setBlock(64);

    std::cout << "[4] Dumping original data ..." << std::endl;
    int i,TSlen;
    Vector<Double> Times;
    Matrix<Double> FX;
    FILE *allout;

    Times = lev.timeValues();
    TSlen = Times.nelements();
    FX = lev.voltage();
    allout = fopen("tUpSampledDR-original.tab","w");
    for (i=0; i< TSlen; i++) {
      fprintf(allout,"\n %e ",Times(i)*1e6);
      fprintf(allout," \t %e ",FX(i,0));
    };
    fprintf(allout,"\n");
    fclose(allout);

    std::cout << "[5] Dumping upsampled data ..." << std::endl;
    Times.resize();
    FX.resize();
    Times = newUpSampledDR.timeValues();
    TSlen = Times.nelements();
    FX = newUpSampledDR.voltage();
    allout = fopen("tUpSampledDR-upsampled.tab","w");
    for (i=0; i< TSlen; i++) {
      fprintf(allout,"\n %e ",Times(i)*1e6);
      fprintf(allout," \t %e ",FX(i,0));
    };
    fprintf(allout,"\n");
    fclose(allout);




//     std::cout << "[6] Arranging storage, ..." << std::endl;
//     Vector<Double> Frequencies;
//     Matrix<DComplex> FFT;
//     Matrix<Double> absFFT,phaseFFT;
//     Int j,fftlen,nants;

//     allout = fopen("tUpSampledDR-original-fft.tab","w");
//     Frequencies = lev.frequencyValues();
//     std::cout << "                      ... retrieving the data from the pipeline," << std::endl;
//     FFT = lev.fft();
//     absFFT = amplitude(FFT);
//     phaseFFT = phase(FFT);
//     fftlen = absFFT.nrow();
//     nants = absFFT.ncolumn();
//     std::cout << "                      ... and dumping the filtered data to file." << std::endl;
//     for (i=0; i< fftlen; i++) {
//       fprintf(allout,"\n %f ",Frequencies(i));
//       for (j=0; j<nants; j++) {
// 	fprintf(allout,"\t %f %f ",absFFT(i,j),phaseFFT(i,j));
//       };
//     };
//     fprintf(allout,"\n");
//     fclose(allout);


//     Frequencies.resize();FFT.resize();absFFT.resize();phaseFFT.resize();
//     allout = fopen("tUpSampledDR-upsampled-fft.tab","w");
//     Frequencies = newUpSampledDR.frequencyValues();
//     std::cout << "                      ... retrieving the data from the pipeline," << std::endl;
//     FFT = newUpSampledDR.fft();
//     absFFT = amplitude(FFT);
//     phaseFFT = phase(FFT);
//     fftlen = absFFT.nrow();
//     nants = absFFT.ncolumn();
//     std::cout << "                      ... and dumping the filtered data to file." << std::endl;
//     for (i=0; i< fftlen; i++) {
//       fprintf(allout,"\n %f ",Frequencies(i));
//       for (j=0; j<nants; j++) {
// 	fprintf(allout,"\t %f %f ",absFFT(i,j),phaseFFT(i,j));
//       };
//     };
//     fprintf(allout,"\n");
//     fclose(allout);




    newUpSampledDR.summary(); 
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
    nofFailedTests += test_UpSampledDR ();
  }

  return nofFailedTests;
}
