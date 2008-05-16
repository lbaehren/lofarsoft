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
#include <LopesStar/reconstruction.h>

// Namespace usage
using CR::UpSampledDR;
using CR::LopesEventIn;

/*!
  \file compareUpSamples.cc

  \ingroup Data

  \brief Test the strange behavior of the tUpSampledDR.cc class
 
  \author Andreas Horneffer
 
  \date 2008/05/16
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
    if (! newUpSampledDR.setup(&lev,160e6,False)){
      std::cout << "  Failed to setup UpSampledDR!" << std::endl;
      nofFailedTests++;
      return nofFailedTests;
    };

    std::cout << "[3] Setting new blocksizes ..." << std::endl;
    //lev.setHanningFilter(0.5,16384);
    //newUpSampledDR.setBlocksize(2048);
    //newUpSampledDR.setBlock(64);

    std::cout << "[4] Dumping original data ..." << std::endl;
    int i,TSlen;
    Vector<Double> Times;
    Matrix<Double> FX;
    FILE *allout;

    Times = lev.timeValues();
    TSlen = Times.nelements();
    FX = lev.voltage();
    allout = fopen("compareUpSamples-original.tab","w");
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
    allout = fopen("compareUpSamples-upsampled.tab","w");
    for (i=0; i< TSlen; i++) {
      fprintf(allout,"\n %e ",Times(i)*1e6);
      fprintf(allout," \t %e ",FX(i,0));
    };
    fprintf(allout,"\n");
    fclose(allout);


    std::cout << "[6] Arranging storage, ..." << std::endl;
    Vector<Double> Frequencies;
    Matrix<DComplex> FFT;
    Matrix<Double> absFFT,phaseFFT;
    Int j,fftlen,nants;



    std::cout << "[7] Dumping original data, ..." << std::endl;
    allout = fopen("compareUpSamples-original-fft.tab","w");
    Frequencies = lev.frequencyValues();
    std::cout << "                      ... retrieving the data," << std::endl;
    FFT = lev.fft();
    absFFT = amplitude(FFT);
    phaseFFT = phase(FFT);
    fftlen = absFFT.nrow();
    nants = absFFT.ncolumn();
    std::cout << "                      ... and dumping the FFT data to file." << std::endl;
    for (i=0; i< fftlen; i++) {
      fprintf(allout,"\n %f ",Frequencies(i));
      for (j=0; j<nants; j++) {
	fprintf(allout,"\t %f %f ",absFFT(i,j),phaseFFT(i,j));
      };
    };
    fprintf(allout,"\n");
    fclose(allout);


    std::cout << "[9] Dumping UpSampledDR data, ..." << std::endl;
    Frequencies.resize();FFT.resize();absFFT.resize();phaseFFT.resize();
    allout = fopen("compareUpSamples-upsampled-fft.tab","w");
    Frequencies = newUpSampledDR.frequencyValues();
    std::cout << "                      ... retrieving the data," << std::endl;
    FFT = newUpSampledDR.fft();
    absFFT = amplitude(FFT);
    phaseFFT = phase(FFT);
    fftlen = absFFT.nrow();
    nants = absFFT.ncolumn();
    std::cout << "                      ... and dumping the FFT data to file." << std::endl;
    for (i=0; i< fftlen; i++) {
      fprintf(allout,"\n %f ",Frequencies(i));
      for (j=0; j<nants; j++) {
	fprintf(allout,"\t %f %f ",absFFT(i,j),phaseFFT(i,j));
      };
    };
    fprintf(allout,"\n");
    fclose(allout);


    std::cout << "[10] Perform the LOPES-STAR upsampling, ..." << std::endl;
    // Get the (not yet upsampled) raw data of all antennas
    Matrix<Double> rawData = lev.voltage(); 
    nants = rawData.ncolumn();
    // create upsampling factor by upsampling exponent
    unsigned int upsampled = 2;
    // get length of trace
    unsigned int tracelength = rawData.nrow();
    // allocate memory for original and upsampled traces
    double* originalTrace = new double[tracelength];
    double* upsampledTrace = new double[tracelength * upsampled];
    // Create Matrix for usampled values
    Matrix<Double> upData(tracelength * upsampled, nants, 0);
    // do upsampling for each antenna in the antennaSelection
    std::cout << "Upsampling the raw data by a factor of " << upsampled << " ...\nAntenna:" << flush;
    for (int i = 0; i < 1; i++) {
      std::cout << " " << i+1 << flush;
      // copy the trace into the array
      for (unsigned int j = 0; j < tracelength; j++) {
	originalTrace[j] = rawData.column(i)(j);
      };
      // do upsampling by factor #upsampled (--> NoZeros = upsampled -1)
      // calcutlate Offset:
      double before = originalTrace[0];
      ZeroPaddingFFT(tracelength, originalTrace, upsampled-1, upsampledTrace);
      double offset = before - originalTrace[0];
      // copy upsampled trace into Matrix with antenna traces and subtract offset
      // if no offset correction is wanted
      // remember: ZeroPaddingFFT removes the offset automatically
      for (unsigned int j = 0; j < tracelength*upsampled; j++)
	upData.column(i)(j) = upsampledTrace[j] + offset;
    } 
    std::cout << " ... done" << endl;
    // delete arrays
    delete[] originalTrace;
    delete[] upsampledTrace;

    std::cout << "[11] Dumping STAR-upsampled data ..." << std::endl;
    Times.resize();
    Times = newUpSampledDR.timeValues();
    TSlen = tracelength*upsampled;
    allout = fopen("compareUpSamples-star-upsampled.tab","w");
    for (i=0; i< TSlen; i++) {
      fprintf(allout,"\n %e ",Times(i)*1e6);
      fprintf(allout," \t %e ",upData(i,0));
      fprintf(allout," \t %e ",FX(i,0)-upData(i,0));
    };
    fprintf(allout,"\n");
    fclose(allout);

    std::cout << "[12] Dumping UpSampledDR FFT data, ..." << std::endl;
    Frequencies.resize();FFT.resize();absFFT.resize();phaseFFT.resize();
    allout = fopen("compareUpSamples-star-upsampled-fft.tab","w");
    Frequencies = newUpSampledDR.frequencyValues();
    std::cout << "                      ... FFT-ing the data," << std::endl;
    FFTServer<Double,DComplex> fftserv(IPosition(1,TSlen), FFTEnums::REALTOCOMPLEX);
    Vector<DComplex> FFTvec,FFTvec2;
    Vector<Double> diffdata,absFFTvec,phaseFFTvec,absFFTvec2,phaseFFTvec2;
    CR::HanningFilter<Double> hfilt(TSlen);
    diffdata = FX.column(0) - upData.column(0);
    hfilt.filter(diffdata);
    diffdata = 1.;
    for (i=1; i< TSlen; i++) {
      diffdata(i) = -1;
      i++;
    }
    fftserv.fft(FFTvec,upData.column(0));
    fftserv.fft(FFTvec2,diffdata);
    cout << "   len(Frequencies): " << Frequencies.nelements() 
	 << " len(FFTvec): " << FFTvec.nelements() << endl;
    absFFTvec = amplitude(FFTvec); absFFTvec2 = amplitude(FFTvec2); 
    phaseFFTvec = phase(FFTvec);   phaseFFTvec2 = phase(FFTvec2);
    fftlen = absFFTvec.nelements();
    std::cout << "                      ... and dumping the FFT data to file." << std::endl;
    for (i=0; i< fftlen; i++) {
      fprintf(allout,"\n %f ",Frequencies(i));
      fprintf(allout,"\t %f %f ",absFFTvec(i),phaseFFTvec(i));
      fprintf(allout,"\t %f %f ",absFFTvec2(i),phaseFFTvec2(i));
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
    nofFailedTests += test_UpSampledDR ();
  }

  return nofFailedTests;
}
