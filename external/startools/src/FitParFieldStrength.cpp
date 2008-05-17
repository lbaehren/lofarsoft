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
#include "util.hh"


using namespace std;


//global fit data pointers
const int gMaxCh=20;
const int gMaxEvents=200;
int gEntries = gMaxCh * gMaxEvents;
const float gEffBand = 42.02;
int gEntriesUsed;

float gFieldStrengthNS[4000], gFieldStrengthEW[4000], gFieldStrengthAnt[4000];
float gFieldStrengthNSErr[4000], gFieldStrengthEWErr[4000], gFieldStrengthAntErr[4000];
float gAzimuth[4000], gZenith[4000];
float gBAngle[4000], gDistanceShowerAxis[4000];
float gEg[4000];



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

Double_t FuncParFS(float theta, float alpha, float r, float E, Double_t *par)
{
 //geht : Fix par 3 4 5
// Double_t value =  par[0] * cos(theta + par[3]) * (par[5] + tan(alpha + par[4])) * exp( -r / par[1]) * powf(10.,(E-8)*par[2]) ;
 
 //geht : Fix par 0 2 3 4 oder Fix par 3 4 5
//  Double_t value =  par[0] * cos(theta + par[3]) * (par[5] + cos(alpha + par[4])) * exp( -r / par[1]) * powf(10.,(E-8)*par[2]) ;

 //geht : Fix par  0 2 3 4 oder Fix par: 2 3 4 5 
//  Double_t value =  par[0] * cos(theta + par[3]) * (par[5] + sin(alpha + par[4])) * exp( -r / par[1]) * powf(10.,(E-8)*par[2]) ;



 //geht : Fix par 3 4 oder Fix par 3 4 5
// Double_t value =  par[0] * cos(theta + par[3]) * (par[5] + cosh(alpha + par[4])) * exp( -r / par[1]) * powf(10.,(E-8)*par[2]) ;

 //geht : Fix par 0 3 5  oder Fix par 0 3 4 5 oder Fix par 3 4 5 
//  Double_t value =  par[0] * cos(theta + par[3]) * (par[5] + sinh(alpha + par[4])) * exp( -r / par[1]) * powf(10.,(E-8)*par[2]) ;
  
// geht : Fix par 2 3 4 5 
//  Double_t value =  par[0] * cos(theta + par[3]) * (par[5] + tanh(alpha + par[4])) * exp( -r / par[1]) * powf(10.,(E-8)*par[2]) ;



//geht : Fix par: 3 4 5
//Double_t value =  par[0] * (1+0*cos(theta + par[3])) * (par[5] + sin(alpha + par[4])) * exp( -r / par[1]) * powf(10.,(E-8)*par[2]) ;

//geht : Fix par 2 3 4 5
//  Double_t value =  par[0] * (1+0*cos(theta + par[3])) * (par[5] + cosh(alpha + par[4])) * exp( -r / par[1]) * powf(10.,(E-8)*par[2]) ;


// geht mit neuer Fehlerabschätzung

//Double_t value =  par[0] * cos(theta + par[3]) * (par[5] + sin(alpha + par[4])) * exp( -r / par[1]) * powf(10.,(E-8)*par[2]) ;
//Double_t value =  par[0] * cos(theta + par[3]) * (par[5] + cos(alpha + par[4])) * exp( -r / par[1]) * powf(10.,(E-8)*par[2]) ;
Double_t value =  par[0] * (1+0*cos(theta + par[3])) * (pow(sin(alpha + par[4]),par[5])) * exp( -r / par[1]) * powf(10.,(E-8)*par[2]) ;

 return value;
}


//______________________________________________________________________________

void FcnParFS(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
   //calculate chisquare
   Double_t chisq = 0;
   Double_t delta;
   Double_t FSGeoMean;
   float tmpBAngle, tmpZenith;
   gEntriesUsed = 0;

   for (int i=0;i<gEntries; i++) {
     if(gFieldStrengthAnt[i]>0 && gFieldStrengthNS[i]>0 && gFieldStrengthEW[i]>0){
       gEntriesUsed++;
       tmpBAngle = gBAngle[i] * TMath::Pi() / 180.;
       tmpZenith = gZenith[i] * TMath::Pi() / 180.;
       FSGeoMean = powf(gFieldStrengthEW[i],2.) + powf(gFieldStrengthNS[i],2.);
       gFieldStrengthAntErr[i] = sqrt( FSGeoMean ) * 
                  sqrt( powf(gFieldStrengthNSErr[i] * gFieldStrengthNS[i] / FSGeoMean, 2.) + powf(gFieldStrengthEWErr[i] * gFieldStrengthEW[i] / FSGeoMean, 2.) );
			    
       delta  = ( gFieldStrengthAnt[i]/gEffBand - FuncParFS(tmpZenith, tmpBAngle, gDistanceShowerAxis[i], gEg[i], par)) 
                / (gFieldStrengthAntErr[i]/gEffBand);
       chisq += delta*delta;
       
//       cout << endl << gFieldStrengthNS[i] << " -- " << gFieldStrengthEW[i] << " -- " << gFieldStrengthAnt[i] << endl;
//       cout << gFieldStrengthNSErr[i] << " -- " << gFieldStrengthEWErr[i] << " -- " << gFieldStrengthAntErr[i] << endl;
     }
   }

   f = chisq;
}

