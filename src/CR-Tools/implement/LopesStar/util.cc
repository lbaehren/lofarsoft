#include <LopesStar/util.h>


//!std c++
#include <iostream>
#include <time.h>
#include <math.h>
#include <libgen.h>

//!root 
#include <TROOT.h>
#include <TSystem.h>
#include <TGraph.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TMath.h>
#include <TRandom.h>
#include <TChain.h>

//!star tool headers
#include <LopesStar/trigger.h>
#include <LopesStar/reconstruction.h>
#include <LopesStar/calibration.h>

using namespace std;
using namespace TMath;

using TMath::Median;

/**********************************************************************************************/

void CreateTevent(TChain *Tevent, struct event *event){
  Tevent->SetBranchAddress("event_id",&event->event_id);  
  Tevent->SetBranchAddress("timestamp",&event->timestamp);
  Tevent->SetBranchAddress("timestamp_db",&event->timestamp_db);
  Tevent->SetBranchAddress("sample_id",&event->sample_id);
  Tevent->SetBranchAddress("leap_second",&event->leap_second);
  Tevent->SetBranchAddress("window_size",&event->window_size);
  Tevent->SetBranchAddress("channel_profile_id",&event->channel_profile_id);
  Tevent->SetBranchAddress("trace",&event->trace);
  Tevent->SetBranchAddress("delta_t",&event->delta_t);
}

/**********************************************************************************************/

void CreateTeventExtend(TChain *Tevent, struct header *header, struct channel_profiles *channel_profiles){
  Tevent->SetBranchAddress("run_id",&header->run_id);
  Tevent->SetBranchAddress("start_time",&header->start_time);
  Tevent->SetBranchAddress("stop_time",&header->stop_time);
  Tevent->SetBranchAddress("profile_id",&header->profile_id);
  Tevent->SetBranchAddress("clock_frequency",&header->clock_frequency);
  Tevent->SetBranchAddress("posttrigger",&header->posttrigger);
  Tevent->SetBranchAddress("DAQ_id",&header->DAQ_id);
  Tevent->SetBranchAddress("gps_position",&header->gps_position);
  Tevent->SetBranchAddress("profile_name",&header->profile_name);
  Tevent->SetBranchAddress("ch_id",&channel_profiles->ch_id);
  Tevent->SetBranchAddress("channel_no",&channel_profiles->channel_no);
  Tevent->SetBranchAddress("threshold",&channel_profiles->threshold);
  Tevent->SetBranchAddress("coincidence",&channel_profiles->coincidence);
  Tevent->SetBranchAddress("board_address",&channel_profiles->board_address);
  Tevent->SetBranchAddress("antenna_pos",&channel_profiles->antenna_pos);
  Tevent->SetBranchAddress("polarisation_id",&channel_profiles->polarisation_id);
  Tevent->SetBranchAddress("polarisation",&channel_profiles->polarisation);
}

/**********************************************************************************************/

void CreateTevent(TTree *Tevent, struct event *event){
  Tevent->SetBranchAddress("event_id",&event->event_id);  
  Tevent->SetBranchAddress("timestamp",&event->timestamp);
  Tevent->SetBranchAddress("timestamp_db",&event->timestamp_db);
  Tevent->SetBranchAddress("sample_id",&event->sample_id);
  Tevent->SetBranchAddress("leap_second",&event->leap_second);
  Tevent->SetBranchAddress("window_size",&event->window_size);
  Tevent->SetBranchAddress("channel_profile_id",&event->channel_profile_id);
  Tevent->SetBranchAddress("trace",&event->trace);
  Tevent->SetBranchAddress("delta_t",&event->delta_t);
}

/**********************************************************************************************/

