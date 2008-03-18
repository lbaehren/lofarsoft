/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

/*!
  \file tUseROOT.cc
  
  \ingroup CR_test

  \brief A number of tests working with ROOT

  \author Lars B&auml;hren

  To run everything in a chain do:
  \code
  make tUseROOT && ./CR-Pipeline/test/tUseROOT && root timeseries.C
  \endcode

  In order to read the data back in and display them:
  \verbatim
  TFile f1 ("demo.root")
  TFile f2 ("lopesevent.root")
  TFile f3 ("lopeseventGraph.root")
  TBrowser browser
  \endverbatim
*/

// --- Standard C++ library --------------------------------
#include <fstream>
#include <iostream>
#include <string>
// -- ROOT header files ------------------------------------
#include <TCanvas.h>
#include <TH1F.h>
#include <TFile.h>
#include <TGraph.h>
#include <TMatrixF.h>
#include <TMatrixD.h>
#include <TMath.h>
#include <TRandom.h>
#include <TObjArray.h>
#include <TSpectrum.h>
#include <TVirtualFFT.h>
// -- casacore header files --------------------------------
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Matrix.h>
// --- CR-Pipeline header files ----------------------------
#include <Utilities/ProgressBar.h>
// --- DAL header files ------------------------------------
#include <dal/dalLopesEvent.h>

using std::cerr;
using std::cout;
using std::endl;

// ------------------------------------------------------------------------------

