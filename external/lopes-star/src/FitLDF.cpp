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
#include <TLegend.h>
#include <TAxis.h>
#include "TMinuit.h"
#include "util.hh"


using namespace std;


//global fit data pointers
const int gMaxCh=20;
const int gMaxEvents=100;
int gEntries = gMaxCh * gMaxEvents;
int gEntriesUsed=0;
float gFieldStrengthNS[2000], gFieldStrengthEW[2000];
float gFieldStrengthNSErr[2000], gFieldStrengthEWErr[2000], gFieldStrengthAntErr[2000];
float gFieldStrengthAnt[2000];
float gDistance[2000], gDistanceErr[2000];



/**********************************************************************************************/

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

void CreateTRecEventChain19(TChain *TRecEvent)
{
    TRecEvent->AddFile("~/analysis/AnaFlag/AF_D19_run3-23_v28.root");
   TRecEvent->AddFile("~/analysis/AnaFlag/AF_D19_run24-86_v28.root");
  TRecEvent->AddFile("~/analysis/AnaFlag/AF_D19_run87-147_v28.root");

}

//______________________________________________________________________________
void CreateTRecEventChain17(TChain *TRecEvent)
{
    TRecEvent->AddFile("~/analysis/AnaFlag/AF_D17_run12-60_v28.root");
   TRecEvent->AddFile("~/analysis/AnaFlag/AF_D17_run61-123_v28.root");
  TRecEvent->AddFile("~/analysis/AnaFlag/AF_D17_run124-185_v28.root");

}

//______________________________________________________________________________

Double_t FuncLDF1(float x,Double_t *par)
{
 Double_t value =  par[0] + par[1] * exp( -1*x / par[2]) ;
 return value;
}


//______________________________________________________________________________

