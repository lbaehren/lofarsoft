{
gROOT->Reset();

int bins=400;
int whiteNoiseLimit = 20;
int content;

TCanvas *can = new TCanvas ("can","can");
can->Divide(2,1);

TH1D *whiteNoise = new TH1D ("whiteNoise","whiteNoise",bins,whiteNoiseLimit*-1,whiteNoiseLimit);
for(int i=0; i<100000; i++) whiteNoise->Fill(gRandom->Gaus(0,4));
can->cd(1);
whiteNoise->Draw();



TH1D *power = new TH1D ("power","power",(int)(bins),0,whiteNoiseLimit*whiteNoiseLimit);
int fillpower;
for(int i=1; i<bins+1;i++){
  content = whiteNoise->GetBinContent(i);
  if(content>0){
    fillpower = (-1*whiteNoiseLimit)+(i*whiteNoiseLimit*2/bins);
    for(int j=0; j<content; j++) power->Fill(fillpower*fillpower);
  }
}

can->cd(2);
power->Draw();
double IntLimit;
double IntAll;
int Limit=100;
IntLimit = power->Integral(1,Limit*bins/(whiteNoiseLimit*whiteNoiseLimit));
IntAll = power->Integral();

cout << "Limit = " << Limit << endl;
cout << "Integral to limit = " << IntLimit << endl;
cout << "Integral over all = " << IntAll << endl;
cout << "ratio = " << IntLimit/IntAll << endl;

}
