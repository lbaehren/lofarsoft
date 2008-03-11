#include <LopesStar/trigger.h>

//!std c++
#include <iostream>
#include <math.h>

//!postgresql
//#include <libpq-fe.h>

//!root 
#include <TROOT.h>
#include <TF1.h>
#include <TMath.h>

//!star tool headers
#include <LopesStar/util.h>
#include <LopesStar/calibration.h>
#include <LopesStar/reconstruction.h>

//!definition of hardware parameter
#define BANDWIDTH 40e6
#define SAMPLE_RATE 80e6
#define LOWER_BAND_LIMIT 40e6
#define UPPER_BAND_LIMIT 80e6

using namespace std;
using namespace TMath;

// --------------------------------------------------------------------- LPFilter

void LPFilter (int NoZero,
	       int order,
	       int frequency,
	       double *coeff)
{
   char win[2048];
   char name[1024];
   
   sprintf(win,"(2*TMath::Pi()*%i/(%i*%i))/TMath::Pi()"
   "*TMath::Sin(2*TMath::Pi()*%i/(%i*%i)*x)/(2*TMath::Pi()*%i/(%i*%i)*x)",
   frequency, (int)SAMPLE_RATE, NoZero+1, frequency, (int)SAMPLE_RATE, NoZero+1, frequency, (int)SAMPLE_RATE, NoZero+1);
  
   TF1 LowPassFilter = TF1 ("LowPassFilter",win);
   for(int i=0; i<(order-1)/2; i++) {
     coeff[i+(order-1)/2+1] = LowPassFilter.Eval(i+1);
     coeff[(order-1)/2-1-i] = coeff[i+(order-1)/2+1];
   }

   sprintf(name,"(2*TMath::Pi()*%i/(%i*%i))/TMath::Pi()*x", frequency, (int)SAMPLE_RATE, NoZero+1);
   TF1 ZeroElement = TF1("ZeroElement",name);

   coeff[(order-1)/2] = ZeroElement.Eval(1);
   
}

// --------------------------------------------------------------------- HPFilter

void HPFilter (int NoZero,
	       int order,
	       int frequency,
	       double *coeff)
{
   char win[2048];
   char name[1024];
   
   sprintf(win,"(1-2*TMath::Pi()*%i/(%i*%i))/TMath::Pi()"
   "*TMath::Sin(2*TMath::Pi()*%i/(%i*%i)*x)/(2*TMath::Pi()*%i/(%i*%i)*x)",
   frequency, (int)SAMPLE_RATE, NoZero+1, frequency, (int)SAMPLE_RATE, NoZero+1, frequency, (int)SAMPLE_RATE, NoZero+1);
  
   TF1 HighPassFilter = TF1 ("HighPassFilter",win);
   for(int i=0; i<(order-1)/2; i++) {
     coeff[i+(order-1)/2+1] = HighPassFilter.Eval(i+1);
     coeff[(order-1)/2-1-i] = coeff[i+(order-1)/2+1];
   }

   sprintf(name,"(1-2*TMath::Pi()*%i/(%i*%i))/TMath::Pi()*x", frequency, (int)SAMPLE_RATE, NoZero+1);
   TF1 ZeroElement = TF1("ZeroElement",name);

   coeff[(order-1)/2] = ZeroElement.Eval(1);

}

/**********************************************************************************************/

