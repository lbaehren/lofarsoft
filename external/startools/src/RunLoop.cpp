#include <iostream>

#include <TROOT.h>
#include <TFile.h>
#include <TH2F.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TMath.h>


#include "util.hh"

using namespace std;
using namespace TMath;

//! run loop
/*!
simple programm which loopes over a run and test some functions
*/



int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();


  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  

  
  char *StarFileName="$HOME/analyisis/default_00.root";
  char *OutputFileName="$HOME/analysis/RunLoop_out.root";

  
  
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "hf:i:")) != -1) //: after var ->Waits for input
    switch (c)
      {
      case 'f':
	OutputFileName = optarg ;
	break;
      case 'i':
	StarFileName = optarg ;
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
	cout << " Overview: run loop\n";
	cout << "===================\n\n";
	cout << " -h	HELP -- you already tried :-)\n";
	cout << " -f	output file name (default: " << OutputFileName << ")\n";
	cout << " -i	input file name in STAR format (default: " << StarFileName << ")\n";
	cout << "\n\n\nmailto: asch@ipe.fzk.de\n";
	exit(1);
        break;      
      default:
	abort ();
      }

  

  int NoChannels=0;
  struct event event;
  struct header header;
  struct channel_profiles channel_profiles;

  TChain *Theader = new TChain("Theader");
  TChain *Tchannel_profile = new TChain("Tchannel_profile");
  TChain *Tevent = new TChain("Tevent");
  CreateTChain(Tevent, StarFileName);
  
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
    if(NoChannels==0) {cerr << "NoChannels wrong!" << endl; exit(1); }
  }
  else{
    Theader->Add(StarFileName);
    Tchannel_profile->Add(StarFileName);
  
    CreateTevent(Tevent, &event);
    CreateTheader(Theader, &header);
    CreateTchannel_profiles(Tchannel_profile, &channel_profiles);

    Tevent->GetEntry(0);
    Theader->GetEntry(0);
    NoChannels = Tchannel_profile->GetEntries();
  }


  TFile *output = new TFile (OutputFileName,"Recreate","dynamic spectra",6);
  delete output;
  
///////////////////////////////   
  int START = 0;
 
  unsigned int event_id_start = 0;//1389063;
   for(int w=0; w<Tevent->GetEntries(); w++){
     Tevent->GetEntry(w);
     START = w;
     if(event.event_id == event_id_start) break;
     if(event.event_id > event_id_start) break;
   }
   Tevent->GetEntry(0);
   //cout << START << " START" << endl;

 int STOP =  /* START+NoChannels*1; // */  Tevent->GetEntries();
