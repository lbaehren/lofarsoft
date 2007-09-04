#include <LopesStar/reconstruction.h>


//!std c++
#include <iostream>
#include <time.h>
#include <math.h>
#include <stdlib.h>

//!root 
#include <TMath.h>

//!star tool headers
#include <LopesStar/trigger.h>
#include <LopesStar/util.h>
#include <LopesStar/calibration.h>

using namespace std;
using namespace TMath;

/**********************************************************************************************/

void ZeroPaddingFFT(int window_size, short int *trace, int NoZeros, float* ZPTrace){
  ZeroPaddingFFTCore( window_size, trace, true, NoZeros, ZPTrace);
}

/**********************************************************************************************/

void ZeroPaddingFFT(int window_size, float *trace, int NoZeros, float* ZPTrace){
  ZeroPaddingFFTCore( window_size, trace, false, NoZeros, ZPTrace);
}

/**********************************************************************************************/

void ZeroPaddingFFTCore(int window_size, void *trace, bool Flag, int NoZeros, float* ZPTrace){
  
  float *FFTdata = new float [window_size*2];
  float *Amp = new float [window_size/2];
  float *Phase = new float [window_size/2];
  
  if( Flag == true  ) TraceFFT(window_size, (short int*)trace, Amp, Phase, FFTdata, true);
  if( Flag == false ) TraceFFT(window_size,     (float*)trace, Amp, Phase, FFTdata, true);
  
  float *ZPfft = new float [window_size*2*(1+NoZeros)];
  
  //fill with zeros
  for(int j=0; j<window_size*2*(1+NoZeros); j++) ZPfft[j]=0;
  
  //add zeros
  if(NoZeros>0)
   for(int j=0; j<window_size/2; j++){
      ZPfft[j+window_size/2]                         = FFTdata[j+window_size/2*1];
      ZPfft[j+window_size/2*(NoZeros*2)]             = FFTdata[j+window_size/2*0];
      ZPfft[j+window_size/2*(NoZeros*2+3)]           = FFTdata[j+window_size/2*3];
      ZPfft[j+window_size/2*(NoZeros*2+2+NoZeros*2)] = FFTdata[j+window_size/2*2];
   }
  if(NoZeros==0)
    for(int j=0; j<window_size*2; j++){
      ZPfft[j]=FFTdata[j];
    }
 
  TraceiFFT(window_size*(NoZeros+1), ZPfft, ZPTrace, NoZeros);
  
  delete[] FFTdata;
  delete[] Amp;
  delete[] Phase;
  delete[] ZPfft;

}

/**********************************************************************************************/

void RFISuppressionSimple(int window_size, short int* trace, float *TraceOut){
  RFISuppressionSimpleCore(window_size, trace, true, TraceOut);
}

/**********************************************************************************************/

void RFISuppressionSimple(int window_size, float *trace, float *TraceOut){
  RFISuppressionSimpleCore(window_size, trace, false, TraceOut);
}

/**********************************************************************************************/

