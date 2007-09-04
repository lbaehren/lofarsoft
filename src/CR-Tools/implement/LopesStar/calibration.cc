#include "calibration.h"

//!std c++
#include <iostream>
#include <time.h>
#include <math.h>

//!root 
#include <TMath.h>

//!star tool headers
#include <LopesStar/trigger.h>
#include <LopesStar/util.h>
#include <LopesStar/reconstruction.h>

using namespace std;
using namespace TMath;

/**********************************************************************************************/

void ADC2FieldStrength(unsigned int window_size, float *trace, float zenith, int daq_id){
  //result is in uV/m

#warning zenith dependece not implemented
  
  float Amp[window_size/2], Phase[window_size/2];
  float rawfft[2*window_size];
  float freq = 0;
  
  TraceFFT(window_size, trace, Amp, Phase, rawfft);
  
  if(daq_id == 17 || daq_id == 30 || daq_id == 19){
    for(unsigned int i=0; i<window_size/2; i++){
      freq = 40+(float)((float)(i*40.0)/(float)(window_size/2));
      Amp[i] *= FieldStrengthCorrection_RefAnt(freq);
    }
  }
  if(daq_id == 42){
   cerr << "ADC2FieldStrength for D42 not implemented" << endl;;
   exit(1);
  }
  
  AmpPhase2Trace(window_size, Amp, Phase, trace, rawfft, 0);
  
}

/**********************************************************************************************/

void PowerTrace(unsigned int window_size, float *trace){

  for(unsigned int j=0; j<window_size; j++){
   trace[j] = pow(trace[j],2) / (120*Pi());  // P = E^2 / (120*Pi)
  }
}

/**********************************************************************************************/

float AmpRefSource_FieldStrength(float frequency){
  //derived from RefAnt_plots.C
  int entries = 41;
  double freq[entries], FS_LPDA[entries];

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
   exit(1);
  }
  if(frequency<40.0){
   cerr << "AmpRefSource_FieldStrength: lower frequency limit reached." << endl;  
   exit(1);
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

  return amp_value;
}

/**********************************************************************************************/

