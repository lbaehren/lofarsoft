#include <iostream>

#include <libgen.h>

#include <TROOT.h>
#include <TSystem.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TFile.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TPad.h>
#include <TH1F.h>

#include "util.hh"


using namespace std;
using namespace TMath;

//! spectra of various channels
/*!

many options allows the user to define spectra and mean spectra over a run.

*/


int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();
  gROOT->SetStyle("Plain");


  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  

  
  char *StarFileName="$HOME/analyisis/default_00.root";
  char *OutputFileName="$HOME/analysis/StarTrace_out.root";
  char name[1024];
  int time_start_sec = 0;
  unsigned int event_id_offset = 0;
  unsigned int event_id_start = 0;
  bool event_search = false;
  bool time_search = false;
  bool power=false;
  int number_analyse = 10;
  bool bRFISupp = false;
  bool bAmpMean = true;
  bool RefAntCal = false;
  bool SingleSave = false;
  
  int MeanEvents = 100;
  int MeanTime = 1; //sec
  bool AverageTime = true ; 
  int AverageCheck = 0;
  int Start_Event = 0;
  int SecOffset = 0;
  


  
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "hf:i:e:t:n:m:a:d:o:psg:cbk")) != -1) //: after var ->Waits for input
    switch (c)
      {
      case 'f':
	OutputFileName = optarg ;
	break;
      case 'p':
	power = true;
	break;
      case 'k':
	SingleSave = true;
	break;
      case 'b':
	RefAntCal = true;
	break;
      case 's':
	bRFISupp = true;
	break;
      case 'c':
	bAmpMean = false;
	break;
      case 'i':
	StarFileName = optarg ;
	break;
      case 'e':
	event_id_offset = (unsigned int) atoi(optarg) ;
	event_search = true;
	break;
      case 'g':
	event_id_start = (unsigned int) atoi(optarg) ;
	event_search = true;
	break;
      case 't':
	 time_start_sec = atoi(optarg) ;
	 time_search = true;
	 break;
      case 'n':
	 number_analyse = atoi(optarg) ;
	 break;
      case 'm':
	MeanTime = atoi(optarg) ;
	AverageTime = true;
	AverageCheck++;
	break;
      case 'a':
	 MeanEvents = atoi(optarg) ;
	 AverageTime = false;
	 AverageCheck++;
	 break;
      case 'd':
	 Start_Event = atoi(optarg) ;
	 break;
      case 'o':
	 SecOffset = atoi(optarg) ;
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
	cout << "spectra viewer for STAR traces\n";
	cout << "===============================\n\n";
	cout << " -h	HELP -- you already tried :-)\n";
	cout << " -f	output file name (default: " << OutputFileName << ")\n";
	cout << " -i	input file name in STAR format (default: " << StarFileName << ")\n";
	cout << " -p	power spectrum is written in the file \n";
	cout << " -b	enable calibration with refererence antenna (default: lab. calibration)\n";
	cout << " -s	enable RFI suppression (default: disabled)\n";
	cout << " -k	save all TGraph objects additional in the output file (default: disabled)\n";

	cout << "\n-----  Amp & Phase Distribution  -----\n\n";
	cout << " -e	event offset to start with (default: from beginning)\n";
	cout << " -g	event ID to start with\n";
	cout << " -t	time offset in sec to start with\n";
	cout << " -n	number of events to plot (default: " << number_analyse << ")\n";
	
	cout << "\n-----------  Mean Spectra ------------\n\n";
	cout << " -m 	time in sec to average over (default: " << MeanTime << "s)\n";
	cout << " -o	time offset in sec to start averaging, works only with -m (default: " << SecOffset << "s)\n";	
	cout << " -a	number of events to average over\n";
	cout << " -c 	enable complex mean and disable amplitude mean\n";
	cout << " -d 	event offset to start the averaging (default: " << Start_Event << ")\n";
	cout << "\n\n\nmailto: asch@ipe.fzk.de\n";
	exit(1);
        break;      
      default:
	abort ();
      }

  if(event_search && time_search ){
   cerr << "Event search by time OR id, not both. \n";
   exit (1);
  }
  
  if(AverageCheck>1){
   cerr << "average over time or events, not both ...\n";
   exit (1);
  }


  int NoChannels=0;
  static struct event event;
  static struct header header;
  static struct channel_profiles channel_profiles;

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

  TFile *output = new TFile (OutputFileName,"RECREATE","trace analaysis");
  output->Close();
  delete output;
  