void RFISuppressionSimpleCore(int window_size, void* trace, bool Flag, float *TraceOut){
 
   float *Tracefft = new float [window_size*2];
   float *Amp = new float [window_size/2];
   float *Phase = new float [window_size/2];
   float *Cut = new float[window_size/2];
   
   //fft without windowing the data-> just rectangle window
   if(Flag == true)  TraceFFT(window_size , (short int*)trace , Amp , Phase, Tracefft, false);
   if(Flag == false) TraceFFT(window_size ,     (float*)trace , Amp , Phase, Tracefft, false);

   float MaxCut=0;
   float MeanCut=0;
   float SumSqr=0;
   float Sum=0;
   float N=0;
   float sigma=0;
   
   //find maximum and prepare calculation of sigma
   for(int j=0; j<((window_size/2)); j++){
     Cut[j] = sqrt( pow(Tracefft[j],2) + pow(Tracefft[j+window_size],2));
     
     SumSqr += pow((double)Cut[j],2.0);
     Sum += Cut[j];
     
     if(Cut[j]>MaxCut) MaxCut=Cut[j];
     
     MeanCut += Cut[j];
   }

   
   N = window_size/2;
   MeanCut /= N;
   sigma = sqrt(  SumSqr/(N-1) - pow((double)Sum,2.0)/(N*(N-1)));
   
//  cout << "Max = " << MaxCut << " - Mean = " << MeanCut <<  " - sigma = " << sigma << endl;
   
   float factor = 0;
   double threshold=MeanCut+2*sigma + (MaxCut-(MeanCut+2*sigma))/4;
#warning threshold in  void RFISuppressionSimple() needs to be checked better

   //cut off the strong mono frequency components in the raw fft data
   for(int j=0; j<window_size/2; j++){
      if( Cut[j]  > threshold){
       if(Tracefft[j+window_size/2*0]>0) Tracefft[j+window_size/2*0] =  1/sqrt(2.0)*(factor);
         else				 Tracefft[j+window_size/2*0] = -1/sqrt(2.0)*(factor);

       if(Tracefft[j+window_size/2*1]>0) Tracefft[j+window_size/2*1] =  1/sqrt(2.0)*(factor);
         else				 Tracefft[j+window_size/2*1] = -1/sqrt(2.0)*(factor);

       if(Tracefft[j+window_size/2*2]>0) Tracefft[j+window_size/2*2] =  1/sqrt(2.0)*(factor);
         else				 Tracefft[j+window_size/2*2] = -1/sqrt(2.0)*(factor);

       if(Tracefft[j+window_size/2*3]>0) Tracefft[j+window_size/2*3] =  1/sqrt(2.0)*(factor);
         else				 Tracefft[j+window_size/2*3] = -1/sqrt(2.0)*(factor);
     }
   }


   //transform back
   TraceiFFT(window_size,  Tracefft, TraceOut);	
   
   //clean up
   delete[] Tracefft;
   delete[] Amp;
   delete[] Phase;
   delete[] Cut;
   
}

/**********************************************************************************************/

void RFISuppression(int window_size, float *trace, float *trace_suppressed, int NoZeros){

  float PHASE[window_size/2];
  float AMP[window_size/2];
  float AMP_orig[window_size/2];
  float AMP_normalised[window_size/2];
  for(int i=0; i<window_size/2; i++) AMP_normalised[i] = 0;
  float AMP_corrected[window_size/2];
  float AMP_suppressed[window_size/2];
  float FFTData[window_size*2];
  int EntriesInterval = 64*window_size/1024; //also possible 2, 4, 8, 16, 32, 64, 128 to get no remainder
  float mean_tmp, rms_tmp;
  float threshold;
  float med_tmp[window_size/2];
  float med_lower, med_upper;
  float AMP_tmp[window_size/2];
  int count_tmp;
  int iter_tmp;


  //get the FFT data
  TraceFFT(window_size, trace, AMP, PHASE, FFTData);

  //square the spectrum
  for(int a=0; a<window_size/2; a++){
    AMP[a] = pow((double)AMP[a],2.0);
    AMP_orig[a] = AMP[a];
  }
  
  //calculate median
  for(int a=EntriesInterval/2; a<(window_size/2-EntriesInterval/2); a++){
    for(int e=0; e<EntriesInterval; e++){
      med_tmp[e] = AMP_orig[a+e-EntriesInterval/2];
    }
    AMP[a] = TMath::Median(EntriesInterval, med_tmp);
  }

  //calculate median for upper range 
  for(int e=0; e<EntriesInterval/2; e++){
    med_tmp[e] = AMP_orig[window_size/2-1-e];
  }
  med_upper = TMath::Median(EntriesInterval/2, med_tmp);
  
  //calculate median for lower range
  for(int e=0; e<EntriesInterval/2; e++){
    med_tmp[e] = AMP_orig[e];
  }
  med_lower = TMath::Median(EntriesInterval/2, med_tmp);

  //set the upper and lower range with constant median value
  for(int a=0; a<EntriesInterval/2; a++){
    AMP[a] = med_lower;
    AMP[window_size/2-EntriesInterval/2+a] = med_upper;
  }

  //calculate the normalised spectrum
  for(int a=0; a<window_size/2; a++) {
    AMP_normalised[a] = AMP_orig[a]/AMP[a];
  }
  
  //find best threshold
  mean_tmp = Mean(window_size/2,AMP_normalised);
  rms_tmp = RMS(window_size/2,AMP_normalised);
  iter_tmp=0;

  while( (rms_tmp / mean_tmp) > 0.5 ){
    count_tmp = 0;
    threshold = mean_tmp + 4*rms_tmp;
    for(int a=0; a<window_size/2; a++){
      if(AMP_normalised[a] < threshold){
        AMP_tmp[count_tmp] = AMP_normalised[a];
	count_tmp++;
      }
    }
    mean_tmp = Mean(count_tmp, AMP_tmp);
    rms_tmp = RMS(count_tmp, AMP_tmp);
    iter_tmp++;
    if(iter_tmp == 25) break;
  }
  
  //fill the corrected AMP distribution
  for(int a=0; a<window_size/2; a++) {
    AMP_corrected[a] = 1;
    if(AMP_normalised[a] > (mean_tmp + 5*rms_tmp) )
      AMP_corrected[a] = 1/AMP_normalised[a];
  }

  //suppress the RFI in the original spectrum
  for(int a=0; a<window_size/2; a++){
    AMP_suppressed[a] = AMP_orig[a]*AMP_corrected[a];
    AMP_suppressed[a] = sqrt(AMP_suppressed[a]);
  }
  
  //recalculate the corrected time trace
  AmpPhase2Trace(window_size, AMP_suppressed , PHASE, trace, FFTData, NoZeros);

}

