#include <iostream>

#include <TROOT.h>
#include <TFile.h>
#include <TH2F.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TStopwatch.h>
#include <TMath.h>

#include <LopesStar/util.h>

using namespace std;
using namespace TMath;

//! optimale window size
/*!
generate for a given window_size RFI Sup plots with the original window size and smaler ones
*/



int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();
  gROOT->SetStyle("Plain");


  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  

  
  char *RunFileName="$HOME/analyisis/default_00.root";
  char *OutputFileName="$HOME/analysis/RunFlag_out.root";
  bool debug = false;

  
  
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "hf:i:")) != -1) //: after var ->Waits for input
    switch (c)
      {
      case 'f':
	OutputFileName = optarg ;
	break;
      case 'i':
	RunFileName = optarg ;
	break;
      case '?':
        if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\nTry '-h'\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\nTry '-h'\n",
                   optopt);
        return 1;
      case 'h':
        cout << endl;
	cout << " Overview: optimal window size\n";
	cout << "===============================\n\n";
	cout << " -h	HELP -- you already tried :-)\n";
	cout << " -f	output file name (default: " << OutputFileName << ")\n";
	cout << " -i	input file name in STAR format (default: " << RunFileName << ")\n";
	cout << "\n\n\nmailto: asch@ipe.fzk.de\n";
	exit(1);
        break;      
      default:
	abort ();
      }

  

  TChain *Tevent = new TChain("Tevent");
  CreateTChain(Tevent, RunFileName);
  
  TChain *Theader = new TChain("Theader");
  Theader->Add(RunFileName);

  TChain *Tchannel_profile = new TChain("Tchannel_profile");
  Tchannel_profile->Add(RunFileName);

  struct event event;
  struct header header;
  struct channel_profiles channel_profiles;
  
  TFile *output = new TFile (OutputFileName,"Recreate","RunFlagOut");
  
  CreateTevent(Tevent, &event);
  CreateTheader(Theader, &header);
  CreateTchannel_profiles(Tchannel_profile, &channel_profiles);

  Tevent->GetEntry(0);
  Theader->GetEntry(0);
  

  //no of channels per event in this run
  int NoChannels = Tchannel_profile->GetEntries();
  if(debug) cout << "NoChannels = " << NoChannels << endl;
  
///////////////////////////////   
  int START = 0;
 
  unsigned int event_id_start = 1553544;
   for(int w=0; w<Tevent->GetEntries(); w++){
     Tevent->GetEntry(w);
     START = w;
     if(event.event_id == event_id_start) break;
     if(event.event_id > event_id_start) break;
   }
   Tevent->GetEntry(0);
   if(debug) cout << START << " START" << endl;

 int STOP =  START+NoChannels*1; //Tevent->GetEntries();
