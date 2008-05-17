{
gROOT->Reset();
cout << "hallo Thomas" << endl;

TH1F *his = new TH1F("his","his",100,-3,3);

his->FillRandom("gaus",10000);

TFile *file = new TFile("/dev/stdout","UPDATE");

file->cd();
his->Write();
file->Close();

}