///////////////////////////////   
  int START = 0;
  
 if(time_search){
   int Sec;
   int Nanosec;
   Tevent->GetEntry(0);
   ConvertTimeStamp(event.timestamp, &Sec, &Nanosec);
   time_start_sec += Sec;
   
   for(int w=0; w<Tevent->GetEntries(); w++){
     Tevent->GetEntry(w);
     START = w;
     ConvertTimeStamp(event.timestamp, &Sec, &Nanosec);
     if( (Sec)>=(time_start_sec) && (Sec-3600)<time_start_sec){
       cout << "event found by time" << endl;
       break;
     }
     if( (Sec-10)>time_start_sec){
       cout << "event NOT found by time" << endl;
       break;
     }
   }
  }
  
 if(event_search){
  if(event_id_start != 0){
   for(int w=START; w<Tevent->GetEntries(); w++){
    Tevent->GetEntry(w);
    START = w;
    if(event.event_id == event_id_start) {
      cout << "event found by id" << endl;
      break;
    }
     if(event.event_id > event_id_start){
      cout << "event NOT found by id" << endl;
      break;
    }
   }
  }
  else{
    START = event_id_offset*NoChannels;
  }
 }
   
   Tevent->GetEntry(0);

 int STOP = START+NoChannels*number_analyse;
 
// cout << START << " START" << endl;
// cout << STOP << " STOP" << endl;
///////////////////////////////
  
  if(STOP>= Tevent->GetEntries()){
   cerr << "entry doesn't exist" << endl;
   STOP = Tevent->GetEntries();
   cerr << "changed end number." <<  STOP << endl;
   if(START>=STOP){
     cerr << "entry range wrong.. exit" << endl;
     exit(1);
   }
  }

  cout << "first event: id = " << event.event_id << " - " << event.timestamp << endl;
  
  
  int ChId=0;
  char title[1024];

  TCanvas **canfft = new TCanvas *[(STOP-START)];
  float *freq = new float [event.window_size/2];
  for(unsigned int i=0; i<event.window_size/2; i++)
    freq[i]=40.0+40.0/(event.window_size/2)*2+(40.0/(event.window_size/2.0))*(i);
    
  float **Amp = new float *[(STOP-START)];
  for(int i=0; i<(STOP-START); i++) Amp[i] = new float [event.window_size/2];
  float **Phase = new float *[(STOP-START)];
  for(int i=0; i<(STOP-START); i++) Phase[i] = new float [event.window_size/2];  
  for(int i=0; i<(STOP-START); i++)for(unsigned int j=0; j<event.window_size/2; j++){
    Amp[i][j]=0;
    Phase[i][j]=0;
  }
  TGraph **gAmp = new TGraph *[(STOP-START)];
  TGraph **gPhase = new TGraph *[(STOP-START)];
  
  float trace[event.window_size];


  
/***********mean freq spectrum over given no of events or time range *****************/  


    int nano_dummy=0;
    int Sec_dummy=0;
    int ecount=Start_Event*NoChannels;
    int dummy_= 0;
   
  if(AverageTime){
    cout << "mean spectra over " << MeanTime << " s." << endl;
    Tevent->GetEntry(ecount);
    ecount +=NoChannels;
    ConvertTimeStamp( event.timestamp, &Sec_dummy, &nano_dummy);
    int Sec_Stop = SecOffset+Sec_dummy+MeanTime;
    while(Sec_dummy<Sec_Stop){
      if(Tevent->GetEntry(ecount)==0){
        cerr << "error - time range too long" << endl;
	break;
      }
      ecount += NoChannels;
      ConvertTimeStamp( event.timestamp, &Sec_dummy, &nano_dummy);
    }
    MeanEvents = ecount - Start_Event*NoChannels;
  }
  else{
    cout << "mean spectra over " << MeanEvents << " events." << endl;
    if(Tevent->GetEntry(Start_Event*NoChannels)==0){
      cerr << "error - start event ID is too high" << endl;
      Start_Event = -1;
    }
    
    ConvertTimeStamp(event.timestamp, &dummy_, &nano_dummy);
    if(Tevent->GetEntry(MeanEvents*NoChannels+Start_Event*NoChannels-1)==0 || Start_Event == -1){
      MeanEvents = Tevent->GetEntries() / NoChannels;
      cerr << "error - too many events" << endl;
    }
    
    ConvertTimeStamp(event.timestamp, &Sec_dummy, &nano_dummy);
    MeanTime = Sec_dummy - dummy_;
  }
  
