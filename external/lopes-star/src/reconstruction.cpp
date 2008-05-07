#include "reconstruction.hh"

/**********************************************************************************************/

void ZeroPaddingFFT(int window_size, short int *trace, int NoZeros, float* ZPTrace, bool bSubtractPedestal){
  float *FFTdata = new float [window_size*2];
  float *Amp = new float [window_size/2];
  float *Phase = new float [window_size/2];
  
  TraceFFT(window_size, trace, Amp, Phase, FFTdata, true, false, bSubtractPedestal);
  ZeroPaddingFFTCore(window_size, FFTdata, NoZeros, ZPTrace);
  
  delete[] FFTdata;
  delete[] Amp;
  delete[] Phase;
}

/**********************************************************************************************/

void ZeroPaddingFFT(int window_size, float *trace, int NoZeros, float* ZPTrace, bool bSubtractPedestal){
  float *FFTdata = new float [window_size*2];
  float *Amp = new float [window_size/2];
  float *Phase = new float [window_size/2];
  
  TraceFFT(window_size, trace, Amp, Phase, FFTdata, true, false, bSubtractPedestal);
  ZeroPaddingFFTCore(window_size, FFTdata, NoZeros, ZPTrace);

  delete[] FFTdata;
  delete[] Amp;
  delete[] Phase;
}

/**********************************************************************************************/

void ZeroPaddingFFT(int window_size, double *Dtrace, int NoZeros, double* DZPTrace, bool bSubtractPedestal){
  float *FFTdata = new float [window_size*2];
  float *Amp = new float [window_size/2];
  float *Phase = new float [window_size/2];
  float *trace = new float [window_size];
  float *ZPTrace = new float [window_size*(NoZeros+1)];
  
  for(int i=0; i<window_size; i++) trace[i] = Dtrace[i];
 
  TraceFFT(window_size, trace, Amp, Phase, FFTdata, true, false, bSubtractPedestal);
  ZeroPaddingFFTCore(window_size, FFTdata, NoZeros, ZPTrace);

  for(int i=0; i<window_size*(NoZeros+1); i++) DZPTrace[i] = ZPTrace[i];
  
  delete[] FFTdata;
  delete[] Amp;
  delete[] Phase;
  delete[] trace;
  delete[] ZPTrace;
}

/**********************************************************************************************/

void ZeroPaddingFFTCore(int window_size, float *FFTdata, int NoZeros, float* ZPTrace){
  
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
  RFISuppressionCutOff( window_size, trace, trace_suppressed, NoZeros);
}

/**********************************************************************************************/

void RFISuppressionCutOff(int window_size, float *trace, float *trace_suppressed, int NoZeros){

  float *PHASE = new float [window_size/2];
  float *AMP = new float [window_size/2];
  float *AMP_orig = new float [window_size/2];
  float *AMP_normalised = new float [window_size/2];
  for(int i=0; i<window_size/2; i++) AMP_normalised[i] = 0;
  float *AMP_corrected = new float [window_size/2];
  float *AMP_suppressed = new float [window_size/2];
  float *FFTData = new float [window_size*2];
  int EntriesInterval = 64*window_size/1024; //also possible 2, 4, 8, 16, 32, 64, 128 to get no remainder
  float mean_tmp, rms_tmp;
  float threshold;
  float *med_tmp = new float [window_size/2];
  float med_lower, med_upper;
  float *AMP_tmp = new float [window_size/2];
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
    AMP[a] = (float)TMath::Median(EntriesInterval, med_tmp);
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
  AmpPhase2Trace(window_size, AMP_suppressed , PHASE, trace_suppressed, FFTData, NoZeros);


  //clean up
  delete[] PHASE;
  delete[] AMP;
  delete[] AMP_orig;
  delete[] AMP_normalised;
  delete[] AMP_corrected;
  delete[] AMP_suppressed;
  delete[] FFTData;
  delete[] med_tmp;
  delete[] AMP_tmp;
  

}
/**********************************************************************************************/