void CreateTeventExtend(TTree *Tevent, struct header *header, struct channel_profiles *channel_profiles){
  Tevent->SetBranchAddress("run_id",&header->run_id);
  Tevent->SetBranchAddress("start_time",&header->start_time);
  Tevent->SetBranchAddress("stop_time",&header->stop_time);
  Tevent->SetBranchAddress("profile_id",&header->profile_id);
  Tevent->SetBranchAddress("clock_frequency",&header->clock_frequency);
  Tevent->SetBranchAddress("posttrigger",&header->posttrigger);
  Tevent->SetBranchAddress("DAQ_id",&header->DAQ_id);
  Tevent->SetBranchAddress("gps_position",&header->gps_position);
  Tevent->SetBranchAddress("profile_name",&header->profile_name);
  Tevent->SetBranchAddress("ch_id",&channel_profiles->ch_id);
  Tevent->SetBranchAddress("channel_no",&channel_profiles->channel_no);
  Tevent->SetBranchAddress("threshold",&channel_profiles->threshold);
  Tevent->SetBranchAddress("coincidence",&channel_profiles->coincidence);
  Tevent->SetBranchAddress("board_address",&channel_profiles->board_address);
  Tevent->SetBranchAddress("antenna_pos",&channel_profiles->antenna_pos);
  Tevent->SetBranchAddress("polarisation_id",&channel_profiles->polarisation_id);
  Tevent->SetBranchAddress("polarisation",&channel_profiles->polarisation);
}

/**********************************************************************************************/

void CreateTheader(TChain *Theader, struct header *header){
  Theader->SetBranchAddress("run_id",&header->run_id);
  Theader->SetBranchAddress("start_time",&header->start_time);
  Theader->SetBranchAddress("stop_time",&header->stop_time);
  Theader->SetBranchAddress("profile_id",&header->profile_id);
  Theader->SetBranchAddress("clock_frequency",&header->clock_frequency);
  Theader->SetBranchAddress("posttrigger",&header->posttrigger);
  Theader->SetBranchAddress("DAQ_id",&header->DAQ_id);
  Theader->SetBranchAddress("gps_position",&header->gps_position);
  Theader->SetBranchAddress("profile_name",&header->profile_name);
}

/**********************************************************************************************/
 
void CreateTchannel_profiles(TChain *Tchannel_profile, struct channel_profiles *channel_profiles){
  Tchannel_profile->SetBranchAddress("ch_id",&channel_profiles->ch_id);
  Tchannel_profile->SetBranchAddress("channel_no",&channel_profiles->channel_no);
  Tchannel_profile->SetBranchAddress("threshold",&channel_profiles->threshold);
  Tchannel_profile->SetBranchAddress("coincidence",&channel_profiles->coincidence);
  Tchannel_profile->SetBranchAddress("board_address",&channel_profiles->board_address);
  Tchannel_profile->SetBranchAddress("antenna_pos",&channel_profiles->antenna_pos);
  Tchannel_profile->SetBranchAddress("polarisation_id",&channel_profiles->polarisation_id);
  Tchannel_profile->SetBranchAddress("polarisation",&channel_profiles->polarisation);
}

/**********************************************************************************************/
 
void CreateTAnaFlag(TChain *TFlag, struct AnaFlag *AnaFlag){
  TFlag->SetBranchAddress("event_id",&AnaFlag->event_id);
  TFlag->SetBranchAddress("DetCh",&AnaFlag->DetCh);
  TFlag->SetBranchAddress("channel_id",&AnaFlag->channel_id);
  TFlag->SetBranchAddress("ana_index",&AnaFlag->ana_index);
  TFlag->SetBranchAddress("description",AnaFlag->description);
  TFlag->SetBranchAddress("run_id",&AnaFlag->run_id);
  TFlag->SetBranchAddress("daq_id",&AnaFlag->daq_id);
  TFlag->SetBranchAddress("tot_events", &AnaFlag->tot_events);
  TFlag->SetBranchAddress("PulseLength",&AnaFlag->PulseLength);
  TFlag->SetBranchAddress("PulsePosition",&AnaFlag->PulsePosition);
  TFlag->SetBranchAddress("PulseMax",&AnaFlag->PulseMax);
  TFlag->SetBranchAddress("NoPulses",&AnaFlag->NoPulses);
  TFlag->SetBranchAddress("snr",&AnaFlag->snr);
  TFlag->SetBranchAddress("snr_mean",&AnaFlag->snr_mean);
  TFlag->SetBranchAddress("snr_rms",&AnaFlag->snr_rms);
  TFlag->SetBranchAddress("L1trigger",&AnaFlag->L1Trigger);
  TFlag->SetBranchAddress("Int_Env",&AnaFlag->Int_Env);
  TFlag->SetBranchAddress("Int_Raw",&AnaFlag->Int_Raw);
  TFlag->SetBranchAddress("Int_RawSq",&AnaFlag->Int_RawSq);
  TFlag->SetBranchAddress("FWHM",&AnaFlag->FWHM);
  TFlag->SetBranchAddress("DynThreshold",&AnaFlag->DynThreshold);
  TFlag->SetBranchAddress("CoincTime",&AnaFlag->CoincTime);
  TFlag->SetBranchAddress("Azimuth",&AnaFlag->Azimuth);
  TFlag->SetBranchAddress("Azimuth_err",&AnaFlag->Azimuth_err);
  TFlag->SetBranchAddress("Zenith",&AnaFlag->Zenith);    
  TFlag->SetBranchAddress("Zenith_err",&AnaFlag->Zenith_err); 

}