/*  cout << "Start_Event " << Start_Event << endl;
  cout << "MeanEvents " << MeanEvents << endl;
  cout << "NoChannels " << NoChannels << endl;
  cout << "MeanTime " << MeanTime << endl;
*/  
  float meanFreq[NoChannels][event.window_size/2];
  float meanFreq_err[NoChannels][event.window_size/2][MeanEvents];
  float meanPhase[NoChannels][event.window_size/2];
  for(int i=0; i<NoChannels; i++) for(unsigned int j=0; j<event.window_size/2; j++){
    meanFreq[i][j]=0;
    meanPhase[i][j]=0;
    for(int k=0; k<MeanEvents; k++) {
      meanFreq_err[i][j][k] = 0;
    }
  }
  float AMP[event.window_size/2];
  float PHASE[event.window_size/2];
  float FFTData[event.window_size*2]; 
  float RAW_FFT[NoChannels][event.window_size*2];
  for(int i=0; i<NoChannels; i++) for(unsigned int j=0; j<event.window_size*2; j++){
    RAW_FFT[i][j] = 0;
    FFTData[j] = 0;
  }
  float dTrace[event.window_size/2];
  
  
  float ***FFT_err = new float **[NoChannels];
  float ***FFT_re = new float **[NoChannels];
  float ***FFT_im = new float **[NoChannels];
  for(int i=0; i<NoChannels; i++){
    FFT_err[i] = new float *[event.window_size*2];
    FFT_re[i] = new float *[event.window_size*2];
    FFT_im[i] = new float *[event.window_size*2];
  }
  for(int i=0; i<NoChannels; i++) for(unsigned int j=0; j<event.window_size*2; j++){
    FFT_err[i][j] = new float [MeanEvents];
    FFT_re[i][j] = new float [MeanEvents];
    FFT_im[i][j] = new float [MeanEvents];
  }
  for(int i=0; i<NoChannels; i++) for(unsigned int j=0; j<event.window_size*2; j++) for(int k=0; k<MeanEvents; k++){
    FFT_err[i][j][k] = 0;
    FFT_re[i][j][k] = 0;
    FFT_im[i][j][k] = 0;
  }
  
  float **Amp_err = new float *[NoChannels];
  for(int i=0; i<NoChannels; i++) Amp_err[i] = new float [event.window_size/2];
  for(int i=0; i<NoChannels; i++) for(unsigned int j=0; j<event.window_size/2; j++) Amp_err[i][j]= 0;
  
  float *Freq_err = new float[event.window_size/2];
  for(unsigned int i=0; i<event.window_size/2; i++) Freq_err[i] = 40.0 / (event.window_size / 2.0);
    

  
  for(unsigned int j=0; j<event.window_size/2; j++){
    AMP[j]=0;
    PHASE[j]=0;
  }
  TGraphErrors **MeanF = new TGraphErrors *[NoChannels];
  TGraph **MeanP = new TGraph *[NoChannels];
  TCanvas **canMeanF = new TCanvas *[NoChannels];
  
  float RFITrace[event.window_size];