void RFISuppressionMedian(int window_size, float *trace, float *trace_suppressed, int NoZeros){

  float *PHASE = new float [window_size/2];
  float *AMP = new float [window_size/2];
  float *AMP_orig = new float [window_size/2];
  float *AMP_normalised = new float [window_size/2];
  for(int i=0; i<window_size/2; i++) AMP_normalised[i] = 0;
  float *AMP_suppressed = new float [window_size/2];
  float *FFTData = new float [window_size*2];
  int EntriesInterval = 64*window_size/1024; //also possible 2, 4, 8, 16, 32, 64, 128 to get no remainder
  float *med_tmp = new float [window_size/2];
  float med_lower, med_upper;

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
    AMP[a] = (float)TMath::Median(EntriesInterval, med_tmp);
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
  
  //replace Amp by median
  for(int a=0; a<window_size/2; a++) {
    AMP_suppressed[a] = sqrt(AMP[a]);
  }
  
  //recalculate the corrected time trace
  AmpPhase2Trace(window_size, AMP_suppressed , PHASE, trace_suppressed, FFTData, NoZeros);

  //clean up
  delete[] PHASE;
  delete[] AMP;
  delete[] AMP_orig;
  delete[] AMP_normalised;
  delete[] AMP_suppressed;
  delete[] FFTData;
  delete[] med_tmp;

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
  float *TraceDummy = new float [New_window_size];
  float *RawTrace = new float [window_size];
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
  Rectifier(NoZeros, New_window_size, TraceDummy);
   
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

  //clean up
  delete[] TraceDummy;
  delete[] RawTrace;

}

/**********************************************************************************************/

void EstimatePulseParameter(	int window_size, const float *trace, float *PulseLength,
				float *PulsePosition, float *Maximum, float *IntegralPre,
				float *IntegralPost, float *IntegralPulse, float threshold, int NoZeros){
  
  bool debug = false;
  
  int PStart = 0;
  int PStop = 0;
  float TraceRMS = 0;
  float TraceMean = 0;
  int New_window_size = window_size*(NoZeros+1);
  float *TraceDummy = new float [New_window_size];
  for(int i=0; i<window_size; i++){
    TraceDummy[i] = trace[i];
  }
  bool found_peak=false;
  bool dynamic_thres=false;
  if(threshold==0) dynamic_thres = true;
  
  *PulseLength = 0;
  *PulsePosition = 0;

  if(NoZeros>0) ZeroPaddingFFT(window_size, TraceDummy, NoZeros,  TraceDummy);
   
  //make envelope
//  RectifierFFT(NoZeros, New_window_size, TraceDummy);
  RectifierHilbertSquare(New_window_size, TraceDummy);
   
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
   threshold = (TraceMean+5*TraceRMS);
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
  *IntegralPre = 0;
  *IntegralPost = 0;
  int Int_Start = PStart - (20 * (NoZeros+1)); // 250 ns
  if(Int_Start < 0 ) Int_Start = 0;
  int Int_Stop = PStop + ((int)*PulseLength * 2 * (NoZeros+1));
  if(Int_Stop > New_window_size) Int_Stop = New_window_size;
  
  for(int i=Int_Start; i<PStart; i++) *IntegralPre += TraceDummy[i];
  for(int i=PStop; i<Int_Stop; i++) *IntegralPost += TraceDummy[i];
  // norm by window size and no of zeros
  *IntegralPre = *IntegralPre / ((double) (NoZeros+1)) / ((double) window_size);
  *IntegralPost = *IntegralPost / ((double) (NoZeros+1)) / ((double) window_size);
  
  //calculate integral over the found pulse width
  *IntegralPulse = 0;
  for(int i=PStart; i<PStop; i++) *IntegralPulse += TraceDummy[i];
  *IntegralPulse = *IntegralPulse / ((double) (NoZeros+1)) / ((double) window_size);

  if(debug) cout << "PulseLengthEstimate:" << endl;
  if(debug) cout << "Peak: Max = " << *Maximum << " - Position = " << *PulsePosition << endl;
  if(debug) cout << "Integral (pre) = " << *IntegralPre << endl;
  if(debug) cout << "Integral (post) = " << *IntegralPost << endl;
  if(debug) cout << "IntegralPulse = " << *IntegralPulse << endl;
  if(debug) cout << "estimate length of max pulse= " << *PulseLength << " - Start = ";
  if(debug) cout << PStart/(NoZeros+1) << " - Stop = " << PStop/(NoZeros+1) << endl;
  
  delete TraceDummy;

}

