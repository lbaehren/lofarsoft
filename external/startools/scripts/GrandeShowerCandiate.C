{
gROOT->Reset();
gSystem->Setenv("TZ","UTC");

  struct Sh3{
    unsigned int Gt;		/*!<global trigger time, in this case \f$\cong\f$ epoch time plus leap seconds*/
    unsigned int Mmn;		/*!<subseconds of Gt*/
    unsigned char Fanka;	/*!<bit array that describes whether GRANDE detected ANKA events or not.
    				    Fanka & 0x7 = 4 means an ANKA event is detected*/
    unsigned int Hit7;		/*!<bit array that describes which GRANDE circle has triggered
    				    Hit7 & 0x80 != 0x80 means GRANDE 17 has triggered*/
    char Nflg;			/*!<trigger flag from the Grande reconstruction*/
    float Ageg;			/*!<Age parameter of the LDF funciton out of the Grande recontstruction*/
    float Sizeg;		/*!<total no of electrons reconstructed by Grande*/
    float Sizmg;		/*!<total no of muons reconstructed by Grande*/
    char Idmx;			/*!<Station ID with maximum energy deposit, if max. dep. at a station on border then IDMX<0*/
    float Zeg;		    	/*!<zenith angle [rad] reconstructed by Grande*/
    char Ngrs; 			/*!<Number of Grande stations in event*/
    unsigned short Iact;	/*!<coded bit word for active parts of this run, if array is not active, then the muon no. is incorrect*/
    float Xcg; 			/*!<X value of the core position in m (Grande)*/
    float Ycg; 			/*!<Y value of the core position in m (Grande)*/
    float Azg;
  };
  
  struct Sh3 ntuple;

char *ntupleName = "$HOME/lopes/NTuple_Info/ntuple_run11.root";
TFile *ntupleFile = new TFile (ntupleName,"READ");
TTree *h3 = (TTree*) ntupleFile->Get("h3");

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
    h3->SetBranchAddress("Ngrs",&ntuple.Ngrs);
    h3->SetBranchAddress("Iact",&ntuple.Iact);
    h3->SetBranchAddress("Xcg",&ntuple.Xcg);
    h3->SetBranchAddress("Ycg",&ntuple.Ycg);
    h3->SetBranchAddress("Azg",&ntuple.Azg);




		
TCut cut="	(Fanka & 0x7) != 4 \
		&& (Hit7 & 0x80) == 0x80 \
		&& Nflg>0 \
		&& Ageg>0.4 && Ageg<1.4 \
	   	&& (0.31*log10(Sizeg)+0.67*log10(Sizmg)+1.24/cos(Zeg)+0.58)>8 \
		&& Zeg<40*2*3.14/360 \
		&& Ngrs>19 \
		&& TMath::Max(TMath::Abs(Xcg),TMath::Abs(Ycg+280) ) < 300  \
	   	&& (Iact & 0x1)==1 ";

int ShowerCandidate=0;
time_t rawtime;

for(int i=0; i<h3->GetEntries(); i++){
  h3->GetEntry(i);
  if(		(ntuple.Fanka & 0x7) != 4 
		&& (ntuple.Hit7 & 0x80) == 0x80 
		&& ntuple.Nflg>0 
		&& ntuple.Ageg>0.4 && ntuple.Ageg<1.4 
	   	&& (0.31*log10(ntuple.Sizeg)+0.67*log10(ntuple.Sizmg)+1.24/cos(ntuple.Zeg)+0.58)>8.5  
		&& ntuple.Zeg<40*2*3.14/360
		&& ntuple.Ngrs>19 
	   	//&& TMath::Max(TMath::Abs(ntuple.Xcg+250),TMath::Abs(ntuple.Ycg+280) ) < 400 
		&& TMath::Max(TMath::Abs(ntuple.Xcg),TMath::Abs(ntuple.Ycg+280) ) < 100 
	   	&& (ntuple.Iact & 0x1)==1 ) {
		
	ShowerCandidate++;
	//corrected for summertime output
	rawtime = (time_t)(ntuple.Gt);
	printf("Candidate: %02i\tEnergy = %02.2f (Gev)\tPosition: X=%4i Y=%4i\tazimuth = %.3f (rad)\ttime = (%i) - (corrected) %s",ShowerCandidate, 
	       0.31*log10(ntuple.Sizeg)+0.67*log10(ntuple.Sizmg)+1.24/cos(ntuple.Zeg)+0.58 ,ntuple.Xcg, ntuple.Ycg, ntuple.Azg, ntuple.Gt, ctime(&rawtime));
  }
  
}

cout << ShowerCandidate << endl;		
	   
	   
}