/**********************************************************************************************/

void CreateTselftrigger(TChain *Tselftrigger, struct selftrigger *selftrigger){
  Tselftrigger->SetBranchAddress("thres", &selftrigger->thres);
  Tselftrigger->SetBranchAddress("notrigger", &selftrigger->notrigger);
  Tselftrigger->SetBranchAddress("trigger", &selftrigger->trigger);  
  Tselftrigger->SetBranchAddress("event_id", &selftrigger->event_id);  
  Tselftrigger->SetBranchAddress("channel_profile_id", &selftrigger->channel_profile_id);
}

/**********************************************************************************************/

void CreateTChain(TChain *Tevent, char *FileName){
  
  char Name[strlen(FileName)];
  strcpy(Name,FileName);

  if(gSystem->Which(Name,Name)!=0){
    Tevent->Add(Name);
    cout << "Add " << basename(Name) << " to TChain." << endl;
  }
  else{
    cerr << "error - use absolute path - name OR filename not found ... exit" << endl;
    exit(1);
  }

  char * pch;
  int fileNo=0;
  char name[1024];
  
  //determined to only 100 files per Chain.. problem with pch .. change?  
  while(fileNo<99){
    sprintf(name,"_%02i.root",fileNo);
    pch = strstr (Name,name);
    fileNo++;
    sprintf(name,"_%02i.root",fileNo);
    if(pch!=NULL){
     strncpy (pch,name,8);
    
     if(gSystem->Which(Name,Name)!=0){
        Tevent->Add(Name);
        cout << "Add " << basename(Name) << " to TChain." << endl;
      }
      else{
        break;
      }
    }
    else break;
  }

}

/**********************************************************************************************/

int ConvertTimeStamp(char *Timestamp, int  *Sec, int *Nanosec, double Timeshift){
  int year=0, month=0, day=0, hour=0, min=0;
  int sec=0; 
  struct tm starTimeTM;
//  char name[256];
  int ShiftSec=0, ShiftSubSec=0;
  bool TimeLimit=false;
  char SUBSEC[1024];
  
  ShiftSec = (int) Timeshift;
  

  
  //database time string
  if(sscanf (Timestamp,"%d-%d-%d %d:%d:%d.%d",&year, &month, &day, &hour, &min, &sec, Nanosec)!=0){

     ShiftSubSec = (int)((Timeshift-(int)Timeshift)*1000000.0);

  //cout << "\nShift: Sec = " << ShiftSec << " - SubSec = " << ShiftSubSec << endl;
  //cout << Timestamp << endl;
    
    starTimeTM.tm_year = year-1900;
    starTimeTM.tm_mon = month-1;
    starTimeTM.tm_mday =  day;
    starTimeTM.tm_hour = hour;
    starTimeTM.tm_min = min;
    starTimeTM.tm_sec = sec + (int)Timeshift;
   
  //cout << "Sec " << sec << " - Shift " << (int)Timeshift << " " << Timestamp << endl;
      
    *Sec = (int) mktime(&starTimeTM);
     
    sscanf (Timestamp,"%*d-%*d-%*d %*d:%*d:%*d.%s", SUBSEC);
    if(strlen(SUBSEC)==5) *Nanosec *= 10;
    if(strlen(SUBSEC)==4) *Nanosec *= 100;
    if(strlen(SUBSEC)==3) *Nanosec *= 1000;  
    if(strlen(SUBSEC)==2) *Nanosec *= 10000;  
    if(strlen(SUBSEC)==1) *Nanosec *= 100000;  
    
    
    if( (Timeshift < 0) && ((*Nanosec + ShiftSubSec) < 0)  ) {
      *Sec -= 1;
      *Nanosec = (1000000 - (abs(ShiftSubSec)-*Nanosec));
      TimeLimit = true;
    }
    
    if( (Timeshift > 0) && ((*Nanosec + ShiftSubSec) > 1000000) ) {
      *Sec += 1;
      *Nanosec = ((*Nanosec + ShiftSubSec) - 1000000 );
      TimeLimit = true;
    }
    
    if(!TimeLimit) *Nanosec += ShiftSubSec;
    *Nanosec *= 1000; // nanosec scale
//sprintf(name,"%d - %u SubSec",*Sec,*Nanosec);
//cout << name << endl;
    return 1;

 }
 else {
 if(sscanf (Timestamp,"CH0 %d.%d.%d %d:%d:%d.%d",&day, &month, &year, &hour, &min, &sec, Nanosec)!=0){ //gps time string

    ShiftSubSec = (int)((Timeshift-(int)Timeshift)*10000000.0);
    
    starTimeTM.tm_year = year+100;
    starTimeTM.tm_mon = month-1;
    starTimeTM.tm_mday =  day;
    starTimeTM.tm_hour = hour;
    starTimeTM.tm_min = min;
    starTimeTM.tm_sec = sec + (int)Timeshift;
    
    *Sec = (int) mktime(&starTimeTM);

    if( (Timeshift < 0) && ((*Nanosec + ShiftSubSec) < 0)  ) {
      *Sec -= 1;
      *Nanosec = (10000000 - (abs(ShiftSubSec)-*Nanosec));
      TimeLimit = true;
    }
    
    if( (Timeshift > 0) && ((*Nanosec + ShiftSubSec) > 10000000) ) {
      *Sec += 1;
      *Nanosec = ((*Nanosec + ShiftSubSec) - 10000000 );
      TimeLimit = true;
    }
    
    if(!TimeLimit) *Nanosec += ShiftSubSec;
    *Nanosec *= 100; // nanosec scale
    return 1;
  }//end if  
  else{ return 0;} 
 }//end else 



}

