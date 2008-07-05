{
gROOT->Reset();

int files = 8;
ifstream data[files];
data[0].open("HF_Board_17/A17__1.TXT");
data[1].open("HF_Board_17/A17__2.TXT");
data[2].open("HF_Board_17/A17__3.TXT");
data[3].open("HF_Board_17/A17__4.TXT");
data[4].open("HF_Board_17/A17__5.TXT");
data[5].open("HF_Board_17/A17__6.TXT");
data[6].open("HF_Board_17/A17__7.TXT");
data[7].open("HF_Board_17/A17__8.TXT");

int entries[files];
for(int i=0; i<files; i++) entries[i] = 0;
int max_entries = 1000;
float freq[files][max_entries];
float amp[files][max_entries];

char tmp[1024];
float f_tmp;
float a_tmp;

for(int i=0; i<files; i++){
  //cout << "file = " << i << " - is_open " << data[i].is_open() << endl;
  while(strncmp("Imag",tmp,4)!=0){
    data[i] >> tmp;
    //cout << tmp << endl;
  }

  while(1){
    data[i] >> f_tmp >> a_tmp >> tmp;
    if(!data[i].good()) break;
    freq[i][entries[i]] = f_tmp;
    amp[i][entries[i]] = a_tmp;
    freq[i][entries[i]] /= 1.0e6;
    //amp[i][entries[i]] += 40.0;
    entries[i]++;
    if(entries[i]==max_entries) exit(1);
  }
//cout << "file = " << i  << " - entries " << entries[i] << endl;
}//end for

TCanvas can1 = TCanvas();
TMultiGraph mg = TMultiGraph();
mg.SetTitle("Filtercharacteristic");

TLegend leg = TLegend(0.87,0.7,0.99,0.99);

TGraph g[files];
for(int i=0; i<files; i++){
  g[i] = new TGraph (entries[i],freq[i],amp[i]);
  g[i].SetLineColor(i+1);
  sprintf(tmp,"D17 - channel %d",i);
  leg.AddEntry(&g[i],tmp,"L");
  mg.Add(&g[i]);
}

mg.Draw("AL");
mg.GetXaxis().SetTitle("f / MHz");
mg.GetYaxis().SetTitle("Amp / dB");
leg.Draw("SAME");



TCanvas can2 = TCanvas();
TMultiGraph mg2 = TMultiGraph();
mg2.SetTitle("channel 0 - channel X");

float amp2[files][max_entries];

TGraph g2[files];
for(int i=0; i<files; i++){
  for(int j=0; j<entries[i]; j++){
    amp2[i][j] = amp[0][j] - amp[i][j];
  }
  g2[i] = new TGraph(entries[i], freq[i], amp2[i]);
  g2[i].SetLineColor(i+1);
  sprintf(tmp,"channel %d",i);
  mg2.Add(&g2[i]);
}

mg2.Draw("AL");
mg2.GetXaxis().SetTitle("f / MHz");
mg2.GetYaxis().SetTitle("Amp / dB");
leg.Draw("SAME");


//HF new && LNA

int files2=2;
ifstream data2[files2];
data2[0].open("HF_Bandpass_rev02/AMPLITUDE.TXT");
data2[1].open("LNA/A_LNA.TXT");

int entries2[files2];
for(int i=0; i<files2; i++) entries2[i] = 0;
int max_entries2 = 1000;
float freq2[files2][max_entries2];
float amp3[files2][max_entries2];
TLegend leg3 = TLegend(0.797414, 0.582627,0.971264, 0.756356 );

float ampC[max_entries];

for(int i=0; i<files2; i++){
  //cout << "file = " << i << " - is_open " << data[i].is_open() << endl;
  while(strncmp("Imag",tmp,4)!=0){
    data2[i] >> tmp;
    //cout << tmp << endl;
  }

  while(1){
    data2[i] >> f_tmp >> a_tmp >> tmp;
    if(!data2[i].good()) break;
    freq2[i][entries2[i]] = f_tmp;
    amp3[i][entries2[i]] = a_tmp;
    if(i==0){
      ampC[entries2[i]] = a_tmp+3;
      ampC[entries2[i]] *= -1;
      //ampC[entries2[i]] -= 3;
    }
    freq2[i][entries2[i]] /= 1.0e6;
    entries2[i]++;
    if(entries2[i]==max_entries2) exit(1);
  }
//cout << "file = " << i  << " - entries " << entries[i] << endl;
}//end for

TCanvas can3 = TCanvas();
TMultiGraph mg3 = TMultiGraph();
mg3.SetTitle("LNA and new HF board");


TGraph g3[files2];

for(int i=0; i<files2; i++){
  g3[i] = new TGraph(entries2[i], freq2[i], amp3[i]);
  g3[i].SetLineColor(i+1);
  g3[i].SetLineWidth(2);
  if(i==0) mg3.Add(&g3[i]);
  if(i==0) sprintf(tmp,"D30-HF board - new");
  if(i==1) sprintf(tmp,"LNA channel");
  if(i==0) leg3.AddEntry(&g3[i],tmp,"L");
}

 TGraph *gC = new TGraph(entries2[0], freq2[0], ampC);
 gC->SetLineColor(4);
 gC->SetLineWidth(2);
 mg3.Add(gC);
 sprintf(tmp,"HF coeff. corrected");
 leg3.AddEntry(gC,tmp,"L");

mg3.Draw("AL");
mg3.GetXaxis().SetTitle("f / MHz");
mg3.GetYaxis().SetTitle("Amp / dB");
leg3.Draw("SAME");

//all HF boards together
mg.Add(&g3[0]);
leg.AddEntry(&g3[0],"new rev.","L");
can1.Update();

int r=0;
while(1){
  if(freq2[0][r]>30) break;
  r++;
  if(r>max_entries) break;
}
int r0 = r;
while(1){
  printf("  freq[%i] = %f; \t amp[%i] = %f;\n",r-r0,freq2[0][r], r-r0,TMath::Power(10,ampC[r]/10));
//  cout << "freq[" << r-r0 << "] = " << freq2[0][r] << ";\t" << "amp[" << r-r0 << "] = " << ampC[r] << ";" << endl;
  if(freq2[0]	[r]>90) break;
  r++;
  if(r>max_entries) break;
}

}
