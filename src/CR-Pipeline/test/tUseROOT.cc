
/*!
  \file tUseROOT.cc

  \brief A number of tests working with ROOT

  \author Lars B&auml;hren
*/

#include <iostream>
#include <string>

#include <TH1F.h>
#include <TFile.h>
#include <TRandom.h>
#include <TObjArray.h>

using std::cerr;
using std::cout;
using std::endl;

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

  cout << nofFailedTests << " failed tests." << endl;

  return nofFailedTests;
}
