#include <iostream>
#include <math.h>

#include <TROOT.h>
#include <TFile.h>
#include <TH2F.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TGraph.h>


#include "util.hh"


using namespace std;

//! run flag output
/*!
makes a loop over a given run and analyse the data. All found results are stored in a output file, which can then be written into the data base
*/



int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();


  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  

  
  char *RunFileName="$HOME/analyisis/default_00.root";
  char *OutputFileName="$HOME/analysis/RunFlag_out.root";
  bool debug = true;

  
  
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "hf:i:")) != -1) //: after var ->Waits for input
    switch (c)
      {
      case 'f':
	OutputFileName = optarg ;
	break;
      case 'i':
	RunFileName = optarg ;
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
	cout << " Overview: run flag output\n";
	cout << "==========================\n\n";
	cout << " -h	HELP -- you already tried :-)\n";
	cout << " -f	output file name (default: " << OutputFileName << ")\n";
	cout << " -i	input file name in STAR format (default: " << RunFileName << ")\n";
	cout << "\n\n\nmailto: asch@ipe.fzk.de\n";
	exit(1);
        break;      
      default:
	abort ();
      }

  

  int NoChannels=0;
  static struct event event;
  static struct header header;
  static struct channel_profiles channel_profiles;

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
  TFile *output = new TFile (OutputFileName,"Recreate","RunFlagOut");
  
///////////////////////////////   
  int START = 0;
 
  unsigned int event_id_start = 0;
   for(int w=0; w<Tevent->GetEntries(); w++){
     Tevent->GetEntry(w);
     START = w;
     if(event.event_id == event_id_start) break;
     if(event.event_id > event_id_start) break;
   }
   Tevent->GetEntry(0);
   if(debug) cout << START << " START" << endl;

 int STOP =  /*START+NoChannels;*/ Tevent->GetEntries();
///////////////////////////////
  
  if(STOP> Tevent->GetEntries()){
   cerr << "entry doesn't exist" << endl;
   STOP = Tevent->GetEntries();
   cerr << "changed end number." <<  endl;
  // exit(1);
  }
  
  if(debug) cout << "STOP " << STOP << endl;
 
  int NoPulse;
  
  //L1Trigger
  float **TriggerTrace = new float *[NoChannels];
  float **RawTrace = new float *[NoChannels];
  for(int i=0; i<NoChannels; i++){
    TriggerTrace[i] = new float [event.window_size];
    RawTrace[i] = new float [event.window_size];
  }
  for(int i=0; i<NoChannels; i++) for(unsigned int j=0; j<event.window_size; j++){
    TriggerTrace[i][j] = 0;
    RawTrace[i][j] = 0;
  }
  
  
  char **ant_pos = new char *[NoChannels];
  for(int i=0; i<NoChannels; i++) ant_pos[i] = new char [15];
  int channel_id[NoChannels];
  for(int i=0; i<NoChannels; i++) channel_id[i] = 0;
  
  static struct AnaFlag AnaFlag;
  
  TTree *TAnaFlag = new TTree("TAnaFlag","analyse flags");
  TAnaFlag->Branch("event_id",&AnaFlag.event_id,"event_id/I");
  TAnaFlag->Branch("DetCh",&AnaFlag.DetCh,"DetCh/I");
  TAnaFlag->Branch("channel_id",&AnaFlag.channel_id,"channel_id[DetCh]/I");
  TAnaFlag->Branch("ana_index",&AnaFlag.ana_index,"ana_index/I");
  TAnaFlag->Branch("description",AnaFlag.description,"description/C");
  TAnaFlag->Branch("run_id",&AnaFlag.run_id,"run_id/I");
  TAnaFlag->Branch("daq_id",&AnaFlag.daq_id,"daq_id/I");
  TAnaFlag->Branch("tot_events",&AnaFlag.tot_events,"tot_events/I");
  TAnaFlag->Branch("PulseLength",&AnaFlag.PulseLength,"PulseLength[DetCh]/F");
  TAnaFlag->Branch("NoPulses",&AnaFlag.NoPulses,"NoPulses[DetCh]/I");
  TAnaFlag->Branch("PulsePosition", &AnaFlag.PulsePosition, "PulsePosition[DetCh]/F");
  TAnaFlag->Branch("PulseMax", &AnaFlag.PulseMax, "PulseMax[DetCh]/F");
  TAnaFlag->Branch("snr",&AnaFlag.snr,"snr[DetCh]/F");
  TAnaFlag->Branch("snr_mean",&AnaFlag.snr_mean,"snr_mean[DetCh]/F");
  TAnaFlag->Branch("snr_rms",&AnaFlag.snr_rms,"snr_rms[DetCh]/F");
  TAnaFlag->Branch("L1trigger",&AnaFlag.L1Trigger,"L1Trigger/O");
  TAnaFlag->Branch("Int_Env",&AnaFlag.Int_Env,"Int_Env[DetCh]/F");
  TAnaFlag->Branch("Int_Pre",&AnaFlag.Int_Pre,"Int_Pre[DetCh]/F");
  TAnaFlag->Branch("Int_Post",&AnaFlag.Int_Post,"Int_Post[DetCh]/F");
  TAnaFlag->Branch("Int_Raw",&AnaFlag.Int_Raw,"Int_Raw[DetCh]/F");
  TAnaFlag->Branch("Int_RawSq",&AnaFlag.Int_RawSq,"Int_RawSq[DetCh]/F");
  TAnaFlag->Branch("Int_Pulse",&AnaFlag.Int_Pulse,"Int_Pulse[DetCh]/F");
  TAnaFlag->Branch("FWHM",&AnaFlag.FWHM,"FWHM[DetCh]/F");
  TAnaFlag->Branch("DynThreshold",&AnaFlag.DynThreshold,"DynThreshold[DetCh]/F");
  TAnaFlag->Branch("CoincTime",&AnaFlag.CoincTime,"CoincTime/F");
  TAnaFlag->Branch("Azimuth",&AnaFlag.Azimuth,"Azimuth/F");
  TAnaFlag->Branch("Azimuth_err",&AnaFlag.Azimuth_err,"Azimuth_err/F");
  TAnaFlag->Branch("Zenith",&AnaFlag.Zenith,"Zenith/F");    
  TAnaFlag->Branch("Zenith_err",&AnaFlag.Zenith_err,"Zenith_err/F"); 
  TAnaFlag->Branch("BAngle",&AnaFlag.BAngle,"BAngle/F"); 
  TAnaFlag->Branch("timestamp",AnaFlag.timestamp,"timestamp/C");

