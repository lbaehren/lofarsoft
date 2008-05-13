#include <iostream>
#include <math.h>
#include <string.h>

#include <TROOT.h>
#include <TStyle.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TGraphErrors.h>
#include <TH2F.h>
#include <TChain.h>
#include <TMath.h>
#include <TPad.h>

#include "util.hh"

using namespace std;
using namespace TMath;

//! beamforming
/*!
  beamforming for a given shower around the reconstructed direction by plane fit
*/

// ------------------------------------------------------------------------------

void TimeDelay (int NoChannels,
		char **AntPos,
		int daq_id,
		float Zenith,
		float Azimuth,
		float *delay,
		bool debug= false)
{
  float Pos[3];
  float tZenith = Zenith * Pi() / 180.0;
  float tAzimuth = Azimuth;
  if(tAzimuth>360) tAzimuth -= 360;
  if(tAzimuth<0) tAzimuth = (360 - tAzimuth) * Pi() / 180.0;
  else tAzimuth = tAzimuth * Pi() / 180.0;
  
  float phi;
  for(int i=0; i<NoChannels; i++){
    GetAntPos(AntPos[i], daq_id, Pos);
    
    //ground distance
    delay[i] = ( sin(tZenith)*cos(tAzimuth)*Pos[0] + sin(tZenith)*sin(tAzimuth)*Pos[1] + cos(tZenith)*Pos[2]*0 );
    delay[i] /= C();
    
    if(debug) printf("%.12f -- channel %i\n",delay[i], i);
  }
#warning z component equal to zero    
}

// ------------------------------------------------------------------------------

float TraceShift (int window_size,
		  int NoZeros,
		  int position,
		  float *Trace,
		  float delay)
{
  float shifted = 0;
  
  //time between to 2 samples in seconds
  float time_step = (float)(12.5e-9) / (float)(NoZeros + 1.0);
  float time_position = time_step * (float)position;
  
  //correctd time position
  time_position -= delay;
  
  //corresponding positions
  int RealPosition = (int) (time_position / time_step);
  
  if(RealPosition<=1 || RealPosition>=window_size-1) shifted = 0;
  else
    shifted = (( (Trace[RealPosition]-Trace[RealPosition+1]) / (RealPosition*time_step - (RealPosition+1)*time_step)) * (time_position-(RealPosition+1)*time_step) + Trace[RealPosition+1]);
 
//     shifted = (Trace[RealPosition+1]-Trace[RealPosition])/((RealPosition+1)*time_step-RealPosition*time_step)*time_position + Trace[RealPosition] - (Trace[RealPosition+1]-Trace[RealPosition])/((RealPosition+1)*time_step-(RealPosition*time_step)) * RealPosition*time_step;
  
  return shifted;
}

// ------------------------------------------------------------------------------

void CalculateBeamforming (int NoChannels,
			   int window_size,
			   int NoZeros,
			   float **Trace,
			   float *delay,
			   float *BF_trace)
{
  int i (0);
  int j (0);

  for(i=0; i<window_size; i++){
    BF_trace[i] = 0;
    for(j=0; j<NoChannels; j++){
      BF_trace[i] += TraceShift(window_size, NoZeros, i, Trace[j], delay[j]);
    }
  }
  
}


void  MCSignals (int window_size,
		 int NoChannels,
		 float **Trace,
		 float *delay)
{
  //define signal
//  float signal[]={1,2,3,4,5,6,5,4,3,2,1};
  float signal[]={1,4,9,16,25,36,25,16,9,4,1};
  

  for(int j=0; j<NoChannels; j++)for(int i=0; i<window_size; i++) Trace[j][i] = 0;
  int signal_count = 0;
  for(int j=0; j<NoChannels; j++){
    signal_count = 0;
    for(int i=0; i<window_size; i++){
/*      if(j==0 || j==1) delay[j] = 0*80/8*12.5e-9;
      if(j==2 || j==3) delay[j] = 0*80/8*12.5e-9;
      if(j==4 || j==5) delay[j] = 0*80/8*12.5e-9;
      if(j==6 || j==7) delay[j] = 0*80/8*12.5e-9;
*/    
      if(i>window_size/2 && signal_count<11){
        Trace[j][i+(int)(delay[j]*8/(12.5e-9))] = signal[signal_count];
	signal_count++;
      }
    }
  }
}
    
