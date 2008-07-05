// fitting script for the TRecEvent Tree results

#include <iostream>
#include <math.h>

#include <TROOT.h>
#include <TFile.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TEventList.h>
#include <TMath.h>
#include <TMinuit.h>
//#include "TMinuit.h"
#include "util.hh"


using namespace std;


//global fit data pointers
const int gMaxCh=20;
const int gMaxEvents=200;
int gEntries = gMaxCh * gMaxEvents;
float gFieldStrengthNS[4000], gFieldStrengthEW[4000], gFieldStrengthAnt[4000];
float gFieldStrengthNSErr[4000], gFieldStrengthEWErr[4000], gFieldStrengthErr[4000];
float gAzimuth[4000];
float gBAngle[4000];



void CreateTRecEventChain30(TChain *TRecEvent)
{
    TRecEvent->AddFile("~/analysis/AnaFlag/AF_D30_run21-49_v28.root");
    TRecEvent->AddFile("~/analysis/AnaFlag/AF_D30_run50-83_v28.root");
   TRecEvent->AddFile("~/analysis/AnaFlag/AF_D30_run84-136_v28.root");
  TRecEvent->AddFile("~/analysis/AnaFlag/AF_D30_run137-234_v28.root");
  TRecEvent->AddFile("~/analysis/AnaFlag/AF_D30_run235-283_v28.root");
  TRecEvent->AddFile("~/analysis/AnaFlag/AF_D30_run284-331_v28.root");
  TRecEvent->AddFile("~/analysis/AnaFlag/AF_D30_run332-393_v28.root");
  TRecEvent->AddFile("~/analysis/AnaFlag/AF_D30_run394-454_v28.root");

}

//______________________________________________________________________________

void CreateTRecEventChain17(TChain *TRecEvent)
{
    TRecEvent->AddFile("~/analysis/AnaFlag/AF_D17_run12-60_v28.root");
   TRecEvent->AddFile("~/analysis/AnaFlag/AF_D17_run61-123_v28.root");
  TRecEvent->AddFile("~/analysis/AnaFlag/AF_D17_run124-185_v28.root");

}

//______________________________________________________________________________

Double_t FuncPol1(float x, Double_t *par)
{
 Double_t value =  par[0] + cos( x*TMath::Pi()/180. + par[1] )*par[2] ;
 return value;
}

//______________________________________________________________________________

Double_t FuncPol2(float x,Double_t *par)
{
 Double_t value =  par[0] + ( pow(sin( x*TMath::Pi()/180. + par[1] ),par[4]) * pow(cos(x*TMath::Pi()/180. + par[2]),par[5])) * par[3] ;
 return value;
}

//______________________________________________________________________________

void FcnPol(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
   //calculate chisquare
   Double_t chisq = 0;
   Double_t delta;
   Double_t ratio;
   Double_t SigmaAnt;
   Double_t sigma;
   Double_t dtmp;
   for (int i=0;i<gEntries; i++) {
     gFieldStrengthErr[i] = -1;
     if(gFieldStrengthAnt[i] == 0){
       ratio = -1;
     }
     else{
       ratio = (gFieldStrengthEW[i]/gFieldStrengthAnt[i]);
       dtmp  =  powf(gFieldStrengthNS[i],2.) + powf(gFieldStrengthEW[i],2.);
       SigmaAnt = sqrt( powf(gFieldStrengthNS[i]*gFieldStrengthNSErr[i],2.)/dtmp + powf(gFieldStrengthEW[i]*gFieldStrengthEWErr[i],2.)/dtmp );
       sigma = sqrt( powf(1./gFieldStrengthAnt[i]*gFieldStrengthEWErr[i],2.) + powf(gFieldStrengthEW[i]*SigmaAnt/gFieldStrengthAnt[i]/gFieldStrengthAnt[i],2.) );
       
       gFieldStrengthErr[i] = sigma;
     }
     
     if(ratio > -1){
       delta  = ( ratio - FuncPol1(gAzimuth[i], par)) / sigma;
       chisq += delta*delta;
     }
   }
   f = chisq;
}

//______________________________________________________________________________

