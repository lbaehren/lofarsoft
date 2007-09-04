#include <iostream>

#include <fftw3.h>
#include <math.h>
#include <libgen.h>


#include <TROOT.h>
#include <TSystem.h>
#include <TGraph.h>
#include <TFile.h>
#include <time.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TMath.h>
#include <TPad.h>
#include <TRandom.h>

#include <LopesStar/util.h>

using namespace std;
using namespace TMath;

//! analysis of the star traces
/*!

test enviroment for the analysis of the traces to develop new methods for 
new trigger algorithms.

*/

void BandFilterCorrection(int window_size, float *AMP){
  int entries = window_size/2;
  int coeff = 87;
  float freq[coeff], amp[coeff];
  
  freq[0] = 30.299999;  	 amp[0] = 1218768.151393;
  freq[1] = 31.000000;  	 amp[1] = 306560.495653;
  freq[2] = 31.700001;  	 amp[2] = 74147.252367;
  freq[3] = 32.400002;  	 amp[3] = 18078.027034;
  freq[4] = 33.099998;  	 amp[4] = 5446.821113;
  freq[5] = 33.799999;  	 amp[5] = 2320.999103;
  freq[6] = 34.500000;  	 amp[6] = 1196.429087;
  freq[7] = 35.200001;  	 amp[7] = 603.781740;
  freq[8] = 35.900002;  	 amp[8] = 274.149899;
  freq[9] = 36.599998;  	 amp[9] = 111.493367;
  freq[10] = 37.299999;          amp[10] = 41.391478;
  freq[11] = 38.000000;          amp[11] = 15.728452;
  freq[12] = 38.700001;          amp[12] = 7.366701;
  freq[13] = 39.400002;          amp[13] = 4.613896;
  freq[14] = 40.099998;          amp[14] = 3.294585;
  freq[15] = 40.799999;          amp[15] = 2.380137;
  freq[16] = 41.500000;          amp[16] = 1.682065;
  freq[17] = 42.200001;          amp[17] = 1.239149;
  freq[18] = 42.900002;          amp[18] = 0.987201;
  freq[19] = 43.599998;          amp[19] = 0.871976;
  freq[20] = 44.299999;          amp[20] = 0.860194;
  freq[21] = 45.000000;          amp[21] = 0.907093;
  freq[22] = 45.700001;          amp[22] = 0.998122;
  freq[23] = 46.400002;          amp[23] = 1.108862;
  freq[24] = 47.099998;          amp[24] = 1.213148;
  freq[25] = 47.799999;          amp[25] = 1.284451;
  freq[26] = 48.500000;          amp[26] = 1.315503;
  freq[27] = 49.200001;          amp[27] = 1.304083;
  freq[28] = 49.900002;          amp[28] = 1.253538;
  freq[29] = 50.599998;          amp[29] = 1.182077;
  freq[30] = 51.299999;          amp[30] = 1.091553;
  freq[31] = 52.000000;          amp[31] = 1.010240;
  freq[32] = 52.700001;          amp[32] = 0.936477;
  freq[33] = 53.400002;          amp[33] = 0.876125;
  freq[34] = 54.099998;          amp[34] = 0.834515;
  freq[35] = 54.799999;          amp[35] = 0.803450;
  freq[36] = 55.500000;          amp[36] = 0.776221;
  freq[37] = 56.200001;          amp[37] = 0.762130;
  freq[38] = 56.900002;          amp[38] = 0.759089;
  freq[39] = 57.599998;          amp[39] = 0.771291;
  freq[40] = 58.299999;          amp[40] = 0.799307;
  freq[41] = 59.000000;          amp[41] = 0.851746;
  freq[42] = 59.700001;          amp[42] = 0.923664;
  freq[43] = 60.400002;          amp[43] = 1.008439;
  freq[44] = 61.099998;          amp[44] = 1.108316;
  freq[45] = 61.799999;          amp[45] = 1.212435;
  freq[46] = 62.500000;          amp[46] = 1.292366;
  freq[47] = 63.200001;          amp[47] = 1.354284;
  freq[48] = 63.900002;          amp[48] = 1.382960;
  freq[49] = 64.599998;          amp[49] = 1.382165;
  freq[50] = 65.300003;          amp[50] = 1.347646;
  freq[51] = 66.000000;          amp[51] = 1.277612;
  freq[52] = 66.699997;          amp[52] = 1.194693;
  freq[53] = 67.400002;          amp[53] = 1.101665;
  freq[54] = 68.099998;          amp[54] = 1.014812;
  freq[55] = 68.800003;          amp[55] = 0.939328;
  freq[56] = 69.500000;          amp[56] = 0.888865;
  freq[57] = 70.199997;          amp[57] = 0.863633;
  freq[58] = 70.900002;          amp[58] = 0.866733;
  freq[59] = 71.599998;          amp[59] = 0.900236;
  freq[60] = 72.300003;          amp[60] = 0.967310;
  freq[61] = 73.000000;          amp[61] = 1.070199;
  freq[62] = 73.699997;          amp[62] = 1.210228;
  freq[63] = 74.400002;          amp[63] = 1.386098;
  freq[64] = 75.099998;          amp[64] = 1.594001;
  freq[65] = 75.800003;          amp[65] = 1.824205;
  freq[66] = 76.500000;          amp[66] = 2.023807;
  freq[67] = 77.199997;          amp[67] = 2.218886;
  freq[68] = 77.900002;          amp[68] = 2.369160;
  freq[69] = 78.599998;          amp[69] = 2.540337;
  freq[70] = 79.300003;          amp[70] = 2.930609;
  freq[71] = 80.000000;          amp[71] = 3.911054;
  freq[72] = 80.699997;          amp[72] = 6.637370;
  freq[73] = 81.400002;          amp[73] = 14.807713;
  freq[74] = 82.099998;          amp[74] = 40.834473;
  freq[75] = 82.800003;          amp[75] = 126.227490;
  freq[76] = 83.500000;          amp[76] = 404.096404;
  freq[77] = 84.199997;          amp[77] = 1274.914196;
  freq[78] = 84.900002;          amp[78] = 3954.948971;
  freq[79] = 85.599998;          amp[79] = 11936.830804;
  freq[80] = 86.300003;          amp[80] = 35446.730809;
  freq[81] = 87.000000;          amp[81] = 104194.769140;
  freq[82] = 87.699997;          amp[82] = 309168.389463;
  freq[83] = 88.400002;          amp[83] = 928164.696764;
  freq[84] = 89.099998;          amp[84] = 2915911.391312;
  freq[85] = 89.800003;          amp[85] = 9371884.654148;
  freq[86] = 90.500000;          amp[86] = 33150513.873813;
  
  float BandFreq[entries];
  for(int i=0; i<entries; i++) BandFreq[i]=40.0+40.0/(entries)*2+(40.0/(entries))*(i);
   
  for(int i=0; i<entries; i++){
    if(BandFreq[i]<42) continue;
    if(BandFreq[i]>73) break;
    for(int j=0; j<coeff; j++){
      if(freq[j]>BandFreq[i]){
        //linear interpolation
        AMP[i] *= (( (amp[j-1]-amp[j]) / (freq[j-1]-freq[j]))*(BandFreq[i]-freq[j])+amp[j]);
	break;
      }
    }
  }

  
}
   
   
int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();
  gROOT->SetStyle("Plain");


  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  

  
  char *StarFileName="$HOME/analyisis/default_00.root";
  char *OutputFileName="$HOME/analysis/StarTrace_out.root";
  char name[1024];
  
  
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "hf:i:")) != -1) //: after var ->Waits for input
    switch (c)
      {
      case 'f':
	OutputFileName = optarg ;
	break;
      case 'i':
	StarFileName = optarg ;
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
	cout << " Overview: STAR trace analysis\n";
	cout << "==============================\n\n";
	cout << " -h	HELP -- you already tried :-)\n";
	cout << " -f	output file name (default: " << OutputFileName << ")\n";
	cout << " -i	input file name in STAR format (default: " << StarFileName << ")\n";
	cout << "\n\n\nmailto: asch@ipe.fzk.de\n";
	exit(1);
        break;      
      default:
	abort ();
      }

  

  int NoChannels=0;
  struct event event;
  struct header header;
  struct channel_profiles channel_profiles;

  TChain *Theader = new TChain("Theader");
  TChain *Tchannel_profile = new TChain("Tchannel_profile");
  TChain *Tevent = new TChain("Tevent");
  CreateTChain(Tevent, StarFileName);
  
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
    Theader->Add(StarFileName);
    Tchannel_profile->Add(StarFileName);
  
    CreateTevent(Tevent, &event);
    CreateTheader(Theader, &header);
    CreateTchannel_profiles(Tchannel_profile, &channel_profiles);

    Tevent->GetEntry(0);
    Theader->GetEntry(0);
    NoChannels = Tchannel_profile->GetEntries();
  }

  TFile *output = new TFile (OutputFileName,"RECREATE","trace analaysis");
  output->Close();
  delete output;

