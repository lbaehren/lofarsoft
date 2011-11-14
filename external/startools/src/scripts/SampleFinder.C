{
gROOT->Reset();

//  char *StarFileName="$HOME/harddisc/run14_backup_00.root";
  char *StarFileName="$HOME/harddisc/run11_backup_00.root";
  
  TChain *Tevent = new TChain("Tevent");
  Tevent->Add(StarFileName);
  
  TChain *Theader = new TChain("Theader");
  Theader->Add(StarFileName);

  TChain *Tchannel_profile = new TChain("Tchannel_profile");
  Tchannel_profile->Add(StarFileName);


  char * pch;
  int fileNo=0;
  char name[1024];
  TFile *file;

  while(1){
    sprintf(name,"_%02i.root",fileNo);
    pch = strstr (StarFileName,name);
//  cerr << name << endl;
    fileNo++;
    sprintf(name,"_%02i.root",fileNo);
//  cerr << name <<  endl;
    if(pch!=NULL){
     strncpy (pch,name,8);
     file = new TFile(StarFileName,"READ");
    
     if(file->IsOpen()){
        Tevent->Add(StarFileName);
        Theader->Add(StarFileName);
	//Tchannel_profile->Add(StarFileName);
        cerr << "Add new file to TChain - " << StarFileName << endl;
      }
      else{
        delete file;
        break;
      }
    }
    else break;
    //cerr << StarFileName << endl;
    delete file;
  }
  int window_size=0;
  short int trace[131072];
  int event_id=0;
  
  Tevent->SetBranchAddress("event_id",&event_id);  
  Tevent->SetBranchAddress("window_size",&window_size);
  Tevent->SetBranchAddress("trace",&trace);

  int NoAntennas = Tchannel_profile->GetEntries();
///////////////////////////////   
  int START = 0;
 
  unsigned int event_id_start = 0;//10201245-1;//26772;
   for(int w=0; w<Tevent->GetEntries(); w++){
     Tevent->GetEntry(w);
     START = w;
     if(event_id == event_id_start) break;
     if(event_id > event_id_start) break;
   }
   Tevent->GetEntry(0);
   //cout << START << " START" << endl;

 int STOP = Tevent->GetEntries()-1; //START+NoAntennas*100;
///////////////////////////////
  
  if(STOP>= Tevent->GetEntries()){
   cerr << "entry doesn't exist" << endl;
   STOP = Tevent->GetEntries();
   cerr << "changed end number." <<  endl;
  // exit(1);
  }
  
 int pedestal = 2150;
  
 for(int i=START; i<STOP; i++){
   if(Tevent->GetEntry(i)==0){
     cerr << "error - Tevent->GetEntry()" << endl;
     exit(0);
   }
   
   for(unsigned int j=window_size/2-100; j<window_size/2+100; j++){
     if(trace[j]>(1500.0+pedestal)  && trace[j]<(2500+pedestal)){
       cout << "Trace sample > 1500+2150: event id = " << event_id << " (" << trace[j]-pedestal << ")" << endl;
     }
   }
   
   //cout << event_id << endl;

 }
}