ChId=-1;
int event_count=0;
if(Start_Event>-1)
for(int i=Start_Event*NoChannels; i<MeanEvents*NoChannels+Start_Event*NoChannels; i++){
   if(Tevent->GetEntry(i)==0){
     cerr << "error - Tevent->GetEntry() -> mean freq spectra" << endl;
     break;
   }
   for(unsigned int j=0; j<event.window_size; j++) trace[j] = event.trace[j];

   if(!bTeventExtend){
     ChId=0;
     Tchannel_profile->GetEntry(ChId);
     while( event.channel_profile_id != channel_profiles.ch_id ){
       ChId++;
       if(Tchannel_profile->GetEntry(ChId)==0){
         cerr << "error - Tchannel_profile->GetEntry()" << endl;
         exit(0);
       }
     }
   }
   else{
     ChId++;
     if(ChId==NoChannels){
       ChId=0;
       event_count++;
     }
   }

   if(bRFISupp==true){
     for(unsigned int j=0; j<event.window_size; j++) RFITrace[j] = trace[j];
     RFISuppression((int)event.window_size, RFITrace, RFITrace);
     if(power){
        ADC2FieldStrength(event.window_size, trace, header.DAQ_id, 30.0);
        PowerTrace(event.window_size, trace);
	TraceFFT(event.window_size, trace, AMP, PHASE, FFTData, true, true);
	for(unsigned int j=0; j<event.window_size/2; j++) AMP[j] *= ( 12.5 / 1000.0 );
     }
     else
     CalibratedFrequencyDomain(event.window_size, RFITrace, AMP, PHASE, header.DAQ_id, 30., RefAntCal, false);
#warning fixed zenith angle to 30 degeree
   }
   else{
    if(power){
        ADC2FieldStrength(event.window_size, trace, header.DAQ_id, 30.0);
        PowerTrace(event.window_size, trace);
	TraceFFT(event.window_size, trace, AMP, PHASE, FFTData, true, true);
	for(unsigned int j=0; j<event.window_size/2; j++) AMP[j] *= ( 12.5 / 1000.0 );
    }
    else
     CalibratedFrequencyDomain(event.window_size, trace, AMP, PHASE, header.DAQ_id, 30., RefAntCal);
   }
   
   if(bAmpMean)
     for(unsigned int a=0; a<event.window_size/2; a++){
     meanFreq[ChId][a] += AMP[a];
     meanFreq_err[ChId][a][MeanEvents] = AMP[a];
     meanPhase[ChId][a] += PHASE[a];
   }
   else{
     for(unsigned int j=0; j<event.window_size; j++) trace[j] = event.trace[j];
     if(bRFISupp) RFISuppression((int)event.window_size, trace, trace);
     ADC2FieldStrength(event.window_size, trace, header.DAQ_id, 30.0);
     TraceFFT(event.window_size, trace, AMP, PHASE, FFTData);
     for(unsigned int a=0; a<event.window_size*2; a++){
       RAW_FFT[ChId][a] += FFTData[a];
       FFT_err[ChId][a][event_count] = FFTData[a];
     }
   }
   
   
}//end for over events

  if(bAmpMean)
   for(int i=0; i<NoChannels; i++)for(unsigned int j=0; j<event.window_size/2; j++){
    meanFreq[i][j] /= MeanEvents;
    meanPhase[i][j] /= MeanEvents;
    Amp_err[i][j] = TMath::RMS(MeanEvents,meanFreq_err[i][j]);
   }
  else{
   for(int i=0; i<NoChannels; i++){
     for(unsigned int j=0; j<event.window_size*2; j++){
       RAW_FFT[i][j] /= MeanEvents;
     }
     for(unsigned int j=0; j<event.window_size/2; j++){
       for(int k=0; k<MeanEvents; k++){
         FFT_re[i][event.window_size/2-1-j][k] = FFT_err[i][j][k];
	 FFT_im[i][event.window_size/2-1-j][k] = FFT_err[i][j+event.window_size][k];
       }
     }
   }
   for(int i=0; i<NoChannels; i++){
    TraceiFFT(event.window_size,  RAW_FFT[i], dTrace);
    TraceFFT(event.window_size , dTrace , meanFreq[i] , meanPhase[i], FFTData);
    float min, max;
    for(unsigned int j=0; j<event.window_size/2; j++){
      min = sqrt( pow(TMath::Mean(MeanEvents,FFT_re[i][j]) + TMath::RMS(MeanEvents,FFT_re[i][j]),2) +
                  pow(TMath::Mean(MeanEvents,FFT_im[i][j]) + TMath::RMS(MeanEvents,FFT_im[i][j]),2) );
      max = sqrt( pow(TMath::Mean(MeanEvents,FFT_re[i][j]) - TMath::RMS(MeanEvents,FFT_re[i][j]),2) +
                  pow(TMath::Mean(MeanEvents,FFT_im[i][j]) - TMath::RMS(MeanEvents,FFT_im[i][j]),2) );
      Amp_err[i][j] = fabs(min-max);
      meanFreq[i][j] *= ( 12.5 / 1000.0 );
      Amp_err[i][j] *= ( 12.5 / 1000.0 );
    }
   }
  }

  
  for(int i=0; i<NoChannels; i++){
     MeanF[i] = new TGraphErrors (event.window_size/2,freq,meanFreq[i],Freq_err,Amp_err[i]);
     sprintf(name,"meanFreq_%03i",i);
     MeanF[i]->SetName(name);
     MeanP[i] = new TGraph (event.window_size/2,freq,meanPhase[i]);
     sprintf(name,"meanPhase_%03i",i);
     MeanP[i]->SetName(name);
     

    if(bTeventExtend){
     if(Tevent->GetEntry(i)==0){
       cerr << "error - Tevent->GetEntry()" << endl;
       exit(0);
      }
    }
    else
     if(Tchannel_profile->GetEntry(i)==0){
       cerr << "error - Tchannel_profile->GetEntry()" << endl;
       exit(0);
     }
     
     sprintf(title,"D%i_%s_%s",header.DAQ_id, channel_profiles.antenna_pos, channel_profiles.polarisation);

     if(AverageTime)
       sprintf(name,", run = %i, mean over %i sec (%i events, %i sec offset, %i events offset)",
       		header.run_id, MeanTime, MeanEvents, SecOffset, Start_Event);	
     else
       sprintf(name,", run = %i, mean over %i events (%i sec, %i events offset)",
       		header.run_id, MeanEvents, MeanTime, Start_Event);	
       
     strcat(title,name);

     MeanF[i]->SetTitle(title);
     if(power) sprintf(title," power / (W #upoint m^{-2} #upoint MHz^{-1}) ");
      else sprintf(title," field strength / (#muV #upoint m^{-1} #upoint MHz^{-1}) ");
     MeanF[i]->GetXaxis()->SetTitle(" frequency / MHz ");
     MeanF[i]->GetYaxis()->SetTitle(title);
          
     sprintf(name,"canMeanF_%03i",i);	
     if(power)sprintf(title,"mean power over %i events and %i s", MeanEvents, MeanTime);	
     else sprintf(title,"mean freq over %i events and %i s", MeanEvents, MeanTime);	
     canMeanF[i]= new TCanvas(name,title);      
  
  }

  
