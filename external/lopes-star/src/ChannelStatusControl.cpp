#include <iostream>
#include <stdlib.h>

#include <TROOT.h>
#include <TFile.h>
#include <TChain.h>
#include <TMath.h>

#include "util.hh"

using namespace std;

//! channel status control
/*!

checks the data for problems in the dataflow or errors thats results to just a few ADC counts (e.g. LNA damage)

*/


int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();


  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  

  
  char *Input_File_Name="/tmp/default_00.root";
  bool weboutput = false;
  
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "hi:w")) != -1) //: after var ->Waits for input
    switch (c)
      {
      case 'i':
	Input_File_Name = optarg ;
	break;
      case 'w':
	weboutput=true; ;
	break;
      case '?':
        if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\nTry '-h'\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\nTry '-h'\n",
                   optopt);
        return 1;
      case 'h':
        cout << endl;
	cout << " Overview: channel status control\n";
	cout << "=================================\n\n";
	cout << " -h	HELP -- you already tried :-)\n";
	cout << " -i	input file name in STAR format (default: " << Input_File_Name << ")\n";
	cout << " -w	enable output for a webpage\n";	
	cout << "\n\n\nmailto: asch@ipe.fzk.de\n";
	exit(1);
        break;      
      default:
	abort ();
      }

  //set up the access stucture
  int NoChannels=0;
  static struct event event;
  static struct header header;
  static struct channel_profiles channel_profiles;

  TChain *Theader = new TChain("Theader");
  TChain *Tchannel_profile = new TChain("Tchannel_profile");
  TChain *Tevent = new TChain("Tevent");
  CreateTChain(Tevent, Input_File_Name);
  
  bool bTeventExtend;
  if(Tevent->FindBranch("run_id")==0) bTeventExtend=false;
  else bTeventExtend=true;
  
  if(bTeventExtend){
    CreateTevent(Tevent, &event);
    CreateTeventExtend(Tevent, &header, &channel_profiles);
    Tevent->GetEntry(0);
    unsigned int tmp=event.event_id;
    NoChannels=0;
    while(tmp==event.event_id){
      NoChannels++;
      if(Tevent->GetEntry(NoChannels)==0) break;
    }
    if(NoChannels==0) {cout << "NoChannels wrong!" << endl; exit(1); }
  }
  else{
    Theader->Add(Input_File_Name);
    Tchannel_profile->Add(Input_File_Name);
  
    CreateTevent(Tevent, &event);
    CreateTheader(Theader, &header);
    CreateTchannel_profiles(Tchannel_profile, &channel_profiles);

    Tevent->GetEntry(0);
    Theader->GetEntry(0);
    NoChannels = Tchannel_profile->GetEntries();
  }
  
  //start program
  float mean = 0;
  float rms = 0;
  float trace[event.window_size];
  int START=Tevent->GetEntries()-NoChannels;
  
  Tevent->GetEntry(START);
  cout << "\ntimestamp: " << event.timestamp << endl << endl;
  //loop over last event in run
  for(int i=START; i<START+NoChannels; i++){
    Tevent->GetEntry(i);
    for(unsigned int j=0; j<event.window_size; j++) trace[j] = event.trace[j];
    SubtractPedestal(event.window_size, trace);
    for(unsigned int j=0; j<event.window_size; j++) if(trace[j]<0) trace[j] *= -1;
    mean = TMath::Mean(event.window_size,trace);
    rms = TMath::RMS(event.window_size,trace);
    
    printf("D%i_%s_%s:\tmean = %.2f\trms = %.2f",header.DAQ_id, channel_profiles.antenna_pos, channel_profiles.polarisation, mean, rms);
    if(mean < 10 || rms < 10 || mean > 600 || rms > 200) printf("\t!!! warning !!!");
    else printf("\tOK");
    printf("\n");

  }



}