int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();
  gROOT->SetStyle("Plain");

  static struct RecEvent RecEvent30;
  static struct RecEvent RecEvent17;
  static struct channel_profiles channel_profiles30;
  static struct channel_profiles channel_profiles17;
  static struct h3 h330;
  static struct h3 h317;

  //get pointer to data
  TChain *TRecEvent30 = new TChain ("TRecEvent");
  CreateTRecEventChain30(TRecEvent30);
  cout << "TRecEvent entries = " << TRecEvent30->GetEntries() << endl;
  CreateTRecEvent(TRecEvent30, &RecEvent30);
  CreateTGrande(TRecEvent30, &h330, false);
  
  TChain *TRecEvent17 = new TChain ("TRecEvent");
  CreateTRecEventChain17(TRecEvent17);
  cout << "TRecEvent entries = " << TRecEvent17->GetEntries() << endl;
  CreateTRecEvent(TRecEvent17, &RecEvent17);
  CreateTGrande(TRecEvent17, &h317, false);

  char cut[2048];
  sprintf(cut, "Eg>0");
  sprintf(cut, "abs(Zenith1Third)+3*Zenith1Third_err>Zeg && abs(Zenith1Third)-3*Zenith1Third_err<Zeg");
  cout << "cut = " << cut << endl;
  TRecEvent30->Draw(">>ListRecEvent30",cut);
  TEventList *EventList30 = (TEventList*) gDirectory->Get("ListRecEvent30");
  if(EventList30==NULL) cerr << "error - EventList30 has null entries" << endl;
  TRecEvent30->SetEventList(EventList30);
  cout << "30: events to analyse = " <<  EventList30->GetN() << endl;
  
  TRecEvent17->Draw(">>ListRecEvent17",cut);
  TEventList *EventList17 = (TEventList*) gDirectory->Get("ListRecEvent17");
  if(EventList17==NULL) cerr << "error - EventList17 has null entries" << endl;
  TRecEvent17->SetEventList(EventList17);
  cout << "17: events to analyse = " <<  EventList17->GetN() << endl;
  
  TChain *Tchannel_profile30 = new TChain("Tchannel_profile");
  int NoChannels30;
  Tchannel_profile30->AddFile("~/analysis/AnaFlag/AF_D30_run21-49_v28.root");
  CreateTchannel_profiles(Tchannel_profile30, &channel_profiles30);
  NoChannels30 = Tchannel_profile30->GetEntries();
  cout << "Tchannel_profile30 entries = " << NoChannels30 << endl;

  TChain *Tchannel_profile17 = new TChain("Tchannel_profile");
  int NoChannels17;
  Tchannel_profile17->AddFile("~/analysis/AnaFlag/AF_D17_run12-60_v28.root");
  CreateTchannel_profiles(Tchannel_profile17, &channel_profiles17);
  NoChannels17 = Tchannel_profile17->GetEntries();
  cout << "Tchannel_profile17 entries = " << NoChannels17 << endl;
  
  
  gEntries = 0;
  float errScale = 1;
  
  bool bNS = false, bEW = false;
  //fill data pointers
  for(int i=0; i<EventList30->GetN(); i++){
    TRecEvent30->GetEntry(EventList30->GetEntry(i));
    bNS = false;
    bEW = false;
    for(int k=0; k<RecEvent30.DetCh; k++){
      for(int j=0; j<NoChannels30; j++){
        Tchannel_profile30->GetEntry(j);
	if((int)channel_profiles30.ch_id == RecEvent30.channel_id[k]){
	  if( strncmp(channel_profiles30.polarisation,"East",4) == 0 ){
	    bEW = true;
	    gFieldStrengthEW[gEntries]    = RecEvent30.FieldStrengthChannel[k];
	    gFieldStrengthEWErr[gEntries] = RecEvent30.ChannelErrBackground[k]*errScale;
	    gFieldStrengthAnt[gEntries]   = RecEvent30.FieldStrengthAntenna[k];
	  }
	  else{
	    bNS = true;
	    gFieldStrengthNS[gEntries]    = RecEvent30.FieldStrengthChannel[k];
	    gFieldStrengthNSErr[gEntries] = RecEvent30.ChannelErrBackground[k]*errScale;
	    gFieldStrengthAnt[gEntries]   = RecEvent30.FieldStrengthAntenna[k];
	  }
	  if(bNS && bEW){
	    gAzimuth[gEntries] = h330.Azg;
            while(gAzimuth[gEntries]>360) gAzimuth[gEntries] -= 360;
	    gBAngle[gEntries] = RecEvent30.BAngle;
            while(gBAngle[gEntries]>360) gBAngle[gEntries] -= 360;
	    
	    
	   if(0){
              cout << gAzimuth[gEntries] << " -- " << gBAngle[gEntries] << endl;
              cout << channel_profiles30.polarisation << endl;
              cout << gFieldStrengthNS[gEntries] << " -- " << gFieldStrengthEW[gEntries] <<  " ----- " << gFieldStrengthNS[gEntries]/gFieldStrengthEW[gEntries] << endl;
	      cout << gFieldStrengthAnt[gEntries] << endl;
              cout << h330.Zeg << " -- " << RecEvent30.Zenith1Third << " -- " << RecEvent30.Zenith1Third_err << endl;
              cout << endl;
            }

	    gEntries++;
	    bNS = false;
	    bEW = false;
	  }
	  break;
	}
      }
    }
   }

  if(true)
  for(int i=0; i<EventList17->GetN(); i++){
    TRecEvent17->GetEntry(EventList17->GetEntry(i));
    bNS = false;
    bEW = false;
    for(int k=0; k<RecEvent17.DetCh; k++){
      for(int j=0; j<NoChannels17; j++){
        Tchannel_profile17->GetEntry(j);
	if((int)channel_profiles17.ch_id == RecEvent17.channel_id[k]){
	  if( strncmp(channel_profiles17.polarisation,"East",4) == 0 ){
	    bEW = true;
	    gFieldStrengthEW[gEntries]    = RecEvent17.FieldStrengthChannel[k];
	    gFieldStrengthEWErr[gEntries] = RecEvent17.ChannelErrBackground[k]*errScale;
	    gFieldStrengthAnt[gEntries]   = RecEvent17.FieldStrengthAntenna[k];
	  }
	  else{
	    bNS = true;
	    gFieldStrengthNS[gEntries]    = RecEvent17.FieldStrengthChannel[k];
	    gFieldStrengthNSErr[gEntries] = RecEvent17.ChannelErrBackground[k]*errScale;
	    gFieldStrengthAnt[gEntries]   = RecEvent17.FieldStrengthAntenna[k];
	  }
	  if(bNS && bEW){
	    gAzimuth[gEntries] = h317.Azg;
            while(gAzimuth[gEntries]>360) gAzimuth[gEntries] -= 360;
	    gBAngle[gEntries] = RecEvent17.BAngle;
            while(gBAngle[gEntries]>360) gBAngle[gEntries] -= 360;
	    
	    
	   if(0){
	      cout << "index = " << i << endl;
              cout << gAzimuth[gEntries] << " -- " << gBAngle[gEntries] << endl;
              cout << channel_profiles17.polarisation << endl;
              cout << gFieldStrengthNS[gEntries] << " -- " << gFieldStrengthEW[gEntries] <<  " ----- " << gFieldStrengthNS[gEntries]/gFieldStrengthEW[gEntries] << endl;
	      cout << gFieldStrengthAnt[gEntries] << endl;
              cout << h317.Zeg << " -- " << RecEvent17.Zenith1Third << " -- " << RecEvent17.Zenith1Third_err << endl;
              cout << endl;
            }

	    gEntries++;
	    bNS = false;
	    bEW = false;
	  }
	  break;
	}
      }
    }
   }

   cout << gEntries << endl;
   if(NoChannels30+NoChannels17 > gMaxCh || gEntries > gMaxEvents) {
     cout << "constants limit reached" << endl;
     exit(1);
   }
 
   TMinuit *gMinuit = new TMinuit(10);
   gMinuit->SetPrintLevel(-1);
   gMinuit->SetFCN(FcnPol);

   Double_t arglist[10];
   Int_t ierflg = 0;