void BandFilterFunc(int NoZero, int order, int start_freq, int stop_freq, double *coeff){
  char cfunc[2048];
  char name[1024];
  
  sprintf(cfunc,"2*(%i-%i)/(%i*%i)*TMath::Sin(TMath::Pi()*(%i-%i)/(%i*%i)*x)/(TMath::Pi()*(%i-%i)/(%i*%i)*x)"
                "*TMath::Cos(2*TMath::Pi()*((%i+%i)/2.0)/(%i*%i)*x)",
  stop_freq, start_freq, (int)SAMPLE_RATE, NoZero+1, stop_freq, start_freq, (int)SAMPLE_RATE, NoZero+1,
  stop_freq, start_freq, (int)SAMPLE_RATE, NoZero+1, stop_freq, start_freq, (int)SAMPLE_RATE, NoZero+1);
  
  TF1 BandPassFilter = TF1 ("BandPassFilter",cfunc);
  for(int i=0; i<(order-1)/2; i++) {
     coeff[i+(order-1)/2+1] = BandPassFilter.Eval(i+1);
     coeff[(order-1)/2-1-i] = coeff[i+(order-1)/2+1];
   }
  sprintf(name,"2*(%i-%i)/(%i*%i)",stop_freq, start_freq, (int)SAMPLE_RATE, NoZero+1);
  TF1 ZeroElement = TF1("ZeroElement",name);

  coeff[(order-1)/2] = ZeroElement.Eval(1);
  
}

/**********************************************************************************************/

void Rectifier(int NoZero, int window_size, float *trace){
  //square the trace
  float CTrace[window_size];
  for(int i=0; i<window_size; i++){
    CTrace[i] = pow(trace[i],2);
    trace[i]  = 0;
  }
  
  //LPFilter
  int order = 126*(NoZero+1)+1;
  double coeff[order];

  LPFilter(NoZero, order, (int)BANDWIDTH, coeff);
  
  //Hann-Window
  char name[1024];
  sprintf(name,"0.5*(1+TMath::Cos(2*TMath::Pi()*x/(%i)))",order);
  TF1 HannWindow = TF1 ("HannWindow",name);
  for(int j=0; j<order; j++){
     coeff[j] *= HannWindow.Eval(j-((order-1)/2));
  }

  //convalution of the filter with the data
  for(int j=(order-1)/2+1; j<window_size-(order-1)/2-1; j++){
    for(int i=0; i<order;i++) trace[j] += coeff[i]*CTrace[-1*(order-1)/2+i+j];
  }
  
  //correct for negative values
  for(int i=0; i< window_size; i++) if(trace[i]<0) trace[i]=0;


}

/**********************************************************************************************/

void RectifierHardware(int NoZero, int window_size, float *trace){
  //square the trace
  float CTrace[window_size];
  for(int i=0; i<window_size; i++){
    CTrace[i] = 16.36*pow(trace[i],2);
    trace[i]  = 0;
  }
  
  //LPFilter
  int order = 126*(NoZero+1)+1;
  double coeff[order];

  LPFilter(NoZero, order, (int)BANDWIDTH, coeff);
  
  //Hann-Window
  char name[1024];
  sprintf(name,"0.5*(1+TMath::Cos(2*TMath::Pi()*x/(%i)))",order);
  TF1 HannWindow = TF1 ("HannWindow",name);
  for(int j=0; j<order; j++){
     coeff[j] *= HannWindow.Eval(j-((order-1)/2));
  }

  //convalution of the filter with the data
  for(int j=(order-1)/2+1; j<window_size-(order-1)/2-1; j++){
    for(int i=0; i<order;i++) trace[j] += coeff[i]*CTrace[-1*(order-1)/2+i+j];
  }
  

  //HPFilter
#warning HPFilter in RectifierHardware enabled
if(1){ 
  int order_hp = 4 * (NoZero+1) + 1;
  double coeff_hp[order_hp];
  
  HPFilter(NoZero, order_hp, 500000, coeff_hp);
  
  //Hann-Window
  sprintf(name,"0.5*(1+TMath::Cos(2*TMath::Pi()*x/(%i)))",order_hp);
  TF1 HannWindow_hp = TF1 ("HannWindow_hp",name);
  for(int j=0; j<order_hp; j++){
     coeff_hp[j] *= HannWindow_hp.Eval(j-((order_hp-1)/2));
  }
  
  for(int i=0; i<window_size; i++) CTrace[i] = trace[i];

  //convalution of the filter with the data
  for(int j=(order_hp-1)/2+1; j<window_size-(order_hp-1)/2-1; j++){
    for(int i=0; i<order_hp; i++) trace[j] += coeff_hp[i] * CTrace[-1*(order_hp-1)/2+i+j];
  }
}

}

