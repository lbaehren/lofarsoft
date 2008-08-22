/*-------------------------------------------------------------------------*
 | $Id:: VectorNorms.h 1862 2008-08-08 10:20:52Z baehren                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

#include <fstream>

#include <Math/BasicFilter.h>
#include <Math/HanningFilter.h>
#include <IO/DataReader.h>
#include <Data/ITS_Capture.h>

using casa::Float;
using casa::Double;
using casa::Complex;
using casa::Vector;

/*!
  \file tHanningFilter.cc
  
  \ingroup CR_Math

  \brief A collection of test routines for the HanningFilter class
 
  \author Lars B&auml;hren
 
  \date 2005/11/16
*/

// template <class T> void exportWeights (string const &filename,
// 				       Vector<T> const &weights);
int test_HanningFilter ();
int test_alpha ();
int test_beta ();
int test_operators ();

// ------------------------------------------------------------- show_HanningFilter

/*!
  \brief Show the parameters of the Hanning filter

  \param h -- HanningFilter object to be displayed
 */
template <class T>
void show_HanningFilter (HanningFilter<T> &h)
{
  cout << " - blocksize = " << h.blocksize() << endl;
  cout << " - alpha     = " << h.alpha()     << endl;
  cout << " - beta      = " << h.beta()      << endl;
  cout << " - beta_rise = " << h.betaRise()  << endl;
  cout << " - beta_fall = " << h.betaFall()  << endl;
}

//! Show the parameters of the Hanning filter
template void show_HanningFilter (HanningFilter<Float> &h);
//! Show the parameters of the Hanning filter
template void show_HanningFilter (HanningFilter<Double> &h);
//! Show the parameters of the Hanning filter
template void show_HanningFilter (HanningFilter<Complex> &h);

// ------------------------------------------------------------- test_HanningFilter