/*******************************************************************************/

#ifdef HAVE_POSTGRESQL

void ResCheck( PGresult *res){
  if(PQresultStatus(res)>5){
    cerr << PQresStatus(PQresultStatus(res)) << endl;
    cerr << "Program terminated ... "<< endl;
    exit(1);
  }
  
}

#endif

/*******************************************************************************/

void SubtractPedestal (int window_size,
		       short int *trace,
		       int start, int stop) {

   int TraceMean=0;
   if(stop==-1) stop = window_size/2-100;
   
   if(stop>window_size){
     cerr << "SubstractPedestal: stop > window_size" << endl;
     exit(1);
   }
   if(start<0){
     cerr << "SubstractPedestal: start < 0" << endl;
     exit(1);
   }
   
   for(int j=start; j<stop; j++) TraceMean += trace[j];
   TraceMean /= (stop-start);
   for(int j=0; j<window_size; j++) trace[j] -= TraceMean;
   

}

/**********************************************************************************************/

void SubtractPedestal(int window_size, float *trace, int start, int stop){

   float TraceMean=0;
   if(stop==-1) stop = window_size/2-100;
   
   if(stop>window_size){
     cerr << "SubstractPedestal: stop > window_size" << endl;
     exit(1);
   }
   if(start<0){
     cerr << "SubstractPedestal: start < 0" << endl;
     exit(1);
   }

   for(int j=start; j<stop; j++) TraceMean += trace[j];
   TraceMean /= (stop-start);
   for(int j=0; j<window_size; j++) trace[j] -= TraceMean;
   

}

/**********************************************************************************************/

void TraceFFT(int window_size, short int *trace, float *Amp, float *Phase, float *RawFFT, bool data_window){
   
   TraceFFTCore(window_size, trace, true , Amp, Phase, RawFFT, data_window);
};

/**********************************************************************************************/

void TraceFFT(int window_size, float *trace, float *Amp, float *Phase, float *RawFFT, bool data_window){

   TraceFFTCore(window_size, trace, false , Amp, Phase, RawFFT, data_window);
};

/**********************************************************************************************/

