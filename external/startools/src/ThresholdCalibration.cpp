#include <iostream>

#include <TROOT.h>
#include <TFile.h>
#include <TH2F.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TF1.h>


#include "util.hh"


using namespace std;

//! calibration of the used threshold into ADC or field strength units
/*!
takes some defined events and creates some histograms. The resulting fits are the calibration of the threshold into field strength or ADC units
*/



int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();
  gROOT->SetStyle("Plain");


  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  

  
  char *RunFileName="$HOME/analyisis/default_00.root";
  char *OutputFileName="$HOME/analysis/RunFlag_out.root";
  int NoAnalyse = 1;
  bool debug = false;

  
  
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "hf:i:n:")) != -1) //: after var ->Waits for input
    switch (c)
      {
      case 'f':
	OutputFileName = optarg ;
	break;
      case 'i':
	RunFileName = optarg ;
	break;
      case 'n':
	NoAnalyse = atoi(optarg) ;
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
	cout << " Overview: threshold calibration\n";
	cout << "================================\n\n";
	cout << " -h	HELP -- you already tried :-)\n";
	cout << " -f	output file name (default: " << OutputFileName << ")\n";
	cout << " -i	input file name in STAR format (default: " << RunFileName << ")\n";
	cout << " -n	number of events to analyse (default: " << NoAnalyse << ")\n";
	cout << "\n\n\nmailto: asch@ipe.fzk.de\n";
	exit(1);
        break;      
      default:
	abort ();
      }

  

  int NoChannels=0;
  static struct event event;
  static struct header header;
  static struct channel_profiles channel_profiles;

  TChain *Theader = new TChain("Theader");
  TChain *Tchannel_profile = new TChain("Tchannel_profile");
  TChain *Tevent = new TChain("Tevent");
  CreateTChain(Tevent, RunFileName);

  bool bTeventExtend;
  if(Tevent->FindBranch("run_id")==0) bTeventExtend=false;
  else bTeventExtend=true;
  
  if(bTeventExtend){
    CreateTevent(Tevent, &event);
    CreateTeventExtend(Tevent, &header, &channel_profiles);
    Tevent->GetEntry(0);
    unsigned int tmp=event.event_id;
    NoChannels=0;
    while(tmp==event.event_id){
      NoChannels++;
      if(Tevent->GetEntry(NoChannels)==0) break;
    }
    if(NoChannels==0) {cerr << "NoChannels wrong!" << endl; exit(1); }
  }
  else{
    Theader->Add(RunFileName);
    Tchannel_profile->Add(RunFileName);
  
    CreateTevent(Tevent, &event);
    CreateTheader(Theader, &header);
    CreateTchannel_profiles(Tchannel_profile, &channel_profiles);

    Tevent->GetEntry(0);
    Theader->GetEntry(0);
    NoChannels = Tchannel_profile->GetEntries();
  }

  if(debug) cout << "NoChannels = " << NoChannels << endl;
  TFile *output = new TFile (OutputFileName,"Recreate","ThresholdCalibration");
  
///////////////////////////////   
  int START = 0;
 
  unsigned int event_id_start = 0;
   for(int w=0; w<Tevent->GetEntries(); w++){
     Tevent->GetEntry(w);
     START = w;
     if(event.event_id == event_id_start) break;
     if(event.event_id > event_id_start) break;
   }
   Tevent->GetEntry(0);
   if(debug) cout << START << " START" << endl;

 int STOP =  1;//NoChannels * NoAnalyse;