/**********************************************************************************************/

void VideoFilter(int NoZero, int window_size, float *trace){
  int order = 126*(NoZero+1)+1;
  double coeff[order];
  float CTrace[window_size];
  for(int i=0; i<window_size; i++){
    CTrace[i] = trace[i];
    trace[i]  = 0;
  }

  BandFilterFunc(NoZero, order, (int)240e3, (int)40e6, coeff);
  
  //Hann-Window
  char name[1024];
  sprintf(name,"0.5*(1+TMath::Cos(2*TMath::Pi()*x/(%i)))",order);
  TF1 HannWindow = TF1 ("HannWindow",name);
  for(int j=0; j<order; j++){
     coeff[j] *= HannWindow.Eval(j-((order-1)/2));
  }
  
  //convalution of the band-pass filter with the data
  for(int j=(order-1)/2+1; j<window_size-(order-1)/2-1; j++){
    for(int i=0; i<order; i++) trace[j] += coeff[i]*CTrace[-1*(order-1)/2+i+j];
  }
}

/**********************************************************************************************/

void PeakDetection(int NoChannels, float *threshold, int window_size, float **trace, int *position, int delta){
  bool debug = false;
  bool FoundPulse;
  int NoPulses[NoChannels];
  for(int i=0; i<NoChannels; i++) NoPulses[i]=0;
  
  int **Position = new int *[NoChannels];
  for(int i=0; i<NoChannels; i++) Position[i] = new int [100];
  for(int i=0; i<NoChannels; i++) for(int j=0; j<100; j++) Position[i][j]=0;
#warning detecable number of pulses limited to 100

  for(int j=0; j<NoChannels; j++){
   FoundPulse = false;
   for(int i=0; i<window_size; i++){
    if(trace[j][i] > threshold[j]){
       if(!FoundPulse){
         Position[j][NoPulses[j]] = i;
         NoPulses[j]++;
	 FoundPulse=true;
         if(NoPulses[j]==100) break;
       }
     }
     if( trace[j][i] < threshold[j] ) FoundPulse = false;
    }
   }

   //find channels with only one pulse
   int good_ch[NoChannels];
   int count_ch=0;
   for(int i=0; i<NoChannels; i++){
     if(NoPulses[i]==1){
       position[i]=Position[i][0];
       good_ch[count_ch] = i;
       count_ch++;
       if(debug) cout << "(1 pulse) channel " << i << " - " << position[i] << endl;       
     }
     else if(NoPulses[i]!=0 && debug) cout << "mulit pulse detected, no pulses = " << NoPulses[i] << " channel " << i << endl;
   }
   
   //calculate mean position of the found channels
   float mean_pos=0;
   for(int i=0; i<count_ch; i++){
     mean_pos += position[good_ch[i]];
   }
   if(count_ch!=0) mean_pos /= count_ch;
   if(debug) cout << "mean position = " << mean_pos << endl;

   //search in multi pulses for simular pulses
   if(debug) cout << "delta = " << delta << endl; 
   int cnt_coinc_multipulse = 0;
   for(int i=0; i<NoChannels; i++){
     if(NoPulses[i] > 1){
      cnt_coinc_multipulse = 0;
      for(int j=0; j<NoPulses[i]; j++){
        if(Position[i][j] < mean_pos+delta && Position[i][j]> mean_pos-delta){
	 position[i] = Position[i][j];
	 cnt_coinc_multipulse++;
         if(debug) cout << "(multi pulse) channel " << i << " - " << position[i] << endl;       	 
	}
      }
      if(cnt_coinc_multipulse>1){
        position[i] = (int) mean_pos;
	if(debug) cout << "pulse position not clear... set position to mean, channel "<< i << endl;
      }
     }
   }
   
  if(debug) cout << endl;
  for(int i=0; i<NoChannels; i++) delete[] Position[i];
  delete[] Position;
}

