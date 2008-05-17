{
gROOT->Reset();
//gStyle->SetPalette(black,white);
gStyle->SetPalette(1);
gStyle->SetOptStat(0);


// beamforming output file:
TFile *file = new TFile("/home/ipefser1/asch/analysis/beamforming/muell7.root","READ");

char name[1024];
int NoHis;
TTree *tree = (TTree*)file->Get("info");
tree->SetBranchAddress("NoHis",&NoHis);
tree->GetEntry(0);

cout << "found number of histogramms: " << NoHis << endl;

TCanvas *can = new TCanvas();
TH2F **his = new TH2F[NoHis];
for(int i=0; i<NoHis; i++){
  sprintf(name,"his%03i",i);
  his[i] = (TH2F*)file->Get(name);
}

//define fix scaling
/*
double max = 0;
double tmax;
for(int i=0; i<NoHis; i++){
 tmax = his[i]->GetMaximum();
 if(tmax>max){
   max = tmax;
   tmax = 0;
 }
}
cout << max << endl;
*/

for(int i=0; i<NoHis; i++){
//  his[i]->SetMaximum(max);
//  his[i]->SetMinimum(0);
  his[i]->Draw("colz");
  RecDirection->Draw("lpsame");
  sprintf(name,"/tmp/bf_%06i.gif",i);
  can->Print(name);
}

gSystem->Exec("convert -adjoin -delay 10 -loop 100 /tmp/bf_*.gif  $HOME/analysis/BF_animated.gif");
gSystem->Exec("rm -f /tmp/bf_*.gif");
}