///////////////////////////////
  
  if(STOP> Tevent->GetEntries()){
   cerr << "entry doesn't exist" << endl;
   STOP = Tevent->GetEntries();
   cerr << "changed end number." <<  endl;
  // exit(1);
  }
  
  if(debug) cout << "STOP " << STOP << endl;
  
  TH2F *his_adc = new TH2F("his_adc","ADC unit", 1000,-500,500, 1000,0,30.0e3); //adc unit
  TH2F *his_fs = new TH2F("his_fs","field strength unit", 1000,-500,500, 1000,0,30.0e3); //field strength unit
  
  int NoZeros = 7;
  int new_window_size = event.window_size*(NoZeros+1);
  float TraceZP_adc[new_window_size];
  float TraceZP_fs[new_window_size];

  float TraceZP_adc_env[new_window_size];
  float TraceZP_fs_env[new_window_size];

  float raw_trace_adc[event.window_size];
  float raw_trace_fs[event.window_size];

  int count_adc_g = 0;
  int count_fs_g = 0;
  float TraceZP_adc_g[new_window_size];
  float TraceZP_fs_g[new_window_size];
  float TraceZP_adc_env_g[new_window_size];
  float TraceZP_fs_env_g[new_window_size];

  int count_adc_g2 = 0;
  int count_fs_g2 = 0;
  float TraceZP_adc_g2[new_window_size];
  float TraceZP_fs_g2[new_window_size];
  float TraceZP_adc_env_g2[new_window_size];
  float TraceZP_fs_env_g2[new_window_size];
  float TraceZP_adc_env_g3[new_window_size];
  
  for(int i=START; i<STOP; i++){
   if(Tevent->GetEntry(i)==0){
     cerr << "error - Tevent->GetEntry()" << endl;
     break;
   }
   
   //RFI suppression
   for(unsigned int j=0; j<event.window_size; j++) raw_trace_adc[j] = event.trace[j];
   RFISuppressionCutOff(event.window_size, raw_trace_adc, raw_trace_adc);
   for(unsigned int j=0; j<event.window_size; j++) raw_trace_fs[j] = raw_trace_adc[j];

   //envelope signal of raw data
   ZeroPaddingFFT(event.window_size, raw_trace_adc, NoZeros, TraceZP_adc);
   for(int j=0; j<new_window_size; j++) {
     TraceZP_adc_env[j] = TraceZP_adc[j];
   }
   Rectifier(NoZeros, new_window_size, TraceZP_adc_env);
  
   //field strength of raw data
   SubtractPedestal(event.window_size, raw_trace_fs);
   ADC2FieldStrength(event.window_size, raw_trace_fs, header.DAQ_id);
   ZeroPaddingFFT(event.window_size, raw_trace_fs, NoZeros, TraceZP_fs);
   for(int j=0; j<new_window_size; j++) {
     TraceZP_fs_env[j] = TraceZP_fs[j];
   }
   Rectifier(NoZeros, new_window_size, TraceZP_fs_env);
   
   for(int j=0; j<new_window_size; j++){
     if(TraceZP_adc_env[j] > 0){
       his_adc->Fill(TraceZP_adc[j], TraceZP_adc_env[j]);
       TraceZP_adc_g[count_adc_g] = TraceZP_adc[j];
       TraceZP_adc_env_g[count_adc_g] = TraceZP_adc_env[j];
       count_adc_g++;
     }
     if(TraceZP_fs_env[j] > 0){
       his_fs->Fill(TraceZP_fs[j], TraceZP_fs_env[j]);
       TraceZP_fs_g[count_fs_g] = TraceZP_fs[j];
       TraceZP_fs_env_g[count_fs_g] = TraceZP_fs_env[j];
       count_fs_g++;
     }
   }
   
   for(int j=2; j<new_window_size; j++){
     if(TraceZP_adc[j-2] < TraceZP_adc[j-1] && TraceZP_adc[j-1] > TraceZP_adc[j] && TraceZP_adc_env[j-1]>0 && TraceZP_adc[j-1]>0) {
       TraceZP_adc_g2[count_adc_g2] = TraceZP_adc[j-1];
       TraceZP_adc_env_g2[count_adc_g2] = TraceZP_adc_env[j-1];
       count_adc_g2++;
     }
     if(TraceZP_fs[j-2] < TraceZP_fs[j-1] && TraceZP_fs[j-1] > TraceZP_fs[j] && TraceZP_fs_env[j-1]>0 && TraceZP_fs[j-1]>0) {
       TraceZP_fs_g2[count_fs_g2] = TraceZP_fs[j-1];
       TraceZP_fs_env_g2[count_fs_g2] = TraceZP_fs_env[j-1];
       TraceZP_adc_env_g3[count_fs_g2] = TraceZP_adc_env[j-1];
       count_fs_g2++;
     }
   }
   
   
  }//end for over data sample
  
  TGraph *g_adc = new TGraph(count_adc_g, TraceZP_adc_g, TraceZP_adc_env_g);
  g_adc->SetTitle("ADC");
  g_adc->SetName("g_adc");
  g_adc->GetXaxis()->SetTitle("samples / adc count");
  g_adc->GetYaxis()->SetTitle("envelope / (adc count)^{2}");
  TGraph *g_fs = new TGraph(count_fs_g, TraceZP_fs_g, TraceZP_fs_env_g);
  g_fs->SetTitle("Field Strength");
  g_fs->SetName("g_fs");
  g_fs->GetXaxis()->SetTitle("field strength / #mu V / m");
  g_fs->GetYaxis()->SetTitle("envelope / (#mu V / m)^{2}");
  TCanvas *can_adc = new TCanvas();
  g_adc->Draw("A*");
  TCanvas *can_fs = new TCanvas();
  g_fs->Draw("A*");

  TF1 fit_adc("fit_adc","[0]*x^2");
  fit_adc.SetParNames("adc_slope");
  TF1 fit_fs("fit_fs","[0]*x^2");
  fit_fs.SetParNames("fs_slope");
  TF1 fit_adc_fs("fit_adc_fs","[0]*x^2");
  fit_adc_fs.SetParNames("slope");
    
  TGraph *g2_adc = new TGraph(count_adc_g2, TraceZP_adc_g2, TraceZP_adc_env_g2);
  g2_adc->SetTitle("ADC");
  g2_adc->SetName("g2_adc");
  g2_adc->GetXaxis()->SetTitle("samples / adc count");
  g2_adc->GetYaxis()->SetTitle("envelope / (adc count)^{2}");
  g2_adc->Fit("fit_adc");
  
  TGraph *g2_fs = new TGraph(count_fs_g2, TraceZP_fs_g2, TraceZP_fs_env_g2);
  g2_fs->SetTitle("Field Strength");
  g2_fs->SetName("g2_fs"); 
  g2_fs->GetXaxis()->SetTitle("field strength / #mu V / m");
  g2_fs->GetYaxis()->SetTitle("envelope / (#mu V / m)^{2}");
  g2_fs->Fit("fit_fs"); 
  
  TCanvas *can2_adc = new TCanvas();
  g2_adc->Draw("A*");
  TCanvas *can2_fs = new TCanvas();
  g2_fs->Draw("A*");
  
  TGraph *g3_adc_fs = new TGraph(count_fs_g2, TraceZP_fs_g2, TraceZP_adc_env_g3);
  g3_adc_fs->SetTitle("ADC vs FS");
  g3_adc_fs->GetXaxis()->SetTitle("field strength / #mu V / m");
  g3_adc_fs->GetYaxis()->SetTitle("envelope / adc count");
  g3_adc_fs->SetName("g3_adc_fs");
  g3_adc_fs->Fit("fit_adc_fs");
  TCanvas *can3_adc_fs = new TCanvas();
  g3_adc_fs->Draw("A*");

      
  output->cd();
  his_adc->Write();
  his_fs->Write();
  can_adc->Write();
  can_fs->Write();
  g_adc->Write();
  g_fs->Write();
  can2_adc->Write();
  can2_fs->Write();
  can3_adc_fs->Write();
  g2_adc->Write();
  g2_fs->Write();
  g3_adc_fs->Write();
  output->Close();

  //do not forget to clean up
  delete output;

//  delete AnaChProf;
//  delete TAnaFlag;

  delete Tevent;
  delete Theader;
  delete Tchannel_profile;
  
}