/****************************/  
int Aevent=-1;

for(int i=START; i<Tevent->GetEntries(); i++){
   Aevent++;
   
   if((Aevent)==((STOP-START))) break;
   
   if(Tevent->GetEntry(i)==0){
     cerr << "error - Tevent->GetEntry() -> main function" << endl;
     break;
   }
   for(unsigned int j=0; j<event.window_size; j++) trace[j] = event.trace[j];
   
   if(!bTeventExtend){
     ChId=0;
     Tchannel_profile->GetEntry(ChId);
     while( event.channel_profile_id != channel_profiles.ch_id ){
       ChId++;
       if(Tchannel_profile->GetEntry(ChId)==0){
         cerr << "error - Tchannel_profile->GetEntry()" << endl;
         exit(0);
       }
     }
   }

   sprintf(title,"D%i_%s_%s, %s, run=%i, event_id=%i",header.DAQ_id, channel_profiles.antenna_pos, 
   		channel_profiles.polarisation, event.timestamp, header.run_id, event.event_id);
		
   if(bRFISupp==true){
     for(unsigned int j=0; j<event.window_size; j++) RFITrace[j] = trace[j];
     RFISuppression((int)event.window_size, RFITrace, RFITrace);
     if(power){
        ADC2FieldStrength(event.window_size, trace, header.DAQ_id, 30.0);
        PowerTrace(event.window_size, trace);
	TraceFFT(event.window_size, trace, Amp[Aevent], Phase[Aevent], FFTData, true , true);
	for(unsigned int j=0; j<event.window_size/2; j++) Amp[Aevent][j] *= ( 12.5 / 1000.0 );
     }
     else
      CalibratedFrequencyDomain(event.window_size, RFITrace, Amp[Aevent], Phase[Aevent], header.DAQ_id, 30., RefAntCal, false);
   }
   else{
    if(power){
        ADC2FieldStrength(event.window_size, trace, header.DAQ_id, 30.0);
        PowerTrace(event.window_size, trace);
	TraceFFT(event.window_size, trace, Amp[Aevent], Phase[Aevent], FFTData, true, true);
	for(unsigned int j=0; j<event.window_size/2; j++) Amp[Aevent][j] *= ( 12.5 / 1000.0 );
    }
    else
     CalibratedFrequencyDomain(event.window_size, trace, Amp[Aevent], Phase[Aevent], header.DAQ_id, 30., RefAntCal);
   }
   

   //graph for Amp and Phase
   gAmp[(Aevent)] = new TGraph (event.window_size/2,freq,Amp[(Aevent)]);
   if(power) sprintf(name,"Power%i_%03i",channel_profiles.ch_id,(Aevent));	
   else sprintf(name,"Amp%i_%03i",channel_profiles.ch_id,(Aevent));	
   gAmp[(Aevent)]->SetName(name);
   gAmp[(Aevent)]->SetTitle(title);
   gAmp[(Aevent)]->GetXaxis()->SetTitle(" frequency / MHz ");
   if(power) gAmp[(Aevent)]->GetYaxis()->SetTitle(" power / (W #upoint m^{-2} #upoint MHz^{-1}) ");
   else gAmp[(Aevent)]->GetYaxis()->SetTitle(" field strength / (#muV #upoint m^{-1} #upoint MHz^{-1}) ");

   gPhase[(Aevent)] = new TGraph (event.window_size/2,freq,Phase[(Aevent)]);
   sprintf(name,"Phase%i_%03i",channel_profiles.ch_id,(Aevent));	
   gPhase[(Aevent)]->SetName(name);
   gPhase[(Aevent)]->SetTitle(title);
   gPhase[(Aevent)]->GetXaxis()->SetTitle("frequency / MHz");
   gPhase[(Aevent)]->GetYaxis()->SetTitle("phase / rad");
   
   sprintf(name,"canfft_%03i",(Aevent));	
   canfft[(Aevent)]= new TCanvas(name,title);  
 
} // end for over all traces and events
  