/**********************************************************************************************/

void PulseLengthEstimate(int window_size, const float *trace, float *PulseLength, float *PulsePosition, float *Integral, bool RFISupp, bool dynamic_thres){
  
  bool debug = false;
  
  float Max = 0;
  int PStart = 0;
  int PStop = 0;
  int NoZeros = 7;
  float TraceRMS = 0;
  float TraceMean = 0;
  float threshold = 0;
  int New_window_size = window_size*(NoZeros+1);
  float TraceDummy[New_window_size];
  float RawTrace[window_size];
  for(int i=0; i<window_size; i++){
    TraceDummy[i] = trace[i];
    RawTrace[i] = trace[i];
  }
  bool found_peak=false;
  
  *PulseLength = 0;
  *PulsePosition = 0;

  if(RFISupp) RFISuppression(window_size, TraceDummy, TraceDummy);
  ZeroPaddingFFT(window_size, TraceDummy, NoZeros,  TraceDummy);
   
  //make envelope
  RectifierHardware(NoZeros, New_window_size, TraceDummy);
   
  //find maximum
  if(dynamic_thres){
   for(int j=0; j<New_window_size; j++){
     if(TraceDummy[j] > Max){
       Max = TraceDummy[j];
       *PulsePosition = j;
     }
   }
   TraceRMS = RMS(window_size, TraceDummy);
   TraceMean = Mean(window_size, TraceDummy);
   threshold = (TraceMean-TraceRMS/5.0);
  }
  else{
  //find maximum by using the dynamic threshold
    DynamicThreshold(window_size, RawTrace, &threshold);
    Max=threshold;
    *PulsePosition=0;
    for(int i=0; i<New_window_size; i++){
    if(TraceDummy[i] > Max){
      Max = TraceDummy[i];
      *PulsePosition = i; 
      found_peak = true;
    }
     if(found_peak && TraceDummy[i]<threshold) break;
    }

  }

  PStart = (int)*PulsePosition;
  PStop = (int)*PulsePosition;

  while(TraceDummy[PStart] > Max/5.0) PStart--;
  while(TraceDummy[PStop]  > Max/5.0) PStop++;   

  *PulseLength = (float)((float)(PStop-PStart)/(float)(NoZeros+1));
  *PulsePosition = (float)(*PulsePosition/(float)(NoZeros+1));
  
  //calculate integral without found peak
  *Integral = 0;
  int Int_Start = PStart - (25 * NoZeros);
  if(Int_Start < 0 ) Int_Start = 0;
  int Int_Stop = PStop + (70 * NoZeros);
  if(Int_Stop > New_window_size) Int_Stop = New_window_size;
  
  for(int i=Int_Start; i<PStart; i++) *Integral += TraceDummy[i];
  for(int i=PStop; i<Int_Stop; i++) *Integral += TraceDummy[i];
  // norm by window size and no of zeros
  *Integral = *Integral / ((double) NoZeros) / ((double) window_size);

  if(debug) cout << "PulseLengthEstimate:" << endl;
  if(debug) cout << "Peak: Max = " << Max << " - Position = " << *PulsePosition << endl;
  if(debug) cout << "Integral = " << *Integral << endl;
  if(debug) cout << "estimate length of max pulse= " << *PulseLength << " - Start = "<< PStart/(NoZeros+1) << " - Stop = " << PStop/(NoZeros+1) << endl;

}

