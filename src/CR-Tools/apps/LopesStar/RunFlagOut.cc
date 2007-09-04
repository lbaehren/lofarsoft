#include <iostream>
#include <cmath>

#include <TROOT.h>
#include <TFile.h>
#include <TH2F.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TGraph.h>


#include <LopesStar/util.h>


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
  bool debug = false;

  
  
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

 int STOP =  Tevent->GetEntries();
///////////////////////////////
  
  if(STOP> Tevent->GetEntries()){
   cerr << "entry doesn't exist" << endl;
   STOP = Tevent->GetEntries();
   cerr << "changed end number." <<  endl;
  // exit(1);
  }
  
  if(debug) cout << "STOP " << STOP << endl;
 
  float Trace[event.window_size];
  double Mean, Sigma, Max;
  int NoPulse;
  
  //L1Trigger
  float **TriggerTrace = new float *[NoChannels];
  for(int i=0; i<NoChannels; i++) TriggerTrace[i] = new float [event.window_size];
  for(int i=0; i<NoChannels; i++) for(unsigned int j=0; j<event.window_size; j++) TriggerTrace[i][j]=0;
  char **ant_pos = new char *[NoChannels];
  for(int i=0; i<NoChannels; i++) ant_pos[i] = new char [15];
  int channel_id[NoChannels];
  for(int i=0; i<NoChannels; i++) channel_id[i] = 0;
  
  struct AnaFlag AnaFlag;
  
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
  TAnaFlag->Branch("Int_Raw",&AnaFlag.Int_Raw,"Int_Raw[DetCh]/F");
  TAnaFlag->Branch("Int_RawSq",&AnaFlag.Int_RawSq,"Int_RawSq[DetCh]/F");
  TAnaFlag->Branch("FWHM",&AnaFlag.FWHM,"FWHM[DetCh]/F");
  TAnaFlag->Branch("DynThreshold",&AnaFlag.DynThreshold,"DynThreshold[DetCh]/F");
  TAnaFlag->Branch("CoincTime",&AnaFlag.CoincTime,"CoincTime/F");
  TAnaFlag->Branch("Azimuth",&AnaFlag.Azimuth,"Azimuth/F");
  TAnaFlag->Branch("Azimuth_err",&AnaFlag.Azimuth_err,"Azimuth_err/F");
  TAnaFlag->Branch("Zenith",&AnaFlag.Zenith,"Zenith/F");    
  TAnaFlag->Branch("Zenith_err",&AnaFlag.Zenith_err,"Zenith_err/F"); 

