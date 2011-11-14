{
// gROOT->Reset();
 
ifstream inputfile;
inputfile.open("RefAnt_FieldStrength.txt");
int entries = 41;
double freq[entries];
double FS_orig[entries]; // Field Strength given by the datasheet
double FS_10[entries]; // Field Strength at 10m -> convert dB
double FS_LPDA[entries]; // Field Strength at LPDA, 11,275m  from the RefAnt
for(int i=0; i<entries; i++){
 freq[i]=0;
 FS_orig[i]=0;
 FS_10[i]=0;
 
 inputfile >> freq[i] >> FS_orig[i];
 
 FS_10[i] = pow(10,FS_orig[i]/20.0);
 FS_LPDA[i] = FS_10[i] * 10.0 / 11.275;
}

TCanvas *can_orig = new TCanvas("orig");
TGraph *graph_orig = new TGraph(entries,freq,FS_orig);
graph_orig->SetTitle("Field Strength of Ref. Source @ 10m");
graph_orig->GetXaxis()->SetTitle("f / MHz");
graph_orig->GetYaxis()->SetTitle("|E| / ( dB#muV #upoint m^{-1} )");
graph_orig->GetYaxis()->CenterTitle();
graph_orig->Draw("AL");

TCanvas *can_10 = new TCanvas("10");
TGraph *graph_10 = new TGraph(entries,freq,FS_10);
graph_10->SetTitle("Field Strength of Ref. Source @ 10m");
graph_10->GetXaxis()->SetTitle("f / MHz");
graph_10->GetYaxis()->SetTitle("|E| / ( #muV #upoint m^{-1} )");
graph_10->GetYaxis()->CenterTitle();
graph_10->Draw("AL");

TCanvas *can_LPDA = new TCanvas("LPDA");
TGraph *graph_LPDA = new TGraph(entries,freq,FS_LPDA);
graph_LPDA->SetTitle("Field Strength of Ref. Source @ LPDA");
graph_LPDA->GetXaxis()->SetTitle("f / MHz");
graph_LPDA->GetYaxis()->SetTitle("|E| / ( #muV #upoint m^{-1} )");
graph_LPDA->GetYaxis()->CenterTitle();
graph_LPDA->Draw("AL");

bool code_output = false; // output of C code for implementation of the amp. calibration

if(code_output){
 cout << "  int entries = " << entries << ";" << endl;
 cout << "  double freq[entries], FS_LPDA[entries]; "<< endl << endl;
 for(int i=0; i<entries; i++){
  printf("  freq[%2i] = %3.1f; \t FS_LPDA[%2i] = %3.5f;\n",i,freq[i],i,FS_LPDA[i]);
 }
}


}