// no effect
  arglist[0] = 1;
  gMinuit->mnexcm("SET ERR", arglist ,1,ierflg);

  arglist[0] = 1;
  gMinuit->mnexcm("SET STRATEGY", arglist, 1, ierflg);


  if(1){ //FuncPol1
   gMinuit->mnparm(0, "offset", 0.5, 0.01, 0, 0, ierflg);
   gMinuit->mnparm(1, "PhAzi",  0., 0.01, 0, 0, ierflg);
   gMinuit->mnparm(2, "scale",  0.5, 0.01, 0, 0, ierflg);
//   gMinuit->FixParameter(0);
//   gMinuit->FixParameter(1);
//   gMinuit->FixParameter(2);
//   gMinuit->FixParameter(3);
  }

  if(0){ //FuncPol2
   gMinuit->mnparm(0, "shift",  0., 0.01, 0, 0, ierflg);
   gMinuit->mnparm(1, "phaseS", 0., 0.01, 0, 0, ierflg);
   gMinuit->mnparm(2, "phaseC", 1., 0.01, 0, 0, ierflg);
   gMinuit->mnparm(3, "scale",  1., 0.01, 0, 0, ierflg);
   gMinuit->mnparm(4, "powS",   1., 0.01, 0, 0, ierflg);
   gMinuit->mnparm(5, "powC",  -1., 0.01, 0, 0, ierflg);
   gMinuit->FixParameter(0);
   gMinuit->FixParameter(4);
   gMinuit->FixParameter(5);
  }
   
   //scan par space
   for(int i=0; i<5; i++){
     gMinuit->mnexcm("SCA", arglist, 0, ierflg);
   }
   