/**********************************************************************************************/

void PeakDetection_Maximum(int window_size, float *trace, int *position){
  float max=0;
  for(int i=0; i<window_size; i++){
    if(trace[i]>max){
      *position = i;
      max = trace[i];
    }
  }
//  cout << "PeakDetection_Maximum " << *position << endl;
}

/**********************************************************************************************/

void ChannelAdd(int window_size, int Ch1, int Ch2, float **TriggerTrace){

  float tracetmp[window_size];
  for(int i=0; i<window_size; i++){
    tracetmp[i] = 0.5*TriggerTrace[Ch1][i] + 0.5*TriggerTrace[Ch2][i];
  }

  for(int i=0; i<window_size; i++){
    TriggerTrace[Ch1][i] = tracetmp[i];
    TriggerTrace[Ch2][i] = tracetmp[i];
  }
  
}

/**********************************************************************************************/

bool TriggerCoincidence(int NoCh, int *position, float *coincidence_time, int NoZero, char **ant_pos, int daq_id){
  
  bool coincidence = false;
  bool NoThres = false;
  int max_pos = 0, min_pos = 131072;
  int max_bins = 0;
  float CoincTime = 0;
  
  if(daq_id==30){
    // 2 valid triangles
    //check first one: CTR - 240 - 300
    for(int i=0; i<NoCh; i++){
      if(!strcmp(ant_pos[i],"CTR") || !strcmp(ant_pos[i],"240") || !strcmp(ant_pos[i],"300")){
        if(position[i]>max_pos) max_pos = position[i];
        if(position[i]<min_pos) min_pos = position[i];
        if(position[i]==0){
          NoThres = true;
          break;
        }
      }
    }
    if(!NoThres){
     //max valid bins
     max_bins = (int)(*coincidence_time * 1e-9 * SAMPLE_RATE * (NoZero+1));
     CoincTime = (max_pos-min_pos)*12.5;
  
     if((max_pos-min_pos)<=max_bins) coincidence = true;
   }

   if(!coincidence) {
    max_pos = 0; min_pos = 131072; max_bins = 0;
    //check second one: CTR - 300 - 360
    for(int i=0; i<NoCh; i++){
      if(!strcmp(ant_pos[i],"CTR") || !strcmp(ant_pos[i],"300") || !strcmp(ant_pos[i],"360")){
        if(position[i]>max_pos) max_pos = position[i];
        if(position[i]<min_pos) min_pos = position[i];
        if(position[i]==0){
          NoThres = true;
          break;
        }
      }
    }
    if(!NoThres){
      //max valid bins
      max_bins = (int)(*coincidence_time * 1e-9 * SAMPLE_RATE * (NoZero+1));
      CoincTime = (max_pos-min_pos)*12.5;
  
      if((max_pos-min_pos)<=max_bins) coincidence = true;
    }
   }//end if coincidence

  } // end if daq_id == 30
  
  if(daq_id==42){
    // 1 valid triangles
    //check first one: CTR - 030 - 090
    for(int i=0; i<NoCh; i++){
      if(!strcmp(ant_pos[i],"CTR") || !strcmp(ant_pos[i],"030") || !strcmp(ant_pos[i],"090")){
        if(position[i]>max_pos) max_pos = position[i];
        if(position[i]<min_pos) min_pos = position[i];
        if(position[i]==0){
          NoThres = true;
          break;
        }
      }
    }
    if(!NoThres){
     //max valid bins
     max_bins = (int)(*coincidence_time * 1e-9 * SAMPLE_RATE * (NoZero+1));
     CoincTime = (max_pos-min_pos)*12.5;
  
     if((max_pos-min_pos)<=max_bins) coincidence = true;
   }
  }

  *coincidence_time = CoincTime;
  return coincidence;
}