/************************************************************
 *  don't forget to update util.h and RunFlagSelection.cc   *
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
      AnaFlag.FWHM[AnaFlag.DetCh-1] = -1;
      AnaFlag.DynThreshold[AnaFlag.DetCh-1] = -1;
      AnaFlag.Azimuth     = -1;
      AnaFlag.Azimuth_err = -1;
      AnaFlag.Zenith      = -1;
      AnaFlag.Zenith_err  = -1;
      AnaFlag.CoincTime = -1;
      if(debug) cout << AnaFlag.description << " " << "event id " << AnaFlag.event_id ;
      if(debug) cout << " - channel id " << AnaFlag.channel_id[AnaFlag.DetCh-1] << endl;
      if(debug) cout << "loop index = " << i << endl;
    } //end if
   }//end for
   if(AnaFlag.DetCh>0){
    TAnaFlag->Fill();
    continue;
   }

   
   //full analysis
  if(0){ 
   AnaFlag.DetCh = 0;
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    AnaFlag.DetCh++;
    AnaFlag.event_id = event.event_id;
    AnaFlag.ana_index = 2;
    strcpy(AnaFlag.description ,"full parameter calculation");
    AnaFlag.channel_id[AnaFlag.DetCh-1]=event.channel_profile_id;

    for(unsigned int a=0; a<event.window_size; a++) Trace[a] = event.trace[a];
    RFISuppression(event.window_size, Trace, Trace);
    SNR(event.window_size, Trace, &AnaFlag.snr[AnaFlag.DetCh-1], &AnaFlag.snr_mean[AnaFlag.DetCh-1], &AnaFlag.snr_rms[AnaFlag.DetCh-1]);
    NoPulse = PulseCount(event.window_size, Trace, &Mean, &Sigma, &Max);
    AnaFlag.NoPulses[AnaFlag.DetCh-1] = NoPulse;
    AnaFlag.PulseLength[AnaFlag.DetCh-1] = -1;
    AnaFlag.PulsePosition[AnaFlag.DetCh-1] = -1;
    if(NoPulse>0 && NoPulse<5){
      PulseLengthEstimate(event.window_size, Trace, &AnaFlag.PulseLength[AnaFlag.DetCh-1], &AnaFlag.PulsePosition[AnaFlag.DetCh-1], &AnaFlag.Int_Env[AnaFlag.DetCh-1], false);
    }

   }//end for
   if(AnaFlag.DetCh>0){
    TAnaFlag->Fill();
    continue;
   }
  }
  
  
  //v08
  //L1Trigger Decision
  AnaFlag.L1Trigger = false;
  if(0){ 
   AnaFlag.DetCh = 0;
   float coincidence_time = 180;
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
    strcpy(ant_pos[j],channel_profiles.antenna_pos);
   }
   AnaFlag.event_id = event.event_id;
   AnaFlag.L1Trigger = L1Decide(NoChannels, event.window_size, TriggerTrace, ant_pos, header.DAQ_id, &coincidence_time);
  
   AnaFlag.DetCh = 0;
   float snr, snr_rms, snr_mean;
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    for(unsigned int a=0; a<event.window_size; a++) Trace[a] = event.trace[a];
    RFISuppression(event.window_size, Trace, Trace);
    SNR(event.window_size, Trace, &snr, &snr_mean, &snr_rms);
    NoPulse = PulseCount(event.window_size, Trace, &Mean, &Sigma, &Max);
    if(NoPulse>0 && NoPulse<5 && snr*snr_rms/snr_mean>300){
      AnaFlag.DetCh++;
      AnaFlag.snr[AnaFlag.DetCh-1] = snr;
      AnaFlag.snr_mean[AnaFlag.DetCh-1] = snr_mean;
      AnaFlag.snr_rms[AnaFlag.DetCh-1] = snr_rms;
      AnaFlag.NoPulses[AnaFlag.DetCh-1] = NoPulse;
      AnaFlag.event_id = event.event_id;
      AnaFlag.ana_index = 2;
      strcpy(AnaFlag.description ,"v08, RFISupp, SNR*RMS/MEAN>300 AND 0<NoPulses<5 AND L1Trigger Decision");
      AnaFlag.channel_id[AnaFlag.DetCh-1]=event.channel_profile_id;
      PulseLengthEstimate(event.window_size, Trace, &AnaFlag.PulseLength[AnaFlag.DetCh-1], &AnaFlag.PulsePosition[AnaFlag.DetCh-1], &AnaFlag.Int_Env[AnaFlag.DetCh-1], false);
    }
   }//end for
 
   if(AnaFlag.DetCh>0 || AnaFlag.L1Trigger){
    TAnaFlag->Fill();
    continue;
   }
  }
   
  //v09
  if(0){ 
   float snr, snr_rms, snr_mean;  
   AnaFlag.L1Trigger = false;
   AnaFlag.DetCh = 0;
   float coincidence_time = 180;
   
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
    strcpy(ant_pos[j],channel_profiles.antenna_pos);
    channel_id[j] = event.channel_profile_id;
   }
   AnaFlag.event_id = event.event_id;
   AnaFlag.L1Trigger = L1Decide(NoChannels, event.window_size, TriggerTrace, ant_pos, header.DAQ_id, &coincidence_time, false);
   
   if(AnaFlag.L1Trigger){
    for(int j=0; j<NoChannels; j++){
     SNR(event.window_size, TriggerTrace[j], &snr, &snr_mean, &snr_rms);
     NoPulse = PulseCount(event.window_size, TriggerTrace[j], &Mean, &Sigma, &Max);
     if(NoPulse>0 && NoPulse<5 && snr>75){
       AnaFlag.DetCh++;
       AnaFlag.snr[AnaFlag.DetCh-1] = snr;
       AnaFlag.snr_mean[AnaFlag.DetCh-1] = snr_mean;
       AnaFlag.snr_rms[AnaFlag.DetCh-1] = snr_rms;
       AnaFlag.NoPulses[AnaFlag.DetCh-1] = NoPulse;
       AnaFlag.event_id = event.event_id;
       AnaFlag.ana_index = 2;
       strcpy(AnaFlag.description ,"v09, RFISupp, L1Decide, SNR>75 AND 0<NoPulses<5");
       AnaFlag.channel_id[AnaFlag.DetCh-1]=channel_id[j];
       PulseLengthEstimate(event.window_size, TriggerTrace[j], &AnaFlag.PulseLength[AnaFlag.DetCh-1], &AnaFlag.PulsePosition[AnaFlag.DetCh-1], &AnaFlag.Int_Env[AnaFlag.DetCh-1], false);
     }
    }//end for
   }
   if(AnaFlag.DetCh>0){
    TAnaFlag->Fill();
    continue;
   }
  }

  //v10
  if(0){ 
   float snr, snr_rms, snr_mean;  
   AnaFlag.L1Trigger = false;
   AnaFlag.DetCh = 0;
   float coincidence_time = 180;
   
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
    strcpy(ant_pos[j],channel_profiles.antenna_pos);
    channel_id[j] = event.channel_profile_id;
   }
   AnaFlag.event_id = event.event_id;
   for(int j=0; j<NoChannels; j++){
     RFISuppression(event.window_size, TriggerTrace[j], TriggerTrace[j]);
     SNR(event.window_size, TriggerTrace[j], &snr, &snr_mean, &snr_rms);
     NoPulse = PulseCount(event.window_size, TriggerTrace[j], &Mean, &Sigma, &Max);
     if(NoPulse>0 && NoPulse<5 && snr>75){
       AnaFlag.DetCh++;
       AnaFlag.snr[AnaFlag.DetCh-1] = snr;
       AnaFlag.snr_mean[AnaFlag.DetCh-1] = snr_mean;
       AnaFlag.snr_rms[AnaFlag.DetCh-1] = snr_rms;
       AnaFlag.NoPulses[AnaFlag.DetCh-1] = NoPulse;
       AnaFlag.event_id = event.event_id;
       AnaFlag.ana_index = 2;
       strcpy(AnaFlag.description ,"v10, RFISupp, SNR>75 AND 0<NoPulses<5 -> DetCh>1: L1Decide");
       AnaFlag.channel_id[AnaFlag.DetCh-1]=channel_id[j];
       PulseLengthEstimate(event.window_size, TriggerTrace[j], &AnaFlag.PulseLength[AnaFlag.DetCh-1], &AnaFlag.PulsePosition[AnaFlag.DetCh-1], &AnaFlag.Int_Env[AnaFlag.DetCh-1], false);
     }
   }
   if(AnaFlag.DetCh>1) AnaFlag.L1Trigger = L1Decide(NoChannels, event.window_size, TriggerTrace, ant_pos, header.DAQ_id, &coincidence_time, false);   
   if(AnaFlag.L1Trigger){
    TAnaFlag->Fill();
    continue;
   }
  }

  //v11
  if(0){ 
   AnaFlag.L1Trigger = false;
   float coincidence_time = 180;
   AnaFlag.DetCh = 0;
   float snr, snr_rms, snr_mean;  
   
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
    strcpy(ant_pos[j],channel_profiles.antenna_pos);
    channel_id[j] = event.channel_profile_id;
   }
   
   AnaFlag.L1Trigger = L0Decide(NoChannels, event.window_size, TriggerTrace, ant_pos, header.DAQ_id, &coincidence_time);
   if(AnaFlag.L1Trigger){
    for(int j=0; j<NoChannels; j++){
       AnaFlag.DetCh++;
       RFISuppression(event.window_size, TriggerTrace[j], TriggerTrace[j]);
       SNR(event.window_size, TriggerTrace[j], &snr, &snr_mean, &snr_rms);
       PulseLengthEstimate(event.window_size, TriggerTrace[j], &AnaFlag.PulseLength[AnaFlag.DetCh-1], &AnaFlag.PulsePosition[AnaFlag.DetCh-1], &AnaFlag.Int_Env[AnaFlag.DetCh-1], false);
       AnaFlag.event_id = event.event_id;
       AnaFlag.snr[AnaFlag.DetCh-1] = snr;
       AnaFlag.snr_mean[AnaFlag.DetCh-1] = snr_mean;
       AnaFlag.snr_rms[AnaFlag.DetCh-1] = snr_rms;
       AnaFlag.NoPulses[AnaFlag.DetCh-1] = PulseCount(event.window_size, TriggerTrace[j], &Mean, &Sigma, &Max);
       AnaFlag.event_id = event.event_id;
       AnaFlag.ana_index = 2;
       strcpy(AnaFlag.description ,"v11, L0Decide with coincidence time of 180ns, L1Trigger=L0Trigger");
       AnaFlag.channel_id[AnaFlag.DetCh-1]=channel_id[j];
    }
       TAnaFlag->Fill();
       continue;
   }
  }
  
  //v12
  if(0){ 
   float snr, snr_rms, snr_mean;  
   float coincidence_time = 180;
   AnaFlag.L1Trigger = false;
   AnaFlag.DetCh = 0;
   
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
    strcpy(ant_pos[j],channel_profiles.antenna_pos);
    channel_id[j] = event.channel_profile_id;
   }
   AnaFlag.event_id = event.event_id;
   for(int j=0; j<NoChannels; j++){
     RFISuppression(event.window_size, TriggerTrace[j], TriggerTrace[j]);
     SNR(event.window_size, TriggerTrace[j], &snr, &snr_mean, &snr_rms);
     NoPulse = PulseCount(event.window_size, TriggerTrace[j], &Mean, &Sigma, &Max);
     if(NoPulse>0 && NoPulse<5 && snr>75){
       AnaFlag.DetCh++;
       AnaFlag.snr[AnaFlag.DetCh-1] = snr;
       AnaFlag.snr_mean[AnaFlag.DetCh-1] = snr_mean;
       AnaFlag.snr_rms[AnaFlag.DetCh-1] = snr_rms;
       AnaFlag.NoPulses[AnaFlag.DetCh-1] = NoPulse;
       AnaFlag.event_id = event.event_id;
       AnaFlag.ana_index = 2;
       strcpy(AnaFlag.description ,"v12, RFISupp, SNR>75 AND 0<NoPulses<5 -> DetCh>1: L0Decide, L1Trigger=L0Trigger");
       AnaFlag.channel_id[AnaFlag.DetCh-1]=channel_id[j];
       PulseLengthEstimate(event.window_size, TriggerTrace[j], &AnaFlag.PulseLength[AnaFlag.DetCh-1], &AnaFlag.PulsePosition[AnaFlag.DetCh-1], &AnaFlag.Int_Env[AnaFlag.DetCh-1], false);
     }
   }
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
   }

   if(AnaFlag.DetCh>1) AnaFlag.L1Trigger = L0Decide(NoChannels, event.window_size, TriggerTrace, ant_pos, header.DAQ_id, &coincidence_time);
   if(AnaFlag.L1Trigger){
    TAnaFlag->Fill();
    continue;
   }
  }
 
  
  //v13 & v14
  if(0){ 
   float snr, snr_rms, snr_mean;  
   float coincidence_time = 0;
   AnaFlag.L1Trigger = false;
   AnaFlag.DetCh = 0;
   
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
    strcpy(ant_pos[j],channel_profiles.antenna_pos);
    channel_id[j] = event.channel_profile_id;
   }
   AnaFlag.event_id = event.event_id;
   for(int j=0; j<NoChannels; j++){
     RFISuppression(event.window_size, TriggerTrace[j], TriggerTrace[j]);
     SNR(event.window_size, TriggerTrace[j], &snr, &snr_mean, &snr_rms);
     NoPulse = PulseCount(event.window_size, TriggerTrace[j], &Mean, &Sigma, &Max);
     if(NoPulse>0 && NoPulse<5 && snr>75){
       AnaFlag.DetCh++;
       AnaFlag.Int_Raw[AnaFlag.DetCh-1] = IntegralRawData(event.window_size, TriggerTrace[j]);
       AnaFlag.Int_RawSq[AnaFlag.DetCh-1] = IntegralRawSquareData(event.window_size, TriggerTrace[j]);
       AnaFlag.Int_Env[AnaFlag.DetCh-1] = -1; //IntegralEnvelope(event.window_size, TriggerTrace[j]);
       AnaFlag.FWHM[AnaFlag.DetCh-1] = FWHMPeak(event.window_size, TriggerTrace[j]);
       DynamicThreshold(event.window_size, TriggerTrace[j], &AnaFlag.DynThreshold[AnaFlag.DetCh-1]);
//       cout << event.event_id << " - " << AnaFlag.DynThreshold[AnaFlag.DetCh-1] << " - " << snr << endl;
       AnaFlag.snr[AnaFlag.DetCh-1] = snr;
       AnaFlag.snr_mean[AnaFlag.DetCh-1] = snr_mean;
       AnaFlag.snr_rms[AnaFlag.DetCh-1] = snr_rms;
       AnaFlag.NoPulses[AnaFlag.DetCh-1] = NoPulse;
       AnaFlag.event_id = event.event_id;
       AnaFlag.ana_index = 2;
       strcpy(AnaFlag.description ,"v14, full parameter after snr>75 && NoPulses<5, DetCh>1: L0 with 200.0ns");
       AnaFlag.channel_id[AnaFlag.DetCh-1]=channel_id[j];
       PulseLengthEstimate(event.window_size, TriggerTrace[j], &AnaFlag.PulseLength[AnaFlag.DetCh-1], &AnaFlag.PulsePosition[AnaFlag.DetCh-1], false, false, true);
     }
     
   }
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
   }

   if(AnaFlag.DetCh>1) {
     coincidence_time = 200.0;
     AnaFlag.L1Trigger = L0Decide(NoChannels, event.window_size, TriggerTrace, ant_pos, header.DAQ_id, &coincidence_time);
     AnaFlag.CoincTime = coincidence_time;
   }
   
   if(AnaFlag.L1Trigger){
    TAnaFlag->Fill();
    continue;
   }
  }
  
  //v15
  if(0){ 
   float snr, snr_rms, snr_mean;  
   float coincidence_time = 0;
   
   float PulseAmp[NoChannels];
   float PulseTime[NoChannels];
   float zenith[2], azimuth[2];
   
   AnaFlag.L1Trigger = false;
   AnaFlag.DetCh = 0;
   
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
    strcpy(ant_pos[j],channel_profiles.antenna_pos);
    channel_id[j] = event.channel_profile_id;
   }
   AnaFlag.event_id = event.event_id;
   for(int j=0; j<NoChannels; j++){
     RFISuppression(event.window_size, TriggerTrace[j], TriggerTrace[j]);
     SNR(event.window_size, TriggerTrace[j], &snr, &snr_mean, &snr_rms);
     NoPulse = PulseCount(event.window_size, TriggerTrace[j], &Mean, &Sigma, &Max);
     if(NoPulse>0 && NoPulse<5 && snr>75){
       AnaFlag.DetCh++;
       AnaFlag.Int_Raw[AnaFlag.DetCh-1] = IntegralRawData(event.window_size, TriggerTrace[j]);
       AnaFlag.Int_RawSq[AnaFlag.DetCh-1] = IntegralRawSquareData(event.window_size, TriggerTrace[j]);
       AnaFlag.Int_Env[AnaFlag.DetCh-1] = -1; //IntegralEnvelope(event.window_size, TriggerTrace[j]);
       AnaFlag.FWHM[AnaFlag.DetCh-1] = FWHMPeak(event.window_size, TriggerTrace[j]);
       DynamicThreshold(event.window_size, TriggerTrace[j], &AnaFlag.DynThreshold[AnaFlag.DetCh-1]);
//       cout << event.event_id << " - " << AnaFlag.DynThreshold[AnaFlag.DetCh-1] << " - " << snr << endl;
       AnaFlag.snr[AnaFlag.DetCh-1] = snr;
       AnaFlag.snr_mean[AnaFlag.DetCh-1] = snr_mean;
       AnaFlag.snr_rms[AnaFlag.DetCh-1] = snr_rms;
       AnaFlag.NoPulses[AnaFlag.DetCh-1] = NoPulse;
       AnaFlag.event_id = event.event_id;
       AnaFlag.ana_index = 2;
       strcpy(AnaFlag.description ,"v15, full parameter after snr>75 && NoPulses<5, DetCh>1: L0 with 150.0ns");
       AnaFlag.channel_id[AnaFlag.DetCh-1]=channel_id[j];
       PulseLengthEstimate(event.window_size, TriggerTrace[j], &AnaFlag.PulseLength[AnaFlag.DetCh-1], &AnaFlag.PulsePosition[AnaFlag.DetCh-1], false, false, true);
     }
     
   }
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
   }

   if(AnaFlag.DetCh>1) {
     coincidence_time = 150.0;
     AnaFlag.L1Trigger = L0Decide(NoChannels, event.window_size, TriggerTrace, ant_pos, header.DAQ_id, &coincidence_time);
     AnaFlag.CoincTime = coincidence_time;
     if(AnaFlag.L1Trigger){
       for(int a=0; a<AnaFlag.DetCh; a++){
         for(int b=0; b<NoChannels; b++){
	   if(AnaFlag.channel_id[a] == channel_id[b]) strcpy(AntPos[a],ant_pos[b]);
	 }
	 PulseAmp[a] = 1;
	 PulseTime[a] = AnaFlag.PulsePosition[a]*12.5/pow(10.0,9); // in nano seconds
       }
       PlaneFit(AnaFlag.DetCh, AntPos, header.DAQ_id, PulseAmp, PulseTime, zenith, azimuth);
       AnaFlag.Azimuth = azimuth[0];
       AnaFlag.Azimuth_err = azimuth[1];
       AnaFlag.Zenith = zenith[0];
       AnaFlag.Zenith_err = zenith[1];
       //cout << "Azimuth " << azimuth[0] << " -+ " << azimuth[1]  << " - Zenith " << zenith[0] << " -+ " << zenith[1] << endl << endl;
     }
   }
   
   if(AnaFlag.L1Trigger){
    TAnaFlag->Fill();
    continue;
   }
  }

  //v16
  if(0){ 
   float snr, snr_rms, snr_mean;  
   float coincidence_time = 0;
   
   float PulseAmp[NoChannels];
   float PulseTime[NoChannels];
   float zenith[2], azimuth[2];
   float Int_RawSq=0;
   
   AnaFlag.L1Trigger = false;
   AnaFlag.DetCh = 0;
   
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
    strcpy(ant_pos[j],channel_profiles.antenna_pos);
    channel_id[j] = event.channel_profile_id;
   }
   AnaFlag.event_id = event.event_id;
   for(int j=0; j<NoChannels; j++){
     RFISuppression(event.window_size, TriggerTrace[j], TriggerTrace[j]);
     SNR(event.window_size, TriggerTrace[j], &snr, &snr_mean, &snr_rms);
     NoPulse = PulseCount(event.window_size, TriggerTrace[j], &Mean, &Sigma, &Max);
     Int_RawSq = IntegralRawSquareData(event.window_size, TriggerTrace[j]);
     if(NoPulse==1 && snr>75){
       AnaFlag.DetCh++;
       AnaFlag.Int_Raw[AnaFlag.DetCh-1] = -1;
       AnaFlag.Int_RawSq[AnaFlag.DetCh-1] = Int_RawSq;
       AnaFlag.Int_Env[AnaFlag.DetCh-1] = -1; 
       AnaFlag.FWHM[AnaFlag.DetCh-1] = FWHMPeak(event.window_size, TriggerTrace[j]);
       AnaFlag.DynThreshold[AnaFlag.DetCh-1] = -1; 
       AnaFlag.snr[AnaFlag.DetCh-1] = snr;
       AnaFlag.snr_mean[AnaFlag.DetCh-1] = snr_mean;
       AnaFlag.snr_rms[AnaFlag.DetCh-1] = snr_rms;
       AnaFlag.NoPulses[AnaFlag.DetCh-1] = NoPulse;
       AnaFlag.event_id = event.event_id;
       AnaFlag.ana_index = 2;
       strcpy(AnaFlag.description ,"v16, parameters after snr>75 && NoPulses==1, DetCh>2: L0 with 170.0ns");
       AnaFlag.channel_id[AnaFlag.DetCh-1]=channel_id[j];
       PulseLengthEstimate(event.window_size, TriggerTrace[j], &AnaFlag.PulseLength[AnaFlag.DetCh-1], &AnaFlag.PulsePosition[AnaFlag.DetCh-1], false, false, true);
     }
     
   }
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
   }

   if(AnaFlag.DetCh>2) {
     coincidence_time = 170.0; //Zenithangle of about 65°
     AnaFlag.L1Trigger = L0Decide(NoChannels, event.window_size, TriggerTrace, ant_pos, header.DAQ_id, &coincidence_time);
     AnaFlag.CoincTime = coincidence_time;
     if(AnaFlag.L1Trigger){
       for(int a=0; a<AnaFlag.DetCh; a++){
         for(int b=0; b<NoChannels; b++){
	   if(AnaFlag.channel_id[a] == channel_id[b]) strcpy(AntPos[a],ant_pos[b]);
	 }
	 PulseAmp[a] = 1;
	 PulseTime[a] = AnaFlag.PulsePosition[a]*12.5/pow(10.0,9); // in nano seconds
       }
       PlaneFit(AnaFlag.DetCh, AntPos, header.DAQ_id, PulseAmp, PulseTime, zenith, azimuth);
       AnaFlag.Azimuth = azimuth[0];
       AnaFlag.Azimuth_err = azimuth[1];
       AnaFlag.Zenith = zenith[0];
       AnaFlag.Zenith_err = zenith[1];
       //cout << "Azimuth " << azimuth[0] << " -+ " << azimuth[1]  << " - Zenith " << zenith[0] << " -+ " << zenith[1] << endl << endl;
     }
   }
   
   if(AnaFlag.L1Trigger){
    TAnaFlag->Fill();
    continue;
   }
  }

  //v18
  if(0){ 
   float snr, snr_rms, snr_mean;  
   float coincidence_time = 0;
   
   float PulseAmp[NoChannels];
   float PulseTime[NoChannels];
   float zenith[2], azimuth[2];
   float PulseLength=0, PulsePosition=0, Int_Env=0, PulseMax=0;
   float threshold=0;
   
   AnaFlag.L1Trigger = false;
   AnaFlag.DetCh = 0;
   
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
    strcpy(ant_pos[j],channel_profiles.antenna_pos);
    channel_id[j] = event.channel_profile_id;
   }
   AnaFlag.event_id = event.event_id;
   for(int j=0; j<NoChannels; j++){
     RFISuppression(event.window_size, TriggerTrace[j], TriggerTrace[j]);
     SNR(event.window_size, TriggerTrace[j], &snr, &snr_mean, &snr_rms);
     NoPulse = PulseCount(event.window_size, TriggerTrace[j], &Mean, &Sigma, &Max);
     if(NoPulse==1 && snr>75){
      DynamicThreshold(event.window_size, TriggerTrace[j], &threshold);
      EstimatePulseParameter(event.window_size, TriggerTrace[j], &PulseLength, &PulsePosition, &PulseMax, &Int_Env, threshold);
      if( Int_Env/PulseMax<0.003 && PulseLength<9 ){
       AnaFlag.DetCh++;
       AnaFlag.Int_Raw[AnaFlag.DetCh-1] = -1;
       AnaFlag.Int_RawSq[AnaFlag.DetCh-1] = -1;
       AnaFlag.FWHM[AnaFlag.DetCh-1] = -1; 
       AnaFlag.DynThreshold[AnaFlag.DetCh-1] = threshold; 
       AnaFlag.snr[AnaFlag.DetCh-1] = snr;
       AnaFlag.snr_mean[AnaFlag.DetCh-1] = snr_mean;
       AnaFlag.snr_rms[AnaFlag.DetCh-1] = snr_rms;
       AnaFlag.NoPulses[AnaFlag.DetCh-1] = NoPulse;
       AnaFlag.PulseLength[AnaFlag.DetCh-1] = PulseLength;
       AnaFlag.PulsePosition[AnaFlag.DetCh-1] = PulsePosition;
       AnaFlag.PulseMax[AnaFlag.DetCh-1] = PulseMax;       
       AnaFlag.Int_Env[AnaFlag.DetCh-1] = Int_Env;
       AnaFlag.event_id = event.event_id;
       AnaFlag.ana_index = 2;
       AnaFlag.run_id = header.run_id;
       AnaFlag.daq_id = header.DAQ_id;
       strcpy(AnaFlag.description ,"v18_8, parameters after snr>75 && NoPulses==1 && PulseLength<9 && Int_Env/PulseMax<0.003, DetCh>1: L0 with 170.0ns");
       AnaFlag.channel_id[AnaFlag.DetCh-1]=channel_id[j];
      }
     }
   }

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
     
     if(pretrigger){
      for(int j=0; j<NoChannels; j++){
       Tevent->GetEntry(i+j);
       for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
      }
      
      AnaFlag.Azimuth = -1;
      AnaFlag.Azimuth_err = -1;
      AnaFlag.Zenith = -1;
      AnaFlag.Zenith_err = -1;
      coincidence_time = 170.0; //Zenithangle of about 65°
      AnaFlag.L1Trigger = L0Decide(NoChannels, event.window_size, TriggerTrace, ant_pos, header.DAQ_id, &coincidence_time);
      AnaFlag.CoincTime = coincidence_time;
      if(AnaFlag.L1Trigger){
        int PlaneFitCh = 0;
        for(int a=0; a<AnaFlag.DetCh; a++){
          if(AnaFlag.NoPulses[a]==1){
	    for(int b=0; b<NoChannels; b++){
	      if(AnaFlag.channel_id[a] == channel_id[b]) strcpy(AntPos[PlaneFitCh],ant_pos[b]);
	    }
  	    PulseAmp[PlaneFitCh] = 1;
	    PulseTime[PlaneFitCh] = AnaFlag.PulsePosition[a]*12.5/pow(10.0,9); // in nano seconds
	    PlaneFitCh++;
         }
	}
        PlaneFit(PlaneFitCh, AntPos, header.DAQ_id, PulseAmp, PulseTime, zenith, azimuth);
        AnaFlag.Azimuth = azimuth[0];
        AnaFlag.Azimuth_err = azimuth[1];
        AnaFlag.Zenith = zenith[0];
        AnaFlag.Zenith_err = zenith[1];
//cout << event.event_id << endl;	
//cout << "Azimuth " << azimuth[0] << " -+ " << azimuth[1]  << " - Zenith " << zenith[0] << " -+ " << zenith[1] << endl << endl;
      }
    }
   }
   
   if( AnaFlag.DetCh>1 /*AnaFlag.L1Trigger*/){
    TAnaFlag->Fill();
    continue;
   }
  }
  
  //v18 - ARG D42
  if(1){ 
   float snr, snr_rms, snr_mean;  
   float coincidence_time = 0;
   
   float PulseAmp[NoChannels];
   float PulseTime[NoChannels];
   float zenith[2], azimuth[2];
   float PulseLength=0, PulsePosition=0, Int_Env=0, PulseMax=0;
   float threshold=0;
   
   AnaFlag.L1Trigger = false;
   AnaFlag.DetCh = 0;
   
   for(int j=0; j<NoChannels; j++){
    Tevent->GetEntry(i+j);
    for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
    strcpy(ant_pos[j],channel_profiles.antenna_pos);
    channel_id[j] = event.channel_profile_id;
   }
   AnaFlag.event_id = event.event_id;
   for(int j=0; j<NoChannels; j++){
     RFISuppression(event.window_size, TriggerTrace[j], TriggerTrace[j]);
     SNR(event.window_size, TriggerTrace[j], &snr, &snr_mean, &snr_rms);
     NoPulse = PulseCount(event.window_size, TriggerTrace[j], &Mean, &Sigma, &Max);
     if(NoPulse==1 && snr>75){
      DynamicThreshold(event.window_size, TriggerTrace[j], &threshold);
      EstimatePulseParameter(event.window_size, TriggerTrace[j], &PulseLength, &PulsePosition, &PulseMax, &Int_Env, threshold);
      if( Int_Env/PulseMax<0.003 && PulseLength<9 ){
       AnaFlag.DetCh++;
       AnaFlag.Int_Raw[AnaFlag.DetCh-1] = -1;
       AnaFlag.Int_RawSq[AnaFlag.DetCh-1] = -1;
       AnaFlag.FWHM[AnaFlag.DetCh-1] = -1; 
       AnaFlag.DynThreshold[AnaFlag.DetCh-1] = threshold; 
       AnaFlag.snr[AnaFlag.DetCh-1] = snr;
       AnaFlag.snr_mean[AnaFlag.DetCh-1] = snr_mean;
       AnaFlag.snr_rms[AnaFlag.DetCh-1] = snr_rms;
       AnaFlag.NoPulses[AnaFlag.DetCh-1] = NoPulse;
       AnaFlag.PulseLength[AnaFlag.DetCh-1] = PulseLength;
       AnaFlag.PulsePosition[AnaFlag.DetCh-1] = PulsePosition;
       AnaFlag.PulseMax[AnaFlag.DetCh-1] = PulseMax;       
       AnaFlag.Int_Env[AnaFlag.DetCh-1] = Int_Env;
       AnaFlag.event_id = event.event_id;
       AnaFlag.ana_index = 2;
       AnaFlag.run_id = header.run_id;
       AnaFlag.daq_id = header.DAQ_id;
       strcpy(AnaFlag.description ,"v18_arg, parameters after snr>75 && NoPulses==1 && PulseLength<9 && Int_Env/PulseMax<0.003, DetCh>1: L0 with 260.0ns");
       AnaFlag.channel_id[AnaFlag.DetCh-1]=channel_id[j];
      }
     }
   }

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
     
     if(pretrigger){
      for(int j=0; j<NoChannels; j++){
       Tevent->GetEntry(i+j);
       for(unsigned int a=0; a<event.window_size; a++) TriggerTrace[j][a] = event.trace[a];
      }
      
      AnaFlag.Azimuth = -1;
      AnaFlag.Azimuth_err = -1;
      AnaFlag.Zenith = -1;
      AnaFlag.Zenith_err = -1;
      coincidence_time = 260.0; //Zenithangle of about 65°
      AnaFlag.L1Trigger = L0Decide(NoChannels, event.window_size, TriggerTrace, ant_pos, header.DAQ_id, &coincidence_time);
      AnaFlag.CoincTime = coincidence_time;
      if(AnaFlag.L1Trigger){
        int PlaneFitCh = 0;
        for(int a=0; a<AnaFlag.DetCh; a++){
          if(AnaFlag.NoPulses[a]==1){
	    for(int b=0; b<NoChannels; b++){
	      if(AnaFlag.channel_id[a] == channel_id[b]) strcpy(AntPos[PlaneFitCh],ant_pos[b]);
	    }
  	    PulseAmp[PlaneFitCh] = 1;
	    PulseTime[PlaneFitCh] = AnaFlag.PulsePosition[a]*12.5/pow(10.0,9); // in nano seconds
	    PlaneFitCh++;
         }
	}
        PlaneFit(PlaneFitCh, AntPos, header.DAQ_id, PulseAmp, PulseTime, zenith, azimuth);
        AnaFlag.Azimuth = azimuth[0];
        AnaFlag.Azimuth_err = azimuth[1];
        AnaFlag.Zenith = zenith[0];
        AnaFlag.Zenith_err = zenith[1];
//cout << event.event_id << endl;	
//cout << "Azimuth " << azimuth[0] << " -+ " << azimuth[1]  << " - Zenith " << zenith[0] << " -+ " << zenith[1] << endl << endl;
      }
    }
   }
   
   if( AnaFlag.DetCh>1 /*AnaFlag.L1Trigger*/){
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