// Now ready for minimization step
   arglist[0] = 5000; // steps of iteration
   arglist[1] = 1.; // tolerance
   gMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);
   cout << " MIGRAD flag is " << ierflg << endl;
   if(ierflg!=0){
     cout << "+++++++++++++++++++++" << endl;
     cout << "+ ups.. check again +" << endl;
     cout << "+++++++++++++++++++++" << endl;
   }

   // Minos: error optimiser
   if(ierflg==0){
     arglist[0] = 5000;
     gMinuit->mnexcm("MINOS", arglist ,1,ierflg);
     cout << " MINOS flag is " << ierflg << endl;
     if(ierflg!=0){
       cout << "+++++++++++++++++++++" << endl;
       cout << "+ ups.. check again +" << endl;
       cout << "+++++++++++++++++++++" << endl;
     }
   }

// Print results
   Double_t amin,edm,errdef;
   Int_t nvpar,nparx,icstat;
   gMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
   gMinuit->mnprin(3,amin);
 
   cout << "prob = " << TMath::Prob(amin,gEntries-gMinuit->GetNumFreePars())*100 << " %" << endl;

   
   cout << "output" << endl;
//   gMinuit->mnexcm("SHO COV", arglist ,0,ierflg);
//   gMinuit->mnexcm("SHO COR", arglist ,0,ierflg);
//   gMinuit->mnexcm("SHO EIG", arglist ,0,ierflg);
   
   
   
   TFile *output = new TFile("FitPolaristaion_out.root","RECREATE");

   float ratio1[20000];
   float ratio2[20000];
   float Azi[20000];
   float BAng[20000];
   float FSerr[20000];
   float Azierr[20000];
   int tcount = 0;
   for(int i=0; i<gEntries; i++){
     if(gFieldStrengthAnt[i]!=0){
       Azi[tcount]   = gAzimuth[i]*TMath::Pi()/180.;
       BAng[tcount]  = gBAngle[i]*TMath::Pi()/180.;
       ratio1[tcount] = gFieldStrengthNS[i] / gFieldStrengthAnt[i];
       if(ratio1[tcount]<0) ratio1[tcount] = 0;
       ratio2[tcount] = gFieldStrengthEW[i] / gFieldStrengthAnt[i];
       if(ratio2[tcount]<0) ratio2[tcount] = 0;
       FSerr[tcount] = gFieldStrengthErr[i];
       Azierr[tcount] = 0;
       tcount++;
     }
   }
   TCanvas *can1 = new TCanvas("can1");
   TCanvas *can2 = new TCanvas("can2");
   TGraphErrors *g1 = new TGraphErrors(tcount,Azi, ratio1, Azierr, FSerr);
   g1->SetMarkerStyle(20);
   g1->SetTitle("Azi, NS / tot");
   TGraphErrors *g2 = new TGraphErrors(tcount,Azi, ratio2, Azierr, FSerr);
   g2->SetMarkerStyle(20);
   g2->SetTitle("Azi, EW / tot");
   
   can1->cd();
   g1->Draw("ap");
   
   can2->cd();
   g2->Draw("ap");
   
   g1->Write();
   g2->Write();
   can1->Write();
   can2->Write();
   output->Close();
   
   
   

}

