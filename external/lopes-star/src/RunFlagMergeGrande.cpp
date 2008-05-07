#include <iostream>
#include <math.h>

#include <TROOT.h>
#include <TFile.h>
#include <TH2F.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TMath.h>


#include "util.hh"


using namespace std;
using namespace TMath;

//! run flag merge Grande
/*!
merges the Grande reconstruction with the reconstructkon given from STAR RunFlagOut
*/

void GetTimeSearchWindow(unsigned int *offset, unsigned *dtime, int daq){
  if(daq == 17){
    *offset = (unsigned int)(0.819e9);
    *dtime = (unsigned int) (0.001e9);
  }

  if(daq == 19){
    *offset = (unsigned int)(0.819e9);
    *dtime = (unsigned int) (0.001e9);
//    cerr << "offset and delta time ranges are not defined" << endl;
  }

  if(daq == 30){
    *offset = (unsigned int)(0.819e9);
    *dtime = (unsigned int) (0.001e9);
  }
}


int main(int argc, char *argv[])
{
  setenv("TZ","UTC",1);
  tzset();


  printf("Asch@ipe.fzk.de (build %s %s)\n", __DATE__, __TIME__);  

  
  char *AFOutputFileName="$HOME/analyisis/RunFlag_out.root";
  char *GrandeNTupleList="$HOME/analysis/GrandeMergeList.txt";
  bool debug = false;
 
  
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "ha:g:")) != -1) //: after var ->Waits for input
    switch (c)
      {
      case 'a':
	AFOutputFileName = optarg ;
	break;
      case 'g':
	GrandeNTupleList = optarg ;
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
	cout << " Overview: run flag merge Grande\n";
	cout << "================================\n\n";
	cout << " -h	HELP -- you already tried :-)\n";
	cout << " -a	RunFlagOut output file (default: " << AFOutputFileName << ")\n";
	cout << " -g	list with file names of correlatied ntuples (default: " << GrandeNTupleList << ")\n";
	cout << "\n\n\nmailto: asch@ipe.fzk.de\n";
	exit(1);
        break;      
      default:
	abort ();
      }

  cout << "output file: " << basename(AFOutputFileName) << endl;
  
  TFile *output = new TFile (AFOutputFileName,"update","RunFlagMergeGrande");
  static struct AnaFlag AnaFlag;
  TChain *TAnaFlag = new TChain("TAnaFlag");
  TAnaFlag->Add(AFOutputFileName);
  CreateTAnaFlag(TAnaFlag, &AnaFlag);
  if(debug) cout << "STAR entries = " <<TAnaFlag->GetEntries() << endl;
  TAnaFlag->GetEntry(0);
  
  static struct h3 h3;
  static struct h3 Zeroh3;
  SetStruct2Zero(&Zeroh3);
  TChain *TGrande_orig = new TChain("h3");
  CreateTGrandeChain(TGrande_orig,GrandeNTupleList );
  CreateTGrande(TGrande_orig, &h3, true);
  if(debug) cout << "Grande entries = " << TGrande_orig->GetEntries() << endl;
  
  static struct h3 h3_out;
  TTree *TGrande = new TTree("TGrande", "Grande Reconstruction");
  TGrande->Branch("Gt",&h3_out.Gt,"Gt/i");			
  TGrande->Branch("Mmn",&h3_out.Mmn,"Mmn/i");			
  TGrande->Branch("Fanka",&h3_out.Fanka,"Fanka/b");		
  TGrande->Branch("Hit7",&h3_out.Hit7,"Hit7/i"); 		
  TGrande->Branch("Nflg",&h3_out.Nflg,"Nflg/B"); 		
  TGrande->Branch("Ageg",&h3_out.Ageg,"Ageg/F"); 		    		 
  TGrande->Branch("Sizeg",&h3_out.Sizeg,"Sizeg/F");		
  TGrande->Branch("Sizmg",&h3_out.Sizmg,"Sizmg/F");		
  TGrande->Branch("Idmx",&h3_out.Idmx,"Idmx/B"); 		
  TGrande->Branch("Ieve",&h3_out.Ieve,"Ieve/i"); 		
  TGrande->Branch("Irun",&h3_out.Irun,"Irun/s"); 		 
  TGrande->Branch("Ifil",&h3_out.Ifil,"Ifil/b");   		
  TGrande->Branch("Ngrs",&h3_out.Ngrs,"Ngrs/B"); 		
  TGrande->Branch("Iact",&h3_out.Iact,"Iact/s"); 		
  TGrande->Branch("Xcg",&h3_out.Xcg,"Xcg/F");			
  TGrande->Branch("Ycg",&h3_out.Ycg,"Ycg/F");			
  TGrande->Branch("Azg",&h3_out.Azg,"Azg/F");			
  TGrande->Branch("Zeg",&h3_out.Zeg,"Zeg/F");			
  TGrande->Branch("Ymd",&h3_out.Ymd,"Ymd/i");			
  TGrande->Branch("Hms",&h3_out.Hms,"Hms/i");
  TGrande->Branch("Eg",&h3_out.Eg,"Eg/F");
							 
  							 
  int GrandeEntry = 0;
  unsigned int time_offset = 0;
  unsigned int delta_t = 0;
  GetTimeSearchWindow(&time_offset, &delta_t, AnaFlag.daq_id);
  
  float *time_diff = new float[TAnaFlag->GetEntries()];
  int found_GrandeEvents = 0;
  
  unsigned int Star_sec = 0; 
  unsigned int Star_nano = 0;
  int Star_STOP = TAnaFlag->GetEntries(); 
  
  for(int i=0; i<Star_STOP; i++){
    //status output
    printf("\t%i of %i\r",i, Star_STOP);
    if(!debug)fflush (stdout);
    else cout << endl;
    
    TAnaFlag->GetEntry(i);
    ConvertTimeStamp(AnaFlag.timestamp, (int*) &Star_sec, (int*) &Star_nano);
    if(debug) cout << "star timestamp: " << AnaFlag.timestamp << endl;
    //expected timestamp from Grande are 0.8s larger than Star time
    Star_nano += time_offset;
    if(Star_nano > 1.0e9){ // correct seconds after adding timeshift
      Star_sec++;
      Star_nano -= (unsigned int)(1.0e9);
    }
  
    for(int j=GrandeEntry; j<TGrande_orig->GetEntries(); j++){
      if( TGrande_orig->GetEntry(j) == 0){
       cerr << "read out of TGrande reconstruction file error or not enough data." << endl;
        break;
      }

//cout << h3.Gt << " -- " << Star_sec << " ----- " << h3.Mmn << " -- " << Star_nano << endl;
      if(h3.Gt == Star_sec && h3.Mmn+delta_t > Star_nano && h3.Mmn-delta_t < Star_nano){
	time_diff[found_GrandeEvents] = Star_sec+Star_nano/1.0e9-time_offset/1.0e9 - (h3.Gt+h3.Mmn/1.0e9);
        
	if(debug) cout << "trigger" << endl;
	if(debug) cout << "time diff = " <<  time_diff[found_GrandeEvents] << " (Star - Grande)" << endl;
	
	//Grande Energy Estimation:
	//R. Glasstetter
	//h3.Eg = (0.31*log10(h3.Sizeg)+0.67*log10(h3.Sizmg)+1.24/cos(h3.Zeg)+0.58);
	//M. Wommer
	h3.Eg = (0.319*log10(h3.Sizeg) + 0.709*log10(h3.Sizmg) + 1.236/cos(h3.Zeg) + 0.238);

	h3.Zeg *=  180.0/Pi();
	h3.Azg *=  180.0/Pi();
	h3_out = h3;
	
	GrandeEntry = j;
	found_GrandeEvents++;
        break;
      }
      
      if(h3.Gt > Star_sec){
        h3_out = Zeroh3;
	GrandeEntry = j-10;
	if(GrandeEntry < 0) GrandeEntry = 0;
	if(j==0) cout << "\nGrande events at begin of analysis time are missing! " << endl;
	if(debug) cout << "Grande time larger than STAR time." << endl;
	break;
      }
    }
    
    TGrande->Fill();
    
    if(GrandeEntry+1 == TGrande_orig->GetEntries()) {
      cerr << "end of Grande reconstruction file reached." << endl;
      break;
    }
    
  } //end for over TAnaFlag loop
  
  if(debug){
    cout << "TGrande entries = " << TGrande->GetEntries() << endl;
    cout << "TAnaFlag entries = " << TAnaFlag->GetEntries() << endl;
  }

  TGrande->AddFriend("TAnaFlag");
  output->cd();
  TGrande->Write(TGrande->GetName(),TObject::kWriteDelete);
  output->Close();
  
  //output:
  cout << "found " << found_GrandeEvents << " from " << Star_STOP << " Grande events in coincidence with STAR." << endl;
  printf("mean time difference are calculated to: %.9f +/- %.9f\n",Mean(found_GrandeEvents, time_diff),RMS(found_GrandeEvents, time_diff));
  cout << "expected time diffrence: " << time_offset/1.0e9 << " +/- " << delta_t / 1.0e9 << endl;

  //do not forget to clean up
  delete output;
  delete time_diff;

}
