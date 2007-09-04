#include <iostream>
#include <stdlib.h>

#include <TROOT.h>
#include <TFile.h>
#include <TH2F.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TEventList.h>
#include <TTree.h>


#include <LopesStar/util.h>

using namespace std;

//! run flag selection
/*!
You can analyse your run files and create a RunFlagOut File. With this programm you can make a sup run file with cuts on the flaged output file. If everything works fine with the flagging, you can wirte your results into the data base with the RunFlagInDB.
*/



int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();


  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  

  
  char *RunFileName="$HOME/analyisis/default_00.root";
  char *OutputFileName="$HOME/analysis/RunFlagSelection_default";
  char *AnaFlagFileName="$HOME/analysis/AF_default.root";
  char CutSelection[2048];
  sprintf(CutSelection,"ana_index==0");
  UInt_t OutputCompression = 6;
  bool debug = false;

  
  
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "hf:o:r:c:")) != -1) //: after var ->Waits for input
    switch (c)
      {
      case 'o':
	OutputFileName = optarg ;
	break;
      case 'r':
	RunFileName = optarg ;
	break;
      case 'f':
	AnaFlagFileName = optarg ;
	break;
      case 'c':
	sprintf(CutSelection,optarg) ;
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
	cout << " Overview: run flag selection\n";
	cout << "=============================\n\n";
	cout << " -h	HELP -- you already tried :-)\n";
	cout << " -o	output file name without extension (default: " << OutputFileName << ")\n";
	cout << " -r	run input file name in STAR format (default: " << RunFileName << ")\n";
	cout << " -f	ana flag file of the analysed run (default: " << AnaFlagFileName << ")\n";
	cout << " -c	cut selection on TAnaFlag tree, do NOT forget to use \"...\" or \'...\'\n";
	cout << "	(default: " << CutSelection << ")\n";
	cout << "	The default ana_index for not classified events is ana_index = 0\n";
	cout << "\n\n\nmailto: asch@ipe.fzk.de\n";
	exit(1);
        break;      
      default:
	abort ();
      }

  
  //init RunFile
  int NoChannels=0;
  struct event event;
  struct header header;
  struct channel_profiles channel_profiles;

  TChain *Theader = new TChain("Theader");
  TChain *Tchannel_profile = new TChain("Tchannel_profile");
  TChain *Tevent = new TChain("Tevent");
  CreateTChain(Tevent, RunFileName);
  
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
    Theader->Add(RunFileName);
    Tchannel_profile->Add(RunFileName);
  
    CreateTevent(Tevent, &event);
    CreateTheader(Theader, &header);
    CreateTchannel_profiles(Tchannel_profile, &channel_profiles);

    Tevent->GetEntry(0);
    Theader->GetEntry(0);
    NoChannels = Tchannel_profile->GetEntries();
  }

  if(debug) cout << "NoChannels = " << NoChannels << endl;
  

  //init RunFlagFile
  TChain *TAnaFlag = new TChain("TAnaFlag");
  TAnaFlag->Add(AnaFlagFileName);
  struct AnaFlag AnaFlag;
  CreateTAnaFlag(TAnaFlag,&AnaFlag);
  TAnaFlag->GetEntry(0);
  
  
  //check if RunFlag File corresponds to RunFile
  if(header.DAQ_id != AnaFlag.daq_id || header.run_id != (unsigned int) AnaFlag.run_id){
    cerr << "daq ID or run ID not the same .. wrong files?" << endl;
//    exit(1);
  }
  
 
  //init VOutputFiles
  char OName[1024];
  time_t zeit;
  time(&zeit);
  struct tm * tmzeit;
  tmzeit = gmtime(&zeit);
  sprintf(OName,"/tmp/vanaflag_%04d-%02d-%02d_%02d:%02d:%02d.root",tmzeit->tm_year+1900, tmzeit->tm_mon+1, tmzeit->tm_mday, tmzeit->tm_hour, tmzeit->tm_min, tmzeit->tm_sec);
  TFile Voutput (OName,"Recreate","VRunFlagSelection_tmp");
  Voutput.cd();
  if(debug) cout << "init Voutput file ... done" << endl;


  //init Virtual RunFlagTree: VTAnaFlag
  struct AnaFlag VAnaFlag;
  struct AnaFlag ZeroVAnaFlag;
  if(debug) cout << "ZeroVAnaFlag->event_id = " << ZeroVAnaFlag.event_id << endl;
  TTree *VTAnaFlag = new TTree("VTAnaFlag","VTAnaFlag");
  VTAnaFlag->Branch("event_id",&VAnaFlag.event_id,"event_id/I");
  VTAnaFlag->Branch("DetCh",&VAnaFlag.DetCh,"DetCh/I");
  VTAnaFlag->Branch("channel_id",&VAnaFlag.channel_id,"channel_id[DetCh]/I");
  VTAnaFlag->Branch("ana_index",&VAnaFlag.ana_index,"ana_index/I");
  VTAnaFlag->Branch("description",VAnaFlag.description,"description/C");
  VTAnaFlag->Branch("run_id",&VAnaFlag.run_id,"run_id/I");
  VTAnaFlag->Branch("daq_id",&VAnaFlag.daq_id,"daq_id/I");
  VTAnaFlag->Branch("tot_events",&VAnaFlag.tot_events,"tot_events/I");
  VTAnaFlag->Branch("NoPulses",&VAnaFlag.NoPulses,"NoPulses[DetCh]/I");
  VTAnaFlag->Branch("PulseLength",&VAnaFlag.PulseLength,"PulseLength[DetCh]/F");
  VTAnaFlag->Branch("PulsePosition",&VAnaFlag.PulsePosition,"PulsePosition[DetCh]/F");
  VTAnaFlag->Branch("PulseMax",&VAnaFlag.PulseMax,"PulseMax[DetCh]/F");  
  VTAnaFlag->Branch("snr",&VAnaFlag.snr,"snr[DetCh]/F");
  VTAnaFlag->Branch("snr_mean",&VAnaFlag.snr_mean,"snr_mean[DetCh]/F");
  VTAnaFlag->Branch("snr_rms",&VAnaFlag.snr_rms,"snr_rms[DetCh]/F");
  VTAnaFlag->Branch("L1trigger",&VAnaFlag.L1Trigger,"L1Trigger/O");
  VTAnaFlag->Branch("Int_Env",&AnaFlag.Int_Env,"Int_Env[DetCh]/F");
  VTAnaFlag->Branch("Int_Raw",&AnaFlag.Int_Raw,"Int_Raw[DetCh]/F");
  VTAnaFlag->Branch("Int_RawSq",&AnaFlag.Int_RawSq,"Int_RawSq[DetCh]/F");
  VTAnaFlag->Branch("FWHM",&AnaFlag.FWHM,"FWHM[DetCh]/F");
  VTAnaFlag->Branch("DynThreshold",&AnaFlag.DynThreshold,"DynThreshold[DetCh]/F");
  VTAnaFlag->Branch("CoincTime",&AnaFlag.CoincTime,"CoincTime/F");
  VTAnaFlag->Branch("Azimuth",&AnaFlag.Azimuth,"Azimuth/F");
  VTAnaFlag->Branch("Azimuth_err",&AnaFlag.Azimuth_err,"Azimuth_err/F");
  VTAnaFlag->Branch("Zenith",&AnaFlag.Zenith,"Zenith/F");    
  VTAnaFlag->Branch("Zenith_err",&AnaFlag.Zenith_err,"Zenith_err/F"); 


