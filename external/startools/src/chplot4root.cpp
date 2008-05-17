
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <fstream>
#include <libgen.h>
#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TApplication.h>

using namespace std;

//! Testprogramm for visualisation with root.
/*!
 Takes data out of a root file and plots it with a TGraph object.
 \n Explanation of some variables:
 \n\b start_event_id - event id to start with
 \n\b delta_event - number of events to loop over
*/


int main(int argc, char *argv[])
{
  char *RootFileName="$HOME/lopes/data/run6_backup_00.root";
  char timestamp[100];
  int window=1024;
  TApplication *app = new TApplication ( "serfit", &argc, argv );
  
  TFile *file = new TFile (RootFileName,"READ");
  TTree *Tevent = (TTree*)file->Get("Tevent");
  
  double trace[7][window];
  for(int i=0; i<7; i++) for(int j=0; j<window; j++) trace[i][j]=0;
  unsigned short int dtrace[window];
  for(int j=0; j<window; j++) dtrace[j]=0;
  unsigned int profile_id, q, event_id;
  double time[window];
  for(int i=0; i<window; i++) time[i]=i*12.5;
  
  Tevent->SetBranchAddress("trace",dtrace);
  Tevent->SetBranchAddress("channel_profile_id",&profile_id);
  Tevent->SetBranchAddress("event_id",&event_id);  
  Tevent->SetBranchAddress("timestamp",timestamp);
  
  int entries, count=0, start_event_id, delta_events, dummy;
  entries=Tevent->GetEntries();
  cout << "Events " << (int)entries/7 << endl;
  
  TCanvas *can = new TCanvas ("can");
  can->Divide(2,4);
  
  TGraph **graph = new TGraph *[7];
  for(int i=0; i<7; i++) graph[i]= new TGraph (window,time,trace[i]);

/*****************************/  
/**/start_event_id=1;
/**/delta_events=7;
/*****************************/  

  for(int i=0; i<(delta_events*7); i++){
  
    Tevent->GetEntry(start_event_id*7+i);
    if(profile_id==21){ q=1; count++;}
    if(profile_id==22){ q=2; count++;}
    if(profile_id==23){ q=3; count++;}
    if(profile_id==24){ q=4; count++;}
    if(profile_id==25){ q=5; count++;}
    if(profile_id==26){ q=6; count++;}
    if(profile_id==27){ q=7; count++;}
    
    for(int h=0; h<window; h++) trace[q-1][h]=dtrace[h];
    
    if(count==7){
      count=0;
      can->cd(1);
      graph[0]->Draw("AL");
      app->Run();
    }
    
    cin >> dummy;
  }
  
  app->Run();
  delete can;
}