/**********************************************************************************************/

bool L1Decide(int NoCh, int window_size, float **trace, char **ant_pos, int daq_id, float *coincidence_time, bool DoRFISupp){
  bool trigger = false;
  int NoZeros = 7;
  int new_window_size = window_size*(NoZeros+1);
  int Position[NoCh];
  for(int i=0; i<NoCh; i++) Position[i]=0;
    
  float **TriggerTrace = new float *[NoCh];
  for(int i=0; i<NoCh; i++) TriggerTrace[i] = new float [window_size*(NoZeros+1)];
  for(int i=0; i<NoCh; i++) for(int j=0; j<window_size*(NoZeros+1); j++) TriggerTrace[i][j]=0;
  
  for(int i=0; i<NoCh; i++){
    if(DoRFISupp) RFISuppression(window_size, trace[i], trace[i]);
    ZeroPaddingFFT(window_size, trace[i], NoZeros, TriggerTrace[i]);
    Rectifier(NoZeros, new_window_size, TriggerTrace[i]);
  }
  
  for(int i=0; i<NoCh; i += 2){
    ChannelAdd(new_window_size, i, i+1, TriggerTrace);
    PeakDetection_Maximum(new_window_size, TriggerTrace[i], &Position[i]);
    Position[i+1] = Position[i];
  }

  trigger = TriggerCoincidence(NoCh, Position, coincidence_time, NoZeros, ant_pos, daq_id);
  
  //don't forget to clean up
  for(int i=0; i<NoCh; i++) delete[] TriggerTrace[i];
  delete[] TriggerTrace;
  
  return trigger;
}

/**********************************************************************************************/

void DynamicThreshold(int window_size, float *trace, float *threshold){
  bool debug = false;
  int NoZeros = 7;
  int new_window_size = window_size*(NoZeros+1);
  float tmpTrace[new_window_size];
  
  ZeroPaddingFFT(window_size, trace, NoZeros, tmpTrace);
  RectifierHardware(NoZeros, new_window_size, tmpTrace);

//  *threshold = (Mean(new_window_size,tmpTrace) + 6*RMS(new_window_size,tmpTrace));  //old
// #warning define threshold in dependence of the window size!  

  double mean = Mean(new_window_size,tmpTrace);
  double rms = RMS(new_window_size,tmpTrace);
  double maximum = MaxElement(new_window_size,tmpTrace);
  *threshold=mean+2*rms + (maximum-(mean+2*rms))/2;
  
  if(debug){
   cout << "Dynamic Threshold: mean = " << Mean(new_window_size,tmpTrace) << " -- rms = " << RMS(new_window_size,tmpTrace) ;
   cout << " -- max = " << MaxElement(new_window_size,tmpTrace) << endl;
  }
}

/**********************************************************************************************/

