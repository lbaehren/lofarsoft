#include <iostream>
#include <math.h>

#include <TROOT.h>
#include <TFile.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TEventList.h>
#include <TMath.h>

#include "util.hh"


using namespace std;

//! run flag rec event
/*!
reconstruction of given raw data combined with reconstruction of Grande
*/

float GetLDFSlope(int NoChannels, float *FieldStrengthAntenna, float *DistanceShowerAxis){
  if(NoChannels>20){
    cerr << "max number of channels per event reached.. exit";
    exit(1);
  }
  
  float slope = 0;
  int tmpNoChannels = 0;
  float x[20], y[20];
  
  for(int i=0; i<NoChannels; i++){
    if(FieldStrengthAntenna[i] > 0 ){
      x[tmpNoChannels] = DistanceShowerAxis[i];
      y[tmpNoChannels] = FieldStrengthAntenna[i];
      tmpNoChannels++;
    }
  }
  
  TGraph *graph = new TGraph(tmpNoChannels, x, y);
  TF1 *fit = new TF1("fit","[0] + [1] * x");
  graph->Fit("fit","Q");
  slope = fit->GetParameter(1);
  
  delete graph;
  delete fit;
  
  return slope;
}

/**********************************************************************************************/

float EstimateTraceError(int window_size, float *trace, int ExpectedPulsePos, int UsedNoZeros=0){
  float Error = 0;
  float *tmpTrace = new float [window_size];
  int count = 0;
  
  for(int i=0; i<ExpectedPulsePos - (15*(UsedNoZeros+1)); i++){
    tmpTrace[count] = trace[i];
    count++;
  }
  for(int i = ExpectedPulsePos + (15*(UsedNoZeros+1)); i<window_size; i++){
    tmpTrace[count] = trace[i];
    count++;
  }
  
  Error = GetMean(count,tmpTrace) + 5.0 * GetSigma(count,tmpTrace);
//  Error = GetRMS(count,tmpTrace);
  
//  cout << "count = " << count << " -- window size = " << window_size << endl;
//  cout << "mean = " << GetMean(count, tmpTrace) << " -- sigma = " << GetSigma(count, tmpTrace) <<  " -- RMS = " << GetRMS(count, tmpTrace) << endl;
  
  delete[] tmpTrace;
  return Error;
}

/**********************************************************************************************/