/************************************************************
 *  don't forget to update util.hh, util.cpp and RunFlagSelection.cpp *
 ************************************************************/  
  
  
  AnaFlag.tot_events = Tevent->GetEntries()/NoChannels;
    
  TTree *AnaChProf = new TTree("Tchannel_profile","global information");
  AnaChProf->Branch("ch_id",&channel_profiles.ch_id,"ch_id/i");
  AnaChProf->Branch("channel_no",&channel_profiles.channel_no,"channel_no/i");
  AnaChProf->Branch("threshold",&channel_profiles.threshold,"threshold/I");
  AnaChProf->Branch("coincidence",&channel_profiles.coincidence,"coincidence/I");
  AnaChProf->Branch("board_address",&channel_profiles.board_address,"board_address/i");
  AnaChProf->Branch("antenna_pos",&channel_profiles.antenna_pos,"antenna_pos/C");
  AnaChProf->Branch("polarisation_id",&channel_profiles.polarisation_id,"polaristation_id/i");
  AnaChProf->Branch("polarisation",channel_profiles.polarisation,"polaristation/C");
  
  for(int i=0; i<NoChannels; i++){
    if(bTeventExtend) Tevent->GetEntry(i);
    else Tchannel_profile->GetEntry(i);
    AnaChProf->Fill();
  }
  
  output->cd();
  AnaChProf->Write();
  
  int SaveCounter = 0;

  char **AntPos = new char *[NoChannels];
  for(int i=0; i<NoChannels; i++) AntPos[i] = new char [10];


  for(int i=START; i<STOP; i += NoChannels){
   if(Tevent->GetEntry(i)==0){
     cerr << "error - Tevent->GetEntry()" << endl;
     //exit(0);
     break;
   }

   AnaFlag.run_id = header.run_id;

   //status output
   printf("\t%i of %i\r",i, STOP);
   if(!debug) fflush (stdout);
   else cout << endl;
   
   if(debug) cout << "id = " << event.event_id << " -  loop index = " << i << " - before analysis" <<  endl;
   
  //temp save of data
  SaveCounter++;
  if(SaveCounter == 1000){
    output->cd();
    TAnaFlag->Write(TAnaFlag->GetName(),TObject::kWriteDelete);
    SaveCounter=0;
  }

   
   AnaFlag.DetCh = 0;
   AnaFlag.L1Trigger = false;
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    if(!TraceCheck(&event)){
      AnaFlag.DetCh++;
      AnaFlag.event_id = event.event_id;
      AnaFlag.ana_index = 1;
      strcpy(AnaFlag.description ,"TraceCheck() - ADC overflow");
      strcpy(AnaFlag.timestamp ,event.timestamp);
      AnaFlag.channel_id[AnaFlag.DetCh-1]=event.channel_profile_id;
      AnaFlag.run_id = header.run_id;
      AnaFlag.daq_id = header.DAQ_id;
      AnaFlag.NoPulses[AnaFlag.DetCh-1] = -1;
      AnaFlag.PulseLength[AnaFlag.DetCh-1] = -1;
      AnaFlag.PulsePosition[AnaFlag.DetCh-1] = -1;
      AnaFlag.PulseMax[AnaFlag.DetCh-1] = -1;
      AnaFlag.snr[AnaFlag.DetCh-1] = -1;
      AnaFlag.snr_mean[AnaFlag.DetCh-1] = -1;
      AnaFlag.snr_rms[AnaFlag.DetCh-1] = -1;
      AnaFlag.Int_Env[AnaFlag.DetCh-1] = -1;
      AnaFlag.Int_Raw[AnaFlag.DetCh-1] = -1;
      AnaFlag.Int_RawSq[AnaFlag.DetCh-1] = -1;
      AnaFlag.Int_Pulse[AnaFlag.DetCh-1] = -1;
      AnaFlag.FWHM[AnaFlag.DetCh-1] = -1;
      AnaFlag.DynThreshold[AnaFlag.DetCh-1] = -1;
      AnaFlag.Azimuth     = -1;
      AnaFlag.Azimuth_err = -1;
      AnaFlag.Zenith      = -1;
      AnaFlag.Zenith_err  = -1;
      AnaFlag.CoincTime = -1;
      AnaFlag.BAngle = -1;
      if(debug) cout << AnaFlag.description << " " << "event id " << AnaFlag.event_id ;
      if(debug) cout << " - channel id " << AnaFlag.channel_id[AnaFlag.DetCh-1] << endl;
      if(debug) cout << "loop index = " << i << endl;
    } //end if
   }//end for
   if(AnaFlag.DetCh>0){
    TAnaFlag->Fill();
    continue;
   }

   


 
  if(1){ 
   float snr;  
   float coincidence_time = 0;
   
   float zenith[2], azimuth[2];
   float PulseLength=0, PulsePosition=0, Int_Env, Int_Pre, Int_Post, PulseMax=0, Int_Pulse;
   float threshold=0;
   int NoZeros = 7;
   
   AnaFlag.L1Trigger = false;
   AnaFlag.DetCh = 0;
   if(debug) cout <<  "event id = " << event.event_id << endl;	
   
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    for(unsigned int a=0; a<event.window_size; a++) RawTrace[j][a] = event.trace[a]; //12bit
    strcpy(ant_pos[j],channel_profiles.antenna_pos);
    channel_id[j] = event.channel_profile_id;
   }
   AnaFlag.event_id = event.event_id;
   for(int j=0; j<NoChannels; j++){

     RFISuppressionCutOff(event.window_size, RawTrace[j], TriggerTrace[j]);

     SNRandNoPulses(event.window_size, TriggerTrace[j], &snr, &NoPulse, debug);
     if(debug) cout << "snr = " << snr << " -- # pulses = " << NoPulse << " -- channel = " << j << " -- " << ant_pos[j] << endl;
     if(NoPulse==1 && snr>0.75){
      DynamicThreshold(event.window_size, TriggerTrace[j], &threshold, NoZeros);
      EstimatePulseParameter(event.window_size, TriggerTrace[j], &PulseLength, &PulsePosition, &PulseMax, &Int_Pre, &Int_Post, &Int_Pulse, threshold, NoZeros);
      if(debug) cout << "Int: Post/Pulse = " << Int_Post/Int_Pulse << " -- pulse length = " << PulseLength << " -- channel = " << j << " -- " << ant_pos[j] << " -- pulse pos = " << PulsePosition << endl;
      Int_Env = Int_Pre + Int_Post;
      //if( Int_Post/Int_Pulse<1.2 && PulseLength<10 )
      {
       AnaFlag.DetCh++;
       AnaFlag.Int_Raw[AnaFlag.DetCh-1] = -1;
       AnaFlag.Int_RawSq[AnaFlag.DetCh-1] = -1;
       AnaFlag.FWHM[AnaFlag.DetCh-1] = -1; 
       AnaFlag.DynThreshold[AnaFlag.DetCh-1] = threshold; 
       AnaFlag.snr[AnaFlag.DetCh-1] = snr;
       AnaFlag.snr_mean[AnaFlag.DetCh-1] = -1;
       AnaFlag.snr_rms[AnaFlag.DetCh-1] = -1;
       AnaFlag.NoPulses[AnaFlag.DetCh-1] = NoPulse;
       AnaFlag.PulseLength[AnaFlag.DetCh-1] = PulseLength;
       AnaFlag.PulsePosition[AnaFlag.DetCh-1] = PulsePosition;
       AnaFlag.PulseMax[AnaFlag.DetCh-1] = PulseMax;       
       AnaFlag.Int_Env[AnaFlag.DetCh-1] = Int_Env;
       AnaFlag.Int_Pre[AnaFlag.DetCh-1] = Int_Pre;
       AnaFlag.Int_Post[AnaFlag.DetCh-1] = Int_Post;
       AnaFlag.Int_Pulse[AnaFlag.DetCh-1] = Int_Pulse;
       AnaFlag.event_id = event.event_id;
       AnaFlag.ana_index = 2;
       AnaFlag.run_id = header.run_id;
       AnaFlag.daq_id = header.DAQ_id;
       strcpy(AnaFlag.timestamp ,event.timestamp);
       strcpy(AnaFlag.description ,"v21, SNR>75 AND NoPulses==1 AND Int_Post/Int_Pulse<1.2 AND PulseLength<10, if L1Trigger==0 OR DetCh>0: L0 with 65degree");
       AnaFlag.channel_id[AnaFlag.DetCh-1]=channel_id[j];
      }
     }
   }

   AnaFlag.Azimuth = -1;
   AnaFlag.Azimuth_err = -1;
   AnaFlag.Zenith = -1;
   AnaFlag.Zenith_err = -1;
   AnaFlag.BAngle = -1;
   AnaFlag.BAngle = -1;
   AnaFlag.CoincTime = -1;
   
   if(AnaFlag.DetCh>1) {
     bool pretrigger = true;
     
     if(AnaFlag.DetCh==2){
       char AntPos2[5];
       char AntPos1[5];
       for(int a=0; a<NoChannels; a++){
         Tevent->GetEntry(i+a);
	 if(event.channel_profile_id == (unsigned int) AnaFlag.channel_id[0]) strcpy(AntPos1,channel_profiles.antenna_pos);
	 if(event.channel_profile_id == (unsigned int) AnaFlag.channel_id[1]) strcpy(AntPos2,channel_profiles.antenna_pos);
	//cout << event.channel_profile_id << " == " << AnaFlag.channel_id[0] << " - " << AnaFlag.channel_id[1] << endl;
	//cout << channel_profiles.antenna_pos << " == " << AntPos1 << " - " << AntPos2 << endl;
       }
       if(strcmp(AntPos1,AntPos2)==0) pretrigger = false;
     }
     
     if(debug){
       if(pretrigger) cout << "pretrigger = true" << endl;
       else cout << "pretrigger = false" << endl;
     }
     
     if(pretrigger){
      for(int j=0; j<NoChannels; j++){
       Tevent->GetEntry(i+j);
       for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
      }
      
      coincidence_time = GetCoincidenceTime(header.DAQ_id, 75);//170.0ns Zenithangle of about 65°
      if(debug) cout << "coinc time = " << coincidence_time << endl;
      AnaFlag.L1Trigger = L0Decide(NoChannels, event.window_size, TriggerTrace, ant_pos, header.DAQ_id, &coincidence_time, NoZeros);
      AnaFlag.CoincTime = coincidence_time;
      if(debug) cout << "detected coinc time = " << coincidence_time << endl;
      if(AnaFlag.L1Trigger){
        RecDirectionSingleArray(NoChannels, event.window_size, TriggerTrace, ant_pos, header.DAQ_id, zenith, azimuth, false);
        AnaFlag.Azimuth = azimuth[0];
        AnaFlag.Azimuth_err = azimuth[1];
        AnaFlag.Zenith = zenith[0];
        AnaFlag.Zenith_err = zenith[1];
	AnaFlag.BAngle = GeoMagAngle(AnaFlag.Azimuth, AnaFlag.Zenith, header.DAQ_id);
        if(debug) cout <<  "event id = " << event.event_id << endl;	
        if(debug) cout << "Azimuth " << azimuth[0] << " -+ " << azimuth[1]  << " - Zenith " << zenith[0] << " -+ " << zenith[1] << endl << endl;
      }
    }
   }
   
   if( AnaFlag.DetCh>0 || AnaFlag.L1Trigger ){
    TAnaFlag->Fill();
    continue;
   }
  }



  
  
 } // end for over all traces and events
  
  cout << endl;
  


  output->cd();
  TAnaFlag->Write(TAnaFlag->GetName(),TObject::kWriteDelete);
  output->Close();

  //do not forget to clean up
  delete output;

//  delete AnaChProf;
//  delete TAnaFlag;

  delete Tevent;
  delete Theader;
  delete Tchannel_profile;
  
}
