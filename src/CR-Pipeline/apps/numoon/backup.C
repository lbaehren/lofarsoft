#include "TH2.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TNtuple.h"
#include "MyObject_newdata.h"
#include "TMath.h"
#include "TSpectrum.h"
#include "TStopwatch.h"
#include "TVirtualFFT.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

//Same list as in analysis_final_v20.C -- didn't check

//This version gives exactly the same result as the old code for Filetotal=1... checking 100


const Long_t numberToAnalyse = 10001, //const Long_t numberToAnalyse = 1024*8 + 1;
             TotalNumbers = numberToAnalyse*2 - 2;
const Int_t NumberOfSpectra = 200, //const Int_t NumberOfSpectra = 1024*40/numberToAnalyse;
            FileTotal = 100, FreqBands = 4;

Double_t gain_x[FreqBands], gain_y[FreqBands];

const Double_t pi = 3.141592653589793238462643383279502884197169;
 
  
char inFileName[FreqBands][256], outFileName[FreqBands][256],
     outFileName4[256];

Double_t Dynamic_ampX[FreqBands][numberToAnalyse][NumberOfSpectra], Dynamic_ampY[FreqBands][numberToAnalyse][NumberOfSpectra], 	 
	 Dynamic_phaseX[FreqBands][numberToAnalyse][NumberOfSpectra], Dynamic_phaseY[FreqBands][numberToAnalyse][NumberOfSpectra], 
         Signalx[TotalNumbers], Signaly[TotalNumbers], Ampx[numberToAnalyse], Phasex[numberToAnalyse], Ampy[numberToAnalyse], Phasey[numberToAnalyse],
         Power_0[TotalNumbers - 5], Power_1[TotalNumbers - 5], Power_21[TotalNumbers - 5], Power_22[TotalNumbers - 5], Power_23[TotalNumbers - 5],
         Power_31[TotalNumbers - 5], Power_32[TotalNumbers - 5], Power_33[TotalNumbers - 5],  
	 reduction_factor, cal_fact_x = 1., cal_fact_y = 1.;

Int_t number_lines=0, ndim[1], ndim2[1];

//why three values and why integers? 

TVirtualFFT *fftx, *ffty;
TVirtualFFT *rft;

  char outFileNameR[256] ="analyses_4freq_new.root";
  
  TFile *myOutput = new TFile(outFileNameR, "RECREATE");
    
  TH1D *Candidates_x = new TH1D("Candidates_x","Candidates_x",numberToAnalyse,0,numberToAnalyse);
  TH1D *Candidates_y = new TH1D("Candidates_y","Candidates_y",numberToAnalyse,0,numberToAnalyse);
  
  TH1D *ProjectionX[FreqBands];
  TH1D *ProjectionY[FreqBands];


  TH1D *TotalProjectionX[FreqBands]; 
  TH1D *TotalProjectionY[FreqBands];

  TH1D *TotalProjectionX_bgfree[FreqBands];
  TH1D *TotalProjectionY_bgfree[FreqBands];
   
  TH1D *N_Peaks_x_1 = new TH1D("N_Peaks_x_1","N_Peaks_x_1",FileTotal,1,FileTotal);
  TH1D *N_Peaks_y_1 = new TH1D("N_Peaks_y_1","N_Peaks_y_1",FileTotal,1,FileTotal);
  TH1D *N_Peaks_x_2 = new TH1D("N_Peaks_x_2","N_Peaks_x_2",FileTotal,1,FileTotal);
  TH1D *N_Peaks_y_2 = new TH1D("N_Peaks_y_2","N_Peaks_y_2",FileTotal,1,FileTotal);
  TH1D *N_Peaks_x_3 = new TH1D("N_Peaks_x_3","N_Peaks_x_3",FileTotal,1,FileTotal);
  TH1D *N_Peaks_y_3 = new TH1D("N_Peaks_y_3","N_Peaks_y_3",FileTotal,1,FileTotal);

  TH1D *cal_factors_x[FreqBands];
  TH1D *cal_factors_y[FreqBands];
  
  TH1D *Px[FreqBands];
  TH1D *Py[FreqBands];

  TH1D *TotalPower[FreqBands];
  TH1D *TotalPowerAll = new TH1D("TotalPower","TotalPower",1000,0,10000); 
  
