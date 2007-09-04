#include <iostream>

#include <TROOT.h>
#include <TFile.h>
#include <TH2F.h>
#include <TSystem.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TStyle.h>

#include <LopesStar/util.h>

using namespace std;

//! dynamic spectra of all channels per run
/*!

Dynamic spectra of all channels per run are illustrated with a log scale.
Two types of dynamic spextra are created:
frequency versus event no
frequency versus time
*/





int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();
  gSystem->Setenv("TZ","UTC");


  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  

  
  char *StarFileName="$HOME/analyisis/default_00.root";
  char *OutputFileName="$HOME/analysis/DynamicSpectra_out.root";
  char name[2048];


  
  
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
	cout << " Overview: dynamic spectra for ARG measurements 2006\n";
	cout << "====================================================\n\n";
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


  TFile *output = new TFile (OutputFileName,"Recreate","dynamic spectra",6);
  delete output;

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
   //cout << START << " START" << endl;

 int STOP = /*START+NoChannels*1000+1; //*/ Tevent->GetEntries(); //START+NoChannels*1000+1; //
 bool debug=true;
///////////////////////////////
  
  if(STOP> Tevent->GetEntries()){
   cerr << "entry doesn't exist" << endl;
   STOP = Tevent->GetEntries();
   cerr << "changed end number." <<  endl;
  // exit(1);
  }
  
 
  int ChId=0;
  char title[2048];
  int isave=0;

  
  
  float *Amp = new float [event.window_size/2];
  float *Phase = new float [event.window_size/2];  
  float *Tracefft = new float [event.window_size*2];  
  int index = 0; 
  
  
  
  TH2F DynamicSpectraTime[NoChannels];
  int Sec=0, SubSec=0;
  Tevent->GetEntry(START);
  ConvertTimeStamp(event.timestamp, &Sec, &SubSec);
  double xStartTime =Sec+(SubSec/1.0e9);

  while(ConvertTimeStamp(event.timestamp, &Sec, &SubSec)==0){
    START+=NoChannels;
    Tevent->GetEntry(START);
    ConvertTimeStamp(event.timestamp, &Sec, &SubSec);
    xStartTime = Sec+(SubSec/1.0e9);
  }
  double StartTime= xStartTime;
  
  Tevent->GetEntry(STOP-1);
  ConvertTimeStamp(event.timestamp, &Sec, &SubSec);
  double xStopTime=(Sec+(SubSec/1.0e9));

  while(ConvertTimeStamp(event.timestamp, &Sec, &SubSec)==0){
    STOP-=NoChannels;
    Tevent->GetEntry(STOP);
    ConvertTimeStamp(event.timestamp, &Sec, &SubSec);
    xStopTime  = (Sec+(SubSec/1.0e9));
  }
  
  if(debug) cout << "Start index = " << START << " - Stop index = " << STOP << endl;
  
  

  TH2F DynamicSpectra[NoChannels];
  int xbin   = (STOP-START)/NoChannels;
   if(xbin>65000) xbin=65000;
  
  int xStart = 0;
  int xStop  = (STOP-START)/NoChannels;
  int MeanEventNo = (int)((double)(STOP-START)/(double)NoChannels/(double)xbin);
  int BinCount[NoChannels];
  for(int i=0; i<NoChannels; i++) BinCount[i]=0;
  int PresentBin[NoChannels];
  for(int i=0; i<NoChannels; i++) PresentBin[i]=1;
  double *DynamicSpectraContent[NoChannels];
  for(int i=0; i<NoChannels; i++) DynamicSpectraContent[i] = new double [event.window_size/2];
  
  for(int i=0; i<NoChannels; i++){
    if(bTeventExtend)Tevent->GetEntry(i);
    else Tchannel_profile->GetEntry(i);
    sprintf(title,"DSpectrum - D%i_%s_%s - run %02i",header.DAQ_id, channel_profiles.antenna_pos, channel_profiles.polarisation,header.run_id);
    sprintf(name,"spectrum_%02i",i);
    DynamicSpectra[i] =  TH2F(name,title,xbin,xStart,xStop,event.window_size/2,40,80);
    DynamicSpectra[i].GetXaxis()->SetTitle("event no.");
    DynamicSpectra[i].GetYaxis()->SetTitle("frequency / MHz");
  }

  if(debug) cout << "Amp vs Event: bins = " << xbin << " - Start = " << xStart  << " - Stop = " << xStop << endl; 
  if(debug) cout << "Mean events pro bin = " << MeanEventNo << " total events = " << (STOP-START)/NoChannels << endl;

  
     
  int xbinTime  = (int)(((int)xStopTime-(int)xStartTime)/30.0);
  if(xbinTime>65000) xbinTime=65000;
   
  
     
  int BinCountTime[NoChannels];
  for(int i=0; i<NoChannels; i++) BinCountTime[i]=0;
  int PresentBinTime[NoChannels];
  for(int i=0; i<NoChannels; i++) PresentBinTime[i]=1;
  double *DynamicSpectraContentTime[NoChannels];
  for(int i=0; i<NoChannels; i++) DynamicSpectraContentTime[i] = new double [event.window_size/2];
  
  for(int i=0; i<NoChannels; i++){
    if(bTeventExtend)Tevent->GetEntry(i);
    else Tchannel_profile->GetEntry(i);
    sprintf(title,"DSpectrum - D%i_%s_%s - run %02i",header.DAQ_id, channel_profiles.antenna_pos, channel_profiles.polarisation,header.run_id);
    sprintf(name,"spectrum_%02i",i+NoChannels);
    DynamicSpectraTime[i] = TH2F(name,title,xbinTime,(int)xStartTime,(int)xStopTime,event.window_size/2,40,80);
    DynamicSpectraTime[i].GetXaxis()->SetTimeDisplay(1);
    DynamicSpectraTime[i].GetXaxis()->SetTimeFormat("#splitline{%H:%M:%S}{%d/%m/%Y}%F1900-01-01 00:00:00");
    DynamicSpectraTime[i].GetXaxis()->SetLabelSize(0.03);
    DynamicSpectraTime[i].GetXaxis()->SetLabelOffset(0.030);
    DynamicSpectraTime[i].GetYaxis()->SetTitle("frequency / MHz");
  }

  if(debug) printf("Amp vs Time: bins = %i - Start = %i - Stop = %i\n", xbinTime, (int)xStartTime, (int)xStopTime );
  
  for(int j=0; j<NoChannels; j++){
    for(unsigned int i=0; i<event.window_size/2; i++){
      DynamicSpectraContent[j][i]=0;
      DynamicSpectraContentTime[j][i]=0;
    }
  }

  
  double SecPerBin = (xStopTime-xStartTime)/xbinTime;
  if(debug) cout << "Seconds per bin = " << SecPerBin << endl;
  double ratio=0;
  double time_dummy=0;
  int wrong_index=0;
  int RLimit = (int)SecPerBin*10;
  bool SkipEvent=false;
  
  char position[4];
  char polarisation[20];
 
  for(int i=START; i<STOP; i++){
   index = -1;
   
   if(Tevent->GetEntry(i)==0){
     cerr << "error - Tevent->GetEntry()" << endl;
     exit(0);
     break;
   }
   
   if(!bTeventExtend){
     ChId=0;
     Tchannel_profile->GetEntry(ChId);
     while( event.channel_profile_id != channel_profiles.ch_id ){
       ChId++;
       if(Tchannel_profile->GetEntry(ChId)==0){
         cout << "error: Tchannel_profile->GetEntry() ... " << endl;
         break;
       }
     }
   }
   
   ratio=0;
   ConvertTimeStamp(event.timestamp, &Sec, &SubSec);
   if(ConvertTimeStamp(event.timestamp, &Sec, &SubSec)==0){
     cerr << "wrong timestamp!? - skip event" << endl;
     time_dummy = -1;
     ratio = RLimit+10;
     SkipEvent=true;
   }
   else {
    ConvertTimeStamp(event.timestamp, &Sec, &SubSec);
    time_dummy = (SecPerBin-(Sec+(SubSec/1.0e9)-StartTime)) ;
    ratio=0;
    SkipEvent=false;
   }

   
   if( time_dummy < 0  ){
     if(!SkipEvent){
       ratio=(Sec+(SubSec/1.0e9)-StartTime)/SecPerBin;
     }
               
     if(ratio>2.0 && debug)
       cout << "detected long periode of no triggered events - bins left = " << ratio << endl;
     if(ratio<RLimit){
        StartTime += SecPerBin;     
     
       if(debug) cout << "Time difference = " << time_dummy << endl;
       if(debug) cout << " event index = " << i << " - divided by NoChannels = " << (i/(double)NoChannels)<< "\nNext (time) bin = ";

       for(int a=0; a<NoChannels; a++){
         if(PresentBinTime[a]>xbinTime){
            if(debug) cout << "last bin in time histogramm reached ... "<< endl;
	    PresentBinTime[a] = xbinTime;
         }
         for(unsigned int j=0; j<event.window_size/2; j++) {
           DynamicSpectraContentTime[a][j] /= BinCountTime[a];
	   DynamicSpectraTime[a].SetBinContent(PresentBinTime[a],j+1,DynamicSpectraContentTime[a][j]);
  	   DynamicSpectraContentTime[a][j] = 0;
         }
         if(ratio>2.0 && ratio) PresentBinTime[a] += (int)ratio;
         else PresentBinTime[a] += 1;
         BinCountTime[a]=0;
       
         if(debug) cout << " - " << PresentBinTime[a] ;
       }
     
       if(debug) cout << endl; 
     }//end RLimit
     else cerr << "wrong timestamp?! - skip this event" << " event index = " << i << " - divided by NoChannels = " << (i/(double)NoChannels) << endl;
   }

   if(i<START+NoChannels*2)
          sprintf(title,"DSpectrum - D%i_%s_%s - run %02i",header.DAQ_id, channel_profiles.antenna_pos, channel_profiles.polarisation,header.run_id);
	  
   TraceFFT((int)event.window_size, event.trace, Amp, Phase, Tracefft);

   
   for(int in=0; in<NoChannels; in++){
     sscanf(DynamicSpectra[in].GetTitle(),"DSpectrum - D%*i_%c%c%c_%s - run", &position[0], &position[1], &position[2], polarisation);
     if(strncmp(position,channel_profiles.antenna_pos,3)==0){
       if(strncmp(polarisation,channel_profiles.polarisation,4)==0){
        index = in;
	//cout << position << " - " << channel_profiles.antenna_pos << endl;
	//cout << polarisation << " - " << channel_profiles.polarisation << endl;
	break;
       }
     }
   }
   
  if(index == -1){
    wrong_index++;
    /*if(debug)*/ cout << " right index not found .. set index = -1 " << endl;
   }
   

    if(index!=-1){
     for(unsigned int j=0; j<event.window_size/2; j++){
       DynamicSpectraContent[index][j] += Amp[j];
       DynamicSpectraContentTime[index][j] += Amp[j];
     }
     
    //Dynamic Spectra - event no
     BinCount[index]++;
     if(BinCount[index] == MeanEventNo){
       for(unsigned int j=0; j<event.window_size/2; j++){
         DynamicSpectraContent[index][j] /= MeanEventNo;
	 DynamicSpectra[index].SetBinContent(PresentBin[index],j+1,DynamicSpectraContent[index][j]);
	 DynamicSpectraContent[index][j]=0;
       }
       BinCount[index]=0;
       PresentBin[index]++; 
       if(PresentBin[index]>xbin+1 && debug) cout << "present bin in event no histogramm is bigger than the overflow bin\tindex = " << index << endl;
       if(PresentBin[index]>xbin+1){
         PresentBin[index]=xbin+1;
	 if(debug) cout << "set bin no of event histogramm to overflow bin - " << index << endl; 
       }
     }
     if(i<START+NoChannels*2) DynamicSpectra[index].SetTitle(title);
       
     //Dynamic Spectra - time
     if(ratio<RLimit){
       BinCountTime[index]++;
       if(i<START+NoChannels*2) DynamicSpectraTime[index].SetTitle(title);
     }
       
    }//end if index != -1
       
 

   isave++;
   if(isave==10e3){
     for(int j=0; j<NoChannels; j++){
       output = new TFile (OutputFileName,"UPDATE");
       DynamicSpectra[j].Write(DynamicSpectra[j].GetName(),TObject::kWriteDelete);
       DynamicSpectraTime[j].Write(DynamicSpectraTime[j].GetName(),TObject::kWriteDelete);
       delete output;
     }
     isave=0;
   }
   

  
  } // end for over all traces and events
  

  //fill the last bin of the histogramms
  for(int i=0; i<NoChannels; i++){
    for(unsigned int j=0; j<event.window_size/2; j++) {
      if(BinCount[i]>0){
        if(debug && j==0) cout << "normalising event no histogramm in last bin " << endl;
	if(PresentBin[i]>xbin+1){
	  PresentBin[i]=xbin+1;
	  if(debug) cout << "set event no histogramm to overflow bin - " << i << endl; 
	}
        DynamicSpectraContent[i][j] /= BinCount[i];
        DynamicSpectra[i].SetBinContent(PresentBin[i],j+1,DynamicSpectraContent[i][j]);
      }
      
      if(BinCountTime[i]>0){
        if(debug && j==0) cout << "normalising time histogramm in last bin " << endl;
	if(PresentBinTime[i]>xbinTime+1){
	  PresentBinTime[i]=xbinTime+1;
 	  if(debug) cout << "set time histogramm to overflow bin - " << i << endl; 
	}
        DynamicSpectraContentTime[i][j] /= BinCountTime[i];
        DynamicSpectraTime[i].SetBinContent(PresentBinTime[i],j+1,DynamicSpectraContentTime[i][j]); 
      }
    }
  }
  
  if(debug){ 
    for(int p=0; p<NoChannels; p++)
      cout << "Time: bin = " << PresentBinTime[p] << "\nEvent No: bin = " << PresentBin[p] << endl;
      cout << "count wrong index = " << wrong_index << endl;
  }

  output = new TFile (OutputFileName,"UPDATE");
  output->cd();
  
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  
   

  TCanvas *can = new TCanvas ("c1","DSpectra vs event no");
  TCanvas *canTime = new TCanvas ("c2","DSpectra vs time");
 
 //to create some more canvas object in the root output file   
/*
  can->cd();
  can->Divide(2,4);
     
  canTime->cd();
  canTime->Divide(2,4);

  for(int j=0; j<NoChannels; j++){
    can->cd(j+1);
    DynamicSpectra[j].Draw("colz");
    
    canTime->cd(j+1);
    DynamicSpectraTime[j].Draw("colz");
  }
  
  can->Write(); 
  canTime->Write();
*/

  for(int i=0; i<NoChannels; i++){
    DynamicSpectra[i].Write(DynamicSpectra[i].GetName(),TObject::kWriteDelete);     
    DynamicSpectraTime[i].Write(DynamicSpectraTime[i].GetName(),TObject::kWriteDelete);     
  }
  
  output->Close();


  //do not forget to clean up
  delete output;
  delete can;
  delete canTime;
  
  delete Tevent;
  delete Theader;
  delete Tchannel_profile;
  
  delete[] Amp;
  delete[] Phase;
  delete[] Tracefft;
  
  for(int i=0; i<NoChannels; i++){
    delete[] DynamicSpectraContent[i];
    delete[] DynamicSpectraContentTime[i];    
  }

}