int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();


  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  

  
  char *fileName_AFOutput="$HOME/analyisis/AF_default.root";
  char *fileName_rawInput="$HOME/analysis/default_00.root";
  char *TGrandeCut="ana_index==2 && L1Trigger==1";
  bool debug = false;

  
  
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "hf:i:c:")) != -1) //: after var ->Waits for input
    switch (c)
      {
      case 'i':
	fileName_rawInput = optarg ;
	break;
      case 'f':
	fileName_AFOutput = optarg ;
	break;
      case 'c':
	TGrandeCut = optarg ;
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
	cout << " -f	input file from RunFlagMergeGrande (default: " << fileName_AFOutput << ")\n";
	cout << " -i	input file name in STAR format (default: " << fileName_rawInput << ")\n";
	cout << " -c	cut on TGrande tree (default: " << TGrandeCut << ")\n";
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
  CreateTChain(Tevent, fileName_rawInput);

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
    Theader->Add(fileName_rawInput);
    Tchannel_profile->Add(fileName_rawInput);
  
    CreateTevent(Tevent, &event);
    CreateTheader(Theader, &header);
    CreateTchannel_profiles(Tchannel_profile, &channel_profiles);

    Tevent->GetEntry(0);
    Theader->GetEntry(0);
    NoChannels = Tchannel_profile->GetEntries();
  }

  if(debug) cout << "NoChannels = " << NoChannels << endl;
  TFile *output = new TFile (fileName_AFOutput,"Update","RunFlagOut");
  
  
  //set up variables for AnaFlag
  TChain *TAnaFlag = new TChain("TAnaFlag");
  TAnaFlag->Add(fileName_AFOutput);
  static struct AnaFlag AnaFlag;
  CreateTAnaFlag(TAnaFlag, &AnaFlag);
  
  //set up variables for Grande (h3)  
  TChain *TGrande = new TChain("TGrande");
  TGrande->Add(fileName_AFOutput);
  if(TGrande==0) cerr << "pointer to TGrande chain are null" << endl;
  static struct h3 h3;
  CreateTGrande(TGrande, &h3, false);
 
  static struct RecEvent RecEvent;
  static struct h3 h3_out;
  
  TTree *TRecEvent = new TTree("TRecEvent","STAR shower reconstruction");
  TRecEvent->Branch("event_id",&RecEvent.event_id,"event_id/I");
  TRecEvent->Branch("DetCh",&RecEvent.DetCh,"DetCh/I");
  TRecEvent->Branch("run_id",&RecEvent.run_id,"run_id/I");
  TRecEvent->Branch("daq_id",&RecEvent.daq_id,"daq_id/I");
  TRecEvent->Branch("channel_id",&RecEvent.channel_id,"channel_id[DetCh]/I");		    

  TRecEvent->Branch("Azimuth1ThirdFit",&RecEvent.Azimuth1ThirdFit,"Azimuth1ThirdFit/F");
  TRecEvent->Branch("Azimuth1ThirdFit_err",&RecEvent.Azimuth1ThirdFit_err,"Azimuth1ThirdFit_err/F");
  TRecEvent->Branch("Zenith1ThirdFit",&RecEvent.Zenith1ThirdFit,"Zenith1ThirdFit/F");
  TRecEvent->Branch("Zenith1ThirdFit_err",&RecEvent.Zenith1ThirdFit_err,"Zenith1ThirdFit_err/F");
  TRecEvent->Branch("AzimuthFWHMFit",&RecEvent.AzimuthFWHMFit,"AzimuthFWHMFit/F");
  TRecEvent->Branch("AzimuthFWHMFit_err",&RecEvent.AzimuthFWHMFit_err,"AzimuthFWHMFit_err/F");
  TRecEvent->Branch("ZenithFWHMFit",&RecEvent.ZenithFWHMFit,"ZenithFWHMFit/F");
  TRecEvent->Branch("ZenithFWHMFit_err",&RecEvent.ZenithFWHMFit_err,"ZenithFWHMFit_err/F");

  TRecEvent->Branch("Azimuth1Third",&RecEvent.Azimuth1Third,"Azimuth1Third/F");
  TRecEvent->Branch("Azimuth1Third_err",&RecEvent.Azimuth1Third_err,"Azimuth1Third_err/F");
  TRecEvent->Branch("Zenith1Third",&RecEvent.Zenith1Third,"Zenith1Third/F");
  TRecEvent->Branch("Zenith1Third_err",&RecEvent.Zenith1Third_err,"Zenith1Third_err/F");
  TRecEvent->Branch("AzimuthFWHM",&RecEvent.AzimuthFWHM,"AzimuthFWHM/F");
  TRecEvent->Branch("AzimuthFWHM_err",&RecEvent.AzimuthFWHM_err,"AzimuthFWHM_err/F");
  TRecEvent->Branch("ZenithFWHM",&RecEvent.ZenithFWHM,"ZenithFWHM/F");
  TRecEvent->Branch("ZenithFWHM_err",&RecEvent.ZenithFWHM_err,"ZenithFWHM_err/F");
  TRecEvent->Branch("AzimuthMax",&RecEvent.AzimuthMax,"AzimuthMax/F");
  TRecEvent->Branch("AzimuthMax_err",&RecEvent.AzimuthMax_err,"AzimuthMax_err/F");
  TRecEvent->Branch("ZenithMax",&RecEvent.ZenithMax,"ZenithMax/F");
  TRecEvent->Branch("ZenithMax_err",&RecEvent.ZenithMax_err,"ZenithMax_err/F");
  TRecEvent->Branch("BAngle",&RecEvent.BAngle,"BAngle/F");
  TRecEvent->Branch("EPhi",&RecEvent.EPhi,"EPhi[DetCh]/F");
  TRecEvent->Branch("ETheta",&RecEvent.ETheta,"ETheta[DetCh]/F");

  TRecEvent->Branch("timestamp",RecEvent.timestamp,"timestamp/C");

  TRecEvent->Branch("description",RecEvent.description,"description/C");

  TRecEvent->Branch("PulseLength1ThirdFit",&RecEvent.PulseLength1ThirdFit,"PulseLength1ThirdFit[DetCh]/F");
  TRecEvent->Branch("PulseLengthFWHMFit",&RecEvent.PulseLengthFWHMFit,"PulseLengthFWHMFit[DetCh]/F");
  TRecEvent->Branch("PulsePos1ThirdFit",&RecEvent.PulsePos1ThirdFit,"PulsePos1ThirdFit[DetCh]/F");
  TRecEvent->Branch("PulsePosFWHMFit",&RecEvent.PulsePosFWHMFit,"PulsePosFWHMFit[DetCh]/F");

  TRecEvent->Branch("PulseLength1Third",&RecEvent.PulseLength1Third,"PulseLength1Third[DetCh]/F");
  TRecEvent->Branch("PulseLengthFWHM",&RecEvent.PulseLengthFWHM,"PulseLengthFWHM[DetCh]/F");
  TRecEvent->Branch("PulsePosMax",&RecEvent.PulsePosMax,"PulsePosMax[DetCh]/F");
  TRecEvent->Branch("PulsePos1Third",&RecEvent.PulsePos1Third,"PulsePos1Third[DetCh]/F");
  TRecEvent->Branch("PulsePosFWHM",&RecEvent.PulsePosFWHM,"PulsePosFWHM[DetCh]/F");
  TRecEvent->Branch("SNR",&RecEvent.SNR,"SNR[DetCh]/F");

  TRecEvent->Branch("DistanceShowerAxis",&RecEvent.DistanceShowerAxis,"DistanceShowerAxis[DetCh]/F");
  TRecEvent->Branch("DistanceShowerAxisErr",&RecEvent.DistanceShowerAxisErr,"DistanceShowerAxisErr[DetCh]/F");

  TRecEvent->Branch("ShowerCoreXRadio",&RecEvent.ShowerCoreXRadio,"ShowerCoreXRadio/F");
  TRecEvent->Branch("ShowerCoreYRadio",&RecEvent.ShowerCoreYRadio,"ShowerCoreYRadio/F");

  TRecEvent->Branch("FieldStrengthChannelFit",&RecEvent.FieldStrengthChannelFit,"FieldStrengthChannelFit[DetCh]/F");
  TRecEvent->Branch("FieldStrengthAntennaFit",&RecEvent.FieldStrengthAntennaFit,"FieldStrengthAntennaFit[DetCh]/F");

  TRecEvent->Branch("FieldStrengthChannel",&RecEvent.FieldStrengthChannel,"FieldStrengthChannel[DetCh]/F");
  TRecEvent->Branch("FieldStrengthAntenna",&RecEvent.FieldStrengthAntenna,"FieldStrengthAntenna[DetCh]/F");
  
  TRecEvent->Branch("ChannelErrFieldStrengthFit",&RecEvent.ChannelErrFieldStrengthFit,"ChannelErrFieldStrengthFit[DetCh]/F");
  TRecEvent->Branch("ChannelErrFieldStrength",&RecEvent.ChannelErrFieldStrength,"ChannelErrFieldStrength[DetCh]/F");
  TRecEvent->Branch("ChannelErrBackground",&RecEvent.ChannelErrBackground,"ChannelErrBackground[DetCh]/F");
  
  TRecEvent->Branch("LDFSlope",&RecEvent.LDFSlope,"LDFSlope/F");

  TRecEvent->Branch("Gt",&h3_out.Gt,"Gt/i");			
  TRecEvent->Branch("Mmn",&h3_out.Mmn,"Mmn/i");			
  TRecEvent->Branch("Fanka",&h3_out.Fanka,"Fanka/b");		
  TRecEvent->Branch("Hit7",&h3_out.Hit7,"Hit7/i"); 		
  TRecEvent->Branch("Nflg",&h3_out.Nflg,"Nflg/B"); 		
  TRecEvent->Branch("Ageg",&h3_out.Ageg,"Ageg/F"); 		    		 
  TRecEvent->Branch("Sizeg",&h3_out.Sizeg,"Sizeg/F");		
  TRecEvent->Branch("Sizmg",&h3_out.Sizmg,"Sizmg/F");		
  TRecEvent->Branch("Idmx",&h3_out.Idmx,"Idmx/B"); 		
  TRecEvent->Branch("Ieve",&h3_out.Ieve,"Ieve/i"); 		
  TRecEvent->Branch("Irun",&h3_out.Irun,"Irun/s"); 		 
  TRecEvent->Branch("Ifil",&h3_out.Ifil,"Ifil/b");   		
  TRecEvent->Branch("Ngrs",&h3_out.Ngrs,"Ngrs/B"); 		
  TRecEvent->Branch("Iact",&h3_out.Iact,"Iact/s"); 		
  TRecEvent->Branch("Xcg",&h3_out.Xcg,"Xcg/F");			
  TRecEvent->Branch("Ycg",&h3_out.Ycg,"Ycg/F");			
  TRecEvent->Branch("Azg",&h3_out.Azg,"Azg/F");			
  TRecEvent->Branch("Zeg",&h3_out.Zeg,"Zeg/F");			
  TRecEvent->Branch("Ymd",&h3_out.Ymd,"Ymd/i");			
  TRecEvent->Branch("Hms",&h3_out.Hms,"Hms/i");
  TRecEvent->Branch("Eg",&h3_out.Eg,"Eg/F");
  
  SetStruct2Zero(&RecEvent);
  SetStruct2Zero(&h3_out);
  
  int NoZeros = 7;
  char ctmp[4096];

  //set description
  strcpy(RecEvent.description,"cali with PulsePosMax, ");
  strcat(RecEvent.description,TGrandeCut);
  sprintf(ctmp,", NoZeros = %i",NoZeros);
  strcat(RecEvent.description,ctmp);
  
  RecEvent.DetCh = NoChannels;
  