/**********************************************************************************************/

void EstimatePulseParameter(int window_size, const float *trace, float *PulseLength, float *PulsePosition, float *Maximum, float *Integral, float threshold){
  
  bool debug = false;
  
  int PStart = 0;
  int PStop = 0;
  int NoZeros = 7;
  float TraceRMS = 0;
  float TraceMean = 0;
  int New_window_size = window_size*(NoZeros+1);
  float TraceDummy[New_window_size];
  float RawTrace[window_size];
  for(int i=0; i<window_size; i++){
    TraceDummy[i] = trace[i];
    RawTrace[i] = trace[i];
  }
  bool found_peak=false;
  bool dynamic_thres=false;
  if(threshold==0) dynamic_thres = true;
  
  *PulseLength = 0;
  *PulsePosition = 0;

  ZeroPaddingFFT(window_size, TraceDummy, NoZeros,  TraceDummy);
   
  //make envelope
  RectifierHardware(NoZeros, New_window_size, TraceDummy);
   
  //find maximum
  if(dynamic_thres){
   for(int j=0; j<New_window_size; j++){
     if(TraceDummy[j] > *Maximum){
       *Maximum = TraceDummy[j];
       *PulsePosition = j;
     }
   }
   TraceRMS = RMS(window_size, TraceDummy);
   TraceMean = Mean(window_size, TraceDummy);
   threshold = (TraceMean-TraceRMS/5.0);
  }
  else{
  //find maximum by using the dynamic threshold
    *Maximum=threshold;
    *PulsePosition=0;
    for(int i=0; i<New_window_size; i++){
    if(TraceDummy[i] > *Maximum){
      *Maximum = TraceDummy[i];
      *PulsePosition = i; 
      found_peak = true;
    }
     if(found_peak && TraceDummy[i]<threshold) break;
    }

  }

  PStart = (int)*PulsePosition;
  PStop = (int)*PulsePosition;

  while(TraceDummy[PStart] > *Maximum/3.0) PStart--;
  while(TraceDummy[PStop]  > *Maximum/3.0) PStop++;   

  *PulseLength = (float)((float)(PStop-PStart)/(float)(NoZeros+1));
  *PulsePosition = (float)(*PulsePosition/(float)(NoZeros+1));
  
  //calculate integral without found peak
  *Integral = 0;
  int Int_Start = PStart - (20 * (NoZeros+1));
  if(Int_Start < 0 ) Int_Start = 0;
  int Int_Stop = PStop + (60 * (NoZeros+1));
  if(Int_Stop > New_window_size) Int_Stop = New_window_size;
  
  for(int i=Int_Start; i<PStart; i++) *Integral += TraceDummy[i];
  for(int i=PStop; i<Int_Stop; i++) *Integral += TraceDummy[i];
  // norm by window size and no of zeros
  *Integral = *Integral / ((double) NoZeros) / ((double) window_size);

  if(debug) cout << "PulseLengthEstimate:" << endl;
  if(debug) cout << "Peak: Max = " << *Maximum << " - Position = " << *PulsePosition << endl;
  if(debug) cout << "Integral = " << *Integral << endl;
  if(debug) cout << "estimate length of max pulse= " << *PulseLength << " - Start = "<< PStart/(NoZeros+1) << " - Stop = " << PStop/(NoZeros+1) << endl;

}

/**********************************************************************************************/

void SNR(int window_size, const float *trace, float *snr, float *mean, float *rms, bool debug){
  float max=0;
  float tmpTrace[window_size];
  
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
  
}

/**********************************************************************************************/