/*!
  \brief Test constructors for a new HanningFilter object

  Test the various constructors for a new HanningFilter object; as the class
  is templated we therefore should check all type for which an object can
  be created.

  \return nofFailedTests -- The number of failed tests.
*/
int test_HanningFilter ()
{
  cout << "\n[test_HanningFilter]\n" << endl;

  int nofFailedTests (0);
  //
  unsigned int channels = 1024;
  Float alpha_float     = 0.7;
  Double alpha_double   = 0.7;
  Complex alpha_complex = 0.7;

  // default constructor
  cout << "[1] HanningFilter<T> ()" << endl;
  try {
    cout << " - float" << endl;
    HanningFilter<Float> filter_float();
    cout << " - double" << endl;
    HanningFilter<Double> filter_double();
    cout << " - complex" << endl;
    HanningFilter<Complex> filter_complex();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  // simplest constructor
  cout << "[2] HanningFilter<T> (const unsigned int& channels)" << endl;
  try {
    cout << " - float" << endl;
    HanningFilter<Float> filter_float(channels);
    cout << " - double" << endl;
    HanningFilter<Double> filter_double(channels);
    cout << " - complex" << endl;
    HanningFilter<Complex> filter_complex(channels);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  // Construction using predefined weights
  cout << "[3] HanningFilter<T> (const unsigned int& channels,const T& alpha)" << endl;
  try {
    cout << " - float" << endl;
    HanningFilter<Float> filter_float(channels,
				      alpha_float);
    cout << " - double" << endl;
    HanningFilter<Double> filter_double(channels,
					alpha_double);
    cout << " - complex" << endl;
    HanningFilter<Complex> filter_complex(channels,
					  alpha_complex);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
    
  // Construction from other HanningFilter object
  cout << "[4] HanningFilter<T> (HanningFilter<T> const &other)" << endl;
  try {
    cout << " - float" << endl;
    HanningFilter<Float> filter_float1 (channels);
    HanningFilter<Float> filter_float2 (filter_float1);
    cout << " - double" << endl;
    HanningFilter<Double> filter_double1 (channels);
    HanningFilter<Double> filter_double2 (filter_double1);
    cout << " - complex" << endl;
    HanningFilter<Complex> filter_complex1 (channels);
    HanningFilter<Complex> filter_complex2 (filter_complex1);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
    
  return nofFailedTests;
}

// ----------------------------------------------------------------- test_alpha

/*!
  \brief Test creation of the hanning window for different slope parameters

  \return nofFailedTests -- The number of failed tests.
*/
int test_alpha ()
{
  cout << "\n[test_alpha]\n" << endl;

  int nofFailedTests (0);
  //
  unsigned int blocksize (1024);
  ofstream outfile;
  
  cout << "[1] float" << endl;
  try {
    outfile.open("tHanningFilter_float.data", ios::out);
    //
    HanningFilter<Float> filter40 (blocksize,0.40);
    HanningFilter<Float> filter50 (blocksize,0.50);
    HanningFilter<Float> filter60 (blocksize,0.60);
    //
    Vector<Float> weights40 (filter40.weights());
    Vector<Float> weights50 (filter50.weights());
    Vector<Float> weights60 (filter60.weights());
    //
    for (unsigned int n(0); n<blocksize; n++) {
      outfile << n
	      << "\t" << weights40(n)
	      << "\t" << weights50(n)
	      << "\t" << weights60(n) << endl;
    }
    //
    outfile.close();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[2] double" << endl;
  try {
    outfile.open("tHanningFilter_double.data", ios::out);
    //
    HanningFilter<Double> filter40 (blocksize,0.40);
    HanningFilter<Double> filter50 (blocksize,0.50);
    HanningFilter<Double> filter60 (blocksize,0.60);
    //
    Vector<Double> weights40 (filter40.weights());
    Vector<Double> weights50 (filter50.weights());
    Vector<Double> weights60 (filter60.weights());
    //
    for (unsigned int n(0); n<blocksize; n++) {
      outfile << n
	      << "\t" << weights40(n)
	      << "\t" << weights50(n)
	      << "\t" << weights60(n) << endl;
    }
    //
    outfile.close();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[2] complex" << endl;
  try {
    outfile.open("tHanningFilter_complex.data", ios::out);
    //
    HanningFilter<Complex> filter40 (blocksize,0.40);
    HanningFilter<Complex> filter50 (blocksize,0.50);
    HanningFilter<Complex> filter60 (blocksize,0.60);
    //
    Vector<Complex> weights40 (filter40.weights());
    Vector<Complex> weights50 (filter50.weights());
    Vector<Complex> weights60 (filter60.weights());
    //
    for (unsigned int n(0); n<blocksize; n++) {
      outfile << n
	      << "\t" << real(weights40(n)) << " " << imag(weights40(n))
	      << "\t" << real(weights50(n)) << " " << imag(weights50(n))
	      << "\t" << real(weights60(n)) << " " << imag(weights60(n))
	      << endl;
    }
    //
    outfile.close();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[3] Filter as function of alpha" << endl;
  {
    HanningFilter<Double> filter (blocksize);
    Vector<Double> weights (blocksize);
    uint channel (0);
    double increment (0.01);

    outfile.open("tHanningFilter_alphaScan.data");

    for (double alpha (0.5); alpha<=1.0; alpha+=increment) {
      filter.setAlpha (alpha);
      weights = filter.weights();
      for (channel=0; channel<blocksize; channel++) {
	outfile << alpha << "\t" << channel << "\t" << weights(channel) << endl;
      }
      outfile << endl;
    }

    outfile.close();
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------ test_beta

/*!
  \brief Test modification of the filter by segmentation
  
  \return nofFailedTests -- The number of failed tests.

  The generated data file can be plotted using gnuplot via the commands
  \verbatim
  set grid 
  set contour base
  set xlabel 'beta/blocksize'
  set ylabel 'sample'
  set zlabel 'filter weight'
  splot 'tHanningFilter_betaWeights.data' w l
  set logscale z
  splot 'tHanningFilter_betaData.data' u 1:2:($3*$3) w l
  \endverbatim
 */
int test_beta ()
{
  cout << "\n[test_beta]\n" << endl;

  int nofFailedTests (0);
  //
  uint blocksize (1024);
  uint stepwidth (blocksize/16);
  float alpha (0.5);
  uint beta (blocksize/2);
  ofstream outfile;

  // -- Test for constructor -------------------------------

  cout << "[1] Test construction with beta provided ..." << endl;
  try {
    HanningFilter<Float> h (blocksize, alpha, beta);
    //
    show_HanningFilter (h);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  // -- Window as function of beta -------------------------
  
  cout << "[2] Test range of beta ..." << endl;
  try {
    HanningFilter<Float> h (blocksize, alpha, beta);
    Vector<Float> weights (blocksize);
    uint n (0);
    
    outfile.open ("tHanningFilter_betaWeights.data", ios::out);
    
    for (beta=stepwidth; beta<blocksize; beta+=stepwidth) {
      h.setBeta (beta);
      weights = h.weights();
      for (n=0; n<blocksize; n++) {
	outfile << beta/float(blocksize) << "\t"
		<< n << "\t"
		<< weights(n) << endl;
      }
      outfile << endl;
    }
    
    outfile.close();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// --------------------------------------------------------------- test_operators

/*!
  \brief Test operation overloading

  \return nofFailedTests -- The number of failed tests.
*/
int test_operators ()
{
  cout << "\n[test_operators]\n" << endl;

  int nofFailedTests (0);
  //
  unsigned int channels (128);
  Float alpha (0.3);

  HanningFilter<Float> filter1 (channels,alpha);
  HanningFilter<Float> filter2 (filter1);

  cout << " - filter 1" << endl;
  cout << " -- alpha   : " << filter1.alpha() << endl;
  cout << " -- weights : " << filter1.weights() << endl;
  cout << " - filter 2 " << endl;
  cout << " -- alpha   : " << filter2.alpha() << endl;
  cout << " -- weights : " << filter2.weights() << endl;

  return nofFailedTests;
}

// -------------------------------------------------------------- test_processing

/*!
  \brief Test processing of data by application of the HanningFilter

  \todo Function currently not included in testing, since error handling for 
  missing input data files does not work yet.

  \return nofFailedTests -- The number of failed tests in this function
*/
int test_processing ()
{
  int nofFailedTests (0);
  bool ok (true);

  string basefile ("/data/ITS/exampledata.its/experiment.meta");
  unsigned int blocksize (16384);
  unsigned int stepwidth (blocksize);
  double alpha (0.5);
  unsigned int beta (blocksize/2);
  ofstream outfile;
  
  cout << "[1] Application to ITS data set ..." << endl;
  try {
    ok = true;
    uint channel (0);
    DataReader *dr;
    ITS_Capture *capture;

    try {
      capture = new ITS_Capture (basefile,
				blocksize);
      dr = capture;
    } catch (std::string message) {
      std::cerr << message << std::endl;
      ok = false;
      nofFailedTests++;
    }

    if (ok) {
      
      Matrix<DComplex> fftRaw (dr->fft());
      Matrix<DComplex> fftFiltered (fftRaw.shape());
      
      // process the data and export the results
      outfile.open("tHanningFilter_dataFilter.data");
      
      for (alpha = 0.5; alpha<=1.0; alpha+=0.01) {
	dr->setHanningFilter(alpha);
	fftFiltered = dr->fft();
	for (channel=0; channel<dr->fftLength(); channel++) {
	  outfile << alpha << "\t"
		  << channel << "\t" 
		  << abs(fftRaw(channel,0))-abs(fftFiltered(channel,0))
		  << endl;
	}
	outfile << endl;
      }
      
      outfile.close();
    }
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Apply parametrized filter to ITS data ..." << endl;
  {
    ok = true;
    blocksize = 16384;
    stepwidth = blocksize/8;
    DataReader *dr;

    try {
      ITS_Capture *capture = new ITS_Capture (basefile,
					    blocksize);
      dr = capture;
    } catch (std::string message) {
      std::cerr << message << std::endl;
      ok = false;
    }
    /*
      The actual test only can be run, if we have been successful opening the
      file with the input data.
    */
    if (ok) {
      uint fftLength (dr->fftLength());
      uint channel (0);
      Matrix<DComplex> fft (fftLength,dr->nofAntennas());
      
      outfile.open ("tHanningFilter_betaData.data", ios::out);

      for (beta=stepwidth; beta<blocksize; beta+=stepwidth) {
	dr->setHanningFilter(alpha,beta);
	fft = dr->fft();
	for (channel=0; channel<fftLength; channel++) {
	  outfile << beta/float(blocksize) << "\t"
		  << channel << "\t"
		  << abs(fft(channel,0))
		  << endl;
	}
	outfile << endl;
      }
      outfile.close();
    }
  }
    
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);
  
  // Test for the constructor(s)
  nofFailedTests += test_HanningFilter ();
  // Test for manipulation of the slope parameter
  nofFailedTests += test_alpha ();
  // Test for manipulation of the segmentation parameters
  nofFailedTests += test_beta ();
  // Test for operators
  nofFailedTests += test_operators ();
  // Test processing of data by application of the HanningFilter
//   nofFailedTests += test_processing();
  
  return nofFailedTests;
}