output = new TFile (OutputFileName,"Update","spectra viewer");
output->cd();

//Amp and Phase distribution of NoChannels
for(int i=0; i<(STOP-START); i++){
   canfft[i]->Divide(1,2);
   canfft[i]->cd(1);
   gPad->SetLogy();
   gAmp[i]->Draw("AL");
   canfft[i]->cd(2);
   gPhase[i]->Draw("AL");
   if(SingleSave){
     gPhase[i]->Write();
     gAmp[i]->Write();
   }
   canfft[i]->Write();
}


// mean freq spectra 
  char position[5];
  char pol[50];
  int division=(int)NoChannels/2;
  if((int)NoChannels%2 != 0 ) division++;

  char AntPos[NoChannels][50];
  char AntPol[NoChannels][50];
  for(int i=0; i<NoChannels; i++){
    if(bTeventExtend){
     if(Tevent->GetEntry(i)==0){
       cerr << "error - Tevent->GetEntry()" << endl;
       exit(0);
      }
    }
    else
     if(Tchannel_profile->GetEntry(i)==0){
       cerr << "error - Tchannel_profile->GetEntry()" << endl;
       exit(0);
    }
    strcpy(AntPos[i],channel_profiles.antenna_pos);
    strcpy(AntPol[i],channel_profiles.polarisation);
    strcat(AntPol[i],",");
//    cout << AntPos[i] << " " << AntPol[i] << endl;
  }

     canMeanF[0]->cd();
     canMeanF[0]->Divide(2,division);

     for(int j=0; j<NoChannels; j++){
       sscanf(MeanF[j]->GetTitle(),"%*c%*i_%c%c%c_%s CH0",&position[0], &position[1], &position[2],pol);
//cout << position << "-" << pol << "-" << endl;

      
       for(int a=0; a<NoChannels; a++){
         if(!strcmp(position,AntPos[a]) && !strcmp(pol,AntPol[a])){
//cout << a << " " <<  AntPos[a] << " " << AntPol[a] << " " << position << " " << pol << endl;
//           if(a>4 && header.DAQ_id==17 && header.run_id < 20 ) a++;
// cout << a << endl;	   
	   canMeanF[0]->cd(a+1);
	   gPad->SetLogy();
	   MeanF[j]->SetLineWidth(2);
	   MeanF[j]->SetFillStyle(3001);
           MeanF[j]->Draw("AL3"); 
           if(SingleSave){
	     MeanF[j]->Write();	   
	     MeanP[j]->Write();	   
	   }
	 }
       }
     }
     canMeanF[0]->Write();


//don't forget to clean up     

output->Close();
delete output;

for(int i=0; i<(STOP-START); i++){
  delete canfft[i];
  delete Amp[i];
  delete Phase[i];
}
delete[] canfft;
delete[] freq; 
delete[] Amp; 
delete[] Phase;
  
delete Tevent;
delete Theader;
delete Tchannel_profile;

}
