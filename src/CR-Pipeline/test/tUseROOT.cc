
/*!
  \file tUseROOT.cc

  \brief A number of tests working with ROOT

  \author Lars B&auml;hren

  In order to read the data back in and display them:
  \verbatim
  TFile f1 ("demo.root")
  TFile f2 ("lopesevent.root")
  TFile f3 ("lopeseventGraph.root")
  TBrowser browser
  \endverbatim
*/

// Standard C++ library
#include <iostream>
#include <string>

#include <TH1F.h>             // -- ROOT header files
#include <TFile.h>            //
#include <TGraph.h>           // 
#include <TRandom.h>          // 
#include <TObjArray.h>        // 
#include <dalLopesEvent.h>    // -- DAL header files
#include <blitz/array.h>      // -- Blitz++ header files

using std::cerr;
using std::cout;
using std::endl;
using blitz::Array;

// ------------------------------------------------------------------------------

/*!
  \brief Create histograms from antenna data stored in a LopesEvent file

  \brief infile -- Input file from which to read the data
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

int main ()
{
  int nofFailedTests (0);

  // Test dataset to work with

  std::string LopesEventFile ("/home/lars/data/lopes/2007-01-31/2007.01.31.23:59:33.960.event");

  // Tests

  nofFailedTests += test_histogram2file();
  nofFailedTests += test_lopesevent2hist (LopesEventFile);

  cout << nofFailedTests << " failed tests." << endl;

  return nofFailedTests;
}
