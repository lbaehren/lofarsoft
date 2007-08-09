#include "TH2.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TNtuple.h"
#include "MyObject_newdata.h"
//#include "MyObject_newdata.cpp"
#include "TMath.h"
//#include "/home/Rachid/bin/Fft/TFourierTransform.cpp"
//#include "/home/Rachid/bin/Fft/TFourierTransform.h"
//#include "/home/rugkvi04/numoon/moon/Fft/TFourierTransform.h"
#include "TSpectrum.h"
#include "TStopwatch.h"
#include "TVirtualFFT.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

Double_t myBackground(Double_t *x, Double_t *par)
{
  Double_t
    fitval = x[0] * exp(par[0] - par[1] * x[0]);
  
  return fitval;
}

//const Long_t numberToAnalyse = 1024*8 + 1;

const Long_t numberToAnalyse = 10001;
  
const Long_t TotalNumbers = numberToAnalyse*2 - 2;

//const Int_t NumberOfSpectra = 1024*40/numberToAnalyse;

const Int_t NumberOfSpectra = 200;

const Int_t FileTotal = 100;

Double_t Dynamic_ampX_0[numberToAnalyse][NumberOfSpectra];
Double_t Dynamic_ampY_0[numberToAnalyse][NumberOfSpectra];
Double_t Dynamic_ampX_1[numberToAnalyse][NumberOfSpectra];
Double_t Dynamic_ampY_1[numberToAnalyse][NumberOfSpectra];
Double_t Dynamic_ampX_2[numberToAnalyse][NumberOfSpectra];
Double_t Dynamic_ampY_2[numberToAnalyse][NumberOfSpectra];
Double_t Dynamic_ampX_3[numberToAnalyse][NumberOfSpectra];
Double_t Dynamic_ampY_3[numberToAnalyse][NumberOfSpectra];
Double_t Dynamic_phaseX_0[numberToAnalyse][NumberOfSpectra];
Double_t Dynamic_phaseY_0[numberToAnalyse][NumberOfSpectra];
Double_t Dynamic_phaseX_1[numberToAnalyse][NumberOfSpectra];
Double_t Dynamic_phaseY_1[numberToAnalyse][NumberOfSpectra];
Double_t Dynamic_phaseX_2[numberToAnalyse][NumberOfSpectra];
Double_t Dynamic_phaseY_2[numberToAnalyse][NumberOfSpectra];
Double_t Dynamic_phaseX_3[numberToAnalyse][NumberOfSpectra];
Double_t Dynamic_phaseY_3[numberToAnalyse][NumberOfSpectra];
  
Double_t Signalx[TotalNumbers];
Double_t Signaly[TotalNumbers];
Double_t Ampx[numberToAnalyse];
Double_t Phasex[numberToAnalyse];
Double_t Ampy[numberToAnalyse];
Double_t Phasey[numberToAnalyse];



