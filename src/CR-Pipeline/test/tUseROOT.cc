
/*!
  \file tUseROOT.cc

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
#include <TRandom.h>
#include <TObjArray.h>
#include <TSpectrum.h>
#include <TVirtualFFT.h>
// --- CR-Pipeline header files ----------------------------
#include <Utilities/ProgressBar.h>
// --- DAL header files ------------------------------------
#include <dalLopesEvent.h>
// --- Blitz++ header files --------------------------------
#include <blitz/array.h>

using std::cerr;
using std::cout;
using std::endl;
using blitz::Array;

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
  Array<short,2> data = le.channeldata();
  
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
    <li>Filling data into histgrams
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

  cout << "-- Open data file..." << endl;

  dalLopesEvent le (infile);

  cout << "-- Retrieve antenna data from file ..." << endl;

  Array<short,2> data = le.channeldata();
  int nofAntennas (le.nofAntennas());
  int blocksize (le.blocksize());
  float minValue (min(data));
  float maxValue (max(data));

  cout << "--> nof. antennas       = " << nofAntennas  << endl;
  cout << "--> samples per antenna = " << blocksize    << endl;
  cout << "--> Shape of data array = " << data.shape() << endl;
  
  cout << "-- Filling data into histograms..." << endl;

  char nameTimeSeries [10];
  char nameSpectrum [10];
  char nameHistogram [10];
  char nameCanvas [10];
  int fftLength (blocksize/2+1);
  TH1F *ts;
  TH1 *fft = 0;
  TH1F *hist;
  TH1F *spec;
  TCanvas *c;
  TObjArray HList (0);

  for (int antenna(0); antenna<nofAntennas; antenna++) {
    // Update names and labels
    sprintf (nameTimeSeries,"Ant%d",antenna);
    sprintf (nameSpectrum,"Spec%d",antenna);
    sprintf (nameHistogram,"Hist%d",antenna);
    sprintf (nameCanvas,"Canv%d",antenna);
    //
    cout << "--> Antenna " << antenna << endl;
    // recreate histograms
    ts   = new TH1F (nameTimeSeries,"Time series",blocksize,0.,blocksize);
    spec = new TH1F (nameSpectrum,"Spectrum",fftLength,0.,fftLength);
    hist = new TH1F (nameHistogram,"Time-series distribution",100,minValue,maxValue);
    c    = new TCanvas (nameCanvas,"Antenna data",10,10,900,300);
    // add histograms to object list
    HList.Add(ts);
    HList.Add(hist);
    HList.Add(spec);
    // fill in the data for this antenna
    for (int sample(0); sample<blocksize; sample++) {
      ts->SetBinContent(sample,float(data(sample,antenna)));
      hist->Fill(float(data(sample,antenna)));
    }
    // fit the histogram by a Gaussian distribution
    hist->SetFillColor(32);
    hist->Fit("gaus");
    // generate the fft
    fft = ts->FFT(fft,"MAG");
    for (int channel(0); channel<fftLength; channel++) {
      spec->SetBinContent(channel,fft->GetBinContent(channel)*fft->GetBinContent(channel));
    }
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

int main ()
{
  int nofFailedTests (0);
  
  // Test dataset to work with
  
  std::string LopesEventFile ("/home/lars/data/lopes/2007-01-31/2007.01.31.23:59:33.960.event");
  
  // Tests
  
  //   nofFailedTests += test_histogram2file();
  //   nofFailedTests += test_lopesevent2hist (LopesEventFile);
  nofFailedTests += test_processing (LopesEventFile);
  
  cout << nofFailedTests << " failed tests." << endl;
  
  return nofFailedTests;
}