void TraceFFTCore(int window_size, void *trace, bool Flag, float *Amp, float *Phase, float *RawFFT, bool data_window){

   fftw_complex *in, *out;
   fftw_plan p;
   in =  (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * window_size);
   out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * window_size);  
   
   for(int j=0; j<window_size; j++){
     in[j][0]=0;
     in[j][1]=0;
     out[j][0]=0;
     out[j][1]=0;

   }   

   char name[1024];
   char name2[1024];   
   int WindowRange=0;
   
   //create fft plan
   p = fftw_plan_dft_1d(window_size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
   

   if(Flag == true)  SubtractPedestal(window_size, (short int*)trace);
   if(Flag == false) SubtractPedestal(window_size, (float*)trace);
   
   for(int j=0; j<window_size; j++) {
     if(Flag ==  true) in[j][0] = ((short int*)trace)[j];
     if(Flag == false) in[j][0] = ((float*)trace)[j];
     in[j][1] = 0;
   }


   
   //Hann-Window
   if(0 && data_window){
     sprintf(name,"0.5*(1+TMath::Cos(2*TMath::Pi()*x/(%i)))",window_size);
     TF1 HannWindow = TF1 ("HannWindow",name);
     for(int j=0; j<(int)window_size; j++){
       in[j][0] *= HannWindow.Eval(j-(window_size/2));
     }
   }   
   
   
   //Gaus and linear window
   if(1 && data_window){
     WindowRange = (int)((double)window_size*0.01);
     //cout << WindowRange << endl;
     sprintf(name,"TMath::Gaus(x,%i,2.5)",WindowRange);//sigma old = 0.5
     TF1 g1 =  TF1 ("g1",name);
     //TF1 g2 =  TF1 ("g2","1+0*x");
     sprintf(name2,"TMath::Gaus(x,%i,2.5)",window_size-WindowRange);//sigma old = 0.5
     TF1 g3 =  TF1 ("g3",name2);
     for(int j=0; j<(int)window_size; j++){
       if(j<WindowRange+1)						in[j][0] *= g1.Eval(j);
       //if(j>WindowRange && j<((int)window_size-WindowRange)+1)	in[j][0] *= g2.Eval(j);
       if(j>((int)window_size-WindowRange))				in[j][0] *= g3.Eval(j);
     }
   }
   
   //FFT
   fftw_execute(p);
   fftw_destroy_plan(p);
   
   
   //normalising
   for(int j=0; j<window_size; j++) {
     out[j][0] /= Sqrt(window_size/2);
     out[j][1] /= Sqrt(window_size/2);
   }

   //copy raw fft output
   for(int j=0; j<window_size; j++) {
     RawFFT[j] = out[j][0];
     RawFFT[j+window_size] = out[j][1];
   }
   
   //calculating Amp and Phase
   for(int j=1; j<((window_size/2)+1); j++){
     Amp[(window_size/2)-j] = sqrt( pow(out[j-1][0],2) + pow(out[j-1][1],2));
     
     Phase[window_size/2-j] = -10;
     if(out[j-1][0] > 1.0e-5 || out[j-1][0]<-1.0e-5) {
       Phase[window_size/2-j] = atan(Abs(out[j-1][1]) / Abs(out[j-1][0])); //rad
       
       //first quadrant -> do nothing
       
       //second quadrant
       if(out[j-1][0] < 0 && out[j-1][1] > 0 ) Phase[window_size/2-j] = TMath::Pi() - Phase[window_size/2-j];
       
       //third quadrant
       if(out[j-1][0] < 0 && out[j-1][1] < 0 ) Phase[window_size/2-j] = TMath::Pi() + Phase[window_size/2-j];

       //fourth quadrant
       if(out[j-1][0] > 0 && out[j-1][1] < 0 ) Phase[window_size/2-j] = 2*TMath::Pi() - Phase[window_size/2-j];

       //what to do if the complex part is zero
       if(out[j-1][1] < 1.0e-5 && out[j-1][1] > -1.0e-5 ){
         if(out[j-1][0] > 0 ) Phase[window_size/2-j] = 0;
         else Phase[window_size/2-j] = TMath::Pi();
       }
     }
     else{
       if(out[j-1][1] > 0 ) Phase[window_size/2-j] = TMath::Pi()/2;
        else Phase[window_size/2-j] = 3/2*TMath::Pi();
     }
     
   }
   
   fftw_free(in);
   fftw_free(out);

   

} //end TraceFFT


/**********************************************************************************************/

