#include <iostream>
#include <math.h>

#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <TRandom.h>
#include <TH1D.h>
#include <TGraph.h>
#include <TMath.h>

#include "util.hh"

using namespace std;
using namespace TMath;

//! Monte Carlo simulations for timestamp and run information
/*!
 Create a root file with a Tevent, Theader, Tchannel_profile and Sh3 struct.
 The timestamp of the Tevent struct member is filled with
 a random generated time.
 Choosable is the trigger range and distribution. 
 The Sh3 struct could also be filled with the Tevent gps timestamp or with a independent
 distribution of trigger times.
 
 \n Explanation of some variables:
 \n\b rand - random generated time between two events
 \n\b number - number of randomly generated events
*/



int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();

  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  
  


  struct header header;
  struct event event;
  struct h3 Sh3;
  struct channel_profiles channel_profiles;
  
  
  char *MCRootFileName="$HOME/analysis/MCout_default.root";
  char name[256];
  
  TFile *MC_file = new TFile (MCRootFileName,"RECREATE");
  TTree *Tevent = new TTree ("Tevent","MC data");
  TTree *Theader = new TTree("Theader","MC data");
  TTree *h3 = new TTree("h3","MC data");
  TTree *Tchannel_profile = new TTree("Tchannel_profile","MC data");
  
  
  
  Tevent->Branch("event_id",&event.event_id,"event_id/i");
  Tevent->Branch("timestamp",&event.timestamp,"timestamp/C");
  Tevent->Branch("timestamp_db",&event.timestamp_db,"timestamp_db/C");
//  Tevent->Branch("leap_second",&event.leap_second,"leap_second/O");
  Tevent->Branch("window_size",&event.window_size,"window_size/i");
//  Tevent->Branch("delta_t",&event.delta_t,"delta_t/i");
  Tevent->Branch("channel_profile_id",&event.channel_profile_id,"channel_profile_id/i");
  Tevent->Branch("sample_id",&event.sample_id,"sample_id/C");
  Tevent->Branch("trace",&event.trace,"trace[window_size]/s");
  
  event.window_size=1024;
  event.channel_profile_id=0;
  strcpy(event.sample_id,"0");

 //Fill trace
  for(unsigned int u=0; u<event.window_size; u++){
    event.trace[u] = (short int)(100*TMath::Sin(2*TMath::Pi()*50.0/80.0*u+TMath::Pi()/4))+(short int)250;
  }


  Tchannel_profile->Branch("ch_id",&channel_profiles.ch_id,"ch_id/i");
  Tchannel_profile->Branch("channel_no",&channel_profiles.channel_no,"channel_no/i");
//  Tchannel_profile->Branch("threshold",&channel_profiles.threshold,"threshold/I");
//  Tchannel_profile->Branch("coincidence",&channel_profiles.coincidence,"coincidence/I");
//  Tchannel_profile->Branch("board_address",&channel_profiles.board_address,"board_address/i");
  Tchannel_profile->Branch("antenna_pos",&channel_profiles.antenna_pos,"antenna_pos/C");
//  Tchannel_profile->Branch("polarisation_id",&channel_profiles.polarisation_id,"polaristation_id/i");
  Tchannel_profile->Branch("polarisation",channel_profiles.polarisation,"polaristation/C");
//  Tchannel_profile->Branch("filter",&filter.order,"order/I:upper_limit:lower_limit");

  channel_profiles.ch_id=0;
  channel_profiles.channel_no = 0;
  strcpy(channel_profiles.antenna_pos,"120");
  strcpy(channel_profiles.polarisation,"North/South");
  Tchannel_profile->Fill();
  

  Theader->Branch("run_id",&header.run_id,"run_id/i");
//  Theader->Branch("start_time",&header.start_time,"start_time/C");
//  Theader->Branch("stop_time",&header.stop_time,"stop_time/C");
//  Theader->Branch("profile_id",&header.profile_id,"profile_id/i");
  Theader->Branch("clock_frequency",&header.clock_frequency,"clock_frequency/i");
//  Theader->Branch("posttrigger",&header.posttrigger,"posttrigger/i");
  Theader->Branch("DAQ_id",&header.DAQ_id,"DAQ_id/S");
