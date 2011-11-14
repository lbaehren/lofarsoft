#include "calibration.hh"


/**********************************************************************************************/

void ADC2FieldStrength(unsigned int window_size, float *trace, int daq_id, float zenith, bool RefAnt){
  //result is in uV/m
  float *Amp = new float [window_size/2];
  float *Phase  = new float [window_size/2];
  float *rawfft = new float [2*window_size];

  TraceFFT(window_size, trace, Amp, Phase, rawfft);  

  ADC2FieldStrengthCore(window_size, Amp, Phase, daq_id, zenith, RefAnt);
  
  AmpPhase2Trace(window_size, Amp, Phase, trace, rawfft, 0);
  
  //clean up
  delete[] Amp;
  delete[] Phase;
  delete[] rawfft;
  
}

/**********************************************************************************************/

void ADC2FieldStrengthUpSampled(unsigned int window_size, float *SubTrace, int NoZeros, float *UpTrace, int daq_id, float zenith, bool RefAnt){
  //result is in uV/m
  float *Amp = new float [window_size/2];
  float *Phase  = new float [window_size/2];
  float *rawfft = new float [2*window_size];

  TraceFFT(window_size, SubTrace, Amp, Phase, rawfft);  

  ADC2FieldStrengthCore(window_size, Amp, Phase, daq_id, zenith, RefAnt);
  
  ZeroPaddingFFTCore(window_size, rawfft, NoZeros, UpTrace);

  //clean up
  delete[] Amp;
  delete[] Phase;
  delete[] rawfft;
}

/**********************************************************************************************/

void ADC2FieldStrengthCore(int window_size, float *Amp, float *Phase, int daq_id, float zenith, bool RefAnt){
  if(daq_id==42) RefAnt=false;
  float freq = 0;
  if(RefAnt){ // calibration by Ref. Antenna
    if(daq_id == 17 || daq_id==0 || daq_id == 30 || daq_id == 19){
      for(int i=0; i<window_size/2; i++){
        freq = 40+(float)((float)(i*40.0)/(float)(window_size/2));
        Amp[i] *= ( FieldStrengthCorrection_RefAnt(freq) / sqrt(AntennaGain(freq,zenith)) );
      }
    }else{
      cerr << "ADC2FieldStrength: daq id = " << daq_id << " not valid" << endl;
      exit(1);
    }
  }
  else{ // calibration by lab. measurement
    for(int i=0; i<window_size/2; i++){
      freq = 40+(float)((float)(i*40.0)/(float)(window_size/2));
      Amp[i] /= ( FieldStrengthElectronicsGain(freq) * sqrt(AntennaGain(freq,zenith)) );
      Amp[i] *= ( sqrt(120.0 * Pi() / 50.0) * 1.0e6 / 4096.0 );
      
      if(daq_id == 17 || daq_id==0 || daq_id == 30 || daq_id == 19){
        Amp[i] *= ( pow(10, CableAttenuationRG058(4, freq) / 20.0) * pow(10, CableAttenuationRG213(100, freq) / 20.0) );
      }else{
      if(daq_id == 42){
        Amp[i] *= ( pow(10, CableAttenuationRG058(4, freq) / 20.0) * pow(10, CableAttenuationRG213(160, freq) / 20.0) );
      }else{
      if(daq_id == 50){
#warning daq_id 50 is not definded for calibration
      }
      else{
        cerr << "CalibratedFrequencyDomain: daq id = " << daq_id << " not valid" << endl;
	cerr << "no calibration is performed" << endl;
      }}}
      
    }
  }

}

/**********************************************************************************************/

void CalibratedFrequencyDomain(int window_size, float *trace, float *Amp, float *Phase, int daq_id, float zenith, bool RefAnt, bool data_window){

  float RawFFT[window_size*2];
  float freq;
  if(daq_id==42) RefAnt=false;
 
  if(RefAnt){
    TraceFFT(window_size, trace, Amp, Phase, RawFFT, data_window);
    
    if(daq_id == 17 || daq_id==0 || daq_id == 30 || daq_id == 19){
      for(int i=0; i<window_size/2; i++){
        freq = 40+(float)((float)(i*40.0)/(float)(window_size/2));
        Amp[i] *= ( 12.5 / 1000.0 );
        Amp[i] *= ( FieldStrengthCorrection_RefAnt(freq) / sqrt(AntennaGain(freq,zenith)) );
      }
    }else{
    if( daq_id == 50 ){
#warning daq_id 50 is not definded for calibration
    }
    else{
      cerr << "CalibratedFrequencyDomain: daq id = " << daq_id << " not valid" << endl;
      cerr << "no calibration is performed" << endl;
    }}
  }
  else{
    TraceFFT(window_size, trace, Amp, Phase, RawFFT, data_window);
      
    for(int i=0; i<window_size/2; i++){
      freq = 40+(float)((float)(i*40.0)/(float)(window_size/2));
      Amp[i] *= ( 12.5 / 1000.0 );
      Amp[i] /= ( FieldStrengthElectronicsGain(freq) * sqrt(AntennaGain(freq,zenith)) );
      Amp[i] *= ( sqrt(120.0 * Pi() / 50.0) * 1.0e6 / 4096.0);
      
      if(daq_id == 17 || daq_id==0 || daq_id == 30 || daq_id == 19){
        Amp[i] *= ( pow(10, CableAttenuationRG058(4, freq) / 20.0) * pow(10, CableAttenuationRG213(100, freq) / 20.0) );
      }else{
      if(daq_id == 42){
        Amp[i] *= ( pow(10, CableAttenuationRG058(4, freq) / 20.0) * pow(10, CableAttenuationRG213(160, freq) / 20.0) );
      }else{
      if(daq_id == 50){
#warning daq_id 50 is not definded for calibration
      }      
      else{
        cerr << "CalibratedFrequencyDomain: daq id = " << daq_id << " not valid" << endl;
	cerr << "no calibration is performed" << endl;
      }}}
      
    }
  }
}

/**********************************************************************************************/
void PowerTrace(unsigned int window_size, float *trace){
  
  SubtractPedestal(window_size, trace);
  
  for(unsigned int j=0; j<window_size; j++){
   trace[j] = pow(trace[j],2) / (120*Pi());  // P = E^2 / (120*Pi)
  }
}

/**********************************************************************************************/

float AmpRefSource_FieldStrength(float frequency){
  //derived from RefAnt_plots.C
  int entries = 41;
  double *freq = new double [entries];
  double *FS_LPDA = new double [entries];

  freq[ 0] = 40.0;	   FS_LPDA[ 0] = 21.27568;
  freq[ 1] = 41.0;	   FS_LPDA[ 1] = 23.06128;
  freq[ 2] = 42.0;	   FS_LPDA[ 2] = 24.99674;
  freq[ 3] = 43.0;	   FS_LPDA[ 3] = 27.09464;
  freq[ 4] = 44.0;	   FS_LPDA[ 4] = 29.36861;
  freq[ 5] = 45.0;	   FS_LPDA[ 5] = 31.83343;
  freq[ 6] = 46.0;	   FS_LPDA[ 6] = 34.50511;
  freq[ 7] = 47.0;	   FS_LPDA[ 7] = 37.40102;
  freq[ 8] = 48.0;	   FS_LPDA[ 8] = 40.53997;
  freq[ 9] = 49.0;	   FS_LPDA[ 9] = 43.94237;
  freq[10] = 50.0;	   FS_LPDA[10] = 47.63031;
  freq[11] = 51.0;	   FS_LPDA[11] = 51.62778;
  freq[12] = 52.0;	   FS_LPDA[12] = 55.32016;
  freq[13] = 53.0;	   FS_LPDA[13] = 59.96301;
  freq[14] = 54.0;	   FS_LPDA[14] = 64.25153;
  freq[15] = 55.0;	   FS_LPDA[15] = 69.64396;
  freq[16] = 56.0;	   FS_LPDA[16] = 74.62485;
  freq[17] = 57.0;	   FS_LPDA[17] = 80.88788;
  freq[18] = 58.0;	   FS_LPDA[18] = 86.67292;
  freq[19] = 59.0;	   FS_LPDA[19] = 93.94712;
  freq[20] = 60.0;	   FS_LPDA[20] = 100.66615;
  freq[21] = 61.0;	   FS_LPDA[21] = 105.41040;
  freq[22] = 62.0;	   FS_LPDA[22] = 111.65636;
  freq[23] = 63.0;	   FS_LPDA[23] = 116.91856;
  freq[24] = 64.0;	   FS_LPDA[24] = 122.42876;
  freq[25] = 65.0;	   FS_LPDA[25] = 128.19865;
  freq[26] = 66.0;	   FS_LPDA[26] = 134.24047;
  freq[27] = 67.0;	   FS_LPDA[27] = 142.19471;
  freq[28] = 68.0;	   FS_LPDA[28] = 148.89614;
  freq[29] = 69.0;	   FS_LPDA[29] = 155.91340;
  freq[30] = 70.0;	   FS_LPDA[30] = 163.26137;
  freq[31] = 71.0;	   FS_LPDA[31] = 168.99873;
  freq[32] = 72.0;	   FS_LPDA[32] = 174.93771;
  freq[33] = 73.0;	   FS_LPDA[33] = 181.08541;
  freq[34] = 74.0;	   FS_LPDA[34] = 185.30343;
  freq[35] = 75.0;	   FS_LPDA[35] = 191.81539;
  freq[36] = 76.0;	   FS_LPDA[36] = 198.55620;
  freq[37] = 77.0;	   FS_LPDA[37] = 205.53389;
  freq[38] = 78.0;	   FS_LPDA[38] = 210.32139;
  freq[39] = 79.0;	   FS_LPDA[39] = 217.71254;
  freq[40] = 80.0;	   FS_LPDA[40] = 225.36343;
  
  if(frequency>80.0){
   cerr << "AmpRefSource_FieldStrength: upper frequency limit reached." << endl;  
   //exit(1);
  }
  if(frequency<40.0){
   cerr << "AmpRefSource_FieldStrength: lower frequency limit reached." << endl;  
   //exit(1);
  }
  
  //linear interpolation
  float amp_value=0;
  for(int i=0; i<entries; i++){
    if(frequency<freq[i]){
      amp_value = (( (FS_LPDA[i-1]-FS_LPDA[i]) / (freq[i-1]-freq[i]))*(frequency-freq[i])+FS_LPDA[i]);
      break;
    }
  }
  if(amp_value == 0 && frequency == 80) amp_value = FS_LPDA[40];
  
  //clean up
  delete[] freq;
  delete[] FS_LPDA;

  return amp_value;
}

