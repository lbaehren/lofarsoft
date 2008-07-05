#include <iostream>

#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <time.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TChain.h>
#include <TAxis.h>
#include <TMath.h>

#include "util.hh"


using namespace std;

//! Comparison of timestamps between Kascade-GRANDE or LOPES30 and LOPES-STAR
/*!
 Input is a root file with the LOPES30 or GRANDE trigger timestamps (Sh3) and a root file with the timestamps (event).
 The choosen timeshift is used by the LOPES-STAR timestamp.
 The algorithm searches for coincidences within a defined time range.
 Output is a root file with several plots an other usefull information.

 \n Explanation of some variables:
 \n\b Range0 - lower time limit of the timeshift
 \n\b Range1 - upper time limit of the timeshift
 \n\b deltax - step size of the timeshift
*/


int FindEntryGrande(int EntryCountGrande, TChain *h3_2, int *AnkaEvent, int *Hit7Event){
    static struct h3 ntuple;
    ntuple.Gt=0;
    ntuple.Mmn=0;
    ntuple.Hit7=0;
    ntuple.Fanka=0;
    ntuple.Nflg=0;
    ntuple.Ageg=0;
    ntuple.Iact=0;
    ntuple.Xcg=0;
    ntuple.Ycg=0;
    
    
    int entry = EntryCountGrande;
    int maxEntries = h3_2->GetEntries();
    entry += 1;
  
    h3_2->SetBranchAddress("Gt",&ntuple.Gt);
    h3_2->SetBranchAddress("Mmn",&ntuple.Mmn);
    h3_2->SetBranchAddress("Fanka",&ntuple.Fanka);
    h3_2->SetBranchAddress("Hit7",&ntuple.Hit7);
    h3_2->SetBranchAddress("Nflg",&ntuple.Nflg);
    h3_2->SetBranchAddress("Ageg",&ntuple.Ageg);
    h3_2->SetBranchAddress("Sizeg",&ntuple.Sizeg);
    h3_2->SetBranchAddress("Sizmg",&ntuple.Sizmg);
    h3_2->SetBranchAddress("Zeg",&ntuple.Zeg);
    h3_2->SetBranchAddress("Ngrs",&ntuple.Ngrs);
    h3_2->SetBranchAddress("Iact",&ntuple.Iact);
    h3_2->SetBranchAddress("Xcg",&ntuple.Xcg);
    h3_2->SetBranchAddress("Ycg",&ntuple.Ycg);
    
    
    h3_2->GetEntry(entry);
    
    float Eest = 0;
    //Energy estimation from R. Glasstetter -> DPG 2006
    if(ntuple.Nflg>0) Eest = (0.31*log10(ntuple.Sizeg)+0.67*log10(ntuple.Sizmg)+1.24/cos(ntuple.Zeg)+0.58);
    
    //skip data while these arguments are true
    while( 	(ntuple.Fanka & 0x7) == 4 
		|| (ntuple.Hit7 & 0x80) != 0x80 
		|| ntuple.Nflg<=0 
		|| ntuple.Ageg<=0.4
		|| ntuple.Ageg>=1.4 
	   	|| Eest<=8  
		|| ntuple.Zeg>=40*2*3.14/360
		|| ntuple.Ngrs<=19 
	   	|| TMath::Max(TMath::Abs(ntuple.Xcg),TMath::Abs(ntuple.Ycg+280) ) >= 300 
	   	|| (ntuple.Iact & 0x1)!=1
	   ) {
	 
      if( (ntuple.Fanka & 0x7)== 4) *AnkaEvent += 1;
      if((ntuple.Hit7 & 0x80) != 0x80) *Hit7Event += 1;

      entry += 1;
      if(entry>=maxEntries) break;

      if(h3_2->GetEntry(entry)==0) {
        cerr << "h3_2->GetEntry() - error 1" << endl;
        exit(0);
      }
      
      Eest = 0;
      if(ntuple.Nflg>0) Eest = (0.31*log10(ntuple.Sizeg)+0.67*log10(ntuple.Sizmg)+1.24/cos(ntuple.Zeg)+0.58);

    }
    return entry;
}



