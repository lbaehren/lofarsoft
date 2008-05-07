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


void FrequencyResponse(){
//gROOT->Reset();

int files = 3;

bool bAbsoluteValue = true;


ifstream data[3];
data[0].open("FreqResp/RefCoeff_Ant.txt");
data[1].open("FreqResp/HF_Filter_Amp.txt");
data[2].open("FreqResp/LNA_Amp.txt");

int entries[3];
for(int i=0; i<files; i++) entries[i] = 0;
int max_entries = 1000;
float freq[4][1000];
float amp[3][1000];
float amp_corr[1000];
float cable[1000];

char tmp[1024];
float f_tmp;
float a_tmp;


for(int i=0; i<files; i++){
//  cout << "file = " << i << " - is_open " << data[i].is_open() << endl;
  sprintf(tmp,"null");
  while(strncmp("Imag",tmp,4)!=0){
    data[i] >> tmp;
//cout << tmp << endl;
  }

  while(1){
    data[i] >> f_tmp >> a_tmp;
//    cout << f_tmp ;
    if(!data[i].good()) break;
    freq[i][entries[i]] = f_tmp;
    amp[i][entries[i]] = a_tmp;
    freq[i][entries[i]] /= 1.0e6;
    entries[i]++;
    if(entries[i]==1000) exit(1);
  }
cout << "file = " << i  << " - entries " << entries[i] << endl;
}//end for

//REF COEF
for(int i=0; i<entries[0]; i++){
  amp[0][i] = 10*TMath::Log10(1-TMath::Power(TMath::Abs(TMath::Power(10,amp[0][i]/20.0)),2));
}

//LNA
if(!bAbsoluteValue)
for(int i=0; i<entries[2]; i++){
  amp[2][i] -= 22;
}

//BAND FILTER
for(int i=0; i<entries[1]; i++){
  if(!bAbsoluteValue) amp[1][i] += 3;
  else amp[1][i] += 40;
}

//cable attenuation
for(int i=0; i<entries[0]; i++){
  freq[3][i] = freq[0][i];
  cable[i]   = -1 * (CableAttenuationRG058(4., freq[3][i]) + CableAttenuationRG213(100., freq[3][i]) );
}

for(int i=0; i<entries[0]; i++){
  amp_corr[i] = 0;
  amp_corr[i] = amp[0][i] + amp[2][i] + cable[i];
 
    for(int j=0; j<entries[1]; j++){
      if(freq[1][j]>freq[0][i] && freq[0][i]<149){
        //linear interpolation
        amp_corr[i] += (( (amp[1][j-1]-amp[1][j]) / (freq[1][j-1]-freq[1][j]))*(freq[0][i]-freq[1][j])+amp[1][j]);
	//cout << amp_corr[i] << " - " << freq[0][i] << " - " << freq[1][j] << endl;
	break;
      }
    }
}



TCanvas *can1 = new TCanvas();
TLegend *leg = new TLegend(0.683908, 0.745763, 0.974138, 0.955508);

TGraph **graph = new TGraph *[3];

TGraph *graph_corr = new TGraph(entries[0],freq[0],amp_corr);
graph_corr->SetLineWidth(2);
graph_corr->SetTitle("Hardware Dependencies of LOPES^{STAR}");
graph_corr->GetXaxis()->SetTitle(" frequency / MHz ");
graph_corr->GetXaxis()->SetRangeUser(10.,110.);
if(!bAbsoluteValue) graph_corr->GetYaxis()->SetTitle(" relative gain / dB ");
else graph_corr->GetYaxis()->SetTitle(" gain / dB ");
graph_corr->GetYaxis()->CenterTitle();

leg->AddEntry(graph_corr,"sum of all","L");

TGraph *graph_cable = new TGraph(entries[0], freq[3], cable);
graph_cable->SetLineWidth(2);
graph_cable->SetLineColor(6);
leg->AddEntry(graph_cable,"cable attenuation","L");

for(int i=0; i<files; i++){
  graph[i] = new TGraph(entries[i],freq[i],amp[i]);
  graph[i]->SetLineColor(i+1);
  graph[i]->SetLineWidth(2);
  if(i==0){
   sprintf(tmp,"antenna insertion loss");
   graph[i]->SetLineColor(1);
   graph[i]->SetLineStyle(2);
  }
  if(i==1){
   sprintf(tmp,"RF bandpass");
   graph[i]->SetLineColor(4);
  }
  if(i==2) {
   sprintf(tmp,"LNA");
   graph[i]->SetLineColor(35);
  }
  leg->AddEntry(graph[i],tmp,"L");
  if(i==1){
    graph[i]->SetTitle("Hardware Dependencies");
    graph[i]->GetXaxis()->SetTitle(" frequency / MHz ");
    if(!bAbsoluteValue) graph[i]->GetYaxis()->SetTitle(" relative gain / dB ");
    else graph[i]->GetYaxis()->SetTitle(" gain / dB ");
    graph[i]->GetYaxis()->CenterTitle();
  }
}


graph_corr->Draw("AL");
graph[1]->Draw("SAMEL");
graph[0]->Draw("SAMEL");
graph[2]->Draw("SAMEL");
graph_cable->Draw("SAMEL");
leg->Draw("SAME");

int noentries=0;
double amp_corr2[1000];
for(int i=0; i<entries[0]; i++){
  if(freq[0][i]>40 && freq[0][i]<80){
    amp_corr2[noentries] = TMath::Power(10,amp_corr[i]/10.0);
    noentries++;
  }
}
cout << "entries = " << noentries << endl;
cout << "Mean = " << TMath::Mean(noentries,amp_corr2) << endl;
cout << "RMS = " << TMath::RMS(noentries,amp_corr2) << endl;


}