/**********************************************************************************************/

float AntennaGain(float frequency, float zenith){

  float antenna_gain=0;

  antenna_gain = GainDirectivity(frequency,zenith) * pow((C()/(frequency * 1.0e6)),2) / ( 4.0 * Pi() );
  
  return antenna_gain;

}

/**********************************************************************************************/

float GainDirectivity(float frequency, float zenith){
  //derived from LPDA_Directivity_plot.C
  
#warning frequency dependence of gain directivity relative to isotropic radiator not implemented
#warning assumed 5.5dBi for all frequencies at zenith equal to zero degree

  int entries = 37;
  double *ZENITH = new double [entries];
  double *gain_fs_036 = new double [entries];
  double *gain_fs_050 = new double [entries];
  double *gain_fs_066 = new double [entries];
  double *gain_fs_100 = new double [entries];

  ZENITH[ 0] = 0.0;     gain_fs_036[ 0] = 1.88365;      gain_fs_050[ 0] = 1.88365;      gain_fs_066[ 0] = 1.88365;      gain_fs_100[ 0] = 1.88365;
  ZENITH[ 1] = 10.0;    gain_fs_036[ 1] = 1.90546;      gain_fs_050[ 1] = 1.88365;      gain_fs_066[ 1] = 1.79887;      gain_fs_100[ 1] = 1.81970;
  ZENITH[ 2] = 20.0;    gain_fs_036[ 2] = 1.86209;      gain_fs_050[ 2] = 1.88365;      gain_fs_066[ 2] = 1.75792;      gain_fs_100[ 2] = 1.75792;
  ZENITH[ 3] = 30.0;    gain_fs_036[ 3] = 1.77828;      gain_fs_050[ 3] = 1.75792;      gain_fs_066[ 3] = 1.65959;      gain_fs_100[ 3] = 1.69824;
  ZENITH[ 4] = 40.0;    gain_fs_036[ 4] = 1.60325;      gain_fs_050[ 4] = 1.56675;      gain_fs_066[ 4] = 1.42889;      gain_fs_100[ 4] = 1.75792;
  ZENITH[ 5] = 50.0;    gain_fs_036[ 5] = 1.28825;      gain_fs_050[ 5] = 1.28825;      gain_fs_066[ 5] = 1.18850;      gain_fs_100[ 5] = 1.60325;
  ZENITH[ 6] = 60.0;    gain_fs_036[ 6] = 1.00000;      gain_fs_050[ 6] = 1.00000;      gain_fs_066[ 6] = 0.97724;      gain_fs_100[ 6] = 1.30317;
  ZENITH[ 7] = 70.0;    gain_fs_036[ 7] = 0.70795;      gain_fs_050[ 7] = 0.65313;      gain_fs_066[ 7] = 0.68391;      gain_fs_100[ 7] = 0.89125;
  ZENITH[ 8] = 80.0;    gain_fs_036[ 8] = 0.44668;      gain_fs_050[ 8] = 0.18408;      gain_fs_066[ 8] = 0.44157;      gain_fs_100[ 8] = 0.35075;
  ZENITH[ 9] = 90.0;    gain_fs_036[ 9] = 0.07943;      gain_fs_050[ 9] = 0.05754;      gain_fs_066[ 9] = 0.24266;      gain_fs_100[ 9] = 0.03428;
  ZENITH[10] = 100.0;   gain_fs_036[10] = 0.07079;      gain_fs_050[10] = 0.09333;      gain_fs_066[10] = 0.06095;      gain_fs_100[10] = 0.06383;
  ZENITH[11] = 110.0;   gain_fs_036[11] = 0.11220;      gain_fs_050[11] = 0.10233;      gain_fs_066[11] = 0.09550;      gain_fs_100[11] = 0.17989;
  ZENITH[12] = 120.0;   gain_fs_036[12] = 0.17783;      gain_fs_050[12] = 0.12882;      gain_fs_066[12] = 0.16982;      gain_fs_100[12] = 0.26915;
  ZENITH[13] = 130.0;   gain_fs_036[13] = 0.28184;      gain_fs_050[13] = 0.16218;      gain_fs_066[13] = 0.22387;      gain_fs_100[13] = 0.33113;
  ZENITH[14] = 140.0;   gain_fs_036[14] = 0.37584;      gain_fs_050[14] = 0.20417;      gain_fs_066[14] = 0.17989;      gain_fs_100[14] = 0.46774;
  ZENITH[15] = 150.0;   gain_fs_036[15] = 0.44668;      gain_fs_050[15] = 0.25704;      gain_fs_066[15] = 0.16032;      gain_fs_100[15] = 0.46238;
  ZENITH[16] = 160.0;   gain_fs_036[16] = 0.50119;      gain_fs_050[16] = 0.25704;      gain_fs_066[16] = 0.23988;      gain_fs_100[16] = 0.32734;
  ZENITH[17] = 170.0;   gain_fs_036[17] = 0.56234;      gain_fs_050[17] = 0.27227;      gain_fs_066[17] = 0.26303;      gain_fs_100[17] = 0.12445;
  ZENITH[18] = 180.0;   gain_fs_036[18] = 0.50119;      gain_fs_050[18] = 0.28184;      gain_fs_066[18] = 0.23174;      gain_fs_100[18] = 0.03199;
  ZENITH[19] = 190.0;   gain_fs_036[19] = 0.56234;      gain_fs_050[19] = 0.27227;      gain_fs_066[19] = 0.26303;      gain_fs_100[19] = 0.12445;
  ZENITH[20] = 200.0;   gain_fs_036[20] = 0.50119;      gain_fs_050[20] = 0.25704;      gain_fs_066[20] = 0.23988;      gain_fs_100[20] = 0.32734;
  ZENITH[21] = 210.0;   gain_fs_036[21] = 0.44668;      gain_fs_050[21] = 0.25704;      gain_fs_066[21] = 0.16032;      gain_fs_100[21] = 0.46238;
  ZENITH[22] = 220.0;   gain_fs_036[22] = 0.37584;      gain_fs_050[22] = 0.20417;      gain_fs_066[22] = 0.17989;      gain_fs_100[22] = 0.46774;
  ZENITH[23] = 230.0;   gain_fs_036[23] = 0.28184;      gain_fs_050[23] = 0.16218;      gain_fs_066[23] = 0.22387;      gain_fs_100[23] = 0.33113;
  ZENITH[24] = 240.0;   gain_fs_036[24] = 0.17783;      gain_fs_050[24] = 0.12882;      gain_fs_066[24] = 0.16982;      gain_fs_100[24] = 0.26915;
  ZENITH[25] = 250.0;   gain_fs_036[25] = 0.11220;      gain_fs_050[25] = 0.10233;      gain_fs_066[25] = 0.09550;      gain_fs_100[25] = 0.17989;
  ZENITH[26] = 260.0;   gain_fs_036[26] = 0.07079;      gain_fs_050[26] = 0.09333;      gain_fs_066[26] = 0.06095;      gain_fs_100[26] = 0.06383;
  ZENITH[27] = 270.0;   gain_fs_036[27] = 0.07943;      gain_fs_050[27] = 0.05754;      gain_fs_066[27] = 0.24266;      gain_fs_100[27] = 0.03428;
  ZENITH[28] = 280.0;   gain_fs_036[28] = 0.44668;      gain_fs_050[28] = 0.18408;      gain_fs_066[28] = 0.44157;      gain_fs_100[28] = 0.35075;
  ZENITH[29] = 290.0;   gain_fs_036[29] = 0.70795;      gain_fs_050[29] = 0.65313;      gain_fs_066[29] = 0.68391;      gain_fs_100[29] = 0.89125;
  ZENITH[30] = 300.0;   gain_fs_036[30] = 1.00000;      gain_fs_050[30] = 1.00000;      gain_fs_066[30] = 0.97724;      gain_fs_100[30] = 1.30317;
  ZENITH[31] = 310.0;   gain_fs_036[31] = 1.28825;      gain_fs_050[31] = 1.28825;      gain_fs_066[31] = 1.18850;      gain_fs_100[31] = 1.60325;
  ZENITH[32] = 320.0;   gain_fs_036[32] = 1.60325;      gain_fs_050[32] = 1.56675;      gain_fs_066[32] = 1.42889;      gain_fs_100[32] = 1.75792;
  ZENITH[33] = 330.0;   gain_fs_036[33] = 1.77828;      gain_fs_050[33] = 1.75792;      gain_fs_066[33] = 1.65959;      gain_fs_100[33] = 1.69824;
  ZENITH[34] = 340.0;   gain_fs_036[34] = 1.86209;      gain_fs_050[34] = 1.88365;      gain_fs_066[34] = 1.75792;      gain_fs_100[34] = 1.75792;
  ZENITH[35] = 350.0;   gain_fs_036[35] = 1.90546;      gain_fs_050[35] = 1.88365;      gain_fs_066[35] = 1.79887;      gain_fs_100[35] = 1.81970;
  ZENITH[36] = 0.0;     gain_fs_036[36] = 1.88365;      gain_fs_050[36] = 1.88365;      gain_fs_066[36] = 1.88365;      gain_fs_100[36] = 1.88365;
  
  if( !(zenith>=0. && zenith<=90.) ){
    cerr << "GainDirectivity: zenith angle is out of range! " << endl;
//    exit(1);
  }
  if(frequency>80.0){
   cerr << "GainDirectivity: upper frequency limit reached." << endl;  
//   exit(1);
  }
  if(frequency<40.0){
   cerr << "GainDirectivity: lower frequency limit reached." << endl;  
//   exit(1);
  }


  //linear interpolation
  float gain_value=0;
  int zp=0; // position in the zenith array
  for(int i=0; i<entries; i++){
    if(zenith<ZENITH[i]){
      zp=i;
      break;
    }
  }
  
  float inter_lower=0;
  float inter_upper=0;
  
  if(frequency>36 && frequency<=50){
    inter_lower = (( (gain_fs_036[zp-1]-gain_fs_036[zp]) / (ZENITH[zp-1]-ZENITH[zp]))*(zenith-ZENITH[zp])+gain_fs_036[zp]);
    inter_upper = (( (gain_fs_050[zp-1]-gain_fs_050[zp]) / (ZENITH[zp-1]-ZENITH[zp]))*(zenith-ZENITH[zp])+gain_fs_050[zp]);
    gain_value  = (( (inter_lower-inter_upper) / (36-50))*(frequency-50)+inter_upper);
  }
  else{
  if(frequency>50 && frequency<=66){
    inter_lower = (( (gain_fs_050[zp-1]-gain_fs_050[zp]) / (ZENITH[zp-1]-ZENITH[zp]))*(zenith-ZENITH[zp])+gain_fs_050[zp]);
    inter_upper = (( (gain_fs_066[zp-1]-gain_fs_066[zp]) / (ZENITH[zp-1]-ZENITH[zp]))*(zenith-ZENITH[zp])+gain_fs_066[zp]);
    gain_value  = (( (inter_lower-inter_upper) / (50-66))*(frequency-66)+inter_upper);
  }
  else{
  if(frequency>66 && frequency<100){
    inter_lower = (( (gain_fs_066[zp-1]-gain_fs_066[zp]) / (ZENITH[zp-1]-ZENITH[zp]))*(zenith-ZENITH[zp])+gain_fs_066[zp]);
    inter_upper = (( (gain_fs_100[zp-1]-gain_fs_100[zp]) / (ZENITH[zp-1]-ZENITH[zp]))*(zenith-ZENITH[zp])+gain_fs_100[zp]);
    gain_value  = (( (inter_lower-inter_upper) / (66-100))*(frequency-100)+inter_upper);
  }}}
  
  //clean up
  delete[] ZENITH;
  delete[] gain_fs_036;
  delete[] gain_fs_050;
  delete[] gain_fs_066;
  delete[] gain_fs_100;
  
  return gain_value;

}