///////////////////////////////   
  int START = 0;
 if(0){
   //run 11 - shower timestamp for log(E) > 17.5
   int time_start_sec = 1140284359-10-3600*2;
//   int time_start_sec = 1140303436-10-3600*2;
//   int time_start_sec = 1140357470-10-3600*2;
//   int time_start_sec = 1140418565-10-3600*2;
   int Sec;
   int Nanosec;
   for(int w=0; w<Tevent->GetEntries(); w++){
     Tevent->GetEntry(w);
     START = w;
     ConvertTimeStamp(event.timestamp, &Sec, &Nanosec);
     if( (Sec+10)>=(time_start_sec) && (Sec-3600)<time_start_sec){
       cerr << "event found by time" << endl;
       break;
     }
     if( (Sec-7)>time_start_sec){
       cerr << "event NOT found by time" << endl;
       break;
     }
   }
  }
  else{
  unsigned int event_id_start = 1389063 ;//1412319;//1389063; //9960424; //10417173; //10417499;//
   for(int w=START; w<Tevent->GetEntries(); w++){
     Tevent->GetEntry(w);
     START = w;
     if(event.event_id == event_id_start) {
       cerr << "event found by id" << endl;
       break;
     }

     if(event.event_id > event_id_start){
       cerr << "event NOT found by id" << endl;
       break;
     }
   }
  }
   
   Tevent->GetEntry(0);
   //cout << START << " START" << endl;

 int STOP = START+NoChannels*1;