void GetAntPos(const char *AntPos, const int daq_id, float *AntCoordinate){
// AntCoordinate[0] : X value in m
// AntCoordinate[1] : Y value in m
// AntCoordinate[2] : Z value in m


  if(daq_id == 17){

    if(strcmp(AntPos,"CTR")==0){ AntCoordinate[0] =  47.233; AntCoordinate[1] = -283.658; AntCoordinate[2] = 126.803;}
    if(strcmp(AntPos,"060")==0){ AntCoordinate[0] = 109.353; AntCoordinate[1] = -247.433; AntCoordinate[2] = 126.235;}
    if(strcmp(AntPos,"120")==0){ AntCoordinate[0] = 111.854; AntCoordinate[1] = -318.309; AntCoordinate[2] = 127.982;}
    if(strcmp(AntPos,"180")==0){ AntCoordinate[0] =  47.836; AntCoordinate[1] = -355.645; AntCoordinate[2] = 126.878;}
  }
  else{
  if(daq_id == 19){

    if(strcmp(AntPos,"CTR")==0){ AntCoordinate[0] = -188.793; AntCoordinate[1] = -273.553; AntCoordinate[2] = 127.298;}
    if(strcmp(AntPos,"030")==0){ AntCoordinate[0] = -162.258; AntCoordinate[1] = -221.831; AntCoordinate[2] = 128.010;}
  
  }
  else{
  if(daq_id == 30){

    if(strcmp(AntPos,"CTR")==0){ AntCoordinate[0] = -186.004; AntCoordinate[1] = -471.813; AntCoordinate[2] = 126.826;}
    if(strcmp(AntPos,"240")==0){ AntCoordinate[0] = -227.652; AntCoordinate[1] = -498.038; AntCoordinate[2] = 128.327;}
    if(strcmp(AntPos,"300")==0){ AntCoordinate[0] = -228.187; AntCoordinate[1] = -447.265; AntCoordinate[2] = 127.604;}
    if(strcmp(AntPos,"360")==0){ AntCoordinate[0] = -186.455; AntCoordinate[1] = -422.287; AntCoordinate[2] = 127.218;}
  
  }
  else{
  if(daq_id == 42){//equiliteral triangle with baseline 100m, CTR at origin

    if(strcmp(AntPos,"CTR")==0){ AntCoordinate[0] =   0.0; AntCoordinate[1] =  0.0; AntCoordinate[2] = 0.0;}
    if(strcmp(AntPos,"030")==0){ AntCoordinate[0] =  50.0; AntCoordinate[1] = 86.6; AntCoordinate[2] = 0.0;}
    if(strcmp(AntPos,"090")==0){ AntCoordinate[0] = 100.0; AntCoordinate[1] =  0.0; AntCoordinate[2] = 0.0;}
  
  }
  }}}
  
}

/**********************************************************************************************/

void GetBaryCenter(const int NoChannels, float **AntCoordinate, const float *PulseAmp, float *b){
 float sum = 0, sumx = 0, sumy = 0, sumz = 0;
 
 for(int i=0; i<NoChannels; i++){
   sum  += PulseAmp[i];
   sumx += PulseAmp[i] * AntCoordinate[i][0];
   sumy += PulseAmp[i] * AntCoordinate[i][1];
   sumz += PulseAmp[i] * AntCoordinate[i][2];
 }

 b[0] = sumx / sum;
 b[1] = sumy / sum;
 b[2] = sumz / sum;
 
}

/**********************************************************************************************/