/**********************************************************************************************/

void GetAntPos(const char *AntPos, const int daq_id, float *AntCoordinate){
// AntCoordinate[0] : X value in m
// AntCoordinate[1] : Y value in m
// AntCoordinate[2] : Z value in m


  if(daq_id == 17 || daq_id == 0){

    if(strcmp(AntPos,"CTR")==0){ AntCoordinate[0] =  43.764; AntCoordinate[1] = -284.214; AntCoordinate[2] = 126.803;}
    if(strcmp(AntPos,"060")==0){ AntCoordinate[0] = 106.322; AntCoordinate[1] = -248.751; AntCoordinate[2] = 126.235;}
    if(strcmp(AntPos,"120")==0){ AntCoordinate[0] = 107.956; AntCoordinate[1] = -319.652; AntCoordinate[2] = 127.982;}
    if(strcmp(AntPos,"180")==0){ AntCoordinate[0] =  43.488; AntCoordinate[1] = -356.203; AntCoordinate[2] = 126.878;}
  }
  else{
  if(daq_id == 19){

    if(strcmp(AntPos,"CTR")==0){ AntCoordinate[0] = -192.121; AntCoordinate[1] = -271.226; AntCoordinate[2] = 127.298;}
    if(strcmp(AntPos,"030")==0){ AntCoordinate[0] = -164.956; AntCoordinate[1] = -219.832; AntCoordinate[2] = 128.010;}
  
  }
  else{
  if(daq_id == 30){

    if(strcmp(AntPos,"CTR")==0){ AntCoordinate[0] = -191.754; AntCoordinate[1] = -469.506; AntCoordinate[2] = 126.826;}
    if(strcmp(AntPos,"240")==0){ AntCoordinate[0] = -233.720; AntCoordinate[1] = -495.219; AntCoordinate[2] = 128.327;}
    if(strcmp(AntPos,"300")==0){ AntCoordinate[0] = -233.634; AntCoordinate[1] = -444.444; AntCoordinate[2] = 127.604;}
    if(strcmp(AntPos,"360")==0){ AntCoordinate[0] = -191.601; AntCoordinate[1] = -419.977; AntCoordinate[2] = 127.218;}
  
  }
  else{
  if(daq_id == 42){

    if(strcmp(AntPos,"CTR")==0){ AntCoordinate[0] = -26973.0; AntCoordinate[1] = -5699.0; AntCoordinate[2] = 1423.0;}
    if(strcmp(AntPos,"030")==0){ AntCoordinate[0] = -26900.0; AntCoordinate[1] = -5771.0; AntCoordinate[2] = 1423.0;}
    if(strcmp(AntPos,"090")==0){ AntCoordinate[0] = -26874.0; AntCoordinate[1] = -5673.0; AntCoordinate[2] = 1423.0;}
  
  }
  else{
  if(daq_id == 50){

    if(strcmp(AntPos,"000")==0){ AntCoordinate[0] = -26973.0; AntCoordinate[1] = -5699.0; AntCoordinate[2] = 1423.0;}
    if(strcmp(AntPos,"001")==0){ AntCoordinate[0] = -26900.0; AntCoordinate[1] = -5771.0; AntCoordinate[2] = 1423.0;}
    if(strcmp(AntPos,"002")==0){ AntCoordinate[0] = -26874.0; AntCoordinate[1] = -5673.0; AntCoordinate[2] = 1423.0;}
  
  }
  else{
  cerr << "daq id " << daq_id << " is not defined" << endl;
  }}}}}
  
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

void PlaneFit(const int NoChannels, char **AntPos, const int daq_id, const float *PulseAmp, float *PulseTime, float *Zenith, float *Azimuth){

  float **AntCoordinate = new float *[NoChannels];
  for(int i=0; i<NoChannels; i++) AntCoordinate[i] = new float [3];
  
  //Get Coordinates of the Antennas
  for(int i=0; i<NoChannels; i++) GetAntPos(AntPos[i], daq_id, AntCoordinate[i]);
  
  PlaneFitCore(NoChannels, AntCoordinate, PulseAmp, PulseTime, Zenith, Azimuth);
  
  for(int i=0; i<NoChannels; i++){
    delete[] AntCoordinate[i];
  }
  delete[] AntCoordinate;
}	      

/**********************************************************************************************/

void PlaneFitCore(const int NoChannels, float **AntCoordinate, const float *PulseAmp, float *PulseTime, float *Zenith, float *Azimuth){
  bool debug = false;
  
  double c = 2.99*pow(10.0,8);
  double sig_t = 12.5*pow(10.0,-9); //expected error in time
  double PI = 3.14159265;
  
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
  if(w<0){
   w *= -1.0; //norm constraint?!
   zenith_err=true;
  }
#warning what about the norm constraint of w?
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
  
  //conversion from radians to degree
  if(zenith_err) Zenith[0]  = -1 * atan(tan_zenith) * 180.0 / PI; 
  else           Zenith[0]  =      atan(tan_zenith) * 180.0 / PI;
  
  
  //Azimuth part
  Azimuth[0] = atan2(u,v);

  if(Azimuth[0] < -0.0001) Azimuth[0] += 2.0*PI;
  if(v>0.0000001){
    if(u<0 && !(fabs(u)<0.0001)) Azimuth[0] += 2*PI;
  }
  if(fabs(v)<0.0000001){
    if(u>0.0000001) Azimuth[0] =  PI / 2.0;
    if(u<-0.0000001) Azimuth[0] = 3 * PI / 2.0;
  }

  //convert from radian to degree
  Azimuth[0] *=  180.0 / PI;

  
  //correction for geographic north
  // KASCADE Grid -> geographic north
  Azimuth[0] += 15; // former 15degree
//  if(Azimuth[0] > 360) Azimuth[0] -= 360;  
  while(Azimuth[0]>360){
    Azimuth[0] -= 360;
    cerr << "Azimuth corrected by 360 degree." << endl;
  }
  //deviation
  Zenith[1]  = sqrt((pow(u,2)*var_u + pow(v,2)*var_v + 2*u*v*var_uv) / ( ( pow(u,2) + pow(v,2) )*pow(w,2) )) * 180.0/PI;
  Azimuth[1] = sqrt((pow(u,2)*var_v + pow(v,2)*var_u - 2*u*v*var_uv) / (pow( (pow(u,2) + pow(v,2)),2))) * 180.0/PI;
}

/**********************************************************************************************/

void RecDirectionSingleArray(const int NoChannels, int window_size, float **trace, char **AntPos, const int daq_id, float *Zenith, float *Azimuth, bool RFISupp){
    
  bool debug = false;
  float **AntCoordinate = new float *[NoChannels];
  for(int i=0; i<NoChannels; i++) AntCoordinate[i] = new float [3];
  
  //Get Coordinates of the Antennas
  for(int i=0; i<NoChannels; i++) GetAntPos(AntPos[i], daq_id, AntCoordinate[i]);
  
  //Get Pulse Position and Amplitude
  int NoZeros = 7;
  int new_window_size = window_size * (NoZeros+1);
  int delta = (int) (GetCoincidenceTime(daq_id, 65.0) / (12.5) * (NoZeros+1) / 1);
  int *Position = new int [NoChannels];
  float *threshold = new float [NoChannels];
  float *PulseAmp = new float [NoChannels];
  float *PulseTime = new float [NoChannels]; // in nanoseconds
  for(int i=0; i<NoChannels; i++){
   Position[i] = 0;
   threshold[i] = 0;
  }
    
  float **Trace = new float *[NoChannels];
  for(int i=0; i<NoChannels; i++) Trace[i] = new float [new_window_size];
  for(int i=0; i<NoChannels; i++) for(int j=0; j<new_window_size; j++) Trace[i][j] = trace[i][j];

  for(int i=0; i<NoChannels; i++){
    if(RFISupp) RFISuppressionMedian(window_size, Trace[i], Trace[i]);   
    DynamicThreshold(window_size, Trace[i], &threshold[i], NoZeros);
    ZeroPaddingFFT(window_size, Trace[i], NoZeros, Trace[i]);
    RectifierHilbertSquare(new_window_size, Trace[i]);
  }
  
  PeakDetection(NoChannels, threshold, new_window_size, Trace, Position, delta);
  
  float tmpPos;
  for(int i=0; i<NoChannels; i++){
    if(Position[i] > 0){
      tmpPos = 0;
      VarPeakWidth(new_window_size, Trace[i], 0.5, &tmpPos, &Position[i], 0, false);
      Position[i] = (int)tmpPos;
    }
  }
  
  //check for channels with no detected peak and re-order
  int good_channels = 0;
  for(int i=0; i<NoChannels; i++){
    if(Position[i]>0){
      PulseAmp[good_channels] = 1;
      PulseTime[good_channels] = Position[i]*12.5/(NoZeros+1)*(1.0e-9);//now nano seconds
      GetAntPos(AntPos[i], daq_id, AntCoordinate[good_channels]);
      good_channels++;
    }
  }
  if(debug) cout << "found " << good_channels << " good channels for direction reconstruction. all = " << NoChannels << endl;
  if(debug) printf("%.20f -- %s\n", PulseTime[0], AntPos[0]);
  PlaneFitCore(good_channels, AntCoordinate, PulseAmp, PulseTime, Zenith, Azimuth);
  

  for(int i=0; i<NoChannels; i++){
    delete[] AntCoordinate[i];
  }
  delete[] AntCoordinate;
  for(int i=0; i<NoChannels; i++) delete[] Trace[i];
  delete[] Trace;
  delete[] Position;
  delete[] threshold;
  delete[] PulseAmp;
  delete[] PulseTime;

}	      

/**********************************************************************************************/

float GeoMagAngle(float Azimuth, float Zenith, int daq_id){
  float BAzimuth = 0;
  float BZenith = 0;
  float GMAngle = 0;
  
  //B field definition at FZK
#warning check geomagnetic field defintion with different models
  if(daq_id == 17 || daq_id==0 || daq_id == 19 || daq_id == 30){
    BAzimuth = 180.0 * Pi() / 180.0;
    BZenith =  25.0 * Pi() / 180.0;
  }
 
  if(daq_id == 42 || daq_id == 50){
    BAzimuth = 180.0 * Pi() / 180.0;
    BZenith =  55.0 * Pi() / 180.0;
  }
  
  if(BAzimuth == 0) cerr << "daq id " << daq_id << " is not defined" << endl;
  
  //angles in radians
  float tAzimuth = Azimuth * Pi() / 180.0;
  float tZenith = Zenith * Pi() / 180.0;
  
  GMAngle = cos(tAzimuth)*sin(tZenith)*cos(BAzimuth)*sin(BZenith) 
  	  + sin(tAzimuth)*sin(tZenith)*sin(BAzimuth)*sin(BZenith)
	  + cos(tZenith)*cos(BZenith);
  GMAngle = acos(fabs(GMAngle));
  
  //conversion to degree
  GMAngle *= 180.0 / Pi();
  return GMAngle;
}

/**********************************************************************************************/

float SNR(int window_size, float *trace, int NoZeros, bool DoRectifier){
  float snr = 0;
  int new_window_size = window_size * (NoZeros + 1);
  float *tmpTrace = new float [new_window_size];
  
  if(NoZeros>0) ZeroPaddingFFT(window_size, trace, NoZeros, tmpTrace);
  else for(int i=0; i<new_window_size; i++) tmpTrace[i] = trace[i];
  if(DoRectifier) RectifierHilbertSquare(new_window_size, tmpTrace);
//  else for(int i=0; i<new_window_size; i++) tmpTrace[i] = powf(tmpTrace[i],2.);
  
  //find maximum
  float max = 0;
  int max_pos = 0;
  for(int i=0; i<new_window_size; i++){
    if(tmpTrace[i]>max){
      max = tmpTrace[i];
      max_pos = i ;
    }
  }
  
  //define array around peak to calculate mean
  float *tmpTraceMean = new float [new_window_size];
  int countTraceMean = 0;
  int StartPre  = 0;
  int StopPre   = max_pos - (NoZeros+1)*10;
  int StartPost = max_pos + (NoZeros+1)*10;
  int StopPost  = new_window_size;
  
  if(StopPre < 0) StopPre = 0;
  if(StartPost > new_window_size) StartPost = new_window_size;
  
  for(int i=StartPre; i<StopPre; i++){
    tmpTraceMean[countTraceMean] = tmpTrace[i];
    countTraceMean++;
  }

  for(int i=StartPost; i<StopPost; i++){
    tmpTraceMean[countTraceMean] = tmpTrace[i];
    countTraceMean++;
  }
  
  //calculate snr
  float mean = 0;
  mean = Mean(countTraceMean, tmpTraceMean);
  snr = max / mean;
  
  delete[] tmpTrace;
  delete[] tmpTraceMean;

  return snr;
}

/**********************************************************************************************/

void GetDistanceToShowerAxis(struct h3 *h3, char *AntPos, int daq_id, float Zenith, float Azimuth, float *distance, float *error){

  //antenna position vector a
  float a[3];
  GetAntPos(AntPos, daq_id, a);

  GetDistanceToShowerAxisRadio(h3->Xcg, h3->Ycg, a, Zenith, Azimuth, distance, error);
/*
  *distance = 0;
  float c_err = 7;
  float ZenithRad = Zenith * TMath::Pi() / 180.;
  float AzimuthRad = Azimuth * TMath::Pi() / 180.;
  
  //direction vector: b, given by spherical coordinates
  float b1 = sinf(ZenithRad) * cosf(AzimuthRad);
  float b2 = sinf(ZenithRad) * sinf(AzimuthRad);
  float b3 = cosf(ZenithRad);
  
  //Shower Core Position taken from Grande: c
  float c1 = h3->Xcg;
  float c2 = h3->Ycg;
  float c3 = 0;
  
  
  float a1 = a[0];
  float a2 = a[1];
  float a3 = a[2];
  
  //distance d = (|b x ( f )| ) / | b| ) with f = a - c
  float f1 = a1 - c1;
  float f2 = a2 - c2;
  float f3 = a3 - c3;
  
  //absolute value of b
  float absb = sqrt( powf(b1,2.) + powf(b2,2.) + powf(b3,2.));

  *distance = sqrt( powf(b2*f3-b3*f2,2.) + powf(b3*f1-b1*f3,2.) + powf(b1*f2-b2*f1,2.) ) / absb ;
  
  float tmperror = powf((powf(b3,2.) + powf(b2,2.))*(2*a1 - 2*c1) - 2*b1*b3*(a3-c3) - 2*b1*b2*(a2-c2),2.) * powf(c_err,2.) 
  		 + powf((powf(b3,2.) + powf(b1,2.))*(2*a2 - 2*c2) - 2*b2*b3*(a3-c3) - 2*b1*b2*(a1-c1),2.) * powf(c_err,2.) ;

  *error = *distance * 0.5 * sqrt(tmperror) / powf(*distance,2.);
*/
}

/**********************************************************************************************/

void GetDistanceToShowerAxisRadio(float CorePosX, float CorePosY, float *AntPosCoordinates, float Zenith, float Azimuth, float *distance, float *error){
  *distance = 0;
  float c_err = 7;
  float ZenithRad = Zenith * TMath::Pi() / 180.;
  float AzimuthRad = (180-(Azimuth-15.7)) * TMath::Pi() / 180.;
  
  //direction vector: b, given by spherical coordinates
  float b1 = sinf(ZenithRad) * cosf(AzimuthRad);
  float b2 = sinf(ZenithRad) * sinf(AzimuthRad);
  float b3 = cosf(ZenithRad);
  
  //Shower Core Position taken from Grande: c
  float c1 = CorePosY;
  float c2 = -1*CorePosX;
  float c3 = 0;
  
  //antenna position vector a
  float a[3];
  a[0] = AntPosCoordinates[1];
  a[1] = -1*AntPosCoordinates[0];
  a[2] = 0;

  float a1 = a[0];
  float a2 = a[1];
  float a3 = a[2];
  
  //distance d = (|b x ( f )| ) / | b| ) with f = a - c
  float f1 = a1 - c1;
  float f2 = a2 - c2;
  float f3 = a3 - c3;
  
  //absolute value of b
  float absb = sqrt( powf(b1,2.) + powf(b2,2.) + powf(b3,2.));

  *distance = sqrt( powf(b2*f3-b3*f2,2.) + powf(b3*f1-b1*f3,2.) + powf(b1*f2-b2*f1,2.) ) / absb ;
  
  float tmperror = powf((powf(b3,2.) + powf(b2,2.))*(2*a1 - 2*c1) - 2*b1*b3*(a3-c3) - 2*b1*b2*(a2-c2),2.) * powf(c_err,2.) 
  		 + powf((powf(b3,2.) + powf(b1,2.))*(2*a2 - 2*c2) - 2*b2*b3*(a3-c3) - 2*b1*b2*(a1-c1),2.) * powf(c_err,2.) ;

  *error = *distance * 0.5 * sqrt(tmperror) / powf(*distance,2.);
};

/**********************************************************************************************/

float GetChannelPeak(int window_size, float *trace, int ExpectedPosition){
  float PeakValue = trace[ExpectedPosition];
  return PeakValue;
};

/**********************************************************************************************/

void CalculateAntennaFieldStrength(int NoChannels, const float *ChannelFieldStrength, char **AntPosition, float *AntennaFieldStrength){
  
  for(int i=0; i<NoChannels-1; i++){
    for(int j=i+1; j<NoChannels; j++){
      if( strncmp(AntPosition[i],AntPosition[j],3) == 0){
        AntennaFieldStrength[i] = sqrt( powf(ChannelFieldStrength[i],2.) + powf(ChannelFieldStrength[j],2.) );
	AntennaFieldStrength[j] = AntennaFieldStrength[i];
      }
    }
  }
  
};

/**********************************************************************************************/

void CalculateTransformedFieldStrength( int NoChannels, const float *ChannelFieldStrength,char **AntPosition,
					char **AntPolarisation, float *EPhi, float *ETheta, float Azimuth){

  
  float AzimuthRad = Azimuth * TMath::Pi() / 180.;
  float E_EW = 0;
  float E_NS = 0;
  
  for(int i=0; i<NoChannels-1; i++){
    for(int j=i+1; j<NoChannels; j++){
      if( strncmp(AntPosition[i],AntPosition[j],3) == 0){
        E_EW = 0; E_NS = 0;
	if( strncmp(AntPolarisation[i],"East/West",9) == 0){
	  E_EW = ChannelFieldStrength[i];
	  E_NS = ChannelFieldStrength[j];
	}
	else{
	  E_EW = ChannelFieldStrength[j];
	  E_NS = ChannelFieldStrength[i];
	  if(strncmp(AntPolarisation[j],"East/West",9) != 0) cerr << "wrong polarisation found" << endl;
	}
	ETheta[i] = ( E_EW - tanf(AzimuthRad) * E_NS ) / ( ( powf(cosf(AzimuthRad),2.) - powf(sinf(AzimuthRad),2.) ) / cosf(AzimuthRad) );
	EPhi[i]   = E_NS / cosf(AzimuthRad) - tanf(AzimuthRad) * ETheta[i];
	EPhi[j]   = EPhi[i];
	ETheta[j] = ETheta[i];
      }
    }
  }

};

/**********************************************************************************************/