int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);


  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  

  
  char *InputFileName="$HOME/analysis/default_00.root";
  char *OutputFileName="$HOME/analysis/BF_out.root";
  
  unsigned int event_id_start = 0;
  bool bRFISupp = true;
  bool debug = true;
  
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "hf:i:e:r")) != -1) //: after var ->Waits for input
    switch (c)
      {
      case 'f':
	OutputFileName = optarg ;
	break;
      case 'r':
	bRFISupp = false;
	break;
      case 'i':
	InputFileName = optarg ;
	break;
      case 'e':
	event_id_start = (unsigned int) atoi(optarg) ;
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
	cout << " Overview: Beamforming\n";
	cout << "======================\n\n";
	cout << " -h	HELP -- you already tried :-)\n";
	cout << " -f	output file name (default: " << OutputFileName << ")\n";
	cout << " -i	input file name in STAR format (default: " << InputFileName << ")\n\n";
	
	cout << " -e	event id for beamforming (default: first in given file)\n";
	cout << " -r	disable RFI suppression for beamforming\n";
	cout << "\n\n\nmailto: asch@ipe.fzk.de\n";
	exit(1);
        break;      
      default:
	abort ();
      }


  int NoChannels=0;
  int NoAntennas=0;
  static struct event event;
  static struct header header;
  static struct channel_profiles channel_profiles;

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
  
  NoChannels -= 2;
  
  NoAntennas = NoChannels/2;
  if( (NoChannels % 2) > 0 ) NoAntennas++;

  
   
if(debug) cout << "no antennas = " << NoAntennas << endl;
///////////////////////////////   
  int START = 0;
 
  
   for(int w=0; w<Tevent->GetEntries(); w++){
     Tevent->GetEntry(w);
     START = w;
     if(event.event_id == event_id_start){
       cout << "found event by id" << endl;
       break;
     }
     if(event.event_id > event_id_start){
       cout << "did NOT find event by id" << endl;
       exit(1);
     }
   }

 int STOP =  START + NoChannels;
 if(debug) cout << "START: " << START << endl;
 if(debug) cout << "STOP: " << STOP << endl;
///////////////////////////////
  
  if(STOP> Tevent->GetEntries()){
   cerr << "entry doesn't exist" << endl;
   STOP = Tevent->GetEntries();
   cerr << "changed end number." <<  endl;
  // exit(1);
  }
  
  if(debug) cout << "event id = " << event.event_id << endl;
  
  int NoZeros = 7;
  int new_window_size = event.window_size * (NoZeros + 1);
  float **Trace =  new float *[NoChannels];

  char **AntPos = new char *[NoChannels];
  float **rawTrace = new float *[NoChannels];
  for(int i=0; i<NoChannels; i++){
    rawTrace[i] = new float [event.window_size];
    Trace[i] = new float [new_window_size];
    AntPos[i] = new char [16];
    if( Tevent->GetEntry(START+i) == 0) cerr << "error readout input file" << endl;
    for(unsigned int j=0; j<event.window_size; j++){
      rawTrace[i][j] = event.trace[j];
      Trace[i][j] = event.trace[j];
    }
    strcpy(AntPos[i],channel_profiles.antenna_pos);
  }
  
  float Zenith[2], Azimuth[2];
  RecDirectionSingleArray(NoChannels, event.window_size, rawTrace, AntPos, header.DAQ_id, Zenith, Azimuth);
  if(debug) cout << "Azimuth = " << Azimuth[0] << " +/- " << Azimuth[1] << endl;  
  if(debug) cout << "Zenith = " << Zenith[0] << " +/- " << Zenith[1] << endl;
  
//  Azimuth[0] = 180+15.7;
//  Zenith[0] = 45;
  
  
  float A[1], AE[1], Z[1], ZE[1];
  A[0] = Azimuth[0];
  AE[0] = Azimuth[1];
  Z[0] = Zenith[0];
  ZE[0] = Zenith[1];  
  
  TGraphErrors *g_ShowerPos = new TGraphErrors(1,A,Z,AE,ZE);
  g_ShowerPos->SetLineColor(0);
  g_ShowerPos->SetLineWidth(3);
  g_ShowerPos->SetName("RecDirection");
  g_ShowerPos->SetTitle("RecDirection");
  
  //correction for Grande grid
  Azimuth[0] -= 15.7;

  float expectedPulsePosition (0);
  int countPulse (0);