//  Theader->Branch("gps_position",&header.gps_position,"gps_position/C");
//  Theader->Branch("profile_name",&header.profile_name,"profile_name/C");

  header.run_id=0;
  header.clock_frequency = (int)80e6;
  header.DAQ_id=0;
  Theader->Fill();

  h3->Branch("Gt",&Sh3.Gt,"Gt/i");
  h3->Branch("Mmn",&Sh3.Mmn,"Mmn/i");
  h3->Branch("Fanka",&Sh3.Fanka,"Fanka/b");
  h3->Branch("Hit7",&Sh3.Hit7,"Hit7/i");
  h3->Branch("Nflg",&Sh3.Nflg,"Nflg/B");
  h3->Branch("Ageg",&Sh3.Ageg,"Ageg/F");
  h3->Branch("Sizeg",&Sh3.Sizeg,"Sizeg/F");
  h3->Branch("Sizmg",&Sh3.Sizmg,"Sizmg/F");
  h3->Branch("Zeg",&Sh3.Zeg,"Zeg/F");
  h3->Branch("Idmx",&Sh3.Idmx,"Idmx/B");
  h3->Branch("Ngrs",&Sh3.Ngrs,"Ngrs/B");

  TH1D *his1 = new TH1D("his1","his1",300,0,30);
  his1->GetXaxis()->SetTitle("f / Hz");
  his1->GetYaxis()->SetTitle("count");
  sprintf(name,"MC data Star - frequency of trigger");
  his1->SetTitle(name);
  his1->SetMarkerStyle(8);

  TH1D *his3 = new TH1D("his3","his3",300,0,30);
  his3->GetXaxis()->SetTitle("f / Hz");
  his3->GetYaxis()->SetTitle("count");
  sprintf(name,"CHECK 1: MC data Star - frequency of trigger");
  his3->SetTitle(name);
  his3->SetMarkerStyle(8);

  TH1D *his4 = new TH1D("his4","his4",300,0,30);
  his4->GetXaxis()->SetTitle("f / Hz");
  his4->GetYaxis()->SetTitle("count");
  sprintf(name,"CHECK 2: MC data Star - frequency of trigger");
  his4->SetTitle(name);
  his4->SetMarkerStyle(8);

  TH1D *his2 = new TH1D("his2","his2",300,0,30);
  his2->GetXaxis()->SetTitle("f / Hz");
  his2->GetYaxis()->SetTitle("count");
  sprintf(name,"MC data Grande - frequency of trigger");
  his2->SetTitle(name);
  his2->SetMarkerStyle(8);

  double rand =0, DeltaTime=0, TimeDummy1=0, TimeDummy2=0, TimeDummy3=0, TimeDummy4=0;
  double DTime1=0, DTime2=0;
  time_t GrandeTime=0, StarTime=0;
  int SubGrandeTime=0;
  struct tm starTimeTM;
  int starTimeSubSec=0;
  int StarTime1=0, StarTime2=0, StarSubSec1=0, StarSubSec2=0;
  

  //start time: 2006-01-01 00:00:00
  starTimeTM.tm_year = 2006-1900;
  starTimeTM.tm_mon = 1-1;
  starTimeTM.tm_mday =  1;
  starTimeTM.tm_hour = 0;
  starTimeTM.tm_min = 0;
  starTimeTM.tm_sec = 0;
//cout << asctime(&starTimeTM) << endl;

  GrandeTime = mktime(&starTimeTM);
  StarTime = mktime(&starTimeTM);

//cout << StarTime << " " << ctime(&StarTime) << endl;
  
  starTimeTM = *localtime(&StarTime);

//cout << asctime(&starTimeTM) << endl;
  

  event.event_id=0;
  sprintf(name,"CH0 %02i.%02i.%02i %02i:%02i:%02i.%07u", starTimeTM.tm_mday, starTimeTM.tm_mon+1,
                    starTimeTM.tm_year-100, starTimeTM.tm_hour, starTimeTM.tm_min, starTimeTM.tm_sec, starTimeSubSec);

  strcpy(event.timestamp,name);
  strcpy(event.timestamp_db,event.timestamp);