bool TriggerCoincidenceHardware(int NoCh, int *position, float *coincidence_time, int NoZero, char **ant_pos, int daq_id){
  
  bool coincidence = false;
  int max_bins = (int)(*coincidence_time * 1e-9 * SAMPLE_RATE * (NoZero+1));
  int CoincPos[3];
  float CoincTime = 0;
  
  if(daq_id==30){
    // 2 valid triangles
    //check first one: CTR - 240 - 300
    
    for(int i=0; i<NoCh; i++){
      if(!strcmp(ant_pos[i],"CTR") ){
        for(int j=0; j<NoCh; j++){
	 if(!strcmp(ant_pos[j],"300")){
	  for(int k=0; k<NoCh; k++){
	   if(!strcmp(ant_pos[k],"240")){
	     CoincPos[0] = position[i];
	     CoincPos[1] = position[j];
	     CoincPos[2] = position[k];
	     if(CoincPos[0]==0 || CoincPos[1]==0 || CoincPos[2]==0) continue;
	     if( (MaxElement(3,CoincPos)-MinElement(3,CoincPos))< max_bins){
	      coincidence = true;
	      CoincTime = (MaxElement(3,CoincPos)-MinElement(3,CoincPos))/(float)(NoZero+1)*12.5;
	     }
	   }
	   if(!strcmp(ant_pos[k],"360")){
	     CoincPos[0] = position[i];
	     CoincPos[1] = position[j];
	     CoincPos[2] = position[k];
	     if(CoincPos[0]==0 || CoincPos[1]==0 || CoincPos[2]==0) continue;
	     if( (MaxElement(3,CoincPos)-MinElement(3,CoincPos))< max_bins){
	      coincidence = true;
	      CoincTime = (MaxElement(3,CoincPos)-MinElement(3,CoincPos))/(float)(NoZero+1)*12.5;
	     }
	   }
	  }
	 }
	}
       }
      }
      
  } // end if daq_id == 30
  
  if(daq_id==42){
    // 1 valid triangles
    //check first one: CTR - 030 - 090
    for(int i=0; i<NoCh; i++){
      if(!strcmp(ant_pos[i],"CTR") ){
        for(int j=0; j<NoCh; j++){
	 if(!strcmp(ant_pos[j],"030")){
	  for(int k=0; k<NoCh; k++){
	   if(!strcmp(ant_pos[k],"090")){
	     CoincPos[0] = position[i];
	     CoincPos[1] = position[j];
	     CoincPos[2] = position[k];
	     if(CoincPos[0]==0 || CoincPos[1]==0 || CoincPos[2]==0) continue;
	     if( (MaxElement(3,CoincPos)-MinElement(3,CoincPos))< max_bins){
	      coincidence = true;
	      CoincTime = (MaxElement(3,CoincPos)-MinElement(3,CoincPos))/(float)(NoZero+1)*12.5;
	     }
	   }
	  }
	 }
	}
       }
      }
  } // end if daq_id == 42

  
  *coincidence_time = CoincTime;
  return coincidence;
}

/**********************************************************************************************/

bool L0Decide(int NoCh, int window_size, float **trace, char **ant_pos, int daq_id, float *coincidence_time){
  bool trigger = false;
  int NoZeros = 7;
  int new_window_size = window_size*(NoZeros+1);
  int Position[NoCh];
  float threshold[NoCh];
  int delta = (int) (*coincidence_time / (12.5) * 8 / 2);
  for(int i=0; i<NoCh; i++){
   Position[i]=0;
   threshold[i]=0;
  }
    
  float **TriggerTrace = new float *[NoCh];
  for(int i=0; i<NoCh; i++) TriggerTrace[i] = new float [window_size*(NoZeros+1)];
  for(int i=0; i<NoCh; i++) for(int j=0; j<window_size*(NoZeros+1); j++) TriggerTrace[i][j]=0;

  for(int i=0; i<NoCh; i++){
    RFISuppression(window_size, trace[i], trace[i]);   
    DynamicThreshold(window_size, trace[i], &threshold[i]);
    ZeroPaddingFFT(window_size, trace[i], NoZeros, TriggerTrace[i]);
    RectifierHardware(NoZeros, new_window_size, TriggerTrace[i]);
  }
  

  PeakDetection(NoCh, threshold, new_window_size, TriggerTrace, Position, delta);
  
  trigger = TriggerCoincidenceHardware(NoCh, Position, coincidence_time, NoZeros, ant_pos, daq_id);
  
  //don't forget to clean up
  for(int i=0; i<NoCh; i++) delete[] TriggerTrace[i];
  delete[] TriggerTrace;
  
  return trigger;
}

/**********************************************************************************************/

