{
gROOT->Reset();
gSystem->Setenv("TZ","UTC");


TChain *h3 = new TChain("h3");

//D30 run: 21-49
if(0){
 h3->Add("SEL8.05897f01-05904f16.root");
 h3->Add("SEL8.05904f17-05910f02.root");
 h3->Add("SEL8.05910f03-05923f31.root");
}
//D30 run: 50-83
if(0){
 h3->Add("SEL8.05910f03-05923f31.root");
 h3->Add("SEL8.05923f32-05936f06.root");
 h3->Add("SEL8.05936f07-05940f25.root");
 h3->Add("SEL8.05940f26-05955f07.root");
}

//D17: run12-60
if(1){
 h3->Add("SEL8.06132f05-06176f03.root");
 h3->Add("SEL8.06176f04-06205f12.root");
 h3->Add("SEL8.06205f13-06236f05.root");
 h3->Add("SEL8.06236f06-06238f01.root");
}

/*

 posx = -100
 posy = -200
 varx = 200;
 vary = 200;
 
 Zenith vs. Azimuth
 Zeg*180.0/3.14:Azg*180.0/3.14
 
 Energy cut:
 && (0.31*log10(Sizeg)+0.67*log10(Sizmg)+1.24/cos(Zeg)+0.58)>8.7
 
h3->Draw("Ycg:Xcg","(Fanka & 0x7) != 4 && (Iact & 0x1)==1 && Ageg>0.4 && Ageg<1.4 && Ngrs>19 && ( Xcg<-100+200 && Xcg>-100-200 && Ycg<-200+200 && Ycg>-200-200)","colz")
//cut auf ganzes Grande Array
h3->Draw("Ycg:Xcg","(Fanka & 0x7) != 4 && (Iact & 0x1)==1 && Ageg>0.4 && Ageg<1.4 && Ngrs>19 && ( Xcg<0+200 && Xcg>0-600 && Ycg<0+200 && Ycg>0-600)","colz")


STAR
Run: 21 - 49 
=> Grande Files:
	SEL8.05897f01-05904f16.root
	SEL8.05904f17-05910f02.root
	SEL8.05910f03-05923f31.root
Run: 50 - 83
=> Grande Files:
	SEL8.05910f03-05923f31.root
	SEL8.05923f32-05936f06.root
	SEL8.05936f07-05940f25.root
	SEL8.05940f26-05955f07.root
*/



  struct Sh3{
    unsigned int Gt;		/*!<global trigger time, in this case \f$\cong\f$ epoch time plus leap seconds*/
    unsigned int Mmn;		/*!<subseconds of Gt*/
    unsigned char Fanka;	/*!<bit array that describes whether GRANDE detected ANKA events or not.
    				    Fanka & 0x7 = 4 means an ANKA event is detected*/
    UChar_t Ifil;		/*!<File ID*/				    
    unsigned int Hit7;		/*!<bit array that describes which GRANDE circle has triggered
    				    Hit7 & 0x80 != 0x80 means GRANDE 17 has triggered*/
    char Nflg;			/*!<trigger flag from the Grande reconstruction*/
    UInt_t Ymd;			/*!<date*/
    UInt_t Hms;			/*!<time*/
    float Ageg;			/*!<Age parameter of the LDF funciton out of the Grande recontstruction*/
    float Sizeg;		/*!<total no of electrons reconstructed by Grande*/
    float Sizmg;		/*!<total no of muons reconstructed by Grande*/
    char Idmx;			/*!<Station ID with maximum energy deposit, if max. dep. at a station on border then IDMX<0*/
    unsigned int Ieve;		/*!<Event ID*/
    UShort_t Irun;		/*!<run ID*/
    float Zeg;		    	/*!<zenith angle [rad] reconstructed by Grande*/
    char Ngrs; 			/*!<Number of Grande stations in event*/
    unsigned short Iact;	/*!<coded bit word for active parts of this run, if array is not active, then the muon no. is incorrect*/
    float Xcg; 			/*!<X value of the core position in m (Grande)*/
    float Ycg; 			/*!<Y value of the core position in m (Grande)*/
    float Azg;			/*!<azimuth angle [rad] reconstructed by Grande*/
  };
  
  struct Sh3 ntuple;

//char *ntupleName = "$HOME/lopes/NTuple_Info/ntuple_run11.root";
//TFile *ntupleFile = new TFile (ntupleName,"READ");
//TTree *h3 = (TTree*) ntupleFile->Get("h3");

    h3->SetBranchAddress("Gt",&ntuple.Gt);
    h3->SetBranchAddress("Mmn",&ntuple.Mmn);
    h3->SetBranchAddress("Fanka",&ntuple.Fanka);
    h3->SetBranchAddress("Hit7",&ntuple.Hit7);
    h3->SetBranchAddress("Nflg",&ntuple.Nflg);
    h3->SetBranchAddress("Ageg",&ntuple.Ageg);
    h3->SetBranchAddress("Sizeg",&ntuple.Sizeg);
    h3->SetBranchAddress("Sizmg",&ntuple.Sizmg);
    h3->SetBranchAddress("Zeg",&ntuple.Zeg);
    h3->SetBranchAddress("Idmx",&ntuple.Idmx);
    h3->SetBranchAddress("Ieve",&ntuple.Ieve);
    h3->SetBranchAddress("Irun",&ntuple.Irun);
    h3->SetBranchAddress("Ifil",&ntuple.Ifil);   
    h3->SetBranchAddress("Ngrs",&ntuple.Ngrs);
    h3->SetBranchAddress("Iact",&ntuple.Iact);
    h3->SetBranchAddress("Xcg",&ntuple.Xcg);
    h3->SetBranchAddress("Ycg",&ntuple.Ycg);
    h3->SetBranchAddress("Azg",&ntuple.Azg);
    h3->SetBranchAddress("Ymd",&ntuple.Ymd);
    h3->SetBranchAddress("Hms",&ntuple.Hms);




		
int ShowerCandidate=0;
time_t rawtime;
cout << "entries = " << h3->GetEntries() << endl;
cout << "Candiate:" <<endl;

int posx = 0; //-250;
int posy = 0; //-450;
int varx1 = 100;
int varx2 = 600;
int vary1 = 100;
int vary2 = 600;

// LOPES-STAR plus one hour = Grande time 
//when using timestamps in seconds from database
int event_time = 1168537654;

for(int i=0; i<h3->GetEntries(); i++){
  h3->GetEntry(i);
  
  if(		(ntuple.Fanka & 0x7) != 4 
/*		&& (ntuple.Hit7 & 0x1000) == 0x1000  // only Circle 13 on
		//&& ntuple.Hit7 > 8 // alle Cluster
//		&& ntuple.Nflg>0 
		&& ntuple.Ageg>0.4 && ntuple.Ageg<1.4 
	   	&& (0.31*log10(ntuple.Sizeg)+0.67*log10(ntuple.Sizmg)+1.24/cos(ntuple.Zeg)+0.58)>8.7  //GeV
//		&& TMath::Log10(ntuple.Sizeg)>7.7
//		&& ntuple.Zeg<40*3.14/180
//		&& ntuple.Ngrs>19 
//		&& ( ntuple.Xcg<posx+varx1 && ntuple.Xcg>posx-varx2 && ntuple.Ycg<posy+vary1 && ntuple.Ycg>posy-vary2)
	   	&& (ntuple.Iact & 0x1)==1 
		&& ntuple.Gt > (event_time-20)
		&& ntuple.Gt < (event_time+10)
//		&& 0
*/		 ) {
		
	ShowerCandidate++;
	rawtime = (time_t)(ntuple.Gt);
	
//	printf("%02i\tEnergy = %02.2f (Gev)\tPosition: X=%4i Y=%4i\tazimuth = %06.2f\tzenith = %05.2f\t%s",ShowerCandidate,	       0.31*log10(ntuple.Sizeg)+0.67*log10(ntuple.Sizmg)+1.24/cos(ntuple.Zeg)+0.58 ,ntuple.Xcg, ntuple.Ycg, ntuple.Azg*180/3.14, ntuple.Zeg*180/3.14, ctime(&rawtime));
	//cout << ntuple.Ymd << " " << ntuple.Hms << endl;
//        cout << ntuple.Gt << " --  " << ntuple.Mmn << endl;
//	cout << "entry = " << i << endl;
        
	//output for STAR time search
	if(0) cout << ntuple.Irun << "\t" << ntuple.Ieve <<  "\t" << ntuple.Gt << "\t" << ntuple.Ymd << "\t" << ntuple.Hms << "\t" << ntuple.Mmn << "\t" << ntuple.Xcg << "\t" << ntuple.Ycg << "\t" << ntuple.Zeg*180.0/3.14 << "\t" << ntuple.Azg*180.0/3.14 << "\t" << (0.31*log10(ntuple.Sizeg)+0.67*log10(ntuple.Sizmg)+1.24/cos(ntuple.Zeg)+0.58) << endl;

  }
//  if( ntuple.Gt > (event_time+10) ) break;

if(1){  // search for Grande Events with defined output
  if(i==0)cout << "run \t event \t global time \t YMD \t HMS \t Mmn \t X \t\t Y \t\t zenith \t azimuth \t energy/GeV" << endl;
  
  if(	(ntuple.Fanka & 0x7) != 4 
  	&& (ntuple.Iact & 0x1)==1 
	&& (ntuple.Hit7 & 0x1000) == 0x1000
	&& ntuple.Ageg>0.4 
	&& ntuple.Ageg<1.4 
	&& ntuple.Ngrs>19 
	&& ( ntuple.Xcg<0+100 && ntuple.Xcg>0-600 && ntuple.Ycg<0+100 && ntuple.Ycg>0-600) 
	&& (0.31*log10(ntuple.Sizeg)+0.67*log10(ntuple.Sizmg)+1.24/cos(ntuple.Zeg)+0.58)>8.7){

  ShowerCandidate++;
  cout << ntuple.Irun << "\t" << ntuple.Ieve <<  "\t" << ntuple.Gt << "\t" << ntuple.Ymd << "\t" << ntuple.Hms << "\t" << ntuple.Mmn << "\t" << ntuple.Xcg << "\t" << ntuple.Ycg << "\t" << ntuple.Zeg*180.0/3.14 << "\t" << ntuple.Azg*180.0/3.14 << "\t" << (0.31*log10(ntuple.Sizeg)+0.67*log10(ntuple.Sizmg)+1.24/cos(ntuple.Zeg)+0.58) << endl;
}  
  
 }
 
 
}

cout << ShowerCandidate << endl;		

	h3->GetEntry(0);
	rawtime = (time_t)(ntuple.Gt);
	printf("\n\nfirst event\tEnergy = %02.2f (Gev)\tPosition: X=%4i Y=%4i\tazimuth = %06.2f\tzenith = %05.2f\t%s", 0.31*log10(ntuple.Sizeg)+0.67*log10(ntuple.Sizmg)+1.24/cos(ntuple.Zeg)+0.58 ,ntuple.Xcg, ntuple.Ycg, ntuple.Azg*180/3.14, ntuple.Zeg*180/3.14, ctime(&rawtime));
	
	long long int entries = h3->GetEntries();   
	h3->GetEntry(entries-2);
	rawtime = (time_t)(ntuple.Gt);
	printf("last event\tEnergy = %02.2f (Gev)\tPosition: X=%4i Y=%4i\tazimuth = %06.2f\tzenith = %05.2f\t%s", 0.31*log10(ntuple.Sizeg)+0.67*log10(ntuple.Sizmg)+1.24/cos(ntuple.Zeg)+0.58 ,ntuple.Xcg, ntuple.Ycg, ntuple.Azg*180/3.14, ntuple.Zeg*180/3.14, ctime(&rawtime));
	   
}
