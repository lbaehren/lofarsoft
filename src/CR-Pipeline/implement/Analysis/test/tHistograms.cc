
/*!
  \file tHistograms.cc

  \ingroup Analysis

  \brief A number of tests working with ROOT histograms

  \author Lars B&auml;hren
*/

#include <iostream>
#include <string>

#include <TCanvas.h>
#include <TH1F.h>
#include <TFile.h>
#include <TRandom.h>
#include <TRandom3.h>
#include <TObjArray.h>

using std::cerr;
using std::cout;
using std::endl;

// ------------------------------------------------------------------------------

/*!
  \brief Create noise time-series and display it and its power distribution

  Unless no real data is provided, create a dummy time-series (via a random
  number generator).
*/
int test_timeSeries ()
{
  int nofFailedTests (0);

  int blocksize (5000);
  double mean (0);
  double sigma (20);
  int histRange (int(5*sigma));
  float tmp;
  TRandom3 r;

  cout << "-- Creating histogram objects..." << endl;

  TH1F *ts = new TH1F ("hist1","Time series",blocksize,0.,blocksize);
  TH1F *hist = new  TH1F ("hist2","Time-series distribution",100,-histRange,histRange);
  
  TObjArray HList (0);
  HList.Add(ts);
  HList.Add(hist);

  cout << "-- Filling histograms..." << endl;
  for (int n(0); n<blocksize; n++) {
    // get random number ...
    tmp = r.Gaus(mean,sigma);
    // ... and fill it into the histograms
    ts->SetBinContent(n,tmp);
    hist->Fill(tmp);
  }

  cout << "-- Fit Gaussian profile to histogram..." << endl;

  hist->SetFillColor(32);
  hist->Fit("gaus");

  cout << "-- Create canvas to display data..." << endl;

  TCanvas *c = new TCanvas ("MyCanvas","MyCanvas",10,10,1000,400);
  c->Divide(2,1);
  c->cd(1);
  ts->Draw();
  c->cd(2);
  hist->Draw();

  HList.Add(c);

  cout << "-- Exporting data to file..." << endl;

  TFile f ("timeseries.root","recreate");
  HList.Write();
  f.Map();
  f.Close();
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test exporting histograms to a file

  In order to read the data back in and display them:
  \verbatim
  TFile f ("demo.root")
  TBrowser browser
  \endverbatim
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

  nofFailedTests += test_histogram2file();
  nofFailedTests += test_timeSeries();

  cout << nofFailedTests << " failed tests." << endl;

  return nofFailedTests;
}
