
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <fstream>
#include <libgen.h>
#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>

#include "util.hh"

using namespace std;

//! convert root database events into csv text files
/*!
 Simple conversion of root event files read out from the database and convert special 
 selected events into a text csv file.
 
 \n Explanation of some variables:
 \n\b start_event_id - event id to start
 \n\b delta_events - number of events to read out and write to the file
*/

/*Root output
Theader->SetScanField(0);
Theader->Scan("*","","col=:10:::15::::35"); > output.txt
Tchannel_profile->SetScanField(0);
Tchannel_profile->Scan("*","","col=:11::12:13:12:17:13::11:11"); >> output.txt
Tevent->SetScanField(0);
Tevent->Scan("*","event_id==1412319","col=:30:30:12:12::18:::::"); >> output.txt
*/

int main(int argc, char *argv[])
{
  char *RootFileName="$HOME/lopes/data/run6_backup_00.root";
  char *csvOutFileName="$HOME/analysis/csvOliver_Run6_end_100E.txt";
  char Command[1024];
  char timestamp[100];
  bool debug=false;

  
  ofstream csvFile;
  csvFile.open(csvOutFileName);
  sprintf(Command,"csv File Format Version 1\nroot antenna data to csv for Oliver\n\n");
  csvFile << Command;
  sprintf(Command, "Ch 1\tCh 2\tCh 3\t Ch 4\tCh 5\t Ch 6\tCh 7\t Ch 8\n");
  csvFile << Command;
  
  TChain *Tevent = new TChain("Tevent");
  CreateTChain(Tevent, RootFileName);
  
  struct event event;
  CreateTevent(Tevent, &event);
  Tevent->GetEntry(0);
  int window=event.window_size;
  
  unsigned short int trace[8][window];
  for(int i=0; i<8; i++) for(int j=0; j<window; j++) trace[i][j]=0;
  unsigned short int dtrace[window];
  for(int j=0; j<window; j++) dtrace[j]=0;
  unsigned int profile_id, q, event_id;
  
  int entries, count=0;
  entries=Tevent->GetEntries();
  cout << "Events " << (int)entries/8 << endl;

///////////////////////////////   
  int START = 0;
 
  unsigned int event_id_start = 1412319;
   for(int w=0; w<Tevent->GetEntries(); w++){
     Tevent->GetEntry(w);
     START = w;
     if(event.event_id == event_id_start) break;
     if(event.event_id > event_id_start) break;
   }
   Tevent->GetEntry(0);
   if(debug) cout << START << " START" << endl;

 int STOP =  START+8*1; //Tevent->GetEntries();
///////////////////////////////
  
  if(STOP> Tevent->GetEntries()){
   cerr << "entry doesn't exist" << endl;
   STOP = Tevent->GetEntries();
   cerr << "changed end number." <<  endl;
  // exit(1);
  }
  
  for(int i=START; i<STOP; i++){
    Tevent->GetEntry(START);
    if(profile_id==21){ q=1; count++;}
    if(profile_id==22){ q=2; count++;}
    if(profile_id==23){ q=3; count++;}
    if(profile_id==24){ q=4; count++;}
    if(profile_id==25){ q=5; count++;}
    if(profile_id==26){ q=6; count++;}
    if(profile_id==27){ q=7; count++;}
    
    for(int h=0; h<window; h++) trace[q-1][h]=dtrace[h];
    
    if(count==8){
      count=0;
      csvFile << "\nEvent ID: " << event_id << "\ttimestamp: " << timestamp << endl;
      for(int u=0; u<window; u++){
        sprintf(Command,"%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\n",trace[0][u],trace[1][u],trace[2][u],trace[3][u],trace[4][u],trace[5][u],trace[6][u],trace[6][u]);
        csvFile << Command;
      }
    }
    
    
  }
  csvFile.close();
}