/**********************************************************************************************/

float FieldStrengthCorrection_RefAnt(float frequency){
  //derived from RefAnt_CorrValue.C
  int entries = 39;
  float *freq = new float [entries];
  float *CorrValue = new float [entries];
  
  freq[ 0] = 41.0;      CorrValue[ 0] = 1.53692;
  freq[ 1] = 42.0;      CorrValue[ 1] = 1.03957;
  freq[ 2] = 43.0;      CorrValue[ 2] = 0.95818;
  freq[ 3] = 44.0;      CorrValue[ 3] = 1.30217;
  freq[ 4] = 45.0;      CorrValue[ 4] = 2.21560;
  freq[ 5] = 46.0;      CorrValue[ 5] = 1.84757;
  freq[ 6] = 47.0;      CorrValue[ 6] = 1.44022;
  freq[ 7] = 48.0;      CorrValue[ 7] = 1.24574;
  freq[ 8] = 49.0;      CorrValue[ 8] = 1.39997;
  freq[ 9] = 50.0;      CorrValue[ 9] = 2.30433;
  freq[10] = 51.0;      CorrValue[10] = 1.80903;
  freq[11] = 52.0;      CorrValue[11] = 1.30351;
  freq[12] = 53.0;      CorrValue[12] = 1.11470;
  freq[13] = 54.0;      CorrValue[13] = 1.10500;
  freq[14] = 55.0;      CorrValue[14] = 1.53296;
  freq[15] = 56.0;      CorrValue[15] = 1.18365;
  freq[16] = 57.0;      CorrValue[16] = 1.03803;
  freq[17] = 58.0;      CorrValue[17] = 1.11576;
  freq[18] = 59.0;      CorrValue[18] = 1.88772;
  freq[19] = 60.0;      CorrValue[19] = 2.30411;
  freq[20] = 61.0;      CorrValue[20] = 1.92355;
  freq[21] = 62.0;      CorrValue[21] = 1.40573;
  freq[22] = 63.0;      CorrValue[22] = 1.50862;
  freq[23] = 64.0;      CorrValue[23] = 1.75997;
  freq[24] = 65.0;      CorrValue[24] = 2.82232;
  freq[25] = 66.0;      CorrValue[25] = 2.04168;
  freq[26] = 67.0;      CorrValue[26] = 1.44317;
  freq[27] = 68.0;      CorrValue[27] = 1.26854;
  freq[28] = 69.0;      CorrValue[28] = 1.45919;
  freq[29] = 70.0;      CorrValue[29] = 2.19622;
  freq[30] = 71.0;      CorrValue[30] = 2.35357;
  freq[31] = 72.0;      CorrValue[31] = 2.17891;
  freq[32] = 73.0;      CorrValue[32] = 2.24220;
  freq[33] = 74.0;      CorrValue[33] = 2.47902;
  freq[34] = 75.0;      CorrValue[34] = 3.16692;
  freq[35] = 76.0;      CorrValue[35] = 2.42735;
  freq[36] = 77.0;      CorrValue[36] = 1.97560;
  freq[37] = 78.0;      CorrValue[37] = 1.61582;
  freq[38] = 79.0;      CorrValue[38] = 1.49325;
  
  if(frequency>80.0){
   cerr << "FieldStrengthCorrection: upper frequency limit ?" << endl;  
   //exit(1);
  }
  if(frequency<40.0){
   cerr << "FieldStrengthCorrection: lower frequency limit ?" << endl;  
   //exit(1);
  }
  
  //linear interpolation
  float correction = 0;
  if(frequency >= 41.0 && frequency <= 79.0)
   for(int i=0; i<entries; i++){
     if(frequency<freq[i]){
       correction = (( (CorrValue[i-1]-CorrValue[i]) / (freq[i-1]-freq[i]))*(frequency-freq[i])+CorrValue[i]);
       break;
     }
   }
  if(correction == 0 && frequency <= 41) correction = (( (CorrValue[0]-CorrValue[1]) / (freq[0]-freq[1]))*(frequency-freq[1])+CorrValue[1]);
  if(correction == 0 && frequency >= 79) correction = (( (CorrValue[37]-CorrValue[38]) / (freq[37]-freq[38]))*(frequency-freq[38])+CorrValue[38]);

  //clean up
  delete[] freq;
  delete[] CorrValue;
  
  return correction;
}

/**********************************************************************************************/

float CableAttenuationRG213(float length, float frequency){
  float attenuation = 0;
  
  if(frequency>80.0){
   cerr << "CableAttenuationRG213: upper frequency limit ?" << endl;  
   //exit(1);
  }
  if(frequency<40.0){
   cerr << "CableAttenuationRG213: lower frequency limit ?" << endl;  
   //exit(1);
  }

  int entries = 5;
  float *freq = new float [entries];
  float *atten = new float [entries];

  freq[0] =  10;	atten[0] = 1.8 / 100.0 * length;
  freq[1] =  20;	atten[1] = 2.7 / 100.0 * length;
  freq[2] =  50;	atten[2] = 4.4 / 100.0 * length;
  freq[3] = 100;	atten[3] = 6.4 / 100.0 * length;
  freq[4] = 200;	atten[4] = 9.5 / 100.0 * length;
    
  //linear interpolation
  for(int i=0; i<entries; i++){
    if(frequency<freq[i]){
      attenuation = (( (atten[i-1]-atten[i]) / (freq[i-1]-freq[i]))*(frequency-freq[i])+atten[i]);
      break;
    }
  }

  //clean up
  delete[] freq;
  delete[] atten;

  return attenuation;
}

/**********************************************************************************************/

float CableAttenuationRG058(float length, float frequency){
  float attenuation = 0;
  
  if(frequency>80.0){
   cerr << "CableAttenuationRG058: upper frequency limit ?" << endl;  
   //exit(1);
  }
  if(frequency<40.0){
   cerr << "CableAttenuationRG058: lower frequency limit ?" << endl;  
   //exit(1);
  }

  int entries = 5;
  float *freq = new float [entries];
  float *atten = new float [entries];

  freq[0] =  10;	atten[0] =  4.5 / 100.0 * length;
  freq[1] =  20;	atten[1] =  6.5 / 100.0 * length;
  freq[2] =  50;	atten[2] = 10.4 / 100.0 * length;
  freq[3] = 100;	atten[3] = 15.1 / 100.0 * length;
  freq[4] = 200;	atten[4] = 21.9 / 100.0 * length;
    
  //linear interpolation
  for(int i=0; i<entries; i++){
    if(frequency<freq[i]){
      attenuation = (( (atten[i-1]-atten[i]) / (freq[i-1]-freq[i]))*(frequency-freq[i])+atten[i]);
      break;
    }
  }
  
  //clean up
  delete[] freq;
  delete[] atten;

  return attenuation;
}

/**********************************************************************************************/

