#include "TH2.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TNtuple.h"
//#include "MyObject_newdata.h"
#include "TMath.h"
#include "TSpectrum.h"
#include "TStopwatch.h"
#include "TVirtualFFT.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

void Fourier(Double_t a=0.004, Double_t b=0.00004)
{

//delete Trans;
//delete Raw;
//delete fft;

TVirtualFFT *fft;
TH1D *Raw, *Trans, *Phase;
TF1 *dist;
Int_t n[1];

n[0]=20000;

Double_t Re, Im;

fft = TVirtualFFT::FFT(1, n, "R2C M");
Trans = new TH1D("Trans","Trans",10000,0,10000);
Raw = new TH1D("Raw","Raw",20000,0,20000);
Phase = new TH1D("Phase","Phase",10000,0,10000);
dist = new TF1("dist","x*gaus(0)",0,100);

//c1 = new TCanvas("c1","The FillRandom example",200,10,700,900);
 
//for (int i=0;i<1000;i++) {fft->SetPoint(i,TMath::Sin((a-b*i)*i));}

for (int i=0;i<20000;i++) {Raw->SetBinContent(i,0);}
for (int i=0;i<10000;i+=5) {Raw->SetBinContent(i,1./(i+1.));}
for (int i=10000;i<20000;i+=5) {Raw->SetBinContent(i,1./((19995.-i)+1.));}
//for (int i=440;i<561;i++) {Raw->SetBinContent(i,TMath::Exp(-(i-500)*(i-500)/1500.));}

for (int i=0;i<20000;i++) {fft->SetPoint(i,Raw->GetBinContent(i));}
//for (int i=440;i<561;i++) {fft->SetPoint(i,TMath::Exp(-(i-500)*(i-500)/1500.));}


fft->Transform();

for (int i=0; i<10000;i++) {fft->GetPointComplex(i,Re,Im);
                          Trans->SetBinContent(i,TMath::Sqrt(Re*Re+Im*Im));
			  Phase->SetBinContent(i,atan2(Im,Re));}     


Raw->Draw();
//Trans->Draw("same");	
};

    