int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();

  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  

  
  char *StarFileName="$HOME/lopes/data/run6_backup_00.root";
  char *GrandeFileName="$HOME/lopes/NTuple_Info/ntuple_run6.root";
  char *RootOutputFileName="$HOME/analysis/TSout_default.root";

  
  int compression=1, NoOfAntennas=7;
  float CoincidenceTime=20.0; //40; Unit: ms
  int meanEventNo=100;
  int RangeHis7=60;
  bool morePlots=true;

  
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "c:f:hs:l:a:t:m:p")) != -1) //: after var ->Waits for input
    switch (c)
      {
      case 'm':
	meanEventNo = atoi ( optarg );
	break;
      case 'p':
	morePlots=false;
	break;
      case 't':
	CoincidenceTime = atof ( optarg );
	break;
      case 'a':
	RangeHis7 = atoi ( optarg );
	break;
      case 'c':
	compression = atoi ( optarg );
	break;
      case 'f':
	RootOutputFileName = optarg ;
	break;
      case 's':
	StarFileName = optarg ;
	break;
      case 'l':
	GrandeFileName = optarg ;
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
	cout << " Overview: comparison of timestamps\n";
	cout << "===================================\n\n";
	cout << " -h	HELP -- you already tried :-)\n";
	cout << " -f	name of the root output file (default: " << RootOutputFileName << ")\n";
	cout << " -s	name of the lopesSTAR root file (default: " << StarFileName << ")\n";
	cout << " -l	name of the Grande root file (default: " << GrandeFileName << ")\n";
	cout << " -a	number of sec to average over (default: " <<  RangeHis7 << " s)\n";	
	cout << " -t	coincidence time in ms (default: " << CoincidenceTime <<" ms)\n";		
	cout << " -m	number of events to average over (default: " << meanEventNo << ")\n";		
	cout << " -p	creates NOT all plots\n";		
	cout << " -c	compression of the root file\n"
		"        0 .. 9 -> default: 1\n";
	
	cout << "\n\n\nmailto: asch@ipe.fzk.de\n";
	exit(1);
        break;      
      default:
	abort ();
      }

  char name[256];

  TChain *Tevent = new TChain("Tevent");
  CreateTChain(Tevent, StarFileName);
  
  TChain *Theader = new TChain("Theader");
  Theader->Add(StarFileName);

  TChain *Tchannel_profile = new TChain("Tchannel_profile");
  Tchannel_profile->Add(StarFileName);

  TChain *h3 = new TChain("h3");
  h3->Add(GrandeFileName);
  
  TChain *h3_2 = new TChain("h3");
  h3_2->Add(GrandeFileName);


  static struct event event;
  static struct header header;
  static struct channel_profiles channel_profiles;
  
  CreateTevent(Tevent, &event);
  CreateTheader(Theader, &header);
  CreateTchannel_profiles(Tchannel_profile, &channel_profiles);

  Theader->GetEntry(0);
  
  NoOfAntennas =  Tchannel_profile->GetEntries();
  cerr << "No of Antennas = " << NoOfAntennas << endl;
  
  static struct h3 Sh3;
  Sh3.Gt=0;
  Sh3.Mmn=0;
  Sh3.Hit7=0;
  Sh3.Fanka=0;
  Sh3.Nflg=0;
  Sh3.Ageg=0;
  
  h3->SetBranchAddress("Gt",&Sh3.Gt);
  h3->SetBranchAddress("Mmn",&Sh3.Mmn);
  h3->SetBranchAddress("Fanka",&Sh3.Fanka);
  h3->SetBranchAddress("Hit7",&Sh3.Hit7);
  h3->SetBranchAddress("Nflg",&Sh3.Nflg);
  h3->SetBranchAddress("Ageg",&Sh3.Ageg);
  h3->SetBranchAddress("Sizeg",&Sh3.Sizeg);

  
  int lowerSec=0, lowerSubsec=0, upperSec=0, upperSubsec=0;
  int EntryCountGrande=0, subsec;
  int starTime=0, CountEvent=0, CountMissEvent=0, DoubleCountEvent=0, AnkaEvent=0, Hit7Event=0;
  bool eventfound=false;
  bool firstrun=false;
  int StartHis7=0;  
  
  Long64_t star_entries=0, Grande_entries=0;
  star_entries=Tevent->GetEntries();
  Grande_entries = h3->GetEntries();