float FieldStrengthElectronicsGain(float frequency){
  //derived from Electronics_CorrValue.C
  int entries = 655;
  float *freq = new float [entries];
  float *gain_el_fs = new float [entries];
  
  freq[  0] = 15.00;     gain_el_fs[  0] = 0.00031;
  freq[  1] = 15.20;     gain_el_fs[  1] = 0.00035;
  freq[  2] = 15.40;     gain_el_fs[  2] = 0.00038;
  freq[  3] = 15.60;     gain_el_fs[  3] = 0.00042;
  freq[  4] = 15.80;     gain_el_fs[  4] = 0.00050;
  freq[  5] = 16.00;     gain_el_fs[  5] = 0.00058;
  freq[  6] = 16.20;     gain_el_fs[  6] = 0.00068;
  freq[  7] = 16.40;     gain_el_fs[  7] = 0.00070;
  freq[  8] = 16.60;     gain_el_fs[  8] = 0.00065;
  freq[  9] = 16.80;     gain_el_fs[  9] = 0.00059;
  freq[ 10] = 17.00;     gain_el_fs[ 10] = 0.00054;
  freq[ 11] = 17.20;     gain_el_fs[ 11] = 0.00047;
  freq[ 12] = 17.40;     gain_el_fs[ 12] = 0.00040;
  freq[ 13] = 17.60;     gain_el_fs[ 13] = 0.00034;
  freq[ 14] = 17.80;     gain_el_fs[ 14] = 0.00033;
  freq[ 15] = 18.00;     gain_el_fs[ 15] = 0.00036;
  freq[ 16] = 18.20;     gain_el_fs[ 16] = 0.00039;
  freq[ 17] = 18.40;     gain_el_fs[ 17] = 0.00042;
  freq[ 18] = 18.60;     gain_el_fs[ 18] = 0.00039;
  freq[ 19] = 18.80;     gain_el_fs[ 19] = 0.00037;
  freq[ 20] = 19.00;     gain_el_fs[ 20] = 0.00034;
  freq[ 21] = 19.20;     gain_el_fs[ 21] = 0.00036;
  freq[ 22] = 19.40;     gain_el_fs[ 22] = 0.00044;
  freq[ 23] = 19.60;     gain_el_fs[ 23] = 0.00053;
  freq[ 24] = 19.80;     gain_el_fs[ 24] = 0.00062;
  freq[ 25] = 20.00;     gain_el_fs[ 25] = 0.00069;
  freq[ 26] = 20.20;     gain_el_fs[ 26] = 0.00075;
  freq[ 27] = 20.40;     gain_el_fs[ 27] = 0.00083;
  freq[ 28] = 20.60;     gain_el_fs[ 28] = 0.00082;
  freq[ 29] = 20.80;     gain_el_fs[ 29] = 0.00073;
  freq[ 30] = 21.00;     gain_el_fs[ 30] = 0.00065;
  freq[ 31] = 21.20;     gain_el_fs[ 31] = 0.00057;
  freq[ 32] = 21.40;     gain_el_fs[ 32] = 0.00063;
  freq[ 33] = 21.60;     gain_el_fs[ 33] = 0.00069;
  freq[ 34] = 21.80;     gain_el_fs[ 34] = 0.00075;
  freq[ 35] = 22.00;     gain_el_fs[ 35] = 0.00084;
  freq[ 36] = 22.20;     gain_el_fs[ 36] = 0.00095;
  freq[ 37] = 22.40;     gain_el_fs[ 37] = 0.00108;
  freq[ 38] = 22.60;     gain_el_fs[ 38] = 0.00122;
  freq[ 39] = 22.80;     gain_el_fs[ 39] = 0.00121;
  freq[ 40] = 23.00;     gain_el_fs[ 40] = 0.00119;
  freq[ 41] = 23.20;     gain_el_fs[ 41] = 0.00117;
  freq[ 42] = 23.40;     gain_el_fs[ 42] = 0.00110;
  freq[ 43] = 23.60;     gain_el_fs[ 43] = 0.00099;
  freq[ 44] = 23.80;     gain_el_fs[ 44] = 0.00088;
  freq[ 45] = 24.00;     gain_el_fs[ 45] = 0.00079;
  freq[ 46] = 24.20;     gain_el_fs[ 46] = 0.00109;
  freq[ 47] = 24.40;     gain_el_fs[ 47] = 0.00147;
  freq[ 48] = 24.60;     gain_el_fs[ 48] = 0.00198;
  freq[ 49] = 24.80;     gain_el_fs[ 49] = 0.00254;
  freq[ 50] = 25.00;     gain_el_fs[ 50] = 0.00310;
  freq[ 51] = 25.20;     gain_el_fs[ 51] = 0.00379;
  freq[ 52] = 25.40;     gain_el_fs[ 52] = 0.00469;
  freq[ 53] = 25.60;     gain_el_fs[ 53] = 0.00549;
  freq[ 54] = 25.80;     gain_el_fs[ 54] = 0.00643;
  freq[ 55] = 26.00;     gain_el_fs[ 55] = 0.00753;
  freq[ 56] = 26.20;     gain_el_fs[ 56] = 0.00878;
  freq[ 57] = 26.40;     gain_el_fs[ 57] = 0.01014;
  freq[ 58] = 26.60;     gain_el_fs[ 58] = 0.01171;
  freq[ 59] = 26.80;     gain_el_fs[ 59] = 0.01353;
  freq[ 60] = 27.00;     gain_el_fs[ 60] = 0.01634;
  freq[ 61] = 27.20;     gain_el_fs[ 61] = 0.01987;
  freq[ 62] = 27.40;     gain_el_fs[ 62] = 0.02433;
  freq[ 63] = 27.60;     gain_el_fs[ 63] = 0.02940;
  freq[ 64] = 27.80;     gain_el_fs[ 64] = 0.03505;
  freq[ 65] = 28.00;     gain_el_fs[ 65] = 0.04178;
  freq[ 66] = 28.20;     gain_el_fs[ 66] = 0.04987;
  freq[ 67] = 28.40;     gain_el_fs[ 67] = 0.06030;
  freq[ 68] = 28.60;     gain_el_fs[ 68] = 0.07293;
  freq[ 69] = 28.80;     gain_el_fs[ 69] = 0.08819;
  freq[ 70] = 29.00;     gain_el_fs[ 70] = 0.10637;
  freq[ 71] = 29.20;     gain_el_fs[ 71] = 0.12761;
  freq[ 72] = 29.40;     gain_el_fs[ 72] = 0.15310;
  freq[ 73] = 29.60;     gain_el_fs[ 73] = 0.18368;
  freq[ 74] = 29.80;     gain_el_fs[ 74] = 0.22255;
  freq[ 75] = 30.00;     gain_el_fs[ 75] = 0.26960;
  freq[ 76] = 30.20;     gain_el_fs[ 76] = 0.32641;
  freq[ 77] = 30.40;     gain_el_fs[ 77] = 0.39766;
  freq[ 78] = 30.60;     gain_el_fs[ 78] = 0.48749;
  freq[ 79] = 30.80;     gain_el_fs[ 79] = 0.59761;
  freq[ 80] = 31.00;     gain_el_fs[ 80] = 0.73522;
  freq[ 81] = 31.20;     gain_el_fs[ 81] = 0.91288;
  freq[ 82] = 31.40;     gain_el_fs[ 82] = 1.13345;
  freq[ 83] = 31.60;     gain_el_fs[ 83] = 1.40732;
  freq[ 84] = 31.80;     gain_el_fs[ 84] = 1.74637;
  freq[ 85] = 32.00;     gain_el_fs[ 85] = 2.17406;
  freq[ 86] = 32.20;     gain_el_fs[ 86] = 2.70992;
  freq[ 87] = 32.40;     gain_el_fs[ 87] = 3.37785;
  freq[ 88] = 32.60;     gain_el_fs[ 88] = 4.08498;
  freq[ 89] = 32.80;     gain_el_fs[ 89] = 4.94017;
  freq[ 90] = 33.00;     gain_el_fs[ 90] = 6.04067;
  freq[ 91] = 33.20;     gain_el_fs[ 91] = 7.20568;
  freq[ 92] = 33.40;     gain_el_fs[ 92] = 8.38517;
  freq[ 93] = 33.60;     gain_el_fs[ 93] = 9.75771;
  freq[ 94] = 33.80;     gain_el_fs[ 94] = 11.33901;
  freq[ 95] = 34.00;     gain_el_fs[ 95] = 12.76916;
  freq[ 96] = 34.20;     gain_el_fs[ 96] = 14.37969;
  freq[ 97] = 34.40;     gain_el_fs[ 97] = 16.19334;
  freq[ 98] = 34.60;     gain_el_fs[ 98] = 18.26338;
  freq[ 99] = 34.80;     gain_el_fs[ 99] = 20.73771;
  freq[100] = 35.00;     gain_el_fs[100] = 23.67099;
  freq[101] = 35.20;     gain_el_fs[101] = 27.01917;
  freq[102] = 35.40;     gain_el_fs[102] = 31.30997;
  freq[103] = 35.60;     gain_el_fs[103] = 36.28207;
  freq[104] = 35.80;     gain_el_fs[104] = 41.86797;
  freq[105] = 36.00;     gain_el_fs[105] = 48.62757;
  freq[106] = 36.20;     gain_el_fs[106] = 56.92477;
  freq[107] = 36.40;     gain_el_fs[107] = 66.63764;
  freq[108] = 36.60;     gain_el_fs[108] = 78.00761;
  freq[109] = 36.80;     gain_el_fs[109] = 90.96600;
  freq[110] = 37.00;     gain_el_fs[110] = 106.07709;
  freq[111] = 37.20;     gain_el_fs[111] = 123.69830;
  freq[112] = 37.40;     gain_el_fs[112] = 144.00699;
  freq[113] = 37.60;     gain_el_fs[113] = 167.36236;
  freq[114] = 37.80;     gain_el_fs[114] = 194.47678;
  freq[115] = 38.00;     gain_el_fs[115] = 225.98382;
  freq[116] = 38.20;     gain_el_fs[116] = 254.86706;
  freq[117] = 38.40;     gain_el_fs[117] = 287.44215;
  freq[118] = 38.60;     gain_el_fs[118] = 323.32193;
  freq[119] = 38.80;     gain_el_fs[119] = 355.26698;
  freq[120] = 39.00;     gain_el_fs[120] = 382.34889;
  freq[121] = 39.20;     gain_el_fs[121] = 411.49506;
  freq[122] = 39.40;     gain_el_fs[122] = 442.86302;
  freq[123] = 39.60;     gain_el_fs[123] = 467.45634;
  freq[124] = 39.80;     gain_el_fs[124] = 493.30251;
  freq[125] = 40.00;     gain_el_fs[125] = 520.57797;
  freq[126] = 40.20;     gain_el_fs[126] = 548.90331;
  freq[127] = 40.40;     gain_el_fs[127] = 578.28762;
  freq[128] = 40.60;     gain_el_fs[128] = 606.42703;
  freq[129] = 40.80;     gain_el_fs[129] = 635.93625;
  freq[130] = 41.00;     gain_el_fs[130] = 668.98176;
  freq[131] = 41.20;     gain_el_fs[131] = 703.74474;
  freq[132] = 41.40;     gain_el_fs[132] = 739.35497;
  freq[133] = 41.60;     gain_el_fs[133] = 771.45975;
  freq[134] = 41.80;     gain_el_fs[134] = 802.57479;
  freq[135] = 42.00;     gain_el_fs[135] = 834.94441;
  freq[136] = 42.20;     gain_el_fs[136] = 868.61996;
  freq[137] = 42.40;     gain_el_fs[137] = 895.24100;
  freq[138] = 42.60;     gain_el_fs[138] = 924.36884;
  freq[139] = 42.80;     gain_el_fs[139] = 954.44482;
  freq[140] = 43.00;     gain_el_fs[140] = 978.26171;
  freq[141] = 43.20;     gain_el_fs[141] = 995.30958;
  freq[142] = 43.40;     gain_el_fs[142] = 1012.95166;
  freq[143] = 43.60;     gain_el_fs[143] = 1031.00652;
  freq[144] = 43.80;     gain_el_fs[144] = 1032.94770;
  freq[145] = 44.00;     gain_el_fs[145] = 1034.89252;
  freq[146] = 44.20;     gain_el_fs[146] = 1036.84055;
  freq[147] = 44.40;     gain_el_fs[147] = 1035.98528;
  freq[148] = 44.60;     gain_el_fs[148] = 1030.21081;
  freq[149] = 44.80;     gain_el_fs[149] = 1024.46852;
  freq[150] = 45.00;     gain_el_fs[150] = 1018.75869;
  freq[151] = 45.20;     gain_el_fs[151] = 1006.28550;
  freq[152] = 45.40;     gain_el_fs[152] = 992.02217;
  freq[153] = 45.60;     gain_el_fs[153] = 977.96144;
  freq[154] = 45.80;     gain_el_fs[154] = 963.44013;
  freq[155] = 46.00;     gain_el_fs[155] = 948.48479;
  freq[156] = 46.20;     gain_el_fs[156] = 934.36218;
  freq[157] = 46.40;     gain_el_fs[157] = 921.04226;
  freq[158] = 46.60;     gain_el_fs[158] = 909.90291;
  freq[159] = 46.80;     gain_el_fs[159] = 898.89790;
  freq[160] = 47.00;     gain_el_fs[160] = 888.02638;
  freq[161] = 47.20;     gain_el_fs[161] = 878.13284;
  freq[162] = 47.40;     gain_el_fs[162] = 869.98521;
  freq[163] = 47.60;     gain_el_fs[163] = 861.91356;
  freq[164] = 47.80;     gain_el_fs[164] = 853.91642;
  freq[165] = 48.00;     gain_el_fs[165] = 850.01870;
  freq[166] = 48.20;     gain_el_fs[166] = 847.90725;
  freq[167] = 48.40;     gain_el_fs[167] = 845.80105;
  freq[168] = 48.60;     gain_el_fs[168] = 845.66696;
  freq[169] = 48.80;     gain_el_fs[169] = 847.50517;
  freq[170] = 49.00;     gain_el_fs[170] = 848.68478;
  freq[171] = 49.20;     gain_el_fs[171] = 847.87821;
  freq[172] = 49.40;     gain_el_fs[172] = 850.80938;
  freq[173] = 49.60;     gain_el_fs[173] = 853.75030;
  freq[174] = 49.80;     gain_el_fs[174] = 856.70177;
  freq[175] = 50.00;     gain_el_fs[175] = 860.87397;
  freq[176] = 50.20;     gain_el_fs[176] = 866.28461;
  freq[177] = 50.40;     gain_el_fs[177] = 871.72964;
  freq[178] = 50.60;     gain_el_fs[178] = 877.20890;
  freq[179] = 50.80;     gain_el_fs[179] = 885.37168;
  freq[180] = 51.00;     gain_el_fs[180] = 896.04265;
  freq[181] = 51.20;     gain_el_fs[181] = 907.66424;
  freq[182] = 51.40;     gain_el_fs[182] = 919.28837;
  freq[183] = 51.60;     gain_el_fs[183] = 930.91091;
  freq[184] = 51.80;     gain_el_fs[184] = 942.68080;
  freq[185] = 52.00;     gain_el_fs[185] = 948.50312;
  freq[186] = 52.20;     gain_el_fs[186] = 954.14390;
  freq[187] = 52.40;     gain_el_fs[187] = 959.81906;
  freq[188] = 52.60;     gain_el_fs[188] = 965.52755;
  freq[189] = 52.80;     gain_el_fs[189] = 972.56250;
  freq[190] = 53.00;     gain_el_fs[190] = 984.86074;
  freq[191] = 53.20;     gain_el_fs[191] = 997.31406;
  freq[192] = 53.40;     gain_el_fs[192] = 1009.92440;
  freq[193] = 53.60;     gain_el_fs[193] = 1020.07451;
  freq[194] = 53.80;     gain_el_fs[194] = 1026.16030;
  freq[195] = 54.00;     gain_el_fs[195] = 1028.10680;
  freq[196] = 54.20;     gain_el_fs[196] = 1029.26923;
  freq[197] = 54.40;     gain_el_fs[197] = 1029.64359;
  freq[198] = 54.60;     gain_el_fs[198] = 1030.01853;
  freq[199] = 54.80;     gain_el_fs[199] = 1034.26095;
  freq[200] = 55.00;     gain_el_fs[200] = 1039.30471;
  freq[201] = 55.20;     gain_el_fs[201] = 1044.37262;
  freq[202] = 55.40;     gain_el_fs[202] = 1049.46615;
  freq[203] = 55.60;     gain_el_fs[203] = 1053.36760;
  freq[204] = 55.80;     gain_el_fs[204] = 1060.10167;
  freq[205] = 56.00;     gain_el_fs[205] = 1066.87879;
  freq[206] = 56.20;     gain_el_fs[206] = 1073.69877;
  freq[207] = 56.40;     gain_el_fs[207] = 1078.35407;
  freq[208] = 56.60;     gain_el_fs[208] = 1082.33534;
  freq[209] = 56.80;     gain_el_fs[209] = 1084.24362;
  freq[210] = 57.00;     gain_el_fs[210] = 1084.60844;
  freq[211] = 57.20;     gain_el_fs[211] = 1083.42966;
  freq[212] = 57.40;     gain_el_fs[212] = 1082.25168;
  freq[213] = 57.60;     gain_el_fs[213] = 1079.41118;
  freq[214] = 57.80;     gain_el_fs[214] = 1071.89613;
  freq[215] = 58.00;     gain_el_fs[215] = 1064.43246;
  freq[216] = 58.20;     gain_el_fs[216] = 1057.02123;
  freq[217] = 58.40;     gain_el_fs[217] = 1047.57399;
  freq[218] = 58.60;     gain_el_fs[218] = 1038.38447;
  freq[219] = 58.80;     gain_el_fs[219] = 1030.01491;
  freq[220] = 59.00;     gain_el_fs[220] = 1021.71327;
  freq[221] = 59.20;     gain_el_fs[221] = 1010.94550;
  freq[222] = 59.40;     gain_el_fs[222] = 1000.29122;
  freq[223] = 59.60;     gain_el_fs[223] = 983.58948;
  freq[224] = 59.80;     gain_el_fs[224] = 966.70074;
  freq[225] = 60.00;     gain_el_fs[225] = 949.64312;
  freq[226] = 60.20;     gain_el_fs[226] = 932.88647;
  freq[227] = 60.40;     gain_el_fs[227] = 917.87799;
  freq[228] = 60.60;     gain_el_fs[228] = 906.55115;
  freq[229] = 60.80;     gain_el_fs[229] = 895.36329;
  freq[230] = 61.00;     gain_el_fs[230] = 884.31350;
  freq[231] = 61.20;     gain_el_fs[231] = 873.69050;
  freq[232] = 61.40;     gain_el_fs[232] = 861.31757;
  freq[233] = 61.60;     gain_el_fs[233] = 846.99131;
  freq[234] = 61.80;     gain_el_fs[234] = 832.90406;
  freq[235] = 62.00;     gain_el_fs[235] = 822.09201;
  freq[236] = 62.20;     gain_el_fs[236] = 811.41995;
  freq[237] = 62.40;     gain_el_fs[237] = 804.93810;
  freq[238] = 62.60;     gain_el_fs[238] = 800.82699;
  freq[239] = 62.80;     gain_el_fs[239] = 797.70709;
  freq[240] = 63.00;     gain_el_fs[240] = 794.60004;
  freq[241] = 63.20;     gain_el_fs[241] = 791.50474;
  freq[242] = 63.40;     gain_el_fs[242] = 786.78988;
  freq[243] = 63.60;     gain_el_fs[243] = 782.10344;
  freq[244] = 63.80;     gain_el_fs[244] = 777.44458;
  freq[245] = 64.00;     gain_el_fs[245] = 774.00265;
  freq[246] = 64.20;     gain_el_fs[246] = 773.15567;
  freq[247] = 64.40;     gain_el_fs[247] = 776.50141;
  freq[248] = 64.60;     gain_el_fs[248] = 779.86095;
  freq[249] = 64.80;     gain_el_fs[249] = 786.00623;
  freq[250] = 65.00;     gain_el_fs[250] = 792.19993;
  freq[251] = 65.20;     gain_el_fs[251] = 796.64942;
  freq[252] = 65.40;     gain_el_fs[252] = 800.93040;
  freq[253] = 65.60;     gain_el_fs[253] = 806.85076;
  freq[254] = 65.80;     gain_el_fs[254] = 812.81524;
  freq[255] = 66.00;     gain_el_fs[255] = 818.82345;
  freq[256] = 66.20;     gain_el_fs[256] = 827.14047;
  freq[257] = 66.40;     gain_el_fs[257] = 835.75924;
  freq[258] = 66.60;     gain_el_fs[258] = 844.46707;
  freq[259] = 66.80;     gain_el_fs[259] = 854.11746;
  freq[260] = 67.00;     gain_el_fs[260] = 864.74019;
  freq[261] = 67.20;     gain_el_fs[261] = 872.33628;
  freq[262] = 67.40;     gain_el_fs[262] = 879.99949;
  freq[263] = 67.60;     gain_el_fs[263] = 887.86298;
  freq[264] = 67.80;     gain_el_fs[264] = 895.79712;
  freq[265] = 68.00;     gain_el_fs[265] = 904.65678;
  freq[266] = 68.20;     gain_el_fs[266] = 915.88353;
  freq[267] = 68.40;     gain_el_fs[267] = 926.93079;
  freq[268] = 68.60;     gain_el_fs[268] = 938.11007;
  freq[269] = 68.80;     gain_el_fs[269] = 949.42502;
  freq[270] = 69.00;     gain_el_fs[270] = 955.82322;
  freq[271] = 69.20;     gain_el_fs[271] = 960.22761;
  freq[272] = 69.40;     gain_el_fs[272] = 964.65315;
  freq[273] = 69.60;     gain_el_fs[273] = 967.27152;
  freq[274] = 69.80;     gain_el_fs[274] = 968.06794;
  freq[275] = 70.00;     gain_el_fs[275] = 968.80885;
  freq[276] = 70.20;     gain_el_fs[276] = 969.52989;
  freq[277] = 70.40;     gain_el_fs[277] = 965.77438;
  freq[278] = 70.60;     gain_el_fs[278] = 962.03299;
  freq[279] = 70.80;     gain_el_fs[279] = 958.30651;
  freq[280] = 71.00;     gain_el_fs[280] = 950.50016;
  freq[281] = 71.20;     gain_el_fs[281] = 940.44780;
  freq[282] = 71.40;     gain_el_fs[282] = 930.50134;
  freq[283] = 71.60;     gain_el_fs[283] = 920.66089;
  freq[284] = 71.80;     gain_el_fs[284] = 907.44143;
  freq[285] = 72.00;     gain_el_fs[285] = 897.14875;
  freq[286] = 72.20;     gain_el_fs[286] = 886.97204;
  freq[287] = 72.40;     gain_el_fs[287] = 875.08332;
  freq[288] = 72.60;     gain_el_fs[288] = 861.55365;
  freq[289] = 72.80;     gain_el_fs[289] = 849.01769;
  freq[290] = 73.00;     gain_el_fs[290] = 837.43871;
  freq[291] = 73.20;     gain_el_fs[291] = 823.43910;
  freq[292] = 73.40;     gain_el_fs[292] = 809.67317;
  freq[293] = 73.60;     gain_el_fs[293] = 796.13771;
  freq[294] = 73.80;     gain_el_fs[294] = 778.69492;
  freq[295] = 74.00;     gain_el_fs[295] = 759.83214;
  freq[296] = 74.20;     gain_el_fs[296] = 741.42628;
  freq[297] = 74.40;     gain_el_fs[297] = 723.46533;
  freq[298] = 74.60;     gain_el_fs[298] = 705.52973;
  freq[299] = 74.80;     gain_el_fs[299] = 694.12384;
  freq[300] = 75.00;     gain_el_fs[300] = 682.90323;
  freq[301] = 75.20;     gain_el_fs[301] = 672.09716;
  freq[302] = 75.40;     gain_el_fs[302] = 661.69133;
  freq[303] = 75.60;     gain_el_fs[303] = 650.90866;
  freq[304] = 75.80;     gain_el_fs[304] = 638.71484;
  freq[305] = 76.00;     gain_el_fs[305] = 629.53711;
  freq[306] = 76.20;     gain_el_fs[306] = 620.49099;
  freq[307] = 76.40;     gain_el_fs[307] = 611.57486;
  freq[308] = 76.60;     gain_el_fs[308] = 603.43950;
  freq[309] = 76.80;     gain_el_fs[309] = 596.05704;
  freq[310] = 77.00;     gain_el_fs[310] = 588.76463;
  freq[311] = 77.20;     gain_el_fs[311] = 581.56195;
  freq[312] = 77.40;     gain_el_fs[312] = 576.62547;
  freq[313] = 77.60;     gain_el_fs[313] = 572.83225;
  freq[314] = 77.80;     gain_el_fs[314] = 569.42848;
  freq[315] = 78.00;     gain_el_fs[315] = 565.87393;
  freq[316] = 78.20;     gain_el_fs[316] = 562.17193;
  freq[317] = 78.40;     gain_el_fs[317] = 558.49390;
  freq[318] = 78.60;     gain_el_fs[318] = 555.33360;
  freq[319] = 78.80;     gain_el_fs[319] = 546.45014;
  freq[320] = 79.00;     gain_el_fs[320] = 537.70951;
  freq[321] = 79.20;     gain_el_fs[321] = 529.10844;
  freq[322] = 79.40;     gain_el_fs[322] = 514.59148;
  freq[323] = 79.60;     gain_el_fs[323] = 493.38181;
  freq[324] = 79.80;     gain_el_fs[324] = 473.04571;
  freq[325] = 80.00;     gain_el_fs[325] = 453.54861;
  freq[326] = 80.20;     gain_el_fs[326] = 420.17984;
  freq[327] = 80.40;     gain_el_fs[327] = 389.70092;
  freq[328] = 80.60;     gain_el_fs[328] = 361.83883;
  freq[329] = 80.80;     gain_el_fs[329] = 329.46732;
  freq[330] = 81.00;     gain_el_fs[330] = 294.18924;
  freq[331] = 81.20;     gain_el_fs[331] = 262.68860;
  freq[332] = 81.40;     gain_el_fs[332] = 234.69395;
  freq[333] = 81.60;     gain_el_fs[333] = 203.46862;
  freq[334] = 81.80;     gain_el_fs[334] = 176.39671;
  freq[335] = 82.00;     gain_el_fs[335] = 152.92765;
  freq[336] = 82.20;     gain_el_fs[336] = 131.50419;
  freq[337] = 82.40;     gain_el_fs[337] = 111.55170;
  freq[338] = 82.60;     gain_el_fs[338] = 94.62709;
  freq[339] = 82.80;     gain_el_fs[339] = 80.26978;
  freq[340] = 83.00;     gain_el_fs[340] = 67.75144;
  freq[341] = 83.20;     gain_el_fs[341] = 57.19994;
  freq[342] = 83.40;     gain_el_fs[342] = 48.32823;
  freq[343] = 83.60;     gain_el_fs[343] = 40.87535;
  freq[344] = 83.80;     gain_el_fs[344] = 34.60763;
  freq[345] = 84.00;     gain_el_fs[345] = 29.30117;
  freq[346] = 84.20;     gain_el_fs[346] = 24.75620;
  freq[347] = 84.40;     gain_el_fs[347] = 20.92256;
  freq[348] = 84.60;     gain_el_fs[348] = 17.68269;
  freq[349] = 84.80;     gain_el_fs[349] = 14.94442;
  freq[350] = 85.00;     gain_el_fs[350] = 12.65502;
  freq[351] = 85.20;     gain_el_fs[351] = 10.77401;
  freq[352] = 85.40;     gain_el_fs[352] = 9.18295;
  freq[353] = 85.60;     gain_el_fs[353] = 7.82690;
  freq[354] = 85.80;     gain_el_fs[354] = 6.68658;
  freq[355] = 86.00;     gain_el_fs[355] = 5.71243;
  freq[356] = 86.20;     gain_el_fs[356] = 4.89701;
  freq[357] = 86.40;     gain_el_fs[357] = 4.20101;
  freq[358] = 86.60;     gain_el_fs[358] = 3.60657;
  freq[359] = 86.80;     gain_el_fs[359] = 3.09622;
  freq[360] = 87.00;     gain_el_fs[360] = 2.65646;
  freq[361] = 87.20;     gain_el_fs[361] = 2.27188;
  freq[362] = 87.40;     gain_el_fs[362] = 1.94297;
  freq[363] = 87.60;     gain_el_fs[363] = 1.66168;
  freq[364] = 87.80;     gain_el_fs[364] = 1.41992;
  freq[365] = 88.00;     gain_el_fs[365] = 1.21436;
  freq[366] = 88.20;     gain_el_fs[366] = 1.04029;
  freq[367] = 88.40;     gain_el_fs[367] = 0.89117;
  freq[368] = 88.60;     gain_el_fs[368] = 0.75849;
  freq[369] = 88.80;     gain_el_fs[369] = 0.64556;
  freq[370] = 89.00;     gain_el_fs[370] = 0.55031;
  freq[371] = 89.20;     gain_el_fs[371] = 0.46859;
  freq[372] = 89.40;     gain_el_fs[372] = 0.39836;
  freq[373] = 89.60;     gain_el_fs[373] = 0.33865;
  freq[374] = 89.80;     gain_el_fs[374] = 0.28789;
  freq[375] = 90.00;     gain_el_fs[375] = 0.24130;
  freq[376] = 90.20;     gain_el_fs[376] = 0.20224;
  freq[377] = 90.40;     gain_el_fs[377] = 0.16951;
  freq[378] = 90.60;     gain_el_fs[378] = 0.14193;
  freq[379] = 90.80;     gain_el_fs[379] = 0.11873;
  freq[380] = 91.00;     gain_el_fs[380] = 0.09937;
  freq[381] = 91.20;     gain_el_fs[381] = 0.08317;
  freq[382] = 91.40;     gain_el_fs[382] = 0.07109;
  freq[383] = 91.60;     gain_el_fs[383] = 0.06076;
  freq[384] = 91.80;     gain_el_fs[384] = 0.05183;
  freq[385] = 92.00;     gain_el_fs[385] = 0.04427;
  freq[386] = 92.20;     gain_el_fs[386] = 0.03792;
  freq[387] = 92.40;     gain_el_fs[387] = 0.03249;
  freq[388] = 92.60;     gain_el_fs[388] = 0.02783;
  freq[389] = 92.80;     gain_el_fs[389] = 0.02443;
  freq[390] = 93.00;     gain_el_fs[390] = 0.02143;
  freq[391] = 93.20;     gain_el_fs[391] = 0.01880;
  freq[392] = 93.40;     gain_el_fs[392] = 0.01646;
  freq[393] = 93.60;     gain_el_fs[393] = 0.01439;
  freq[394] = 93.80;     gain_el_fs[394] = 0.01254;
  freq[395] = 94.00;     gain_el_fs[395] = 0.01093;
  freq[396] = 94.20;     gain_el_fs[396] = 0.01068;
  freq[397] = 94.40;     gain_el_fs[397] = 0.01044;
  freq[398] = 94.60;     gain_el_fs[398] = 0.01019;
  freq[399] = 94.80;     gain_el_fs[399] = 0.01062;
  freq[400] = 95.00;     gain_el_fs[400] = 0.01186;
  freq[401] = 95.20;     gain_el_fs[401] = 0.01324;
  freq[402] = 95.40;     gain_el_fs[402] = 0.01479;
  freq[403] = 95.60;     gain_el_fs[403] = 0.01630;
  freq[404] = 95.80;     gain_el_fs[404] = 0.01780;
  freq[405] = 96.00;     gain_el_fs[405] = 0.01944;
  freq[406] = 96.20;     gain_el_fs[406] = 0.02043;
  freq[407] = 96.40;     gain_el_fs[407] = 0.02064;
  freq[408] = 96.60;     gain_el_fs[408] = 0.02200;
  freq[409] = 96.80;     gain_el_fs[409] = 0.02386;
  freq[410] = 97.00;     gain_el_fs[410] = 0.02685;
  freq[411] = 97.20;     gain_el_fs[411] = 0.03022;
  freq[412] = 97.40;     gain_el_fs[412] = 0.03401;
  freq[413] = 97.60;     gain_el_fs[413] = 0.03562;
  freq[414] = 97.80;     gain_el_fs[414] = 0.03659;
  freq[415] = 98.00;     gain_el_fs[415] = 0.03758;
  freq[416] = 98.20;     gain_el_fs[416] = 0.03859;
  freq[417] = 98.40;     gain_el_fs[417] = 0.03974;
  freq[418] = 98.60;     gain_el_fs[418] = 0.04095;
  freq[419] = 98.80;     gain_el_fs[419] = 0.04219;
  freq[420] = 99.00;     gain_el_fs[420] = 0.04332;
  freq[421] = 99.20;     gain_el_fs[421] = 0.04432;
  freq[422] = 99.40;     gain_el_fs[422] = 0.04531;
  freq[423] = 99.60;     gain_el_fs[423] = 0.04629;
  freq[424] = 99.80;     gain_el_fs[424] = 0.04625;
  freq[425] = 100.00;    gain_el_fs[425] = 0.04621;
  freq[426] = 100.20;    gain_el_fs[426] = 0.04616;
  freq[427] = 100.40;    gain_el_fs[427] = 0.04628;
  freq[428] = 100.60;    gain_el_fs[428] = 0.04650;
  freq[429] = 100.80;    gain_el_fs[429] = 0.04672;
  freq[430] = 101.00;    gain_el_fs[430] = 0.04693;
  freq[431] = 101.20;    gain_el_fs[431] = 0.04660;
  freq[432] = 101.40;    gain_el_fs[432] = 0.04627;
  freq[433] = 101.60;    gain_el_fs[433] = 0.04595;
  freq[434] = 101.80;    gain_el_fs[434] = 0.04567;
  freq[435] = 102.00;    gain_el_fs[435] = 0.04544;
  freq[436] = 102.20;    gain_el_fs[436] = 0.04516;
  freq[437] = 102.40;    gain_el_fs[437] = 0.04477;
  freq[438] = 102.60;    gain_el_fs[438] = 0.04505;
  freq[439] = 102.80;    gain_el_fs[439] = 0.04534;
  freq[440] = 103.00;    gain_el_fs[440] = 0.04562;
  freq[441] = 103.20;    gain_el_fs[441] = 0.04549;
  freq[442] = 103.40;    gain_el_fs[442] = 0.04493;
  freq[443] = 103.60;    gain_el_fs[443] = 0.04439;
  freq[444] = 103.80;    gain_el_fs[444] = 0.04385;
  freq[445] = 104.00;    gain_el_fs[445] = 0.04403;
  freq[446] = 104.20;    gain_el_fs[446] = 0.04434;
  freq[447] = 104.40;    gain_el_fs[447] = 0.04471;
  freq[448] = 104.60;    gain_el_fs[448] = 0.04464;
  freq[449] = 104.80;    gain_el_fs[449] = 0.04414;
  freq[450] = 105.00;    gain_el_fs[450] = 0.04365;
  freq[451] = 105.20;    gain_el_fs[451] = 0.04329;
  freq[452] = 105.40;    gain_el_fs[452] = 0.04342;
  freq[453] = 105.60;    gain_el_fs[453] = 0.04354;
  freq[454] = 105.80;    gain_el_fs[454] = 0.04367;
  freq[455] = 106.00;    gain_el_fs[455] = 0.04389;
  freq[456] = 106.20;    gain_el_fs[456] = 0.04443;
  freq[457] = 106.40;    gain_el_fs[457] = 0.04499;
  freq[458] = 106.60;    gain_el_fs[458] = 0.04555;
  freq[459] = 106.80;    gain_el_fs[459] = 0.04575;
  freq[460] = 107.00;    gain_el_fs[460] = 0.04522;
  freq[461] = 107.20;    gain_el_fs[461] = 0.04397;
  freq[462] = 107.40;    gain_el_fs[462] = 0.04268;
  freq[463] = 107.60;    gain_el_fs[463] = 0.04136;
  freq[464] = 107.80;    gain_el_fs[464] = 0.04008;
  freq[465] = 108.00;    gain_el_fs[465] = 0.03887;
  freq[466] = 108.20;    gain_el_fs[466] = 0.03831;
  freq[467] = 108.40;    gain_el_fs[467] = 0.03776;
  freq[468] = 108.60;    gain_el_fs[468] = 0.03722;
  freq[469] = 108.80;    gain_el_fs[469] = 0.03607;
  freq[470] = 109.00;    gain_el_fs[470] = 0.03457;
  freq[471] = 109.20;    gain_el_fs[471] = 0.03313;
  freq[472] = 109.40;    gain_el_fs[472] = 0.03175;
  freq[473] = 109.60;    gain_el_fs[473] = 0.03104;
  freq[474] = 109.80;    gain_el_fs[474] = 0.03037;
  freq[475] = 110.00;    gain_el_fs[475] = 0.02978;
  freq[476] = 110.20;    gain_el_fs[476] = 0.02932;
  freq[477] = 110.40;    gain_el_fs[477] = 0.02899;
  freq[478] = 110.60;    gain_el_fs[478] = 0.02866;
  freq[479] = 110.80;    gain_el_fs[479] = 0.02844;
  freq[480] = 111.00;    gain_el_fs[480] = 0.02731;
  freq[481] = 111.20;    gain_el_fs[481] = 0.02622;
  freq[482] = 111.40;    gain_el_fs[482] = 0.02517;
  freq[483] = 111.60;    gain_el_fs[483] = 0.02464;
  freq[484] = 111.80;    gain_el_fs[484] = 0.02453;
  freq[485] = 112.00;    gain_el_fs[485] = 0.02440;
  freq[486] = 112.20;    gain_el_fs[486] = 0.02428;
  freq[487] = 112.40;    gain_el_fs[487] = 0.02335;
  freq[488] = 112.60;    gain_el_fs[488] = 0.02246;
  freq[489] = 112.80;    gain_el_fs[489] = 0.02163;
  freq[490] = 113.00;    gain_el_fs[490] = 0.02088;
  freq[491] = 113.20;    gain_el_fs[491] = 0.02019;
  freq[492] = 113.40;    gain_el_fs[492] = 0.01953;
  freq[493] = 113.60;    gain_el_fs[493] = 0.01891;
  freq[494] = 113.80;    gain_el_fs[494] = 0.01881;
  freq[495] = 114.00;    gain_el_fs[495] = 0.01872;
  freq[496] = 114.20;    gain_el_fs[496] = 0.01862;
  freq[497] = 114.40;    gain_el_fs[497] = 0.01874;
  freq[498] = 114.60;    gain_el_fs[498] = 0.01909;
  freq[499] = 114.80;    gain_el_fs[499] = 0.01946;
  freq[500] = 115.00;    gain_el_fs[500] = 0.01983;
  freq[501] = 115.20;    gain_el_fs[501] = 0.01896;
  freq[502] = 115.40;    gain_el_fs[502] = 0.01813;
  freq[503] = 115.60;    gain_el_fs[503] = 0.01728;
  freq[504] = 115.80;    gain_el_fs[504] = 0.01655;
  freq[505] = 116.00;    gain_el_fs[505] = 0.01595;
  freq[506] = 116.20;    gain_el_fs[506] = 0.01537;
  freq[507] = 116.40;    gain_el_fs[507] = 0.01481;
  freq[508] = 116.60;    gain_el_fs[508] = 0.01426;
  freq[509] = 116.80;    gain_el_fs[509] = 0.01373;
  freq[510] = 117.00;    gain_el_fs[510] = 0.01322;
  freq[511] = 117.20;    gain_el_fs[511] = 0.01265;
  freq[512] = 117.40;    gain_el_fs[512] = 0.01203;
  freq[513] = 117.60;    gain_el_fs[513] = 0.01145;
  freq[514] = 117.80;    gain_el_fs[514] = 0.01090;
  freq[515] = 118.00;    gain_el_fs[515] = 0.01096;
  freq[516] = 118.20;    gain_el_fs[516] = 0.01102;
  freq[517] = 118.40;    gain_el_fs[517] = 0.01110;
  freq[518] = 118.60;    gain_el_fs[518] = 0.01077;
  freq[519] = 118.80;    gain_el_fs[519] = 0.01006;
  freq[520] = 119.00;    gain_el_fs[520] = 0.00939;
  freq[521] = 119.20;    gain_el_fs[521] = 0.00876;
  freq[522] = 119.40;    gain_el_fs[522] = 0.00911;
  freq[523] = 119.60;    gain_el_fs[523] = 0.00946;
  freq[524] = 119.80;    gain_el_fs[524] = 0.00982;
  freq[525] = 120.00;    gain_el_fs[525] = 0.00933;
  freq[526] = 120.20;    gain_el_fs[526] = 0.00811;
  freq[527] = 120.40;    gain_el_fs[527] = 0.00710;
  freq[528] = 120.60;    gain_el_fs[528] = 0.00621;
  freq[529] = 120.80;    gain_el_fs[529] = 0.00605;
  freq[530] = 121.00;    gain_el_fs[530] = 0.00589;
  freq[531] = 121.20;    gain_el_fs[531] = 0.00573;
  freq[532] = 121.40;    gain_el_fs[532] = 0.00582;
  freq[533] = 121.60;    gain_el_fs[533] = 0.00617;
  freq[534] = 121.80;    gain_el_fs[534] = 0.00654;
  freq[535] = 122.00;    gain_el_fs[535] = 0.00693;
  freq[536] = 122.20;    gain_el_fs[536] = 0.00614;
  freq[537] = 122.40;    gain_el_fs[537] = 0.00543;
  freq[538] = 122.60;    gain_el_fs[538] = 0.00480;
  freq[539] = 122.80;    gain_el_fs[539] = 0.00451;
  freq[540] = 123.00;    gain_el_fs[540] = 0.00449;
  freq[541] = 123.20;    gain_el_fs[541] = 0.00450;
  freq[542] = 123.40;    gain_el_fs[542] = 0.00451;
  freq[543] = 123.60;    gain_el_fs[543] = 0.00491;
  freq[544] = 123.80;    gain_el_fs[544] = 0.00535;
  freq[545] = 124.00;    gain_el_fs[545] = 0.00582;
  freq[546] = 124.20;    gain_el_fs[546] = 0.00620;
  freq[547] = 124.40;    gain_el_fs[547] = 0.00646;
  freq[548] = 124.60;    gain_el_fs[548] = 0.00673;
  freq[549] = 124.80;    gain_el_fs[549] = 0.00702;
  freq[550] = 125.00;    gain_el_fs[550] = 0.00715;
  freq[551] = 125.20;    gain_el_fs[551] = 0.00730;
  freq[552] = 125.40;    gain_el_fs[552] = 0.00745;
  freq[553] = 125.60;    gain_el_fs[553] = 0.00756;
  freq[554] = 125.80;    gain_el_fs[554] = 0.00764;
  freq[555] = 126.00;    gain_el_fs[555] = 0.00773;
  freq[556] = 126.20;    gain_el_fs[556] = 0.00782;
  freq[557] = 126.40;    gain_el_fs[557] = 0.00821;
  freq[558] = 126.60;    gain_el_fs[558] = 0.00861;
  freq[559] = 126.80;    gain_el_fs[559] = 0.00904;
  freq[560] = 127.00;    gain_el_fs[560] = 0.00955;
  freq[561] = 127.20;    gain_el_fs[561] = 0.01016;
  freq[562] = 127.40;    gain_el_fs[562] = 0.01082;
  freq[563] = 127.60;    gain_el_fs[563] = 0.01151;
  freq[564] = 127.80;    gain_el_fs[564] = 0.01164;
  freq[565] = 128.00;    gain_el_fs[565] = 0.01179;
  freq[566] = 128.20;    gain_el_fs[566] = 0.01194;
  freq[567] = 128.40;    gain_el_fs[567] = 0.01221;
  freq[568] = 128.60;    gain_el_fs[568] = 0.01260;
  freq[569] = 128.80;    gain_el_fs[569] = 0.01297;
  freq[570] = 129.00;    gain_el_fs[570] = 0.01325;
  freq[571] = 129.20;    gain_el_fs[571] = 0.01481;
  freq[572] = 129.40;    gain_el_fs[572] = 0.01656;
  freq[573] = 129.60;    gain_el_fs[573] = 0.01850;
  freq[574] = 129.80;    gain_el_fs[574] = 0.01948;
  freq[575] = 130.00;    gain_el_fs[575] = 0.01932;
  freq[576] = 130.20;    gain_el_fs[576] = 0.01917;
  freq[577] = 130.40;    gain_el_fs[577] = 0.01901;
  freq[578] = 130.60;    gain_el_fs[578] = 0.01954;
  freq[579] = 130.80;    gain_el_fs[579] = 0.01996;
  freq[580] = 131.00;    gain_el_fs[580] = 0.02033;
  freq[581] = 131.20;    gain_el_fs[581] = 0.02082;
  freq[582] = 131.40;    gain_el_fs[582] = 0.02143;
  freq[583] = 131.60;    gain_el_fs[583] = 0.02206;
  freq[584] = 131.80;    gain_el_fs[584] = 0.02282;
  freq[585] = 132.00;    gain_el_fs[585] = 0.02231;
  freq[586] = 132.20;    gain_el_fs[586] = 0.02181;
  freq[587] = 132.40;    gain_el_fs[587] = 0.02132;
  freq[588] = 132.60;    gain_el_fs[588] = 0.02117;
  freq[589] = 132.80;    gain_el_fs[589] = 0.02131;
  freq[590] = 133.00;    gain_el_fs[590] = 0.02146;
  freq[591] = 133.20;    gain_el_fs[591] = 0.02160;
  freq[592] = 133.40;    gain_el_fs[592] = 0.02118;
  freq[593] = 133.60;    gain_el_fs[593] = 0.02071;
  freq[594] = 133.80;    gain_el_fs[594] = 0.02019;
  freq[595] = 134.00;    gain_el_fs[595] = 0.02005;
  freq[596] = 134.20;    gain_el_fs[596] = 0.02028;
  freq[597] = 134.40;    gain_el_fs[597] = 0.02052;
  freq[598] = 134.60;    gain_el_fs[598] = 0.02079;
  freq[599] = 134.80;    gain_el_fs[599] = 0.02112;
  freq[600] = 135.00;    gain_el_fs[600] = 0.02146;
  freq[601] = 135.20;    gain_el_fs[601] = 0.02180;
  freq[602] = 135.40;    gain_el_fs[602] = 0.02177;
  freq[603] = 135.60;    gain_el_fs[603] = 0.02142;
  freq[604] = 135.80;    gain_el_fs[604] = 0.02107;
  freq[605] = 136.00;    gain_el_fs[605] = 0.02073;
  freq[606] = 136.20;    gain_el_fs[606] = 0.02036;
  freq[607] = 136.40;    gain_el_fs[607] = 0.02001;
  freq[608] = 136.60;    gain_el_fs[608] = 0.01970;
  freq[609] = 136.80;    gain_el_fs[609] = 0.01951;
  freq[610] = 137.00;    gain_el_fs[610] = 0.01943;
  freq[611] = 137.20;    gain_el_fs[611] = 0.01935;
  freq[612] = 137.40;    gain_el_fs[612] = 0.01931;
  freq[613] = 137.60;    gain_el_fs[613] = 0.02059;
  freq[614] = 137.80;    gain_el_fs[614] = 0.02195;
  freq[615] = 138.00;    gain_el_fs[615] = 0.02340;
  freq[616] = 138.20;    gain_el_fs[616] = 0.02356;
  freq[617] = 138.40;    gain_el_fs[617] = 0.02237;
  freq[618] = 138.60;    gain_el_fs[618] = 0.02122;
  freq[619] = 138.80;    gain_el_fs[619] = 0.02014;
  freq[620] = 139.00;    gain_el_fs[620] = 0.02028;
  freq[621] = 139.20;    gain_el_fs[621] = 0.02043;
  freq[622] = 139.40;    gain_el_fs[622] = 0.02068;
  freq[623] = 139.60;    gain_el_fs[623] = 0.02064;
  freq[624] = 139.80;    gain_el_fs[624] = 0.02033;
  freq[625] = 140.00;    gain_el_fs[625] = 0.02002;
  freq[626] = 140.20;    gain_el_fs[626] = 0.01970;
  freq[627] = 140.40;    gain_el_fs[627] = 0.02054;
  freq[628] = 140.60;    gain_el_fs[628] = 0.02141;
  freq[629] = 140.80;    gain_el_fs[629] = 0.02232;
  freq[630] = 141.00;    gain_el_fs[630] = 0.02257;
  freq[631] = 141.20;    gain_el_fs[631] = 0.02214;
  freq[632] = 141.40;    gain_el_fs[632] = 0.02172;
  freq[633] = 141.60;    gain_el_fs[633] = 0.02131;
  freq[634] = 141.80;    gain_el_fs[634] = 0.02137;
  freq[635] = 142.00;    gain_el_fs[635] = 0.02143;
  freq[636] = 142.20;    gain_el_fs[636] = 0.02152;
  freq[637] = 142.40;    gain_el_fs[637] = 0.02146;
  freq[638] = 142.60;    gain_el_fs[638] = 0.02123;
  freq[639] = 142.80;    gain_el_fs[639] = 0.02101;
  freq[640] = 143.00;    gain_el_fs[640] = 0.02079;
  freq[641] = 143.20;    gain_el_fs[641] = 0.02146;
  freq[642] = 143.40;    gain_el_fs[642] = 0.02214;
  freq[643] = 143.60;    gain_el_fs[643] = 0.02285;
  freq[644] = 143.80;    gain_el_fs[644] = 0.02301;
  freq[645] = 144.00;    gain_el_fs[645] = 0.02264;
  freq[646] = 144.20;    gain_el_fs[646] = 0.02233;
  freq[647] = 144.40;    gain_el_fs[647] = 0.02203;
  freq[648] = 144.60;    gain_el_fs[648] = 0.02215;
  freq[649] = 144.80;    gain_el_fs[649] = 0.02227;
  freq[650] = 145.00;    gain_el_fs[650] = 0.02235;
  freq[651] = 145.20;    gain_el_fs[651] = 0.02206;
  freq[652] = 145.40;    gain_el_fs[652] = 0.02146;
  freq[653] = 145.60;    gain_el_fs[653] = 0.02088;
  freq[654] = 145.80;    gain_el_fs[654] = 0.02032;


  if(frequency>140.0){
   cerr << "FieldStrengthElectronicsGain: upper frequency limit reached." << endl;  
   exit(1);
  }
  if(frequency<20.0){
   cerr << "FieldStrengthElectronicsGain: lower frequency limit reached." << endl;  
   exit(1);
  }

  float fsElectronicsGain = 0;

  //linear interpolation
  for(int i=0; i<entries; i++){
    if(frequency<freq[i]){
      fsElectronicsGain = (( (gain_el_fs[i-1]-gain_el_fs[i]) / (freq[i-1]-freq[i]))*(frequency-freq[i])+gain_el_fs[i]);
      break;
    }
  }

  if(fsElectronicsGain==0){
    cerr << "FieldStrengthElectronicsGain: no interpolation possible .." << endl;
    exit(1);
  }
  
  //clean up
  delete[] freq;
  delete[] gain_el_fs;

  return fsElectronicsGain;
}