/**********************************************
 * Set all branches up, you want to cut at.   *
 * => here and a few lines down.              *
 * Don't forget to update util.h + utill.cc   *
 *********************************************/  
  

  //fill VTAnaFlagTree
  int AnaFlagEntry=0;
  int AnaFlagEntrySame=0;
  int VTAnaFlagTreeCount = 0;
  for(int i=0; i<Tevent->GetEntries(); i++){
    VTAnaFlagTreeCount++;
    Tevent->GetEntry(i);
    TAnaFlag->GetEntry(AnaFlagEntry);
    while(event.event_id != (unsigned int)AnaFlag.event_id  && (unsigned int) AnaFlag.event_id<=event.event_id){
      AnaFlagEntry++;
      if(TAnaFlag->GetEntry(AnaFlagEntry)==0)break;
    }//end while over AnaFlagTree
    
    if(event.event_id == (unsigned int)AnaFlag.event_id){
      VAnaFlag = AnaFlag;
      VTAnaFlag->Fill();
      AnaFlagEntrySame = AnaFlagEntry;
    }
    else{
      VAnaFlag = ZeroVAnaFlag;
      VTAnaFlag->Fill();
      AnaFlagEntry=AnaFlagEntrySame;
    }
    if(VTAnaFlagTreeCount==100000){
      if(debug) cout << "write VTAnaFlag to tmp file" << endl;
      VTAnaFlagTreeCount=0;
      VTAnaFlag->Write(VTAnaFlag->GetName(),TObject::kWriteDelete);
      delete VTAnaFlag;
      VTAnaFlag = (TTree*)Voutput.Get("VTAnaFlag");
      VTAnaFlag->SetBranchAddress("event_id",&VAnaFlag.event_id);
      VTAnaFlag->SetBranchAddress("DetCh",&VAnaFlag.DetCh);
      VTAnaFlag->SetBranchAddress("channel_id",&VAnaFlag.channel_id);
      VTAnaFlag->SetBranchAddress("ana_index",&VAnaFlag.ana_index);
      VTAnaFlag->SetBranchAddress("description",VAnaFlag.description);
      VTAnaFlag->SetBranchAddress("run_id",&VAnaFlag.run_id);
      VTAnaFlag->SetBranchAddress("daq_id",&VAnaFlag.daq_id);
      VTAnaFlag->SetBranchAddress("tot_events",&VAnaFlag.tot_events);
      VTAnaFlag->SetBranchAddress("NoPulses",&VAnaFlag.NoPulses);
      VTAnaFlag->SetBranchAddress("PulseLength",&VAnaFlag.PulseLength);
      VTAnaFlag->SetBranchAddress("PulsePosition",&VAnaFlag.PulsePosition);
      VTAnaFlag->SetBranchAddress("PulseMax",&VAnaFlag.PulseMax);      
      VTAnaFlag->SetBranchAddress("snr",&VAnaFlag.snr);
      VTAnaFlag->SetBranchAddress("snr_mean",&VAnaFlag.snr_mean);
      VTAnaFlag->SetBranchAddress("snr_rms",&VAnaFlag.snr_rms);
      VTAnaFlag->SetBranchAddress("L1trigger",&VAnaFlag.L1Trigger);
      VTAnaFlag->SetBranchAddress("Int_Env",&AnaFlag.Int_Env);
      VTAnaFlag->SetBranchAddress("Int_Raw",&AnaFlag.Int_Raw);
      VTAnaFlag->SetBranchAddress("Int_RawSq",&AnaFlag.Int_RawSq);
      VTAnaFlag->SetBranchAddress("FWHM",&AnaFlag.FWHM);
      VTAnaFlag->SetBranchAddress("DynThreshold",&AnaFlag.DynThreshold);
      VTAnaFlag->SetBranchAddress("CoincTime",&AnaFlag.CoincTime);
      VTAnaFlag->SetBranchAddress("Azimuth",&AnaFlag.Azimuth);
      VTAnaFlag->SetBranchAddress("Azimuth_err",&AnaFlag.Azimuth_err);
      VTAnaFlag->SetBranchAddress("Zenith",&AnaFlag.Zenith);    
      VTAnaFlag->SetBranchAddress("Zenith_err",&AnaFlag.Zenith_err); 


    }
  }//end for over Tevents
  
  cout << "VTAnaFlag ready" << endl;
  

  //save VTAnaFlag at /tmp/
  Voutput.cd();
  VTAnaFlag->Write(VTAnaFlag->GetName(),TObject::kWriteDelete);
  Voutput.Close();
  if(debug) cout << "written to Voutput file" << endl;
  
  
  //add friend tree to get a TEventList
  Tevent->AddFriend("VTAnaFlag",OName);
  Tevent->Draw(">>myList",CutSelection);
  TEventList *EventList = (TEventList*) gDirectory->Get("myList");
  if(EventList==NULL) cout << "error - TEventList " << endl;
  Tevent->SetEventList(EventList);
  if(debug) cout << "TEventList entries = " << EventList->GetN() << endl;
  
  
  //init Toutput file
  char Name[1024]; 
  sprintf(Name,"%s_00.root",OutputFileName);
  TFile tmpfile (RunFileName,"READ");
  OutputCompression = tmpfile.GetCompressionLevel();
  tmpfile.Close();
  TFile *output = new TFile (Name,"Recreate","RunFlagSelection",OutputCompression);

  
  //init output trees for new file
  struct header Oheader;
  struct channel_profiles Ochannel_profiles;
  struct event Oevent;
  
  TTree *OTevent = new TTree("Tevent","event and their samples");
  TTree *OCutInfo = new TTree("CutInfo","information about the selection cut");

  OTevent->SetAutoSave();
  OTevent->SetMaxTreeSize();
  OTevent->Branch("event_id",&Oevent.event_id,"event_id/i");
  OTevent->Branch("timestamp",&Oevent.timestamp,"timestamp/C");
  OTevent->Branch("timestamp_db",&Oevent.timestamp_db,"timestamp_db/C");
  OTevent->Branch("leap_second",&Oevent.leap_second,"leap_second/O");
  OTevent->Branch("window_size",&Oevent.window_size,"window_size/i");
  OTevent->Branch("delta_t",&Oevent.delta_t,"delta_t/i");
  OTevent->Branch("channel_profile_id",&Oevent.channel_profile_id,"channel_profile_id/i");
  OTevent->Branch("sample_id",&Oevent.sample_id,"sample_id/C");
  OTevent->Branch("trace",&Oevent.trace,"trace[window_size]/s");
  OTevent->Branch("run_id",&Oheader.run_id,"run_id/i");
  OTevent->Branch("start_time",&Oheader.start_time,"start_time/C");
  OTevent->Branch("stop_time",&Oheader.stop_time,"stop_time/C");
  OTevent->Branch("profile_id",&Oheader.profile_id,"profile_id/i");
  OTevent->Branch("clock_frequency",&Oheader.clock_frequency,"clock_frequency/i");
  OTevent->Branch("posttrigger",&Oheader.posttrigger,"posttrigger/i");
  OTevent->Branch("DAQ_id",&Oheader.DAQ_id,"DAQ_id/S");
  OTevent->Branch("gps_position",&Oheader.gps_position,"gps_position/C");
  OTevent->Branch("profile_name",&Oheader.profile_name,"profile_name/C");
  OTevent->Branch("ch_id",&Ochannel_profiles.ch_id,"ch_id/i");
  OTevent->Branch("channel_no",&Ochannel_profiles.channel_no,"channel_no/i");
  OTevent->Branch("threshold",&Ochannel_profiles.threshold,"threshold/I");
  OTevent->Branch("coincidence",&Ochannel_profiles.coincidence,"coincidence/I");
  OTevent->Branch("board_address",&Ochannel_profiles.board_address,"board_address/i");
  OTevent->Branch("antenna_pos",&Ochannel_profiles.antenna_pos,"antenna_pos/C");
  OTevent->Branch("polarisation_id",&Ochannel_profiles.polarisation_id,"polaristation_id/i");
  OTevent->Branch("polarisation",Ochannel_profiles.polarisation,"polaristation/C");
 
  OCutInfo->SetAutoSave();
  OCutInfo->Branch("CutSelection",&CutSelection, "CutSelection/C");
  OCutInfo->Fill();

  //loop over Event List to fill output trees
  int FileEventCounter = 0;
  int FileCounter = 0;
  
  cout << "filling " << Name << " ..." << endl;

  if(EventList->GetN()>0)for(int i=0; i<EventList->GetN(); i++){
    Tevent->GetEntry(EventList->GetEntry(i));
    
    if(!bTeventExtend){
     int ChId=0;
     Tchannel_profile->GetEntry(ChId);
     while( event.channel_profile_id != channel_profiles.ch_id ){
       ChId++;
       if(Tchannel_profile->GetEntry(ChId)==0){
         cerr << "error - Tchannel_profile->GetEntry()" << endl;
         exit(0);
       }
     }
     Theader->GetEntry(0);
    }
    Oevent = event;
    Oheader = header;
    Ochannel_profiles = channel_profiles;
    OTevent->Fill();
    
    FileEventCounter++;
    if(FileEventCounter == 10000){
      FileEventCounter=0;
      output->cd();
      OTevent->Write(OTevent->GetName(),TObject::kWriteDelete);
      delete OTevent;
      if(output->GetSize()>1850000000){
        if(debug) cout << "File Counter = " << FileCounter << endl;
        FileCounter++;
	output->Close();
	sprintf(Name,"%s_%02i.root",OutputFileName,FileCounter);
	output = new TFile(Name,"Recreate","RunFlagSelection",OutputCompression);
	OTevent = new TTree("Tevent","event and their samples");
	OTevent->SetAutoSave();
        OTevent->SetMaxTreeSize();
        OTevent->Branch("event_id",&Oevent.event_id,"event_id/i");
        OTevent->Branch("timestamp",&Oevent.timestamp,"timestamp/C");
        OTevent->Branch("timestamp_db",&Oevent.timestamp_db,"timestamp_db/C");
        OTevent->Branch("leap_second",&Oevent.leap_second,"leap_second/O");
        OTevent->Branch("window_size",&Oevent.window_size,"window_size/i");
        OTevent->Branch("delta_t",&Oevent.delta_t,"delta_t/i");
        OTevent->Branch("channel_profile_id",&Oevent.channel_profile_id,"channel_profile_id/i");
        OTevent->Branch("sample_id",&Oevent.sample_id,"sample_id/C");
        OTevent->Branch("trace",&Oevent.trace,"trace[window_size]/s");
        OTevent->Branch("run_id",&Oheader.run_id,"run_id/i");
        OTevent->Branch("start_time",&Oheader.start_time,"start_time/C");
        OTevent->Branch("stop_time",&Oheader.stop_time,"stop_time/C");
        OTevent->Branch("profile_id",&Oheader.profile_id,"profile_id/i");
        OTevent->Branch("clock_frequency",&Oheader.clock_frequency,"clock_frequency/i");
        OTevent->Branch("posttrigger",&Oheader.posttrigger,"posttrigger/i");
        OTevent->Branch("DAQ_id",&Oheader.DAQ_id,"DAQ_id/S");
        OTevent->Branch("gps_position",&Oheader.gps_position,"gps_position/C");
        OTevent->Branch("profile_name",&Oheader.profile_name,"profile_name/C");
        OTevent->Branch("ch_id",&Ochannel_profiles.ch_id,"ch_id/i");
        OTevent->Branch("channel_no",&Ochannel_profiles.channel_no,"channel_no/i");
        OTevent->Branch("threshold",&Ochannel_profiles.threshold,"threshold/I");
        OTevent->Branch("coincidence",&Ochannel_profiles.coincidence,"coincidence/I");
        OTevent->Branch("board_address",&Ochannel_profiles.board_address,"board_address/i");
        OTevent->Branch("antenna_pos",&Ochannel_profiles.antenna_pos,"antenna_pos/C");
        OTevent->Branch("polarisation_id",&Ochannel_profiles.polarisation_id,"polaristation_id/i");
        OTevent->Branch("polarisation",Ochannel_profiles.polarisation,"polaristation/C");
	
	output->cd();
	OTevent->Write(OTevent->GetName(),TObject::kWriteDelete);
	delete OTevent;
        cout << "filling " << Name << " ..." << endl;
      }
      OTevent = (TTree*)output->Get("Tevent");
      CreateTevent(OTevent,&Oevent); 
      CreateTeventExtend(OTevent, &Oheader, &Ochannel_profiles);
    }
  }//end for over EventList entries
  else cout << "TEventList entries are zero, no output file is written" << endl;
  

  output->cd();
  OTevent->Write(OTevent->GetName(),TObject::kWriteDelete);
  OCutInfo->Write();
  output->Close();
  delete output;
  
  
  sprintf(Name,"rm %s",OName);
  if(system(Name)==1) cerr << "could not delete " << OName << " file ... exit" << endl;
  if(debug) cout << Name << endl;
  
  delete Theader;
  delete Tchannel_profile;
  delete Tevent;
//  delete TAnaFlag;
//  delete OCutInfo;

}
