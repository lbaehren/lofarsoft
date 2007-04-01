//C stuff
#include <iostream>
#include <map>
#include <vector>

//Header files
#include "dipole.hh"
#include "Detector.hh"
#include "detector_reader.hh"
#include "Shower.hh"
#include "mathclasses.hh"
#include "xyzvec.hh"
#include "azelvec.hh"

//ROOT includes
#include <TFile.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TRandom.h>

using namespace std; 

/*!
  \brief Main routine of the simulation program

  For the moment we assume a shower core and which crosses the plane of the
  detector at the (0,0) point. Another assumption is that the wavefront is
  exactly spherical. 
*/

int main( int argc, char *argv[])
{
  int retval (0);

  try {
    if(argc > 2) {
      cout << "You gave in "
	   << argc-1
	   << " command line arguments. Ignoring all but first"
	   << endl;
    }
    if(!argv[1]) {
	throw DetectorReaderException("No command line arguments given, please specify the file to load");
      }
    
    DetectReader detector(argv[1]) ;

    // Shower show(AZ, ZE, dist, tnul)
    //    Shower show(0,180,200,1000) ;

    

    



    
        
    //    detector.array()->interact( &show ) ;
    // testrun(x,y,z,t0)
    detector.array()->testrun(100,250,130,10); 

    vector<double> henk ;
    henk = detector.array()->skypos();
      

    // map<double,int> baselines = (detector.array())->bases();
    
    // map<double,int>::iterator iter; 
    
    // iter=baselines.begin();
    
    // while(iter!=baselines.end()){
    //  cout<<iter->first<<" occurs "<< iter->second<<" time(s)."<<endl;
    //  ++iter;
    // }
    
    // TFile f("output.root", "recreate");

    //    TGraph* gr = new TGraph(detector.array()->getSize(), detector.array()->xes(), detector.array()->ys());
    //    gr ->SetTitle("Detector layout");
    //    gr ->SetMarkerStyle(21);

    //    TH1F* test = new TH1F('  ',"Time of arrivals of the different dipoles", )
    //    gr->Write();
    //    f.Close();
    
    // To find: in parameter space, chop the atmosphere in 4 pieces, calculate the values for the middle points and the chi-squared. 
    // The box with the least chi squared is the one in which you will look. Chop it in pieces again and so on...
    
  }
  
  catch(DetectorReaderException dae) {
    cout<<dae;
  }
  
  return retval;
}