///////////////////////////////
  
  if(STOP> Tevent->GetEntries()){
   cerr << "entry doesn't exist" << endl;
   STOP = Tevent->GetEntries();
   cerr << "changed end number." <<  endl;
  // exit(1);
  }
  
  if(debug) cout << "STOP " << STOP << endl;
  
  /***************************************/
  int stepsize = 1*1024;
  int startbin = 8*1024;



  /***************************************/
  int tmp_start=0;
  int tmp_stop=0;
  
  //calculate how many window sizes to prozess
  Tevent->GetEntry(0);
  int kwindow_size = event.window_size / stepsize;
    
  //ZeroPadding
  int NoZeros = 7;
  int New_window_size = (NoZeros+1)*event.window_size;
  float *TraceZP[(STOP-START)*(kwindow_size+1)];
  for(int i=0; i<(STOP-START)*(kwindow_size+1); i++) TraceZP[i] = new float [New_window_size];
  float TimeFFTZP[New_window_size];
  for(int i=0; i<New_window_size; i++) TimeFFTZP[i]=(float)i/(NoZeros+1);

  //spectra
  float spectrum[(STOP-START)*(kwindow_size+1)][event.window_size/2];
  float freq[event.window_size/2];
  for(unsigned int i=0; i<event.window_size/2; i++)
    freq[i]=40.0+40.0/(event.window_size/2)*2+(40.0/(event.window_size/2.0))*(i);
  float phase[event.window_size/2], rawfft[2*event.window_size];
  
  //graph data
  float d_ws[kwindow_size+1];
  float d_mean[NoChannels][kwindow_size+1];
  float d_rms[NoChannels][kwindow_size+1];
  float d_RMS_with[NoChannels][kwindow_size+1];
  float d_RMS_without[NoChannels][kwindow_size+1];
  float tmp1_mean, tmp2_mean, tmp1_rms, tmp2_rms;
  unsigned int min_channel_id=999;
  
  //stopwatch
  TStopwatch timer[NoChannels][kwindow_size+1];
  float real_time[NoChannels][kwindow_size+1];
  float  cpu_time[NoChannels][kwindow_size+1];

 
  //define output TGraphs and canvases
  TGraph **gWin = new TGraph *[(STOP-START)*(kwindow_size+1)];
  TCanvas **canWin = new TCanvas *[(STOP-START)*(kwindow_size+1)];
  TGraph **gSpec = new TGraph *[(STOP-START)*(kwindow_size+1)];
  TCanvas **canSpec = new TCanvas *[(STOP-START)*(kwindow_size+1)];
  TGraph **gd_mean = new TGraph *[NoChannels];
  TGraph **gd_rms = new TGraph *[NoChannels];
  TGraph **gd_RMS_with = new TGraph *[NoChannels];
  TGraph **gd_RMS_without = new TGraph *[NoChannels];  
  TGraph **gtime_cpu = new TGraph *[NoChannels];
  TGraph **gtime_real = new TGraph *[NoChannels];
  
  float tmpTrace[event.window_size];
  float tmpTraceSpec[event.window_size];
  char name[1024];
  char title[1024];
  int ChId = 0;
  int window_size = 0;
  float tmpTraceStat[New_window_size];
  
  
  for(int i=0; i<Tchannel_profile->GetEntries(); i++){
     Tchannel_profile->GetEntry(i);
     if(channel_profiles.ch_id < min_channel_id)
       min_channel_id = channel_profiles.ch_id;
  } 
  cout << "min channel id = " << min_channel_id << endl;
   
  for(int i=0; i<(kwindow_size+1); i++){
   printf("\nwithout RFISuppression\t\t\t\t\twith RFISuppression\n");
   printf("Channel\t\twindow_size\tmean\trms\t\tChannel\t\twindow_size\tmean\trms\n");
   if(i==1) window_size = event.window_size+stepsize;
   if(i!=0){
     window_size -= stepsize;
     New_window_size = (NoZeros+1)*window_size;
     for(int c=0; c<New_window_size; c++) TimeFFTZP[c]=(float)c/(NoZeros+1);
     for(int c=0; c<window_size/2; c++)
       freq[c]=40.0+40.0/(window_size/2)*2+(40.0/(window_size/2.0))*(c);

   }
   else  window_size = event.window_size;
   
   d_ws[i] = window_size;
   
   if(debug){
     cout << "kwindow_size = " << kwindow_size << " - i = " << i << endl;
     cout << "window_size = " << window_size << endl;
     cout << "New_window_size = " << New_window_size << endl;
   }
   
   for(int a=START;a<STOP; a++){
    Tevent->GetEntry(a);
    
    ChId=0;
    Tchannel_profile->GetEntry(ChId);
    while( event.channel_profile_id != channel_profiles.ch_id ){
      ChId++;
      if(Tchannel_profile->GetEntry(ChId)==0){
        cerr << "error - Tchannel_profile->GetEntry()" << endl;
        exit(0);
      }
    }
    if(i!=0)sprintf(title,"D%i_%s_%s, %s, run=%i, event_id=%i, ws=%i",header.DAQ_id, channel_profiles.antenna_pos, 
   		channel_profiles.polarisation, event.timestamp, header.run_id, event.event_id, window_size);
    else sprintf(title,"D%i_%s_%s, %s, run=%i, event_id=%i, ws=%i without RFI suppression",header.DAQ_id, channel_profiles.antenna_pos, 
   		channel_profiles.polarisation, event.timestamp, header.run_id, event.event_id, window_size);

    if(debug) cout << title <<  " - " << a-START+(i)*NoChannels << " " << a << "  " << i << endl;
    
    //define readout range
    tmp_start = startbin-window_size/2;
    if( (tmp_start)<0 ) tmp_start = 0;
    tmp_stop = tmp_start + window_size;
    if((unsigned int) tmp_stop > event.window_size) tmp_stop = event.window_size;
    
    for(int j=tmp_start; j<tmp_stop; j++) tmpTrace[j-tmp_start] = (event.trace[j] & 0x0fff);
    
    
    if(debug) cout << "tmp: start:stop  = " << tmp_start << ":" << tmp_stop << " - ws=" << tmp_stop-tmp_start << endl; 
    
    for(int v=0; v<2; v++){
      if(v==1)RFISuppression(window_size, tmpTrace, tmpTrace);
      ZeroPaddingFFT(window_size, tmpTrace, NoZeros, TraceZP[a-START+(i)*NoChannels]);
    
      for(int w=0; w<window_size; w++) tmpTraceStat[w] = Abs(TraceZP[a-START+(i)*NoChannels][w]);
      if(v==0){ tmp1_mean = Mean(window_size,tmpTraceStat); tmp1_rms = RMS(window_size,tmpTraceStat);}
      if(v==1){ tmp2_mean = Mean(window_size,tmpTraceStat); tmp2_rms = RMS(window_size,tmpTraceStat);}
      printf("%s:%s\t%d\t\t%.2f\t%.2f",channel_profiles.antenna_pos, channel_profiles.polarisation, window_size,
       Mean(window_size,tmpTraceStat), RMS(window_size,tmpTraceStat));
      if(v==0) cout << "\t\t";
    }
    cout << endl;
    
    d_mean[event.channel_profile_id-min_channel_id][i] = (tmp1_mean-tmp2_mean)/tmp1_mean;
     d_rms[event.channel_profile_id-min_channel_id][i] = (tmp1_rms-tmp2_rms)/tmp1_rms;
        d_RMS_with[event.channel_profile_id-min_channel_id][i] = tmp2_rms;
     d_RMS_without[event.channel_profile_id-min_channel_id][i] = tmp1_rms;

    //be sure to have the original data
    for(int j=tmp_start; j<tmp_stop; j++) tmpTrace[j-tmp_start] = (event.trace[j] & 0x0fff);
    
    timer[event.channel_profile_id-min_channel_id][i].Start();

    if(i!=0)RFISuppression(window_size, tmpTrace, tmpTrace);
    for(int j=0; j<window_size; j++) tmpTraceSpec[j] = tmpTrace[j];    
    TraceFFT(window_size , tmpTraceSpec , spectrum[a-START+(i)*NoChannels] , phase, rawfft);
    ZeroPaddingFFT(window_size, tmpTrace, NoZeros, TraceZP[a-START+(i)*NoChannels]);

    timer[event.channel_profile_id-min_channel_id][i].Stop();
    
    sprintf(name,"can%03i_ws%i",a-START+(i)*NoChannels,window_size);
    gWin[a-START+(i)*NoChannels] = new TGraph(New_window_size, TimeFFTZP, TraceZP[a-START+(i)*NoChannels]);
    gWin[a-START+(i)*NoChannels]->SetTitle(title);
    gWin[a-START+(i)*NoChannels]->GetXaxis()->SetTitle(" time #upoint 12.5 / ns");
    gWin[a-START+(i)*NoChannels]->GetYaxis()->SetTitle(" ADC value ");
    canWin[a-START+(i)*NoChannels] = new TCanvas();
    canWin[a-START+(i)*NoChannels]->SetTitle(name);
    canWin[a-START+(i)*NoChannels]->SetName(name);

    sprintf(name,"spec%03i_ws%i",a-START+(i)*NoChannels,window_size);
    gSpec[a-START+(i)*NoChannels] = new TGraph(window_size/2, freq, spectrum[a-START+(i)*NoChannels]);
    gSpec[a-START+(i)*NoChannels]->SetTitle(title);
    gSpec[a-START+(i)*NoChannels]->GetXaxis()->SetTitle(" frequency / MHz ");
    gSpec[a-START+(i)*NoChannels]->GetYaxis()->SetTitle(" Amp ");
    canSpec[a-START+(i)*NoChannels] = new TCanvas();
    canSpec[a-START+(i)*NoChannels]->SetTitle(name);
    canSpec[a-START+(i)*NoChannels]->SetName(name);
    
   } //end over START STOP
  }//end for over kwindwo_size

  if(debug) cout << "end of loop over kwindow_size" << endl;
  
  TMultiGraph *mg = new TMultiGraph();
  mg->SetTitle("optimal window size");
  mg->SetName("mg");
  TLegend leg = TLegend(0.87,0.7,0.99,0.99);

  TMultiGraph *mg3 = new TMultiGraph();
  mg3->SetTitle("RMS vs window size");
  mg3->SetName("mg3");
  TLegend leg3 = TLegend(0.87,0.7,0.99,0.99);
  
  for(int i=0; i<NoChannels; i++){
   sprintf(name,"gd_mean%02i",i+min_channel_id);
   gd_mean[i] = new TGraph(kwindow_size+1, d_ws, d_mean[i]);
   gd_mean[i]->SetTitle(name);
   gd_mean[i]->SetName(name);
   gd_mean[i]->SetLineColor(i+1);   
   gd_mean[i]->SetLineWidth(2);
   sprintf(title,"mean: channel %02i",i+min_channel_id);
   mg->Add(gd_mean[i]);
   leg.AddEntry(gd_mean[i],title,"L");
   gd_mean[i]->Write();
   
   sprintf(name,"gd_rms%02i",i+min_channel_id);
   gd_rms[i] = new TGraph(kwindow_size+1, d_ws, d_rms[i]);
   gd_rms[i]->SetTitle(name);
   gd_rms[i]->SetName(name);
   gd_rms[i]->SetLineColor(i+1);
   gd_rms[i]->SetLineStyle(2);
   gd_rms[i]->SetLineWidth(4);
   sprintf(title,"rms: channel %02i",i+min_channel_id);
   mg->Add(gd_rms[i]);
   leg.AddEntry(gd_rms[i],title,"L");
   gd_rms[i]->Write();
   
   sprintf(name,"gd_rms_wiht%02i",i+min_channel_id);
   gd_RMS_with[i] = new TGraph(kwindow_size+1, d_ws, d_RMS_with[i]);
   gd_RMS_with[i]->SetTitle(name);
   gd_RMS_with[i]->SetName(name);
   gd_RMS_with[i]->SetLineColor(i+1);
   gd_RMS_with[i]->SetLineStyle(2);
   gd_RMS_with[i]->SetLineWidth(4);
   sprintf(title,"RMS with RFISupp: channel %02i",i+min_channel_id);
   mg3->Add(gd_RMS_with[i]);
   leg3.AddEntry(gd_RMS_with[i],title,"L");
   gd_RMS_with[i]->Write();

   sprintf(name,"gd_rms_wiht%02i",i+min_channel_id);
   gd_RMS_without[i] = new TGraph(kwindow_size+1, d_ws, d_RMS_without[i]);
   gd_RMS_without[i]->SetTitle(name);
   gd_RMS_without[i]->SetName(name);
   gd_RMS_without[i]->SetLineColor(i+1);
   gd_RMS_without[i]->SetLineWidth(2);
   sprintf(title,"RMS without RFISupp: channel %02i",i+min_channel_id);
   mg3->Add(gd_RMS_without[i]);
   leg3.AddEntry(gd_RMS_without[i],title,"L");
   gd_RMS_without[i]->Write();

 }
 