void TraceiFFT(int window_size,  float *RawFFT, float *trace, int NoZeros){
   fftw_complex *in, *out;
   fftw_plan p;
   in =  (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * window_size);
   out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * window_size);  
   
   for(int j=0; j<window_size; j++){
     in[j][0]=0;
     in[j][1]=0;
     out[j][0]=0;
     out[j][1]=0;
   }   

   //create fft plan
   p = fftw_plan_dft_1d(window_size, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
   
   //copy fft data
   for(int j=0; j<window_size; j++){
     in[j][0] = RawFFT[j];
     in[j][1] = RawFFT[j+window_size];
   }
  
   //iFFT
   fftw_execute(p);
   fftw_destroy_plan(p);
   
   //copy trace back and normalise
   for(int j=0; j<window_size; j++) {
     trace[j] = out[j][0]  / Sqrt((window_size*2)/(1+NoZeros));
   }
   
   fftw_free(in);
   fftw_free(out);

} //end iFFT

/**********************************************************************************************/

void AmpPhase2Trace(int window_size,  float *Amp, float *Phase, float *trace, float *fft_data, int NoZeros){
  float RawFFT[window_size*2];
  for(int i=0;  i<window_size*2; i++) RawFFT[i]=0;
  
  for(int i=1; i<((window_size/2)+1); i++){
    RawFFT[i+window_size/2*0-1] = Amp[window_size/2-i]*cos(Phase[window_size/2-i]);
    RawFFT[i+window_size/2*2-1] = Amp[window_size/2-i]*sin(Phase[window_size/2-i]);
  }
  
  for(int i=1; i<((window_size/2)+1); i++){
    RawFFT[i+window_size/2*1-1] =    RawFFT[window_size/2*1-i+1];
    RawFFT[i+window_size/2*3-1] = -1*RawFFT[window_size/2*3-i+1];
  }
  
  // difference found for position window_size/2 in raw data ??
  RawFFT[window_size/2] = fft_data[window_size/2];
  
  TraceiFFT(window_size, RawFFT, trace, NoZeros);	
  //for(int i=0; i<window_size*2; i++)trace[i] = RawFFT[i];
}

/**********************************************************************************************/

int PulseCount(unsigned int window_size, const float *Trace, double *Mean, double *Sigma, double *Max, bool debug){
   
   int no_peaks=0;
   float Sum=0;
   float SumSqr=0;
//   double Mean=0;
   float N=0;
//   double Sigma=0;
//   double Max=0;
   bool peak_detected = false;
   float TraceDummy[window_size];
   for(unsigned int i=0; i<window_size; i++) TraceDummy[i] = Trace[i];
   
   *Mean=0;
   *Sigma=0;
   *Max=0;
   
   SubtractPedestal(window_size, TraceDummy);
   
   //square the trace
   for(unsigned int j=0; j<window_size; j++) TraceDummy[j] = pow(TraceDummy[j],2);
   
   //make a smooth curve
   for(unsigned int j=3; j<window_size-3; j++) TraceDummy[j] = (TraceDummy[j-3] + TraceDummy[j-2] + TraceDummy[j-1] + TraceDummy[j] + TraceDummy[j+1] + TraceDummy[j+2] + TraceDummy[j+3]) / 7.0;

   //preparation for sigma calculation
   for(unsigned int j=3; j<(window_size-3); j++){
     SumSqr += pow((double)TraceDummy[j],2.0);
     Sum += TraceDummy[j];
     *Mean += TraceDummy[j];
     if(*Max < TraceDummy[j]) *Max = TraceDummy[j];
   }

   N = window_size-3-3;
   *Mean /= N;
   *Sigma = sqrt(  SumSqr/(N-1) - pow((double)Sum,2.0)/(N*(N-1)));
  
   if(debug) cout << "Mean = " << *Mean << " - Sigma = " << *Sigma << " - Max = " << *Max << endl;
   
   double threshold=*Mean+2**Sigma + (*Max-(*Mean+2**Sigma))/4;

   if(debug) cout << "Threshold = " << threshold << endl;
   
   if(no_peaks==0){
     for(unsigned int j=3; j<window_size-3; j++){
       if(TraceDummy[j] > threshold && !peak_detected){
         no_peaks++;
	 peak_detected = true;
       }
       if(peak_detected && TraceDummy[j] < threshold){
         peak_detected = false;
       }
     }
   }
   
   
  return no_peaks;
}

/**********************************************************************************************/