//cerr << "Enries: Grande = " << Grande_entries << " - Star = " << star_entries << endl;
 
  //Mean frequency of trigger of the run
  int StartTime=0, StopTime=0, dummy1=0, Duration=0;
  double MeanStarF=0, MeanGrandeF=0;

  Tevent->GetEntry(0);
  ConvertTimeStamp(event.timestamp, &StartTime, &dummy1, 0.0);
  StartHis7=StartTime;

  Tevent->GetEntry(star_entries-1);
  ConvertTimeStamp(event.timestamp, &StopTime, &dummy1, 0.0);
 
  Duration=(StopTime-StartTime);//in s
  MeanStarF = (double)((star_entries/NoOfAntennas)/(double)(Duration));
//cerr << MeanStarF << " Hz" << endl;
  
  h3->GetEntry(0);
  StartTime = Sh3.Gt;
  h3->GetEntry(Grande_entries-1);
  StopTime = Sh3.Gt;
    
//cerr << "duration " << Duration << " " << StopTime-StartTime << endl;
  cerr << "Create output file";

  TFile *output = new TFile (RootOutputFileName,"Recreate","timestamp analysis",compression);
  float x=0.0;
  TTree *info = new TTree ("info","information about the run");
  info->Branch("fEvent",&CountEvent,"FoundEvent/i");
  info->Branch("mEvent",&CountMissEvent,"MissedEvent/i");
  info->Branch("dEvent",&DoubleCountEvent,"DoubleFoundEvent/i");
  info->Branch("fStar",&MeanStarF,"MeanStarF/D");
  info->Branch("fGrande",&MeanGrandeF,"MeanGrandeF/D");
  info->Branch("cTime",&CoincidenceTime,"CoincidenceTime(ms)/F");
  info->Branch("Anka",&AnkaEvent,"FoundAnkaEvent/i");
  info->Branch("NoTrigDaq",&Hit7Event,"NoTriggerDAQ17/i");
  info->Branch("ts",&x,"TimeShift/F");
  info->Branch("duration",&Duration,"RunDuration/i");
  
  cerr << " - done." << endl;
  
  double frequency=0;
  TTree *freq = new TTree ("freq","high frequencies of the trigger");
  freq->Branch("f",&frequency,"frequency/D");
  freq->Branch("event_id",&event.event_id,"eventID/i");
  
  TTree *crate = new TTree ("crate","coincidance info");
  int gt_star=0;
  crate->Branch("event_id",&event.event_id,"eventID/i");
  crate->Branch("timestamp",&event.timestamp,"timestamp/C");
  crate->Branch("gt",&gt_star,"gt/i");
  crate->Branch("ts",&x,"TimeShift/F");
  

//definition of the Histogramms
  TH1D *his1 = new TH1D ("his1","his1",300,0,30);
  his1->GetXaxis()->SetTitle("1/t  s");
  his1->GetYaxis()->SetTitle("count");
  sprintf(name,"Run %d - 1/(#Delta t) of next events",header.run_id);
  his1->SetTitle(name);
  double TS1=1, TS2=1; 
  
  int MaxEventId, MinEventId;
  Tevent->GetEntry(0);
  MinEventId=(int)(event.event_id/1000);
  Tevent->GetEntry(star_entries-1);
  MaxEventId=(int)(event.event_id/1000);
  
  int his2Bins = 0;
  
  if( (MaxEventId-MinEventId)>1.0e6) his2Bins = (int)1.0e6;
    else his2Bins = (MaxEventId-MinEventId)*100;
  
  TH2D *his2 = new TH2D ("his2","his2",his2Bins,MinEventId,MaxEventId,100,0,100);
  his2->GetXaxis()->SetTitle("event id");
  his2->GetYaxis()->SetTitle("f / Hz");
  sprintf(name,"Run %d - frequency of trigger versus event id",header.run_id);
  his2->SetTitle(name);
  his2->SetMarkerStyle(29);
  