void FFTChannel(int freq, int filestep, int step_aver_fft)
{
     MyObject_newdata myMoon(inFileName[freq], 4096 + 2*NumberOfSpectra*TotalNumbers*filestep+(2*TotalNumbers*step_aver_fft));
     Bool_t onereadfailed = 0;
     Double_t re_x=0., im_x=0., re_y=0., im_y=0.;
	  
     for (Long_t event = 0; event < TotalNumbers; event++)
     {
	      
	if (myMoon.unpackEventC())
        {
 		 Signalx[event]=(Double_t) (myMoon.getSubEventX());
		 Signaly[event]=(Double_t) (myMoon.getSubEventY());
	}
	else onereadfailed = 1;

	fftx->SetPoint(event, Signalx[event]);
	ffty->SetPoint(event, Signaly[event]);

     }
	  
    fftx->Transform(); ffty->Transform();
    
    
	  
    for (Int_t binx=0; binx < numberToAnalyse; binx++) 
    {
	fftx->GetPointComplex(binx, re_x, im_x);
	ffty->GetPointComplex(binx, re_y, im_y);
	     
	Ampx[binx]=TMath::Sqrt(re_x*re_x + im_x*im_x);
	Ampy[binx]=TMath::Sqrt(re_y*re_y + im_y*im_y);      
	
	Phasex[binx]=atan2(im_x,re_x);
	Phasey[binx]=atan2(im_y,re_y);
	
	Dynamic_ampX[freq][binx][step_aver_fft] = Ampx[binx];
	Dynamic_phaseX[freq][binx][step_aver_fft] = Phasex[binx];
	Dynamic_ampY[freq][binx][step_aver_fft] = Ampy[binx];
	Dynamic_phaseY[freq][binx][step_aver_fft] = Phasey[binx];
    }
   
}

