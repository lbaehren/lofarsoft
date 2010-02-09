/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
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

#include <IO/UpSampledDR.h>
#include <IO/LopesEventIn.h>

// Namespace usage
using CR::UpSampledDR;
using CR::LopesEventIn;
using std::cout;
using std::endl;

/*!
  \file tUpSampledDR.cc

  \ingroup CR
  \ingroup IO

  \brief A collection of test routines for the UpSampledDR class
 
  \author Andreas Horneffer
 
  \date 2008/04/23
*/

//_______________________________________________________________________________
//                                                                           show

/*!
  \param vec -- Vector of which to display the first \e nelem elements
  \param nelem -- Number of elements to display from the vector \e vec.
*/
template <class T> void show (std::vector<T> const &vec,
			      unsigned int nelem=4)
{
  cout << "\t[ ";
  
  for (unsigned int n(0); n<nelem; ++n) {
    cout << vec[n] << ", ";
  }

  cout << ".. ]" << endl;
}

//_______________________________________________________________________________
//                                                               test_UpSampledDR

/*!
  \brief Test constructors for a new UpSampledDR object

  \param filename -- Example LOPES event file to use for the test

  \return nofFailedTests -- The number of failed tests.
*/
int test_UpSampledDR (std::string const &filename)
{
  int nofFailedTests (0);
  
  std::cout << "\n[tUpSampledDR::test_UpSampledDR]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    LopesEventIn lev;
    UpSampledDR newUpSampledDR;

    std::cout << "[2] Testing attaching a file ..." << std::endl;
    std::cout << "                             ..." << " opening File" << std::endl;
    if (! lev.attachFile(filename)){
      std::cout << "  Failed to attach file!" << std::endl;
      std::cout << "  (Maybe no file called \"" << filename
		<< "\" in the local directory. Copy one here.)" 
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

    lev.timeValues(Times);
    TSlen  = Times.nelements();
    FX     = lev.voltage();
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
    newUpSampledDR.timeValues(Times);
    TSlen  = Times.nelements();
    FX     = newUpSampledDR.voltage();
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

//_______________________________________________________________________________
//                                                              test_sampleValues

/*!
  \brief Test retrieval of the values along the sample axis

  \param filename -- Example LOPES event file to use for the test

  \return nofFailedTests -- The number of failed tests.
*/
int test_sampleValues (std::string const &filename)
{
  cout << "\n[tUpSampledDR::test_sampleValues]\n" << endl;

  int nofFailedTests (0);
  uint blocksize (1024);
  uint nyquistZone (1);
  double sampleFrequency (200e06);
  std::vector<uint> samples;
  uint maxOffset (5);

  // create UpSampledDR object to perform the tests with
  UpSampledDR dr;
  dr.setBlocksize(blocksize);
  dr.setNyquistZone (nyquistZone);
  dr.setSampleFrequency (sampleFrequency);
  dr.summary();

  cout << "[1] sampleValues (samples) ..." << endl;
  {
    dr.sampleValues(samples);
    // 
    show(samples);
  }

  cout << "[2] sampleValues (samples,offset) ..." << endl;
  {
    for (uint offset(0); offset<maxOffset; ++offset) {
      dr.sampleValues(samples,offset);
      // 
      show(samples);
    }
  }
  
  cout << "[3] sampleValues (samples,offset,false) ..." << endl;
  {
    for (uint offset(0); offset<maxOffset; ++offset) {
      dr.sampleValues(samples,offset,false);
      // 
      show(samples);
    }
  }

  cout << "[4] setSampleOffset (offset) ..." << endl;
  {
    dr.setSampleOffset(0);
    
    for (uint offset(0); offset<maxOffset; ++offset) {
      // set the offset
      dr.setSampleOffset(offset);
      // get the sample values
      dr.sampleValues(samples);
      // 
      show(samples);
    }
  }
  
  cout << "[5] setSampleOffset (offset,false) ..." << endl;
  {
    dr.setSampleOffset(0);

    for (uint offset(0); offset<maxOffset; ++offset) {
      // set the offset
      dr.setSampleOffset(offset,false);
      // get the sample values
      dr.sampleValues(samples);
      // 
      show(samples);
    }
  }
  
  cout << "[6] nextBlock () ..." << endl;
  {
    dr.setSampleOffset(0);

    for (uint offset(0); offset<maxOffset; ++offset) {
      // get the sample values
      dr.sampleValues(samples);
      // 
      show(samples);
      // increment block counter
      dr.nextBlock();
    }
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                test_timeValues

/*!
  \brief Test retrival of the values along the time axis
  
  \param blocksize -- The number of samples per block of data per antenna.

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
 */
int test_timeValues (std::string const &filename)
{
  cout << "\n[tUpSampledDR::test_timeValues]\n" << endl;

  int nofFailedTests (0);
  uint blocksize (1024);
  uint nyquistZone (1);
  double sampleFrequency (200e06);
  std::vector<double> times;
  uint maxOffset (5);

  // create UpSampledDR object to perform the tests with
  UpSampledDR dr;
  dr.setBlocksize(blocksize);
  dr.setNyquistZone (nyquistZone);
  dr.setSampleFrequency (sampleFrequency);
  dr.summary();
  
  cout << "[1] timeValues (times)" << endl;
  {
    dr.timeValues(times);
    // 
    show(times);
  }

  cout << "[2] timeValues (times,offset)" << endl;
  {
    for (uint offset(0); offset<maxOffset; ++offset) {
      dr.timeValues(times,offset);
      // 
      show(times);
    }
  }
  
  cout << "[3] timeValues (times,offset,false)" << endl;
  {
    for (uint offset(0); offset<maxOffset; ++offset) {
      dr.timeValues(times,offset,false);
      // 
      show(times);
    }
  }

  cout << "[4] setSampleOffset (offset) ..." << endl;
  {
    dr.setSampleOffset(0);

    for (uint offset(0); offset<maxOffset; ++offset) {
      // set the offset
      dr.setSampleOffset(offset);
      // get the sample values
      dr.timeValues(times);
      // 
      show(times);
    }
  }
  
  cout << "[5] setSampleOffset (offset,false) ..." << endl;
  {
    dr.setSampleOffset(0);

    for (uint offset(0); offset<maxOffset; ++offset) {
      // set the offset
      dr.setSampleOffset(offset,false);
      // get the sample values
      dr.timeValues(times);
      // 
      show(times);
    }
  }
  
  cout << "[5] nextBlock () ..." << endl;
  {
    dr.setSampleOffset(0);

    for (uint offset(0); offset<maxOffset; ++offset) {
      // get the sample values
      dr.timeValues(times);
      // 
      show(times);
      // increment block counter
      dr.nextBlock();
    }
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main (int argc,
          char *argv[])
{
  int nofFailedTests (0);
  bool haveDataset (true);
  std::string filename ("example.event");
  
  //________________________________________________________
  // Process parameters from the command line
  
  if (argc < 2) {
    haveDataset = false;
  } else {
    filename    = argv[1];
    haveDataset = true;
  }

  //________________________________________________________
  // Run the tests
  
  // Test for the constructor(s)
  nofFailedTests += test_UpSampledDR (filename);
  //! Test retrieval of the values along the sample axis
  nofFailedTests += test_sampleValues (filename);
  //! Test retrieval of the values along the time axis
  nofFailedTests += test_timeValues (filename);
  
  return nofFailedTests;
}
