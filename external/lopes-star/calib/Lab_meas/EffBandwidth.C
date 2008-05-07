{
gROOT->Reset();

int LineWidth = 2;

ifstream data;
data.open("FreqResp/HF_Filter_Amp.txt");

int entries=0;
int max_entries = 1000;
float freq[max_entries];
float amp[max_entries];
float amp_approx[max_entries];

char tmp[1024];
float f_tmp;
float a_tmp;

float CalibStartFreq = 40.;
float CalibStopFreq = 80.;

  sprintf(tmp,"null");
  while(strncmp("Imag",tmp,4)!=0){
    data >> tmp;
//cout << tmp << endl;
  }

  while(1){
    data >> f_tmp >> a_tmp;
//    cout << f_tmp ;
    if(!data.good()) break;
    freq[entries] = f_tmp;
    amp[entries] = a_tmp+3; //TMath::Power(10.,(a_tmp)/20);
    freq[entries] /= 1.0e6;
    amp_approx[entries] = amp[entries];
    if( freq[entries]>CalibStartFreq && freq[entries]<CalibStopFreq ) amp_approx[entries] = 0;
    entries++;
    if(entries==max_entries) exit(1);
  }



TCanvas *can1 = new TCanvas("can1");
TLegend *leg = new TLegend(0.738506, 0.78178, 0.95977, 0.966102);

TMultiGraph *mg = new TMultiGraph();
mg->SetTitle("Calculation of effective Bandwidth #Deltaf");

TGraph *graph_filter = new TGraph(entries,freq,amp);
graph_filter->SetLineWidth(LineWidth);
graph_filter->SetLineColor(1);
graph_filter->SetLineStyle(2);
leg->AddEntry(graph_filter,"band filter","L");
mg->Add(graph_filter);

TGraph *graph_approx = new TGraph(entries,freq,amp_approx);
graph_approx->SetLineWidth(LineWidth);
graph_approx->SetLineColor(2);
leg->AddEntry(graph_approx,"calibrated","L");
mg->Add(graph_approx);

Double_t ADClsb = -1*20*TMath::Log10(TMath::Power(2.,12));
cout << "LSB threshold = " << ADClsb << " dB" << endl;
TLine *ADC = new TLine(10.,ADClsb,110.,ADClsb);
ADC->SetLineWidth(LineWidth);
ADC->SetLineColor(27);
leg->AddEntry(ADC,"LSB","L");

mg->Draw("AL");
mg->GetXaxis()->SetTitle(" frequency / MHz ");
mg->GetXaxis()->SetRangeUser(10.,110.);
mg->GetYaxis()->SetTitle(" gain_{rel.} / dB ");
mg->GetYaxis()->CenterTitle();

leg->Draw("SAME");
ADC->Draw("SAME");

int EntLower = 0;
int EntUpper = 0;
float FreqLower[max_entries];
float FreqUpper[max_entries];
float AmpLower[max_entries];
float AmpUpper[max_entries];

FreqUpper[EntLower] = -1;
AmpUpper[EntUpper] = ADClsb;
EntUpper++;

double integral = 0;
double DeltaFreq = 0;
int tmpCount = 0;
for(int i=0; i<entries; i++){
  if( abs(amp_approx[i]) < abs(ADClsb) ){
    integral += TMath::Power(10.,amp_approx[i]/20);
    DeltaFreq += (freq[i+1] - freq[i]);
    tmpCount ++;
    if(freq[i]<CalibStartFreq){
      FreqLower[EntLower] = freq[i];
      AmpLower[EntLower] = amp_approx[i];
      EntLower++;
    }
    if(!(freq[i]<CalibStopFreq)){
      FreqUpper[EntUpper] = freq[i];
      AmpUpper[EntUpper] = amp_approx[i];
      EntUpper++;
    }
    
    
  }
}
DeltaFreq /= tmpCount;

cout << "effective bandwidth  = " << integral*DeltaFreq <<  endl;

FreqUpper[0] = FreqUpper[1];
FreqLower[EntLower] = FreqLower[EntLower-1];
AmpLower[EntLower] = ADClsb;
EntLower++;

FreqUpper[EntUpper] = 90.;
AmpUpper[EntUpper] = ADClsb;
EntUpper++;

TGraph *lower = new TGraph(EntLower,FreqLower,AmpLower);
lower->SetLineWidth(LineWidth);
lower->SetFillColor(2);
lower->SetFillStyle(3004);
lower->Draw("Fsame");

TGraph *upper = new TGraph(EntUpper,FreqUpper,AmpUpper);
upper->SetLineWidth(LineWidth);
upper->SetFillColor(2);
upper->SetFillStyle(3005);
upper->Draw("Fsame");

char tmpChar[128];
TPaveText BandWidth(0.343391,0.322034, 0.650862, 0.417373 ,"NDC");
sprintf(tmpChar,"#Deltaf_{eff} = %.2f MHz",integral*DeltaFreq);
BandWidth.AddText(tmpChar);
BandWidth.SetFillColor(10);
BandWidth.Draw("same");

}