void analyses_final_v20(Double_t TEC = 0.0, Double_t Threshold0 = 120.0, Double_t Threshold1 = 120.0, Double_t Threshold2 = 120.0, Double_t Threshold3 = 120.0, Double_t gain_gate_x_0 = 1.20, Double_t gain_gate_y_0 = 1.20, Double_t gain_gate_x_1 = 1.20, Double_t gain_gate_y_1 = 1.20, Double_t gain_gate_x_2 = 1.20, Double_t gain_gate_y_2 = 1.20, Double_t gain_gate_x_3 = 1.20, Double_t gain_gate_y_3 = 1.20)
{
  TStopwatch *klok1 = new TStopwatch;
  TStopwatch *klok2 = new TStopwatch;
  TStopwatch *klok3 = new TStopwatch;
  TStopwatch *klok4 = new TStopwatch;
  TStopwatch *klok5 = new TStopwatch;
  TStopwatch *klok6 = new TStopwatch;
  TStopwatch *klok7 = new TStopwatch;
  TStopwatch *klok8 = new TStopwatch;
  TStopwatch *klok9 = new TStopwatch;
  TStopwatch *klok10 = new TStopwatch;
  TStopwatch *klok11 = new TStopwatch;
  TStopwatch *klok12 = new TStopwatch;
  
  klok1->Start();
  
  //for (int filenumber = 0; filenumber < 1; filenumber++)
  
  char
    inFileName0[256];
  char
    inFileName1[256];
  char
    inFileName2[256];
  char
    inFileName3[256];
  char
    outFileName0[256];
  char
    outFileName1[256];
  char
    outFileName2[256];
  char
    outFileName3[256];
  char
    outFileName4[256];
  
  char
    outFileName[256];
  
 
  //sprintf(inFileName0, "/net/daqnfs.kvi.nl/daq/numoon/data/MOON.0000%d.dada.0",filenumber);
  //sprintf(inFileName1, "/net/daqnfs.kvi.nl/daq/numoon/data/MOON.0000%d.dada.2",filenumber);
  //sprintf(inFileName2, "/net/daqnfs.kvi.nl/daq/numoon/data/MOON.0000%d.dada.4",filenumber);
  //sprintf(inFileName3, "/net/daqnfs.kvi.nl/daq/numoon/data/MOON.0000%d.dada.6",filenumber);
  
  sprintf(inFileName0, "freq1.dada");
  sprintf(inFileName1, "freq2.dada");
  sprintf(inFileName2, "freq3.dada");
  sprintf(inFileName3, "freq4.dada");

  // sprintf(outFileName, "./root/timingtest_%d_%d_4freq%d.root",TEC,(int) Threshold,filenumber);
  sprintf(outFileName, "analyses_4freq.root");

  
  
  TFile *myOutput = new TFile(outFileName, "RECREATE");
  
  //Int_t pulsewidth_0 = (int)((1.34e+09*0.2*TEC*(1/(113e+06*113e+06)-1/(133e+06*133e+06)))/25e-09 + 1);
  //Int_t pulsewidth_1 = (int)((1.34e+09*0.2*TEC*(1/(127e+06*127e+06)-1/(147e+06*147e+06)))/25e-09 + 1);
  //Int_t pulsewidth_2 = (int)((1.34e+09*0.2*TEC*(1/(141e+06*141e+06)-1/(161e+06*161e+06)))/25e-09 + 1);
  //Int_t pulsewidth_3 = (int)((1.34e+09*0.2*TEC*(1/(155e+06*155e+06)-1/(175e+06*175e+06)))/25e-09 + 1);
  
  Int_t delay_1 = (int)((1.34e+09*0.2*TEC*(1/(151e+06*151e+06)-1/(165e+06*165e+06)))/25e-09 + 1);
  Int_t delay_2 = (int)((1.34e+09*0.2*TEC*(1/(137e+06*137e+06)-1/(165e+06*165e+06)))/25e-09 + 1);
  Int_t delay_3 = (int)((1.34e+09*0.2*TEC*(1/(123e+06*123e+06)-1/(165e+06*165e+06)))/25e-09 + 1);


  
  Double_t gain_x_0 = 6.19084967e+08;
  
  Double_t gain_y_0 = 5.98952122e+08;
  
  Double_t gain_x_1 = 7.774859231e+08;
  
  Double_t gain_y_1 = 7.79267692e+08;
  
  Double_t gain_x_2 = 3.27578819e+08;
  
  Double_t gain_y_2 = 3.29266576e+08;

  Double_t gain_x_3 = 2.833504e+08;
  
  Double_t gain_y_3 = 3.16298135e+08;
  
  Double_t cal_fact_x = 1.;
  
  Double_t cal_fact_y = 1.;

  Int_t ndim[1]; 
  ndim[0] = TotalNumbers;
  Int_t ndim2[1];
  ndim2[0] = numberToAnalyse;

  TVirtualFFT *fft;
      
  TVirtualFFT *rft;
      
	    
  fft = TVirtualFFT::FFT(1, ndim, "R2C M");
      
  rft = TVirtualFFT::FFT(1, ndim, "C2R M K");

  Double_t pi = 3.141592653589793238462643383279502884197169;
  //Double_t Threshold_0 = 133.94;
  //Double_t Threshold_1 = 106.44;
  //Double_t Threshold_2 = 229.07;
  //Double_t Threshold_3 = 152.64;
  //Double_t Threshold_0 = 10000000.;
  //Double_t Threshold_1 = 10000000.;
  //Double_t Threshold_2 = 10000000.;
  //Double_t Threshold_3 = 10000000.;

  //   TH2D
  //    *Dynamic1 = new TH2D("Dynamic_x","Dynamic_x",(Long_t) TotalNumbers/2,0,(Long_t) TotalNumbers/2,(Long_t) TotalNumbers/2,-0.00025,0.49975);
  //  TH2D
  //    *Dynamic2 = new TH2D("Dynamic_y","Dynamic_y",(Long_t) TotalNumbers/2,0,(Long_t) TotalNumbers/2,(Long_t) TotalNumbers/2,-0.00025,0.49975);
  //TH2D
  // *Dynamic1 = new TH2D("Dynamic_x","Dynamic_x", TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
  //TH2D
  //*Dynamic2 = new TH2D("Dynamic_y","Dynamic_y", TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
  // TH2D
  //  *Dynamic1_bgfree = new TH2D("Dynamic_x_bgfree","Dynamic_x_bgfree", 100,0,100, TotalNumbers/2,-0.00025,0.49975);
  //TH2D
  //  *Dynamic2_bgfree = new TH2D("Dynamic_y_bgfree","Dynamic_y_bgfree", 100,0,100, TotalNumbers/2,-0.00025,0.49975);
  //TH2D
  //  *Px_dynamic = new TH2D("Px_Dynamic","Px_Dynamic", 100,0,100, 100,-49.5,49.5);
  //TH2D
  //  *Py_dynamic = new TH2D("Py_Dynamic","Py_Dynamic", 100,0,100, 100,-49.5,49.5);
  //TH2D
  //  *Pow_X_dynamic = new TH2D("Pow_X_Dynamic","Pow_X_Dynamic", 100,0,100, 4000,0,4000);
  //TH2D
  //  *Pow_Y_dynamic = new TH2D("Pow_Y_Dynamic","Pow_Y_Dynamic", 100,0,100, 4000,0,4000);
  //TH1D
  //  *Pow_X1 = new TH1D("Pow_X1","Pow_X1",4000,0,4000);
  //TH1D
  //  *Pow_Y1 = new TH1D("Pow_Y1","Pow_Y1",4000,0,4000);
  
  TH1D
    *Candidates_x = new TH1D("Candidates_x","Candidates_x",numberToAnalyse,0,numberToAnalyse);
  TH1D
    *Candidates_y = new TH1D("Candidates_y","Candidates_y",numberToAnalyse,0,numberToAnalyse);
  TH1D
    *ProjectionX_0 = new TH1D("ProjectionX_0","ProjectionX_0",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *ProjectionY_0 = new TH1D("ProjectionY_0","ProjectionY_0",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *ProjectionX_1 = new TH1D("ProjectionX_1","ProjectionX_1",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *ProjectionY_1 = new TH1D("ProjectionY_1","ProjectionY_1",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *ProjectionX_2 = new TH1D("ProjectionX_2","ProjectionX_2",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *ProjectionY_2 = new TH1D("ProjectionY_2","ProjectionY_2",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *ProjectionX_3 = new TH1D("ProjectionX_3","ProjectionX_3",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *ProjectionY_3 = new TH1D("ProjectionY_3","ProjectionY_3",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *TotalProjectionX_0 = new TH1D("TotalProjectionX_0","TotalProjectionX_0",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *TotalProjectionY_0 = new TH1D("TotalProjectionY_0","TotalProjectionY_0",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *TotalProjectionX_1 = new TH1D("TotalProjectionX_1","TotalProjectionX_1",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *TotalProjectionY_1 = new TH1D("TotalProjectionY_1","TotalProjectionY_1",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *TotalProjectionX_2 = new TH1D("TotalProjectionX_2","TotalProjectionX_2",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *TotalProjectionY_2 = new TH1D("TotalProjectionY_2","TotalProjectionY_2",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *TotalProjectionX_3 = new TH1D("TotalProjectionX_3","TotalProjectionX_3",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *TotalProjectionY_3 = new TH1D("TotalProjectionY_3","TotalProjectionY_3",TotalNumbers/2,-0.00025,0.49975);

  TH1D
    *TotalProjectionX_0_bgfree = new TH1D("TotalProjectionX_0_bgfree","TotalProjectionX_0_bgfree",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *TotalProjectionY_0_bgfree = new TH1D("TotalProjectionY_0_bgfree","TotalProjectionY_0_bgfree",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *TotalProjectionX_1_bgfree = new TH1D("TotalProjectionX_1_bgfree","TotalProjectionX_1_bgfree",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *TotalProjectionY_1_bgfree = new TH1D("TotalProjectionY_1_bgfree","TotalProjectionY_1_bgfree",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *TotalProjectionX_2_bgfree = new TH1D("TotalProjectionX_2_bgfree","TotalProjectionX_2_bgfree",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *TotalProjectionY_2_bgfree = new TH1D("TotalProjectionY_2_bgfree","TotalProjectionY_2_bgfree",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *TotalProjectionX_3_bgfree = new TH1D("TotalProjectionX_3_bgfree","TotalProjectionX_3_bgfree",TotalNumbers/2,-0.00025,0.49975);
  TH1D
    *TotalProjectionY_3_bgfree = new TH1D("TotalProjectionY_3_bgfree","TotalProjectionY_3_bgfree",TotalNumbers/2,-0.00025,0.49975);

  TH1D
    *N_Peaks_x_1 = new TH1D("N_Peaks_x_1","N_Peaks_x_1",FileTotal,1,FileTotal);
  TH1D
    *N_Peaks_y_1 = new TH1D("N_Peaks_y_1","N_Peaks_y_1",FileTotal,1,FileTotal);
  TH1D
    *N_Peaks_x_2 = new TH1D("N_Peaks_x_2","N_Peaks_x_2",FileTotal,1,FileTotal);
  TH1D
    *N_Peaks_y_2 = new TH1D("N_Peaks_y_2","N_Peaks_y_2",FileTotal,1,FileTotal);
  TH1D
    *N_Peaks_x_3 = new TH1D("N_Peaks_x_3","N_Peaks_x_3",FileTotal,1,FileTotal);
  TH1D
    *N_Peaks_y_3 = new TH1D("N_Peaks_y_3","N_Peaks_y_3",FileTotal,1,FileTotal);
  TH1D
    *cal_factors_x_0 = new TH1D("cal_factors_x_0","cal_factors_x_0",FileTotal,1,FileTotal);
  TH1D
    *cal_factors_y_0 = new TH1D("cal_factors_y_0","cal_factors_y_0",FileTotal,1,FileTotal);
  TH1D
    *cal_factors_x_1 = new TH1D("cal_factors_x_1","cal_factors_x_1",FileTotal,1,FileTotal);
  TH1D
    *cal_factors_y_1 = new TH1D("cal_factors_y_1","cal_factors_y_1",FileTotal,1,FileTotal);
  TH1D
    *cal_factors_x_2 = new TH1D("cal_factors_x_2","cal_factors_x_2",FileTotal,1,FileTotal);
  TH1
    *cal_factors_y_2 = new TH1D("cal_factors_y_2","cal_factors_y_2",FileTotal,1,FileTotal);
  TH1D
    *cal_factors_x_3 = new TH1D("cal_factors_x_3","cal_factors_x_3",FileTotal,1,FileTotal);
  TH1D
    *cal_factors_y_3 = new TH1D("cal_factors_y_3","cal_factors_y_3",FileTotal,1,FileTotal);
  TH1D
    *Px_0 = new TH1D("Px_0","Px_0",1000,-49.5,49.5);
  TH1D
    *Py_0 = new TH1D("Py_0","Py_0",1000,-49.5,49.5);
  TH1D
    *Px_1 = new TH1D("Px_1","Px_1",1000,-49.5,49.5);
  TH1D
    *Py_1 = new TH1D("Py_1","Py_1",1000,-49.5,49.5);
  TH1D
    *Px_2 = new TH1D("Px_2","Px_2",1000,-49.5,49.5);
  TH1D
    *Py_2 = new TH1D("Py_2","Py_2",1000,-49.5,49.5);
  TH1D
    *Px_3 = new TH1D("Px_3","Px_3",1000,-49.5,49.5);
  TH1D
    *Py_3 = new TH1D("Py_3","Py_3",1000,-49.5,49.5);
  TH1D
    *TotalPower_0 = new TH1D("TotalPower_0","TotalPower_0",1000,0,10000);
  TH1D
    *TotalPower_1 = new TH1D("TotalPower_1","TotalPower_1",1000,0,10000);
  TH1D
    *TotalPower_2 = new TH1D("TotalPower_2","TotalPower_2",1000,0,10000);
  TH1
    *TotalPower_3 = new TH1D("TotalPower_3","TotalPower_3",1000,0,10000);
  TH1
    *TotalPower = new TH1D("TotalPower","TotalPower",1000,0,10000);
  
  Double_t Power_0[TotalNumbers - 5];
  Double_t Power_1[TotalNumbers - 5];
  Double_t Power_21[TotalNumbers - 5];
  Double_t Power_22[TotalNumbers - 5];
  Double_t Power_23[TotalNumbers - 5];
  Double_t Power_31[TotalNumbers - 5];
  Double_t Power_32[TotalNumbers - 5];
  Double_t Power_33[TotalNumbers - 5];  



  //sprintf(outFileName0, "MOON%d_%d.0000%d.dada1.output.0",TEC,(int) Threshold,filenumber);
  //sprintf(outFileName1, "MOON%d_%d.0000%d.dada1.output.1",TEC,(int) Threshold,filenumber);
  //sprintf(outFileName2, "MOON%d_%d.0000%d.dada1.output.2",TEC,(int) Threshold,filenumber);
  //sprintf(outFileName3, "MOON%d_%d.0000%d.dada1.output.3",TEC,(int) Threshold,filenumber);
  //sprintf(outFileName4, "MOON%d_%d.0000%d.dada1.time.output",TEC,(int) Threshold,filenumber);

  sprintf(outFileName0, "dada.output.1");
  sprintf(outFileName1, "dada.output.2");
  sprintf(outFileName2, "dada.output.3");
  sprintf(outFileName3, "dada.output.4");
  sprintf(outFileName4, "dada.time.output");
  

  ofstream out_file0(outFileName0,ofstream::out|ofstream::binary);
  ofstream out_file1(outFileName1,ofstream::out|ofstream::binary);
  ofstream out_file2(outFileName2,ofstream::out|ofstream::binary);
  ofstream out_file3(outFileName3,ofstream::out|ofstream::binary); 
  ofstream out_file4(outFileName4,ofstream::out|ofstream::binary);




  //cout << "test" << endl;

  
  //cout << "test2" << endl;

  Double_t reduction_factor;
  Int_t number_lines = 0;
  
  for (int filestep = 0; filestep < FileTotal; filestep++)
    {
      cout << filestep << endl;
      
      /*
	TH2D
        *Dynamic_ampX_0= new TH2D("Dynamic_aver_x_0","Dynamic_aver_x_0",(Long_t) TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
	TH2D
        *Dynamic_ampY_0= new TH2D("Dynamic_aver_y_0","Dynamic_aver_y_0",(Long_t) TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
	TH2D
        *Dynamic_phaseX_0= new TH2D("Dynamic_phase_x_0","Dynamic_phase_x_0",(Long_t) TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
	TH2D
        *Dynamic_phaseY_0= new TH2D("Dynamic_phase_y_0","Dynamic_phase_y_0",(Long_t) TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
	TH2D
        *Dynamic_ampX_1= new TH2D("Dynamic_aver_x_1","Dynamic_aver_x_1",(Long_t) TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
	TH2D
        *Dynamic_ampY_1= new TH2D("Dynamic_aver_y_1","Dynamic_aver_y_1",(Long_t) TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
	TH2D
        *Dynamic_phaseX_1= new TH2D("Dynamic_phase_x_1","Dynamic_phase_x_1",(Long_t) TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
	TH2D
        *Dynamic_phaseY_1= new TH2D("Dynamic_phase_y_1","Dynamic_phase_y_1",(Long_t) TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
	TH2D
        *Dynamic_ampX_2= new TH2D("Dynamic_aver_x_2","Dynamic_aver_x_2",(Long_t) TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
	TH2D
        *Dynamic_ampY_2= new TH2D("Dynamic_aver_y_2","Dynamic_aver_y_2",(Long_t) TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
	TH2D
        *Dynamic_phaseX_2= new TH2D("Dynamic_phase_x_2","Dynamic_phase_x_2",(Long_t) TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
	TH2D
        *Dynamic_phaseY_2= new TH2D("Dynamic_phase_y_2","Dynamic_phase_y_2",(Long_t) TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
	TH2D
        *Dynamic_ampX_3= new TH2D("Dynamic_aver_x_3","Dynamic_aver_x_3",(Long_t) TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
	TH2D
	*Dynamic_ampY_3= new TH2D("Dynamic_aver_y_3","Dynamic_aver_y_3",(Long_t) TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
	TH2D
        *Dynamic_phaseX_3= new TH2D("Dynamic_phase_x_3","Dynamic_phase_x_3",(Long_t) TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
	TH2D
        *Dynamic_phaseY_3= new TH2D("Dynamic_phase_y_3","Dynamic_phase_y_3",(Long_t) TotalNumbers/2,-0.00025,0.49975,NumberOfSpectra,0,NumberOfSpectra - 1);
	*/
      
	
     
   
      klok6->Start();
      
      //Read and FFT 1st frequency.
      for (int step_aver_fft = 0; step_aver_fft < NumberOfSpectra; step_aver_fft++)
        {
	  MyObject_newdata
	    myMoon_0(inFileName0, 4096 + 2*NumberOfSpectra*TotalNumbers*filestep+(2*TotalNumbers*step_aver_fft));
	      
	  /*
	    TH1D
	    *P1 = new TH1D("myPx","myPx",(Long_t) TotalNumbers,-0.00025,0.49975);
	    TH1D
	    *P2 = new TH1D("myPy","myPy",(Long_t) TotalNumbers,-0.00025,0.49975);
	    
	    TH1D 
	    *Amp1 = new TH1D("Amp1","Amp1",(Long_t) numberToAnalyse,0,(Long_t) TotalNumbers);
	    TH1D
	    *Amp2 = new TH1D("Amp2","Amp2",(Long_t) numberToAnalyse,0,(Long_t) TotalNumbers);
	    TH1D
	    *Phase1 = new TH1D("Phase1","Phase1",(Long_t) numberToAnalyse,0,(Long_t) TotalNumbers);
	    TH1D
	    *Phase2 = new TH1D("Phase2","Phase2",(Long_t) numberToAnalyse,0,(Long_t) TotalNumbers);
	    */
	      
	     
	      
	  Bool_t
	    onereadfailed = 0;
	  
	  Int_t in=0;
	  
	  for (Long_t eventTeller = 1; eventTeller < TotalNumbers  + 1; eventTeller++)
	    {
	      
	      if (myMoon_0.unpackEventC())
		{
		  
		  //P1->SetBinContent((Long_t) eventTeller,(Double_t) (myMoon.getSubEventX()));
		  
		  //P2->SetBinContent((Long_t) eventTeller,(Double_t) (myMoon.getSubEventY()));
		  Signalx[eventTeller-1]=(Double_t) (myMoon_0.getSubEventX());
		  Signaly[eventTeller-1]=(Double_t) (myMoon_0.getSubEventY());
		  

		  // Power->SetBinContent((Long_t) eventTeller,(Double_t) (myMoon.getPower() - 127.5));	
		  
		  // Px->Fill(myMoon.getSubEventX());
		  //  Py->Fill(myMoon.getSubEventY());
		  
		  
		}
	      
	      else
		{
		  //cout << "hallo3" << endl;
		  onereadfailed = 1;
		  
		}
	      
	      //fft->SetPoint(in, P1->GetBinContent(eventTeller));
	      fft->SetPoint(in, Signalx[eventTeller-1]);
	      
	      in++;
	      
	    }
	  
	  
	  
	  fft->Transform();
	  
	  
	  Double_t re_x, im_x;
	  Double_t re_y, im_y;
	  Int_t ind;
	  
	  
	  
	  for (Int_t binx=1; binx < numberToAnalyse + 1; binx++) 
	    {
	      
	      ind = binx-1; 
	      fft->GetPointComplex(ind, re_x, im_x);
	      
	      
	      
	      //Amp1->SetBinContent(binx, TMath::Sqrt(re_x*re_x + im_x*im_x));
	      Ampx[binx-1]=TMath::Sqrt(re_x*re_x + im_x*im_x);
	      
	      /*
		if (re_x==0)
		{
		Phase1->SetBinContent(binx, TMath::Pi()/2.);
		}
		else
		{
		Phase1->SetBinContent(binx, TMath::ATan(im_x/re_x));			  
		}
	      */
	      
	      //Phase1->SetBinContent(binx,atan2(im_x,re_x));
	      Phasex[binx-1]=atan2(im_x,re_x);
	      
	    }
	  
	  Int_t in2=0;
	  
	  for (Int_t binx=1; binx < ndim[0] + 1 ; binx++) 
	    {
	      //fft->SetPoint(in2, P2->GetBinContent(binx));
	      fft->SetPoint(in2, Signaly[binx-1]);
	      in2++;
	    }	
	  
	  fft->Transform();
	  
	  
	  
	  Int_t ind2;
	  
	  
	  
	  for (Int_t binx=1; binx < numberToAnalyse + 1; binx++) 
	    {
	      
	      ind2 = binx-1; 
	      fft->GetPointComplex(ind2, re_y, im_y);
	      
	      
	      
	      //Amp2->SetBinContent(binx, TMath::Sqrt(re_y*re_y + im_y*im_y));
	      Ampy[binx-1]=TMath::Sqrt(re_y*re_y + im_y*im_y);
	      
	      /*
		if (re_y==0)
		{
		Phase2->SetBinContent(binx, TMath::Pi()/2.);
		}
		else
		{
		Phase2->SetBinContent(binx, TMath::ATan(im_y/re_y));			  
		}
	      */
	      //Phase2->SetBinContent(binx,atan2(im_y,re_y));
	      Phasey[binx-1]=atan2(im_y,re_y);
	    }
	  
	  


		   
	      
	  for (Int_t binx = 1; binx < numberToAnalyse+1; binx++)
	    {
	      
	      //Dynamic_ampX->SetBinContent(binx,step_aver_fft,Amp1->GetBinContent(binx));
	      //Dynamic_phaseX->SetBinContent(binx,step_aver_fft+1,Phase1->GetBinContent(binx));		  
	      //Dynamic_ampY->SetBinContent(binx,step_aver_fft+1,Amp2->GetBinContent(binx));
	      //Dynamic_phaseY->SetBinContent(binx,step_aver_fft+1,Phase2->GetBinContent(binx));
	      
	      Dynamic_ampX_0[binx-1][step_aver_fft] = Ampx[binx-1];
	      Dynamic_phaseX_0[binx-1][step_aver_fft] = Phasex[binx-1];
	      Dynamic_ampY_0[binx-1][step_aver_fft] = Ampy[binx-1];
	      Dynamic_phaseY_0[binx-1][step_aver_fft] = Phasey[binx-1];
								 


	    }
	      
	  //delete Amp1;
	  //delete Phase1;
	  //delete P1;
	  //delete Amp2;
	  //delete Phase2;
	  //delete P2;
	}

      klok6->Stop();
      klok6->Print("u");
      
      Double_t *thresholdrfi_0 =  new Double_t[3];
	      
      thresholdrfi_0[0] = 20.0;
      thresholdrfi_0[1] = 10.0;
      thresholdrfi_0[2] = 5.0;
	  
      //Removing RFI 1st frequency.

      Double_t value_x;
      Double_t value_y;

      for (Int_t iterations = 0; iterations < 2; iterations++)	
	{
	  klok2->Start();
	  
	  //Dynamic_ampX->ProjectionX("ProjectionX",1,NumberOfSpectra);
	  //Dynamic_ampY->ProjectionX("ProjectionY",1,NumberOfSpectra);
	  
	  for (Int_t xstep = 1; xstep < numberToAnalyse + 1; xstep++)
	    {
	      
	      value_x = 0;
	      value_y = 0;
	      
	      for (Int_t ystep = 1; ystep < NumberOfSpectra + 1; ystep++)
		{
		  //value_x += Dynamic_ampX->GetBinContent(xstep,ystep);
		  //value_y += Dynamic_ampY->GetBinContent(xstep,ystep);
		  value_x += Dynamic_ampX_0[xstep-1][ystep-1];
		  value_y += Dynamic_ampY_0[xstep-1][ystep-1];
		  
		}
	      ProjectionX_0->SetBinContent(xstep,value_x);
	      ProjectionY_0->SetBinContent(xstep,value_y);
	      TotalProjectionX_0->AddBinContent(xstep,value_x);
	      TotalProjectionY_0->AddBinContent(xstep,value_y);
	    }
	  
	  Int_t window = 100;

	  Int_t max_peaks = 80;
	  
	  //	      Double_t *Peakpos_x = new Double_t[max_peaks];
	  
	  //Double_t *Peakheight_x = new Double_t[max_peaks];
	  
	  Double_t *candidates_x =  new Double_t[numberToAnalyse];
	  
	  Double_t *candidates_y =  new Double_t[numberToAnalyse];
	  
	  //Double_t *constant_x =  new Double_t[numberToAnalyse/window];
	  
	  //Double_t *slope_x =  new Double_t[numberToAnalyse/window];
	  
	  //Double_t *constant_y =  new Double_t[numberToAnalyse/window];
	  
	  //Double_t *slope_y =  new Double_t[numberToAnalyse/window];
	  
	  Int_t NPeaks_x= 0;
	  
	  //Double_t *Peakpos_y = new Double_t[max_peaks];
	  
	  //Double_t *Peakheight_y = new Double_t[max_peaks];
	  
	  
	  
	  Int_t NPeaks_y = 0;
	  
	  
	  for(Int_t i = 0 ; i <numberToAnalyse;i++)
	    {
	      candidates_x[i] = 0;
	      candidates_y[i] = 0;
	    }
	  
	  
	  
	  TF1 *f1 = new TF1("f1","pol9",0.001,0.5);
	  TF1 *f2 = new TF1("f2","pol9",0.001,0.5);
	  
	  ProjectionX_0->Fit("f1","R,N,Q");
	  ProjectionY_0->Fit("f2","R,N,Q");
	  
	  //Int_t i;
	  //Double_t fit_start = (i+1)/1000.*0.5;
	  //Double_t fit_end = (i + window + 1)/1000.*0.5;
	  //Int_t endpoint = 1001-window;
	  
	  
	      // Double_t average_x = ProjectionX->Integral(i + 1, i + window)/window;
	      // Double_t average_y = ProjectionY->Integral(i + 1, i + window)/window;
	      
	      for(Int_t bin = 1  ; bin < numberToAnalyse + 1 ; bin++)
		{
		  Double_t bincontent_x = ProjectionX_0->GetBinContent(bin);
		  // if(bincontent_x > (100 + thresholdrfi_0)*average_x/100.)
		  //	candidates_x[bin] = bincontent_x;
		  
		  if(bincontent_x > (100 + thresholdrfi_0[iterations])*(f1->Eval(bin*0.5/numberToAnalyse))/100.)
		    { 
		      NPeaks_x++;
		      //candidates_x[bin-1] = f1->Eval(bin*0.5/1001.)/NumberOfSpectra;
		      candidates_x[bin-1] = 1;
		    }
		  Double_t bincontent_y = ProjectionY_0->GetBinContent(bin);
		  // if(bincontent_y > (100 + thresholdrfi_0)*average_y/100.)
		  //	candidates_y[bin] = bincontent_y;
		  
		  if(bincontent_y > (100 + thresholdrfi_0[iterations])*(f2->Eval(bin*0.5/numberToAnalyse))/100.)
		    {
		      //candidates_y[bin-1] = f2->Eval(bin*0.5/1001.)/NumberOfSpectra;
		      candidates_y[bin-1] = 1;
		      NPeaks_y++;
		    }
		}
	      
	    
	  
	  delete f1;
	  delete f2;
	  
	  
	  
	  
	     
	  
	  if ( (filestep % 200) == 0)
	    {
	      
	      for(Int_t bin = 0; bin<numberToAnalyse; bin++)
		{
		  Candidates_x->SetBinContent(bin+1,candidates_x[bin]);
		  Candidates_y->SetBinContent(bin+1,candidates_y[bin]);
		}
	      Candidates_x->Write();
	      Candidates_y->Write();
	    }
	  
	  /* for(Int_t n = 0 ; n < 1000 ; n++)
	     {
	     if(candidates_x[n] > candidates_x[n-1] && candidates_x[n] > candidates_x[n+1])
	     {
	     if(NPeaks_x < max_peaks)
	     
	     {
	     Peakpos_x[NPeaks_x] = n;
	     Peakheight_x[NPeaks_x] = candidates_x[n];
	     NPeaks_x += 1;
	     }
	     else
	     {
	     printf("Warning", "Peak buffer full"); 
	     }
	     }

	     if(candidates_y[n] > candidates_y[n-1] && candidates_y[n] > candidates_y[n+1])
	     {
	     if(NPeaks_y < max_peaks)
			
	     {
	     Peakpos_y[NPeaks_y] = n;
	     Peakheight_y[NPeaks_y] = candidates_y[n];
	     NPeaks_y += 1;
	     }
	     else
	     {
	     printf("Warning", "Peak buffer full"); 
	     }
	     }
	     }*/

	      

	  /*
	    Double_t peak_width_parm_x;
	    Double_t max_peak_x = 0; Double_t min_peak_x=100000000;
	    for (Int_t i = 0; i < NPeaks_x; i++)
	    {
	    if(Peakheight_x[i] >  max_peak_x)max_peak_x = Peakheight_x[i];
	    if(Peakheight_x[i] <  min_peak_x)min_peak_x = Peakheight_x[i];
	    }
	    peak_width_parm_x = sqrt((max_peak_x-min_peak_x)/min_peak_x);
	    //cout << peak_width_parm_x << endl;
	      
	    Double_t peak_width_parm_y;
	    Double_t max_peak_y = 0; Double_t min_peak_y=100000000;
	    for (Int_t i = 0; i < NPeaks_y; i++)
	    {
	    if(Peakheight_y[i] >  max_peak_y)max_peak_y = Peakheight_y[i];
	    if(Peakheight_y[i] <  min_peak_y)min_peak_y = Peakheight_y[i];
	    }
	    peak_width_parm_y = sqrt((max_peak_y-min_peak_y)/min_peak_y);
	    //cout << peak_width_parm_y << endl;
	    Dynamic_ampX
	  */
	  Double_t average_x;
	  Double_t average_y;
	  
	  Int_t from;
	  Int_t to;
	 
	  Double_t integralx;
	  Double_t integraly;


	  for (int step_aver_fft = 0; step_aver_fft < NumberOfSpectra; step_aver_fft++)
	    {		   
	      
	      
	     
	      //Dynamic_ampX->SetBinContent(1,step_aver_fft+1, 0);
	      //Dynamic_ampY->SetBinContent(1,step_aver_fft+1, 0);
	      
	     

	      for (Int_t i = 15; i < numberToAnalyse; i++)
		{     
		  
		  if (candidates_x[i]>0)
		    {
		      
		      from = i - 19;
		      to = i;
		      
		      if(from < 2)
			{
			  from = 14;
			  to = i;
			}
		      
		      
		      //average_x = Dynamic_ampX->Integral(from,to,step_aver_fft+1,step_aver_fft+1)/(to-from+1); 
		      
		      integralx = 0;
		      
		      
		      for (int j = from; j < to+1; j++)
			{
			  integralx += Dynamic_ampX_0[j-1][step_aver_fft];			      
			}
		      
		      //average_x = Dynamic_ampX->Integral(from,to,step_aver_fft+1,step_aver_fft+1)/(to-from+1); 
		      
		      //Dynamic_ampX->SetBinContent(i+1,step_aver_fft+1,average_x);
		      
		      
		      
		      average_x = integralx/(to-from+1);

		      Dynamic_ampX_0[i][step_aver_fft] = average_x;
		      
		    }
		  
		  if (candidates_y[i]>0)
		    {
		      
		      from = i - 19;
		      to = i;
		      
		      if(from < 2)
			{
			  from = 14;
			  to = i;
			}
		      
		      //average_y = Dynamic_ampY->Integral(from,to,step_aver_fft+1,step_aver_fft+1)/(to-from+1); 
		      
		      integraly = 0;
		      
		      
		      for (int j = from; j < to+1; j++)
			{
			  integraly += Dynamic_ampY_0[j-1][step_aver_fft];			      
			}
		      
		      //average_y = Dynamic_ampY->Integral(from,to,step_aver_fft+1,step_aver_fft+1)/(to-from+1); 
		      
		      //Dynamic_ampY->SetBinContent(i+1,step_aver_fft+1,average_y);
		      
		      
		      
		      average_y = integraly/(to-from+1);
		      
		      Dynamic_ampY_0[i][step_aver_fft] = average_y;
		      
		      
		      
		    }
		  
		 
		     
		      
		    
		    
		}  
		   
		   
		   
	    } 

	  Double_t value_x;
	  Double_t value_y;

	  for (Int_t xstep = 0; xstep < numberToAnalyse; xstep++)
	    {
	      
	      value_x = 0;
	      value_y = 0;
	      
	      for (Int_t ystep = 0; ystep < NumberOfSpectra; ystep++)
		{
		  value_x += Dynamic_ampX_0[xstep][ystep];
		  value_y += Dynamic_ampY_0[xstep][ystep];
		  
		}
	      TotalProjectionX_0_bgfree->AddBinContent(xstep+1,value_x);
	      TotalProjectionY_0_bgfree->AddBinContent(xstep+1,value_y);
	    }
	      
	  delete candidates_x;	      
	  delete candidates_y;
	      
	  if (iterations == 0) 
	    {
	      N_Peaks_x_1->SetBinContent(filestep+1,NPeaks_x);
	      N_Peaks_y_1->SetBinContent(filestep+1,NPeaks_y);
		
	    }
	  else if (iterations == 1)
	    {
	      N_Peaks_x_2->SetBinContent(filestep+1,NPeaks_x);
	      N_Peaks_y_2->SetBinContent(filestep+1,NPeaks_y);
		
	    }
	  else
	    { 
	      N_Peaks_x_3->SetBinContent(filestep+1,NPeaks_x);
	      N_Peaks_y_3->SetBinContent(filestep+1,NPeaks_y);
		
	    }

	      
	      
	  /*  if ( (filestep % 100) == 0)
	      {
	      ProjectionX->Write();
	      ProjectionY->Write();
	      }*/
	  klok2->Stop();
	  klok2->Print("u");
	}

      delete thresholdrfi_0;

      Double_t sum_x_0 = 0;

      for (int ystep = 0; ystep < NumberOfSpectra; ystep++)
	{
	  for (int xstep = 0; xstep < numberToAnalyse; xstep++ )
	    {
	      sum_x_0 +=  Dynamic_ampX_0[xstep][ystep];
	    }
	}
      
      if (sum_x_0 == 0)
	{ 	  
	  cout << "zero integral in X pol." << endl;
	  sum_x_0 = 1.0;
	}
      
      //cal_fact_x = 25900.0/sum_x;
      cal_fact_x = gain_x_0/sum_x_0;

      //Double_t sum_y = Dynamic_ampY->Integral(1,1000,1,NumberOfSpectra);
      
      Double_t sum_y_0 = 0;
      
      for (int ystep = 0; ystep < NumberOfSpectra; ystep++)
	{
	  for (int xstep = 0; xstep < numberToAnalyse; xstep++ )
	    {
	      sum_y_0 +=  Dynamic_ampY_0[xstep][ystep];
	    }
	}
      
     
      if (sum_y_0 == 0)
	{ 	  
	  cout << "zero integral in Y pol." << endl;
	  sum_y_0 = 1.0;
	}

      cal_fact_y = gain_y_0/sum_y_0;

      cal_factors_x_0->SetBinContent(filestep+1,cal_fact_x);
      cal_factors_y_0->SetBinContent(filestep+1,cal_fact_y);
      

      if (cal_fact_x > gain_gate_x_0)
	{
	  //cout<< cal_fact_x <<endl;
	  //cout<< "band 1" <<endl;
	  continue;
	}
                  
            
      if (cal_fact_y > gain_gate_y_0)
	{
	  //cout<< cal_fact_y <<endl;
	  //cout<< "band 1" <<endl;
	  continue;
	}


      
      //Read, FFT, remove RFI for 2nd freq. at myMoon_1(inFileName1, 4096 + 800*numberToAnalyse*filestep).		 
      //Search for event at step_aver_fft and step1 + timedelay1.

      klok7->Start();
      for (int step_aver_fft = 0; step_aver_fft < NumberOfSpectra; step_aver_fft++)
	{
	  MyObject_newdata
	    myMoon_1(inFileName1, 4096 + 2*NumberOfSpectra*TotalNumbers*filestep+(2*TotalNumbers*step_aver_fft));
	  
	  /*
	    TH1D
	    *P1 = new TH1D("myPx","myPx",(Long_t) TotalNumbers,-0.00025,0.49975);
	    TH1D
	    *P2 = new TH1D("myPy","myPy",(Long_t) TotalNumbers,-0.00025,0.49975);
	    
	    TH1D 
	    *Amp1 = new TH1D("Amp1","Amp1",(Long_t) numberToAnalyse,0,(Long_t) TotalNumbers);
	    TH1D
	    *Amp2 = new TH1D("Amp2","Amp2",(Long_t) numberToAnalyse,0,(Long_t) TotalNumbers);
	    TH1D
	    *Phase1 = new TH1D("Phase1","Phase1",(Long_t) numberToAnalyse,0,(Long_t) TotalNumbers);
	    TH1D
	    *Phase2 = new TH1D("Phase2","Phase2",(Long_t) numberToAnalyse,0,(Long_t) TotalNumbers);
	    */
	      
	     
	      
	  Bool_t
	    onereadfailed = 0;
	  
	  Int_t in=0;
	  
	  for (Long_t eventTeller = 1; eventTeller < TotalNumbers  + 1; eventTeller++)
	    {
	      
	      if (myMoon_1.unpackEventC())
		{
		  
		  //P1->SetBinContent((Long_t) eventTeller,(Double_t) (myMoon.getSubEventX()));
		  
		  //P2->SetBinContent((Long_t) eventTeller,(Double_t) (myMoon.getSubEventY()));
		  Signalx[eventTeller-1]=(Double_t) (myMoon_1.getSubEventX());
		  Signaly[eventTeller-1]=(Double_t) (myMoon_1.getSubEventY());
		  

		  // Power->SetBinContent((Long_t) eventTeller,(Double_t) (myMoon.getPower() - 127.5));	
		  
		  // Px->Fill(myMoon.getSubEventX());
		  //  Py->Fill(myMoon.getSubEventY());
		  
		  
		}
	      
	      else
		{
		  //cout << "hallo3" << endl;
		  onereadfailed = 1;
		  
		}
	      
	      //fft->SetPoint(in, P1->GetBinContent(eventTeller));
	      fft->SetPoint(in, Signalx[eventTeller-1]);
	      
	      in++;
	      
	    }
	  
	  
	  
	  fft->Transform();
	  
	  
	  Double_t re_x, im_x;
	  Double_t re_y, im_y;
	  Int_t ind;
	  
	  
	  
	  for (Int_t binx=1; binx < numberToAnalyse + 1; binx++) 
	    {
	      
	      ind = binx-1; 
	      fft->GetPointComplex(ind, re_x, im_x);
	      
	      
	      
	      //Amp1->SetBinContent(binx, TMath::Sqrt(re_x*re_x + im_x*im_x));
	      Ampx[binx-1]=TMath::Sqrt(re_x*re_x + im_x*im_x);
	      
	      /*
		if (re_x==0)
		{
		Phase1->SetBinContent(binx, TMath::Pi()/2.);
		}
		else
		{
		Phase1->SetBinContent(binx, TMath::ATan(im_x/re_x));			  
		}
	      */
	      
	      //Phase1->SetBinContent(binx,atan2(im_x,re_x));
	      Phasex[binx-1]=atan2(im_x,re_x);
	      
	    }
	  
	  Int_t in2=0;
	  
	  for (Int_t binx=1; binx < ndim[0] + 1 ; binx++) 
	    {
	      //fft->SetPoint(in2, P2->GetBinContent(binx));
	      fft->SetPoint(in2, Signaly[binx-1]);
	      in2++;
	    }	
	  
	  fft->Transform();
	  
	  
	  
	  Int_t ind2;
	  
	  
	  
	  for (Int_t binx=1; binx < numberToAnalyse + 1; binx++) 
	    {
	      
	      ind2 = binx-1; 
	      fft->GetPointComplex(ind2, re_y, im_y);
	      
	      
	      
	      //Amp2->SetBinContent(binx, TMath::Sqrt(re_y*re_y + im_y*im_y));
	      Ampy[binx-1]=TMath::Sqrt(re_y*re_y + im_y*im_y);
	      
	      /*
		if (re_y==0)
		{
		Phase2->SetBinContent(binx, TMath::Pi()/2.);
		}
		else
		{
		Phase2->SetBinContent(binx, TMath::ATan(im_y/re_y));			  
		}
	      */
	      //Phase2->SetBinContent(binx,atan2(im_y,re_y));
	      Phasey[binx-1]=atan2(im_y,re_y);
	    }
	  
	  


		   
	      
	  for (Int_t binx = 1; binx < numberToAnalyse+1; binx++)
	    {
	      
	      //Dynamic_ampX->SetBinContent(binx,step_aver_fft+1,Amp1->GetBinContent(binx));
	      //Dynamic_phaseX->SetBinContent(binx,step_aver_fft+1,Phase1->GetBinContent(binx));		  
	      //Dynamic_ampY->SetBinContent(binx,step_aver_fft+1,Amp2->GetBinContent(binx));
	      //Dynamic_phaseY->SetBinContent(binx,step_aver_fft+1,Phase2->GetBinContent(binx));
	      
	      Dynamic_ampX_1[binx-1][step_aver_fft] = Ampx[binx-1];
	      Dynamic_phaseX_1[binx-1][step_aver_fft] = Phasex[binx-1];
	      Dynamic_ampY_1[binx-1][step_aver_fft] = Ampy[binx-1];
	      Dynamic_phaseY_1[binx-1][step_aver_fft] = Phasey[binx-1];
								 


	    }
	      
	  //delete Amp1;
	  //delete Phase1;
	  //delete P1;
	  //delete Amp2;
	  //delete Phase2;
	  //delete P2;
	}
      klok7->Stop();
      klok7->Print("u");

      Double_t *thresholdrfi_1 =  new Double_t[3];
      
      thresholdrfi_1[0] = 20.0;
      thresholdrfi_1[1] = 10.0;
      thresholdrfi_1[2] = 5.0;
      
      //Removing RFI 2nd frequency.
      for (Int_t iterations = 0; iterations < 2; iterations++)
	{
	  klok3->Start();
	  
	  //Dynamic_ampX->ProjectionX("ProjectionX",1,NumberOfSpectra);
	  //Dynamic_ampY->ProjectionX("ProjectionY",1,NumberOfSpectra);
	  
	   Double_t value_x;
	   Double_t value_y;

	  for (Int_t xstep = 1; xstep < numberToAnalyse + 1; xstep++)
	    {
	      
	      value_x = 0;
	      value_y = 0;
	      
	      for (Int_t ystep = 1; ystep < NumberOfSpectra + 1; ystep++)
		{
		  //value_x += Dynamic_ampX->GetBinContent(xstep,ystep);
		  //value_y += Dynamic_ampY->GetBinContent(xstep,ystep);
		  value_x += Dynamic_ampX_1[xstep-1][ystep-1];
		  value_y += Dynamic_ampY_1[xstep-1][ystep-1];
		  
		}
	      ProjectionX_1->SetBinContent(xstep,value_x);
	      ProjectionY_1->SetBinContent(xstep,value_y);
	      TotalProjectionX_1->AddBinContent(xstep,value_x);
	      TotalProjectionY_1->AddBinContent(xstep,value_y);
	    }
	  
	  Int_t window = 100;

	  Int_t max_peaks = 80;
	  
	  //	      Double_t *Peakpos_x = new Double_t[max_peaks];
	  
	  //Double_t *Peakheight_x = new Double_t[max_peaks];
	  
	  Double_t *candidates_x =  new Double_t[numberToAnalyse];
	  
	  Double_t *candidates_y =  new Double_t[numberToAnalyse];
	  
	  //Double_t *constant_x =  new Double_t[numberToAnalyse/window];
	  
	  //Double_t *slope_x =  new Double_t[numberToAnalyse/window];
	  
	  //Double_t *constant_y =  new Double_t[numberToAnalyse/window];
	  
	  //Double_t *slope_y =  new Double_t[numberToAnalyse/window];
	  
	  Int_t NPeaks_x= 0;
	  
	  //Double_t *Peakpos_y = new Double_t[max_peaks];
	  
	  //Double_t *Peakheight_y = new Double_t[max_peaks];
	  
	  
	  
	  Int_t NPeaks_y = 0;
	  
	  
	  for(Int_t i = 0 ; i <numberToAnalyse;i++)
	    {
	      candidates_x[i] = 0;
	      candidates_y[i] = 0;
	    }
	  
	  
	  
	  TF1 *f1 = new TF1("f1","pol9",0.001,0.5);
	  TF1 *f2 = new TF1("f2","pol9",0.001,0.5);
	  
	  ProjectionX_1->Fit("f1","R,N,Q");
	  ProjectionY_1->Fit("f2","R,N,Q");
	  
	  //Int_t i;
	  //Double_t fit_start = (i+1)/1000.*0.5;
	  //Double_t fit_end = (i + window + 1)/1000.*0.5;
	  //Int_t endpoint = 1001-window;
	  
	  
	      // Double_t average_x = ProjectionX_1->Integral(i + 1, i + window)/window;
	      // Double_t average_y = ProjectionY_1->Integral(i + 1, i + window)/window;
	      
	      for(Int_t bin = 1 ; bin < numberToAnalyse + 1 ; bin++)
		{
		  Double_t bincontent_x = ProjectionX_1->GetBinContent(bin);
		  // if(bincontent_x > (100 + thresholdrfi_1)*average_x/100.)
		  //	candidates_x[bin] = bincontent_x;
		  
		  if(bincontent_x > (100 + thresholdrfi_1[iterations])*(f1->Eval(bin*0.5/numberToAnalyse))/100.)
		    { 
		      NPeaks_x++;
		      //candidates_x[bin-1] = f1->Eval(bin*0.5/1001.)/NumberOfSpectra;
		      candidates_x[bin-1] = 1;
		    }
		  Double_t bincontent_y = ProjectionY_1->GetBinContent(bin);
		  // if(bincontent_y > (100 + thresholdrfi_1)*average_y/100.)
		  //	candidates_y[bin] = bincontent_y;
		  
		  if(bincontent_y > (100 + thresholdrfi_1[iterations])*(f2->Eval(bin*0.5/numberToAnalyse))/100.)
		    {
		      //candidates_y[bin-1] = f2->Eval(bin*0.5/1001.)/NumberOfSpectra;
		      candidates_y[bin-1] = 1;
		      NPeaks_y++;
		    }
		}
	      
	    
	  
	  delete f1;
	  delete f2;
	  
	  
	  
	  
	     
	  
	  if ( (filestep % 200) == 0)
	    {
	      
	      for(Int_t bin = 0; bin<numberToAnalyse; bin++)
		{
		  Candidates_x->SetBinContent(bin+1,candidates_x[bin]);
		  Candidates_y->SetBinContent(bin+1,candidates_y[bin]);
		}
	      Candidates_x->Write();
	      Candidates_y->Write();
	    }
	  
	  /* for(Int_t n = 0 ; n < 1000 ; n++)
	     {
	     if(candidates_x[n] > candidates_x[n-1] && candidates_x[n] > candidates_x[n+1])
	     {
	     if(NPeaks_x < max_peaks)
	     
	     {
	     Peakpos_x[NPeaks_x] = n;
	     Peakheight_x[NPeaks_x] = candidates_x[n];
	     NPeaks_x += 1;
	     }
	     else
	     {
	     printf("Warning", "Peak buffer full"); 
	     }
	     }

	     if(candidates_y[n] > candidates_y[n-1] && candidates_y[n] > candidates_y[n+1])
	     {
	     if(NPeaks_y < max_peaks)
			
	     {
	     Peakpos_y[NPeaks_y] = n;
	     Peakheight_y[NPeaks_y] = candidates_y[n];
	     NPeaks_y += 1;
	     }
	     else
	     {
	     printf("Warning", "Peak buffer full"); 
	     }
	     }
	     }*/

	      

	  /*
	    Double_t peak_width_parm_x;
	    Double_t max_peak_x = 0; Double_t min_peak_x=100000000;
	    for (Int_t i = 0; i < NPeaks_x; i++)
	    {
	    if(Peakheight_x[i] >  max_peak_x)max_peak_x = Peakheight_x[i];
	    if(Peakheight_x[i] <  min_peak_x)min_peak_x = Peakheight_x[i];
	    }
	    peak_width_parm_x = sqrt((max_peak_x-min_peak_x)/min_peak_x);
	    //cout << peak_width_parm_x << endl;
	      
	    Double_t peak_width_parm_y;
	    Double_t max_peak_y = 0; Double_t min_peak_y=100000000;
	    for (Int_t i = 0; i < NPeaks_y; i++)
	    {
	    if(Peakheight_y[i] >  max_peak_y)max_peak_y = Peakheight_y[i];
	    if(Peakheight_y[i] <  min_peak_y)min_peak_y = Peakheight_y[i];
	    }
	    peak_width_parm_y = sqrt((max_peak_y-min_peak_y)/min_peak_y);
	    //cout << peak_width_parm_y << endl;
	    Dynamic_ampX
	  */
	  Double_t average_x;
	  Double_t average_y;
	  Int_t from;
	  Int_t to;
	 
	 
	  Double_t integralx;
	  Double_t integraly;
	
	  for (int step_aver_fft = 0; step_aver_fft < NumberOfSpectra; step_aver_fft++)
	    {		   
	      
	      
	      
	      
	      //Dynamic_ampX->SetBinContent(1,step_aver_fft+1, 0);
	      //Dynamic_ampY->SetBinContent(1,step_aver_fft+1, 0);
	      
	      for (Int_t i = 15; i < numberToAnalyse; i++)
		{     
		  
		  if (candidates_x[i]>0)
		    {
		      
		      from = i - 19;
		      to = i;
		      
		      if(from < 2)
			{
			  from = 14;
			  to = i;
			}
		      
		      
		      //average_x = Dynamic_ampX->Integral(from,to,step_aver_fft+1,step_aver_fft+1)/(to-from+1); 
		      
		      integralx = 0;
		      
		      
		      for (int j = from; j < to+1; j++)
			{
			  integralx += Dynamic_ampX_1[j-1][step_aver_fft];			      
			}
		      
		      //average_x = Dynamic_ampX->Integral(from,to,step_aver_fft+1,step_aver_fft+1)/(to-from+1); 
		      
		      //Dynamic_ampX->SetBinContent(i+1,step_aver_fft+1,average_x);
		      
		      
		      
		      average_x = integralx/(to-from+1);
		      
		      Dynamic_ampX_1[i][step_aver_fft] = average_x;
		      
		    }
		  
		  if (candidates_y[i]>0)
		    {
		      
		      from = i - 19;
		      to = i;
		      
		      if(from < 2)
			{
			  from = 14;
			  to = i;
			}
		      
		      //average_y = Dynamic_ampY->Integral(from,to,step_aver_fft+1,step_aver_fft+1)/(to-from+1); 
		      
		      integraly = 0;
		      
		      
		      for (int j = from; j < to+1; j++)
			{
			  integraly += Dynamic_ampY_1[j-1][step_aver_fft];			      
			}
		      
		      //average_y = Dynamic_ampY->Integral(from,to,step_aver_fft+1,step_aver_fft+1)/(to-from+1); 
		      
		      //Dynamic_ampY->SetBinContent(i+1,step_aver_fft+1,average_y);
		      
		      
		      
		      average_y = integraly/(to-from+1);
		      
		      Dynamic_ampY_1[i][step_aver_fft] = average_y;
		      
		      
		      
		    }
		  
		 
		  
		}  
	      
	      
	      
	    } 

	  for (Int_t xstep = 0; xstep < numberToAnalyse; xstep++)
	    {
	      
	      Double_t value_x = 0;
	      Double_t value_y = 0;
	      
	      for (Int_t ystep = 0; ystep < NumberOfSpectra; ystep++)
		{
		  value_x += Dynamic_ampX_1[xstep][ystep];
		  value_y += Dynamic_ampY_1[xstep][ystep];
		  
		}
	      TotalProjectionX_1_bgfree->AddBinContent(xstep+1,value_x);
	      TotalProjectionY_1_bgfree->AddBinContent(xstep+1,value_y);
	    }
	  
	  delete candidates_x;	      
	  delete candidates_y;
	  
	  if (iterations == 0) 
	    {
	      N_Peaks_x_1->SetBinContent(filestep+1,NPeaks_x);
	      N_Peaks_y_1->SetBinContent(filestep+1,NPeaks_y);
	      
	    }
	  else if (iterations == 1)
	    {
	      N_Peaks_x_2->SetBinContent(filestep+1,NPeaks_x);
	      N_Peaks_y_2->SetBinContent(filestep+1,NPeaks_y);
	      
	    }
	  else
	    { 
	      N_Peaks_x_3->SetBinContent(filestep+1,NPeaks_x);
	      N_Peaks_y_3->SetBinContent(filestep+1,NPeaks_y);
	      
	    }
	  
	  
	  
	  /*  if ( (filestep % 100) == 0)
	      {
	      ProjectionX->Write();
	      ProjectionY->Write();
	      }*/
	  klok3->Stop();
	  klok3->Print("u");
	}
      
      
      delete thresholdrfi_1;
      
      Double_t sum_x_1 = 0;
      
      for (int ystep = 0; ystep < NumberOfSpectra; ystep++)
	{
	  for (int xstep = 0; xstep < numberToAnalyse; xstep++ )
	    {
	      sum_x_1 +=  Dynamic_ampX_1[xstep][ystep];
	    }
	}
      
      if (sum_x_1 == 0)
	{ 	  
	  cout << "zero integral in X pol." << endl;
	  sum_x_1 = 1.0;
	}
      
      //cal_fact_x = 25900.0/sum_x_1;
      cal_fact_x = gain_x_1/sum_x_1;

      //Double_t sum_y = Dynamic_ampY->Integral(1,1000,1,NumberOfSpectra);
      
      Double_t sum_y_1 = 0;
      
      for (int ystep = 0; ystep < NumberOfSpectra; ystep++)
	{
	  for (int xstep = 0; xstep < numberToAnalyse; xstep++ )
	    {
	      sum_y_1 +=  Dynamic_ampY_1[xstep][ystep];
	    }
	}
      
      
      if (sum_y_1 == 0)
	{ 	  
	  cout << "zero integral in Y pol." << endl;
	  sum_y_1 = 1.0;
	}

      cal_fact_y = gain_y_1/sum_y_1;

      cal_factors_x_1->SetBinContent(filestep+1,cal_fact_x);
      cal_factors_y_1->SetBinContent(filestep+1,cal_fact_y);
      
      if (cal_fact_x > gain_gate_x_1)
	{
	  //cout<< cal_fact_x <<endl;
	  //cout<< "band 2" <<endl;
	  continue;
	}
                             
      if (cal_fact_y > gain_gate_y_1)
	{
	  //cout<< cal_fact_y <<endl;
	  //cout<< "band 2" <<endl;
	  continue;
	}


      klok8->Start();
      //Same for freq3
      for (int step_aver_fft = 0; step_aver_fft < NumberOfSpectra; step_aver_fft++)
	{
	  MyObject_newdata
	    myMoon_2(inFileName2, 4096 + 2*NumberOfSpectra*TotalNumbers*filestep+(2*TotalNumbers*step_aver_fft));
	  
	  /*
	    TH1D
	    *P1 = new TH1D("myPx","myPx",(Long_t) TotalNumbers,-0.00025,0.49975);
	    TH1D
	    *P2 = new TH1D("myPy","myPy",(Long_t) TotalNumbers,-0.00025,0.49975);
	    
	    TH1D 
	    *Amp1 = new TH1D("Amp1","Amp1",(Long_t) numberToAnalyse,0,(Long_t) TotalNumbers);
	    TH1D
	    *Amp2 = new TH1D("Amp2","Amp2",(Long_t) numberToAnalyse,0,(Long_t) TotalNumbers);
	    TH1D
	    *Phase1 = new TH1D("Phase1","Phase1",(Long_t) numberToAnalyse,0,(Long_t) TotalNumbers);
	    TH1D
	    *Phase2 = new TH1D("Phase2","Phase2",(Long_t) numberToAnalyse,0,(Long_t) TotalNumbers);
	    */
	      
	     
	      
	  Bool_t
	    onereadfailed = 0;
	  
	  Int_t in=0;
	  
	  for (Long_t eventTeller = 1; eventTeller < TotalNumbers  + 1; eventTeller++)
	    {
	      
	      if (myMoon_2.unpackEventC())
		{
		  
		  //P1->SetBinContent((Long_t) eventTeller,(Double_t) (myMoon.getSubEventX()));
		  
		  //P2->SetBinContent((Long_t) eventTeller,(Double_t) (myMoon.getSubEventY()));
		  Signalx[eventTeller-1]=(Double_t) (myMoon_2.getSubEventX());
		  Signaly[eventTeller-1]=(Double_t) (myMoon_2.getSubEventY());
		  

		  // Power->SetBinContent((Long_t) eventTeller,(Double_t) (myMoon.getPower() - 127.5));	
		  
		  // Px->Fill(myMoon.getSubEventX());
		  //  Py->Fill(myMoon.getSubEventY());
		  
		  
		}
	      
	      else
		{
		  //cout << "hallo3" << endl;
		  onereadfailed = 1;
		  
		}
	      
	      //fft->SetPoint(in, P1->GetBinContent(eventTeller));
	      fft->SetPoint(in, Signalx[eventTeller-1]);
	      
	      in++;
	      
	    }
	  
	  
	  
	  fft->Transform();
	  
	  
	  Double_t re_x, im_x;
	  Double_t re_y, im_y;
	  Int_t ind;
	  
	  
	  
	  for (Int_t binx=1; binx < numberToAnalyse + 1; binx++) 
	    {
	      
	      ind = binx-1; 
	      fft->GetPointComplex(ind, re_x, im_x);
	      
	      
	      
	      //Amp1->SetBinContent(binx, TMath::Sqrt(re_x*re_x + im_x*im_x));
	      Ampx[binx-1]=TMath::Sqrt(re_x*re_x + im_x*im_x);
	      
	      /*
		if (re_x==0)
		{
		Phase1->SetBinContent(binx, TMath::Pi()/2.);
		}
		else
		{
		Phase1->SetBinContent(binx, TMath::ATan(im_x/re_x));			  
		}
	      */
	      
	      //Phase1->SetBinContent(binx,atan2(im_x,re_x));
	      Phasex[binx-1]=atan2(im_x,re_x);
	      
	    }
	  
	  Int_t in2=0;
	  
	  for (Int_t binx=1; binx < ndim[0] + 1 ; binx++) 
	    {
	      //fft->SetPoint(in2, P2->GetBinContent(binx));
	      fft->SetPoint(in2, Signaly[binx-1]);
	      in2++;
	    }	
	  
	  fft->Transform();
	  
	  
	  
	  Int_t ind2;
	  
	  
	  
	  for (Int_t binx=1; binx < numberToAnalyse + 1; binx++) 
	    {
	      
	      ind2 = binx-1; 
	      fft->GetPointComplex(ind2, re_y, im_y);
	      
	      
	      
	      //Amp2->SetBinContent(binx, TMath::Sqrt(re_y*re_y + im_y*im_y));
	      Ampy[binx-1]=TMath::Sqrt(re_y*re_y + im_y*im_y);
	      
	      /*
		if (re_y==0)
		{
		Phase2->SetBinContent(binx, TMath::Pi()/2.);
		}
		else
		{
		Phase2->SetBinContent(binx, TMath::ATan(im_y/re_y));			  
		}
	      */
	      //Phase2->SetBinContent(binx,atan2(im_y,re_y));
	      Phasey[binx-1]=atan2(im_y,re_y);
	    }
	  
	  


		   
	      
	  for (Int_t binx = 1; binx < numberToAnalyse+1; binx++)
	    {
	      
	      //Dynamic_ampX->SetBinContent(binx,step_aver_fft+1,Amp1->GetBinContent(binx));
	      //Dynamic_phaseX->SetBinContent(binx,step_aver_fft+1,Phase1->GetBinContent(binx));		  
	      //Dynamic_ampY->SetBinContent(binx,step_aver_fft+1,Amp2->GetBinContent(binx));
	      //Dynamic_phaseY->SetBinContent(binx,step_aver_fft+1,Phase2->GetBinContent(binx));
	      
	      Dynamic_ampX_2[binx-1][step_aver_fft] = Ampx[binx-1];
	      Dynamic_phaseX_2[binx-1][step_aver_fft] = Phasex[binx-1];
	      Dynamic_ampY_2[binx-1][step_aver_fft] = Ampy[binx-1];
	      Dynamic_phaseY_2[binx-1][step_aver_fft] = Phasey[binx-1];
								 


	    }
	      
	  //delete Amp1;
	  //delete Phase1;
	  //delete P1;
	  //delete Amp2;
	  //delete Phase2;
	  //delete P2;
	}

      klok8->Stop();
      klok8->Print("u");
      Double_t *thresholdrfi_2 =  new Double_t[3];
      
      thresholdrfi_2[0] = 20.0;
      thresholdrfi_2[1] = 10.0;
      thresholdrfi_2[2] = 5.0;
      
      //Removing RFI 3rd frequency.
      for (Int_t iterations = 0; iterations < 2; iterations++)
	{
	  klok4->Start();
	  
	  //Dynamic_ampX->ProjectionX("ProjectionX",1,NumberOfSpectra);
	  //Dynamic_ampY->ProjectionX("ProjectionY",1,NumberOfSpectra);
	  
	  for (Int_t xstep = 1; xstep < numberToAnalyse + 1; xstep++)
	    {
	      
	      Double_t value_x = 0;
	      Double_t value_y = 0;
	      
	      for (Int_t ystep = 1; ystep < NumberOfSpectra + 1; ystep++)
		{
		  //value_x += Dynamic_ampX->GetBinContent(xstep,ystep);
		  //value_y += Dynamic_ampY->GetBinContent(xstep,ystep);
		  value_x += Dynamic_ampX_2[xstep-1][ystep-1];
		  value_y += Dynamic_ampY_2[xstep-1][ystep-1];
		  
		}
	      ProjectionX_2->SetBinContent(xstep,value_x);
	      ProjectionY_2->SetBinContent(xstep,value_y);
	      TotalProjectionX_2->AddBinContent(xstep,value_x);
	      TotalProjectionY_2->AddBinContent(xstep,value_y);
	    }
	  
	  Int_t window = 100;

	  Int_t max_peaks = 80;
	  
	  //	      Double_t *Peakpos_x = new Double_t[max_peaks];
	  
	  //Double_t *Peakheight_x = new Double_t[max_peaks];
	  
	  Double_t *candidates_x =  new Double_t[numberToAnalyse];
	  
	  Double_t *candidates_y =  new Double_t[numberToAnalyse];
	  
	  Double_t *constant_x =  new Double_t[numberToAnalyse/window];
	  
	  Double_t *slope_x =  new Double_t[numberToAnalyse/window];
	  
	  Double_t *constant_y =  new Double_t[numberToAnalyse/window];
	  
	  Double_t *slope_y =  new Double_t[numberToAnalyse/window];
	  
	  Int_t NPeaks_x= 0;
	  
	  //Double_t *Peakpos_y = new Double_t[max_peaks];
	  
	  //Double_t *Peakheight_y = new Double_t[max_peaks];
	  
	  
	  
	  Int_t NPeaks_y = 0;
	  
	  
	  for(Int_t i = 0 ; i <numberToAnalyse;i++)
	    {
	      candidates_x[i] = 0;
	      candidates_y[i] = 0;
	    }
	  
	  
	  
	  TF1 *f1 = new TF1("f1","pol9",0.001,0.5);
	  TF1 *f2 = new TF1("f2","pol9",0.001,0.5);
	  
	  ProjectionX_2->Fit("f1","R,N,Q");
	  ProjectionY_2->Fit("f2","R,N,Q");
	  
	  //Int_t i;
	  //Double_t fit_start = (i+1)/1000.*0.5;
	  //Double_t fit_end = (i + window + 1)/1000.*0.5;
	  //Int_t endpoint = 1001-window;
	  
	  
	      // Double_t average_x = ProjectionX->Integral(i + 1, i + window)/window;
	      // Double_t average_y = ProjectionY->Integral(i + 1, i + window)/window;
	      
	      for(Int_t bin = 1 ; bin < numberToAnalyse + 1 ; bin++)
		{
		  Double_t bincontent_x = ProjectionX_2->GetBinContent(bin);
		  // if(bincontent_x > (100 + thresholdrfi_2)*average_x/100.)
		  //	candidates_x[bin] = bincontent_x;
		  
		  if(bincontent_x > (100 + thresholdrfi_2[iterations])*(f1->Eval(bin*0.5/numberToAnalyse))/100.)
		    { 
		      NPeaks_x++;
		      //candidates_x[bin-1] = f1->Eval(bin*0.5/1001.)/NumberOfSpectra;
		      candidates_x[bin-1] = 1;
		    }
		  Double_t bincontent_y = ProjectionY_2->GetBinContent(bin);
		  // if(bincontent_y > (100 + thresholdrfi_2)*average_y/100.)
		  //	candidates_y[bin] = bincontent_y;
		  
		  if(bincontent_y > (100 + thresholdrfi_2[iterations])*(f2->Eval(bin*0.5/numberToAnalyse))/100.)
		    {
		      //candidates_y[bin-1] = f2->Eval(bin*0.5/1001.)/NumberOfSpectra;
		      candidates_y[bin-1] = 1;
		      NPeaks_y++;
		    }
		}
	      
	    
	  
	  delete f1;
	  delete f2;
	  
	  
	  
	  
	     
	  
	  if ( (filestep % 200) == 0)
	    {
	      
	      for(Int_t bin = 0; bin<numberToAnalyse; bin++)
		{
		  Candidates_x->SetBinContent(bin+1,candidates_x[bin]);
		  Candidates_y->SetBinContent(bin+1,candidates_y[bin]);
		}
	      Candidates_x->Write();
	      Candidates_y->Write();
	    }
	  
	  /* for(Int_t n = 0 ; n < 1000 ; n++)
	     {
	     if(candidates_x[n] > candidates_x[n-1] && candidates_x[n] > candidates_x[n+1])
	     {
	     if(NPeaks_x < max_peaks)
	     
	     {
	     Peakpos_x[NPeaks_x] = n;
	     Peakheight_x[NPeaks_x] = candidates_x[n];
	     NPeaks_x += 1;
	     }
	     else
	     {
	     printf("Warning", "Peak buffer full"); 
	     }
	     }

	     if(candidates_y[n] > candidates_y[n-1] && candidates_y[n] > candidates_y[n+1])
	     {
	     if(NPeaks_y < max_peaks)
			
	     {
	     Peakpos_y[NPeaks_y] = n;
	     Peakheight_y[NPeaks_y] = candidates_y[n];
	     NPeaks_y += 1;
	     }
	     else
	     {
	     printf("Warning", "Peak buffer full"); 
	     }
	     }
	     }*/

	      

	  /*
	    Double_t peak_width_parm_x;
	    Double_t max_peak_x = 0; Double_t min_peak_x=100000000;
	    for (Int_t i = 0; i < NPeaks_x; i++)
	    {
	    if(Peakheight_x[i] >  max_peak_x)max_peak_x = Peakheight_x[i];
	    if(Peakheight_x[i] <  min_peak_x)min_peak_x = Peakheight_x[i];
	    }
	    peak_width_parm_x = sqrt((max_peak_x-min_peak_x)/min_peak_x);
	    //cout << peak_width_parm_x << endl;
	      
	    Double_t peak_width_parm_y;
	    Double_t max_peak_y = 0; Double_t min_peak_y=100000000;
	    for (Int_t i = 0; i < NPeaks_y; i++)
	    {
	    if(Peakheight_y[i] >  max_peak_y)max_peak_y = Peakheight_y[i];
	    if(Peakheight_y[i] <  min_peak_y)min_peak_y = Peakheight_y[i];
	    }
	    peak_width_parm_y = sqrt((max_peak_y-min_peak_y)/min_peak_y);
	    //cout << peak_width_parm_y << endl;
	    Dynamic_ampX
	  */
	  Double_t average_x;
	  Double_t average_y;
	  
	  for (int step_aver_fft = 0; step_aver_fft < NumberOfSpectra; step_aver_fft++)
	    {		   
	      
	      
	      Int_t from;
	      Int_t to;
	      //Dynamic_ampX->SetBinContent(1,step_aver_fft+1, 0);
	      //Dynamic_ampY->SetBinContent(1,step_aver_fft+1, 0);
	      
	      for (Int_t i = 15; i < numberToAnalyse; i++)
		{     
		  
		  if (candidates_x[i]>0)
		    {
		      
		      from = i - 19;
		      to = i;
		      
		      if(from < 2)
			{
			  from = 14;
			  to = i;
			}
		      
		      
		      //average_x = Dynamic_ampX->Integral(from,to,step_aver_fft+1,step_aver_fft+1)/(to-from+1); 
		      
		      Double_t integralx = 0;
		      
		      
		      for (int j = from; j < to+1; j++)
			{
			  integralx += Dynamic_ampX_2[j-1][step_aver_fft];			      
			}
		      
		      //average_x = Dynamic_ampX->Integral(from,to,step_aver_fft+1,step_aver_fft+1)/(to-from+1); 
		      
		      //Dynamic_ampX->SetBinContent(i+1,step_aver_fft+1,average_x);
		      
		      
		      
		      average_x = integralx/(to-from+1);

		      Dynamic_ampX_2[i][step_aver_fft] = average_x;
		      
		    }
		  
		  if (candidates_y[i]>0)
		    {
		      
		      from = i - 19;
		      to = i;
		      
		      if(from < 2)
			{
			  from = 14;
			  to = i;
			}
		      
		      //average_y = Dynamic_ampY->Integral(from,to,step_aver_fft+1,step_aver_fft+1)/(to-from+1); 
		      
		      Double_t integraly = 0;
		      
		      
		      for (int j = from; j < to+1; j++)
			{
			  integraly += Dynamic_ampY_2[j-1][step_aver_fft];			      
			}
		      
		      //average_y = Dynamic_ampY->Integral(from,to,step_aver_fft+1,step_aver_fft+1)/(to-from+1); 
		      
		      //Dynamic_ampY->SetBinContent(i+1,step_aver_fft+1,average_y);
		      
		      
		      
		      average_y = integraly/(to-from+1);
		      
		      Dynamic_ampY_2[i][step_aver_fft] = average_y;
		      
		      
		      
		    }
		  
		 
		      		           	    		    
		}  
		   
		   
		   
	    } 
	      
	  for (Int_t xstep = 0; xstep < numberToAnalyse; xstep++)
	    {
	      
	      Double_t value_x = 0;
	      Double_t value_y = 0;
	      
	      for (Int_t ystep = 0; ystep < NumberOfSpectra; ystep++)
		{
		  value_x += Dynamic_ampX_2[xstep][ystep];
		  value_y += Dynamic_ampY_2[xstep][ystep];
		  
		}
	      TotalProjectionX_2_bgfree->AddBinContent(xstep+1,value_x);
	      TotalProjectionY_2_bgfree->AddBinContent(xstep+1,value_y);
	    }

	  delete candidates_x;	      
	  delete candidates_y;
	      
	  if (iterations == 0) 
	    {
	      N_Peaks_x_1->SetBinContent(filestep+1,NPeaks_x);
	      N_Peaks_y_1->SetBinContent(filestep+1,NPeaks_y);
		
	    }
	  else if (iterations == 1)
	    {
	      N_Peaks_x_2->SetBinContent(filestep+1,NPeaks_x);
	      N_Peaks_y_2->SetBinContent(filestep+1,NPeaks_y);
		
	    }
	  else
	    { 
	      N_Peaks_x_3->SetBinContent(filestep+1,NPeaks_x);
	      N_Peaks_y_3->SetBinContent(filestep+1,NPeaks_y);
		
	    }

	      
	      
	  /*  if ( (filestep % 100) == 0)
	      {
	      ProjectionX->Write();
	      ProjectionY->Write();
	      }*/
	  klok4->Stop();
	  klok4->Print("u");
	}
	
      
      delete thresholdrfi_2;
      

      Double_t sum_x_2 = 0;
      
      for (int ystep = 0; ystep < NumberOfSpectra; ystep++)
	{
	  for (int xstep = 0; xstep < numberToAnalyse; xstep++ )
	    {
	      sum_x_2 +=  Dynamic_ampX_2[xstep][ystep];
	    }
	}
      
      if (sum_x_2 == 0)
	{ 	  
	  cout << "zero integral in X pol." << endl;
	  sum_x_2 = 1.0;
	}
      
      //cal_fact_x = 25900.0/sum_x_2;
      cal_fact_x = gain_x_2/sum_x_2;

      //Double_t sum_y_2 = Dynamic_ampY->Integral(1,1000,1,NumberOfSpectra);
      
      Double_t sum_y_2 = 0;
      
      for (int ystep = 0; ystep < NumberOfSpectra; ystep++)
	{
	  for (int xstep = 0; xstep < numberToAnalyse; xstep++ )
	    {
	      sum_y_2 +=  Dynamic_ampY_2[xstep][ystep];
	    }
	}
      
      
      if (sum_y_2 == 0)
	{ 	  
	  cout << "zero integral in Y pol." << endl;
	  sum_y_2 = 1.0;
	}
      
      cal_fact_y = gain_y_2/sum_y_2;
      
      
      cal_factors_x_2->SetBinContent(filestep+1,cal_fact_x);
      cal_factors_y_2->SetBinContent(filestep+1,cal_fact_y);
      
      if (cal_fact_x > gain_gate_x_2)
	{
	  //cout<< cal_fact_x <<endl;
	  //cout<< "band 3" <<endl;
	  continue;
	}
      
      
      
      if (cal_fact_y > gain_gate_y_2)
	{  
	    //cout<< cal_fact_y <<endl;
	  //cout<< "band 3" <<endl;
	  continue;
	}
      
      klok9->Start();
      //Same for freq4
      for (int step_aver_fft = 0; step_aver_fft < NumberOfSpectra; step_aver_fft++)
	{
	  MyObject_newdata
	    myMoon_3(inFileName3, 4096 + 2*NumberOfSpectra*TotalNumbers*filestep+(2*TotalNumbers*step_aver_fft));
	  
	  /*
	    TH1D
	    *P1 = new TH1D("myPx","myPx",(Long_t) TotalNumbers,-0.00025,0.49975);
	    TH1D
	    *P2 = new TH1D("myPy","myPy",(Long_t) TotalNumbers,-0.00025,0.49975);
	    
	    TH1D 
	    *Amp1 = new TH1D("Amp1","Amp1",(Long_t) numberToAnalyse,0,(Long_t) TotalNumbers);
	    TH1D
	    *Amp2 = new TH1D("Amp2","Amp2",(Long_t) numberToAnalyse,0,(Long_t) TotalNumbers);
	    TH1D
	    *Phase1 = new TH1D("Phase1","Phase1",(Long_t) numberToAnalyse,0,(Long_t) TotalNumbers);
	    TH1D
	    *Phase2 = new TH1D("Phase2","Phase2",(Long_t) numberToAnalyse,0,(Long_t) TotalNumbers);
	    */
	      
	     
	      
	  Bool_t
	    onereadfailed = 0;
	  
	  Int_t in=0;
	  
	  for (Long_t eventTeller = 1; eventTeller < TotalNumbers  + 1; eventTeller++)
	    {
	      
	      if (myMoon_3.unpackEventC())
		{
		  
		  //P1->SetBinContent((Long_t) eventTeller,(Double_t) (myMoon.getSubEventX()));
		  
		  //P2->SetBinContent((Long_t) eventTeller,(Double_t) (myMoon.getSubEventY()));
		  Signalx[eventTeller-1]=(Double_t) (myMoon_3.getSubEventX());
		  Signaly[eventTeller-1]=(Double_t) (myMoon_3.getSubEventY());
		  

		  // Power->SetBinContent((Long_t) eventTeller,(Double_t) (myMoon.getPower() - 127.5));	
		  
		  // Px->Fill(myMoon.getSubEventX());
		  //  Py->Fill(myMoon.getSubEventY());
		  
		  
		}
	      
	      else
		{
		  //cout << "hallo3" << endl;
		  onereadfailed = 1;
		  
		}
	      
	      //fft->SetPoint(in, P1->GetBinContent(eventTeller));
	      fft->SetPoint(in, Signalx[eventTeller-1]);
	      
	      in++;
	      
	    }
	  
	  
	  
	  fft->Transform();
	  
	  
	  Double_t re_x, im_x;
	  Double_t re_y, im_y;
	  Int_t ind;
	  
	  
	  
	  for (Int_t binx=1; binx < numberToAnalyse + 1; binx++) 
	    {
	      
	      ind = binx-1; 
	      fft->GetPointComplex(ind, re_x, im_x);
	      
	      
	      
	      //Amp1->SetBinContent(binx, TMath::Sqrt(re_x*re_x + im_x*im_x));
	      Ampx[binx-1]=TMath::Sqrt(re_x*re_x + im_x*im_x);
	      
	      /*
		if (re_x==0)
		{
		Phase1->SetBinContent(binx, TMath::Pi()/2.);
		}
		else
		{
		Phase1->SetBinContent(binx, TMath::ATan(im_x/re_x));			  
		}
	      */
	      
	      //Phase1->SetBinContent(binx,atan2(im_x,re_x));
	      Phasex[binx-1]=atan2(im_x,re_x);
	      
	    }
	  
	  Int_t in2=0;
	  
	  for (Int_t binx=1; binx < ndim[0] + 1 ; binx++) 
	    {
	      //fft->SetPoint(in2, P2->GetBinContent(binx));
	      fft->SetPoint(in2, Signaly[binx-1]);
	      in2++;
	    }	
	  
	  fft->Transform();
	  
	  
	  
	  Int_t ind2;
	  
	  
	  
	  for (Int_t binx=1; binx < numberToAnalyse + 1; binx++) 
	    {
	      
	      ind2 = binx-1; 
	      fft->GetPointComplex(ind2, re_y, im_y);
	      
	      
	      
	      //Amp2->SetBinContent(binx, TMath::Sqrt(re_y*re_y + im_y*im_y));
	      Ampy[binx-1]=TMath::Sqrt(re_y*re_y + im_y*im_y);
	      
	      /*
		if (re_y==0)
		{
		Phase2->SetBinContent(binx, TMath::Pi()/2.);
		}
		else
		{
		Phase2->SetBinContent(binx, TMath::ATan(im_y/re_y));			  
		}
	      */
	      //Phase2->SetBinContent(binx,atan2(im_y,re_y));
	      Phasey[binx-1]=atan2(im_y,re_y);
	    }
	  
	  


		   
	      
	  for (Int_t binx = 1; binx < numberToAnalyse+1; binx++)
	    {
	      
	      //Dynamic_ampX->SetBinContent(binx,step_aver_fft+1,Amp1->GetBinContent(binx));
	      //Dynamic_phaseX->SetBinContent(binx,step_aver_fft+1,Phase1->GetBinContent(binx));		  
	      //Dynamic_ampY->SetBinContent(binx,step_aver_fft+1,Amp2->GetBinContent(binx));
	      //Dynamic_phaseY->SetBinContent(binx,step_aver_fft+1,Phase2->GetBinContent(binx));
	      
	      Dynamic_ampX_3[binx-1][step_aver_fft] = Ampx[binx-1];
	      Dynamic_phaseX_3[binx-1][step_aver_fft] = Phasex[binx-1];
	      Dynamic_ampY_3[binx-1][step_aver_fft] = Ampy[binx-1];
	      Dynamic_phaseY_3[binx-1][step_aver_fft] = Phasey[binx-1];
								 


	    }
	      
	  //delete Amp1;
	  //delete Phase1;
	  //delete P1;
	  //delete Amp2;
	  //delete Phase2;
	  //delete P2;
	}

      klok9->Stop();
      klok9->Print("u");
      Double_t *thresholdrfi_3 =  new Double_t[3];
      
      thresholdrfi_3[0] = 20.0;
      thresholdrfi_3[1] = 10.0;
      thresholdrfi_3[2] = 5.0;
      
      //Removing RFI 4th frequency.
      for (Int_t iterations = 0; iterations < 2; iterations++)
	{
	  klok5->Start();
	  
	  //Dynamic_ampX->ProjectionX("ProjectionX",1,NumberOfSpectra);
	  //Dynamic_ampY->ProjectionX("ProjectionY",1,NumberOfSpectra);
	  
	  for (Int_t xstep = 1; xstep < numberToAnalyse + 1; xstep++)
	    {
	      
	      Double_t value_x = 0;
	      Double_t value_y = 0;
	      
	      for (Int_t ystep = 1; ystep < NumberOfSpectra + 1; ystep++)
		{
		  //value_x += Dynamic_ampX->GetBinContent(xstep,ystep);
		  //value_y += Dynamic_ampY->GetBinContent(xstep,ystep);
		  value_x += Dynamic_ampX_3[xstep-1][ystep-1];
		  value_y += Dynamic_ampY_3[xstep-1][ystep-1];
		  
		}
	      ProjectionX_3->SetBinContent(xstep,value_x);
	      ProjectionY_3->SetBinContent(xstep,value_y);
	      TotalProjectionX_3->AddBinContent(xstep,value_x);
	      TotalProjectionY_3->AddBinContent(xstep,value_y);
	    }
	  
	  Int_t window = 100;

	  Int_t max_peaks = 80;
	  
	  //	      Double_t *Peakpos_x = new Double_t[max_peaks];
	  
	  //Double_t *Peakheight_x = new Double_t[max_peaks];
	  
	  Double_t *candidates_x =  new Double_t[numberToAnalyse];
	  
	  Double_t *candidates_y =  new Double_t[numberToAnalyse];
	  
	  Double_t *constant_x =  new Double_t[numberToAnalyse/window];
	  
	  Double_t *slope_x =  new Double_t[numberToAnalyse/window];
	  
	  Double_t *constant_y =  new Double_t[numberToAnalyse/window];
	  
	  Double_t *slope_y =  new Double_t[numberToAnalyse/window];
	  
	  Int_t NPeaks_x= 0;
	  
	  //Double_t *Peakpos_y = new Double_t[max_peaks];
	  
	  //Double_t *Peakheight_y = new Double_t[max_peaks];
	  
	  
	  
	  Int_t NPeaks_y = 0;
	  
	  
	  for(Int_t i = 0 ; i <numberToAnalyse;i++)
	    {
	      candidates_x[i] = 0;
	      candidates_y[i] = 0;
	    }
	  
	  
	  
	  TF1 *f1 = new TF1("f1","pol9",0.001,0.5);
	  TF1 *f2 = new TF1("f2","pol9",0.001,0.5);
	  
	  ProjectionX_3->Fit("f1","R,N,Q");
	  ProjectionY_3->Fit("f2","R,N,Q");
	  
	  //Int_t i;
	  //Double_t fit_start = (i+1)/1000.*0.5;
	  //Double_t fit_end = (i + window + 1)/1000.*0.5;
	  //Int_t endpoint = 1001-window;
	  
	  
	      // Double_t average_x = ProjectionX->Integral(i + 1, i + window)/window;
	      // Double_t average_y = ProjectionY->Integral(i + 1, i + window)/window;
	      
	      for(Int_t bin = 1 ; bin < numberToAnalyse + 1 ; bin++)
		{
		  Double_t bincontent_x = ProjectionX_3->GetBinContent(bin);
		  // if(bincontent_x > (100 + thresholdrfi_3)*average_x/100.)
		  //	candidates_x[bin] = bincontent_x;
		  
		  if(bincontent_x > (100 + thresholdrfi_3[iterations])*(f1->Eval(bin*0.5/numberToAnalyse))/100.)
		    { 
		      NPeaks_x++;
		      //candidates_x[bin-1] = f1->Eval(bin*0.5/1001.)/NumberOfSpectra;
		      candidates_x[bin-1] = 1;
		    }
		  Double_t bincontent_y = ProjectionY_3->GetBinContent(bin);
		  // if(bincontent_y > (100 + thresholdrfi_3)*average_y/100.)
		  //	candidates_y[bin] = bincontent_y;
		  
		  if(bincontent_y > (100 + thresholdrfi_3[iterations])*(f2->Eval(bin*0.5/numberToAnalyse))/100.)
		    {
		      //candidates_y[bin-1] = f2->Eval(bin*0.5/1001.)/NumberOfSpectra;
		      candidates_y[bin-1] = 1;
		      NPeaks_y++;
		    }
		}
	      
	   
	  
	  delete f1;
	  delete f2;
	  
	  
	  
	  
	     
	  
	  if ( (filestep % 200) == 0)
	    {
	      
	      for(Int_t bin = 0; bin<numberToAnalyse; bin++)
		{
		  Candidates_x->SetBinContent(bin+1,candidates_x[bin]);
		  Candidates_y->SetBinContent(bin+1,candidates_y[bin]);
		}
	      Candidates_x->Write();
	      Candidates_y->Write();
	    }
	  
	  /* for(Int_t n = 0 ; n < 1000 ; n++)
	     {
	     if(candidates_x[n] > candidates_x[n-1] && candidates_x[n] > candidates_x[n+1])
	     {
	     if(NPeaks_x < max_peaks)
	     
	     {
	     Peakpos_x[NPeaks_x] = n;
	     Peakheight_x[NPeaks_x] = candidates_x[n];
	     NPeaks_x += 1;
	     }
	     else
	     {
	     printf("Warning", "Peak buffer full"); 
	     }
	     }

	     if(candidates_y[n] > candidates_y[n-1] && candidates_y[n] > candidates_y[n+1])
	     {
	     if(NPeaks_y < max_peaks)
			
	     {
	     Peakpos_y[NPeaks_y] = n;
	     Peakheight_y[NPeaks_y] = candidates_y[n];
	     NPeaks_y += 1;
	     }
	     else
	     {
	     printf("Warning", "Peak buffer full"); 
	     }
	     }
	     }*/

	      

	  /*
	    Double_t peak_width_parm_x;
	    Double_t max_peak_x = 0; Double_t min_peak_x=100000000;
	    for (Int_t i = 0; i < NPeaks_x; i++)
	    {
	    if(Peakheight_x[i] >  max_peak_x)max_peak_x = Peakheight_x[i];
	    if(Peakheight_x[i] <  min_peak_x)min_peak_x = Peakheight_x[i];
	    }
	    peak_width_parm_x = sqrt((max_peak_x-min_peak_x)/min_peak_x);
	    //cout << peak_width_parm_x << endl;
	      
	    Double_t peak_width_parm_y;
	    Double_t max_peak_y = 0; Double_t min_peak_y=100000000;
	    for (Int_t i = 0; i < NPeaks_y; i++)
	    {
	    if(Peakheight_y[i] >  max_peak_y)max_peak_y = Peakheight_y[i];
	    if(Peakheight_y[i] <  min_peak_y)min_peak_y = Peakheight_y[i];
	    }
	    peak_width_parm_y = sqrt((max_peak_y-min_peak_y)/min_peak_y);
	    //cout << peak_width_parm_y << endl;
	    Dynamic_ampX
	  */
	  Double_t average_x;
	  Double_t average_y;
	  
	  for (int step_aver_fft = 0; step_aver_fft < NumberOfSpectra; step_aver_fft++)
	    {		   
	      
	      
	      Int_t from;
	      Int_t to;
	      //Dynamic_ampX->SetBinContent(1,step_aver_fft+1, 0);
	      //Dynamic_ampY->SetBinContent(1,step_aver_fft+1, 0);
	      
	      for (Int_t i = 15; i < numberToAnalyse; i++)
		{     
		  
		  if (candidates_x[i]>0)
		    {
		      
		      from = i - 19;
		      to = i;
		      
		      if(from < 2)
			{
			  from = 14;
			  to = i;
			}
		      
		      
		      //average_x = Dynamic_ampX->Integral(from,to,step_aver_fft+1,step_aver_fft+1)/(to-from+1); 
		      
		      Double_t integralx = 0;
		      
		      
		      for (int j = from; j < to+1; j++)
			{
			  integralx += Dynamic_ampX_3[j-1][step_aver_fft];			      
			}
		      
		      //average_x = Dynamic_ampX->Integral(from,to,step_aver_fft+1,step_aver_fft+1)/(to-from+1); 
		      
		      //Dynamic_ampX->SetBinContent(i+1,step_aver_fft+1,average_x);
		      
		      
		      
		      average_x = integralx/(to-from+1);

		      Dynamic_ampX_3[i][step_aver_fft] = average_x;
		      
		    }
		  
		  if (candidates_y[i]>0)
		    {
		      
		      from = i - 19;
		      to = i;
		      
		      if(from < 2)
			{
			  from = 14;
			  to = i;
			}
		      
		      //average_y = Dynamic_ampY->Integral(from,to,step_aver_fft+1,step_aver_fft+1)/(to-from+1); 
		      
		      Double_t integraly = 0;
		      
		      
		      for (int j = from; j < to+1; j++)
			{
			  integraly += Dynamic_ampY_3[j-1][step_aver_fft];			      
			}
		      
		      //average_y = Dynamic_ampY->Integral(from,to,step_aver_fft+1,step_aver_fft+1)/(to-from+1); 
		      
		      //Dynamic_ampY->SetBinContent(i+1,step_aver_fft+1,average_y);
		      
		      
		      
		      average_y = integraly/(to-from+1);
		      
		      Dynamic_ampY_3[i][step_aver_fft] = average_y;
		      
		      
		      
		    }
		  
		  
		    
		      

		    
		    
		}  
		   
		   
		   
	    } 

	  for (Int_t xstep = 0; xstep < numberToAnalyse; xstep++)
	    {
	      
	      Double_t value_x = 0;
	      Double_t value_y = 0;
	      
	      for (Int_t ystep = 0; ystep < NumberOfSpectra; ystep++)
		{
		  value_x += Dynamic_ampX_3[xstep][ystep];
		  value_y += Dynamic_ampY_3[xstep][ystep];
		  
		}
	      TotalProjectionX_3_bgfree->AddBinContent(xstep+1,value_x);
	      TotalProjectionY_3_bgfree->AddBinContent(xstep+1,value_y);
	    }
	      
	  delete candidates_x;	      
	  delete candidates_y;
	      
	  if (iterations == 0) 
	    {
	      N_Peaks_x_1->SetBinContent(filestep+1,NPeaks_x);
	      N_Peaks_y_1->SetBinContent(filestep+1,NPeaks_y);
		
	    }
	  else if (iterations == 1)
	    {
	      N_Peaks_x_2->SetBinContent(filestep+1,NPeaks_x);
	      N_Peaks_y_2->SetBinContent(filestep+1,NPeaks_y);
		
	    }
	  else
	    { 
	      N_Peaks_x_3->SetBinContent(filestep+1,NPeaks_x);
	      N_Peaks_y_3->SetBinContent(filestep+1,NPeaks_y);
		
	    }

	      
	      
	  /*  if ( (filestep % 100) == 0)
	      {
	      ProjectionX->Write();
	      ProjectionY->Write();
	      }*/
	  klok5->Stop();
	  klok5->Print("u");
	}
	
      
      delete thresholdrfi_3;

      Double_t sum_x_3 = 0;

      for (int ystep = 0; ystep < NumberOfSpectra; ystep++)
	{
	  for (int xstep = 0; xstep < numberToAnalyse; xstep++ )
	    {
	      sum_x_3 +=  Dynamic_ampX_3[xstep][ystep];
	    }
	}
      
      if (sum_x_3 == 0)
	{ 	  
	  cout << "zero integral in X pol." << endl;
	  sum_x_3 = 1.0;
	}
      
      //cal_fact_x = 25900.0/sum_x_3;
      cal_fact_x = gain_x_3/sum_x_3;

      //Double_t sum_y = Dynamic_ampY->Integral(1,1000,1,NumberOfSpectra);
      
      Double_t sum_y_3 = 0;
      
      for (int ystep = 0; ystep < NumberOfSpectra; ystep++)
	{
	  for (int xstep = 0; xstep < numberToAnalyse; xstep++ )
	    {
	      sum_y_3 +=  Dynamic_ampY_3[xstep][ystep];
	    }
	}
      
      
      if (sum_y_3 == 0)
	{ 	  
	  cout << "zero integral in Y pol." << endl;
	  sum_y_3 = 1.0;
	}

      cal_fact_y = gain_y_3/sum_y_3;

      cal_factors_x_3->SetBinContent(filestep+1,cal_fact_x);
      cal_factors_y_3->SetBinContent(filestep+1,cal_fact_y);

      if (cal_fact_x > gain_gate_x_3)
	{
	  //cout<< cal_fact_x <<endl;
	  //cout<< "band 4" <<endl;
	  continue;
	}
      
      
           
      
      if (cal_fact_y > gain_gate_y_3)
	{
	  //cout<< cal_fact_x <<endl;
	  //cout<< "band 4" <<endl;
	  continue;
	}

      

      klok10->Start();
      Double_t re_x1[numberToAnalyse],re_y1[numberToAnalyse],im_x1[numberToAnalyse],im_y1[numberToAnalyse];
      Double_t re_x2[numberToAnalyse],re_y2[numberToAnalyse],im_x2[numberToAnalyse],im_y2[numberToAnalyse];
      Double_t re_x3[numberToAnalyse],re_y3[numberToAnalyse],im_x3[numberToAnalyse],im_y3[numberToAnalyse];
      Double_t Ampx;
      Double_t Ampy;
      Double_t Phasex1;	   
      Double_t Phasey1;
      Double_t Phasex2;	   
      Double_t Phasey2;
      Double_t Phasex3;	   
      Double_t Phasey3;
      Double_t data_x_0[TotalNumbers],data_y_0[TotalNumbers];
      Double_t data_x_1[TotalNumbers],data_y_1[TotalNumbers];
      Double_t data_x_21[TotalNumbers],data_y_21[TotalNumbers];
      Double_t data_x_22[TotalNumbers],data_y_22[TotalNumbers];
      Double_t data_x_23[TotalNumbers],data_y_23[TotalNumbers];
      Double_t data_x_31[TotalNumbers],data_y_31[TotalNumbers];
      Double_t data_x_32[TotalNumbers],data_y_32[TotalNumbers];
      Double_t data_x_33[TotalNumbers],data_y_33[TotalNumbers];
      Double_t DM1;
      Double_t DM2;
      Double_t DM3;
      Double_t nu;
      Double_t t0;
      Double_t oldphasey;
      Double_t oldphasex;
      
      
      int number_events = 0, number_events_band1 = 0, number_events_band2 = 0, number_events_band3 = 0;
      
      for (int step_aver_fft_0 = 0; step_aver_fft_0 < NumberOfSpectra; step_aver_fft_0++)
        {
	  Double_t Power;
	  Double_t Power1;
	  Double_t Power2;
	  Double_t Power3;
	  int  rft_band0 = 0, rft_band1 = 0, rft_band2 = 0, rft_band3 = 0;
	  //cout<<  step_aver_fft_0<< endl;

	  //klok11->Start();
	  if (rft_band0 == 0)
	    {
	      for(Int_t stepx=0; stepx < numberToAnalyse; stepx++)
		{
		  //Dedispersion 1st frequency		 
		  oldphasex = Dynamic_phaseX_0[stepx][step_aver_fft_0];
		  oldphasey = Dynamic_phaseY_0[stepx][step_aver_fft_0];
		  DM1 = 1.34e+9*TEC;
		  nu = 155e+6 + stepx*20e+6/(numberToAnalyse - 1);
		  t0 = 8.e+5/3.e+8;			  
		  Dynamic_phaseX_0[stepx][step_aver_fft_0]=fmod(oldphasex + pi + (2*pi*sqrt(1-2*DM1/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
		  Dynamic_phaseY_0[stepx][step_aver_fft_0]=fmod(oldphasey + pi + (2*pi*sqrt(1-2*DM1/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
		  
		  Ampx = Dynamic_ampX_0[stepx][step_aver_fft_0];
		  Ampy = Dynamic_ampY_0[stepx][step_aver_fft_0];
		  Phasex1 = Dynamic_phaseX_0[stepx][step_aver_fft_0];
		  Phasey1 = Dynamic_phaseY_0[stepx][step_aver_fft_0];
		  re_x1[stepx] = Ampx*TMath::Cos(Phasex1);	      
		  im_x1[stepx] = Ampx*TMath::Sin(Phasex1);
		  re_y1[stepx] = Ampy*TMath::Cos(Phasey1);	      
		  im_y1[stepx] = Ampy*TMath::Sin(Phasey1);
		}
	      
	      
	      rft->SetPointsComplex(re_x1,im_x1);
	      
	      rft->Transform();
	      
	      
	      
	      rft->GetPoints(data_x_0);
	      
	      //rft->GetPoints(Signalx);
	      
	      rft->SetPointsComplex(re_y1,im_y1);		    		  					  
	      rft->Transform();
	      
	      
	      
	      rft->GetPoints(data_y_0);
	      rft_band0 = 1;
	      
	      for (int xstep = 1; xstep < TotalNumbers+1; xstep++)
		{		 
		  Px_0->Fill(data_x_0[xstep-1]/TotalNumbers);
		  Py_0->Fill(data_y_0[xstep-1]/TotalNumbers);		  
		}
	      
	      Double_t Powerx;
	      Double_t Powery;
	      for (Int_t step1_0 = 0; step1_0 < TotalNumbers - 5; step1_0++)
		{
		  
		  Power = 0;
	      
		  for(Int_t step2_0 = step1_0; step2_0 < step1_0 + 5; step2_0++)
		    {
		      //Powerx = pow(data_x_0[step2_0]/TotalNumbers,2.0);
		      //Powery = pow(data_y_0[step2_0]/TotalNumbers,2.0);

		      Powerx = (data_x_0[step2_0]/TotalNumbers)*(data_x_0[step2_0]/TotalNumbers);
		      Powery = (data_y_0[step2_0]/TotalNumbers)*(data_y_0[step2_0]/TotalNumbers);

		      Power = Power + Powerx + Powery;
		    }
		  TotalPower_0->Fill(Power);
		  Power_0[step1_0] = Power;
		  
		  //Power_0[step1_0] = 0.0;
		}
	      
	    }
	  
	  //klok11->Stop();
	  //klok11->Print("u");
	  
	  for (Int_t step1_0 = 0; step1_0 < TotalNumbers - 5; step1_0++)
	    {
	      
	      //cout << Power << endl;
	      if(Power_0[step1_0] > Threshold0)
		{
		  number_events_band1++;
		  //cout << "freq1" << endl;
		  
		  if (rft_band1 == 0)
		    {
		  
		      for(Int_t stepx=0; stepx < numberToAnalyse; stepx++)
			{	
			  //Dedispersion 2nd frequency		 
			  oldphasex = Dynamic_phaseX_1[stepx][step_aver_fft_0];
			  oldphasey = Dynamic_phaseY_1[stepx][step_aver_fft_0];
			  DM1 = 1.34e+9*TEC;
			  nu = 141e+6 + stepx*20e+6/(numberToAnalyse - 1);
			  t0 = 8.e+5/3.e+8;			  
			  Dynamic_phaseX_1[stepx][step_aver_fft_0]=fmod(oldphasex + pi + (2*pi*sqrt(1-2*DM1/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
			  Dynamic_phaseY_1[stepx][step_aver_fft_0]=fmod(oldphasey + pi + (2*pi*sqrt(1-2*DM1/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
     
			  Ampx = Dynamic_ampX_1[stepx][step_aver_fft_0];
			  Ampy = Dynamic_ampY_1[stepx][step_aver_fft_0];
			  Phasex1 = Dynamic_phaseX_1[stepx][step_aver_fft_0];
			  Phasey1 = Dynamic_phaseY_1[stepx][step_aver_fft_0];
			  re_x1[stepx] = Ampx*TMath::Cos(Phasex1);	      
			  im_x1[stepx] = Ampx*TMath::Sin(Phasex1);
			  re_y1[stepx] = Ampy*TMath::Cos(Phasey1);	      
			  im_y1[stepx] = Ampy*TMath::Sin(Phasey1);
			}
		      
		      
		      rft->SetPointsComplex(re_x1,im_x1);
		      
		      rft->Transform();
		      
		      
		      
		      rft->GetPoints(data_x_1);
		      
		      //rft->GetPoints(Signalx);
		      
		      rft->SetPointsComplex(re_y1,im_y1);		    		  					  
		      rft->Transform();
		      
		      
		      
		      rft->GetPoints(data_y_1);
		      rft_band1 = 1;

		      for (int xstep = 1; xstep < TotalNumbers+1; xstep++)
			{		 
			  Px_1->Fill(data_x_1[xstep-1]/TotalNumbers);
			  Py_1->Fill(data_y_1[xstep-1]/TotalNumbers);		  
			}

		      Double_t Powerx;
		      Double_t Powery;
		      for (Int_t step = 0; step < TotalNumbers - 5; step++)
			{
			  Power=0;
      
			  for(Int_t step2_0 = step; step2_0 < step + 5; step2_0++)
			    {
			      //Powerx = pow(data_x_1[step2_0]/TotalNumbers,2.0);
			      //Powery = pow(data_y_1[step2_0]/TotalNumbers,2.0);

			       Powerx = (data_x_1[step2_0]/TotalNumbers)*(data_x_1[step2_0]/TotalNumbers);
			       Powery = (data_y_1[step2_0]/TotalNumbers)*(data_y_1[step2_0]/TotalNumbers);

			       Power = Power + Powerx + Powery;
			    }
			  TotalPower_1->Fill(Power);
			  Power_1[step] = Power;
			}   
		    }
		  //cout << "rft" << endl;
		  //Int_t pos_1 = step1_0; //- (int) (1.34e+9*TEC*(1/((120e+6)*(120e+6)) - 1/((135e+6)*(135e+6)))/25e-9);
		  
		  Int_t startloop;
		  Int_t endloop;
		  startloop = step1_0 - delay_1;
		  endloop = step1_0 + delay_1 + 1;
		  if(startloop<0)
		    startloop=0;
		  if(endloop>TotalNumbers-5)
		    endloop = TotalNumbers-5;

		  for(Int_t margin_1 = startloop; margin_1 < endloop; margin_1++)
		    {
		 
		      if(Power_1[margin_1] > Threshold1)
			{
			  number_events_band2++;
			  //cout << Power << endl;
			  //cout << "freq2" << endl;
			  
			  if (rft_band2 == 0)
			    {
			      for(Int_t stepx=0; stepx < numberToAnalyse; stepx++)
				{
				  //Dedispersion 3rd frequency				  
				  oldphasex = Dynamic_phaseX_2[stepx][step_aver_fft_0];
				  oldphasey = Dynamic_phaseY_2[stepx][step_aver_fft_0];
				  DM1 = 1.34e+9*TEC*0.8;
				  DM2 = 1.34e+9*TEC*1.0;
				  DM3 = 1.34e+9*TEC*1.2;
				  nu = 127e+6 + stepx*20e+6/(numberToAnalyse - 1);
				  t0 = 8.e+5/3.e+8;				  
				  Ampx = Dynamic_ampX_2[stepx][step_aver_fft_0];
				  Ampy = Dynamic_ampY_2[stepx][step_aver_fft_0];
				  Phasex1 = fmod(oldphasex + pi + (2*pi*sqrt(1-2*DM1/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
				  Phasey1 = fmod(oldphasey + pi + (2*pi*sqrt(1-2*DM1/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
				  Phasex2 = fmod(oldphasex + pi + (2*pi*sqrt(1-2*DM2/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
				  Phasey2 = fmod(oldphasey + pi + (2*pi*sqrt(1-2*DM2/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
				  Phasex3 = fmod(oldphasex + pi + (2*pi*sqrt(1-2*DM3/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
				  Phasey3 = fmod(oldphasey + pi + (2*pi*sqrt(1-2*DM3/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
				  re_x1[stepx] = Ampx*TMath::Cos(Phasex1);	      
				  im_x1[stepx] = Ampx*TMath::Sin(Phasex1);
				  re_y1[stepx] = Ampy*TMath::Cos(Phasey1);	      
				  im_y1[stepx] = Ampy*TMath::Sin(Phasey1);
				  re_x2[stepx] = Ampx*TMath::Cos(Phasex2);	      
				  im_x2[stepx] = Ampx*TMath::Sin(Phasex2);
				  re_y2[stepx] = Ampy*TMath::Cos(Phasey2);	      
				  im_y2[stepx] = Ampy*TMath::Sin(Phasey2);
				  re_x3[stepx] = Ampx*TMath::Cos(Phasex3);	      
				  im_x3[stepx] = Ampx*TMath::Sin(Phasex3);
				  re_y3[stepx] = Ampy*TMath::Cos(Phasey3);	      
				  im_y3[stepx] = Ampy*TMath::Sin(Phasey3);
				}
			      
			      
			      rft->SetPointsComplex(re_x1,im_x1);
			      
			      rft->Transform();
			      
			      
			      
			      rft->GetPoints(data_x_21);
			      
			      //rft->GetPoints(Signalx);
			      
			      rft->SetPointsComplex(re_y1,im_y1);		    		  					  
			      rft->Transform();
			      
			      
			      
			      rft->GetPoints(data_y_21);

			      rft->SetPointsComplex(re_x2,im_x2);
			      
			      rft->Transform();
			      
			      
			      
			      rft->GetPoints(data_x_22);
			      
			      //rft->GetPoints(Signalx);
			      
			      rft->SetPointsComplex(re_y2,im_y2);		    		  					  
			      rft->Transform();
			      
			      
			      
			      rft->GetPoints(data_y_22);

			      rft->SetPointsComplex(re_x3,im_x3);
			      
			      rft->Transform();
			      
			      
			      
			      rft->GetPoints(data_x_23);
			      
			      //rft->GetPoints(Signalx);
			      
			      rft->SetPointsComplex(re_y3,im_y3);		    		  					  
			      rft->Transform();
			      
			      
			      
			      rft->GetPoints(data_y_23);
			      rft_band2 = 1;

			      for (int xstep = 1; xstep < TotalNumbers+1; xstep++)
				{		 
				  Px_2->Fill(data_x_22[xstep-1]/TotalNumbers);
				  Py_2->Fill(data_y_22[xstep-1]/TotalNumbers);		  
				}

			      Double_t Powerx1;
			      Double_t Powery1;
			      Double_t Powerx2;
			      Double_t Powery2;
			      Double_t Powerx3;
			      Double_t Powery3;
			      for (Int_t step = 0; step < TotalNumbers - 5; step++)
				{
				  Power1=0;
				  Power2=0;
				  Power3=0;

      
				  for(Int_t step2_0 = step; step2_0 < step + 5; step2_0++)
				    {
				      //Powerx = pow(data_x_2[step2_0]/TotalNumbers,2.0);
				      //Powery = pow(data_y_2[step2_0]/TotalNumbers,2.0);

				      Powerx1 = (data_x_21[step2_0]/TotalNumbers)*(data_x_21[step2_0]/TotalNumbers);
				      Powery1 = (data_y_21[step2_0]/TotalNumbers)*(data_y_21[step2_0]/TotalNumbers);
				      
				      Powerx2 = (data_x_22[step2_0]/TotalNumbers)*(data_x_22[step2_0]/TotalNumbers);
				      Powery2 = (data_y_22[step2_0]/TotalNumbers)*(data_y_22[step2_0]/TotalNumbers);

				      Powerx3 = (data_x_23[step2_0]/TotalNumbers)*(data_x_23[step2_0]/TotalNumbers);
				      Powery3 = (data_y_23[step2_0]/TotalNumbers)*(data_y_23[step2_0]/TotalNumbers);

				      Power1 = Power1 + Powerx1 + Powery1;
				      Power2 = Power2 + Powerx2 + Powery2;
				      Power3 = Power3 + Powerx3 + Powery3;
				    }
				  TotalPower_2->Fill(Power2);
				  Power_21[step] = Power1;
				  Power_22[step] = Power2;
				  Power_23[step] = Power3;
				}   
 
			    }
			  //cout << "rft2" << endl;
			  // Int_t pos_2 = step1_0 - (int) (1.34e+9*TEC*(1/((120e+6)*(120e+6)) - 1/((135e+6)*(135e+6)))/25e-9);
			  
			  Int_t begin_2;
			  Int_t end_2;
		      
			 
			  begin_2 = step1_0 - delay_2;
			  end_2 = step1_0 + delay_2;
			  
			  if(begin_2<0)
			    begin_2 = 0;
			  if(end_2>TotalNumbers-5)
			    end_2 = TotalNumbers-5;

			  for (Int_t margin_2 = begin_2; margin_2 < end_2 + 1; margin_2++)
			    {
			      
			      if(Power_21[margin_2] > Threshold2 || Power_22[margin_2] > Threshold2 || Power_23[margin_2] > Threshold2)
			        {
				  number_events_band3++;
				  //cout << "freq3" << endl;
				  
				  if (rft_band3 == 0)
				    {
				      for(Int_t stepx=0; stepx < numberToAnalyse; stepx++)					
					{
					  //Dedispersion 4rd frequency				  
					  oldphasex = Dynamic_phaseX_3[stepx][step_aver_fft_0];
					  oldphasey = Dynamic_phaseY_3[stepx][step_aver_fft_0];
					  DM1 = 1.34e+9*TEC*0.8;
					  DM2 = 1.34e+9*TEC*1.0;
					  DM3 = 1.34e+9*TEC*1.2;
					  nu = 113e+6 + stepx*20e+6/(numberToAnalyse - 1);
					  t0 = 8.e+5/3.e+8;				  
					  Ampx = Dynamic_ampX_3[stepx][step_aver_fft_0];
					  Ampy = Dynamic_ampY_3[stepx][step_aver_fft_0];
					  Phasex1 = fmod(oldphasex + pi + (2*pi*sqrt(1-2*DM1/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
					  Phasey1 = fmod(oldphasey + pi + (2*pi*sqrt(1-2*DM1/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
					  Phasex2 = fmod(oldphasex + pi + (2*pi*sqrt(1-2*DM2/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
					  Phasey2 = fmod(oldphasey + pi + (2*pi*sqrt(1-2*DM2/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
					  Phasex3 = fmod(oldphasex + pi + (2*pi*sqrt(1-2*DM3/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
					  Phasey3 = fmod(oldphasey + pi + (2*pi*sqrt(1-2*DM3/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
					  re_x1[stepx] = Ampx*TMath::Cos(Phasex1);	      
					  im_x1[stepx] = Ampx*TMath::Sin(Phasex1);
					  re_y1[stepx] = Ampy*TMath::Cos(Phasey1);	      
					  im_y1[stepx] = Ampy*TMath::Sin(Phasey1);
					  re_x2[stepx] = Ampx*TMath::Cos(Phasex2);	      
					  im_x2[stepx] = Ampx*TMath::Sin(Phasex2);
					  re_y2[stepx] = Ampy*TMath::Cos(Phasey2);	      
					  im_y2[stepx] = Ampy*TMath::Sin(Phasey2);
					  re_x3[stepx] = Ampx*TMath::Cos(Phasex3);	      
					  im_x3[stepx] = Ampx*TMath::Sin(Phasex3);
					  re_y3[stepx] = Ampy*TMath::Cos(Phasey3);	      
					  im_y3[stepx] = Ampy*TMath::Sin(Phasey3);
					}
				      
				       rft->SetPointsComplex(re_x1,im_x1);
			      
				       rft->Transform();
				       
				       
				       
				       rft->GetPoints(data_x_31);
				       
				       //rft->GetPoints(Signalx);
				       
				       rft->SetPointsComplex(re_y1,im_y1);		    		  					  
				       rft->Transform();
				       
				       
				       
				       rft->GetPoints(data_y_31);
				       
				       rft->SetPointsComplex(re_x2,im_x2);
				       
				       rft->Transform();
				       
				       
				       
				       rft->GetPoints(data_x_32);
				       
				       //rft->GetPoints(Signalx);
				       
				       rft->SetPointsComplex(re_y2,im_y2);		    		  					  
				       rft->Transform();
				       
				       
				       
				       rft->GetPoints(data_y_32);
				       
				       rft->SetPointsComplex(re_x3,im_x3);
				       
				       rft->Transform();
				       
				       
				       
				       rft->GetPoints(data_x_33);
				       
				       //rft->GetPoints(Signalx);
				       
				       rft->SetPointsComplex(re_y3,im_y3);		    		  					  
				       rft->Transform();
				       
				       
				       
				       rft->GetPoints(data_y_33);
				       rft_band3 = 1;
				       

				       Double_t Powerx1;
				       Double_t Powery1;
				       Double_t Powerx2;
				       Double_t Powery2;
				       Double_t Powerx3;
				       Double_t Powery3;
				       

				      for (Int_t step = 0; step < TotalNumbers - 5; step++)
					{
					  Power1=0;
					  Power2=0;
					  Power3=0;
      
					  for(Int_t step2_0 = step; step2_0 < step + 5; step2_0++)
					    {
					      //Powerx = pow(data_x_3[step2_0]/TotalNumbers,2.0);
					      //Powery = pow(data_y_3[step2_0]/TotalNumbers,2.0);

					      Powerx1 = (data_x_31[step2_0]/TotalNumbers)*(data_x_31[step2_0]/TotalNumbers);
					      Powery1 = (data_y_31[step2_0]/TotalNumbers)*(data_y_31[step2_0]/TotalNumbers);
					      Powerx2 = (data_x_32[step2_0]/TotalNumbers)*(data_x_32[step2_0]/TotalNumbers);
					      Powery2 = (data_y_32[step2_0]/TotalNumbers)*(data_y_32[step2_0]/TotalNumbers);
					      Powerx3 = (data_x_33[step2_0]/TotalNumbers)*(data_x_33[step2_0]/TotalNumbers);
					      Powery3 = (data_y_33[step2_0]/TotalNumbers)*(data_y_33[step2_0]/TotalNumbers);



					      Power1 = Power1 + Powerx1 + Powery1;
					      Power2 = Power2 + Powerx2 + Powery2;
					      Power3 = Power3 + Powerx3 + Powery3;
					    }
					  TotalPower_3->Fill(Power2);
					  Power_31[step] = Power1;
					  Power_32[step] = Power2;
					  Power_33[step] = Power3;
					}   

				      for (int xstep = 1; xstep < TotalNumbers+1; xstep++)
					{		 
					  Px_3->Fill(data_x_32[xstep-1]/TotalNumbers);
					  Py_3->Fill(data_y_32[xstep-1]/TotalNumbers);		  
					}
			      
				    }
				  //Int_t pos_3 = step1_0 - (int) (1.34e+9*TEC*(1/((120e+6)*(120e+6)) - 1/((135e+6)*(135e+6)))/25e-9);
				  
				
				  Int_t begin_3;
				  Int_t end_3;
			  
				  
				  begin_3 = step1_0 - delay_3;
				  end_3 = step1_0 + delay_3;
				  
				  if(begin_3<0)
				    begin_3 = 0;
				  if(end_3>TotalNumbers-5)
				    end_3 = TotalNumbers-5;
				  
			  
				  for (Int_t margin_3 = begin_3; margin_3 < end_3 + 1; margin_3++)
				    {			 				      
				      
				      if(Power_31[margin_3] > Threshold3 || Power_32[margin_3] > Threshold3 || Power_33[margin_3] > Threshold3)					
					{

			      
					  number_events++;
			      
					  //cout << "all thresholds:" << endl;
					  // cout << step_aver_fft_0 << endl;
					  //cout << step1_0 << endl;
					  TotalPower->Fill((Power_32[margin_3]+Power_22[margin_2]+Power_1[margin_1]+Power_0[step1_0])/4.);
			     
					  Int_t filepointer = 2*NumberOfSpectra*TotalNumbers*filestep+(2*TotalNumbers*step_aver_fft_0 + step1_0);
					  
					  out_file4.write((char*) &filepointer, sizeof(filepointer));
									 
					  //cout << 2*NumberOfSpectra*TotalNumbers*filestep+(2*TotalNumbers*step_aver_fft_0) << endl;
				  
					  Short_t datax0;
					  Short_t datay0;
					  Short_t datax1;
					  Short_t datay1;
					  Short_t datax2;
					  Short_t datay2;
					  Short_t datax3;
					  Short_t datay3;
 
					  //klok12->Start();
					  
					  for (int xstep = 0; xstep < TotalNumbers; xstep++)
					    {
						if(data_x_0[xstep] > 0.0)					       
						{
						    datax0 = (short)(data_x_0[xstep]/TotalNumbers*50 + 0.5);
						}
						else
						{
						    datax0 = (short)(data_x_0[xstep]/TotalNumbers*50 - 0.5);
						}
						
						if(data_y_0[xstep] > 0.0)
						{
						    datay0 = (short)(data_y_0[xstep]/TotalNumbers*50 + 0.5);
						}
						else
						{
						    datay0 = (short)(data_y_0[xstep]/TotalNumbers*50 - 0.5);
						}
						
						if(data_x_1[xstep] > 0.0)
						{
						    datax1 = (short)(data_x_1[xstep]/TotalNumbers*50 + 0.5);
						}
						else
						{
						    datax1 = (short)(data_x_1[xstep]/TotalNumbers*50 - 0.5);
						}
						
						if(data_y_1[xstep] > 0.0)
						{
						    datay1 = (short)(data_y_1[xstep]/TotalNumbers*50 + 0.5);
						}
						else
						{
						    datay1 = (short)(data_y_1[xstep]/TotalNumbers*50 - 0.5);
						}
						
						if(data_x_22[xstep] > 0.0)
						{
						    datax2 = (short)(data_x_22[xstep]/TotalNumbers*50 + 0.5);
						}
						else
						{
						    datax2 = (short)(data_x_22[xstep]/TotalNumbers*50 - 0.5);
						}
						
						if(data_y_22[xstep] > 0.0)
						{
						    datay2 = (short)(data_y_22[xstep]/TotalNumbers*50 + 0.5);
						}
						else
						{
						    datay2 = (short)(data_y_22[xstep]/TotalNumbers*50 - 0.5);
						}
						
						if(data_x_32[xstep] > 0.0)
						{
						    datax3 = (short)(data_x_32[xstep]/TotalNumbers*50 + 0.5);
						}
						else
						{
						    datax3 = (short)(data_x_32[xstep]/TotalNumbers*50 - 0.5);
						}
						
						if(data_y_32[xstep] > 0.0)
						{
						    datay3 = (short)(data_y_32[xstep]/TotalNumbers*50 + 0.5);
						}
						else
						{
						    datay3 = (short)(data_y_32[xstep]/TotalNumbers*50 - 0.5);
						}

					

					      out_file0.write((char*) &datax0, sizeof(datax0));
					      out_file0.write((char*) &datay0, sizeof(datay0));
					      out_file1.write((char*) &datax1, sizeof(datax1));
					      out_file1.write((char*) &datay1, sizeof(datay1));
					      out_file2.write((char*) &datax2, sizeof(datax2));
					      out_file2.write((char*) &datay2, sizeof(datay2));
					      out_file3.write((char*) &datax3, sizeof(datax3));
					      out_file3.write((char*) &datay3, sizeof(datay3));
					 
				    
					    }// Write out data.
					  
					  //klok12->Stop();
					  //klok12->Print("u");
					  goto label;
					}// If-statement 4th freq.
				      
				    }
				  // Search for events 4th freq.
				  //delete Signal1_3;
				  //delete Signal2_3;
				  margin_2 = margin_2 + 4;
				}// If-statement 3rd freq.
			    }
			  // Search for events 3rd freq.
			  //delete Signal1_2;
			  //delete Signal2_2;
			  margin_1 = margin_1 + 4;
			}// If-statement 2nd freq.
		    }
		  // Search for events 2nd freq.
		  // delete Signal1_1;
		  //delete Signal2_1;
		  step1_0 = step1_0 + 4;
		}// If-statement 1st freq.
	    }// Search for events 1st freq.
	  //delete Signal1_0;
	  //delete Signal2_0;
	label:;
	} // Rft loop 1st freq.
      klok10->Stop();
      klok10->Print("u");
      //cout << "Number of events:"<<endl;
      //cout<<number_events_band1 <<  endl;
      //cout<<number_events_band2 << endl;
      //cout<<number_events_band3 << endl ;
      //cout<<number_events << endl;
      number_lines += number_events;
      if (filestep%10 == 0 )
	{
	  reduction_factor = number_lines / 10.0/ NumberOfSpectra;
	  //cout << reduction_factor << endl;
	  number_lines = 0;

	}
    }// FFT and RFI-removal all frequencies.
  
  TotalProjectionX_0->Write();
  TotalProjectionY_0->Write();
  TotalProjectionX_1->Write();
  TotalProjectionY_1->Write();
  TotalProjectionX_2->Write();
  TotalProjectionY_2->Write();
  TotalProjectionX_3->Write();
  TotalProjectionY_3->Write();
  TotalProjectionX_0_bgfree->Write();
  TotalProjectionY_0_bgfree->Write();
  TotalProjectionX_1_bgfree->Write();
  TotalProjectionY_1_bgfree->Write();
  TotalProjectionX_2_bgfree->Write();
  TotalProjectionY_2_bgfree->Write();
  TotalProjectionX_3_bgfree->Write();
  TotalProjectionY_3_bgfree->Write();
  Px_0->Write();
  Px_1->Write();
  Px_2->Write();
  Px_3->Write();
  Py_0->Write();
  Py_1->Write();
  Py_2->Write();
  Py_3->Write();
  cal_factors_y_3->Write();
  cal_factors_x_3->Write();
  cal_factors_y_2->Write();
  cal_factors_x_2->Write();
  cal_factors_y_1->Write();
  cal_factors_x_1->Write();
  cal_factors_y_0->Write();
  cal_factors_x_0->Write();
  TotalPower_0->Write();
  TotalPower_1->Write();
  TotalPower_2->Write();
  TotalPower_3->Write();
  TotalPower->Write();
  klok1->Stop();
  klok1->Print("u");
  out_file0.close();
  out_file1.close();
  out_file2.close();
  out_file3.close();
  out_file4.close();
  myOutput->Close();
}// End of program.
/*
delete Dynamic_ampX_0;
delete Dynamic_phaseX_0;
delete Dynamic_ampX_1;
delete Dynamic_phaseX_1;
delete Dynamic_ampX_2;
delete Dynamic_phaseX_2;
delete Dynamic_ampX_3;
delete Dynamic_phaseX_3;
delete Dynamic_ampY_0;
delete Dynamic_phaseY_0;
delete Dynamic_ampY_1;
delete Dynamic_phaseY_1;
delete Dynamic_ampY_2;
delete Dynamic_phaseY_2;
delete Dynamic_ampY_3;
delete Dynamic_phaseY_3;
*/
