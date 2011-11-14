{
gROOT->Reset();

int files = 1;
ifstream data[files];
data[0].open("RefCoef/S11_TypAachen.TXT");

int entries[files];
for(int i=0; i<files; i++) entries[i] = 0;
int max_entries = 1000;
float freq[files][max_entries];
float amp[files][max_entries];
float amp2[files][max_entries];

char tmp[1024];
double f_tmp = 1;
double a_tmp = 1;

for(int i=0; i<files; i++){
//  cout << "file = " << i << " - is_open " << data[i].is_open() << endl;
  while(strncmp("Imag",tmp,4)!=0){
    data[i] >> tmp;
//    cout << tmp << endl;
  }

  while(1){
    data[i] >> f_tmp >> a_tmp >> tmp >> tmp >> tmp;
//cout << f_tmp << " - " << a_tmp << endl;
    if(!data[i].good()) break;
    freq[i][entries[i]] = f_tmp;
    amp[i][entries[i]] = a_tmp;
    amp2[i][entries[i]] = 10*TMath::Log10(1-TMath::Power(TMath::Abs(TMath::Power(10,a_tmp/20.0)),2));
    freq[i][entries[i]] /= 1.0e6;
    entries[i]++;
    if(entries[i]==max_entries) exit(1);
  }
//cout << "file = " << i  << " - entries " << entries[i] << endl;
}//end for

TCanvas can3 = TCanvas();
TMultiGraph *mg4 = new TMultiGraph();
mg4->SetTitle("refelction coefficient");
TLegend leg4 = TLegend(0.87,0.7,0.99,0.99);

TGraph gRC[2];

  gRC[0] = new TGraph (entries[0],freq[0],amp[0]);
//  gRC[0].GetXaxis()->SetTitle("f / MHz");
//  gRC[0].GetYaxis()->SetTitle("S11 / dB");
  gRC[0].SetLineColor(1);
  leg4.AddEntry(&gRC[0],"S11 - D30","L");
  mg4->Add(&gRC[0]);
  
  gRC[1] = new TGraph (entries[0],freq[0],amp2[0]);
//  gRC[1].GetXaxis()->SetTitle("f / MHz");
//  gRC[1].GetYaxis()->SetTitle("S11 / dB");
  gRC[1].SetLineColor(4);
  leg4.AddEntry(&gRC[1],"transformed - D30","L");
  mg4->Add(&gRC[1]);


mg4->Draw("AL");
mg4->GetXaxis()->SetTitle("f / MHz");
mg4->GetYaxis()->SetTitle("Amp / dB");
leg4.Draw("SAME");

}