//______________________________________________________________________________

Double_t FuncParFSChannel(float theta, float alpha, float r, float E, bool EW, float phi, Double_t *par)
{
  Double_t value = 0;

  if(EW) value = FuncParFS(theta, alpha, r, E, par) ;// fabs(cos(phi));
  else   value = FuncParFS(theta, alpha, r, E, par) ;// fabs(sin(phi));

 return value;
}


//______________________________________________________________________________

void FcnParFSChannel(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
   //calculate chisquare
   Double_t chisq = 0;
   Double_t delta;
   float tmpBAngle, tmpZenith, tmpAzimuth;
   gEntriesUsed=0;

   for (int i=0; i<gEntries; i++) {
       tmpBAngle  = gBAngle[i]  * TMath::Pi() / 180.;
       tmpZenith  = gZenith[i]  * TMath::Pi() / 180.;
       tmpAzimuth = gAzimuth[i] * TMath::Pi() / 180.;
    
     if(gFieldStrengthNS[i]>0 && 0){
       gEntriesUsed++;
       delta  = (gFieldStrengthNS[i]/gEffBand - FuncParFSChannel(tmpZenith, tmpBAngle, gDistanceShowerAxis[i], gEg[i], false, tmpAzimuth, par)) / (gFieldStrengthNSErr[i]/gEffBand);
       chisq += delta*delta;
     }
     
     if(gFieldStrengthEW[i]>0 && 1){
       gEntriesUsed++;
       delta  = (gFieldStrengthEW[i]/gEffBand - FuncParFSChannel(tmpZenith, tmpBAngle, gDistanceShowerAxis[i], gEg[i], true, tmpAzimuth, par)) / (gFieldStrengthEWErr[i]/gEffBand);
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
  sprintf(cut, "Eg>8.5 && abs(Zenith1Third)+2*Zenith1Third_err>Zeg && abs(Zenith1Third)-2*Zenith1Third_err<Zeg && LDFSlope<0");
//  sprintf(cut, "Eg>6 && abs(Zenith1Third)+2*Zenith1Third_err>Zeg && abs(Zenith1Third)-2*Zenith1Third_err<Zeg && event_id!=2768428"); //slope!!
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
  float errScale2 = 0.14; //0.0737; //0.048*1;
  
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
	    gFieldStrengthEWErr[gEntries] = RecEvent30.ChannelErrBackground[k]*errScale + RecEvent30.FieldStrengthChannel[k]*errScale2;
	    gFieldStrengthAnt[gEntries]   = RecEvent30.FieldStrengthAntenna[k];
	    gDistanceShowerAxis[gEntries] = RecEvent30.DistanceShowerAxis[k];
	  }
	  else{
	    bNS = true;
	    gFieldStrengthNS[gEntries]    = RecEvent30.FieldStrengthChannel[k];
	    gFieldStrengthNSErr[gEntries] = RecEvent30.ChannelErrBackground[k]*errScale + RecEvent30.FieldStrengthChannel[k]*errScale2;
	    gFieldStrengthAnt[gEntries]   = RecEvent30.FieldStrengthAntenna[k];
	    gDistanceShowerAxis[gEntries] = RecEvent30.DistanceShowerAxis[k];
	  }
	  if(bNS && bEW){
	    gAzimuth[gEntries] = h330.Azg;
            while(gAzimuth[gEntries]>360) gAzimuth[gEntries] -= 360;
	    gBAngle[gEntries] = RecEvent30.BAngle;
            while(gBAngle[gEntries]>360) gBAngle[gEntries] -= 360;
	    gZenith[gEntries] = h330.Zeg;	    
	    gEg[gEntries] = h330.Eg;
	    
	   if(0){
              cout << gAzimuth[gEntries] << " -- " << gBAngle[gEntries] << endl;
              cout << channel_profiles30.polarisation << endl;
              cout << gFieldStrengthNS[gEntries] << " -- " << gFieldStrengthEW[gEntries] <<  " ----- " << gFieldStrengthNS[gEntries]/gFieldStrengthEW[gEntries] << endl;
	      cout << gFieldStrengthAnt[gEntries] << endl;
              cout << h330.Zeg << " -- " << RecEvent30.Zenith1Third << " -- " << RecEvent30.Zenith1Third_err << endl;
	      cout << RecEvent30.event_id << endl;
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
	    gFieldStrengthEWErr[gEntries] = RecEvent17.ChannelErrBackground[k]*errScale + RecEvent17.FieldStrengthChannel[k]*errScale2;
	    gFieldStrengthAnt[gEntries]   = RecEvent17.FieldStrengthAntenna[k];
	    gDistanceShowerAxis[gEntries] = RecEvent17.DistanceShowerAxis[k];
	  }
	  else{
	    bNS = true;
	    gFieldStrengthNS[gEntries]    = RecEvent17.FieldStrengthChannel[k];
	    gFieldStrengthNSErr[gEntries] = RecEvent17.ChannelErrBackground[k]*errScale + RecEvent17.FieldStrengthChannel[k]*errScale2;
	    gFieldStrengthAnt[gEntries]   = RecEvent17.FieldStrengthAntenna[k];
	    gDistanceShowerAxis[gEntries] = RecEvent17.DistanceShowerAxis[k];
	  }
	  if(bNS && bEW){
	    gAzimuth[gEntries] = h317.Azg;
            while(gAzimuth[gEntries]>360) gAzimuth[gEntries] -= 360;
	    gBAngle[gEntries] = RecEvent17.BAngle;
            while(gBAngle[gEntries]>360) gBAngle[gEntries] -= 360;
    	    gZenith[gEntries] = h317.Zeg;
	    gEg[gEntries] = h317.Eg;

	    
	    
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
   Double_t arglist[10];
   Int_t ierflg = 0;
   int NoPar = 6;
   
   bool AntFit = false;
   
   if(AntFit){
   // Antenna Fit Settings
   //++++++++++++++++++++++
   cout << "Antenna Fit" << endl;
   cout << "+++++++++++" << endl;
   
   gMinuit->SetFCN(FcnParFS);
   
// no effect
  arglist[0] = 1;
  gMinuit->mnexcm("SET ERR", arglist ,1,ierflg);

  arglist[0] = 1;
  gMinuit->mnexcm("SET STRATEGY", arglist, 1, ierflg);
  

   gMinuit->mnparm(0, "scale",  1., 0.01, 0, 0, ierflg);
   gMinuit->mnparm(1, "r0",    400., 1, 0, 0, ierflg);
   gMinuit->mnparm(2, "Epower",  1., 0.01, 0.8, 1.2, ierflg);
//   gMinuit->mnparm(2, "Epower",  1., 0.1, 0, 0, ierflg);
   gMinuit->mnparm(3, "Zphase",  0., 0.1, -TMath::Pi()*2, TMath::Pi()*2, ierflg);
   gMinuit->mnparm(4, "Bphase",  0., 0.1, -TMath::Pi()*2, TMath::Pi()*2, ierflg);
   gMinuit->mnparm(5, "offset",  2., 0.1, 0, 0, ierflg);
//   gMinuit->FixParameter(0);
//   gMinuit->FixParameter(2);

   gMinuit->FixParameter(3);
   gMinuit->FixParameter(4);
   gMinuit->FixParameter(5);
   }
   else{
   // Channel Fit Settings
   //+++++++++++++++++++++
   cout << "Channel Fit" << endl;
   cout << "+++++++++++" << endl;

   gMinuit->SetFCN(FcnParFSChannel);
   
// no effect
  arglist[0] = 1;
  gMinuit->mnexcm("SET ERR", arglist ,1,ierflg);

  arglist[0] = 1;
  gMinuit->mnexcm("SET STRATEGY", arglist, 1, ierflg);
  
   gMinuit->mnparm(0, "scale",  1., 0.01, 0.1, 100, ierflg);
   gMinuit->mnparm(1, "r0",    400., 1, 100, 1000, ierflg);
   gMinuit->mnparm(2, "Epower",  1., 0.01, 0.8, 1.2, ierflg);
//   gMinuit->mnparm(2, "Epower",  1., 0.01, 0, 0, ierflg);
   gMinuit->mnparm(3, "Zphase",  0., 0.1, -TMath::Pi()*2, TMath::Pi()*2, ierflg);
   gMinuit->mnparm(4, "Bphase",  0., 0.1, -TMath::Pi()*2, TMath::Pi()*2, ierflg);
   gMinuit->mnparm(5, "offset",  2., 0.1, 0, 0, ierflg);
//   gMinuit->FixParameter(0);
//   gMinuit->FixParameter(2);

   gMinuit->FixParameter(3);
   gMinuit->FixParameter(4);
   gMinuit->FixParameter(5);
   }

   
   //scan par space
   for(int i=0; i<1; i++){
     gMinuit->mnexcm("SCA", arglist, 0, ierflg);
   }
   
// Now ready for minimization step
   arglist[0] = 50000; // steps of iteration
   arglist[1] = 1.; // tolerance
   gMinuit->mnexcm("MIGRAD", arglist ,2,ierflg); //MIGRAD
   cout << " MINIMIZE flag is " << ierflg << endl;
   if(ierflg!=0){
     cout << "+++++++++++++++++++++" << endl;
     cout << "+ ups.. check again +" << endl;
     cout << "+++++++++++++++++++++" << endl;
   }

   // MIGRAD: error optimiser
   if(ierflg==0 && 0){
     arglist[0] = 500000;
     gMinuit->mnexcm("MIGRAD", arglist ,1,ierflg);
     cout << " MIGRAD flag is " << ierflg << endl;
     if(ierflg!=0){
       cout << "+++++++++++++++++++++" << endl;
       cout << "+ ups.. check again +" << endl;
       cout << "+++++++++++++++++++++" << endl;
     }
   }

   // Minos: error optimiser
   if(ierflg==0 && 0){
     arglist[0] = 500000;
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
 
   cout << "prob = " << TMath::Prob(amin,gEntriesUsed-gMinuit->GetNumFreePars())*100 << " %" << endl;
   cout << "ndf = " << gEntriesUsed-gMinuit->GetNumFreePars() << endl;

   
   cout << "output" << endl;
//   gMinuit->mnexcm("SHO COV", arglist ,0,ierflg);
//   gMinuit->mnexcm("SHO COR", arglist ,0,ierflg);
//   gMinuit->mnexcm("SHO EIG", arglist ,0,ierflg);
   
   //get fit parameters
   double par[15], epar;
   for(int i=0; i<NoPar; i++){
     gMinuit->GetParameter(i,par[i],epar);
   }

   
   TFile *output = new TFile("FitParFieldStrength_out.root","RECREATE");

   int tcount = 0;
   float tmpBAngle, tmpZenith;
   float r0x[gMaxEvents], r0y[gMaxEvents];
   float bx[gMaxEvents], by[gMaxEvents];
   
   for(int i=0; i<gEntries; i++){
     if(gFieldStrengthAnt[i]>0 && gFieldStrengthNS[i]>0 && gFieldStrengthEW[i]>0){
       tmpBAngle = gBAngle[i] * TMath::Pi() / 180.;
       tmpZenith = gZenith[i] * TMath::Pi() / 180.;
       
       r0x[tcount] = -gDistanceShowerAxis[i];
       r0y[tcount] = (log( (gFieldStrengthAnt[i]/gEffBand) / ( par[0]*cos(tmpZenith+par[3])*(par[5]+sin(tmpBAngle+par[4]))*powf(10,par[2]*(gEg[i]-8))))  );
       //if(r0y[tcount] < -4 && r0x[tcount] > 50) cout << "E = " << gEg[i] << " --  i = " << i << endl;
       
       bx[tcount] = tmpBAngle;
       by[tcount] = gFieldStrengthAnt[i]/gEffBand / (par[0]*cos(tmpZenith+par[3])*exp(-gDistanceShowerAxis[i]/par[1])*powf(10,par[2]*(gEg[i]-8))) - par[5];
//       if(by[tcount]>1.5) cout << " E = " << gEg[i] << " -- i = " << i << endl;

       tcount++;
     }
   }
   TCanvas *can1 = new TCanvas("can1");
   can1->Divide(2,2);

   TGraph *gr0 = new TGraph(tcount,r0x, r0y);
   gr0->SetName("gr0");
   gr0->SetMarkerStyle(20);
   gr0->SetTitle("lateral distribution");

   TGraph *gb = new TGraph(tcount,bx, by);
   gb->SetName("gb");
   gb->SetMarkerStyle(20);
   gb->SetTitle("geomagnetic angle");
 
   can1->cd(1);
   gr0->Draw("ap");
   
   can1->cd(2);
   gb->Draw("ap");
 
 
   gr0->Write();
   gb->Write();
   can1->Write();
   output->Close();
   
   
   

}