// mg->GetXaxis()->SetTitle("ich");
// mg->GetYaxis()->SetTitle("relative mean or rms");
 mg->Write();
 mg3->Write();

 TCanvas *gdcan = new TCanvas();
 gdcan->SetName("canmg"); 
 gdcan->SetTitle("canmg");
 gdcan->cd();
 mg->Draw("AL");
 leg.Draw("SAME");
 gdcan->Write();

 TCanvas *gdcan3 = new TCanvas();
 gdcan3->SetName("canmg3"); 
 gdcan3->SetTitle("canmg3");
 gdcan3->cd();
 mg3->Draw("AL");
 leg3.Draw("SAME");
 gdcan3->Write();
 
 
 //stopwatch
  for(int j=0; j<NoChannels; j++)for(int i=0; i<kwindow_size+1; i++) {
    real_time[j][i] = timer[j][i].RealTime();
    cpu_time[j][i] = timer[j][i].CpuTime();    
//    cout << "WS = " << i << " - no channel = " << j  << endl;
//    cout << "Real = " <<  timer[j[i].RealTime();
//    cout << "CPU = " << timer[j][i].CpuTime() << endl;
  }
  TMultiGraph *mg2 = new TMultiGraph();
  mg2->SetTitle("process time");
  mg2->SetName("mg2");
  TLegend leg2 = TLegend(0.87,0.7,0.99,0.99);
  
  for(int i=0; i<NoChannels; i++){
   sprintf(name,"gtime_cpu%02i",i+min_channel_id);
   gtime_cpu[i] = new TGraph(kwindow_size+1, d_ws, cpu_time[i]);
   gtime_cpu[i]->SetTitle(name);
   gtime_cpu[i]->SetName(name);
   gtime_cpu[i]->SetLineColor(i+1);
   gtime_cpu[i]->SetLineWidth(4);
   gtime_cpu[i]->SetLineStyle(2);
   sprintf(title,"cpu time: channel %02i",i+min_channel_id);
   mg2->Add(gtime_cpu[i]);
   leg2.AddEntry(gtime_cpu[i],title,"L");
   gd_mean[i]->Write();
   
   sprintf(name,"gtime_real%02i",i+min_channel_id);
   gtime_real[i] = new TGraph(kwindow_size+1, d_ws, real_time[i]);
   gtime_real[i]->SetTitle(name);
   gtime_real[i]->SetName(name);
   gtime_real[i]->SetLineColor(i+1);
   gtime_real[i]->SetLineWidth(2);
   sprintf(title,"real time: channel %02i",i+min_channel_id);
   mg2->Add(gtime_real[i]);
   leg2.AddEntry(gtime_real[i],title,"L");
   gd_rms[i]->Write();
 }
 mg2->Write();

 TCanvas *gtimecan = new TCanvas();
 gtimecan->SetName("canmg2"); 
 gtimecan->SetTitle("canmg2");
 gtimecan->cd();
 mg2->Draw("AL");
 leg2.Draw("SAME");
 gtimecan->Write();
  

  char AntPos[NoChannels][50];
  char AntPol[NoChannels][50];
  char position[5];
  char pol[50];
  for(int i=0; i<NoChannels; i++){
    if(Tchannel_profile->GetEntry(i)==0){
      cerr << "Tchannel_profile->GetEntry() error!" << endl;
      exit(0);
    }
    strcpy(AntPos[i],channel_profiles.antenna_pos);
    strcpy(AntPol[i],channel_profiles.polarisation);
    strcat(AntPol[i],",");
//cout << AntPos[i] << " " << AntPol[i] << endl;
  }


  //fill and write the output file
  output->cd();
  int division=(int)NoChannels/2;
  if((int)NoChannels%2 != 0 ) division++;

  for(int j=0; j<kwindow_size+1; j++){
    canWin[j*NoChannels]->cd();
    canWin[j*NoChannels]->Divide(2,division);
    canSpec[j*NoChannels]->cd();
    canSpec[j*NoChannels]->Divide(2,division);
    for(int i=0; i<NoChannels; i++){
      sscanf(gWin[i+j*NoChannels]->GetTitle(),"%*c%*i_%c%c%c_%s CH0",&position[0], &position[1], &position[2], pol);
      for(int a=0; a<NoChannels; a++){
        if(!strcmp(position,AntPos[a]) && !strcmp(pol,AntPol[a])){
//cout << a << " " <<  AntPos[a] << " " << AntPol[a] << " " << position << " " << pol << endl;
          canWin[j*NoChannels]->cd(a+1);
          gWin[i+j*NoChannels]->Draw("AL");
          //define user range
//	  gWin[i+j*NoChannels]->GetXaxis()->SetLimits(gWin[i+j*NoChannels]->GetN()/2-stepsize/2,gWin[i+j*NoChannels]->GetN()/2+stepsize/2);
          
	  canSpec[j*NoChannels]->cd(a+1);
	  gPad->SetLogy();
          gSpec[i+j*NoChannels]->Draw("AL");

	}
       }
    }
    canWin[j*NoChannels]->Write();
    canSpec[j*NoChannels]->Write();
  }
  
  output->Close();
  

  //do not forget to clean up
  delete output;

  delete Tevent;
  delete Theader;
  delete Tchannel_profile;
  
}