void BandFilterFFT(int window_size, float *trace, float start_freq, float stop_freq, float *trace_out){

  if(start_freq<40 || stop_freq>80){
    cerr << "BandFilterFFT: wrong frequency range" << endl;
    exit (1);
  }
  
  float Amp[window_size/2];
  float Phase[window_size/2];
  float RawFFT[window_size*2];
  
  TraceFFT(window_size, trace, Amp, Phase, RawFFT);
 
  //set range bevor start_freq zero
  for(int i=0; i<(start_freq-40)*(window_size*12.5e-3); i ++){
    RawFFT[window_size/2*1-1-i]=0;
    RawFFT[window_size/2*1  +i]=0;
    RawFFT[window_size/2*3-1-i]=0;
    RawFFT[window_size/2*3  +i]=0;
  }
  
  //set range after stop_freq zero
  for(int i=0; i<(80-stop_freq)*(window_size*12.5e-3); i ++){
    RawFFT[window_size/2*0  +i]=0;
    RawFFT[window_size/2*2-1-i]=0;
    RawFFT[window_size/2*2  +i]=0;
    RawFFT[window_size/2*4-1-i]=0;
  }
  
  
  //inverse FFT
  TraceiFFT(window_size, RawFFT, trace_out);	

}

/**********************************************************************************************/

void FrequencyFilter(int window_size, float *trace, float freq, float *trace_out){

  if(freq<40 || freq>80){
    cerr << "FrequencyFilter: wrong frequency" << endl;
    exit (1);
  }
  
  float Amp[window_size/2];
  float Phase[window_size/2];
  float RawFFT[window_size*2];
  int position_range = 1;
  
  
  TraceFFT(window_size, trace, Amp, Phase, RawFFT);
  
  //defines the position of freq in the spectrum
  int position = (int) ((freq-(40+2*40.0/(window_size/2))) * window_size/2 / 40.0);

  //check if the position is within the spectrum
  if(position > window_size/2-3-1) position_range = 0;
  if(position < 3) position_range = 0;
  
  //correct the frequency in the amp distribution
  for(int i=position-position_range; i<position+position_range+1; i++){
    Amp[i] = 0 ;
  }
  
  AmpPhase2Trace(window_size, Amp, Phase, trace_out, RawFFT, 0);
}

/**********************************************************************************************/

bool TraceCheck(struct event *event) {
 bool TraceStatus=true;
 
 for(unsigned int i=0; i<event->window_size; i++) 
   if( (event->trace[i] & 0x1000) == 0x1000){
     TraceStatus = false;
     break;
   }
 
 return TraceStatus;
}

/**********************************************************************************************/

void TraceFFTReal(int window_size, short int *trace, float *Amp, float *Phase, float *RawFFT, bool data_window){

  double in[window_size];
  fftw_complex *out;
  char name[1024];
  char name2[1024]; 
  int WindowRange=0;
  
  
  SubtractPedestal(window_size, trace);
  for(int i=0; i<window_size; i++) in[i] = trace[i];
  
  
  //Hann-Window
   if(0 && data_window){
     sprintf(name,"0.5*(1+TMath::Cos(2*TMath::Pi()*x/(%i)))",window_size);
     TF1 HannWindow = TF1 ("HannWindow",name);
     for(int j=0; j<(int)window_size; j++){
       in[j] *= HannWindow.Eval(j-(window_size/2));
     }
   }   
   
   
   //Gaus and linear window
   if(1 && data_window){
     WindowRange = (int)((double)window_size*0.01);
     //cout << WindowRange << endl;
     sprintf(name,"TMath::Gaus(x,%i,2.5)",WindowRange);//sigma old = 0.5
     TF1 g1 =  TF1 ("g1",name);
     //TF1 g2 =  TF1 ("g2","1+0*x");
     sprintf(name2,"TMath::Gaus(x,%i,2.5)",window_size-WindowRange);//sigma old = 0.5
     TF1 g3 =  TF1 ("g3",name2);
     for(int j=0; j<(int)window_size; j++){
       if(j<WindowRange+1)						in[j] *= g1.Eval(j);
       //if(j>WindowRange && j<((int)window_size-WindowRange)+1)	in[j] *= g2.Eval(j);
       if(j>((int)window_size-WindowRange))				in[j] *= g3.Eval(j);
     }
   }
   

  out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (window_size));
  for(int i=0; i<window_size; i++){
     out[i][0] = 0;
     out[i][1] = 0;
  }
  
  fftw_plan p;
  p =  fftw_plan_dft_r2c_1d(window_size, in, out, FFTW_ESTIMATE);
  
  fftw_execute(p);
  fftw_destroy_plan(p);
  
  //copy rawdata
  //array too small
  for(int i=0; i<window_size; i++){
    RawFFT[i]             = out[i][0];
    RawFFT[i+window_size] = out[i][1];
  }
  
   //calculating Amp and Phase
   for(int j=1; j<((window_size/2)+1); j++){
     Amp[(window_size/2)-j] = sqrt( pow(out[j-1][0],2) + pow(out[j-1][1],2));
     
     Phase[window_size/2-j] = -10;
     if(out[j-1][0] > 1.0e-5 || out[j-1][0]<-1.0e-5) {
       Phase[window_size/2-j] = atan(Abs(out[j-1][1]) / Abs(out[j-1][0])); //rad
       
       //first quadrant -> do nothing
       
       //second quadrant
       if(out[j-1][0] < 0 && out[j-1][1] > 0 ) Phase[window_size/2-j] = TMath::Pi() - Phase[window_size/2-j];
       
       //third quadrant
       if(out[j-1][0] < 0 && out[j-1][1] < 0 ) Phase[window_size/2-j] = TMath::Pi() + Phase[window_size/2-j];

       //fourth quadrant
       if(out[j-1][0] > 0 && out[j-1][1] < 0 ) Phase[window_size/2-j] = 2*TMath::Pi() - Phase[window_size/2-j];

       //what to do if the complex part is zero
       if(out[j-1][1] < 1.0e-5 && out[j-1][1] > -1.0e-5 ){
         if(out[j-1][0] > 0 ) Phase[window_size/2-j] = 0;
         else Phase[window_size/2-j] = TMath::Pi();
       }
     }
     else{
       if(out[j-1][1] > 0 ) Phase[window_size/2-j] = TMath::Pi()/2;
        else Phase[window_size/2-j] = 3/2*TMath::Pi();
     }
     
   }
  

  fftw_free(out);
  
}