///////////////////////////////
  
  if(STOP> Tevent->GetEntries()){
   cerr << "entry doesn't exist" << endl;
   STOP = Tevent->GetEntries();
   cerr << "changed end number." <<  endl;
  // exit(1);
  }
  
 
  int ChId=0;
  char title[2048];
  
  
  float *Amp = new float [event.window_size/2];
  float *Phase = new float [event.window_size/2];  
  float *Tracefft = new float [event.window_size*2];  
  float *Trace = new float[event.window_size];
  int NoZeros = 7;
  int index = 0; 
  
  int DefNoChannels=4*2;
  
  if(DefNoChannels<NoChannels){
    cerr << "wrong no of channels ... " << endl;
    exit(0);
  }
  
  //find threshold
  bool FindThreshold = false;
  
  //level X Trigger
  bool trigger = true;
  float threshold[NoChannels];
  for(int i=0; i<NoChannels; i++) threshold[i]=1;

  //Trigger check
  int NoCh = NoChannels;
  int NoZerosTr = NoZeros;
  int new_window_size = event.window_size*(NoZeros+1);
  float TriggerTracetmp[event.window_size*(NoZerosTr+1)];
  float **TriggerTrace = new float *[NoCh];
  float **TriggerTrace2 = new float *[NoCh];
  for(int i=0; i<NoCh; i++) TriggerTrace[i] = new float [event.window_size*(NoZerosTr+1)];
  for(int i=0; i<NoCh; i++) TriggerTrace2[i] = new float [event.window_size*(NoZerosTr+1)];
  for(int i=0; i<NoCh; i++) for(unsigned int j=0; j<event.window_size*(NoZerosTr+1); j++){
    TriggerTrace[i][j]=0;
    TriggerTrace2[i][j]=0;
    TriggerTracetmp[j]=0;
  }
  char **ant_pos = new char *[NoCh];
  for(int i=0; i<NoCh; i++) ant_pos[i] = new char [15];
  
  int accepted=0;
  int rejected=0;

  
  float time[event.window_size*(NoZerosTr+1)];
  for(unsigned int i=0; i<event.window_size*(NoZerosTr+1); i++) time[i]=i;
  
  TGraph *gTriggerTrace;
  int itmp=0;
  
  float coincidence_time = 180.0;
  
  Tevent->GetEntry(START);
  cout << "start event_id = " << event.event_id << endl;
  
  for(int i=START; i<STOP; i++){
   //status output
   printf("\t%i of %i\r",i, STOP);
   fflush (stdout);

   if(Tevent->GetEntry(i)==0){
     cerr << "error - Tevent->GetEntry()" << endl;
     //exit(0);
     break;
   }
   
   if(!bTeventExtend){
    ChId=0;
    Tchannel_profile->GetEntry(ChId);
    while( event.channel_profile_id != channel_profiles.ch_id ){
      ChId++;
      Tchannel_profile->GetEntry(ChId);
    }
   }
   if(i<START+NoChannels*2)
     sprintf(title,"RunLoop - D%i_%s_%s - run %02i",header.DAQ_id, channel_profiles.antenna_pos, channel_profiles.polarisation,header.run_id);
   

   if(!strcmp(channel_profiles.antenna_pos,"CTR")){
     if(!strcmp(channel_profiles.polarisation,"North/South")){
       index = 0;
     }
     if(!strcmp(channel_profiles.polarisation,"East/West")){
       index = 1;
     }
   }
   else{
   if(!strcmp(channel_profiles.antenna_pos,"240")){
     if(!strcmp(channel_profiles.polarisation,"North/South")){
       index = 2;
     }
     if(!strcmp(channel_profiles.polarisation,"East/West")){
       index = 3;
     }
   }
   else{
   if(!strcmp(channel_profiles.antenna_pos,"300")){
     if(!strcmp(channel_profiles.polarisation,"North/South")){
       index = 4;
     }
     if(!strcmp(channel_profiles.polarisation,"East/West")){
       index = 5;
     }
   }
   else{
   if(!strcmp(channel_profiles.antenna_pos,"360")){
     if(!strcmp(channel_profiles.polarisation,"North/South")){
       index = 6;
     }
     if(!strcmp(channel_profiles.polarisation,"East/West")){
       index = 7;
     }
   }
   else{
      index = -1 ;
      cerr << " right index not found .. set index = -1 " << endl;
   }}}}
   
   
   
   if(index!=-1){
    
    if(trigger){  
      for(unsigned int w=0; w<event.window_size; w++)TriggerTrace[index][w]=event.trace[w];
      strcpy(ant_pos[index],channel_profiles.antenna_pos);
      itmp++;
      
      if(itmp==NoCh){
        if(L0Decide(NoCh, event.window_size, TriggerTrace, ant_pos, header.DAQ_id, &coincidence_time)){
	  cout << "\n* trigger * " << event.event_id << endl;
	  accepted++;
	}
          else {
	    //cout << "nix ";
	    rejected++;
	  }
        itmp=0;
	//break;
      }
    }

   if(FindThreshold){
     for(unsigned int j=0; j<event.window_size; j++) TriggerTracetmp[j]=event.trace[j];
//     RFISuppression(event.window_size, TriggerTracetmp, TriggerTracetmp);
     ZeroPaddingFFT(event.window_size, TriggerTracetmp, NoZeros,  TriggerTracetmp);
     Rectifier(NoZeros, new_window_size, TriggerTracetmp);
     for(unsigned int j=0; j<(event.window_size*(NoZeros+1)); j++) TriggerTrace2[index][j] += TriggerTracetmp[j];
   }

   }
   
  
  } // end for over all traces and events
  
  if(FindThreshold){
    for(int i=0; i<NoChannels; i++) for(unsigned int j=0; j<(event.window_size*(NoZeros+1)); j++) TriggerTrace2[i][j] /= ((STOP-START)/(double)(NoChannels));
    for(int i=0; i<NoChannels; i++) cout << "channel " << i << ": Mean = " << Mean(new_window_size,TriggerTrace2[i]) << " -- RMS = " << RMS(new_window_size,TriggerTrace2[i]) << endl;
  }
  


  output = new TFile (OutputFileName,"UPDATE");
  output->cd();
  
  gTriggerTrace = new TGraph (new_window_size,time, TriggerTrace2[0]);
  gTriggerTrace->SetName("l1trigger");
  gTriggerTrace->SetTitle("L1 Trigger Check");
  gTriggerTrace->GetXaxis()->SetTitle(" arbitary unit");
  gTriggerTrace->GetYaxis()->SetTitle(" ADC count ");
  TCanvas *can = new TCanvas();
  gTriggerTrace->Draw("AL");
  can->Write();
  gTriggerTrace->Write();

  
  cout << "Entries: " << Tevent->GetEntries() << endl;
  cout << "Accepted = " << accepted <<  " - Recjected = " << rejected << endl;
  cout << "Accpeted = " << (double)((double)accepted/(double)(accepted+rejected))*100 << "%" << endl;

  
  output->Close();

  //do not forget to clean up
  delete output;

  delete Tevent;
  delete Theader;
  delete Tchannel_profile;
  
  delete[] Amp;
  delete[] Phase;
  delete[] Tracefft;
  delete[] Trace;
  
  for(int i=0; i<NoCh; i++) delete[] TriggerTrace[i];
  delete[] TriggerTrace;
  delete gTriggerTrace;
  delete can;

}