float IntegralEnvelope(int window_size, float *trace){
  float Int_Env=0;
  int NoZeros=7;
  int new_window_size = window_size*(1+NoZeros);
  float TriggerTrace[new_window_size];
  
  ZeroPaddingFFT(window_size, trace, NoZeros, TriggerTrace);
  RectifierHardware(NoZeros, new_window_size, TriggerTrace);
  
  for(int i=0; i<new_window_size; i++) Int_Env += TriggerTrace[i];
  
  return Int_Env/(float)((float)(NoZeros));
}

/**********************************************************************************************/

float IntegralRawData(int window_size, float *trace){
  float Int_Raw=0;
  
  for(int i=0; i<window_size; i++) {
   if(trace[i]>0) Int_Raw += trace[i];
   if(trace[i]<0) Int_Raw += (trace[i]*-1);
  }
  
  return Int_Raw/(float)(window_size);
}

/**********************************************************************************************/

float IntegralRawSquareData(int window_size, float *trace){
  float Int_RawSq=0;
  
  for(int i=0; i<window_size; i++) Int_RawSq += pow(trace[i],2);
  
  return Int_RawSq/(float)(window_size);
}

/**********************************************************************************************/

float FWHMPeak(int window_size, float *trace){
  float FWHM=0;
  int NoZeros=7;
  int new_window_size = window_size*(1+NoZeros);
  float TriggerTrace[new_window_size];
  float threshold=0;
  
  DynamicThreshold(window_size, trace, &threshold);
  ZeroPaddingFFT(window_size, trace, NoZeros, TriggerTrace);
  RectifierHardware(NoZeros, new_window_size, TriggerTrace);

  //find maximum by using the dynamic threshold
  float max_value=threshold;
  int max_position=0;
  bool found_peak=false;
  for(int i=0; i<new_window_size; i++){
   if(TriggerTrace[i] > max_value){
     max_value = TriggerTrace[i];
     max_position = i; 
     found_peak = true;
   }
   if(found_peak && TriggerTrace[i]<threshold) break;
  }
  
  //find upper and lower limit of the peak
  int upper_limit = 0;
  int lower_limit = 0;
  for(int i=0; i<new_window_size-max_position; i++)
   if(TriggerTrace[max_position+i] < max_value/2.0) {
     upper_limit = max_position+i; 
     break;
   }
  for(int i=0; i<max_position; i++)
   if(TriggerTrace[max_position-i] < max_value/2.0) {
     lower_limit = max_position-i;
     break;
   }
  
  return FWHM = (upper_limit - lower_limit)/(float)(NoZeros+1);
}

/**********************************************************************************************/

float VarPeakWidth(int window_size, float *trace, float fraction){
  float width=0;
  int NoZeros=7;
  int new_window_size = window_size*(1+NoZeros);
  float TriggerTrace[new_window_size];
  float threshold=0;
  
  DynamicThreshold(window_size, trace, &threshold);
  ZeroPaddingFFT(window_size, trace, NoZeros, TriggerTrace);
  RectifierHardware(NoZeros, new_window_size, TriggerTrace);

  //find maximum by using the dynamic threshold
  float max_value=threshold;
  int max_position=0;
  bool found_peak=false;
  for(int i=0; i<new_window_size; i++){
   if(TriggerTrace[i] > max_value){
     max_value = TriggerTrace[i];
     max_position = i; 
     found_peak = true;
   }
   if(found_peak && TriggerTrace[i]<threshold) break;
  }
  
  //find upper and lower limit of the peak
  int upper_limit = 0;
  int lower_limit = 0;
  for(int i=0; i<new_window_size-max_position; i++)
   if(TriggerTrace[max_position+i] < max_value/fraction) {
     upper_limit = max_position+i; 
     break;
   }
  for(int i=0; i<max_position; i++)
   if(TriggerTrace[max_position-i] < max_value/fraction) {
     lower_limit = max_position-i;
     break;
   }
  
  return width = (upper_limit - lower_limit)/(float)(NoZeros+1);
}

/**********************************************************************************************/
