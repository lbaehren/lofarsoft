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

/**********************************************************************************************/

void LPFilter(int NoZero, int order, int frequency, double *coeff){
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

/**********************************************************************************************/

void HPFilter(int NoZero, int order, int frequency, double *coeff){
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

   float sum=0;
   for(int i=0; i<order; i++) sum += coeff[i];

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
  float *CTrace = new float [window_size];
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
  

  //HPFilter
#warning HPFilter in Rectifier enabled
if(1){ 
  int order_hp_basic = 4;
  int order_hp = order_hp_basic * (NoZero+1) + 1;
  double *coeff_hp = new double [order_hp];
  
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
  //take filter order into account -> re-normalisation
  for(int i=0; i<window_size; i++) trace[i] /= order_hp_basic;
  
  delete[] coeff_hp;
}
  delete[] CTrace;
}

/**********************************************************************************************/

void RectifierFFT(int NoZeros, int window_size, float *trace, float HighFilterFreq, float LowFilterFreq){
  
  float *RawFFT = new float [window_size*2];
  float *Amp    = new float [window_size/2];
  float *Phase  = new float [window_size/2];

  //square the trace
  for(int i=0; i<window_size; i++){
    trace[i] = pow(trace[i],2);
  }
  
  //calculate frequency 
  float *Freq = new float [window_size/2];
  for(int i=0; i<window_size/2; i++) Freq[i] = float((float)i*40.0*(float)(NoZeros+1)/(float)(window_size/2)); 

  TraceFFT(window_size, trace, Amp, Phase, RawFFT, true, true);
  
  //filtering:
  int HP_start = 0;
  int LP_start = 0;
  for(int i=0; i<window_size/2; i++){
    if(Freq[i]>LowFilterFreq && LP_start==0) LP_start = i;
    if(Freq[i]<HighFilterFreq) HP_start = i;
  }
  
  char name[1024];
  //high pass filter
  sprintf(name,"TMath::Gaus(x,100,5.0)");
  TF1 g1 =  TF1 ("g1",name);
  if(HighFilterFreq!=0.0) for(int i=HP_start; i>=0; i--) Amp[i] *= g1.Eval(100+HP_start-i);
  
  //low pass filter
  sprintf(name,"TMath::Gaus(x,100,2.5)");
  TF1 g2 =  TF1 ("g2",name);
  if(LowFilterFreq!=0.0) for(int i=LP_start; i<window_size/2; i++) Amp[i] *= g1.Eval(100-LP_start+i);

  
  //re order amp and phase
  float *tmp_amp = new float [window_size/2];
  float *tmp_phase = new float [window_size/2];
  for(int j=0; j<window_size/2; j++){
    tmp_amp[j] = Amp[j];
    tmp_phase[j] = Phase[j];
  }
  for(int j=0; j<window_size/2; j++){
    Amp[j] = tmp_amp[window_size/2-1-j];
    Phase[j] = tmp_phase[window_size/2-1-j];
  }
  
  AmpPhase2Trace(window_size,  Amp, Phase, trace, RawFFT, NoZeros);
  
  delete[] RawFFT;
  delete[] Amp;
  delete[] Phase;
  delete[] Freq;
  delete[] tmp_amp;
  delete[] tmp_phase;
}

/**********************************************************************************************/

void VideoFilter(int NoZero, int window_size, float *trace){
  int order = 126*(NoZero+1)+1;
  double *coeff = new double [order];
  float *CTrace = new float [window_size];
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
  
  //clean up
  delete[] coeff;
  delete[] CTrace;
}

/**********************************************************************************************/

void PeakDetection(int NoChannels, float *threshold, int window_size, float **trace, int *position, int delta){
  bool debug = false;
  bool FoundPulse;
  int *NoPulses = new int [NoChannels];
  for(int i=0; i<NoChannels; i++) NoPulses[i]=0;
  
  int **Position = new int *[NoChannels];
  for(int i=0; i<NoChannels; i++) Position[i] = new int [100];
  for(int i=0; i<NoChannels; i++) for(int j=0; j<100; j++) Position[i][j]=0;
#warning detectable number of pulses limited to 100
  
  int StartPulse = 0;
  float *tmpTrace = new float [window_size];
  int tmpTraceCount = 0;
  float PulseMax=0;
  for(int j=0; j<NoChannels; j++){
   FoundPulse = false;
   for(int i=0; i<window_size; i++){
    if(trace[j][i] > threshold[j]){
       if(!FoundPulse){
         StartPulse = i;
	 FoundPulse=true;
         if(NoPulses[j]==100) break;
       }
       tmpTrace[tmpTraceCount] = trace[j][i];
       tmpTraceCount++;
     }
     if( trace[j][i] < threshold[j] ){
       if(FoundPulse){
         PulseMax=0;
         for(int k=0; k < tmpTraceCount; k++){
           if(PulseMax < tmpTrace[k]){
	     PulseMax = tmpTrace[k];
  	     Position[j][NoPulses[j]] = k+StartPulse;
	   }
         }
         NoPulses[j]++;
         FoundPulse = false;
         tmpTraceCount = 0;
      }
     }
    }
   }

   //find channels with only one pulse
   int *good_ch = new int [NoChannels];
   int count_ch=0;
   for(int i=0; i<NoChannels; i++){
     if(NoPulses[i]==1){
       position[i]=Position[i][0];
       good_ch[count_ch] = i;
       count_ch++;
       if(debug) cout << "(1 pulse) channel " << i << " - " << position[i] << " -- threshold = " << threshold[i] << endl;       
     }
     else if(NoPulses[i]!=0 && debug) cout << "mulit pulse detected, no pulses = " << NoPulses[i] << " channel " << i << " -- threshold = " << threshold[i] << endl;
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
         if(debug) cout << "(multi pulse) channel " << i << " - " << position[i] << " -- threshold = " << threshold[i] << endl;       	 
	}
      }
      if(cnt_coinc_multipulse>1){
//	if(debug) cout << "pulse position not clear... set position to mean, channel "<< i << " -- threshold = " << threshold[i] << endl;
//        position[i] = (int) mean_pos;

	if(debug) cout << "pulse position not clear... set position to max peak, channel "<< i << " -- threshold = " << threshold[i] << endl;
	float tMax=0;
        for(int j=0; j<NoPulses[i]; j++){
          if(Position[i][j] < mean_pos+delta && Position[i][j]> mean_pos-delta){
	   if(tMax < trace[i][Position[i][j]]){
	     position[i] = Position[i][j];
	     tMax = trace[i][Position[i][j]];
	   }
  	 }
	}
        if(debug) cout << "(multi pulse) channel " << i << " - " << position[i] << " -- threshold = " << threshold[i] << endl;       	 
      }
     }
   }
   
  if(debug) cout << endl;
  for(int i=0; i<NoChannels; i++) delete[] Position[i];
  delete[] Position;
  delete[] NoPulses;
  delete[] good_ch;
  delete[] tmpTrace;
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

  float *tracetmp = new float [window_size];
  for(int i=0; i<window_size; i++){
    tracetmp[i] = 0.5*TriggerTrace[Ch1][i] + 0.5*TriggerTrace[Ch2][i];
  }

  for(int i=0; i<window_size; i++){
    TriggerTrace[Ch1][i] = tracetmp[i];
    TriggerTrace[Ch2][i] = tracetmp[i];
  }
  
  delete[] tracetmp;
}