/*****************************/  
/**/
/**/  int Range0=-5, Range1=5; //Range of TimeShift in s
/**/  float deltax = 0.2; //Delta TimeShift in s
/**/
/*****************************/
  TH2D *his3 = new TH2D ("his3","his3",(int)((Range1+2-Range0)*1000),Range0-1,Range1+1,1000,0,1.01);
  his3->GetXaxis()->SetTitle("shift second / s");
  his3->GetYaxis()->SetTitle("efficiency");
  sprintf(name,"Run %d - efficiency versus timeshift",header.run_id);
  his3->SetTitle(name);
  his3->SetMarkerStyle(29);
  
  TH2D *his4 = new TH2D ("his4","his4",his2Bins,MinEventId,MaxEventId,100,0,100);
  his4->GetXaxis()->SetTitle("event id");
  his4->GetYaxis()->SetTitle("f / Hz");
  sprintf(name,"Run %d - mean frequency over %d events",header.run_id, meanEventNo);
  his4->SetTitle(name);
  his4->SetMarkerStyle(29);
  int his4count=0;
  double meanf=0;

  TH1D *his5 = new TH1D ("his5","his5",300,0,30);
  his5->GetXaxis()->SetTitle("f /Hz");
  his5->GetYaxis()->SetTitle("count");
  sprintf(name,"Grande ntuple - Run %d - 1/(#Delta t) of next events",header.run_id);
  his5->SetTitle(name);
  his5->SetMarkerStyle(29);
  
  TH1D *his6 = new TH1D ("his6","his6",100,0,5);
  his6->GetXaxis()->SetTitle("f /Hz");
  his6->GetYaxis()->SetTitle("count");
  sprintf(name,"Grande ntuple - Run %d - 1/(#Delta t) of next events - smaller scale",header.run_id);
  his6->SetTitle(name);
  his6->SetMarkerStyle(29);

  int CountEventHis7=0, DeltaTimeHis7=0;
  TH2D *his7 = new TH2D ("his7","his7",Duration%RangeHis7,0,Duration,30,0,30);
  his7->GetXaxis()->SetTitle("time / s");
  his7->GetYaxis()->SetTitle("f / Hz");
  sprintf(name,"Run %d - frequency of trigger vs time over %d s",header.run_id,RangeHis7);
  his7->SetTitle(name);
  his7->SetMarkerStyle(29);
  
  TH1D *his8 = new TH1D ("his8","his8",3000,0,30);
  his8->GetXaxis()->SetTitle("t / s");
  his8->GetYaxis()->SetTitle("count");
  sprintf(name,"Run %d - #Delta t of next events",header.run_id);
  his8->SetTitle(name);



  double Time1=0, Time2=0, DFreq=0, DFreq2=0, DFreq3=0, DFreq4=0;
  
  for(int i=0; i<Grande_entries; i++){
    Time1=Time2;
    h3->GetEntry(i);
    Time2 = Sh3.Gt + ((double)Sh3.Mmn)/1000000000.0;
//sprintf(name,"%lf",Time2);
//cerr << Sh3.Gt << " " << Sh3.Mmn << " " << name << endl;
    DFreq = 1.0/(Time2-Time1);
    if(Time1!=0){
      his5->Fill( DFreq );
      his6->Fill( DFreq );
    }
  }
  his5->Write();
  his6->Write();
  
  DeltaTimeHis7 += RangeHis7;
  
  char OldTimestamp[256];    
  firstrun = true;
  
