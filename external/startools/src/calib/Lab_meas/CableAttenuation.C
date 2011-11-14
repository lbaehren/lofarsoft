/**********************************************************************************************/

float CableAttenuationRG213(float length, float frequency){
  float attenuation = 0;
  
/*  if(frequency>80.0){
   cerr << "CableAttenuationRG213: upper frequency limit reached." << endl;  
   exit(1);
  }
  if(frequency<40.0){
   cerr << "CableAttenuationRG213: lower frequency limit reached." << endl;  
   exit(1);
  }
*/
  int entries = 5;
  float freq[5], atten[5];
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

  return attenuation;
}

/**********************************************************************************************/

float CableAttenuationRG058(float length, float frequency){
  float attenuation = 0;
  
/*  if(frequency>80.0){
   cerr << "CableAttenuationRG058: upper frequency limit reached." << endl;  
   exit(1);
  }
  if(frequency<40.0){
   cerr << "CableAttenuationRG058: lower frequency limit reached." << endl;  
   exit(1);
  }
*/
  int entries = 5;
  float freq[5], atten[5];
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

  return attenuation;
}

/**********************************************************************************************/





void CableAttenuation(){
  
  int entries = 400;
  float freq[400], atten_058[400], atten_213[400];
  
  for(int i=0; i<entries; i++){
    freq[i] = (float)(190.0 / (float)entries * i) + 10;
    atten_058[i] = CableAttenuationRG058(10, freq[i]);
    atten_213[i] = CableAttenuationRG213(100, freq[i]);
  }
  
  TGraph *g_058 = new TGraph(entries, freq, atten_058);
  g_058->SetTitle("RG058 attenaution @ 10m");
  g_058->GetXaxis()->SetTitle("f / MHz");
  g_058->GetYaxis()->SetTitle("attenuation / dB");  

  TGraph *g_213 = new TGraph(entries, freq, atten_213);
  g_213->SetTitle("RG213 attenaution @ 100m");
  g_213->GetXaxis()->SetTitle("f / MHz");
  g_213->GetYaxis()->SetTitle("attenuation / dB");  
  
  TCanvas *can1 = new TCanvas("can1");
  g_058->Draw("AL");
  TCanvas *can2 = new TCanvas("can2");
  g_213->Draw("AL");


}