/**********************************************************************************************/

bool TriggerCoincidence(int NoCh, int *position, float *coincidence_time, int NoZero, char **ant_pos, int daq_id){
  
  bool coincidence = false;
  bool NoThres = false;
  bool bDaqId = false;
  int max_pos = 0, min_pos = 131072;
  int max_bins = 0;
  float CoincTime = 0;
  
  if(daq_id==30){
    bDaqId = true;
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
    bDaqId = true;
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
  
  if(daq_id==50){
    bDaqId = true;
    // 1 valid triangles
    //check first one: CTR - 030 - 090
    for(int i=0; i<NoCh; i++){
      if(!strcmp(ant_pos[i],"000") || !strcmp(ant_pos[i],"001") || !strcmp(ant_pos[i],"002")){
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
  
  if(!bDaqId) cerr << "daq id " << daq_id << " is not definded" << endl;

  *coincidence_time = CoincTime;
  return coincidence;
}

/**********************************************************************************************/

bool L1Decide(int NoCh, int window_size, float **trace, char **ant_pos, int daq_id, float *coincidence_time, bool DoRFISupp){
  bool trigger = false;
  int NoZeros = 7;
  int new_window_size = window_size*(NoZeros+1);
  int *Position = new int [NoCh];
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
  delete[] Position;
  return trigger;
}

/**********************************************************************************************/

void DynamicThreshold(int window_size, float *trace, float *threshold, int NoZeros, bool DoRectifier){
  bool debug = false;
  int new_window_size = window_size*(NoZeros+1);
  float *tmpTrace = new float [new_window_size];
  
  if(NoZeros>0) ZeroPaddingFFT(window_size, trace, NoZeros, tmpTrace);
  else for(int i=0; i<new_window_size; i++) tmpTrace[i] = trace[i];
  if(DoRectifier) RectifierHilbertSquare(new_window_size, tmpTrace);

if(1){
  *threshold = (Mean(new_window_size,tmpTrace) + 4.5*RMS(new_window_size,tmpTrace)); // before 5*RMS
}
else{
  double mean = Mean(new_window_size,tmpTrace);
  double rms = RMS(new_window_size,tmpTrace);
  double maximum = MaxElement(new_window_size,tmpTrace);
  *threshold=mean+2*rms + (maximum-(mean+2*rms))/2;
}  
  if(debug){
   cout << "Dynamic Threshold: mean = " << Mean(new_window_size,tmpTrace) << " -- rms = " << RMS(new_window_size,tmpTrace) ;
   cout << " -- max = " << MaxElement(new_window_size,tmpTrace) << endl;
  }
  
  delete[] tmpTrace;
}

/**********************************************************************************************/

bool TriggerCoincidenceHardware(int NoCh, int *position, float *coincidence_time, int NoZero, char **ant_pos, int daq_id){
  
  bool coincidence = false;
  bool bDaqId = false;
  int max_bins = (int)(*coincidence_time * 1e-9 * SAMPLE_RATE * (NoZero+1));
  int CoincPos[3];
  float CoincTime = 0;
  
  if(daq_id==17 || daq_id==0){
    bDaqId = true;
    // 2 valid triangles
    for(int i=0; i<NoCh; i++){
      if(!strcmp(ant_pos[i],"CTR") ){
        for(int j=0; j<NoCh; j++){
	 if(!strcmp(ant_pos[j],"060")){
	  for(int k=0; k<NoCh; k++){
	   if(!strcmp(ant_pos[k],"120")){
	     CoincPos[0] = position[i];
	     CoincPos[1] = position[j];
	     CoincPos[2] = position[k];
	     if(CoincPos[0]==0 || CoincPos[1]==0 || CoincPos[2]==0) continue;
	     if( (MaxElement(3,CoincPos)-MinElement(3,CoincPos))< max_bins){
	      coincidence = true;
	      CoincTime = (MaxElement(3,CoincPos)-MinElement(3,CoincPos))/(float)(NoZero+1)*12.5;
	     }
	   }
	   if(!strcmp(ant_pos[k],"180")){
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

  if(daq_id==30){
    bDaqId = true;
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
    bDaqId = true;
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

  if(daq_id==50){
    bDaqId = true;
    // 1 valid triangles
    for(int i=0; i<NoCh; i++){
      if(!strcmp(ant_pos[i],"000") ){
        for(int j=0; j<NoCh; j++){
	 if(!strcmp(ant_pos[j],"001")){
	  for(int k=0; k<NoCh; k++){
	   if(!strcmp(ant_pos[k],"002")){
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
  } // end if daq_id == 50
  
  if(!bDaqId) cerr << "daq id " << daq_id << " is not defined" << endl;
  
  *coincidence_time = CoincTime;
  return coincidence;
}

/**********************************************************************************************/

bool L0Decide(int NoCh, int window_size, float **trace, char **ant_pos, int daq_id, float *coincidence_time, int NoZeros){
  bool trigger = false;
  int new_window_size = window_size*(NoZeros+1);
  int *Position = new int [NoCh];
  float threshold[NoCh];
  int delta = (int) (*coincidence_time / (12.5) * (NoZeros+1) / 1);
  for(int i=0; i<NoCh; i++){
   Position[i]=0;
   threshold[i]=0;
  }
    
  float **TriggerTrace = new float *[NoCh];
  for(int i=0; i<NoCh; i++) TriggerTrace[i] = new float [window_size*(NoZeros+1)];
  for(int i=0; i<NoCh; i++) for(int j=0; j<window_size*(NoZeros+1); j++) TriggerTrace[i][j]=0;

  for(int i=0; i<NoCh; i++){
    RFISuppressionMedian(window_size, trace[i], trace[i]);   
    DynamicThreshold(window_size, trace[i], &threshold[i], NoZeros);
    ZeroPaddingFFT(window_size, trace[i], NoZeros, TriggerTrace[i]);
    RectifierHilbertSquare(new_window_size, TriggerTrace[i]);
  }

  PeakDetection(NoCh, threshold, new_window_size, TriggerTrace, Position, delta);
  
  trigger = TriggerCoincidenceHardware(NoCh, Position, coincidence_time, NoZeros, ant_pos, daq_id);
  
  //don't forget to clean up
  for(int i=0; i<NoCh; i++) delete[] TriggerTrace[i];
  delete[] TriggerTrace;
  delete[] Position;
  
  return trigger;
}

/**********************************************************************************************/

float IntegralEnvelope(int window_size, float *trace){
  float Int_Env=0;
  int NoZeros=7;
  int new_window_size = window_size*(1+NoZeros);
  float *TriggerTrace = new float [new_window_size];
  
  ZeroPaddingFFT(window_size, trace, NoZeros, TriggerTrace);
  RectifierHilbert(new_window_size, TriggerTrace);
  
  for(int i=0; i<new_window_size; i++) Int_Env += TriggerTrace[i];
  
  delete[] TriggerTrace;
  
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

float FWHMPeak(int window_size, float *trace, int *RaisingEdge, int NoZeros){
  float FWHM=0;
  int new_window_size = window_size*(1+NoZeros);
  float *TriggerTrace = new float [new_window_size];
  float threshold=0;
  
  DynamicThreshold(window_size, trace, &threshold, NoZeros);
  if(NoZeros>0) ZeroPaddingFFT(window_size, trace, NoZeros, TriggerTrace);
  else for(int i=0; i<window_size; i++) TriggerTrace[i] = trace[i];
  RectifierHilbert(new_window_size, TriggerTrace);

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
   if(TriggerTrace[max_position+i] <= max_value/2.0) {
     upper_limit = max_position+i; 
     break;
   }
  for(int i=0; i<max_position; i++)
   if(TriggerTrace[max_position-i] <= max_value/2.0) {
     lower_limit = max_position-i;
     *RaisingEdge = lower_limit;
     break;
   }
   
  delete[] TriggerTrace;
  
  return FWHM = (upper_limit - lower_limit)/(float)(NoZeros+1);
}

/**********************************************************************************************/

float VarPeakWidth(int window_size, float *trace, float fraction, float *RaisingEdge, int *ExpectedPosition, int NoZeros, bool DoRectifier){
  float width=0;
  int new_window_size = window_size*(1+NoZeros);
  float *TriggerTrace = new float [new_window_size];
  
  if(NoZeros>0) ZeroPaddingFFT(window_size, trace, NoZeros, TriggerTrace);
  else for(int i=0; i<window_size; i++) TriggerTrace[i] = trace[i];
  if(DoRectifier) RectifierHilbert(new_window_size, TriggerTrace);
  
  //define local maximum
  float max_value = TriggerTrace[*ExpectedPosition];

  //find upper and lower limit of the peak
  int upper_limit = 0;
  int lower_limit = 0;
  for(int i=0; i<new_window_size-*ExpectedPosition; i++)
   if(TriggerTrace[*ExpectedPosition+i] <= max_value*fraction) {
     upper_limit = *ExpectedPosition+i; 
     break;
   }
  for(int i=0; i<*ExpectedPosition; i++)
   if(TriggerTrace[*ExpectedPosition-i] <= max_value*fraction) {
     lower_limit = *ExpectedPosition-i;
     *RaisingEdge = lower_limit;
     break;
   }
  
  delete[] TriggerTrace;
  
  return width = (upper_limit - lower_limit)/(float)(NoZeros+1);
}

/**********************************************************************************************/

void SNR(int window_size, const float *trace, float *snr, float *mean, float *rms, bool debug){
  float max=0;
  float *tmpTrace = new float [window_size];
  
  *snr = 0;
  *mean = 0;
  *rms = 0;

  for(int i=0; i<window_size; i++){
    tmpTrace[i] = Power(trace[i],2);
    if(tmpTrace[i]>max) max = tmpTrace[i];
  }
  
  *mean = TMath::Mean(window_size, tmpTrace);
  *rms = RMS(window_size, tmpTrace);
  *snr = max / *mean;
  
  if(debug){
    cout << "SNR:" << endl;
    cout << "signal to noise = " << *snr << " - Mean = " << *mean << " - RMS = " << *rms;
    cout << " - Max = " << max << endl;
    cout << "quality estimate = " << *snr * *rms / *mean << endl;
  }
  
  delete[] tmpTrace;
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
   float *TraceDummy = new float [window_size];
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
  
   if(debug)  cout << "Mean = " << *Mean << " - Sigma = " << *Sigma << " - Max = " << *Max << endl;
   
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
   
  delete[] TraceDummy;
  
  return no_peaks;
}

/**********************************************************************************************/

void SNRandNoPulses(int window_size, const float *trace, float *SNR, int *NoPulses, bool debug){
  float *tmpTrace = new float [window_size];
  float max = 0, mean = 0, rms = 0, threshold = 0;
  bool peak_detected = false;
  int no_peaks = 0;

  for(int i=0; i<window_size; i++) tmpTrace[i] = trace[i];
  SubtractPedestal(window_size, tmpTrace);

  // signal to noise ratio
  for(int i=0; i<window_size; i++){
    tmpTrace[i] = Power(tmpTrace[i],2);
  }
  max = MaxElement(window_size, tmpTrace);
  mean = Mean(window_size, tmpTrace);
  *SNR = max / mean;

  if(debug) cout << "snr = " << *SNR << " -- mean = " << mean << " -- max = " << max << endl; 
  
  // number of pulses
  for(int j=3; j<window_size-3; j++) 
    tmpTrace[j] = (tmpTrace[j-3] + tmpTrace[j-2] + tmpTrace[j-1] + tmpTrace[j] + tmpTrace[j+1] + tmpTrace[j+2] + tmpTrace[j+3]) / 7.0;
  
  for(int j=0; j<3; j++){
   tmpTrace[j] = 0;
   tmpTrace[window_size-3+j] = 0;
  }

  mean = Mean(window_size, tmpTrace); 
  rms = RMS(window_size, tmpTrace);
  max = MaxElement(window_size, tmpTrace);
  threshold = mean + 2*rms + (max - ( mean + 2*rms)) / 4.0;

  if(debug) cout << "Mean = " << mean << " - Sigma = " << rms << " - Max = " << max << endl;
  if(debug) cout << "Threshold = " << threshold << endl;

  if(no_peaks==0){
     for(int j=3; j<window_size-3; j++){
       if(tmpTrace[j] > threshold && !peak_detected){
         no_peaks++;
	 peak_detected = true;
       }
       if(peak_detected && tmpTrace[j] < threshold){
         peak_detected = false;
       }
     }
  }
  *NoPulses = no_peaks;
  
  delete[] tmpTrace;

}

/**********************************************************************************************/

void hilbert(int window_size, float *data, float *result, bool data_window){
	int i, l2;
	static int planlen = 0;
	static fftw_plan p1, p2;
	static fftw_complex *h, *H;
	int len = window_size;
	
       //Gaus and linear window
       
       if(1 && data_window){
         char name[1024], name2[1024];
         int WindowRange = (int)((double)len*0.01);
         //cout << WindowRange << endl;
         sprintf(name,"TMath::Gaus(x,%i,2.5)",WindowRange);//sigma old = 0.5
         TF1 g1 =  TF1 ("g1",name);
         //TF1 g2 =  TF1 ("g2","1+0*x");
         sprintf(name2,"TMath::Gaus(x,%i,2.5)",len-WindowRange);//sigma old = 0.5
         TF1 g3 =  TF1 ("g3",name2);
         for(int j=0; j<len; j++){
           if(j<WindowRange+1)					data[j] *= g1.Eval(j);
           //if(j>WindowRange && j<((int)len-WindowRange)+1)	data[j] *= g2.Eval(j);
           if(j>((int)len-WindowRange))				data[j] *= g3.Eval(j);
         }
       }


	/* Keep the arrays and plans around from last time, since this
	is a very common case. Reallocate them if they change. */

	if (len != planlen && planlen > 0) {
		fftw_destroy_plan(p1);
		fftw_destroy_plan(p2);
		fftw_free(h);
		fftw_free(H);
		planlen = 0;
	}

	if (planlen == 0) {
		planlen = len;
		h = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * len);
		H = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * len);


		/* Set up the fftw plans. */

		p1 = fftw_plan_dft_1d(len, h, H, FFTW_FORWARD, FFTW_MEASURE);
		p2 = fftw_plan_dft_1d(len, H, h, FFTW_BACKWARD, FFTW_MEASURE);

	}

	/* Convert the input to complex. */

	memset(h, 0, sizeof(fftw_complex) * len);
	for (i = 0; i < len; i++) {
		h[i][0] = data[i];
	}

	/* FFT. */

	fftw_execute(p1); /* h -> H */

	/* Hilbert transform. The upper half-circle gets multiplied by
	two, and the lower half-circle gets set to zero.  The real axis
	is left alone. */

	l2 = (len + 1) / 2;
	for (i = 1; i < l2; i++) {
		H[i][0] *= 2.;
		H[i][1] *= 2.;
	}
	l2 = len / 2 + 1;
	for (i = l2; i < len; i++) {
		H[i][0] = 0.;
		H[i][1] = 0.;
	}

	/* Inverse FFT. */

	fftw_execute(p2); /* H -> h */

	/* Fill in the rows of the result. */

	for (i = 0; i < len; i++) {
	 result[i] = h[i][1] / len;
	}
}

/**********************************************************************************************/

void RectifierHilbert(int NoZeros, int window_size, const float* sub_trace, float* up_trace){
   
   int new_window_size = window_size*(NoZeros+1);
   float *sub_trace_in_hilbert = new float [new_window_size];
   float *sub_trace_out_hilbert = new float [new_window_size];
   float *sub_trace_in_raw = new float [new_window_size];
   float *up_trace_hilbert = new float [new_window_size];
   float *up_trace_raw = new float [new_window_size];

   for(int i=0; i<window_size; i++){
     sub_trace_in_hilbert[i]     = sub_trace[i];
     sub_trace_in_raw[i]         = sub_trace[i];
   }
   
   hilbert(window_size, sub_trace_in_hilbert, sub_trace_out_hilbert);

   //Zeropadding
   if(NoZeros>0) ZeroPaddingFFT(window_size, sub_trace_out_hilbert, NoZeros, up_trace_hilbert);
   else for(int i=0; i<new_window_size; i++) up_trace_hilbert[i] = sub_trace_out_hilbert[i];
   if(NoZeros>0) ZeroPaddingFFT(window_size, sub_trace_in_raw, NoZeros, up_trace_raw);
   else for(int i=0; i<new_window_size; i++) up_trace_raw[i] = sub_trace_in_raw[i];
   
   for(int i=0; i<new_window_size; i++){
     up_trace[i] = sqrt( pow(up_trace_hilbert[i],2) + pow(up_trace_raw[i],2) );
   }
   
   //clean up
   delete[] sub_trace_in_hilbert;
   delete[] sub_trace_out_hilbert;
   delete[] sub_trace_in_raw;
   delete[] up_trace_hilbert;
   delete[] up_trace_raw;
}
  
/**********************************************************************************************/

void RectifierHilbertSquare(int window_size, float *trace){
   int new_window_size = window_size;
   
   float *trace_in_hilbert = new float [new_window_size];
   float *trace_out_hilbert = new float [new_window_size];
   float *trace_raw = new float [new_window_size];
   
   for(int i=0; i<window_size; i++){
     trace_in_hilbert[i]     = trace[i];
     trace_raw[i]         = trace[i];
   }
   
   hilbert(window_size, trace_in_hilbert, trace_out_hilbert);

   for(int i=0; i<new_window_size; i++){
     trace[i] = ( pow(trace_out_hilbert[i],2) + pow(trace_raw[i],2) );
   }
   
   //clean up
   delete[] trace_in_hilbert;
   delete[] trace_out_hilbert;
   delete[] trace_raw;
}

/**********************************************************************************************/

void RectifierHilbert(int window_size, float* trace){
  RectifierHilbertSquare(window_size,trace);
  for(int i=0; i<window_size; i++){
    trace[i] = sqrt( trace[i] );
  }
}

/**********************************************************************************************/

float GetCoincidenceTime(short int ID, float MaxZenith){
  float cTime = 0;
  float baseline = 0;
  if(ID == 17 || ID == 0) baseline = 70;
  if(ID == 19) { cerr << "GetCoincidenceTime: not usable" << endl; baseline = 0;}
  if(ID == 30) baseline = 65;
  if(ID == 42) baseline = 100;
  if(ID == 50) baseline = 100;
  
  if(baseline==0) cerr << "daq id " << ID << " is not definded" << endl;
  
  if(baseline != 0) cTime = sqrt( pow(baseline,2) - pow(baseline,2)/4.0)  / TMath::C() * sin( MaxZenith*TMath::Pi()/180.0 ) * (1.0e9);
  return cTime;
}

/**********************************************************************************************/

float MaxThresholdCrossingDelay(int window_size, float *Trace, int ChannelID, struct AnaFlag *AnaFlag, int NoZeros){
   bool debug = false;
   int NoCross = 0;
   float ThresCross[1000000];
   int new_window_size = window_size * (NoZeros +1);
   float *ZPTrace = new float [new_window_size];
   float threshold = 0;
   
   ZeroPaddingFFT(window_size, Trace, NoZeros, ZPTrace);
   DynamicThreshold(new_window_size, ZPTrace, &threshold, 0);
   threshold = sqrt(threshold);
   
   int tmp1 = 0;
   int tmp2 = 0;
   bool bFirst = false;
   bool bSecond = false;
   for(int i=1; i<new_window_size; i++){
     if( !bFirst && ZPTrace[i-1] < threshold && ZPTrace[i] >= threshold){
       tmp1 = i;
       bFirst = true;
       continue;
     }
     if( !bSecond && bFirst && ZPTrace[i-1] < threshold && ZPTrace[i] >= threshold){
       tmp2 = i;
       bSecond = true;
       continue;
     }
     if( bFirst && bSecond ){
       ThresCross[NoCross] = (float)(tmp2 - tmp1) / (NoZeros+1);
       NoCross++;
       bFirst = false;
       bSecond = false;
       if(debug) cout << "Crossing found, time difference = " << ThresCross[NoCross-1] << " ns" << endl;
       if(NoCross==1000000){
         cerr << "max number of threshold crossing detected ... continue" << endl;
	 break;  
       }
     }
   }
   
   //find maximum delay
   float Max = 0;
   for(int i=0; i<NoCross; i++){
     if(Max < ThresCross[i]) Max = ThresCross[i];
   }
   
   //fill correct output stuff
   if((unsigned int)NoCross < MaxNoCross){
   if(ChannelID==0){
     AnaFlag->NoCross00 = NoCross;
     for(int i=0; i<NoCross; i++) AnaFlag->ThresCross00[i] = ThresCross[i];
   }
   else{
   if(ChannelID==1){
     AnaFlag->NoCross01 = NoCross;
     for(int i=0; i<NoCross; i++) AnaFlag->ThresCross01[i] = ThresCross[i];
   }
   else{
   if(ChannelID==2){
     AnaFlag->NoCross02 = NoCross;
     for(int i=0; i<NoCross; i++) AnaFlag->ThresCross02[i] = ThresCross[i];
   }
   else{
   if(ChannelID==3){
     AnaFlag->NoCross03 = NoCross;
     for(int i=0; i<NoCross; i++) AnaFlag->ThresCross03[i] = ThresCross[i];
   }
   else{
   if(ChannelID==4){
     AnaFlag->NoCross04 = NoCross;
     for(int i=0; i<NoCross; i++) AnaFlag->ThresCross04[i] = ThresCross[i];
   }
   else{
   if(ChannelID==5){
     AnaFlag->NoCross05 = NoCross;
     for(int i=0; i<NoCross; i++) AnaFlag->ThresCross05[i] = ThresCross[i];
   }
   else{
   if(ChannelID==6){
     AnaFlag->NoCross06 = NoCross;
     for(int i=0; i<NoCross; i++) AnaFlag->ThresCross06[i] = ThresCross[i];
   }
   else{
   if(ChannelID==7){
     AnaFlag->NoCross07 = NoCross;
     for(int i=0; i<NoCross; i++) AnaFlag->ThresCross07[i] = ThresCross[i];
   }
   else{
   if(ChannelID==8){
     AnaFlag->NoCross08 = NoCross;
     for(int i=0; i<NoCross; i++) AnaFlag->ThresCross08[i] = ThresCross[i];
   }
   else{
   if(ChannelID==9){
     AnaFlag->NoCross09 = NoCross;
     for(int i=0; i<NoCross; i++) AnaFlag->ThresCross09[i] = ThresCross[i];
   }
   else cerr << "channel id for threshold crossing not definded" << endl;
   }}}}}}}}}
   }
   else{
    cerr << "max number of found crossing bigger than allocated memory " << endl;
   }
      
   delete ZPTrace;
   
   return Max;
}

/**********************************************************************************************/