#warning algorithm needed to fill these variables
  RecEvent.ShowerCoreXRadio = -1;
  RecEvent.ShowerCoreYRadio = -1;
  
  if(debug) cout << RecEvent.description << endl;
  
  int new_window_size = event.window_size * (1 + NoZeros);
  float **RawTrace = new float *[NoChannels];
  for(int i=0; i<NoChannels; i++) RawTrace[i] = new float[event.window_size];
    
  float **UpTrace = new float *[NoChannels];
  for(int i=0; i<NoChannels; i++) UpTrace[i] = new float[new_window_size];
  
  char **AntPos = new char *[NoChannels];
  for(int i=0; i<NoChannels; i++) AntPos[i] = new char [5];
  char **AntPol = new char *[NoChannels];
  for(int i=0; i<NoChannels; i++) AntPol[i] = new char [20];
  
  //tmp variables for direction reconsruction and analysis
  char **tmpAntPos = new char *[NoChannels];
  for(int i=0; i<NoChannels; i++) tmpAntPos[i] = new char [5];
  float tmpPulseAmp[20];
  float tmpPulseTime[20];
  float tmpf;
  int tmpCount = 0;
  float Zenith[2], Azimuth[2];
  float threshold[20];
  int ExpectedPosition[20];
  int delta;
  bool bADCOverflow;
  
  int SaveCounter = 0;
  int RawEventCounter = 0;
  bool RawEventFound;
  
  TGrande->AddFriend("TAnaFlag");
  TGrande->Draw(">>ListGrande",TGrandeCut);
  TEventList *EventListGrande = (TEventList*) gDirectory->Get("ListGrande");
  if(EventListGrande==NULL) cerr << "error - TEventList for Grande is null " << endl;
  TGrande->SetEventList(EventListGrande);

  TAnaFlag->AddFriend("TGrande");
  TAnaFlag->Draw(">>ListSTAR",TGrandeCut);
  TEventList *EventListSTAR = (TEventList*) gDirectory->Get("ListSTAR");
  if(EventListSTAR==NULL) cerr << "error - TEventList for STAR is null " << endl;
  TAnaFlag->SetEventList(EventListSTAR);

  if(EventListSTAR->GetN() != EventListGrande->GetN() ){
    cerr << "number of entries in EventList STAR and Grande are different!" <<  endl;
    exit(1);
  }

  if(debug) cout << "EventListSTAR entries = " << EventListSTAR->GetN() << endl;
  if(debug) cout << "EventListGrande entries = " << EventListGrande->GetN() << endl;
  
  int START = 0;
  int STOP = EventListGrande->GetN();
  
  if(EventListGrande->GetN()>0)
  for(int i=START; i<STOP; i++){
  
   //status output
   printf("\t%i of %i\r",i, STOP);
   if(!debug) fflush (stdout);
   else cout << endl;
   
   //temp save of data
   SaveCounter++;
   if(SaveCounter == 1000){
     output->cd();
     TRecEvent->Write(TRecEvent->GetName(),TObject::kWriteDelete);
     SaveCounter=0;
   }

   TGrande->GetEntry(EventListGrande->GetEntry(i));
   TAnaFlag->GetEntry(EventListGrande->GetEntry(i));
   h3_out = h3;

   if(debug) cout << "event id = " << AnaFlag.event_id << endl;
   if(debug){ cout << "get raw data and do channel analysis ... "; fflush (stdout);}
   //get raw data per event
   RawEventFound = false;
   for(int j=RawEventCounter; j<Tevent->GetEntries(); j += NoChannels){
     Tevent->GetEntry(j);
     if(AnaFlag.event_id == (int)event.event_id){
       RawEventFound=true;
       for(int k=0; k<NoChannels; k++){
         if(debug){ cout << " " << k ; fflush(stdout);}
         if(Tevent->GetEntry(j+k)==0){
           cerr << "error - Tevent->GetEntry()" << endl;
           break;
         }
	 bADCOverflow = false;
	 for(unsigned int l=0; l<event.window_size; l++) RawTrace[k][l] = event.trace[l];
	 RecEvent.channel_id[k] = channel_profiles.ch_id;
	 strcpy(AntPos[k], channel_profiles.antenna_pos);
	 strcpy(AntPol[k], channel_profiles.polarisation);
	 
	 //RFISuppression
	 RFISuppressionMedian(event.window_size, RawTrace[k], RawTrace[k]);
	 
	 //up-sampling
	 ZeroPaddingFFT(event.window_size, RawTrace[k], NoZeros, UpTrace[k]);
	 
	 for(int l=0; l<new_window_size; l++){
	   if( fabs(UpTrace[k][l]) > pow(2.,12.)/2*0.95 ){
	      if(debug) cout << "\nADC overflow after up-sampling " <<  UpTrace[k][l] << " -- " << AntPos[k] << " -- " << AntPol[k] << endl ;
	      for(int m=0; m<new_window_size; m++) UpTrace[k][m] = 0.;
	      bADCOverflow = true;
	      break;
	   }
	 }
	 
	 //Hilbert Rectifier
         RectifierHilbertSquare(new_window_size, UpTrace[k]);

	 //signal to noise ratio
	 if(!bADCOverflow)
	   RecEvent.SNR[k] = SNR(new_window_size, UpTrace[k], 0, false);
	 else
	   RecEvent.SNR[k] = 0;
	   
         //Dynamic Threshold
         DynamicThreshold(new_window_size, UpTrace[k], &threshold[k], 0, false);
	 //threshold[k] = sqrt(threshold[k]);
       }
       tmpCount = j;
       break;
     }
   } // end for over raw event
   RawEventCounter = tmpCount;
   if(!RawEventFound){
     cerr << "no raw event found, continue loop ... " << endl;
     continue;
   }
   if(debug) cout << " done" << endl;

   RecEvent.run_id = header.run_id;
   RecEvent.daq_id = header.DAQ_id;
   RecEvent.event_id = AnaFlag.event_id;
   strcpy(RecEvent.timestamp,AnaFlag.timestamp);
   
   //dynamic peak detection
   for(int j=0; j<20; j++) ExpectedPosition[j] = 0;
   delta = (int) (GetCoincidenceTime(RecEvent.daq_id, 65.0) / (12.5) * (NoZeros+1) * 1);
   PeakDetection(NoChannels, threshold, new_window_size, UpTrace, ExpectedPosition, delta);

   if(debug){ cout <<"direction reconstruction ... "; fflush (stdout);}
   //direction reconstruction and pulse positions
   //1Third
   if(1 && RecEvent.daq_id != 19){
     tmpCount = 0;
     for(int j=0; j<NoChannels; j++){
       if(ExpectedPosition[j]>0){
         strcpy(tmpAntPos[tmpCount], AntPos[j]);
	 tmpPulseAmp[tmpCount] = 1;
	 RecEvent.PulseLength1Third[j] = VarPeakWidth(new_window_size, UpTrace[j], 0.33, &RecEvent.PulsePos1Third[j], &ExpectedPosition[j], 0, false);
	 //conversion to seconds
	 RecEvent.PulsePos1Third[j]    *= (12.5 / (NoZeros +1)) * (1.0e-9);
	 RecEvent.PulseLength1Third[j] *= (12.5 / (NoZeros +1)) * (1.0e-9);
	 tmpPulseTime[tmpCount] = RecEvent.PulsePos1Third[j];
	 tmpCount++;
       }
       else{
         RecEvent.PulsePos1Third[j]    = 0;
	 RecEvent.PulseLength1Third[j] = 0;
       }
     }
     PlaneFit(tmpCount, tmpAntPos, RecEvent.daq_id, tmpPulseAmp, tmpPulseTime, Zenith, Azimuth);
     RecEvent.Zenith1Third      = Zenith[0];
     RecEvent.Zenith1Third_err  = Zenith[1];
     RecEvent.Azimuth1Third     = Azimuth[0];
     RecEvent.Azimuth1Third_err = Azimuth[1];
   }
   //1Third with gaus fit
   if(1 && RecEvent.daq_id != 19){
     tmpCount = 0;
     for(int j=0; j<NoChannels; j++){
       if(ExpectedPosition[j]>0){
         strcpy(tmpAntPos[tmpCount], AntPos[j]);
	 tmpPulseAmp[tmpCount] = 1;
	 RecEvent.PulseLength1ThirdFit[j] = VarPeakWidthGausFit(new_window_size, UpTrace[j], 0.33, &RecEvent.PulsePos1ThirdFit[j], &ExpectedPosition[j], &tmpf, &tmpf, 0 , false);
	 //conversion to seconds
	 RecEvent.PulsePos1ThirdFit[j]    *= (12.5 / (NoZeros +1)) * (1.0e-9);
	 RecEvent.PulseLength1ThirdFit[j] *= (12.5 / (NoZeros +1)) * (1.0e-9);
	 tmpPulseTime[tmpCount] = RecEvent.PulsePos1ThirdFit[j];
	 tmpCount++;
       }
       else{
         RecEvent.PulsePos1ThirdFit[j]    = 0;
	 RecEvent.PulseLength1ThirdFit[j] = 0;
       }
     }
     PlaneFit(tmpCount, tmpAntPos, RecEvent.daq_id, tmpPulseAmp, tmpPulseTime, Zenith, Azimuth);
     RecEvent.Zenith1ThirdFit      = Zenith[0];
     RecEvent.Zenith1ThirdFit_err  = Zenith[1];
     RecEvent.Azimuth1ThirdFit     = Azimuth[0];
     RecEvent.Azimuth1ThirdFit_err = Azimuth[1];
   }
   //FWHM
   if(1 && RecEvent.daq_id != 19){
     tmpCount = 0;
     for(int j=0; j<NoChannels; j++){
       if(ExpectedPosition[j]>0){
         strcpy(tmpAntPos[tmpCount], AntPos[j]);
	 tmpPulseAmp[tmpCount] = 1;
	 RecEvent.PulseLengthFWHM[j] = VarPeakWidth(new_window_size, UpTrace[j], 0.5, &RecEvent.PulsePosFWHM[j], &ExpectedPosition[j], 0, false);
	 //conversion to seconds
	 RecEvent.PulsePosFWHM[j]    *= (12.5 / (NoZeros +1)) * (1.0e-9);
	 RecEvent.PulseLengthFWHM[j] *= (12.5 / (NoZeros +1)) * (1.0e-9);
	 tmpPulseTime[tmpCount] = RecEvent.PulsePosFWHM[j];
	 tmpCount++;
       }
       else{
         RecEvent.PulsePosFWHM[j]    = 0;
	 RecEvent.PulseLengthFWHM[j] = 0;
       }
     }
     PlaneFit(tmpCount, tmpAntPos, RecEvent.daq_id, tmpPulseAmp, tmpPulseTime, Zenith, Azimuth);
     RecEvent.ZenithFWHM      = Zenith[0];
     RecEvent.ZenithFWHM_err  = Zenith[1];
     RecEvent.AzimuthFWHM     = Azimuth[0];
     RecEvent.AzimuthFWHM_err = Azimuth[1];
   }
   //FWHM with gaus fit
   if(1 && RecEvent.daq_id != 19){
     tmpCount = 0;
     for(int j=0; j<NoChannels; j++){
       if(ExpectedPosition[j]>0){
         strcpy(tmpAntPos[tmpCount], AntPos[j]);
	 tmpPulseAmp[tmpCount] = 1;
	 RecEvent.PulseLengthFWHMFit[j] = VarPeakWidthGausFit(new_window_size, UpTrace[j], 0.50, &RecEvent.PulsePosFWHMFit[j], &ExpectedPosition[j], &tmpf, &tmpf, 0 , false);
	 //conversion to seconds
	 RecEvent.PulsePosFWHMFit[j]    *= (12.5 / (NoZeros +1)) * (1.0e-9);
	 RecEvent.PulseLengthFWHMFit[j] *= (12.5 / (NoZeros +1)) * (1.0e-9);
	 tmpPulseTime[tmpCount] = RecEvent.PulsePosFWHMFit[j];
	 tmpCount++;
       }
       else{
         RecEvent.PulsePosFWHMFit[j]    = 0;
	 RecEvent.PulseLengthFWHMFit[j] = 0;
       }
     }
     PlaneFit(tmpCount, tmpAntPos, RecEvent.daq_id, tmpPulseAmp, tmpPulseTime, Zenith, Azimuth);
     RecEvent.ZenithFWHMFit      = Zenith[0];
     RecEvent.ZenithFWHMFit_err  = Zenith[1];
     RecEvent.AzimuthFWHMFit     = Azimuth[0];
     RecEvent.AzimuthFWHMFit_err = Azimuth[1];
   }
   //Max
   if(1 && RecEvent.daq_id != 19){
     tmpCount = 0;
     for(int j=0; j<NoChannels; j++){
       if(ExpectedPosition[j]>0){
         strcpy(tmpAntPos[tmpCount], AntPos[j]);
	 tmpPulseAmp[tmpCount] = 1;
	 VarPeakWidth(new_window_size, UpTrace[j], 1., &RecEvent.PulsePosMax[j], &ExpectedPosition[j], 0, false);
	 //conversion to seconds
	 RecEvent.PulsePosMax[j] *= (12.5 / (NoZeros +1)) * (1.0e-9);
	 tmpPulseTime[tmpCount]   = RecEvent.PulsePosMax[j];
	 tmpCount++;
       }
       else{
         RecEvent.PulsePosMax[j] = 0;
       }
     }
     PlaneFit(tmpCount, tmpAntPos, RecEvent.daq_id, tmpPulseAmp, tmpPulseTime, Zenith, Azimuth);
     RecEvent.ZenithMax      = Zenith[0];
     RecEvent.ZenithMax_err  = Zenith[1];
     RecEvent.AzimuthMax     = Azimuth[0];
     RecEvent.AzimuthMax_err = Azimuth[1];
   }
   if(debug) cout << "done" << endl;
   
   //definiton of direction for further analysis
   Zenith[0]  = h3.Zeg; //fabs(RecEvent.Zenith1Third);
   Zenith[1]  = 2;	//RecEvent.Zenith1Third_err;
   Azimuth[0] = h3.Azg; //RecEvent.Azimuth1Third;
   Azimuth[1] = 2;	//RecEvent.Azimuth1Third_err;
     
   if(debug){ cout << "calibration and channel analysis ..." ; fflush (stdout);}
   //quantities with field strength and calibration
   for(int j=0; j<NoChannels; j++){
     if(debug){ cout << " " << j ; fflush(stdout);}
     // calibration and up-sampling
     ADC2FieldStrengthUpSampled(event.window_size, RawTrace[j], NoZeros, UpTrace[j], RecEvent.daq_id, Zenith[0], false);
     
     //Hilbert Rectifier
     RectifierHilbert(new_window_size, UpTrace[j]);
     
     if(ExpectedPosition[j]>0){
       //pulse postion and length => calibrated
       RecEvent.PulseLengthFWHM[j]    =        VarPeakWidth(new_window_size, UpTrace[j], 0.50, &RecEvent.PulsePosFWHM[j],    &ExpectedPosition[j], 0, false);
       RecEvent.PulseLengthFWHMFit[j] = VarPeakWidthGausFit(new_window_size, UpTrace[j], 0.50, &RecEvent.PulsePosFWHMFit[j], &ExpectedPosition[j], &RecEvent.FieldStrengthChannelFit[j], &RecEvent.ChannelErrFieldStrengthFit[j], 0 , false);

       RecEvent.PulseLength1Third[j]    =        VarPeakWidth(new_window_size, UpTrace[j], 0.33, &RecEvent.PulsePos1Third[j],    &ExpectedPosition[j], 0, false);
       RecEvent.PulseLength1ThirdFit[j] = VarPeakWidthGausFit(new_window_size, UpTrace[j], 0.33, &RecEvent.PulsePos1ThirdFit[j], &ExpectedPosition[j], &RecEvent.FieldStrengthChannelFit[j], &RecEvent.ChannelErrFieldStrengthFit[j], 0 , false);

       VarPeakWidth(new_window_size, UpTrace[j], 1.0, &RecEvent.PulsePosMax[j], &ExpectedPosition[j], 0, false);

       //conversion to mu seconds
       RecEvent.PulsePos1Third[j]       *= (12.5 / (NoZeros +1)) / 1000.;
       RecEvent.PulsePosFWHM[j]         *= (12.5 / (NoZeros +1)) / 1000.;
       RecEvent.PulsePos1ThirdFit[j]    *= (12.5 / (NoZeros +1)) / 1000.;
       RecEvent.PulsePosFWHMFit[j]      *= (12.5 / (NoZeros +1)) / 1000.;
       RecEvent.PulsePosMax[j]          *= (12.5 / (NoZeros +1)) / 1000.;
       RecEvent.PulseLength1Third[j]    *= (12.5 / (NoZeros +1)) / 1000.;
       RecEvent.PulseLengthFWHM[j]      *= (12.5 / (NoZeros +1)) / 1000.;
       RecEvent.PulseLength1ThirdFit[j] *= (12.5 / (NoZeros +1)) / 1000.;
       RecEvent.PulseLengthFWHMFit[j]   *= (12.5 / (NoZeros +1)) / 1000.;
       
       //calibrated pulse peak
       RecEvent.FieldStrengthChannel[j]    = GetChannelPeak(new_window_size, UpTrace[j], ExpectedPosition[j]);
       
       //errors
       RecEvent.ChannelErrFieldStrength[j] = RecEvent.FieldStrengthChannel[j] * 7.4 / 100.;
       RecEvent.ChannelErrFieldStrengthFit[j] = RecEvent.FieldStrengthChannelFit[j] * 7.4 / 100.;
       
//       cout << endl << RecEvent.channel_id[j] << " -- " << RecEvent.event_id << endl;
       RecEvent.ChannelErrBackground[j] = EstimateTraceError(new_window_size, UpTrace[j], ExpectedPosition[j], NoZeros);
//       cout << "Estimated trace error = " << RecEvent.ChannelErrBackground[j] << " -- calib error = " << RecEvent.FieldStrengthChannel[j] << " +/-" << RecEvent.ChannelErrFieldStrength[j] << endl;
       
     }
     else{
       RecEvent.PulsePos1ThirdFit[j]          = 0;
       RecEvent.PulsePosFWHMFit[j]            = 0;
       RecEvent.PulsePos1Third[j]             = 0;
       RecEvent.PulsePosFWHM[j]               = 0;
       RecEvent.PulsePosMax[j]                = 0;
       RecEvent.PulseLength1Third[j]          = 0;
       RecEvent.PulseLengthFWHM[j]            = 0;
       RecEvent.FieldStrengthChannel[j]       = 0;
       RecEvent.PulseLength1ThirdFit[j]       = 0;
       RecEvent.PulseLengthFWHMFit[j]         = 0;
       RecEvent.FieldStrengthChannelFit[j]    = 0;
       RecEvent.ChannelErrFieldStrengthFit[j] = 0;
       RecEvent.ChannelErrFieldStrength[j]    = 0;
       RecEvent.ChannelErrBackground[j]       = 0;
     }
     
     //shower core distance => with the benefit of Grande
     GetDistanceToShowerAxis(&h3, AntPos[j], RecEvent.daq_id, Zenith[0], Azimuth[0], &RecEvent.DistanceShowerAxis[j], &RecEvent.DistanceShowerAxisErr[j]);

   } // end for over all channels
   
   //absolute field strength of one antenna
   CalculateAntennaFieldStrength(NoChannels, RecEvent.FieldStrengthChannel, AntPos, RecEvent.FieldStrengthAntenna);
   
   //absolute field strength of one antenna
   CalculateAntennaFieldStrength(NoChannels, RecEvent.FieldStrengthChannelFit, AntPos, RecEvent.FieldStrengthAntennaFit);
   
   // slope of LDF distribution
   RecEvent.LDFSlope = GetLDFSlope(NoChannels, RecEvent.FieldStrengthAntenna, RecEvent.DistanceShowerAxis);

   //Ephi and Etheta transformation
   CalculateTransformedFieldStrength(NoChannels, RecEvent.FieldStrengthChannel, AntPos, AntPol, RecEvent.EPhi, RecEvent.ETheta, Azimuth[0]);

   //geomagnetic angle
   RecEvent.BAngle = GeoMagAngle(Azimuth[0], Zenith[0], RecEvent.daq_id);
   
   //Fill the TRecEventTree
   TRecEvent->Fill();
  }//end for over data
  if(debug) cout << " done" << endl;

  output->cd();
  TRecEvent->Write(TRecEvent->GetName(),TObject::kWriteDelete);
  output->Close();

  //do not forget to clean up
  delete output;
  
  for(int i=0; i<NoChannels; i++) { delete[] AntPos[i]; delete[] AntPol[i]; delete[] tmpAntPos[i];}
  delete[] AntPos; delete[] AntPol; delete[] tmpAntPos;

  delete Tevent;
  delete Theader;
  delete Tchannel_profile;
  
}