void PlaneFit(const int NoChannels, char **AntPos, const int daq_id, const float *PulseAmp, float *PulseTime,
              float *Zenith, float *Azimuth){
  bool debug = false;
	      
  double c = 2.99*pow(10.0,8);
  double sig_t = 1*12.5*pow(10.0,-9); //expected error in time
  double PI = 3.14159265;
  
  float **AntCoordinate = new float *[NoChannels];
  for(int i=0; i<NoChannels; i++) AntCoordinate[i] = new float [3];
  
  //Get Coordinates of the Antennas
  for(int i=0; i<NoChannels; i++) GetAntPos(AntPos[i], daq_id, AntCoordinate[i]);
  
  float b[3];
  b[0] = 0;
  b[1] = 0;
  b[2] = 0;
  //GetBaryCenter of used channels
  GetBaryCenter(NoChannels, AntCoordinate, PulseAmp, b);
  //cout << "\nBarycenter: x = " << b[0] << " - y = " << b[1] << " - z = " << b[2] << endl;
  
  double S_xx = 0, S_xy = 0, S_x = 0, S_yy = 0, S_y = 0, S_1 = 0, S_t = 0, S_yt = 0, S_xt = 0;
  for(int i=0; i<NoChannels; i++){
    S_x  += (AntCoordinate[i][0]-b[0]) / (pow(c,2) * pow(sig_t,2));    
    S_xx += pow((AntCoordinate[i][0]-b[0]),2) / (pow(c,2) * pow(sig_t,2));    
    S_xy += ((AntCoordinate[i][0]-b[0])*(AntCoordinate[i][1]-b[1])) / (pow(c,2) * pow(sig_t,2));    
    S_y  += (AntCoordinate[i][1]-b[1]) / (pow(c,2) * pow(sig_t,2));    
    S_yy += pow((AntCoordinate[i][1]-b[1]),2) / (pow(c,2) * pow(sig_t,2));    
    S_t  += c*PulseTime[i] / (pow(c,2) * pow(sig_t,2));
    S_xt += (AntCoordinate[i][0]-b[0])*c*PulseTime[i] / (pow(c,2) * pow(sig_t,2));
    S_yt += (AntCoordinate[i][1]-b[1])*c*PulseTime[i] / (pow(c,2) * pow(sig_t,2));
    S_1  += 1 / (pow(c,2) * pow(sig_t,2));
  }
  
  // u, v, w are the components of the vector pointing to the soruce
  // impact point: u, v
  double D = 0; //determinant 
  double u = 0, var_u = 0;
  double v = 0, var_v = 0;
  double var_uv = 0;
  double w = 0, var_w = 0;
  bool zenith_err=false;
  
  D = S_1*S_xx*S_yy - S_1*pow(S_xy,2) + 2*S_x*S_y*S_xy - pow(S_y,2)*S_xx - pow(S_x,2)*S_yy;
  u = (S_xt*pow(S_y,2) - S_t*S_y*S_xy + (S_1*S_xy - S_x*S_y)*S_yt + (S_t*S_x - S_1*S_xt)*S_yy) / D;
  v = (S_yt*pow(S_x,2) - S_t*S_x*S_xy + (S_1*S_xy - S_x*S_y)*S_xt + (S_t*S_y - S_1*S_yt)*S_xx) / D;
//  w = sqrt( Abs(1-pow(u,2)-pow(v,2)));
  w = (1-pow(u,2)-pow(v,2));
//  if(w<0) w *= -1.0; //norm constraint?!
  if(w<0) zenith_err=true;
  w = sqrt(w);
  
  var_u  = (S_1*S_yy - pow(S_y,2)) / D;
  var_v  = (S_1*S_xx - pow(S_x,2)) / D;
  var_uv = -1*(S_1*S_xy - S_x*S_y) / D;
  var_w  = pow(u,2)*var_v+pow(v,2)*var_u-2*u*v*var_uv / pow(w,2);
  
  if(debug){
   printf("\nx = %f +/- %f\n",u, sqrt(var_u));
   printf("y = %f +/- %f\n",v, sqrt(var_v));
   printf("z = %f +/- %f\n",w, sqrt(var_w));
   printf("Determinant = %f\n",D);
  }
  
  double tan_zenith  = sqrt( (pow(u,2)+pow(v,2))) / w;
  double tan_azimuth = (v/u);
  
  //conversion from radians to degree
  if(zenith_err) Zenith[0]=-1;
  else Zenith[0]  = atan(tan_zenith) * 180 / PI;
  Azimuth[0] = atan(tan_azimuth) * 180 / PI;


  if(u>0 && v>0) Azimuth[0]  =  90 - Azimuth[0];
  if(u<0 && v>0) Azimuth[0] += 270;
  if(u<0 && v<0) Azimuth[0]  = 270 - Azimuth[0];
  if(u>0 && v<0) Azimuth[0] +=  90;

  //correction for geographic north
  // KASCADE Grid -> geographic north
  Azimuth[0] += 15.7;
  
  //deviation
  if(zenith_err) Zenith[1]=-1;
  else Zenith[1]  = sqrt((pow(u,2)*var_u + pow(v,2)*var_v + 2*u*v*var_uv) / ( ( pow(u,2) + pow(v,2) )*pow(w,2) )) * 180.0/PI;
  Azimuth[1] = sqrt((pow(u,2)*var_v + pow(v,2)*var_u - 2*u*v*var_uv) / (pow( (pow(u,2) + pow(v,2)),2))) * 180.0/PI;


  for(int i=0; i<NoChannels; i++){
    delete[] AntCoordinate[i];
  }
  delete[] AntCoordinate;
}

/**********************************************************************************************/