/**********************************************************************************************/

void TraceiFFTReal(int window_size,  float *RawFFT, float *trace, int NoZeros){
   fftw_complex *in;
   double out[window_size];
   fftw_plan p;
   in =  (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * window_size);
   
   for(int j=0; j<window_size; j++){
     in[j][0]=0;
     in[j][1]=0;
   }   

   //create fft plan
   p = fftw_plan_dft_c2r_1d(window_size, in, out, FFTW_ESTIMATE);
   
   //copy fft data
   for(int j=0; j<window_size; j++){
     in[j][0] = RawFFT[j];
     in[j][1] = RawFFT[j+window_size];
   }
  
   //iFFT
   fftw_execute(p);
   fftw_destroy_plan(p);
   
   //copy trace back
   for(int j=0; j<window_size; j++) {
     trace[j] = out[j]/(window_size/(1+NoZeros));
   }
   
   fftw_free(in);

}

/**********************************************************************************************/

void ZeroPaddingFFTReal(int window_size, short int *trace, int NoZeros, float* ZPTrace){
  
  float *FFTdata = new float [window_size*2];
  float *Amp = new float [window_size/2];
  float *Phase = new float [window_size/2];
  
  TraceFFTReal(window_size, trace, Amp, Phase, FFTdata, false);
  
  float *ZPfft = new float [window_size*2*(1+NoZeros)];
  
  //fill with zeros
  for(int j=0; j<window_size*2*(1+NoZeros); j++) ZPfft[j]=0;
  
  
  //add zeros
   if(NoZeros>0)
    for(int j=1; j<window_size/2+1; j++){
       ZPfft[j-1+window_size/2]                 =    FFTdata[window_size/2-j];
       ZPfft[j-1+window_size/2*(3+2*(NoZeros))] = -1*FFTdata[window_size/2*3-j];
    }

   if(NoZeros==0)
    for(int j=0; j<window_size*2; j++){
      ZPfft[j]=FFTdata[j];
    }
    

  TraceiFFTReal(window_size*(NoZeros+1), ZPfft, ZPTrace, NoZeros);
  
  delete[] FFTdata;
  delete[] Amp;
  delete[] Phase;
  delete[] ZPfft;

}

/**********************************************************************************************/

void CorrectADCClipping(unsigned int window_size, short int *trace){
//remove 13th bit info

  for(unsigned int i=0; i<window_size; i++){
    trace[i] = (trace[i] & 0x0fff);
  }
}

/**********************************************************************************************/