void FcnLDF1(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
   //calculate chisquare
   Double_t chisq = 0;
   Double_t delta;
   Double_t FSGeoMean;
   gEntriesUsed = 0;
   for (int i=0;i<gEntries; i++) {
     if(gFieldStrengthAnt[i] > 0){
       gEntriesUsed++;
       FSGeoMean = powf(gFieldStrengthEW[i],2.) + powf(gFieldStrengthNS[i],2.);
       gFieldStrengthAntErr[i] = sqrt( FSGeoMean ) * 
                  sqrt( powf(gFieldStrengthNSErr[i] * gFieldStrengthNS[i] / FSGeoMean, 2.) + powf(gFieldStrengthEWErr[i] * gFieldStrengthEW[i] / FSGeoMean, 2.) );

       delta  = (gFieldStrengthAnt[i] - FuncLDF1(gDistance[i],par)) / sqrt(powf(gFieldStrengthAntErr[i],2.) + powf(gDistanceErr[i],2.));
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

  static struct RecEvent RecEvent17;
  static struct channel_profiles channel_profiles17;
  static struct h3 h317;
  //get pointer to data
  TChain *TRecEvent17 = new TChain ("TRecEvent");
  CreateTRecEventChain17(TRecEvent17);
  cout << "TRecEvent entries = " << TRecEvent17->GetEntries() << endl;
  CreateTRecEvent(TRecEvent17, &RecEvent17);
  CreateTGrande(TRecEvent17, &h317, false);
  
  static struct RecEvent RecEvent19;
  static struct channel_profiles channel_profiles19;
  static struct h3 h319;
  //get pointer to data
  TChain *TRecEvent19 = new TChain ("TRecEvent");
  CreateTRecEventChain19(TRecEvent19);
  cout << "TRecEvent entries = " << TRecEvent19->GetEntries() << endl;
  CreateTRecEvent(TRecEvent19, &RecEvent19);
  CreateTGrande(TRecEvent19, &h319, false);

  static struct RecEvent RecEvent30;
  static struct channel_profiles channel_profiles30;
  static struct h3 h330;
  //get pointer to data
  TChain *TRecEvent30 = new TChain ("TRecEvent");
  CreateTRecEventChain30(TRecEvent30);
  cout << "TRecEvent entries = " << TRecEvent30->GetEntries() << endl;
  CreateTRecEvent(TRecEvent30, &RecEvent30);
  CreateTGrande(TRecEvent30, &h330, false);

  char cut[2048];
  bool bRadioCore = false;
  
  bool NSPol   = false;
  bool EWPol   = false;
  bool NSEWPol = true;
  
  if(0){
    cout << "first event" << endl;
    cout << "+++++++++++" << endl;
    bRadioCore=false;
    sprintf(cut, "event_id==2362938 || event_id==150759 || event_id==17542");
  }
  else{
    cout << "second event" << endl;
    cout << "+++++++++++" << endl;
    bRadioCore = true;
    sprintf(cut, "event_id==617843 || event_id==2830061 || event_id==484641"); // second shower => rejected by Grande
  }
  cout << "cut = " << cut << endl;

  TRecEvent17->Draw(">>ListRecEvent17",cut);
  TEventList *EventList17 = (TEventList*) gDirectory->Get("ListRecEvent17");
  if(EventList17==NULL) cerr << "error - EventList17 has null entries" << endl;
  TRecEvent17->SetEventList(EventList17);

  TRecEvent19->Draw(">>ListRecEvent19",cut);
  TEventList *EventList19 = (TEventList*) gDirectory->Get("ListRecEvent19");
  if(EventList19==NULL) cerr << "error - EventList19 has null entries" << endl;
  TRecEvent19->SetEventList(EventList19);

  TRecEvent30->Draw(">>ListRecEvent30",cut);
  TEventList *EventList30 = (TEventList*) gDirectory->Get("ListRecEvent30");
  if(EventList30==NULL) cerr << "error - EventList30 has null entries" << endl;
  TRecEvent30->SetEventList(EventList30);
  
  cout << "Entries 17 = " << EventList17->GetN() << endl;
  cout << "Entries 19 = " << EventList19->GetN() << endl;
  cout << "Entries 30 = " << EventList30->GetN() << endl;

  
  TChain *Tchannel_profile17 = new TChain("Tchannel_profile");
  int NoChannels17;
  Tchannel_profile17->AddFile("~/analysis/AnaFlag/AF_D17_run12-60_v28.root");
  CreateTchannel_profiles(Tchannel_profile17, &channel_profiles17);
  NoChannels17 = Tchannel_profile17->GetEntries();
  cout << "Tchannel_profile17 entries = " << NoChannels17 << endl;
  
  TChain *Tchannel_profile19 = new TChain("Tchannel_profile");
  int NoChannels19;
  Tchannel_profile19->AddFile("~/analysis/AnaFlag/AF_D19_run3-23_v28.root");
  CreateTchannel_profiles(Tchannel_profile19, &channel_profiles19);
  NoChannels19 = Tchannel_profile19->GetEntries();
  cout << "Tchannel_profile19 entries = " << NoChannels19 << endl;

  TChain *Tchannel_profile30 = new TChain("Tchannel_profile");
  int NoChannels30;
  Tchannel_profile30->AddFile("~/analysis/AnaFlag/AF_D30_run21-49_v28.root");
  CreateTchannel_profiles(Tchannel_profile30, &channel_profiles30);
  NoChannels30 = Tchannel_profile30->GetEntries();
  cout << "Tchannel_profile30 entries = " << NoChannels30 << endl;

  //core position
  double AntSignalTot[50], AntSignalEW[50], AntSignalNS[50];
  double AntSignalEWErr[50], AntSignalNSErr[50], AntSignalTotErr[50];
  double AntPosX[50], AntPosY[50];
  double AntPosXErr[50], AntPosYErr[50];
  float tmpAntPos[3];
  double FSGeoMean;
  double AntSignalErr[4000];

  
  double FSerrScale = 1;
  
  
  //fill data pointers
  gEntries =  0;
  bool bNS, bEW;
  bool bout = true;
  if(bout) cout << "field strength -- distance -- daq id -- ant pos -- event id" << endl;
  TRecEvent17->GetEntry(EventList17->GetEntry(0));
  bNS=false; bEW=false;
  for(int k=0; k<RecEvent17.DetCh; k++){
    for(int j=0; j<NoChannels17; j++){
      Tchannel_profile17->GetEntry(j);
      if((int)channel_profiles17.ch_id == RecEvent17.channel_id[k]){
	if( strncmp(channel_profiles17.polarisation,"East",4) == 0 ){
	  gFieldStrengthEW[gEntries]	 = RecEvent17.FieldStrengthChannel[k];
	  gFieldStrengthEWErr[gEntries]  = RecEvent17.ChannelErrBackground[k]*FSerrScale;
	  gFieldStrengthAnt[gEntries]	 = RecEvent17.FieldStrengthAntenna[k];
 	  gDistance[gEntries] = RecEvent17.DistanceShowerAxis[k];
	  gDistanceErr[gEntries] = RecEvent17.DistanceShowerAxisErr[k];
	  bEW = true;
	}
	else{
	  gFieldStrengthNS[gEntries]	 = RecEvent17.FieldStrengthChannel[k];
	  gFieldStrengthNSErr[gEntries]  = RecEvent17.ChannelErrBackground[k]*FSerrScale;
 	  gFieldStrengthAnt[gEntries]	 = RecEvent17.FieldStrengthAntenna[k];
	  gDistance[gEntries] = RecEvent17.DistanceShowerAxis[k];
	  gDistanceErr[gEntries] = RecEvent17.DistanceShowerAxisErr[k];
	  bNS = true;
	}
	if(bNS && bEW){
	  if(bout){
	    cout << gFieldStrengthAnt[gEntries] << " -- " << gDistance[gEntries] << " -- " << RecEvent17.daq_id << " -- " << channel_profiles17.antenna_pos;
	    cout << " -- " << RecEvent17.event_id << endl;
	  }
  	  GetAntPos(channel_profiles17.antenna_pos, RecEvent17.daq_id, tmpAntPos);
	  AntPosX[gEntries] = tmpAntPos[0];
	  AntPosY[gEntries] = tmpAntPos[1];
	  gEntries++;
	  bNS = false;
	  bEW = false;
	}
	break;
	}
      }
    }
    
  TRecEvent19->GetEntry(EventList19->GetEntry(0));
  bNS=false; bEW=false;
  for(int k=0; k<RecEvent19.DetCh; k++){
    for(int j=0; j<NoChannels19; j++){
      Tchannel_profile19->GetEntry(j);
      if((int)channel_profiles19.ch_id == RecEvent19.channel_id[k]){
	if( strncmp(channel_profiles19.polarisation,"East",4) == 0 ){
	  gFieldStrengthEW[gEntries]	 = RecEvent19.FieldStrengthChannel[k];
	  gFieldStrengthEWErr[gEntries]  = RecEvent19.ChannelErrBackground[k]*FSerrScale;
	  gFieldStrengthAnt[gEntries]	 = RecEvent19.FieldStrengthAntenna[k];
	  gDistance[gEntries] = RecEvent19.DistanceShowerAxis[k];
	  gDistanceErr[gEntries] = RecEvent19.DistanceShowerAxisErr[k];
	  bEW = true;
	}
	else{
	  gFieldStrengthNS[gEntries]	 = RecEvent19.FieldStrengthChannel[k];
	  gFieldStrengthNSErr[gEntries]  = RecEvent19.ChannelErrBackground[k]*FSerrScale;
 	  gFieldStrengthAnt[gEntries]	 = RecEvent19.FieldStrengthAntenna[k];
	  gDistance[gEntries] = RecEvent19.DistanceShowerAxis[k];
	  gDistanceErr[gEntries] = RecEvent19.DistanceShowerAxisErr[k];
	  bNS = true;
	}
	if(bNS && bEW){
	  if(bout){
	    cout << gFieldStrengthAnt[gEntries] << " -- " << gDistance[gEntries] << " -- " << RecEvent19.daq_id << " -- " << channel_profiles19.antenna_pos;
	    cout << " -- " << RecEvent19.event_id << endl;
	  }
  	  GetAntPos(channel_profiles19.antenna_pos, RecEvent19.daq_id, tmpAntPos);
	  AntPosX[gEntries] = tmpAntPos[0];
	  AntPosY[gEntries] = tmpAntPos[1];
	  gEntries++;
	  bNS = false;
	  bEW = false;
	}
	break;
	}
      }
    }
  

  TRecEvent30->GetEntry(EventList30->GetEntry(0));
  bNS=false; bEW=false;
  for(int k=0; k<RecEvent30.DetCh; k++){
    for(int j=0; j<NoChannels30; j++){
      Tchannel_profile30->GetEntry(j);
      if((int)channel_profiles30.ch_id == RecEvent30.channel_id[k]){
	if( strncmp(channel_profiles30.polarisation,"East",4) == 0 ){
	  gFieldStrengthEW[gEntries]	 = RecEvent30.FieldStrengthChannel[k];
	  gFieldStrengthEWErr[gEntries]  = RecEvent30.ChannelErrBackground[k]*FSerrScale;
	  gFieldStrengthAnt[gEntries]	 = RecEvent30.FieldStrengthAntenna[k];
	  gDistance[gEntries] = RecEvent30.DistanceShowerAxis[k];
	  gDistanceErr[gEntries] = RecEvent30.DistanceShowerAxisErr[k];
	  bEW = true;
	}
	else{
	  gFieldStrengthNS[gEntries]	 = RecEvent30.FieldStrengthChannel[k];
	  gFieldStrengthNSErr[gEntries]  = RecEvent30.ChannelErrBackground[k]*FSerrScale;
 	  gFieldStrengthAnt[gEntries]	 = RecEvent30.FieldStrengthAntenna[k];
	  gDistance[gEntries] = RecEvent30.DistanceShowerAxis[k];
	  gDistanceErr[gEntries] = RecEvent30.DistanceShowerAxisErr[k];
	  bNS = true;
	}
	if(bNS && bEW){
	  if(bout){
	    cout << gFieldStrengthAnt[gEntries] << " -- " << gDistance[gEntries] << " -- " << RecEvent30.daq_id << " -- " << channel_profiles30.antenna_pos;
    	    cout << " -- " << RecEvent30.event_id << endl;
	  }
  	  GetAntPos(channel_profiles30.antenna_pos, RecEvent30.daq_id, tmpAntPos);
	  AntPosX[gEntries] = tmpAntPos[0];
	  AntPosY[gEntries] = tmpAntPos[1];
	  gEntries++;
	  bNS = false;
	  bEW = false;
	}
	break;
	}
      }
    }
    
  for(int i=0; i<gEntries; i++){
    AntSignalTot[i] = gFieldStrengthAnt[i];
    AntSignalEW[i]  = gFieldStrengthEW[i];
    AntSignalNS[i]  = gFieldStrengthNS[i];
    AntSignalEWErr[i] = gFieldStrengthEWErr[i];
    AntSignalNSErr[i] = gFieldStrengthNSErr[i];
    AntSignalTotErr[i] = sqrt( pow(AntSignalEWErr[i],2.) + pow(AntSignalNSErr[i],2.) );
    AntPosXErr[i] = 2;
    AntPosYErr[i] = 2;
    FSGeoMean = powf(gFieldStrengthEW[i],2.) + powf(gFieldStrengthNS[i],2.);
    AntSignalErr[i] = sqrt( FSGeoMean ) * sqrt( powf(gFieldStrengthNSErr[i] * gFieldStrengthNS[i] / FSGeoMean, 2.) + powf(gFieldStrengthEWErr[i] * gFieldStrengthEW[i] / FSGeoMean, 2.) );

  }

  
  //bary center
  float sum = 0, sumx = 0, sumy = 0;
 
  for(int i=0; i<gEntries; i++){
    sum  += AntSignalTot[i];
    sumx += AntSignalTot[i] * AntPosX[i];
    sumy += AntSignalTot[i] * AntPosY[i];
  }
  float BaryX = sumx/sum;
  float BaryY = sumy/sum;
  float BaryXerr = 0;
  float BaryYerr = 0;
  
  double SumTmp1=0, SumTmp2=0, SumTmp3=0;
  
  for(int i=0; i<gEntries; i++){
    SumTmp1 += AntSignalTot[i];
    SumTmp2 += AntSignalTot[i] * AntPosX[i];
    SumTmp3 += AntSignalTot[i] * AntPosY[i];
  }
  
  for(int i=0; i<gEntries; i++){
    BaryXerr += pow( (AntPosX[i]*SumTmp1 - SumTmp2), 2.) / pow(SumTmp1,4.) * pow(AntSignalErr[i],2.)  + pow(AntPosX[i]/SumTmp1,2.) * pow(AntPosXErr[i],2.);
    BaryYerr += pow( (AntPosY[i]*SumTmp1 - SumTmp3), 2.) / pow(SumTmp1,4.) * pow(AntSignalErr[i],2.)  + pow(AntPosY[i]/SumTmp1,2.) * pow(AntPosYErr[i],2.);
  }
  BaryXerr = sqrt(BaryXerr);
  BaryYerr = sqrt(BaryYerr);
  
  cout << "bary center x = " << BaryX << " +/- " << BaryXerr << endl;
  cout << "bary center y = " << BaryY << " +/- " << BaryYerr << endl;
  
  cout << "mean x = " << TMath::Mean(gEntries, AntPosX, AntSignalTot) << endl;
  cout << "mean y = " << TMath::Mean(gEntries, AntPosY, AntSignalTot) << endl;
  
  
  //calculate distance to shower axis with found bary center
  if(bRadioCore){
    float AntPosCoordinate[3];
    for(int i=0; i<gEntries; i++){
      AntPosCoordinate[0] = AntPosX[i];
      AntPosCoordinate[1] = AntPosY[i];
      AntPosCoordinate[2] = 0;
      
      GetDistanceToShowerAxisRadio(BaryX, BaryY, AntPosCoordinate, h330.Zeg, h330.Azg, &gDistance[i], &gDistanceErr[i]);
    }
  }
  

  char ctmp[128];
  
  TGraphErrors *ShowerCoreX = new TGraphErrors(gEntries, AntPosX, AntSignalTot, AntPosXErr, AntSignalTotErr);
  ShowerCoreX->SetMarkerStyle(20);

  sprintf(ctmp,"Bary Center X: LOPES^{STAR} = %.2f +/- %.2f m, Grande = %.2f m", BaryX, BaryXerr, h330.Xcg);
  ShowerCoreX->SetTitle(ctmp);
  ShowerCoreX->GetXaxis()->SetTitle("distance on ground / m");
  ShowerCoreX->GetYaxis()->SetTitle("field strength / #muV/m");
  ShowerCoreX->GetYaxis()->CenterTitle();
  TGraphErrors *ShowerCoreY = new TGraphErrors(gEntries, AntPosY, AntSignalTot, AntPosYErr, AntSignalTotErr);
  ShowerCoreY->SetMarkerStyle(20);
  ShowerCoreY->GetXaxis()->SetTitle("distance on ground / m");
  ShowerCoreY->GetYaxis()->SetTitle("field strength / #muV/m");
  ShowerCoreY->GetYaxis()->CenterTitle();
  sprintf(ctmp,"Bary Center Y: LOPES^{STAR} = %.2f +/- %.2f m, Grande = %.2f m", BaryY, BaryYerr, h330.Ycg);
  ShowerCoreY->SetTitle(ctmp);
  TCanvas *cancore = new TCanvas("cancore");
  cancore->Divide(2,1);
  cancore->cd(1);
  ShowerCoreX->Draw("ap");
  cancore->cd(2);
  ShowerCoreY->Draw("ap");

  cout << "gEntries = " << gEntries << endl;
 
   if(gEntries > gMaxEvents) {
     cout << "constants limit reached" << endl;
     exit(1);
   }

   TMinuit *gMinuit = new TMinuit(5);
   gMinuit->SetPrintLevel(-1);
   gMinuit->SetFCN(FcnLDF1);

   Double_t arglist[10];
   Int_t ierflg = 0;

// no effect
  arglist[0] = 1;
  gMinuit->mnexcm("SET ERR", arglist ,1,ierflg);

  arglist[0] = 1;
  gMinuit->mnexcm("SET STRATEGY", arglist, 1, ierflg);

   gMinuit->mnparm(0, "shift", 0., 0.01, 0, 0, ierflg);
   gMinuit->mnparm(1, "scale", 2000., 0.1, 0, 0, ierflg);
   gMinuit->mnparm(2, "r0",    200., 0.01, 0, 0, ierflg);
   gMinuit->FixParameter(0);

   //scan par space
   for(int i=0; i<1; i++){
     gMinuit->mnexcm("SCA", arglist, 0, ierflg);
   }
   
   for(int i=0; i<1; i++){
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
   }

// Print results
   Double_t amin,edm,errdef;
   Int_t nvpar,nparx,icstat;
   gMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
   gMinuit->mnprin(3,amin);
 

   
   cout << "output" << endl;
//   gMinuit->mnexcm("SHO COV", arglist ,0,ierflg);
//   gMinuit->mnexcm("SHO COR", arglist ,0,ierflg);
//   gMinuit->mnexcm("SHO EIG", arglist ,0,ierflg);
   
   
   TFile *output = new TFile("FitLDF_out.root","RECREATE");

   float distance[20000];
   float distanceerr[20000];
   float fieldstrength[20000];
   float fieldstrengthEW[20000];
   float fieldstrengthNS[20000];
   float fieldstrengtherr[20000];
   int tcount = 0;
   for(int i=0; i<gEntries; i++){
     if(gFieldStrengthAnt[i]>0.){
       distance[tcount] = gDistance[i];
       distanceerr[tcount] = gDistanceErr[i];
       fieldstrength[tcount] = gFieldStrengthAnt[i];
       fieldstrengthEW[tcount] = gFieldStrengthEW[i];
       fieldstrengthNS[tcount] = gFieldStrengthNS[i];
       fieldstrengtherr[tcount] = sqrt( powf(gFieldStrengthNSErr[i],2) + powf(gFieldStrengthEWErr[i],2));
       tcount++;
     }
   }

   TGraphErrors *g1;
   if(NSPol) g1 = new TGraphErrors(gEntries, distance, fieldstrengthNS, distanceerr, fieldstrengtherr);
   if(EWPol) g1 = new TGraphErrors(gEntries, distance, fieldstrengthEW, distanceerr, fieldstrengtherr);
   if(NSEWPol) g1 = new TGraphErrors(gEntries, distance, fieldstrength, distanceerr, fieldstrengtherr);
   
   g1->SetMarkerStyle(20);
   g1->GetXaxis()->SetTitle("distance to shower core / m");
   g1->GetYaxis()->SetTitle("field strength / #muV/m");
   g1->GetYaxis()->CenterTitle();
   g1->SetTitle("LDF for single shower event");
   
   float LFitLimit = TMath::MinElement(gEntries, distance);
   float UFitLimit = TMath::MaxElement(gEntries, distance);
   
   TF1 *minu = new TF1("minu","[0] * exp( -1*x / [1])",LFitLimit, UFitLimit);
   minu->SetLineColor(2);
   minu->SetParName(0,"const");
   minu->SetParName(1,"R_{0}");
   
   TLegend *leg = new TLegend(0.577889, 0.711538, 0.884422, 0.865385);
   leg->AddEntry(minu,"b*e^{-r/R_{0}}","l");
   
   double par, epar;
   for(int i=1; i<3; i++){
     gMinuit->GetParameter(i,par,epar);
     minu->SetParameter(i-1,par);
//     minu->SetParLimits(i-1,par*0.80,par*1.20);
   }
   g1->Fit("minu","R");


   cout << "\nMinuit:" << endl;
   cout << "Chisquare = " << amin << endl;
   cout << "ndf       = " << gEntriesUsed - gMinuit->GetNumFreePars() << endl;
   cout << "prob      = " << TMath::Prob(amin,gEntriesUsed-gMinuit->GetNumFreePars())*100 << " %" << endl;

   TCanvas *can = new TCanvas("can");
   can->cd();
   g1->Draw("ap");
   minu->Draw("same");
//   leg->Draw("same");
   
   g1->Write();
   can->Write();
   cancore->Write();
   output->Close();
   
   
   

}