//   float PulsePosition, PulseLength, tmpMax, tmpInt, Int_Post, Int_Pulse, tmpthreshold, snr;
//   int NoPulse (0);

  for(int i=0; i<NoChannels; i++){
    if(bRFISupp) RFISuppression(event.window_size, Trace[i], Trace[i]);
    
//    SNRandNoPulses(event.window_size, Trace[i], &snr, &NoPulse);
//    DynamicThreshold(event.window_size, Trace[i], &tmpthreshold, 7);
//    EstimatePulseParameter(event.window_size, Trace[i], &PulseLength, &PulsePosition, &tmpMax, &tmpInt, &Int_Post, &Int_Pulse, tmpthreshold, 7);

//    if(NoPulse == 1 && snr > 75 && Int_Post/Int_Pulse < 1 && PulseLength < 10){
//      if(debug) printf("Pulse Position = %.4f mus of channel %i\n",PulsePosition*12.5e-3,i);
//      expectedPulsePosition += PulsePosition;
//      countPulse++;
//    }
    ZeroPaddingFFT(event.window_size, Trace[i], NoZeros, Trace[i]);     
//RectifierHilbert(new_window_size, Trace[i]);

  }
  if(countPulse>0) expectedPulsePosition /= countPulse;
  
  //calculate delay per channel
  float *delay = new float [NoChannels];
  for(int i=0; i<NoChannels; i++) delay[i] = 0;
//  TimeDelay(NoChannels, AntPos, header.DAQ_id, Zenith[0], Azimuth[0], delay, debug);

  //fill in defined signals
  bool bmc = false;
  if(bmc) MCSignals(new_window_size, NoChannels, Trace, delay);
  
  float MeanDelay = 0;
  for(int i=0; i<NoChannels; i++) MeanDelay += delay[i];
  MeanDelay /= NoChannels;
  if(debug) printf("mean delay per channel = %.12f\n",MeanDelay);
  
  float AzimuthStart = Azimuth[0] - 20;
  float AzimuthStop = Azimuth[0] + 20;
  float AzimuthStep = 2;
  float ZenithStart = Zenith[0] - 20;
  float ZenithStop = Zenith[0] + 20;
  float ZenithStep = 2;

/*  float AzimuthStart = 0;
  float AzimuthStop = 360;
  float AzimuthStep = 10;
  float ZenithStart = 0;
  float ZenithStop = 90;
  float ZenithStep = 10;
*/
  if(ZenithStart>90) ZenithStart = 90;
  if(ZenithStop<0) ZenithStop = 0;
  
  int NumberOfPixels = (int) (((AzimuthStop-AzimuthStart)/AzimuthStep+1) * ((ZenithStop-ZenithStart)/ZenithStep+1));
  int countPixel=0;
  float **BF_trace = new float *[NumberOfPixels];
  for(int i=0; i<NumberOfPixels; i++) BF_trace[i] = new float [new_window_size];
  
  TGraph **g_bf_trace = new TGraph *[NumberOfPixels];
  float *timeaxis = new float [new_window_size];
  for(int i=0; i<new_window_size; i++) timeaxis[i] = (float)((i*12.5)/(NoZeros+1)/1000.0);
  char name[1024];

  //time postion in beamforming trace
  float *tBF_trace = new float [new_window_size];
  int RealPosition = 0;
  TimeDelay(NoChannels, AntPos, header.DAQ_id, Zenith[0], Azimuth[0], delay);
  for(int k=0; k<NoChannels; k++) {
//     delay[k] -= MeanDelay;
  }
  CalculateBeamforming(NoChannels, new_window_size, NoZeros, Trace, delay, tBF_trace);
