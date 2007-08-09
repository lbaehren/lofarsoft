#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TCanvas.h"
#include <stdio.h>
#include <stdlib.h>
#include "TStopwatch.h"
#include <iostream>



void gaussian_sum_4freq_v5_11Aug_jb()
{

  Char_t filename[256], buffer1[256], buffer2[256], buffer3[256], Px[256], Py[256], frequency[10], Totalpower[256], Histx[256], Histy[256], Hist_power[256];

  Int_t empty_files =0, tailx_0 = 0,taily_0 = 0,tailx_1 = 0,taily_1 = 0,tailx_2 = 0,taily_2 = 0,tailx_3 = 0,taily_3 = 0,powertail_0 = 0,powertail_1 = 0,powertail_2 = 0,powertail_3 = 0,sigma_test_0 = 0,sigma_test_1 = 0,sigma_test_2 = 0,sigma_test_3 = 0,mean_test_0 = 0,mean_test_1 = 0,mean_test_2 = 0,mean_test_3 = 0,tail2x,tail2y, threesigma_x, threesigma2_x, threesigma_y, threesigma2_y, gaus_test[4], power_test[4], bin0, bin1, bin2, bin3, test10 = 0, test11 = 0, test12 = 0, test13 = 0, test20 = 0, test21 = 0, test22 = 0, test23 = 0, test2 = 0, test1 = 0;

  Double_t sigmax, sigmay, meanx, meany, new_sigma, resize_scalex, resize_scaley, resize_scalep, bincontent, binwidth, binwidth_p, newbinwidth, newposition, newbin, binoverlap, low_edge, min_x, max_x, tail_check_x, tail_check2_x, min_y, max_y, tail_check_y, tail_check2_y, min_p, max_p, freq_array[4], power0_rms, power0_mean, power0_entries, power1_rms, power1_mean, power1_entries, power2_rms, power2_mean, power2_entries, power3_rms, power3_mean, power3_entries, begin_0, begin_1, begin_2, begin_3, integr_0, integr_1, integr_2, integr_3, fintegral_0, fintegral_1, fintegral_2, fintegral_3, acceptance1_x, acceptance2_y, acceptance2_x, acceptance1_y, testx_0 = 0, testy_0 = 0, testx_1 = 0, testy_1 = 0, testx_2 = 0, testy_2 = 0, testx_3 = 0, testy_3 = 0;
 
  TF1 *f2;
  TStopwatch *klok = new TStopwatch;
  TStopwatch *klok1 = new TStopwatch;
  
  klok->Start();
  

  TFile *g = new TFile("total_power_11Aug_jb.root","RECREATE");
  TH1D* total_amplitude_x_0 = new TH1D("total_amplitude_x_0","total_amplitude_x_0",1000,-49.5,49.5);
  TH1D* total_amplitude_y_0 = new TH1D("total_amplitude_y_0","total_amplitude_y_0",1000,-49.5,49.5);
  TH1D* total_amplitude_x_1 = new TH1D("total_amplitude_x_1","total_amplitude_x_1",1000,-49.5,49.5);
  TH1D* total_amplitude_y_1 = new TH1D("total_amplitude_y_1","total_amplitude_y_1",1000,-49.5,49.5);
  TH1D* total_amplitude_x_2 = new TH1D("total_amplitude_x_2","total_amplitude_x_2",1000,-49.5,49.5);
  TH1D* total_amplitude_y_2 = new TH1D("total_amplitude_y_2","total_amplitude_y_2",1000,-49.5,49.5);
  TH1D* total_amplitude_x_3 = new TH1D("total_amplitude_x_3","total_amplitude_x_3",1000,-49.5,49.5);
  TH1D* total_amplitude_y_3 = new TH1D("total_amplitude_y_3","total_amplitude_y_3",1000,-49.5,49.5);
  TH1D* total_power_0 = new TH1D("total_power_0","total_power_0",1000,0,10000);
  TH1D* total_power_1 = new TH1D("total_power_1","total_power_1",1000,0,10000);
  TH1D* total_power_2 = new TH1D("total_power_2","total_power_2",1000,0,10000);
  TH1D* total_power_3 = new TH1D("total_power_3","total_power_3",1000,0,10000);
  TH1D* newpower_0;
  TH1D* newpower_1;
  TH1D* newpower_2;
  TH1D* newpower_3;
  TH1D* par0_orig_f0 = new TH1D("parameter_0_orig_f0","parameter_0_orig_f0",2160,0,2160);
  TH1D* par0_orig_f1 = new TH1D("parameter_0_orig_f1","parameter_0_orig_f1",2160,0,2160);
  TH1D* par0_orig_f2 = new TH1D("parameter_0_orig_f2","parameter_0_orig_f2",2160,0,2160);
  TH1D* par0_orig_f3 = new TH1D("parameter_0_orig_f3","parameter_0_orig_f3",2160,0,2160);
  TH1D* par1_orig_f0 = new TH1D("parameter_1_orig_f0","parameter_1_orig_f0",2160,0,2160);
  TH1D* par1_orig_f1 = new TH1D("parameter_1_orig_f1","parameter_1_orig_f1",2160,0,2160);
  TH1D* par1_orig_f2 = new TH1D("parameter_1_orig_f2","parameter_1_orig_f2",2160,0,2160);
  TH1D* par1_orig_f3 = new TH1D("parameter_1_orig_f3","parameter_1_orig_f3",2160,0,2160);
  TH1D* par2_orig_f0 = new TH1D("parameter_2_orig_f0","parameter_2_orig_f0",2160,0,2160);
  TH1D* par2_orig_f1 = new TH1D("parameter_2_orig_f1","parameter_2_orig_f1",2160,0,2160);
  TH1D* par2_orig_f2 = new TH1D("parameter_2_orig_f2","parameter_2_orig_f2",2160,0,2160);
  TH1D* par2_orig_f3 = new TH1D("parameter_2_orig_f3","parameter_2_orig_f3",2160,0,2160);
  TH1D* chis_orig_f0 = new TH1D("parameter_chis_orig_f0","parameter_chis_orig_f0",2160,0,2160);
  TH1D* chis_orig_f1 = new TH1D("parameter_chis_orig_f1","parameter_chis_orig_f1",2160,0,2160);
  TH1D* chis_orig_f2 = new TH1D("parameter_chis_orig_f2","parameter_chis_orig_f2",2160,0,2160);
  TH1D* chis_orig_f3 = new TH1D("parameter_chis_orig_f3","parameter_chis_orig_f3",2160,0,2160);
  TH1D* par0_fin_f0 = new TH1D("parameter_0_final_f0","parameter_0_final_f0",2160,0,2160);
  TH1D* par0_fin_f1 = new TH1D("parameter_0_final_f1","parameter_0_final_f1",2160,0,2160);
  TH1D* par0_fin_f2 = new TH1D("parameter_0_final_f2","parameter_0_final_f2",2160,0,2160);
  TH1D* par0_fin_f3 = new TH1D("parameter_0_final_f3","parameter_0_final_f3",2160,0,2160);
  TH1D* par1_fin_f0 = new TH1D("parameter_1_final_f0","parameter_1_final_f0",2160,0,2160);
  TH1D* par1_fin_f1 = new TH1D("parameter_1_final_f1","parameter_1_final_f1",2160,0,2160);
  TH1D* par1_fin_f2 = new TH1D("parameter_1_final_f2","parameter_1_final_f2",2160,0,2160);
  TH1D* par1_fin_f3 = new TH1D("parameter_1_final_f3","parameter_1_final_f3",2160,0,2160);
  TH1D* par2_fin_f0 = new TH1D("parameter_2_final_f0","parameter_2_final_f0",2160,0,2160);
  TH1D* par2_fin_f1 = new TH1D("parameter_2_final_f1","parameter_2_final_f1",2160,0,2160);
  TH1D* par2_fin_f2 = new TH1D("parameter_2_final_f2","parameter_2_final_f2",2160,0,2160);
  TH1D* par2_fin_f3 = new TH1D("parameter_2_final_f3","parameter_2_final_f3",2160,0,2160);
  TH1D* chis_fin_f0 = new TH1D("parameter_chis_final_f0","parameter_chis_final_f0",2160,0,2160);
  TH1D* chis_fin_f1 = new TH1D("parameter_chis_final_f1","parameter_chis_final_f1",2160,0,2160);
  TH1D* chis_fin_f2 = new TH1D("parameter_chis_final_f2","parameter_chis_final_f2",2160,0,2160);
  TH1D* chis_fin_f3 = new TH1D("parameter_chis_final_f3","parameter_chis_final_f3",2160,0,2160);
  TH1D* parameter_0 = new TH1D("parameter_0","parameter_0",2160,0,2160);
  TH1D* parameter_1 = new TH1D("parameter_1","parameter_1",2160,0,2160);
  TH1D* parameter_2 = new TH1D("parameter_2","parameter_2",2160,0,2160);
  TH1D* parameter_3 = new TH1D("parameter_3","parameter_3",2160,0,2160);
  TH1D* integral_0 = new TH1D("integral_0","integral_0",2160,0,2160);
  TH1D* integral_1 = new TH1D("integral_1","integral_1",2160,0,2160);
  TH1D* integral_2 = new TH1D("integral_2","integral_2",2160,0,2160);
  TH1D* integral_3 = new TH1D("integral_3","integral_3",2160,0,2160);
  TH1D* integral_0_jb = new TH1D("integral_0_jb","integral_0_jb",2160,0,2160);
  TH1D* integral_1_jb = new TH1D("integral_1_jb","integral_1_jb",2160,0,2160);
  TH1D* integral_2_jb = new TH1D("integral_2_jb","integral_2_jb",2160,0,2160);
  TH1D* integral_3_jb = new TH1D("integral_3_jb","integral_3_jb",2160,0,2160);
  TH1D* stats_0_jb = new TH1D("stats_0_jb","stats_0_jb",1001,0,1000);
  TH1D* stats_1_jb = new TH1D("stats_1_jb","stats_1_jb",1001,0,1000);
  TH1D* stats_2_jb = new TH1D("stats_2_jb","stats_2_jb",1001,0,1000);
  TH1D* stats_3_jb = new TH1D("stats_3_jb","stats_3_jb",1001,0,1000);

  TH1D* files1 = new TH1D("files1","files1",2187,0,2186);
  TH1D* files2 = new TH1D("files2","files2",2187,0,2186);

  for(Int_t filenumber = 0;filenumber<2160;filenumber++)
    {
      //klok1->Start();
      cout << filenumber << endl;
      sprintf(buffer2,"%d", filenumber);
      sprintf(buffer3,"/analyses_4freq.root");

      if(filenumber < 10)
	{
	  sprintf(buffer1,"/daq/numoon/data_11Aug/side1/00000");
	  strcpy(filename,buffer1);
	  strcat(filename,buffer2);
	  strcat(filename,buffer3);
	}
      else if(filenumber < 100)
	{
	  sprintf(buffer1,"/daq/numoon/data_11Aug/side1/0000");
	  strcpy(filename,buffer1);
	  strcat(filename,buffer2);
	  strcat(filename,buffer3);
	}
      else if(filenumber < 1000)
	{
	  sprintf(buffer1,"/daq/numoon/data_11Aug/side1/000");
	  strcpy(filename,buffer1);
	  strcat(filename,buffer2);
	  strcat(filename,buffer3);
	}
      else
	{
	  sprintf(buffer1,"/daq/numoon/data_11Aug/side1/00");
	  strcpy(filename,buffer1);
	  strcat(filename,buffer2);
	  strcat(filename,buffer3);
	}
      
      


      TFile *f = new TFile(filename,"READ");
            
      g->cd();
      total_amplitude_x_0->SetDirectory(f);
      total_amplitude_y_0->SetDirectory(f);      
      total_amplitude_x_1->SetDirectory(f);
      total_amplitude_y_1->SetDirectory(f);    
      total_amplitude_x_2->SetDirectory(f);
      total_amplitude_y_2->SetDirectory(f);
      total_amplitude_x_3->SetDirectory(f);
      total_amplitude_y_3->SetDirectory(f);
      f->cd();

      for(Int_t i = 0; i<4; i++)
	{
	  gaus_test[i] = 0;
	  power_test[i] = 0;
	}
	  
      for(Int_t freq = 0; freq < 4; freq++)
	{
	  	  
	  for(Int_t i = 0; i<4; i++)
	    {
	      freq_array[i] = 0.0;
	    }

	  freq_array[freq] = 1.0;

	  sprintf(frequency,"%d", freq);
	  sprintf(Px,"Px_");
	  sprintf(Py,"Py_");
	  sprintf(Totalpower,"TotalPower_");

	  strcpy(Histx,Px);
	  strcat(Histx,frequency);
	  strcpy(Histy,Py);
	  strcat(Histy,frequency);
	  strcpy(Hist_power,Totalpower);
	  strcat(Hist_power,frequency);

	  TH1D* gausx = (TH1D*)f->Get(Histx);
	  TH1D* gausy = (TH1D*)f->Get(Histy);
	  TH1D* power = (TH1D*)f->Get(Hist_power);
      
	  if(gausx->Integral(1,1000) == 0.0)
	    {
	      empty_files++;
	      delete gausx;
	      delete gausy;
	      delete power;
	      total_amplitude_x_0->SetDirectory(g);
	      total_amplitude_y_0->SetDirectory(g);
	  
	      total_amplitude_x_1->SetDirectory(g);
	      total_amplitude_y_1->SetDirectory(g);
	  
	      total_amplitude_x_2->SetDirectory(g);
	      total_amplitude_y_2->SetDirectory(g);
	  
	      total_amplitude_x_3->SetDirectory(g);
	      total_amplitude_y_3->SetDirectory(g);	  
	     
	      f->Close();
	      goto label2;
	    }

	  sigmax = gausx->GetRMS();
	  sigmay = gausy->GetRMS();
	  
	  f2 = new TF1("myfunc","gaus",-20.0,20.0);
                
	  gausx->Fit("myfunc","R,O,Q,+");
	  TF1 *myfunc = gausx->GetFunction("myfunc");
	  Double_t ChiS = myfunc->GetChisquare();
	  Double_t par0 = myfunc->GetParameter(0);
	  Double_t par1 = myfunc->GetParameter(1);
	  Double_t par2 = myfunc->GetParameter(2);
	  if (freq==0) {chis_orig_f0->SetBinContent(filenumber+1,ChiS);par0_orig_f0->SetBinContent(filenumber+1,par0);
	  par1_orig_f0->SetBinContent(filenumber+1,par1);par2_orig_f0->SetBinContent(filenumber+1,par2);}
	  if (freq==1)  {chis_orig_f1->SetBinContent(filenumber+1,ChiS);par0_orig_f1->SetBinContent(filenumber+1,par0);
	  par1_orig_f1->SetBinContent(filenumber+1,par1);par2_orig_f1->SetBinContent(filenumber+1,par2);} 
	  if (freq==2)  {chis_orig_f2->SetBinContent(filenumber+1,ChiS);par0_orig_f2->SetBinContent(filenumber+1,par0);
	  par1_orig_f2->SetBinContent(filenumber+1,par1);par2_orig_f2->SetBinContent(filenumber+1,par2);} 
	  if (freq==3)  {chis_orig_f3->SetBinContent(filenumber+1,ChiS);par0_orig_f3->SetBinContent(filenumber+1,par0);
	  par1_orig_f3->SetBinContent(filenumber+1,par1);par2_orig_f3->SetBinContent(filenumber+1,par2);}

	  meanx = gausx->GetMean();
	  meany = gausy->GetMean();
     
	

	  TH1D* newgausx = new TH1D("newgausx","newgausx",1000,-49.5,49.5);
	  TH1D* newgausy = new TH1D("newgausy","newgausy",1000,-49.5,49.5);
	  TH1D* newpower = new TH1D("newpower","newpower",1000,0,10000);

	  new_sigma = 2.0;
      
	  resize_scalex = new_sigma/sigmax;
	  resize_scaley = new_sigma/sigmay;
	  
	  resize_scalep = 20.0/power->GetRMS();

	  //cout<<power->GetRMS()<<endl;
	  //resize_scalep = pow(new_sigma/sigmax,2);
   
	  min_x = gausx->GetXaxis()->GetXmin();
	  max_x = gausx->GetXaxis()->GetXmax();
	  min_y = gausy->GetXaxis()->GetXmin();
	  max_y = gausy->GetXaxis()->GetXmax();
   
	  min_p = power->GetXaxis()->GetXmin();
	  max_p = power->GetXaxis()->GetXmax();
      
	  
	  binwidth = (max_x-min_x)/1000;
	  binwidth_p = (max_p-min_p)/1000;
	  for(Int_t bins = 1; bins < 1001; bins++)
	    {
	      bincontent = gausx->GetBinContent(bins);	 
	      newbinwidth = binwidth*resize_scalex;
	      low_edge = gausx->GetBinCenter(bins) - binwidth/2.;
	      newposition = (low_edge - meanx)*resize_scalex;
	      binoverlap = (binwidth - fmod(newposition - min_x,binwidth))/newbinwidth;
	      newbin = (int)((newposition - min_x)/binwidth + 1);

	      if(binoverlap > 1.0)
		{
		  newgausx->AddBinContent(newbin,bincontent);
		}
	      else
		{
		  newgausx->AddBinContent(newbin,bincontent*binoverlap);
		  newgausx->AddBinContent(newbin + 1,bincontent*(1 - binoverlap)); 
		}
	  
	      bincontent = gausy->GetBinContent(bins);	 
	      newbinwidth = binwidth*resize_scaley;
	      low_edge = gausy->GetBinCenter(bins) - binwidth/2.;
	      newposition = (low_edge - meany)*resize_scaley;
	      binoverlap = (binwidth - fmod(newposition - min_y,binwidth))/newbinwidth;
	      newbin = (int)((newposition - min_y)/binwidth + 1);

	      if(binoverlap > 1.0)
		{
		  newgausy->AddBinContent(newbin,bincontent);
		}
	      else
		{
		  newgausy->AddBinContent(newbin,bincontent*binoverlap);
		  newgausy->AddBinContent(newbin + 1,bincontent*(1 - binoverlap)); 
		}
	
	      bincontent = power->GetBinContent(bins);	  
	      newbinwidth = binwidth_p*resize_scalep;
	      low_edge = power->GetBinCenter(bins) - binwidth/2.;
	      newposition = low_edge*resize_scalep;
	      binoverlap = (binwidth_p - fmod(newposition - min_p,binwidth_p))/newbinwidth;
	      newbin = (int)((newposition - min_p)/binwidth_p + 1);

	      if(binoverlap > 1.0)
		{
		  newpower->AddBinContent(newbin,bincontent);
		}
	      else
		{
		  newpower->AddBinContent(newbin,bincontent*binoverlap);
		  newpower->AddBinContent(newbin + 1,bincontent*(1 - binoverlap)); 
		}

	  
	    }

	  //cout<<newpower->GetRMS()<<endl;

	  if(freq==0)
	    {
	      newpower_0 = (TH1D*)newpower->Clone("newpower_0");
	      newpower_0->SetDirectory(g);
	      g->cd();
	      newpower_0->SetName("newpower_0");
	      newpower_0->SetTitle("newpower_0");
	    }
	  else if(freq==1)
	    {
	      newpower_1 = (TH1D*)newpower->Clone("newpower_1");
	      newpower_1->SetDirectory(g);
	      g->cd();
	      newpower_1->SetName("newpower_1");
	      newpower_1->SetTitle("newpower_1");
	    }
	  else if(freq==2)
	    {
	      newpower_2 = (TH1D*)newpower->Clone("newpower_2");
	      newpower_2->SetDirectory(g);
	      g->cd();	      
	      newpower_2->SetName("newpower_2");
	      newpower_2->SetTitle("newpower_2");
	    }
	  else
	    {
	      newpower_3 = (TH1D*)newpower->Clone("newpower_3");
	      newpower_3->SetDirectory(g);
	      g->cd();
	      newpower_3->SetName("newpower_3");
	      newpower_3->SetTitle("newpower_3");
	    }
	  f->cd();
	  
	  /*
	  power->SetDirectory(g);
	  newpower->SetDirectory(g); 
	  g->cd(); 
	  power->Write(); 
	  newpower->Write(); 
	  f->cd(); 
	  */

	  

	  threesigma_x = (int)((4.8*new_sigma - min_x)/binwidth + 1);
	  threesigma2_x = (int)((-5*new_sigma - min_x)/binwidth + 1);  

	  threesigma_y = (int)((5*new_sigma - min_y)/binwidth + 1);
	  threesigma2_y = (int)((-5*new_sigma - min_y)/binwidth + 1);
	  // Double_t 2sigma_y = (int)((2*new_sigma - min_y)/binwidth_y + 1);

	  acceptance1_x = newgausx->Integral(1,1000);
	  acceptance1_y = acceptance1_x * 0.3e-08;
	  acceptance2_x = 3.4e-07;
	  acceptance2_y = 3.4e-07;

	  if (acceptance1_x > 3.9e08)
	    {
	      newgausx->Fit("myfunc","R,O,Q,+");
	      TF1 *myfunc = newgausx->GetFunction("myfunc");
	      Double_t ChiS = myfunc->GetChisquare();
	      Double_t par0 = myfunc->GetParameter(0);
	      Double_t par1 = myfunc->GetParameter(1);
	      Double_t par2 = myfunc->GetParameter(2);
	      if (freq==0) {chis_fin_f0->SetBinContent(filenumber+1,ChiS);par0_fin_f0->SetBinContent(filenumber+1,par0);
	      par1_fin_f0->SetBinContent(filenumber+1,par1);par2_fin_f0->SetBinContent(filenumber+1,par2);}
	      if (freq==1)  {chis_fin_f1->SetBinContent(filenumber+1,ChiS);par0_fin_f1->SetBinContent(filenumber+1,par0);
	      par1_fin_f1->SetBinContent(filenumber+1,par1);par2_fin_f1->SetBinContent(filenumber+1,par2);} 
	      if (freq==2)  {chis_fin_f2->SetBinContent(filenumber+1,ChiS);par0_fin_f2->SetBinContent(filenumber+1,par0);
	      par1_fin_f2->SetBinContent(filenumber+1,par1);par2_fin_f2->SetBinContent(filenumber+1,par2);} 
	      if (freq==3)  {chis_fin_f3->SetBinContent(filenumber+1,ChiS);par0_fin_f3->SetBinContent(filenumber+1,par0);
	      par1_fin_f3->SetBinContent(filenumber+1,par1);par2_fin_f3->SetBinContent(filenumber+1,par2);}

	      if (freq==0) {integral_0_jb->SetBinContent(filenumber+1,newgausx->Integral(threesigma_x,1000));
	      newbin = (int)(newgausx->Integral(threesigma_x,1000)/acceptance1_y); if (newbin > 999) newbin = 999;
	      stats_0_jb->AddBinContent(newbin+1,1);}
	      if (freq==1)  {integral_1_jb->SetBinContent(filenumber+1,newgausx->Integral(threesigma_x,1000));
	      newbin = (int)(newgausx->Integral(threesigma_x,1000)/acceptance1_y); if (newbin > 999) newbin = 999;
	      stats_1_jb->AddBinContent(newbin+1,1);}
	      if (freq==2)  {integral_2_jb->SetBinContent(filenumber+1,newgausx->Integral(threesigma_x,1000));
	      newbin = (int)(newgausx->Integral(threesigma_x,1000)/acceptance1_y); if (newbin > 999) newbin = 999;
	      stats_2_jb->AddBinContent(newbin+1,1);}
	      if (freq==3)  {integral_3_jb->SetBinContent(filenumber+1,newgausx->Integral(threesigma_x,1000));
	      newbin = (int)(newgausx->Integral(threesigma_x,1000)/acceptance1_y); if (newbin > 999) newbin = 999;
	      stats_3_jb->AddBinContent(newbin+1,1);}
	    }
            
	label:;
   	 
	  total_amplitude_x_0->SetDirectory(g);
	  total_amplitude_y_0->SetDirectory(g);
	  
	  total_amplitude_x_1->SetDirectory(g);
	  total_amplitude_y_1->SetDirectory(g);
	  
	  total_amplitude_x_2->SetDirectory(g);
	  total_amplitude_y_2->SetDirectory(g);
	  
	  total_amplitude_x_3->SetDirectory(g);
	  total_amplitude_y_3->SetDirectory(g);
	  	  
      
	  delete newgausx;
	  delete newgausy;	 
	  delete newpower;	
	  delete gausx;
	  delete gausy;
	  delete power;
	  
	}

      
      
      f->Close();
      g->cd();

     
      f2 = new TF1("f2","expo",100,1000);
                
     
        
       delete newpower_0;
       delete newpower_1;
       delete newpower_2;
       delete newpower_3;
       
    label2:;
    }
 
  g->cd();
  total_amplitude_x_0->Write();
  total_amplitude_y_0->Write();
  total_power_0->Write();
  total_amplitude_x_1->Write();
  total_amplitude_y_1->Write();
  total_power_1->Write();
  total_amplitude_x_2->Write();
  total_amplitude_y_2->Write();
  total_power_2->Write();
  total_amplitude_x_3->Write();
  total_amplitude_y_3->Write();
  total_power_3->Write();
  files1->Write();
  files2->Write();
  parameter_0->Write();
  parameter_1->Write();
  parameter_2->Write();
  parameter_3->Write();
  integral_0->Write();
  integral_1->Write();
  integral_2->Write();
  integral_3->Write();
  integral_0_jb->Write();
  integral_1_jb->Write();
  integral_2_jb->Write();
  integral_3_jb->Write();
  stats_0_jb->Write();
  stats_1_jb->Write();
  stats_2_jb->Write();
  stats_3_jb->Write();
  chis_orig_f0->Write();
  chis_fin_f0->Write();
  chis_orig_f1->Write();
  chis_fin_f1->Write();
  chis_orig_f2->Write();
  chis_fin_f2->Write();
  chis_orig_f3->Write();
  chis_fin_f3->Write();
  par0_orig_f0->Write();
  par0_fin_f0->Write();
  par1_orig_f0->Write();
  par1_fin_f0->Write();
  par2_orig_f0->Write();
  par2_fin_f0->Write();
 par0_orig_f1->Write();
  par0_fin_f1->Write();
  par1_orig_f1->Write();
  par1_fin_f1->Write();
  par2_orig_f1->Write();
  par2_fin_f1->Write();
 par0_orig_f2->Write();
  par0_fin_f2->Write();
  par1_orig_f2->Write();
  par1_fin_f2->Write();
  par2_orig_f2->Write();
  par2_fin_f2->Write();
 par0_orig_f3->Write();
  par0_fin_f3->Write();
  par1_orig_f3->Write();
  par1_fin_f3->Write();
  par2_orig_f3->Write();
  par2_fin_f3->Write();


  delete total_amplitude_x_0;
  delete total_amplitude_y_0;
  delete total_power_0;
  delete total_amplitude_x_1;
  delete total_amplitude_y_1;
  delete total_power_1;
  delete total_amplitude_x_2;
  delete total_amplitude_y_2;
  delete total_power_2;
  delete total_amplitude_x_3;
  delete total_amplitude_y_3;
  delete total_power_3;
  delete files1;  
  delete files2;  
  delete parameter_0;
  delete parameter_1;
  delete parameter_2;
  delete parameter_3;
  g->Close();
 
  klok->Stop();
  klok->Print("u");
 
  cout << "Empty files: \t" << empty_files<< endl;
  cout << "No tails in X_0: \t" << tailx_0 << endl;
  cout << "No tails in X_1: \t" << tailx_1 << endl;
  cout << "No tails in X_2: \t" << tailx_2 << endl;
  cout << "No tails in X_3: \t" << tailx_3 << endl;
  cout << "No tails in Y_0: \t" << taily_0 << endl;
  cout << "No tails in Y_1: \t" << taily_1 << endl;
  cout << "No tails in Y_2: \t" << taily_2 << endl;
  cout << "No tails in Y_3: \t" << taily_3 << endl;
  cout << "Sigma equal in freq 0: \t" << sigma_test_0 << endl;
  cout << "Sigma equal in freq 1: \t" << sigma_test_1 << endl;
  cout << "Sigma equal in freq 2: \t" << sigma_test_2 << endl;
  cout << "Sigma equal in freq 3: \t" << sigma_test_3 << endl;
  cout << "Mean ~ 0 in freq 0: \t" << mean_test_0 << endl;
  cout << "Mean ~ 0 in freq 1: \t" << mean_test_1 << endl;
  cout << "Mean ~ 0 in freq 2: \t" << mean_test_2 << endl;
  cout << "Mean ~ 0 in freq 3: \t" << mean_test_3 << endl;
  cout << "Tails in all powerspectra: \t" << test1 << endl;
  cout << "Tails in all powerspectra + good gaussians freq 0: \t" << test10 << endl;
  cout << "Tails in all powerspectra + good gaussians freq 1: \t" << test11 << endl;
  cout << "Tails in all powerspectra + good gaussians freq 2: \t" << test12 << endl;
  cout << "Tails in all powerspectra + good gaussians freq 3: \t" << test13 << endl;
  cout << "No tails in powerspectra: \t" << test2 << endl;
  cout << "No tails in power + good gaussians freq 0: \t" << test20 << endl;
  cout << "No tails in power + good gaussians freq 1: \t" << test21 << endl;
  cout << "No tails in power + good gaussians freq 2: \t" << test22 << endl;
  cout << "No tails in power + good gaussians freq 3: \t" << test23 << endl;
  cout << testx_0 << "\t" << testy_0 << endl;
  cout << testx_1 << "\t" << testy_1 << endl;
  cout << testx_2 << "\t" << testy_2 << endl;
  cout << testx_3 << "\t" << testy_3 << endl;
}