/*!
  \brief Test classes and operations contained in TMatrix

  \return nofFailedTests -- The number of failed tests within this function
*/
int test_TMatrix () 
{
  cout << "\n[test_TMatrix]\n" << endl;

  int nofFailedTests (0);

  cout << "[1] Testing TMatrixF ..." << endl;
  try {
    TMatrixF b(2,3);
    TMatrixF a(4,4);
    b.ResizeTo(a);
    b = a;
  } catch (std::string message) {
    cerr << "[test_TMatrix] " << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Testing TMatrixD ..." << endl;
  try {
    TMatrixD b(2,3);
    TMatrixD a(4,4);
    //
    b.ResizeTo(a);
    b = a;
    //
    a.Invert();
  } catch (std::string message) {
    cerr << "[test_TMatrix] " << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test classes and operations contained in TMath

  \return nofFailedTests -- The number of failed tests within this function
*/
int test_TMath () 
{
  cout << "\n[test_TMath]\n" << endl;

  int nofFailedTests (0);

  cout << "[1] Round to nearest integer ..." << endl;
  try {
    Int_t intValue;
    Float_t floatValue (1.4);
    Double_t doubleValue (1.6);
    
    intValue = TMath::Nint (floatValue);
    intValue = TMath::Nint (doubleValue);
  } catch (std::string message) {
    cerr << "[test_TMath] " << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Special functions ..." << endl;
  try {
    Double_t x (0.5);
    //
    cout << "-- DiLog(x)      = " << TMath::DiLog(x) << endl;
    cout << "-- Erf(x)        = " << TMath::Erf(x) << endl;
    cout << "-- Erfc(x)       = " << TMath::Erfc(x) << endl;
    cout << "-- ErfInverse(x) = " << TMath::ErfInverse(x) << endl;
  } catch (std::string message) {
    cerr << "[test_TMath] " << message << endl;
    nofFailedTests++;
  }  

  cout << "[3] Array operations ..." << endl;
  try {
    Long64_t nelem (20);
    Double_t *array;
    // adjust array size
    array = new Double_t [nelem];
    // fill the array with some values
    for (Long64_t n(0); n<nelem; n++) {
      array[n] = 0.5*n;
    }
    // do something with the array
    cout << "-- nof. array elements = " << nelem << endl;
//     cout << "-- min. array value    = " << TMath::MinElement(nelem,array) << endl;
//     cout << "-- max. array value    = " << TMath::MaxElement(nelem,array) << endl;
    // release allocated memory
    delete [] array;
  } catch (std::string message) {
    cerr << "[test_TMath] " << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Create histograms from antenna data stored in a LopesEvent file

  \param infile -- Input file from which to read the data
*/
int test_lopesevent2hist (std::string const &infile)
{
  int nofFailedTests (0);
  int antenna (0);
  int sample (0);
  
  cout << "-- Opening file " << infile << " ..." << endl;
  dalLopesEvent le (infile);

  int nofAntennas (le.nofAntennas());
  int blocksize (le.blocksize());

  cout << "-- Retrieve antenna data from file ..." << endl;
  casa::Matrix<short> data = le.channeldata();
  
  /*
    Fill the data from the file into a set of histogram, which then can be fitted
  */
  try {
    short dataMin = min (data);
    short dataMax = max (data);
    char name  [10];
    char title [20];
    TObjArray HList (0);
    TH1F *h;
    float tmp;
    
    cout << "-- Filling antenna data into histograms ..." << endl;
    for (antenna=0; antenna<nofAntennas; antenna++) {
      // set histogram name and title
      sprintf (name,"ant%d",antenna);
      sprintf (title,"Antenna %d",antenna);
      // Create new histogram and add it to list
      h = new TH1F (name,title,100,dataMin,dataMax);
      HList.Add (h);
      // fill the data into the histogram
      for (sample=0; sample<blocksize; sample++) {
	tmp = 1.0*data(sample,antenna);
 	h->Fill(tmp);
      }
    }

    cout << "-- Writing histograms to ROOT file..." << endl;
    TFile f ("lopesevent.root","recreate");
    HList.Write();
    f.Map();
    f.Close();
  } catch (std::string err) {
    cerr << err << endl;
    nofFailedTests++;
  }
  
  /*
    Take the data from the file and fill them into a graph, which can be used to
    display the original time-series data for the individual antennas
  */
  try {
    double timeaxis [blocksize];
    double channeldata [blocksize];
    double sampleinterval (1/le.samplerate());
    TObjArray GList (0);
    TGraph *g;

    // generate values along the time-axis
    for (int n(0); n<blocksize; n++) {
      timeaxis[n] = n*sampleinterval;
    }

    cout << "-- Filling antenna data into graphs per data channel..." << endl;
    for (antenna=0; antenna<nofAntennas; antenna++) {
      // extract the channel data
      for (int n(0); n<blocksize; n++) {
	channeldata[n] = 1.0*data(sample,antenna);
      }
      // create a new graph
      g = new TGraph (blocksize,timeaxis,channeldata);
      GList.Add (g);
    }


    cout << "-- Writing graphs to ROOT file..." << endl;
    TFile f ("lopeseventGraph.root","recreate");
    GList.Write();
    f.Map();
    f.Close();
  } catch (std::string err) {
    cerr << err << endl;
    nofFailedTests++;
  }
  

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test exporting histograms to a file
*/
int test_histogram2file ()
{
  int nofFailedTests (0);
  int nofHistograms (20);
  int nofHistogramPoints (2000);
  char name [10];
  char title [20];

  try {
    TObjArray HList (0);
    TH1F *h;
    
    cout << "-- Filling histograms ..." << endl;
    for (int numHist (0); numHist<nofHistograms; numHist++) {
      // set histogram name and title
      sprintf (name,"hist%d",numHist);
      sprintf (title, "Hist. nr.: %d" , numHist);
      // Create new histogram and add it to list
      h = new TH1F (name,title,100,-4,4);
      HList.Add (h);
      // Fill histogram with random numbers
      h->FillRandom ("gaus",nofHistogramPoints);
    }
    
    TFile f ("demo.root","recreate");
    HList.Write();
    f.Map();
    f.Close();
  } catch (std::string err) {
    cerr << err << endl;
    nofFailedTests++;
  }
    
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test some minimal processing of the time-series data from a LOPES event

  <ol>
    <li>Read in LopesEvent data via the DAL.
    <li>Extract the antenna time-series data
    <li>Filling data into histograms
    <li>Fitting of the histograms by Gausssian distribution
  </ol>

  The resuls will be written to a ROOT file; in order to inspect the contents:
  \code
  root timeseries.C
  \endcode

  \param infile -- Input file from which to read the data

  \return nofFailedTests -- The number of failed test in this function
*/
int test_processing (std::string const &infile)
{
  int nofFailedTests (0);
  int channel (0);

  cout << "-- Open data file..." << endl;

  dalLopesEvent le (infile);

  cout << "-- Retrieve antenna data from file ..." << endl;

  casa::Matrix<short> data = le.channeldata();
  int nofAntennas (le.nofAntennas());
  int blocksize (le.blocksize());
  int fftLength (blocksize/2+1);
  float minValue (min(data));
  float maxValue (max(data));
  float channelPower (0.0);
  float stationSpectrum [fftLength];

  cout << "--> Shape of data array = " << data.shape() << endl;
  cout << "--> nof. antennas       = " << nofAntennas  << endl;
  cout << "--> samples per antenna = " << blocksize    << endl;
  cout << "--> FFT output length   = " << fftLength    << endl;
  
  cout << "-- Filling data into histograms..." << endl;

  char nameTimeSeries [10];
  char nameSpectrum [10];
  char nameHistogram [10];
  char nameCanvas [10];
  TH1F *ts;
  TH1 *fft = 0;
  TH1F *hist;
  TH1F *spec;
  TCanvas *c;
  TObjArray HList (0);
  
  for (channel=0; channel<fftLength; channel++) {
  	stationSpectrum[channel] = 0.0;
  }

  for (int antenna(0); antenna<nofAntennas; antenna++) {
    // Update names and labels
    sprintf (nameTimeSeries,"Ant%d",antenna);
    sprintf (nameSpectrum,"Spec%d",antenna);
    sprintf (nameHistogram,"Hist%d",antenna);
    sprintf (nameCanvas,"Canv%d",antenna);
    //
    cout << "--> Processing Antenna " << antenna << endl;
    // recreate histograms
    ts   = new TH1F (nameTimeSeries,"Time series",blocksize,0.,blocksize);
    hist = new TH1F (nameHistogram,"Time-series distribution",100,minValue,maxValue);
    spec = new TH1F (nameSpectrum,"Spectrum",fftLength,0.,fftLength);
    c    = new TCanvas (nameCanvas,"Antenna data",1000,800);
    // add histograms to object list
    HList.Add(ts);
    HList.Add(hist);
    HList.Add(spec);
    HList.Add(c);
    // fill in the data for this antenna
    for (int sample(0); sample<blocksize; sample++) {
      ts->SetBinContent(sample,float(data(sample,antenna)));
      hist->Fill(float(data(sample,antenna)));
    }
    // fit the histogram by a Gaussian distribution
    hist->SetFillColor(32);
//    hist->Fit("gaus");
    // generate the fft
    fft = ts->FFT(fft,"MAG");
    for (channel=0; channel<fftLength; channel++) {
      channelPower = fft->GetBinContent(channel)*fft->GetBinContent(channel);
      spec->SetBinContent(channel,channelPower);
      stationSpectrum[channel] += channelPower;
    }
    
    // Collect the various plots in a canvas to display them along-side
    c->Divide(1,3);
        
    c->cd(1);
    ts->Draw();
    c->cd(2);
    hist->Draw();
    c->cd(3);
    spec->Draw();
    c->Update();
  }

  cout << "-- Joint power spectrum for all antennas in the station..." << endl;
    
  TH1F *specStation;
  specStation = new TH1F ("specStation","Station Spectrum",fftLength,0.,fftLength);
  HList.Add(specStation);
    
  for (channel=0; channel<fftLength; channel++) {
    specStation->SetBinContent(channel,stationSpectrum[channel]);
  }

  cout << "-- Exporting data to ROOT file..." << endl;

  TFile f ("timeseries.root","recreate");
  HList.Write();
  f.Close();

  cout << "-- Creating ROOT script to display the data..." << endl;

  std::ofstream outfile;
  outfile.open("timeseries.C");
  
  outfile << "{"                               << endl;
  outfile << " TFile f (\"timeseries.root\");" << endl;
  outfile << " TBrowser browser;"              << endl;
  outfile << "}"                               << endl;

  outfile.close();


  return nofFailedTests;
}

// ------------------------------------------------------------------------------

int main (int argc,
	  char *argv[])
{
  int nofFailedTests (0);
  bool haveData (false);
 
  if (argc < 2) {
    cerr << "[tUseROOT] Missing name of input data file!" << std::endl;
    cerr << "" << std::endl;    
    cerr << "\ttUseROOT <eventfile>" << std::endl;    
    cerr << "" << std::endl;    
    cerr << "Omitting tests working on external data." << std::endl;    
    cerr << "" << std::endl;    
    //
    haveData = false;
  }
  
  /* Basic tests for working with ROOT classes */
  
  nofFailedTests += test_TMatrix ();
  nofFailedTests += test_TMath ();

  /* Operational tests working on external data */
  
  if (haveData) {
    // get filename from argument list
    std::string filename = argv[1];
    // run the tests
    nofFailedTests += test_histogram2file();
    nofFailedTests += test_lopesevent2hist (filename);
    nofFailedTests += test_processing (filename);
  }
  
  cout << nofFailedTests << " failed tests." << endl;
  
  return nofFailedTests;
}
