#include <iostream>

#include <fftw3.h>
#include <math.h>
#include <libgen.h>


#include <TROOT.h>
#include <TSystem.h>
#include <TGraph.h>
#include <TFile.h>
#include <time.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TMath.h>
#include <TPad.h>
#include <TRandom.h>

#include "util.hh"

//!definition of hardware parameter
#define BANDWIDTH 40e6
#define SAMPLE_RATE 80e6
#define LOWER_BAND_LIMIT 40e6
#define UPPER_BAND_LIMIT 80e6

using namespace std;
using namespace TMath;

//! analysis of the star traces
/*!

test enviroment for the analysis of the traces to develop new methods for 
new trigger algorithms.

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
	cout << " Overview: STAR trace analysis\n";
	cout << "==============================\n\n";
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
 if(0){
   //run 11 - shower timestamp for log(E) > 17.5
   int time_start_sec = 1140284359-10-3600*2;
//   int time_start_sec = 1140303436-10-3600*2;
//   int time_start_sec = 1140357470-10-3600*2;
//   int time_start_sec = 1140418565-10-3600*2;
   int Sec;
   int Nanosec;
   for(int w=0; w<Tevent->GetEntries(); w++){
     Tevent->GetEntry(w);
     START = w;
     ConvertTimeStamp(event.timestamp, &Sec, &Nanosec);
     if( (Sec+10)>=(time_start_sec) && (Sec-3600)<time_start_sec){
       cerr << "event found by time" << endl;
       break;
     }
     if( (Sec-7)>time_start_sec){
       cerr << "event NOT found by time" << endl;
       break;
     }
   }
  }
  else{
  unsigned int event_id_start = 0; //1389063 ;//1412319;//1389063; //9960424; //10417173; //10417499;//
   for(int w=START; w<Tevent->GetEntries(); w++){
     Tevent->GetEntry(w);
     START = w;
     if(event.event_id == event_id_start) {
       cerr << "event found by id" << endl;
       break;
     }

     if(event.event_id > event_id_start){
       cerr << "event NOT found by id" << endl;
       break;
     }
   }
  }
   
   Tevent->GetEntry(0);
   //cout << START << " START" << endl;

 int STOP = 1; // START+NoChannels*1;
 START = 0; STOP=1;
// cout << endl << endl << "window_size = " << event.window_size << endl;
///////////////////////////////
  
  if(STOP>= Tevent->GetEntries()){
   cerr << "entry doesn't exist" << endl;
   STOP = Tevent->GetEntries();
   cerr << "changed end number." <<  endl;
  // exit(1);
  }
  
  
  int ChId=0;
  char title[1024];
 
  float *TimeFFT = new float [event.window_size*2];
  for(unsigned int i=0; i<event.window_size*2; i++) TimeFFT[i]=(float)i;
  
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
  float *RawFFT = new float [event.window_size*2];
  
  TGraph **gAmp = new TGraph *[(STOP-START)];
  TGraph **gPhase = new TGraph *[(STOP-START)];
  
  int NoZeros = 7;
  int new_window_size = event.window_size*(NoZeros+1);
  float TraceZP_adc[STOP-START][new_window_size];
  float TraceZP_env[STOP-START][new_window_size];
  float time[new_window_size];
  for(int i=0; i<new_window_size; i++) time[i] = (float)((i*12.5)/(NoZeros+1)/1000.0);
  float trace_adc[STOP-START][event.window_size];
  
  
  //LPFilter
  int NoZero = 0;
  int order = 126*(NoZero+1)+1;
  double Dcoeff[order];
  float Fcoeff[order];
  float FilterFreq[order];
  for(int i=0; i<order; i++) FilterFreq[i] = float((float)i*40.0*(float)(NoZero+1)/(float)(order));
  
  //HPFilter
  int order_hp_basic = 4;
  int order_hp = order_hp_basic * (NoZero+1) + 1;
  double Dcoeff_hp[order_hp];
  float Fcoeff_hp[order_hp];
  
  //rectifier fft
  float FilterFreq2[new_window_size/2];
  for(int i=0; i<new_window_size/2; i++) FilterFreq2[i] = float((float)i*40.0*(float)(NoZeros+1)/(float)(new_window_size/2)); 

  int Aevent=-1;
  float tracedummy[event.window_size*2];
  
  
  for(int i=START; i<STOP; i++){
   Aevent++;
   
   if((Aevent)==((STOP-START))) break;
   
   if(Tevent->GetEntry(i)==0){
     cerr << "error - Tevent->GetEntry() -> main function" << endl;
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
    

   sprintf(title,"D%i_%s_%s, %s, run=%i, event_id=%i",header.DAQ_id, channel_profiles.antenna_pos, 
   		channel_profiles.polarisation, event.timestamp, header.run_id, event.event_id);
   for(unsigned int j=0; j<event.window_size; j++) tracedummy[j] = event.trace[j];   
   cout << endl << title << endl;

//***************************************************

 for(unsigned int a=0; a<event.window_size; a++)   trace_adc[Aevent][a] = event.trace[a];

//  RFISuppressionCutOff(event.window_size, trace_adc[Aevent], trace_adc[Aevent]);
  RFISuppressionMedian(event.window_size, trace_adc[Aevent], trace_adc[Aevent]);

/* Envelope with Hilbert Trafo */

  SubtractPedestal(event.window_size, trace_adc[Aevent]);
  RectifierHilbert(NoZeros, event.window_size, trace_adc[Aevent], TraceZP_adc[Aevent]);
  ZeroPaddingFFT(event.window_size, trace_adc[Aevent], NoZeros, TraceZP_env[Aevent]);
