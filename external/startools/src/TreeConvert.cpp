#include <iostream>

#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <time.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TChain.h>

#include "util.hh"


using namespace std;


//! Conversion of differnt struct into the other
/*!
 Input is a root file with, e.g. the Sh3 struct. The timestamps of this struct is then converted
 into a event, header and channel_profile sturcts.
 The output files can be used for the timestamp_compare algorithm.

*/


int main(int argc, char *argv[])
{
  static struct header header;
  static struct header header_orig;
  static struct event event_orig;
  static struct event event;
  static struct h3 Sh3;
  static struct h3 Sh3_orig;
  static struct channel_profiles channel_profiles;
  static struct channel_profiles channel_profiles_orig;
  static struct filter filter;
  static struct filter filter_orig;


  setenv("TZ","UTC",1);
  tzset();

  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  

  char *RootFileNameInput="$HOME/analysis/TCin_default.root";
  char *RootFileNameOutput="$HOME/analysis/TCout_default.root";

  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "i:ho:")) != -1) //: after var ->Waits for input
    switch (c)
      {
      case 'i':
	RootFileNameInput = optarg ;
	break;
      case 'o':
	RootFileNameOutput = optarg ;
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
	cout << " Overview: Conversion of Tree structs\n";
	cout << "=====================================\n\n";
	cout << " -h	HELP -- you already tried :-)\n";
	cout << " -i	name of the root input file (default: " << RootFileNameInput << ")\n";
	cout << " -o	name of the root output file (default: " << RootFileNameOutput << ")\n";

	cout << "\n\n\nmailto: asch@ipe.fzk.de\n";
	exit(1);
        break;      
      default:
	abort ();
      }


  bool StarFile=true;

  
  TFile *In_file = new TFile (RootFileNameInput,"READ");
  TFile *Out_file = new TFile (RootFileNameOutput,"RECREATE");
  

  

  if( ((TTree*)In_file->Get("h3"))!=0 ) StarFile=false;

  TTree *h3;
  TTree *Tevent;
  TTree *Theader;
  TTree *Tchannel_profile;

  
  if(StarFile){
    TChain *Tevent_orig = new TChain("Tevent");
    CreateTChain(Tevent_orig, RootFileNameInput);
    
    TChain *Theader_orig = new TChain("Theader");
    Theader_orig->Add(RootFileNameInput);

    TChain *Tchannel_profile_orig = new TChain("Tchannel_profile");
    Tchannel_profile_orig->Add(RootFileNameInput);
    
    CreateTevent(Tevent_orig, &event_orig);
    CreateTheader(Theader_orig, &header_orig);
    CreateTchannel_profiles(Tchannel_profile_orig, &channel_profiles_orig);

    Tevent = new TTree("Tevent","copied data");
    Tevent->Branch("event_id",&event.event_id,"event_id/i");
    Tevent->Branch("timestamp",&event.timestamp,"timestamp/C");
    Tevent->Branch("timestamp_db",&event.timestamp_db,"timestamp_db/C");
    Tevent->Branch("leap_second",&event.leap_second,"leap_second/O");
    Tevent->Branch("window_size",&event.window_size,"window_size/i");
    Tevent->Branch("delta_t",&event.delta_t,"delta_t/i");
    Tevent->Branch("channel_profile_id",&event.channel_profile_id,"channel_profile_id/i");
    Tevent->Branch("sample_id",&event.sample_id,"sample_id/C");

    Theader = new TTree("Theader","copied data");
    Theader->Branch("run_id",&header.run_id,"run_id/i");
    Theader->Branch("start_time",&header.start_time,"start_time/C");
    Theader->Branch("stop_time",&header.stop_time,"stop_time/C");
    Theader->Branch("profile_id",&header.profile_id,"profile_id/i");
    Theader->Branch("clock_frequency",&header.clock_frequency,"clock_frequency/i");
    Theader->Branch("posttrigger",&header.posttrigger,"posttrigger/i");
    Theader->Branch("DAQ_id",&header.DAQ_id,"DAQ_id/S");
    Theader->Branch("gps_position",&header.gps_position,"gps_position/C");
    Theader->Branch("profile_name",&header.profile_name,"profile_name/C");
 
    Tchannel_profile = new TTree("Tchannel_profile","copied data");
    Tchannel_profile->Branch("ch_id",&channel_profiles.ch_id,"ch_id/i");
    Tchannel_profile->Branch("channel_no",&channel_profiles.channel_no,"channel_no/i");
    Tchannel_profile->Branch("threshold",&channel_profiles.threshold,"threshold/I");
    Tchannel_profile->Branch("coincidence",&channel_profiles.coincidence,"coincidence/I");
    Tchannel_profile->Branch("board_address",&channel_profiles.board_address,"board_address/i");
    Tchannel_profile->Branch("antenna_pos",&channel_profiles.antenna_pos,"antenna_pos/C");
    Tchannel_profile->Branch("polarisation_id",&channel_profiles.polarisation_id,"polaristation_id/i");
    Tchannel_profile->Branch("polarisation",channel_profiles.polarisation,"polaristation/C");

    Theader_orig->GetEntry(0);
    header = header_orig;
    Theader->Fill();
   
    Tchannel_profile->GetEntry(0);
    channel_profiles = channel_profiles_orig;
    channel_profiles.channel_no = 0;
    Tchannel_profile->Fill();


    h3 = new TTree("h3","copied data");
    h3->Branch("Gt",&Sh3.Gt,"Gt/i");
    h3->Branch("Mmn",&Sh3.Mmn,"Mmn/i");
    h3->Branch("Fanka",&Sh3.Fanka,"Fanka/i");
    h3->Branch("Hit7",&Sh3.Hit7,"Hit7/i");
    h3->Branch("Nflg",&Sh3.Nflg,"Nflg/i");
    h3->Branch("Ageg",&Sh3.Ageg,"Ageg/F");
    h3->Branch("Sizeg",&Sh3.Sizeg,"Sizeg/F");
    h3->Branch("Sizmg",&Sh3.Sizmg,"Sizeg/F");
    h3->Branch("Zeg",&Sh3.Zeg,"Zeg/F");
    h3->Branch("Ngrs",&Sh3.Ngrs,"Ngrs/B");
    h3->Branch("Xcg",&Sh3.Xcg,"Xcg/F");                
    h3->Branch("Ycg",&Sh3.Ycg,"Ycg/F");
    h3->Branch("Iact",&Sh3.Iact,"Iact/s");        

    
    Sh3.Fanka = 0;
    Sh3.Hit7 = 0x80;
    Sh3.Nflg = 1;
    Sh3.Ageg = 1;
    Sh3.Sizeg = 1.0e8;
    Sh3.Sizmg = 1.0e8;
    Sh3.Zeg = 10*2*3.14/360;
    Sh3.Ngrs = 25;
    Sh3.Xcg = 0;
    Sh3.Ycg = 0;
    Sh3.Iact = 1;
    
    
   
    int NoOfAntennas = Tchannel_profile_orig->GetEntries();
    int StarEntries = Tevent_orig->GetEntries();
    int Sec=0, SubSec=0;
    int DSec=0, DSubSec=0;
    
    unsigned int OldEvent_id=0;
    event.event_id=0;
    cout << "No of antennas = " << NoOfAntennas << endl;
    
    for(long int i=0; i<(StarEntries/NoOfAntennas); i++){
      OldEvent_id = event.event_id;
      
      if(Tevent_orig->GetEntry(i*NoOfAntennas)<=0 ) cerr << "GetEntry() - error" << endl;
      if(Sec!=0) 
        while(ConvertTimeStamp(event_orig.timestamp,&DSec,&DSubSec,0.0)==0){
	  cout << "found wrong timestamp .. skip it" << endl;
          i++;
  	  Tevent_orig->GetEntry(i*NoOfAntennas);
        }
      
      while( strlen(event_orig.timestamp)<20 
             || strcmp(event_orig.timestamp,event.timestamp)==0 
	     || strcmp(event_orig.timestamp,"NULL")==0
	     || (DSec==Sec && DSubSec<SubSec)){
	     
        cerr << "error with timestamp - take next one -";
	if(strlen(event_orig.timestamp)==0) cerr << " length zero " ;
	if(strlen(event_orig.timestamp)==19) cerr << " length  19 " ;
	if(strcmp(event_orig.timestamp,event.timestamp)==0) cerr << " same string ";
	if(strcmp(event_orig.timestamp,"NULL")==0) cerr << " NULL string ";
	if((DSec==Sec && DSubSec<SubSec)) cerr << " wrong timestamp order ";
	cerr << "- ID " << event_orig.event_id  << " - " << event_orig.timestamp << " - " <<event.timestamp << endl;
	
        i++;
	if(i>=(StarEntries/NoOfAntennas)) break;
        if(Tevent_orig->GetEntry(i*NoOfAntennas)<=0 ) cerr << "GetEntry() - error" << endl;
	if(ConvertTimeStamp(event_orig.timestamp,&DSec,&DSubSec,0.0)==0) cout << "error1 - timestamp needs check" << endl;
      } 
      
      if(OldEvent_id==event_orig.event_id){ cerr << "equal event id = " << event_orig.event_id << endl;}
      
      event = event_orig;
      
      Tevent->Fill();
      
      
      if(ConvertTimeStamp(event.timestamp,&Sec,&SubSec,0.0)==0) cout << "error2 - timestamp needs check" << endl;;
      Sh3.Gt = (unsigned int) Sec;
      Sh3.Mmn = (unsigned int) SubSec;
      
      h3->Fill();
      
      
     
    }
  

  }
  else{
#warning h3 tree needs update  
  TTree *Tchannel_profile_orig;
  TTree *h3_orig;
  TTree *Theader_orig;
  
  
    h3_orig = (TTree*)In_file->Get("h3");
    if(h3_orig==0){cout << "error1" << endl; exit(1);}
    
    h3_orig->SetBranchAddress("Gt",&Sh3_orig.Gt);
    h3_orig->SetBranchAddress("Mmn",&Sh3_orig.Mmn);
    h3_orig->SetBranchAddress("Fanka",&Sh3_orig.Fanka);
    h3_orig->SetBranchAddress("Hit7",&Sh3_orig.Hit7);
    

    h3 = new TTree ("h3","copied data");
    h3->Branch("Gt",&Sh3.Gt,"Gt/i");
    h3->Branch("Mmn",&Sh3.Mmn,"Mmn/i");
    h3->Branch("Fanka",&Sh3.Fanka,"Fanka/i");
    h3->Branch("Hit7",&Sh3.Hit7,"Hit7/i");
    
    
    Tevent = new TTree ("Tevent","copied data");
    Theader = new TTree("Theader","copied data");
    Tchannel_profile = new TTree("Tchannel_profile","copied data");
    
    Tevent->Branch("event_id",&event.event_id,"event_id/i");
    Tevent->Branch("timestamp",&event.timestamp,"timestamp/C");
    event.event_id = 0;
    
    Theader->Branch("run_id",&header.run_id,"run_id/i");
    header.run_id=0;
    Theader->Fill();
  
    Tchannel_profile->Branch("channel_no",&channel_profiles.channel_no,"NoOfAntennas/i");
    channel_profiles.channel_no = 0;
    Tchannel_profile->Fill();


    struct tm starTimeTM;
    time_t StarTimeSec=0;
    unsigned int StarTimeSubSec=0;
    char name[256];
    int GrandeEntries = h3_orig->GetEntries();
    
    for(int i=0; i<GrandeEntries; i++){
      h3_orig->GetEntry(i);
      
      Sh3.Fanka = Sh3_orig.Fanka;
      Sh3.Hit7 = Sh3_orig.Hit7;
      Sh3.Gt = Sh3_orig.Gt;
      Sh3.Mmn = Sh3_orig.Mmn;
      
      h3->Fill();
      
      
      event.event_id++;
      
      StarTimeSec = Sh3.Gt;
      StarTimeSubSec = Sh3.Mmn/1000;
      starTimeTM = *localtime(&StarTimeSec);

//to do: GPS time stamp
      sprintf(name,"%i-%02i-%02i %02i:%02i:%02i.%06u", starTimeTM.tm_year+1900, starTimeTM.tm_mon+1,
                    starTimeTM.tm_mday, starTimeTM.tm_hour, starTimeTM.tm_min, starTimeTM.tm_sec, StarTimeSubSec);
      strcpy(event.timestamp,name);
      
      
      Tevent->Fill();

    }
  }
  

    
   int StarTime2=0, StarSubSec2=0;
    
   for(int i=0; i<h3->GetEntries();i++){
      h3->GetEntry(i);
      Tevent->GetEntry(i);

      if(ConvertTimeStamp(event.timestamp,&StarTime2, &StarSubSec2, 0.0)==0) cout << "error3 - timestamp needs check" << endl;
    
      if(   abs(StarTime2 - (int)Sh3.Gt) != 0
  	 || abs(StarSubSec2 - (int)Sh3.Mmn) != 0 ){
  	       cout << "error1 Sec: " << StarTime2 << " " << Sh3.Gt << "\tSubSec: " << StarSubSec2 << " " << Sh3.Mmn;
	       cout << "\t ID: " << event.event_id << " " << event.timestamp << endl;
      }
    
   }
  
  
  Out_file->cd();
  
  h3->Write();
  Tevent->Write();
  Theader->Write();
  Tchannel_profile->Write();
  
  
  Out_file->Close();
  In_file->Close();

}