if(!bmc) RectifierHilbert(new_window_size, tBF_trace);
  float ftmp = MaxElement(new_window_size, tBF_trace);
  for(int i=0; i<new_window_size; i++){
    if(tBF_trace[i]+0.001>ftmp && tBF_trace[i]-0.001<ftmp){
      RealPosition = i;
      break;
    }
  }
  if(debug) cout << "real position = " << RealPosition << endl;

  //fill TH2F histogramm
  int hisStep = 20; // new_window_size/2-2; // 20
  TH2F **his = new TH2F *[hisStep*2+1];
  for(int i=0; i<hisStep*2+1; i++){ 
    sprintf(name,"his%03i",i);
    his[i] = new TH2F(name,name,(int)((AzimuthStop-AzimuthStart)/AzimuthStep+1), AzimuthStart+15.7, AzimuthStop+15.7, (int)((ZenithStop-ZenithStart)/ZenithStep+1), ZenithStart,ZenithStop);
    sprintf(name,"Beamforming: step = %i (%.2f ns)",i, 12.5/(float)(NoZeros+1));
    his[i]->SetTitle(name);
    his[i]->GetXaxis()->SetTitle("azimuth / degree");
    his[i]->GetYaxis()->SetTitle("zenith / degree");
  }
  
  TH1F *hisdelay = new TH1F ("hisdelay","hisdelay",10000,-5.e-6,5.e-6);
  
  int AzimuthCount = 1;
  int ZenithCount = 1;
  for(float i=AzimuthStart; i<=AzimuthStop; i += AzimuthStep){
    for(float j=ZenithStart; j<=ZenithStop; j += ZenithStep){
      //status output
      printf("\tAzimuth = %.2f, Zenith = %.2f\r",i, j);
      fflush (stdout);

      TimeDelay(NoChannels, AntPos, header.DAQ_id, j, i, delay);

      for(int k=0; k<NoChannels; k++) {
//        delay[k] -= MeanDelay;
        hisdelay->Fill(delay[k]);
      }

      CalculateBeamforming(NoChannels, new_window_size, NoZeros, Trace, delay, BF_trace[countPixel]);
    
    
if(!bmc) RectifierHilbert(new_window_size, BF_trace[countPixel]);
     for(int k=0; k<new_window_size; k++){
       BF_trace[countPixel][k] = pow(BF_trace[countPixel][k],2);
     }

    
      for(int k=0; k<hisStep*2+1; k++){
        his[k]->SetBinContent(AzimuthCount, ZenithCount,  BF_trace[countPixel][/*RealPosition*/+(-hisStep-1+k)*2]);
      }
if(j==0 && 0){
  float tt = MaxElement(new_window_size,BF_trace[countPixel]);
  cout << "bf content = " << tt << endl;
  for(int k=0; k<new_window_size; k++){
   if(BF_trace[countPixel][k]+0.001>tt && BF_trace[countPixel][k]-0.001<tt) cout << "position = " << k << endl;
  }
  cout << "real position = " << RealPosition << endl;
}
    
      g_bf_trace[countPixel] = new TGraph(new_window_size, timeaxis, BF_trace[countPixel]);
      sprintf(name,"Azimuth = %.2f, Zenith = %.2f",i,j);
      g_bf_trace[countPixel]->SetTitle(name);
      sprintf(name,"bf_trace%04i",countPixel);
      g_bf_trace[countPixel]->SetName(name);
      countPixel++;
      ZenithCount++;
    }//for over zenith angle
    AzimuthCount++;
    ZenithCount=1;
  }//for over azimuth angle
  
  cout << endl;
  if(debug) cout << "real postion of shower = " << (float)RealPosition/(float)(NoZeros+1)*12.5e-3  << "mus" << endl;
  if(debug) cout << "differnce to expected position = " << fabs(expectedPulsePosition*12.5e-3-RealPosition/(NoZeros+1)*12.5e-3) << "mus" << endl;
  if(debug) cout << "expected Pulse Position = " << expectedPulsePosition*12.5e-3 << "mus mean over " << countPulse << " pulses" << endl;
  

  int NoHis;
  TTree *tree = new TTree("info","info");
  tree->Branch("NoHis",&NoHis,"NoHis/I");
  
  NoHis = hisStep*2+1;
  
  tree->Fill();	
  
  TFile *output = new TFile (OutputFileName,"RECREATE","beamforming");
  for(int i; i<hisStep*2+1; i++) his[i]->Write();
//  for(int i=0; i<NumberOfPixels; i++) g_bf_trace[i]->Write();
  g_ShowerPos->Write();
  hisdelay->Write();
  tree->Write();


//clean up
output->Close();
delete output;


delete Tevent;
delete Theader;
delete Tchannel_profile;

}