//  for(int a=0; a<new_window_size; a++) TraceZP_env[Aevent][a] = fabs(TraceZP_env[Aevent][a]);


  

/* Envelope with High and Low filtering */

/*  ZeroPaddingFFT(event.window_size, trace_adc[Aevent], NoZeros, TraceZP_adc[Aevent]);
  for(int a=0; a<new_window_size; a++) TraceZP_env[Aevent][a] = TraceZP_adc[Aevent][a];
  Rectifier(NoZeros, new_window_size, TraceZP_env[Aevent]);
  
  for(int a=0; a<new_window_size; a++){
    TraceZP_env[Aevent][a] = sqrt(TraceZP_env[Aevent][a]/0.76);
    TraceZP_adc[Aevent][a] = fabs(TraceZP_adc[Aevent][a]);
  }
*/


/* Rectifier with filtering in the frequency domain */
/*  ZeroPaddingFFT(event.window_size, trace_adc[Aevent], NoZeros, TraceZP_env[Aevent]);
  ZeroPaddingFFT(event.window_size, trace_adc[Aevent], NoZeros, TraceZP_adc[Aevent]);
  RectifierFFT(NoZeros, new_window_size, TraceZP_env[Aevent]); 
  for(int j=0; j<new_window_size; j++){
    TraceZP_adc[Aevent][j] = fabs(TraceZP_adc[Aevent][j]);
    TraceZP_env[Aevent][j] = sqrt( fabs(TraceZP_env[Aevent][j]/1.467) );
  }
*/


/* low and high filter illustration in the frequency domain */
  
/*  LPFilter(NoZero, order, (int)(40e6), Dcoeff);
  //Hann-Window
  sprintf(name,"0.5*(1+TMath::Cos(2*TMath::Pi()*x/(%i)))",order);
  TF1 HannWindow = TF1 ("HannWindow",name);
  for(int j=0; j<order; j++){
     Dcoeff[j] *= HannWindow.Eval(j-((order-1)/2));
     Fcoeff[j] = (float) Dcoeff[j];
  }
  
  TraceFFT(order, Fcoeff, Amp[Aevent], Phase[Aevent], RawFFT, true, true);
*/

  
/*  HPFilter(NoZero, order_hp, 500000, Dcoeff_hp);
  
  //Hann-Window
  sprintf(name,"0.5*(1+TMath::Cos(2*TMath::Pi()*x/(%i)))",order_hp);
  TF1 HannWindow_hp = TF1 ("HannWindow_hp",name);
  for(int j=0; j<order_hp; j++){
     Dcoeff_hp[j] *= HannWindow_hp.Eval(j-((order_hp-1)/2));
     Fcoeff_hp[j] = Dcoeff_hp[j];
  }
  TraceFFT(order_hp, Fcoeff_hp, Amp[Aevent], Phase[Aevent], RawFFT, true, true);
*/

//***************************************************
   //graph for Amp and Phase
//   gAmp[(Aevent)] = new TGraph (new_window_size, time , TraceZP_env[Aevent]); // envelope
//   gAmp[(Aevent)] = new TGraph (order, FilterFreq , Amp[Aevent]); // filter
  gAmp[(Aevent)] = new TGraph (new_window_size, time , TraceZP_env[Aevent]); 
   sprintf(name,"Amp%i_%03i",channel_profiles.ch_id,(Aevent));	
   gAmp[(Aevent)]->SetName(name);
   gAmp[(Aevent)]->SetTitle(title);
   gAmp[(Aevent)]->GetXaxis()->SetTitle(" frequency / arbitrary");
   gAmp[(Aevent)]->GetYaxis()->SetTitle(" |Amp| ");

   gPhase[(Aevent)] = new TGraph (new_window_size, time, TraceZP_adc[(Aevent)]); // envelope
//   gPhase[(Aevent)] = new TGraph (order, FilterFreq, Phase[Aevent]); // filter
   sprintf(name,"Phase%i_%03i",channel_profiles.ch_id,(Aevent));	
   gPhase[(Aevent)]->SetName(name);
   gPhase[(Aevent)]->SetTitle(title);
   gPhase[(Aevent)]->GetXaxis()->SetTitle("frequency / arbitrary");
   gPhase[(Aevent)]->GetYaxis()->SetTitle("phase / rad");
   gPhase[Aevent]->SetLineColor(2);
   
   sprintf(name,"canfft_%03i",(Aevent));	
   canfft[(Aevent)]= new TCanvas(name,title);  
   canfft[Aevent]->Divide(1,2);
   
   
  
   
  
  } // end for over all traces and events
  
 



  output = new TFile (OutputFileName,"Update","trace analaysis");
  
  output->cd();

    cout << "writting .." << endl;
    for(int i=0; i<(STOP-START); i++){
      canfft[i]->cd(1);
      gAmp[i]->Draw("AL");
//      gPad->SetLogy(); 
      gPhase[i]->Draw("SAMEL");
      canfft[i]->cd(2);
      gPhase[i]->Draw("AL");
      gAmp[i]->Draw("SAMEL");
      canfft[i]->Write();
      
      gAmp[i]->Write();
      gPhase[i]->Write();
    }





     
     
     

output->Close();

delete output;
  

}