//for(x=Range0; x<(Range1+deltax); x += deltax)
 {
//x = 0;
//cerr << x << endl;
  
//  if(x<0.01 && x>-0.01) firstrun = true;
 
  EntryCountGrande=0;
  CountEvent=0;
  CountMissEvent=0;
  DoubleCountEvent=0;
  his4count=0;
  AnkaEvent=0;
  Hit7Event=0;

//  cout << EntryCountGrande << " - " ;
  EntryCountGrande = FindEntryGrande(EntryCountGrande, h3_2, &AnkaEvent, &Hit7Event);
//  cout << " - " << EntryCountGrande << endl;
  h3->GetEntry(EntryCountGrande);

  lowerSec=Sh3.Gt;
  lowerSubsec=Sh3.Mmn;

  
  
//  cout << EntryCountGrande << " - " ;
  EntryCountGrande = FindEntryGrande(EntryCountGrande, h3_2, &AnkaEvent, &Hit7Event);
//  cout << " - " << EntryCountGrande << endl;
  h3->GetEntry(EntryCountGrande);
 
  upperSec=Sh3.Gt;
  upperSubsec=Sh3.Mmn;

  
  for(int i=0; i<(star_entries/NoOfAntennas); i++){

    TS1=TS2;
    strcpy(OldTimestamp,event.timestamp);
    
    Tevent->GetEntry(i*NoOfAntennas);
    while( strlen(event.timestamp)==0 || strcmp(OldTimestamp,event.timestamp)==0 || strcmp(event.timestamp,"NULL")==0 ){
      cerr << "Found timestamp of length NULL or the same again - skipped" << endl;
      i++;
      if(i>=(star_entries/NoOfAntennas)) break;
      Tevent->GetEntry(i*NoOfAntennas);
    }
 
    
    ConvertTimeStamp(event.timestamp, &starTime, &subsec, x);
    gt_star=starTime;
    
    

/***************************************************************/
    //Fill histogramms
  
  if(firstrun){      
    TS2=starTime + (double)(((double)subsec)/1000000000.0);
//    sprintf(name,"%lf",TS2);
//    cerr << name << endl;
    
    DFreq = 1.0/(TS2-TS1);
    
    //if(DFreq > 4.99 && DFreq <5.01) cout << "Peak found (5Hz) - event_id = " << event.event_id << endl;
    
    
    if( /*DFreq>30 &&*/ TS1!=1){     
      frequency = DFreq;
// cerr << "f = "<< frequency << " Hz - event id = " << event.event_id << endl;
      freq->Fill();
     }
    
    if(TS1!=1){
       his1->Fill(DFreq);
       his8->Fill(1.0/DFreq);
       DFreq2 = (double)(((double)event.event_id) / 1000.0);
       his2->Fill(DFreq2,DFreq);
     }
    
    if(TS1!=1){
      his4count++;
      meanf += (TS2-TS1);
    }
    if(his4count==meanEventNo){
      meanf /= meanEventNo;
      DFreq3 = DFreq2; 
      DFreq4 = 1.0/meanf;
      his4->Fill(DFreq3,DFreq4);
      his4count=0;
      meanf=0;
    }
    
    CountEventHis7++;
    if((StartHis7+DeltaTimeHis7) <= starTime){
      DeltaTimeHis7 += RangeHis7;
      his7->Fill((DeltaTimeHis7),(double)((double)CountEventHis7/(double)RangeHis7));
      //cerr << (double)((double)CountEventHis7/(double)RangeHis7) << " " <<  DeltaTimeHis7 << endl;
      CountEventHis7=0;
    }
    
  }
/***************************************************************/   


 
    if(  starTime==lowerSec 
         && subsec>=(lowerSubsec-(int)((double)(CoincidenceTime)/2.0*1000000.0)) 
	 && subsec<=(lowerSubsec+(int)((double)(CoincidenceTime)/2.0*1000000.0))  ){
	 
        if(!eventfound){
	  CountEvent++;
	  eventfound=true;
	  crate->Fill();
//        cerr << "Star entry = " << i << " - Grande entry = " << EntryCountGrande << endl;
	}
	else{
	  DoubleCountEvent++;
          if(firstrun) cerr << "Double Count: f = " << 1/(TS2-TS1) << " Hz - event id = " << event.event_id << endl;
	  
        }
    }


    while((EntryCountGrande < Grande_entries ) 
          && (starTime>lowerSec || (starTime==lowerSec && subsec>(lowerSubsec+(int)((double)(CoincidenceTime)/2.0*1000000.0)) )) ){

      
      

      if(starTime==upperSec 
         && subsec>=(upperSubsec-(int)((double)(CoincidenceTime)/2.0*1000000.0)) 
	 && subsec<=(upperSubsec+(int)((double)(CoincidenceTime)/2.0*1000000.0))){
	 
	 
	  CountEvent++;
	  eventfound=true;
	  crate->Fill();
      }
      
      
      
      if(!eventfound) {
          CountMissEvent++; 
//cerr << event.event_id << " " << event.timestamp << " " << starTime << " " << subsec << " " << lowerSec << " " << lowerSubsec << " " << upperSec << " " << upperSubsec << endl;
        }
        else eventfound=false;


       lowerSec=upperSec;
       lowerSubsec=upperSubsec;
      
 //    cout << EntryCountGrande << " - " ;
       EntryCountGrande = FindEntryGrande(EntryCountGrande, h3_2, &AnkaEvent, &Hit7Event);
 //    cout << " - " << EntryCountGrande << endl;
       h3->GetEntry(EntryCountGrande);

        if(EntryCountGrande>=Grande_entries){ 
          //cerr << "end of Grande data ... (2)" << endl;
  	  break;
	}

       upperSec=Sh3.Gt;
       upperSubsec=Sh3.Mmn;

    }//end while
    
  //if(i==star_entries-1)cerr << "Star entry = " << i << " - Grande entry = " << EntryCountGrande << endl;  
  }//end for loop over events
  
   if(firstrun){
     his1->Write();
     if(morePlots){
       his2->Write();
       his4->Write();
       his7->Write();
     }
     his8->Write();
     firstrun=false;
   }
   
   MeanGrandeF = (double)(CountEvent+CountMissEvent)/(double)(StopTime-StartTime);
   
   cout << "\nstatistically found events = " << (int)(MeanGrandeF*MeanStarF*CoincidenceTime/1000.0*Duration) << endl;
   cout << "Mean frequency of trigger: lopes-Star = " << MeanStarF << " Hz\tGrande = " << MeanGrandeF << " Hz" << endl << endl;
   cout << "Found Events = " << CountEvent << "\tMissed Events = " << CountMissEvent << "\tDouble Count Events = " << DoubleCountEvent << endl;
   sprintf(name,"%lf",(double)((double)(CountEvent)/(double)(CountEvent+CountMissEvent))*100.0);
   cout << "Efficiency = " << name << " \%" << endl;
   cout << "Coincidence time = " << CoincidenceTime << " ms" << endl << endl;
   cout << "DAQ 17 not triggered events = " << Hit7Event << "\nAnka events = " << AnkaEvent << endl;
   cout << "Timeshift t = " << x << " s" << endl;
 
   his3->Fill(x,((double)((double)(CountEvent)/(double)(CountEvent+CountMissEvent))));
   info->Fill();
   
 }//end for timeshift
  

  output->cd();
  if(morePlots) his3->Write();
  info->Write(info->GetName(),TObject::kWriteDelete);//Write();
  freq->Write(freq->GetName(),TObject::kWriteDelete);//Write();
  crate->Write(crate->GetName(),TObject::kWriteDelete);//Write();

  output->Close();

  delete output;
/*  
  delete info;
  delete freq;
  delete crate;
  
  delete h3;
  delete h3_2;
  
  delete Tevent;
  delete Theader;
  delete Tchannel_profile;
  
  delete his1;
  delete his2;   
  delete his3;   
  delete his4;   
  delete his5;   
  delete his6;   
  delete his7;   
  delete his8;   
*/  

}
