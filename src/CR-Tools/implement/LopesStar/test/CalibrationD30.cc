#include <iostream>

#include <TROOT.h>
#include <TSystem.h>
#include <TGraph.h>
#include <TFile.h>
#include <time.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TMath.h>

#include <LopesStar/util.h>

using namespace std;
using namespace TMath;

/*!
  Calculation of the calibration parameters determined from the amp. measurement
  with a ref. antenne from LOPES30

  The used ref. antenne is well known and calibrated in field strength over the
  used frequency spectrum. Therefore the measured field strength distribution
  on frequency space can be calibrated - amp. calibration.
*/
  
   
int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();
  gROOT->SetStyle("Plain");


  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  

  char *InputFileName="$HOME/inpput_00.root";
  char *OutputFileName="output_calib_D30.root";
  char *AntPos="CTR";
  char *polarisation="North/South";
  
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "hi:o:a:p:")) != -1) //: after var ->Waits for input
    switch (c)
      {
      case 'i':
	InputFileName = optarg ;
	break;
      case 'o':
	OutputFileName = optarg ;
	break;
      case 'a':
	AntPos = optarg ;
	break;
      case 'p':
	polarisation = optarg ;
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
	cout << " Overview: calibration of D30\n";
	cout << "=============================\n\n";
	cout << " -h	HELP -- you already tried :-)\n";
	cout << " -i	input file name (default: " << InputFileName << ")\n";
	cout << " -o	output file name (default: " << OutputFileName << ")\n";
	cout << " -a	antenna position name (default: " << AntPos << ")\n";
	cout << " -p	polarisation of the channel (default: " << polarisation << ")\n";
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
  CreateTChain(Tevent, InputFileName);
  
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
    Theader->Add(InputFileName);
    Tchannel_profile->Add(InputFileName);
  
    CreateTevent(Tevent, &event);
    CreateTheader(Theader, &header);
    CreateTchannel_profiles(Tchannel_profile, &channel_profiles);

    Tevent->GetEntry(0);
    Theader->GetEntry(0);
    NoChannels = Tchannel_profile->GetEntries();
  }
  
  //find channel id of given antenna channel
  unsigned int channel_id = 0;
  for(int i=0; i<NoChannels; i++){
    Tevent->GetEntry(i);
    if(strcmp(channel_profiles.antenna_pos,AntPos)==0 && strcmp(channel_profiles.polarisation,polarisation)==0){
      channel_id = channel_profiles.ch_id;
      break;
    }
  }
  if(channel_id == 0){
    cerr << "channel id not found" << endl;
    exit(1);
  }
  
  TFile *output = new TFile(OutputFileName,"RECREATE","Calibration of D30");
  
  struct cal cal;
  int freq_cnt;
  TTree *Tcal = new TTree("Tcal","paramters of the calibration -- D30");
  Tcal->Branch("NoChannels",&NoChannels,"NoChannels/I");
  Tcal->Branch("NoCorrValue",&freq_cnt,"NoCorrValue/I");
  Tcal->Branch("CorrValue",&cal.CorrValue,"CorrValue[NoCorrValue]/F");
  Tcal->Branch("channel_id",&cal.channel_id,"channel_id[NoChannels]/I");
  Tcal->Branch("MaxHF",&cal.MaxHF,"MaxHF[NoChannels]/F");
  Tcal->Branch("MaxEnv",&cal.MaxEnv,"MaxEnv[NoChannels]/F");  
  Tcal->Branch("ADCoverflow",&cal.ADCoverflow,"ADCoverflow/O");  
  Tcal->Branch("CalCh",&cal.CalCh,"CalCh/I");
 
  //used frequency range: 41MHz up to 79MHz -> becauss of the bandpass filter!!
  
  //calculate the expectd field strength at antenne position
  int used_freq_values = 39;
  float RefAmp[used_freq_values];
  for(int i=41; i<(used_freq_values+41); i++) RefAmp[i-41] = AmpRefSource_FieldStrength(i);
  
  //get the gain directivity of the antenne to correct
  float GainAnt[used_freq_values];
  for(int i=41; i<(used_freq_values+41); i++) GainAnt[i-41] = 1.88365; // 5.5dBi for zenith=0 //GainDirectivity(i,0);
 
  float Amp[event.window_size/2];
  float Phase[event.window_size/2];
  float rawfft[2*event.window_size];
  float Amp_mean = 0;
  float spec_res = (float)((float)40e6 / (float)(event.window_size/2)); // spectral resolution
  float step_size = (float)((float)1e6 / spec_res); //amp values between 1MHz
  float local_max = 0;
  int local_max_pos = 0;
  int NoZeros = 7;
  int new_window_size = (NoZeros +1)*event.window_size;
  float Trace[new_window_size];
  float TraceTmp[new_window_size];
  float RefAntPulseRate = 1.0e-6;
  float NoPulses = event.window_size*12.5e-9 / RefAntPulseRate;
  float HF_mean;
  
  cal.CalCh = channel_id;
  //loop over file in steps of events
  for(int i=0; i<Tevent->GetEntries(); i += NoChannels){
    //looop over one event
    for(int j=0; j<NoChannels; j++){
      if(Tevent->GetEntry(i+j)==0) cerr << "Tevent->GetEntry(): error" << endl;
      cal.channel_id[j] = channel_profiles.ch_id;
      
      if(channel_profiles.ch_id==channel_id){
        //check for ADC overflow
	cal.ADCoverflow = !(TraceCheck(&event));
	
	TraceFFT(event.window_size, event.trace, Amp, Phase, rawfft);
	
	//correct for number of pulses in the time window
	for(int a=0; a<event.window_size/2; a++) Amp[a] /= NoPulses;
	
	Amp_mean = Mean(event.window_size/2, Amp);
	
	//find local MHz maximum
	freq_cnt = 0;
	//used frequencies are from 41MHz up to 79MHz -> bandpass filter!!
	for(unsigned int a=(int)step_size; a<event.window_size/2-step_size; a += (int)step_size){
	  local_max = 0;
	  local_max_pos = 0;
	  for(int b=-25; b<25; b++){
	    if( (a+b) < 0) continue;
	    if( (a+b) > event.window_size/2) continue;
	    if(Amp[a+b]>local_max){
	      local_max = Amp[a+b];
	      local_max_pos = a+b;
	    }
	  }
	  cal.CorrValue[freq_cnt] = RefAmp[freq_cnt] / ( local_max * GainAnt[freq_cnt] );
	  freq_cnt++;
	}
      }
    
/*
      ZeroPaddingFFT(event.window_size, event.trace, NoZeros, Trace);
      
      //find mean HF maximum of pulses
      for(int a=0; a<new_window_size; a++) TraceTmp[a] = Abs(Trace[a]);
      HF_mean = Mean(new_window_size, TraceTmp);
      for(int a=0; a<new_window_size; a++){
        if(TraceTmp[a]>HF_mean*2){
	  cout << TraceTmp[a] << " -- " << a << " -- " << HF_mean << " -- " << channel_profiles.ch_id << " -- " << channel_id <<endl;
//search local for max in time domain	  
	  break;
	}
      }
    
      //find mean Env maximum of pulses
*/    
    }
    Tcal->Fill();
  }

  //write and clean up
  Tcal->Write();
  output->Close();
  delete output;

  delete Tevent;
  delete Theader;
  delete Tchannel_profile;
}