///////////////////////////////
  
  if(STOP>= Tevent->GetEntries()){
   cerr << "entry doesn't exist" << endl;
   STOP = Tevent->GetEntries();
   cerr << "changed end number." <<  endl;
  // exit(1);
  }
  
  
  int ChId=0;
  char title[1024];

  float *TimeFFT = new float [event.window_size*2];
  for(unsigned int i=0; i<event.window_size*2; i++) TimeFFT[i]=(float)i;


  
  TCanvas **canfft = new TCanvas *[(STOP-START)];
  float *freq = new float [event.window_size/2];
  for(unsigned int i=0; i<event.window_size/2; i++)
    freq[i]=40.0+40.0/(event.window_size/2)*2+(40.0/(event.window_size/2.0))*(i);
    
  float **Amp = new float *[(STOP-START)];
  for(int i=0; i<(STOP-START); i++) Amp[i] = new float [event.window_size/2];
  float **Phase = new float *[(STOP-START)];
  for(int i=0; i<(STOP-START); i++) Phase[i] = new float [event.window_size/2];  
  for(int i=0; i<(STOP-START); i++)for(unsigned int j=0; j<event.window_size/2; j++){
    Amp[i][j]=0;
    Phase[i][j]=0;
  }
  
  TGraph **gAmp = new TGraph *[(STOP-START)];
  TGraph **gPhase = new TGraph *[(STOP-START)];



  int Aevent=-1;
  float tracedummy[event.window_size*2];
  
  
  for(int i=START; i<Tevent->GetEntries(); i++){
   Aevent++;
   
   if((Aevent)==((STOP-START))) break;
   
   if(Tevent->GetEntry(i)==0){
     cerr << "error - Tevent->GetEntry() -> main function" << endl;
     break;
   }

   if(!bTeventExtend){
    ChId=0;
    Tchannel_profile->GetEntry(ChId);
    while( event.channel_profile_id != channel_profiles.ch_id ){
      ChId++;
      Tchannel_profile->GetEntry(ChId);
    }
   }
    

   sprintf(title,"D%i_%s_%s, %s, run=%i, event_id=%i",header.DAQ_id, channel_profiles.antenna_pos, 
   		channel_profiles.polarisation, event.timestamp, header.run_id, event.event_id);
   for(unsigned int j=0; j<event.window_size; j++) tracedummy[j] = event.trace[j];   
   cout << endl << title << endl;

   

   //graph for Amp and Phase
   gAmp[(Aevent)] = new TGraph (event.window_size/2,freq,Amp[Aevent]);
   sprintf(name,"Amp%i_%03i",channel_profiles.ch_id,(Aevent));	
   gAmp[(Aevent)]->SetName(name);
   gAmp[(Aevent)]->SetTitle(title);
   gAmp[(Aevent)]->GetXaxis()->SetTitle(" frequency / MHz ");
   gAmp[(Aevent)]->GetYaxis()->SetTitle(" |Amp| ");

   gPhase[(Aevent)] = new TGraph (event.window_size/2,freq,Phase[(Aevent)]);
   sprintf(name,"Phase%i_%03i",channel_profiles.ch_id,(Aevent));	
   gPhase[(Aevent)]->SetName(name);
   gPhase[(Aevent)]->SetTitle(title);
   gPhase[(Aevent)]->GetXaxis()->SetTitle("frequency / MHz");
   gPhase[(Aevent)]->GetYaxis()->SetTitle("phase / rad");
   gPhase[Aevent]->SetLineColor(2);
   
   sprintf(name,"canfft_%03i",(Aevent));	
   canfft[(Aevent)]= new TCanvas(name,title);  
   
   
  
   
  
  } // end for over all traces and events
  
 



  output = new TFile (OutputFileName,"Update","trace analaysis");
  
  output->cd();

    cout << "writing .." << endl;
    for(int i=0; i<(STOP-START); i++){
      canfft[i]->cd();
      gAmp[i]->Draw("AL");
      gPhase[i]->Draw("SAMEL");
      gPad->SetLogy(); 
    }





     
     
     

output->Close();

delete output;
  

}