//cout << event.timestamp << endl << endl;

  Tevent->Fill();
  event.event_id++;

  Sh3.Gt=(int)GrandeTime;
  Sh3.Mmn=SubGrandeTime;
  Sh3.Fanka = 0;
  Sh3.Hit7 = 0x80;
  Sh3.Nflg = 1;
  Sh3.Ageg = 1;
  Sh3.Sizeg = 1e9;
  Sh3.Sizmg = 1e9;
  Sh3.Zeg = 0;
  Sh3.Idmx = 17;
  Sh3.Ngrs = 32;
  h3->Fill();

  
  StarTime = mktime(&starTimeTM);
  TimeDummy1 = (int)StarTime;
  TimeDummy2 = (int)GrandeTime;
  StarTime2= (int)StarTime;
  StarSubSec2=0;
  
  int number = 3;
  
  for(int i=0; i<number; i++){
    TimeDummy3=TimeDummy1;
    StarTime1=StarTime2;
    StarSubSec1=StarSubSec2;

    rand = gRandom->Uniform(0.01,25); // frequency range
    //rand = gRandom->Gaus(12.5,2);
    
    his1->Fill(rand);
    
    DeltaTime = (double)(1/rand);
//  cout << DeltaTime << " " << rand << " " << (double)(1/rand)  << endl;
    
    //manipulate starTimeStamp
    
    TimeDummy1 += DeltaTime;

// sprintf(name,"%lf",TimeDummy1);
// cout << name << endl;

    if(TimeDummy3!=0) his3->Fill((1.0/(TimeDummy1-TimeDummy3)));    
    
    
    StarTime = (time_t)TimeDummy1;
    starTimeSubSec = (int)trunc(((TimeDummy1-(int)(TimeDummy1))*10000000.0));

// sprintf(name,"%d - %u",(int)StarTime,starTimeSubSec);   
// cout << name << endl;    
    
    
    starTimeTM = *localtime(&StarTime);


  sprintf(name,"CH0 %02i.%02i.%02i %02i:%02i:%02i.%07u", starTimeTM.tm_mday, starTimeTM.tm_mon+1,
                    starTimeTM.tm_year-100, starTimeTM.tm_hour, starTimeTM.tm_min, starTimeTM.tm_sec, starTimeSubSec);

    strcpy(event.timestamp,name);
    strcpy(event.timestamp_db,event.timestamp);
//cout << event.timestamp << " " << " " << name << " " <<  starTimeSubSec << endl;
//cout << event.timestamp << endl;    

    ConvertTimeStamp(event.timestamp,&StarTime2, &StarSubSec2, 0.0);
    
    if(StarTime1!=0){
      DTime1=(double)((double)(StarTime1)+(double)(StarSubSec1)/1000000000.0);
      DTime2=(double)((double)(StarTime2)+(double)(StarSubSec2)/1000000000.0);
// sprintf(name,"%lf - %lf",DTime1,DTime2);   
// cout << name << endl;
      TimeDummy4 = 1.0/(DTime2-DTime1);
      his4->Fill(TimeDummy4);
    }
    
    if( fabs((1.0/(TimeDummy1-TimeDummy3)) - (1.0/(DTime2-DTime1))) > 0.1 ){
       cout << "error1 " << (1.0/(TimeDummy1-TimeDummy3)) << " " << (1.0/(DTime2-DTime1));
       cout << " " << rand << endl;
    }
    
    //Fill trace
    for(unsigned int u=0; u<event.window_size; u++){
      //event.trace[u] = (short int)(100*TMath::Sin(2*TMath::Pi()*79.0/80.0*u)) +(short int)250;
      event.trace[u] = (short int)(100*TMath::Sin(2*TMath::Pi()*70.0/80.0*u+TMath::Pi()/4) + 50*TMath::Sin(2*TMath::Pi()*60.0/80.0*u+TMath::Pi()/4) + 10*TMath::Sin(2*TMath::Pi()*58.0/80.0*u+TMath::Pi()/4)) +(short int)250;
    }
    

    
    Tevent->Fill();
    event.event_id++;
    
#if 0

    }
  
  for(int i=0; i<number; i++){
    //rand = gRandom->Uniform(0.1,25); // frequency range
    rand = gRandom->Gaus(12.5,2);

#else
#define SWITCH
#endif

    
    his2->Fill(rand);
    
    DeltaTime = (double)(1/rand);
    //cout << DeltaTime << " " << rand << " " << (double)(1/rand)  << endl;
    
    //manipulate GrandeTimeStamp
    
    TimeDummy2  += DeltaTime;
    GrandeTime = (int)TimeDummy2;
    SubGrandeTime = (int)((TimeDummy2-(int)TimeDummy2)*10000000.0);
    SubGrandeTime *= 100;
    
    //cout << GrandeTime << " " << SubGrandeTime << endl;
    
 
    Sh3.Gt=GrandeTime;
    Sh3.Mmn=SubGrandeTime;
    
    h3->Fill();
  }





#ifdef SWITCH

  for(int i=0; i<h3->GetEntries();i++){
    h3->GetEntry(i);
    Tevent->GetEntry(i);

    ConvertTimeStamp(event.timestamp,&StarTime2, &StarSubSec2, 0.0);
    
    if(   abs(StarTime2 - (int)Sh3.Gt) != 0
	 || abs(StarSubSec2 - (int)Sh3.Mmn) != 0 )
	     cout << "error2 " << StarTime2 << " " << Sh3.Gt << " " << StarSubSec2 << " " << Sh3.Mmn << endl;
    
  }

#endif

   TH1F *TraceCheck = new TH1F ("TraceCheck","TraceCheck",event.window_size,0,event.window_size);
   TraceCheck->SetTitle("trace check - count vs time");   
   Tevent->GetEntry(2);
   for(unsigned int i=0; i<event.window_size; i++) TraceCheck->SetBinContent(i+1,event.trace[i]);
   
   int *Trace = new int [event.window_size];
   int *TimeTrace = new int [event.window_size];
   for(unsigned int i=0; i<event.window_size; i++) {
     Trace[i]= event.trace[i];
     TimeTrace[i]=i;
   }
   TGraph *TraceCheck2 = new TGraph (event.window_size,TimeTrace,Trace);


  his1->Write();
  his2->Write();
  his3->Write();
  his4->Write();
  TraceCheck->Write();
  TraceCheck2->Write();
  Tevent->Write();
  Theader->Write();
  h3->Write();
  Tchannel_profile->Write();
  MC_file->Close();
  
/*
  delete Tevent;
//  delete Theader;
//  delete h3;
//  delete Tchannel_profile;
  delete his1;
  delete his3;
  delete his4;
  delete his2;
  delete TraceCheck;
  delete TraceCheck2;

  delete[] Trace;
  delete[] TimeTrace;
*/  
  delete MC_file;
 
}  


