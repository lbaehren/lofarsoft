#include<cstdlib>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<time.h>

/* Root header files */
#include<TFile.h>
#include<TTree.h>
#include<TMath.h>
#include<TCut.h>

using namespace std;
using namespace TMath;

///***** Preparing Energy and primary Mass functions reconstructed by KASCADE (Ralph's formulas) ******///
double D=(-186.5562)+(1222.6562)*exp(-110.*100./994186.38); //atm depht at KASCADE level
double xatte=158.;
double xattm=823.;
double dwb1, lgNe0, lgNmu0, lgEn, lnM; //Ne, Nmu En in log, Mass in ln
double varNe, varNmu, dEdTheta, varTheta, errlgEn, errlnM, dMassdTheta;
//variables from "QGSjet2+FLUKA"
double matrix[2][2]={{0.31753, 0.66273},{-7.04813, 9.62589}};
double vector[]={1.79453, -4.19989};

//_______________________________________________________________________________
//                                                                     lg_EnergyK

/*!
  \brief Energy function
  
  \param lgNe
  \param lgNmu
  \param theta
*/
double lg_EnergyK (double lgNe,
		   double lgNmu,
		   double theta){ 
  // transform to average 0-18deg sizes
  dwb1=(D/cos((theta)/57.29578))-(D*1.025);
  lgNe0=lgNe+(dwb1/xatte/log(10.));
  lgNmu0=lgNmu+(dwb1/xattm/log(10.));
  //variables from "QGSjet2+FLUKA"
  double pare[]={ 1.93499, 0.25788, 0.66704, 0.07507, 0.09277, -0.16131};
  
  lgEn = pare[0] + pare[1]*lgNe0 + pare[2]*lgNmu0;
  lgEn += pare[3]*lgNe0*lgNe0 + pare[4]*lgNmu0*lgNmu0 + pare[5]*lgNe0*lgNmu0;
  return lgEn; //in log
}

//_______________________________________________________________________________
//                                                                 err_lg_EnergyK

/*!
  \brief Energy error function

  \param theta
  \param errlgNe
  \param errlgNmu
  \param errTheta
*/
double err_lg_EnergyK (double theta,
		       double errlgNe,
		       double errlgNmu,
		       double errTheta){
  
  varNe = pow(matrix[0][0]*errlgNe,2);
  varNmu = pow(matrix[0][1]*errlgNmu,2);
  dEdTheta = D/log(10.) * (matrix[0][0]/xatte+matrix[0][1]/xattm);
  dEdTheta *= sin((theta)/57.29578) / pow(cos((theta)/57.29578),2); 
  varTheta = pow(dEdTheta*(errTheta/57.29578),2);
  
  errlgEn=sqrt(varNe + varNmu + varTheta);
  return errlgEn;
}

//_______________________________________________________________________________
//                                                                        lg_Mass

/*!
  \brief Mass function
*/
double lg_Mass (double lgNe,
		double lgNmu,
		double theta){
  // transform to average 0-18deg sizes
  dwb1=(D/cos((theta)/57.29578))-(D*1.025);
  lgNe0=lgNe+(dwb1/xatte/log(10.));
  lgNmu0=lgNmu+(dwb1/xattm/log(10.));
  //variables from "QGSjet2+FLUKA"
  double parm[]={  -6.64185, -4.90102, 8.36298, -2.63904, -3.09907, 5.62201};
  
  lnM = parm[0] + parm[1]*lgNe0 + parm[2]*lgNmu0;
  lnM += parm[3]*lgNe0*lgNe0 + parm[4]*lgNmu0*lgNmu0 + parm[5]*lgNe0*lgNmu0;
  return lnM; //in ln
}

//_______________________________________________________________________________
//                                                                    err_lg_Mass

/*!
  \brief Mass error function
*/
double err_lg_Mass (double theta,
		    double errlgNe,
		    double errlgNmu,
		    double errTheta){
  
  varNe = pow(matrix[1][0]*errlgNe,2);
  varNmu = pow(matrix[1][1]*errlgNmu,2);
  dMassdTheta = D/log(10.) * (matrix[1][0]/xatte+matrix[1][1]/xattm);
  dMassdTheta *= sin((theta)/57.29578) / pow(cos((theta)/57.29578),2); 
  varTheta = pow(dMassdTheta*(errTheta/57.29578),2);
  
  errlnM=sqrt(varNe + varNmu + varTheta);
  return errlnM;
}

//_______________________________________________________________________________
//                                                                           main