float GainDirectivity(float frequency, float zenith){
  //derived from LPDA_Directivity_plot.C
  int entries = 37;
  double ZENITH[entries], gain_fs_036[entries], gain_fs_050[entries], gain_fs_066[entries], gain_fs_100[entries];

  ZENITH[ 0] = 0.0;	gain_fs_036[ 0] = 1.00000;	gain_fs_050[ 0] = 1.00000;	gain_fs_066[ 0] = 1.00000;	gain_fs_100[ 0] = 1.00000;
  ZENITH[ 1] = 10.0;	gain_fs_036[ 1] = 1.01158;	gain_fs_050[ 1] = 1.00000;	gain_fs_066[ 1] = 0.95499;	gain_fs_100[ 1] = 0.96605;
  ZENITH[ 2] = 20.0;	gain_fs_036[ 2] = 0.98855;	gain_fs_050[ 2] = 1.00000;	gain_fs_066[ 2] = 0.93325;	gain_fs_100[ 2] = 0.93325;
  ZENITH[ 3] = 30.0;	gain_fs_036[ 3] = 0.94406;	gain_fs_050[ 3] = 0.93325;	gain_fs_066[ 3] = 0.88105;	gain_fs_100[ 3] = 0.90157;
  ZENITH[ 4] = 40.0;	gain_fs_036[ 4] = 0.85114;	gain_fs_050[ 4] = 0.83176;	gain_fs_066[ 4] = 0.75858;	gain_fs_100[ 4] = 0.93325;
  ZENITH[ 5] = 50.0;	gain_fs_036[ 5] = 0.68391;	gain_fs_050[ 5] = 0.68391;	gain_fs_066[ 5] = 0.63096;	gain_fs_100[ 5] = 0.85114;
  ZENITH[ 6] = 60.0;	gain_fs_036[ 6] = 0.53088;	gain_fs_050[ 6] = 0.53088;	gain_fs_066[ 6] = 0.51880;	gain_fs_100[ 6] = 0.69183;
  ZENITH[ 7] = 70.0;	gain_fs_036[ 7] = 0.37584;	gain_fs_050[ 7] = 0.34674;	gain_fs_066[ 7] = 0.36308;	gain_fs_100[ 7] = 0.47315;
  ZENITH[ 8] = 80.0;	gain_fs_036[ 8] = 0.23714;	gain_fs_050[ 8] = 0.09772;	gain_fs_066[ 8] = 0.23442;	gain_fs_100[ 8] = 0.18621;
  ZENITH[ 9] = 90.0;	gain_fs_036[ 9] = 0.04217;	gain_fs_050[ 9] = 0.03055;	gain_fs_066[ 9] = 0.12882;	gain_fs_100[ 9] = 0.01820;
  ZENITH[10] = 100.0;   gain_fs_036[10] = 0.03758;      gain_fs_050[10] = 0.04955;      gain_fs_066[10] = 0.03236;      gain_fs_100[10] = 0.03388;
  ZENITH[11] = 110.0;   gain_fs_036[11] = 0.05957;      gain_fs_050[11] = 0.05433;      gain_fs_066[11] = 0.05070;      gain_fs_100[11] = 0.09550;
  ZENITH[12] = 120.0;   gain_fs_036[12] = 0.09441;      gain_fs_050[12] = 0.06839;      gain_fs_066[12] = 0.09016;      gain_fs_100[12] = 0.14289;
  ZENITH[13] = 130.0;   gain_fs_036[13] = 0.14962;      gain_fs_050[13] = 0.08610;      gain_fs_066[13] = 0.11885;      gain_fs_100[13] = 0.17579;
  ZENITH[14] = 140.0;   gain_fs_036[14] = 0.19953;      gain_fs_050[14] = 0.10839;      gain_fs_066[14] = 0.09550;      gain_fs_100[14] = 0.24831;
  ZENITH[15] = 150.0;   gain_fs_036[15] = 0.23714;      gain_fs_050[15] = 0.13646;      gain_fs_066[15] = 0.08511;      gain_fs_100[15] = 0.24547;
  ZENITH[16] = 160.0;   gain_fs_036[16] = 0.26607;      gain_fs_050[16] = 0.13646;      gain_fs_066[16] = 0.12735;      gain_fs_100[16] = 0.17378;
  ZENITH[17] = 170.0;   gain_fs_036[17] = 0.29854;      gain_fs_050[17] = 0.14454;      gain_fs_066[17] = 0.13964;      gain_fs_100[17] = 0.06607;
  ZENITH[18] = 180.0;   gain_fs_036[18] = 0.26607;      gain_fs_050[18] = 0.14962;      gain_fs_066[18] = 0.12303;      gain_fs_100[18] = 0.01698;
  ZENITH[19] = 190.0;   gain_fs_036[19] = 0.29854;      gain_fs_050[19] = 0.14454;      gain_fs_066[19] = 0.13964;      gain_fs_100[19] = 0.06607;
  ZENITH[20] = 200.0;   gain_fs_036[20] = 0.26607;      gain_fs_050[20] = 0.13646;      gain_fs_066[20] = 0.12735;      gain_fs_100[20] = 0.17378;
  ZENITH[21] = 210.0;   gain_fs_036[21] = 0.23714;      gain_fs_050[21] = 0.13646;      gain_fs_066[21] = 0.08511;      gain_fs_100[21] = 0.24547;
  ZENITH[22] = 220.0;   gain_fs_036[22] = 0.19953;      gain_fs_050[22] = 0.10839;      gain_fs_066[22] = 0.09550;      gain_fs_100[22] = 0.24831;
  ZENITH[23] = 230.0;   gain_fs_036[23] = 0.14962;      gain_fs_050[23] = 0.08610;      gain_fs_066[23] = 0.11885;      gain_fs_100[23] = 0.17579;
  ZENITH[24] = 240.0;   gain_fs_036[24] = 0.09441;      gain_fs_050[24] = 0.06839;      gain_fs_066[24] = 0.09016;      gain_fs_100[24] = 0.14289;
  ZENITH[25] = 250.0;   gain_fs_036[25] = 0.05957;      gain_fs_050[25] = 0.05433;      gain_fs_066[25] = 0.05070;      gain_fs_100[25] = 0.09550;
  ZENITH[26] = 260.0;   gain_fs_036[26] = 0.03758;      gain_fs_050[26] = 0.04955;      gain_fs_066[26] = 0.03236;      gain_fs_100[26] = 0.03388;
  ZENITH[27] = 270.0;   gain_fs_036[27] = 0.04217;      gain_fs_050[27] = 0.03055;      gain_fs_066[27] = 0.12882;      gain_fs_100[27] = 0.01820;
  ZENITH[28] = 280.0;   gain_fs_036[28] = 0.23714;      gain_fs_050[28] = 0.09772;      gain_fs_066[28] = 0.23442;      gain_fs_100[28] = 0.18621;
  ZENITH[29] = 290.0;   gain_fs_036[29] = 0.37584;      gain_fs_050[29] = 0.34674;      gain_fs_066[29] = 0.36308;      gain_fs_100[29] = 0.47315;
  ZENITH[30] = 300.0;   gain_fs_036[30] = 0.53088;      gain_fs_050[30] = 0.53088;      gain_fs_066[30] = 0.51880;      gain_fs_100[30] = 0.69183;
  ZENITH[31] = 310.0;   gain_fs_036[31] = 0.68391;      gain_fs_050[31] = 0.68391;      gain_fs_066[31] = 0.63096;      gain_fs_100[31] = 0.85114;
  ZENITH[32] = 320.0;   gain_fs_036[32] = 0.85114;      gain_fs_050[32] = 0.83176;      gain_fs_066[32] = 0.75858;      gain_fs_100[32] = 0.93325;
  ZENITH[33] = 330.0;   gain_fs_036[33] = 0.94406;      gain_fs_050[33] = 0.93325;      gain_fs_066[33] = 0.88105;      gain_fs_100[33] = 0.90157;
  ZENITH[34] = 340.0;   gain_fs_036[34] = 0.98855;      gain_fs_050[34] = 1.00000;      gain_fs_066[34] = 0.93325;      gain_fs_100[34] = 0.93325;
  ZENITH[35] = 350.0;   gain_fs_036[35] = 1.01158;      gain_fs_050[35] = 1.00000;      gain_fs_066[35] = 0.95499;      gain_fs_100[35] = 0.96605;
  ZENITH[36] = 0.0;     gain_fs_036[36] = 1.00000;      gain_fs_050[36] = 1.00000;      gain_fs_066[36] = 1.00000;      gain_fs_100[36] = 1.00000;
  
  if(zenith>360 || zenith<0){
    cerr << "GainDirectivity: zenith angle is out of range! " << endl;
    exit(1);
  }
  if(frequency>80.0){
   cerr << "GainDirectivity: upper frequency limit reached." << endl;  
   exit(1);
  }
  if(frequency<40.0){
   cerr << "GainDirectivity: lower frequency limit reached." << endl;  
   exit(1);
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
  
  return gain_value;

}

/**********************************************************************************************/

float FieldStrengthCorrection_RefAnt(float frequency){
  //derived from RefAnt_CorrValue.C
  int entries = 39;
  float freq[entries]; float CorrValue[entries];
  freq[ 0] = 41.0;      CorrValue[ 0] = 1.41216;
  freq[ 1] = 42.0;      CorrValue[ 1] = 1.43824;
  freq[ 2] = 43.0;      CorrValue[ 2] = 1.60495;
  freq[ 3] = 44.0;      CorrValue[ 3] = 1.47896;
  freq[ 4] = 45.0;      CorrValue[ 4] = 1.68266;
  freq[ 5] = 46.0;      CorrValue[ 5] = 1.85692;
  freq[ 6] = 47.0;      CorrValue[ 6] = 2.26283;
  freq[ 7] = 48.0;      CorrValue[ 7] = 2.30399;
  freq[ 8] = 49.0;      CorrValue[ 8] = 1.71221;
  freq[ 9] = 50.0;      CorrValue[ 9] = 1.96947;
  freq[10] = 51.0;      CorrValue[10] = 1.92977;
  freq[11] = 52.0;      CorrValue[11] = 2.20537;
  freq[12] = 53.0;      CorrValue[12] = 2.27525;
  freq[13] = 54.0;      CorrValue[13] = 1.54637;
  freq[14] = 55.0;      CorrValue[14] = 1.29917;
  freq[15] = 56.0;      CorrValue[15] = 1.32838;
  freq[16] = 57.0;      CorrValue[16] = 1.87616;
  freq[17] = 58.0;      CorrValue[17] = 2.54186;
  freq[18] = 59.0;      CorrValue[18] = 3.01530;
  freq[19] = 60.0;      CorrValue[19] = 2.22566;
  freq[20] = 61.0;      CorrValue[20] = 2.29222;
  freq[21] = 62.0;      CorrValue[21] = 2.72332;
  freq[22] = 63.0;      CorrValue[22] = 3.78876;
  freq[23] = 64.0;      CorrValue[23] = 2.97106;
  freq[24] = 65.0;      CorrValue[24] = 2.87738;
  freq[25] = 66.0;      CorrValue[25] = 2.65495;
  freq[26] = 67.0;      CorrValue[26] = 3.04583;
  freq[27] = 68.0;      CorrValue[27] = 3.41443;
  freq[28] = 69.0;      CorrValue[28] = 2.68651;
  freq[29] = 70.0;      CorrValue[29] = 2.50449;
  freq[30] = 71.0;      CorrValue[30] = 3.10280;
  freq[31] = 72.0;      CorrValue[31] = 11.02928;
  freq[32] = 73.0;      CorrValue[32] = 23.85014;
  freq[33] = 74.0;      CorrValue[33] = 51.10698;
  freq[34] = 75.0;      CorrValue[34] = 63.26281;
  freq[35] = 76.0;      CorrValue[35] = 55.10677;
  freq[36] = 77.0;      CorrValue[36] = 46.41801;
  freq[37] = 78.0;      CorrValue[37] = 49.72363;
  freq[38] = 79.0;      CorrValue[38] = 39.15591;
  
  if(frequency>80.0){
   cerr << "FieldStrengthCorrection: upper frequency limit reached." << endl;  
   exit(1);
  }
  if(frequency<40.0){
   cerr << "FieldStrengthCorrection: lower frequency limit reached." << endl;  
   exit(1);
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

#warning correction for f > 71 MHz
  if(frequency>71.0) correction = Mean(31,CorrValue);

  return correction;
  

  
}

/**********************************************************************************************/