void RemoveRFIFreqChannel(int freq, int filestep)
{
      Double_t *thresholdrfi_0 =  new Double_t[3];
	      
      thresholdrfi_0[0] = 20.0;
      thresholdrfi_0[1] = 10.0;
      thresholdrfi_0[2] = 5.0;

	  
      //Removing RFI 1st frequency.

      Double_t value_x, value_y;

      for (Int_t iterations = 0; iterations < 2; iterations++)	
	{
	  for (Int_t xstep = 0; xstep < numberToAnalyse; xstep++)
	    {
	      value_x = 0;
	      value_y = 0;
	      
	      for (Int_t ystep = 0; ystep < NumberOfSpectra; ystep++)
		{
		  value_x += Dynamic_ampX[freq][xstep][ystep];
		  value_y += Dynamic_ampY[freq][xstep][ystep];
		  
		}
	      ProjectionX[freq]->SetBinContent(xstep+1,value_x);
	      ProjectionY[freq]->SetBinContent(xstep+1,value_y);
	      TotalProjectionX[freq]->AddBinContent(xstep+1,value_x);
	      TotalProjectionY[freq]->AddBinContent(xstep+1,value_y);
	    }
	  
	  //Int_t window = 100, max_peaks = 80;

	  Double_t *candidates_x =  new Double_t[numberToAnalyse];
	  
	  Double_t *candidates_y =  new Double_t[numberToAnalyse];

	  Int_t NPeaks_x = 0, NPeaks_y = 0;
	  
	  
	  for(Int_t i = 0 ; i <numberToAnalyse;i++)
	    {
	      candidates_x[i] = 0;
	      candidates_y[i] = 0;
	    }
	  
	  
	  
	  TF1 *f1 = new TF1("f1","pol9",0.001,0.5);
	  TF1 *f2 = new TF1("f2","pol9",0.001,0.5);
	  
	  ProjectionX[freq]->Fit("f1","R,N,Q");
	  ProjectionY[freq]->Fit("f2","R,N,Q");
   
	      for(Int_t bin = 1  ; bin < numberToAnalyse + 1 ; bin++)
		{
		  Double_t bincontent_x = ProjectionX[freq]->GetBinContent(bin);
		  
		  if(bincontent_x > (100 + thresholdrfi_0[iterations])*(f1->Eval(bin*0.5/numberToAnalyse))/100.)
		    { 
		      NPeaks_x++;
		      candidates_x[bin-1] = 1;
		    }
		  Double_t bincontent_y = ProjectionY[freq]->GetBinContent(bin);
		  if(bincontent_y > (100 + thresholdrfi_0[iterations])*(f2->Eval(bin*0.5/numberToAnalyse))/100.)
		    {
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


	  Double_t average_x;
	  Double_t average_y;
	  
	  Int_t from;
	  Int_t to;
	 
	  Double_t integralx;
	  Double_t integraly;


	  for (int step_aver_fft = 0; step_aver_fft < NumberOfSpectra; step_aver_fft++)
	    {		   
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
	  
		      integralx = 0;
		      
		      
		      for (int j = from; j < to+1; j++)
			{
			  integralx += Dynamic_ampX[freq][j-1][step_aver_fft];			      
			}
		    
		      average_x = integralx/(to-from+1);

		      Dynamic_ampX[freq][i][step_aver_fft] = average_x;
		      
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
		      
		      integraly = 0;
		      
		      
		      for (int j = from; j < to+1; j++)
			{
			  integraly += Dynamic_ampY[freq][j-1][step_aver_fft];			      
			}
		     		      
		      average_y = integraly/(to-from+1);
		      
		      Dynamic_ampY[freq][i][step_aver_fft] = average_y;
		      
		    }
		    
		}  
		   
	    } 

	  Double_t value_x, value_y;

	  for (Int_t xstep = 0; xstep < numberToAnalyse; xstep++)
	    {
	      value_x = 0;
	      value_y = 0;
	      
	      for (Int_t ystep = 0; ystep < NumberOfSpectra; ystep++)
		{
		  value_x += Dynamic_ampX[freq][xstep][ystep];
		  value_y += Dynamic_ampY[freq][xstep][ystep];
		  
		}
	      TotalProjectionX_bgfree[freq]->AddBinContent(xstep+1,value_x);
	      TotalProjectionY_bgfree[freq]->AddBinContent(xstep+1,value_y);
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
	}

      delete thresholdrfi_0;

      Double_t sum_x = 0;

      for (int ystep = 0; ystep < NumberOfSpectra; ystep++)
	{
	  for (int xstep = 0; xstep < numberToAnalyse; xstep++ )
	    {
	      sum_x +=  Dynamic_ampX[freq][xstep][ystep];
	    }
	}
      
      if (sum_x == 0)
	{ 	  
	  cout << "zero integral in X pol." << endl;
	  sum_x = 1.0;
	}
      
      cal_fact_x = gain_x[freq]/sum_x;

      
      Double_t sum_y = 0;
      
      for (int ystep = 0; ystep < NumberOfSpectra; ystep++)
	{
	  for (int xstep = 0; xstep < numberToAnalyse; xstep++ )
	    {
	      sum_y +=  Dynamic_ampY[freq][xstep][ystep];
	    }
	}
      
     
      if (sum_y == 0)
	{ 	  
	  cout << "zero integral in Y pol." << endl;
	  sum_y = 1.0;
	}

      cal_fact_y = gain_y[freq]/sum_y;

      cal_factors_x[freq]->SetBinContent(filestep+1,cal_fact_x);
      cal_factors_y[freq]->SetBinContent(filestep+1,cal_fact_y);
      

//End of RFI removal
}

void analyses_new_v20(Double_t TEC = 0.0, Double_t Threshold0 = 120.0, Double_t Threshold1 = 120.0, Double_t Threshold2 = 120.0, 
Double_t Threshold3 = 120.0)
{

//Skipped Stopwatches here
  Double_t gain_gate_x[4] = {10.0,10.0,10.0,10.0};
  Double_t gain_gate_y[4] = {10.0,10.0,10.0,10.0}; 
 
  //inFileName[0]="/raid/pv2/numoon/freq1.data";
  //inFileName[1]="/raid/pv2/numoon/freq2.data"; 
  //inFileName[2]="/raid/pv2/numoon/freq3.data"; 
  //inFileName[3]="/raid/pv2/numoon/freq4.data"; 
  //outFileName[0]="dada.output.1";
  //outFileName[1]="dada.output.2"; 
  //outFileName[2]="dada.output.3"; 
  //outFileName[3]="dada.output.4"; 
  //outFileName4="dada.time.output";
  //outFileNameR="analyses_4freq_new.root";

  sprintf(inFileName[0], "/raid/pv2/numoon/freq1.data");
  sprintf(inFileName[1], "/raid/pv2/numoon/freq2.data");
  sprintf(inFileName[2], "/raid/pv2/numoon/freq3.data");
  sprintf(inFileName[3], "/raid/pv2/numoon/freq4.data");
  sprintf(outFileName[0], "/raid/pv2/numoon/outp2/dada.output.1");
  sprintf(outFileName[1], "/raid/pv2/numoon/outp2/dada.output.2");
  sprintf(outFileName[2], "/raid/pv2/numoon/outp2/dada.output.3");
  sprintf(outFileName[3], "/raid/pv2/numoon/outp2/dada.output.4");
  sprintf(outFileName4, "/raid/pv2/numoon/outp2/dada.time.output");
  sprintf(outFileNameR, "analyses_4freq_new.root"); 
  
  ofstream out_file0(outFileName[0],ofstream::out|ofstream::binary);
  ofstream out_file1(outFileName[1],ofstream::out|ofstream::binary);
  ofstream out_file2(outFileName[2],ofstream::out|ofstream::binary);
  ofstream out_file3(outFileName[3],ofstream::out|ofstream::binary); 
  ofstream out_file4(outFileName4,ofstream::out|ofstream::binary);
   
  
  ProjectionX[0] = new TH1D("ProjectionX_0","ProjectionX_0",TotalNumbers/2,-0.00025,0.49975);
  ProjectionY[0] = new TH1D("ProjectionY_0","ProjectionY_0",TotalNumbers/2,-0.00025,0.49975);
  ProjectionX[1] = new TH1D("ProjectionX_1","ProjectionX_1",TotalNumbers/2,-0.00025,0.49975);
  ProjectionY[1] = new TH1D("ProjectionY_1","ProjectionY_1",TotalNumbers/2,-0.00025,0.49975);
  ProjectionX[2] = new TH1D("ProjectionX_2","ProjectionX_2",TotalNumbers/2,-0.00025,0.49975);
  ProjectionY[2] = new TH1D("ProjectionY_2","ProjectionY_2",TotalNumbers/2,-0.00025,0.49975);
  ProjectionX[3] = new TH1D("ProjectionX_3","ProjectionX_3",TotalNumbers/2,-0.00025,0.49975);
  ProjectionY[3] = new TH1D("ProjectionY_3","ProjectionY_3",TotalNumbers/2,-0.00025,0.49975);

  TotalProjectionX[0] = new TH1D("TotalProjectionX_0","TotalProjectionX_0",TotalNumbers/2,-0.00025,0.49975);
  TotalProjectionY[0] = new TH1D("TotalProjectionY_0","TotalProjectionY_0",TotalNumbers/2,-0.00025,0.49975);
  TotalProjectionX[1] = new TH1D("TotalProjectionX_1","TotalProjectionX_1",TotalNumbers/2,-0.00025,0.49975);
  TotalProjectionY[1] = new TH1D("TotalProjectionY_1","TotalProjectionY_1",TotalNumbers/2,-0.00025,0.49975);
  TotalProjectionX[2] = new TH1D("TotalProjectionX_2","TotalProjectionX_2",TotalNumbers/2,-0.00025,0.49975);
  TotalProjectionY[2] = new TH1D("TotalProjectionY_2","TotalProjectionY_2",TotalNumbers/2,-0.00025,0.49975);
  TotalProjectionX[3] = new TH1D("TotalProjectionX_3","TotalProjectionX_3",TotalNumbers/2,-0.00025,0.49975);
  TotalProjectionY[3] = new TH1D("TotalProjectionY_3","TotalProjectionY_3",TotalNumbers/2,-0.00025,0.49975);
  TotalProjectionX_bgfree[0] = new TH1D("TotalProjectionX_0_bgfree","TotalProjectionX_0_bgfree",TotalNumbers/2,-0.00025,0.49975);
  TotalProjectionY_bgfree[0] = new TH1D("TotalProjectionY_0_bgfree","TotalProjectionY_0_bgfree",TotalNumbers/2,-0.00025,0.49975);
  TotalProjectionX_bgfree[1] = new TH1D("TotalProjectionX_1_bgfree","TotalProjectionX_1_bgfree",TotalNumbers/2,-0.00025,0.49975);
  TotalProjectionY_bgfree[1] = new TH1D("TotalProjectionY_1_bgfree","TotalProjectionY_1_bgfree",TotalNumbers/2,-0.00025,0.49975);
  TotalProjectionX_bgfree[2] = new TH1D("TotalProjectionX_2_bgfree","TotalProjectionX_2_bgfree",TotalNumbers/2,-0.00025,0.49975);
  TotalProjectionY_bgfree[2] = new TH1D("TotalProjectionY_2_bgfree","TotalProjectionY_2_bgfree",TotalNumbers/2,-0.00025,0.49975);
  TotalProjectionX_bgfree[3] = new TH1D("TotalProjectionX_3_bgfree","TotalProjectionX_3_bgfree",TotalNumbers/2,-0.00025,0.49975);
  TotalProjectionY_bgfree[3] = new TH1D("TotalProjectionY_3_bgfree","TotalProjectionY_3_bgfree",TotalNumbers/2,-0.00025,0.49975);

  cal_factors_x[0] = new TH1D("cal_factors_x_0","cal_factors_x_0",FileTotal,1,FileTotal);
  cal_factors_y[0] = new TH1D("cal_factors_y_0","cal_factors_y_0",FileTotal,1,FileTotal);
  cal_factors_x[1] = new TH1D("cal_factors_x_1","cal_factors_x_1",FileTotal,1,FileTotal);
  cal_factors_y[1] = new TH1D("cal_factors_y_1","cal_factors_y_1",FileTotal,1,FileTotal);
  cal_factors_x[2] = new TH1D("cal_factors_x_2","cal_factors_x_2",FileTotal,1,FileTotal);
  cal_factors_y[2] = new TH1D("cal_factors_y_2","cal_factors_y_2",FileTotal,1,FileTotal);
  cal_factors_x[3] = new TH1D("cal_factors_x_3","cal_factors_x_3",FileTotal,1,FileTotal);
  cal_factors_y[3] = new TH1D("cal_factors_y_3","cal_factors_y_3",FileTotal,1,FileTotal);
  Px[0] = new TH1D("Px_0","Px_0",1000,-49.5,49.5);
  Py[0] = new TH1D("Py_0","Py_0",1000,-49.5,49.5);
  Px[1] = new TH1D("Px_1","Px_1",1000,-49.5,49.5);
  Py[1] = new TH1D("Py_1","Py_1",1000,-49.5,49.5);
  Px[2] = new TH1D("Px_2","Px_2",1000,-49.5,49.5);
  Py[2] = new TH1D("Py_2","Py_2",1000,-49.5,49.5);
  Px[3] = new TH1D("Px_3","Px_3",1000,-49.5,49.5);
  Py[3] = new TH1D("Py_3","Py_3",1000,-49.5,49.5);
  TotalPower[0] = new TH1D("TotalPower_0","TotalPower_0",1000,0,10000);
  TotalPower[1] = new TH1D("TotalPower_1","TotalPower_1",1000,0,10000);
  TotalPower[2] = new TH1D("TotalPower_2","TotalPower_2",1000,0,10000);
  TotalPower[3] = new TH1D("TotalPower_3","TotalPower_3",1000,0,10000);

  ndim[0] = TotalNumbers;
  ndim2[0] = numberToAnalyse;
 
  fftx = TVirtualFFT::FFT(1, ndim, "R2C M");
  ffty = TVirtualFFT::FFT(1, ndim, "R2C M K");
  rft = TVirtualFFT::FFT(1, ndim, "C2R M K");

  gain_x[0] = 6.19084967e+08; gain_y[0] = 5.98952122e+08; gain_x[1] = 7.774859231e+08; gain_y[1] = 7.79267692e+08;
  gain_x[2] = 3.27578819e+08; gain_y[2] = 3.29266576e+08; gain_x[3] = 2.833504e+08; gain_y[3] = 3.16298135e+08;


  Int_t delay_1 = (int)((1.34e+09*0.2*TEC*(1/(151e+06*151e+06)-1/(165e+06*165e+06)))/25e-09 + 1),
      delay_2 = (int)((1.34e+09*0.2*TEC*(1/(137e+06*137e+06)-1/(165e+06*165e+06)))/25e-09 + 1),
      delay_3 = (int)((1.34e+09*0.2*TEC*(1/(123e+06*123e+06)-1/(165e+06*165e+06)))/25e-09 + 1);


  for (int filestep = 0; filestep < FileTotal; filestep++)
  {
    cout << filestep << endl;

      for (int freq =0; freq < 4; freq++)
      {  
	 for (int step_aver_fft = 0; step_aver_fft < NumberOfSpectra; step_aver_fft++)
           {
	     FFTChannel(freq, filestep, step_aver_fft);
	     
	      //if (cal_fact_x > gain_gate_x[freq])
		//{
		  //continue;
		//}
                  
            
 	     //if (cal_fact_y > gain_gate_y[freq])
		//{
		  //continue;
		//}
	   } 
 	     
      RemoveRFIFreqChannel(freq, filestep);
      }
      
      //end loop	  
      
      //Read, FFT, remove RFI for 2nd freq. at myMoon_1(inFileName1, 4096 + 800*numberToAnalyse*filestep).		 
      //Search for event at step_aver_fft and step1 + timedelay1.

      //This will be done with a loop... check for differences
      
      //We are now at line 2731 ... proceeding with dedispersion     
      
      Double_t re_x1[numberToAnalyse],re_y1[numberToAnalyse],im_x1[numberToAnalyse],im_y1[numberToAnalyse];
      Double_t re_x2[numberToAnalyse],re_y2[numberToAnalyse],im_x2[numberToAnalyse],im_y2[numberToAnalyse];
      Double_t re_x3[numberToAnalyse],re_y3[numberToAnalyse],im_x3[numberToAnalyse],im_y3[numberToAnalyse];
      Double_t Ampx, Ampy, Phasex1, Phasey1, Phasex2, Phasey2, Phasex3, Phasey3;
      Double_t data_x_0[TotalNumbers],data_y_0[TotalNumbers];
      Double_t data_x_1[TotalNumbers],data_y_1[TotalNumbers];
      Double_t data_x_21[TotalNumbers],data_y_21[TotalNumbers];
      Double_t data_x_22[TotalNumbers],data_y_22[TotalNumbers];
      Double_t data_x_23[TotalNumbers],data_y_23[TotalNumbers];
      Double_t data_x_31[TotalNumbers],data_y_31[TotalNumbers];
      Double_t data_x_32[TotalNumbers],data_y_32[TotalNumbers];
      Double_t data_x_33[TotalNumbers],data_y_33[TotalNumbers];
      Double_t DM1, DM2, DM3, nu, t0, oldphasey, oldphasex;

      int number_events = 0, number_events_band1 = 0, number_events_band2 = 0, number_events_band3 = 0;
      
      for (int step_aver_fft_0 = 0; step_aver_fft_0 < NumberOfSpectra; step_aver_fft_0++)
        {
	  Double_t Power;
	  Double_t Power1;
	  Double_t Power2;
	  Double_t Power3;
	  int  rft_band0 = 0, rft_band1 = 0, rft_band2 = 0, rft_band3 = 0;
	  
	  if (rft_band0 == 0)
	    {
	      for(Int_t stepx=0; stepx < numberToAnalyse; stepx++)
		{
		  //Dedispersion 1st frequency		 
		  oldphasex = Dynamic_phaseX[0][stepx][step_aver_fft_0];
		  oldphasey = Dynamic_phaseY[0][stepx][step_aver_fft_0];
		  DM1 = 1.34e+9*TEC;
		  nu = 155e+6 + stepx*20e+6/(numberToAnalyse - 1);
		  t0 = 8.e+5/3.e+8;			  
		  Dynamic_phaseX[0][stepx][step_aver_fft_0]=fmod(oldphasex + pi + (2*pi*sqrt(1-2*DM1/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
		  Dynamic_phaseY[0][stepx][step_aver_fft_0]=fmod(oldphasey + pi + (2*pi*sqrt(1-2*DM1/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
		  
		  Ampx = Dynamic_ampX[0][stepx][step_aver_fft_0];
		  Ampy = Dynamic_ampY[0][stepx][step_aver_fft_0];
		  Phasex1 = Dynamic_phaseX[0][stepx][step_aver_fft_0];
		  Phasey1 = Dynamic_phaseY[0][stepx][step_aver_fft_0];
		  re_x1[stepx] = Ampx*TMath::Cos(Phasex1);	      
		  im_x1[stepx] = Ampx*TMath::Sin(Phasex1);
		  re_y1[stepx] = Ampy*TMath::Cos(Phasey1);	      
		  im_y1[stepx] = Ampy*TMath::Sin(Phasey1);
		}
	      
	      
	      rft->SetPointsComplex(re_x1,im_x1);
	      
	      rft->Transform();
	      rft->GetPoints(data_x_0);
	      rft->SetPointsComplex(re_y1,im_y1);		    		  					  
	      rft->Transform();
	      rft->GetPoints(data_y_0);
	      rft_band0 = 1;
	      
	      for (int xstep = 1; xstep < TotalNumbers+1; xstep++)
		{		 
		  Px[0]->Fill(data_x_0[xstep-1]/TotalNumbers);
		  Py[0]->Fill(data_y_0[xstep-1]/TotalNumbers);		  
		}
	      
	      Double_t Powerx;
	      Double_t Powery;
	      for (Int_t step1_0 = 0; step1_0 < TotalNumbers - 5; step1_0++)
		{
		  
		  Power = 0;
	      
		  for(Int_t step2_0 = step1_0; step2_0 < step1_0 + 5; step2_0++)
		    {
         	      Powerx = (data_x_0[step2_0]/TotalNumbers)*(data_x_0[step2_0]/TotalNumbers);
		      Powery = (data_y_0[step2_0]/TotalNumbers)*(data_y_0[step2_0]/TotalNumbers);

		      Power = Power + Powerx + Powery;
		    }
		  TotalPower[0]->Fill(Power);
		  Power_0[step1_0] = Power;

		}
	      
	    }	  
	  for (Int_t step1_0 = 0; step1_0 < TotalNumbers - 5; step1_0++)
	    {
	      if(Power_0[step1_0] > Threshold0)
		{
		  number_events_band1++;
		  if (rft_band1 == 0)
		    {
		  
		      for(Int_t stepx=0; stepx < numberToAnalyse; stepx++)
			{	
			  //Dedispersion 2nd frequency		 
			  oldphasex = Dynamic_phaseX[1][stepx][step_aver_fft_0];
			  oldphasey = Dynamic_phaseY[1][stepx][step_aver_fft_0];
			  DM1 = 1.34e+9*TEC;
			  nu = 141e+6 + stepx*20e+6/(numberToAnalyse - 1);
			  t0 = 8.e+5/3.e+8;			  
			  Dynamic_phaseX[1][stepx][step_aver_fft_0]=fmod(oldphasex + pi + (2*pi*sqrt(1-2*DM1/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
			  Dynamic_phaseY[1][stepx][step_aver_fft_0]=fmod(oldphasey + pi + (2*pi*sqrt(1-2*DM1/(t0)/(nu*nu))*nu*t0 - 2*pi*nu*t0),2*pi);
     
			  Ampx = Dynamic_ampX[1][stepx][step_aver_fft_0];
			  Ampy = Dynamic_ampY[1][stepx][step_aver_fft_0];
			  Phasex1 = Dynamic_phaseX[1][stepx][step_aver_fft_0];
			  Phasey1 = Dynamic_phaseY[1][stepx][step_aver_fft_0];
			  re_x1[stepx] = Ampx*TMath::Cos(Phasex1);	      
			  im_x1[stepx] = Ampx*TMath::Sin(Phasex1);
			  re_y1[stepx] = Ampy*TMath::Cos(Phasey1);	      
			  im_y1[stepx] = Ampy*TMath::Sin(Phasey1);
			}
		      
		      
		      rft->SetPointsComplex(re_x1,im_x1);
		      rft->Transform();
		      rft->GetPoints(data_x_1);
		      rft->SetPointsComplex(re_y1,im_y1);		    		  					  
		      rft->Transform();
		      rft->GetPoints(data_y_1);
		      rft_band1 = 1;

		      for (int xstep = 1; xstep < TotalNumbers+1; xstep++)
			{		 
			  Px[1]->Fill(data_x_1[xstep-1]/TotalNumbers);
			  Py[1]->Fill(data_y_1[xstep-1]/TotalNumbers);		  
			}

		      Double_t Powerx;
		      Double_t Powery;
		      for (Int_t step = 0; step < TotalNumbers - 5; step++)
			{
			  Power=0;
      
			  for(Int_t step2_0 = step; step2_0 < step + 5; step2_0++)
			    {
			       Powerx = (data_x_1[step2_0]/TotalNumbers)*(data_x_1[step2_0]/TotalNumbers);
			       Powery = (data_y_1[step2_0]/TotalNumbers)*(data_y_1[step2_0]/TotalNumbers);

			       Power = Power + Powerx + Powery;
			    }
			  TotalPower[1]->Fill(Power);
			  Power_1[step] = Power;
			}   
		    }
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
			  if (rft_band2 == 0)
			    {
			      for(Int_t stepx=0; stepx < numberToAnalyse; stepx++)
				{
				  //Dedispersion 3rd frequency				  
				  oldphasex = Dynamic_phaseX[2][stepx][step_aver_fft_0];
				  oldphasey = Dynamic_phaseY[2][stepx][step_aver_fft_0];
				  DM1 = 1.34e+9*TEC*0.8;
				  DM2 = 1.34e+9*TEC*1.0;
				  DM3 = 1.34e+9*TEC*1.2;
				  nu = 127e+6 + stepx*20e+6/(numberToAnalyse - 1);
				  t0 = 8.e+5/3.e+8;				  
				  Ampx = Dynamic_ampX[2][stepx][step_aver_fft_0];
				  Ampy = Dynamic_ampY[2][stepx][step_aver_fft_0];
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
			      rft->SetPointsComplex(re_y1,im_y1);		    		  					  
			      rft->Transform();
			      
			      
			      
			      rft->GetPoints(data_y_21);

			      rft->SetPointsComplex(re_x2,im_x2);
			      
			      rft->Transform();
			      
			      
			      
			      rft->GetPoints(data_x_22);
			      rft->SetPointsComplex(re_y2,im_y2);		    		  					  
			      rft->Transform();
			      
			      
			      
			      rft->GetPoints(data_y_22);

			      rft->SetPointsComplex(re_x3,im_x3);
			      
			      rft->Transform();
			      
			      
			      
			      rft->GetPoints(data_x_23);
			      
			      rft->SetPointsComplex(re_y3,im_y3);		    		  					  
			      rft->Transform();
			      
			      
			      
			      rft->GetPoints(data_y_23);
			      rft_band2 = 1;

			      for (int xstep = 1; xstep < TotalNumbers+1; xstep++)
				{		 
				  Px[2]->Fill(data_x_22[xstep-1]/TotalNumbers);
				  Py[2]->Fill(data_y_22[xstep-1]/TotalNumbers);		  
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
				  TotalPower[2]->Fill(Power2);
				  Power_21[step] = Power1;
				  Power_22[step] = Power2;
				  Power_23[step] = Power3;
				}   
 
			    }
			  
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
				  if (rft_band3 == 0)
				    {
				      for(Int_t stepx=0; stepx < numberToAnalyse; stepx++)					
					{
					  //Dedispersion 4rd frequency				  
					  oldphasex = Dynamic_phaseX[3][stepx][step_aver_fft_0];
					  oldphasey = Dynamic_phaseY[3][stepx][step_aver_fft_0];
					  DM1 = 1.34e+9*TEC*0.8;
					  DM2 = 1.34e+9*TEC*1.0;
					  DM3 = 1.34e+9*TEC*1.2;
					  nu = 113e+6 + stepx*20e+6/(numberToAnalyse - 1);
					  t0 = 8.e+5/3.e+8;				  
					  Ampx = Dynamic_ampX[3][stepx][step_aver_fft_0];
					  Ampy = Dynamic_ampY[3][stepx][step_aver_fft_0];
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
				       rft->SetPointsComplex(re_y1,im_y1);		    		  					  
				       rft->Transform();
				       
				       
				       
				       rft->GetPoints(data_y_31);
				       
				       rft->SetPointsComplex(re_x2,im_x2);
				       
				       rft->Transform();
				       
				       
				       
				       rft->GetPoints(data_x_32);
				       

				       rft->SetPointsComplex(re_y2,im_y2);		    		  					  
				       rft->Transform();
				       
				       
				       
				       rft->GetPoints(data_y_32);
				       
				       rft->SetPointsComplex(re_x3,im_x3);
				       
				       rft->Transform();
				       
				       
				       
				       rft->GetPoints(data_x_33);
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
					  TotalPower[3]->Fill(Power2);
					  Power_31[step] = Power1;
					  Power_32[step] = Power2;
					  Power_33[step] = Power3;
					}   

				      for (int xstep = 1; xstep < TotalNumbers+1; xstep++)
					{		 
					  Px[3]->Fill(data_x_32[xstep-1]/TotalNumbers);
					  Py[3]->Fill(data_y_32[xstep-1]/TotalNumbers);		  
					}
			      
				    }
				  Int_t begin_3,end_3;
			  
				  
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
			         	  TotalPowerAll->Fill((Power_32[margin_3]+Power_22[margin_2]+Power_1[margin_1]+Power_0[step1_0])/4.);
			     
					  Int_t filepointer = 2*NumberOfSpectra*TotalNumbers*filestep+(2*TotalNumbers*step_aver_fft_0 + step1_0);
					  
					  out_file4.write((char*) &filepointer, sizeof(filepointer));
					
					  Short_t datax0;
					  Short_t datay0;
					  Short_t datax1;
					  Short_t datay1;
					  Short_t datax2;
					  Short_t datay2;
					  Short_t datax3;
					  Short_t datay3;
 				  
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
      number_lines += number_events;
      if (filestep%10 == 0 )
	{
	  reduction_factor = number_lines / 10.0/ NumberOfSpectra;
	  number_lines = 0;

	}
    }// FFT and RFI-removal all frequencies.
  
  TotalProjectionX[0]->Write();
  TotalProjectionY[0]->Write();
  TotalProjectionX[1]->Write();
  TotalProjectionY[1]->Write();
  TotalProjectionX[2]->Write();
  TotalProjectionY[2]->Write();
  TotalProjectionX[3]->Write();
  TotalProjectionY[3]->Write();
  TotalProjectionX_bgfree[0]->Write();
  TotalProjectionY_bgfree[0]->Write();
  TotalProjectionX_bgfree[1]->Write();
  TotalProjectionY_bgfree[1]->Write();
  TotalProjectionX_bgfree[2]->Write();
  TotalProjectionY_bgfree[2]->Write();
  TotalProjectionX_bgfree[3]->Write();
  TotalProjectionY_bgfree[3]->Write();
  Px[0]->Write();
  Px[1]->Write();
  Px[2]->Write();
  Px[3]->Write();
  Py[0]->Write();
  Py[1]->Write();
  Py[2]->Write();
  Py[3]->Write();
  cal_factors_y[3]->Write();
  cal_factors_x[3]->Write();
  cal_factors_y[2]->Write();
  cal_factors_x[2]->Write();
  cal_factors_y[1]->Write();
  cal_factors_x[1]->Write();
  cal_factors_y[0]->Write();
  cal_factors_x[0]->Write();
  TotalPower[0]->Write();
  TotalPower[1]->Write();
  TotalPower[2]->Write();
  TotalPower[3]->Write();
  TotalPowerAll->Write();
  out_file0.close();
  out_file1.close();
  out_file2.close();
  out_file3.close();
  out_file4.close();
  myOutput->Close();
}// End of program.