int main(int argc, char* argv[]){
  if(argc!=4&&argc!=3){
    
    cout<<"\n Application to generate a eventlist based on TCut selection of a ROOT-Tree."<<endl
        <<" Syntax: ./genEventlist SEL10.root options.cfg [\"optional selection string\"] "<<endl
        <<" Where the optional selection string can be sth. like \"Nmu>5e5\"."<<endl
        <<" For more information look at README. "<<endl<<endl;
    
    cout<<"Wrong number of arguments!"<<endl;
    exit(1);
  }
  
  
  /*
   * Processing the config-file to set standart-cuts and give a minimum and
   * maximum geomagnetic angle and energy
   */
  
  ifstream conf(argv[2]);
  string buf;
  stringstream opt;
  char tmp[256];
  
  // variables to read in
  string cut_str, fn2005, fn2006, fn2007, fn2008, fn2009, namebase="eventlist";
  Double_t geomag_min=0, geomag_max=90, energy_min=0, energy_max=1e20;
  Bool_t createInfoFile, Grande;	
  
  while (conf.getline(tmp,256)){
    
    opt.clear();
    opt.str(tmp);
    opt>>buf;
    if(buf.length()==0||buf.at(0)=='#') { continue;}
    if(buf.compare("energy_min")==0) { opt >> energy_min; }
    if(buf.compare("energy_max")==0) { opt >> energy_max; }
    if(buf.compare("geomag_min")==0) { opt >> geomag_min; }
    if(buf.compare("geomag_max")==0) { opt >> geomag_max; }
    if(buf.compare("TCut")==0) { opt>>cut_str; }
    if(buf.compare("createInfoFile")==0) { opt>>createInfoFile; }
    if(buf.compare("fn2005")==0) { opt>>fn2005; }
    if(buf.compare("fn2006")==0) { opt>>fn2006; }
    if(buf.compare("fn2007")==0) { opt>>fn2007; }
    if(buf.compare("fn2008")==0) { opt>>fn2008; }
    if(buf.compare("fn2009")==0) { opt>>fn2009; }
    if(buf.compare("namebase")==0) { opt>>namebase; }
    buf="";
     }
 
  // Decide which reconstruction should be used...
  if(cut_str.find("sqrt(Yc*Yc+Xc*Xc)<")!=string::npos)
	Grande = false;
  else
	Grande = true;


  TCut cut((cut_str).c_str());
  if(argc==4)
	cut += argv[3];

  opt.clear();
  opt.str("");

  // Eventually some events get lost with this but the search is a lot of faster...
  if(!Grande && energy_min>1){
	double mumin = (energy_min * 5.0e5/2.5e17);
	opt<<mumin;
	opt>>buf;
	cut+= ( string("Nmu>")+buf ).c_str();
    }
  if( Grande && energy_min>1){
	double mumin = (energy_min * 6.0e5/2.0e17);
	opt<<mumin;
	opt>>buf;
	cut+= ( string("Sizmg>")+buf ).c_str();
    }



  // Prepare ROOT-Files ******************************************************************************
  cout<<"Applying cut:"<<endl<<"     "<<cut<<endl;
  TFile *f = new TFile( argv[1], "READ");
  TTree *t1 =  (TTree*)f->Get("h3");

  TFile *ftemp = new TFile ("tempfile.root", "RECREATE");
  TTree *t2 = t1->CopyTree(cut);

  UInt_t Gt, Mmn;
  Float_t Az,Ze,Xc,Yc,Xcg,Ycg,Azg,Zeg,Nmu,Lmuo,Size,Sizeg,Sizmg,Age,Ageg;

  t2->SetBranchAddress("Gt",&Gt);
  t2->SetBranchAddress("Mmn",&Mmn);
  t2->SetBranchAddress("Az",&Az);
  t2->SetBranchAddress("Ze",&Ze);
  t2->SetBranchAddress("Xc",&Xc);
  t2->SetBranchAddress("Yc",&Yc);
  t2->SetBranchAddress("Xcg",&Xcg);
  t2->SetBranchAddress("Ycg",&Ycg);
  t2->SetBranchAddress("Azg",&Azg);
  t2->SetBranchAddress("Zeg",&Zeg);
  t2->SetBranchAddress("Nmu",&Nmu);
  t2->SetBranchAddress("Lmuo",&Lmuo);
  t2->SetBranchAddress("Sizmg",&Sizmg);
  t2->SetBranchAddress("Size",&Size);
  t2->SetBranchAddress("Sizeg",&Sizeg);
  t2->SetBranchAddress("Age",&Age);
  t2->SetBranchAddress("Ageg",&Ageg);


  int nentries = t2->GetEntries();
  cout<<"Number of events that passed the cut conditions: "<<nentries<<endl<<endl;


  cout <<"\nScanning for events..."<<endl
       <<"TCut = "<<cut<<endl
       <<"min. geomag. angel = " <<geomag_min <<"    max geomag. angel = "<<geomag_max<<endl
       <<"min. energy = " <<energy_min <<"    max energy = "<<energy_max<<endl<<endl
       <<"filename(event) azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]\n"
       <<"============================================================================================\n";
	


  int count=0;
  string gtstring;
  char eventname[64];
  bool found=0;
  fstream fin;


  TFile *fout = new TFile( (namebase+".root").c_str(),"recreate");
  TTree *k = new TTree("k","KASCADE-Information for eventlist");
  Double_t energy, geomag, cos_geomag;
  Double_t lgE, lgEg, lnMass, lnMassg, err_lgE, err_lgEg, err_lnMass, err_lnMassg;

  k->Branch("Eventname",&eventname,"Eventname/C");
  k->Branch("energy",&energy,"energy/D");
  k->Branch("Size",&Size,"Size/F");
  k->Branch("Sizeg",&Sizeg,"Sizeg/F");
  k->Branch("Nmu",&Nmu,"Nmu/F");
  k->Branch("Lmuo",&Lmuo,"Lmuo/F");
  k->Branch("Sizmg",&Sizmg,"Sizmg/F");
  k->Branch("Xc",&Xc,"Xc/F");
  k->Branch("Yc",&Yc,"Yc/F");
  k->Branch("Xcg",&Xcg,"Xcg/F");
  k->Branch("Ycg",&Ycg,"Ycg/F");
  k->Branch("Az",&Az,"Az/F");
  k->Branch("Ze",&Ze,"Ze/F");
  k->Branch("Azg",&Azg,"Azg/F");
  k->Branch("Zeg",&Zeg,"Zeg/F");
  k->Branch("geomag",&geomag,"geomag/D");
  k->Branch("Gt",&Gt,"Gt/i");
  k->Branch("Mmn",&Mmn,"Mmn/i");
  k->Branch("lgE",&lgE,"lgE/D");
  k->Branch("err_lgE",&err_lgE,"err_lgE/D");
  k->Branch("lgEg",&lgEg,"lgEg/D");
  k->Branch("err_lgEg",&err_lgEg,"err_lgEg/D");
  k->Branch("lnMass",&lnMass,"lnMass/D");
  k->Branch("err_lnMass",&err_lnMass,"err_lnMass/D");
  k->Branch("lnMassg",&lnMassg,"lnMassg/D");
  k->Branch("err_lnMassg",&err_lnMassg,"err_lnMassg/D");


  // Additional eventlist info file *******************************************************************
  ofstream f1, f2;

  f1.open( (namebase+".txt").c_str(), ios::out);

  // Write eventlist header, including the selection conditions ***************************************
  f1    <<"Automatic generated eventlist for the following conditions:"<<endl
        <<"TCut = "<<cut<<endl
        <<"min. geomag. angel = " <<geomag_min <<"    max geomag. angel = "<<geomag_max<<endl
        <<"min. energy = " <<energy_min <<"    max energy = "<<energy_max<<endl<<endl
        <<"filename(event) azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]\n"
        <<"============================================================================================\n";



  if(createInfoFile){
       f2.open( (namebase+".info").c_str(), ios::out);
       f2<<"Filename		Gt	Mmn	Az	Ze	Geomag	Xc	Yc	Size	Sizeg 	Sizmg	Nmu	Lmuo	Energy	Grande"<<endl
       	 <<"=================================================================================================================================="<<endl;
   }

  // Loop over ROOT-File ******************************************************************************
  for (int i=0; i<nentries; i++){

   t2->GetEntry(i);
   found=0;

   time_t tm_s = Gt-1;
   tm * ptm = gmtime ( &tm_s );
   char tstr[32];
   strftime (tstr, 32, "%Y.%m.%d.%H:%M:%S", ptm);
   gtstring = tstr;


   // Looking for filenames in eventlists **********************************************************

   if( Gt >= 1104537600 && Gt < 1136073600 ) //Jahr 2005
	   fin.open(fn2006.c_str(), ios::in);

   else if( Gt >= 1136073600 && Gt < 1167609600 ) //Jahr 2006
	   fin.open(fn2006.c_str(), ios::in);

   else if( Gt >= 1167609600 && Gt < 1199145600 ) //Jahr 2007
	   fin.open(fn2007.c_str(), ios::in);

   else if ( Gt >= 1199145600 && Gt < 1230768000 ) //Jahr 2008
	   fin.open(fn2008.c_str(), ios::in);

   else if ( Gt >= 1230768000  && Gt < 1262390400 ) //Jahr 2009
	   fin.open(fn2009.c_str(), ios::in);


   else
       {
	   cerr<<"Not in timerange of 2005 to 2009. Skipping event...\n";
	   continue;
       }

   fin.clear();
   fin.seekg (0, ios::beg);

   while(fin>>buf){
	   if(buf.find(gtstring)!=string::npos){
	       gtstring=buf;
	       found=1;
	       break;
	     };
        }
   fin.close();
   if (!found) continue;
	
   // preparing filename for writing to root tree	
   gtstring.resize(64,char(0));
   strcpy (eventname, gtstring.c_str());

   // Calculating energy and geomagnetic angel for KASCADE and GRANDE data **************************
   cos_geomag=-1.;
   geomag=0;

   energy=0;

   lgE=0.;
   err_lgE=0.;
   lnMass=0.;
   err_lnMass=0.;
   lgEg=0.;
   lnMassg=0.;
   double errNe, errNmu, errTheta;
   double log10Size, log10Lmuo, errlog10Size,errlog10Lmuo, log10sizeg,log10sizmg; 

   errNe = (Size*20.)/100.;  //preliminar value!
   errNmu = (Lmuo*20.)/100.; //preliminar value!
   errTheta = 0.3;
   //transf into log10

   log10Size = Log10(Size);
   log10Lmuo = Log10(Lmuo);
   errlog10Size = (errNe)/(Size*log(10.));
   errlog10Lmuo = (errNmu)/(Lmuo*log(10.));
   log10sizeg = Log10(Sizeg);
   log10sizmg = Log10(Sizmg);


   ///********** Energy and primary Mass reconstructed by Grande (Wommer's formulas) *****///
   if(Azg!=0 && Zeg!=0 && Sizeg>0 && Sizmg>0){
      lgEg=(0.3069*log10sizeg)+(0.7064*log10sizmg)+(1.2699/TMath::Cos(Zeg))+0.2931;
      lnMassg=(-3.5822*log10sizeg)+(4.6829*log10sizmg)+(-6.3948*TMath::Cos(Zeg))+5.3495;
      err_lgEg=0.1;    //preliminar value!
      err_lnMassg=1.2; //preliminar value!
      }
   else{
      lgEg=0.;
      lnMassg=0.;
      err_lgEg=0.;
      err_lnMassg=0.;

      }

   ///********** Energy and primary Mass reconstructed by KASCADE (Ralph's formulas) ******///
    //NB- here no log---no LgE and LnA
   if (Az!=0 && Ze!=0 && Size>0 && Lmuo>0){
     //Energy and Energy error
     lgE = lg_EnergyK(log10Size,log10Lmuo,Ze);
     err_lgE = err_lg_EnergyK(Ze,errlog10Size, errlog10Lmuo, errTheta);     
     //Mass and Mass error
     lnMass=lg_Mass(log10Size,log10Lmuo,Ze);
     err_lnMass=err_lg_Mass(Ze,errlog10Size, errlog10Lmuo, errTheta);

      cout<<"energy k      "<<lgE<<endl;
      cout<<"err energy k  "<<err_lgE<<endl;
      cout<<"mass k        "<<lnMass<<endl;
      cout<<"err mass k    "<<err_lnMass<<endl;

    }


   if( !Grande && Sqrt(Xc*Xc+Yc*Yc)<90 && Az!=0 && Ze!=0 && Size>0 && Lmuo>0){
	   cos_geomag = TMath::Cos(Az) * TMath::Sin(Ze) * TMath::Cos(TMath::Pi()) * TMath::Sin(TMath::Pi()/180.0 * 25) + TMath::Sin(Az) * TMath::Sin(Ze) * TMath::Sin(TMath::Pi()) * TMath::Sin(TMath::Pi()/180.0 * 25)+TMath::Cos(Ze) * TMath::Cos(TMath::Pi()/180.0 * 25);   
	   
	   geomag = ACos(cos_geomag)*180./Pi();
	   
	   Double_t log10size = Log10(Size);
	   Double_t NeNmu = log10size-Log10(Lmuo);
    	   Double_t Ke = -0.0854 + 0.0634 *NeNmu -0.0115 *Power(NeNmu,2.);
    	   
 	   if ((log10size-Ke) > 6.304) 
		Ke = Ke + 0.000745 -0.0497 * Power((log10size-Ke-6.304),2.);

	   Double_t ne = Power(10. , (log10size-Ke));

    	   Double_t Kmu = -0.824 +0.472 *(log10size-Ke)  -.0939 * Power( (log10size-Ke),2) + 0.00639 * Power( (log10size - Ke) ,3);
           Double_t log10mu = Log10(Lmuo);
	   if ((log10mu-Kmu) > 5.22) 
              Kmu = Kmu -0.00293 -0.194*Power((log10mu-Kmu-5.22),2);
           Double_t log10ne = Log10(ne);
	   Double_t nm = Power(10,(log10mu - Kmu +0.6877 -0.03835*log10ne +0.001186*Power(log10ne,2)));

	   Double_t a = 1. / Cos(Ze) - 1.025;
	   energy = Power( 10.,(0.3015*( log10ne + 1023./158./2.302585093*a ) +0.6762*( Log10(nm) + 1023./823./2.302585093*a ) +1.856 +9.) );
       }
       else if (Grande && Azg!=0 && Zeg!= 0 && Sizeg>0 && Sizmg>0 && Xcg<-50&&Xcg>-420 &&Ycg<-30&&Ycg>-550)
       {
           cos_geomag = TMath::Cos(Azg) * TMath::Sin(Zeg) * TMath::Cos(TMath::Pi()) * TMath::Sin(TMath::Pi()/180.0 * 25) + TMath::Sin(Azg) * TMath::Sin(Zeg) * TMath::Sin(TMath::Pi()) * TMath::Sin(TMath::Pi()/180.0 * 25)+TMath::Cos(Zeg) * TMath::Cos(TMath::Pi()/180.0 * 25);

	   geomag = ACos(cos_geomag)*180./Pi();

	   energy = Power(10., ( 0.319*Log10(Sizeg) + 0.709*Log10(Sizmg) + 1.236/ Cos(Zeg) + 0.238 +9.));
       }
       else continue;
       
       // Printing KASCADE or GRANDE data **************************************************************

        if(  
	    found && 
	    geomag >= geomag_min &&  
	    geomag <= geomag_max 
          )
       { 
	   if( !Grande && Az!=0 && Ze!=0 && Age>=0.4&&Age<=1.4 && Ze<0.7854 && energy>energy_min && energy<energy_max)
           {
	       if( createInfoFile )
		   f2<<eventname<<"    "<< Gt<<"    "<< Mmn <<"    "<< Az*180./Pi()<<"    "<< Ze*180./Pi() <<"    "<< geomag<<"    "<< Xc <<"    "<< Yc <<"    "<< Size <<"    0    "<< Nmu<<"    "<< Lmuo <<"    "<< energy  <<"    0"<<endl;

	       f1<<eventname<<"    "<<Az*180./TMath::Pi()<<"    "<<90.-Ze*180./TMath::Pi()<<"    "<<"2500"<<"    "<<Xc<<"    "<<Yc<<endl;
	       cout<<eventname<<"    "<<Az*180./TMath::Pi()<<"    "<<90.-Ze*180./TMath::Pi()<<"    "<<"2500"<<"    "<<Xc<<"    "<<Yc<<endl;
	       count++;
	       k->Fill();
           } 
           else if ( Grande && Azg!=0 && Zeg!= 0 && Xcg<-50&&Xcg>-420 &&Ycg<-30&&Ycg>-550 &&Ageg>=-0.385&&Ageg<=1.485 && Zeg<0.7854 && energy>energy_min && energy<energy_max)
          {
	       if( createInfoFile )
		                      f2<<eventname<<"    "<< Gt<<"    "<< Mmn <<"    "<< Azg*180./Pi()<<"    "<< Zeg*180./Pi() <<"    "<< geomag<<"    "<< Xcg <<"    "<< Ycg <<"    0    "<< Sizeg <<"    "<< Sizmg<<"    0    "<< energy  <<"    1"<<endl;
	       f1<<eventname<<"    "<<Azg*180./TMath::Pi()<<"    "<<90.-Zeg*180./TMath::Pi()<<"    "<<"2500"<<"    "<<Xcg<<"    "<<Ycg<<endl;
	       cout<<eventname<<"    "<<Azg*180./TMath::Pi()<<"    "<<90.-Zeg*180./TMath::Pi()<<"    "<<"2500"<<"    "<<Xcg<<"    "<<Ycg<<endl;
	       count++;
	       k->Fill();
          }
       }
   }
k->Write();
fout->Close();
f1.close();
f2.close();

ftemp->Close();
ftemp->Delete();
cout<<namebase<<".txt written"<<endl
    <<namebase<<".root written"<<endl
    <<namebase<<".info written"<<endl;
}
