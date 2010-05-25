/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Frank Schröder, Moses Ender, Katrin Link, Nunzia Palmieri             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#include<cstdlib>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<time.h>

#include<TFile.h>
#include<TTree.h>
#include<TMath.h>
#include<TCut.h>

/*!
  \file genEventlist.cc

  \ingroup CR_Applications

  \brief Creates eventlist (selection of LOPES events) for call_pipeline

  \author Moses Ender, Frank Schr&ouml;der, Katrin Link, Nunzia Palmieri

  \date 2009/17/12

  <h3>Usage</h3>

  \verbatim
  ./genEventlist SEL10.root options.cfg "optional selection string"
  \endverbatim

  <h3>Prerequisites</h3>

  options.cfg

  Example:
  \verbatim
  # Option file to generate a LOPES-eventlist from the SEL10 KASCADE file

  # Filenames of eventlists of 2007 and 2008
  fn2005                  /users/iklx/lopesuser/code/genEventlist/eventlist-2005.txt
  fn2006                  /users/iklx/lopesuser/code/genEventlist/eventlist-2006.txt
  fn2007                  /users/iklx/lopesuser/code/genEventlist/eventlist-2007.txt
  fn2008                  /users/iklx/lopesuser/code/genEventlist/eventlist-2008.txt
  fn2009                  /users/iklx/lopesuser/code/genEventlist/eventlist-2009.txt

  # use default kascade cut
  # Fanka<4&&Ze<0.7854&&Age>0.4&&Age<1.4&&sqrt(Yc*Yc+Xc*Xc)<90

  useKASCADE

  # use default Grande cut (Fabiana's cuts)
  # Fanka<4&&Zeg<0.7854&&Ageg>-0.385&&Ageg<1.485&&Hit7>0
  # &&Xcg<-50&&Xcg>-420&&Ycg<-30&&Ycg>-550
  # &&Sizmg>1111&&Sizeg>11111&&Idmx>0&&Nflg>0&&Ndtg>11&&log10(Nctot/8.5)>(1/4.2*(2.9*log10(Sizeg)-8.4))

  #useGrande

  # additional cuts (e.g., time range)
  #TCut                   Ymd>051201&&Ymd<100101


  # Define geomagnetic angel range
  geomag_min              0
  geomag_max              180

  # Define energy range for the selection
  energy_min              1e17
  energy_max              1e20

  # Select KRETA version of KASCADE results
  KRETAversion            1.18/05

  # Write an additional textfile with all information
  createInfoFile          1

  # create script for copying event files
  createCopyScript        0

  # Name of the different files that should be created ( .txt, .info and .root )
  namebase                eventlist
  \endverbatim

*/

using namespace std;
using namespace TMath;


   ///***** Preparing Energy and primary Mass functions reconstructed by KASCADE (Ralph's formulas) ******///
   double D=(-186.5562)+(1222.6562)*exp(-110.*100./994186.38); //atm depht at KASCADE level
   double xatte=158.;
   double xattm=823.;
   double dwb1, lgNe0, lgNmu0, lgEn, lnM; //Ne, Nmu En in log, Mass in ln
   //double varNe, varNmu, dEdTheta, varTheta, errlgEn, errlnM, dAdTheta;
   //variables from "QGSjet2+FLUKA"
   double matrix[2][2]={{0.31753, 0.66273},{-7.04813, 9.62589}};
   double vector[]={1.79453, -4.19989};


   // Energy function***********************
   double lg_EnergyK(double lgNe, double lgNmu, double theta) //theta in rad && "Nmu" NOT truncated muon number!!!
   { 
     // transform to average 0-18deg sizes
     dwb1=(D/cos(theta))-(D*1.025);
     lgNe0=lgNe+(dwb1/xatte/log(10.));
     lgNmu0=lgNmu+(dwb1/xattm/log(10.));
    //variables from "QGSjet2+FLUKA"
     double pare[]={ 1.93499, 0.25788, 0.66704, 0.07507, 0.09277, -0.16131};

     lgEn = pare[0] + pare[1]*lgNe0 + pare[2]*lgNmu0;
     lgEn += pare[3]*lgNe0*lgNe0 + pare[4]*lgNmu0*lgNmu0 + pare[5]*lgNe0*lgNmu0;
     return lgEn; //in log
   }
  /*    // Energy error function***********************
    double err_lg_EnergyK(double theta, double errlgNe, double errlgNmu, double errTheta){

     varNe = pow(matrix[0][0]*errlgNe,2);
     varNmu = pow(matrix[0][1]*errlgNmu,2);
     dEdTheta = D/log(10.) * (matrix[0][0]/xatte+matrix[0][1]/xattm);
     dEdTheta *= sin(theta) / pow(cos(theta),2); 
     varTheta = pow(dEdTheta*errTheta,2);

     errlgEn=sqrt(varNe + varNmu + varTheta);
     return errlgEn;
    }*/
    
   // Mass function***********************
   double lg_A(double lgNe, double lgNmu, double theta) 
   {
     // transform to average 0-18deg sizes
     dwb1=(D/cos(theta))-(D*1.025);
     lgNe0=lgNe+(dwb1/xatte/log(10.));
     lgNmu0=lgNmu+(dwb1/xattm/log(10.));
    //variables from "QGSjet2+FLUKA"
     double parm[]={-6.64185, -4.90102, 8.36298, -2.63904, -3.09907, 5.62201};

     lnM = parm[0] + parm[1]*lgNe0 + parm[2]*lgNmu0;
     lnM += parm[3]*lgNe0*lgNe0 + parm[4]*lgNmu0*lgNmu0 + parm[5]*lgNe0*lgNmu0;
     return lnM; //in ln
   }
  /*  // Mass error function***********************
     double err_lg_A(double theta, double errlgNe, double errlgNmu, double errTheta){

      varNe = pow(matrix[1][0]*errlgNe,2);
      varNmu = pow(matrix[1][1]*errlgNmu,2);
      dAdTheta = D/log(10.) * (matrix[1][0]/xatte+matrix[1][1]/xattm);
      dAdTheta *= sin(theta) / pow(cos(theta),2); 
      varTheta = pow(dAdTheta*errTheta,2);
 
      errlnM=sqrt(varNe + varNmu + varTheta);
      return errlnM;
    }*/


int main(int argc, char* argv[])
{
  if(argc!=4&&argc!=3) {

    cout<<"\n Application to generate a eventlist based on TCut selection of a ROOT-Tree."<<endl
        <<" Syntax: ./genEventlist SEL10.root options.cfg [\"optional selection string\"] "<<endl
        <<" Where the optional selection string can be sth. like \"Nmu>5e5\"."<<endl
        <<" For more information look at README. "<<endl<<endl;

    cout<<"Wrong number of arguments!"<<endl;
    exit(1);
  }


  // Processing the config-file to set standart-cuts and give a minimum and maximum geomagnetic angle 
  // and energy **************************************************************************************

  ifstream conf(argv[2]);
  string buf;
  stringstream opt;
  char tmp[16000];

  // variables to read in
  string cut_str(""), fn2005(""), fn2006(""), fn2007(""), fn2008(""), fn2009(""), namebase="eventlist";
  Double_t geomag_min=0, geomag_max=180, energy_min=0, energy_max=1e20;
  Bool_t createInfoFile=false, createCopyScript=false, preferGrande=false, useBothReconstructions=false;
  bool minimumCuts = true; // set to true to make things faster.
  string KRETAversion("");
  char KRETAver[1024];

   TCut cut;

  while (conf.getline(tmp,16000)) {
    opt.clear();
    opt.str(tmp);
    opt>>buf;
    if(buf.length()==0||buf.at(0)=='#') 
      continue;
    if(buf.compare("energy_min")==0) 
      opt >> energy_min;
    if(buf.compare("energy_max")==0) 
      opt >> energy_max;
    if(buf.compare("geomag_min")==0) 
      opt >> geomag_min;
    if(buf.compare("geomag_max")==0) 
      opt >> geomag_max;
    if(buf.compare("TCut")==0)
      opt>>cut_str;
    if(buf.compare("noMinimumCuts")==0) {
      cut = TCut("");
      minimumCuts = false; 
    }  
    if(buf.compare("useBothReconstructions")==0) {
      useBothReconstructions = true; 
      cut = TCut("(Fanka<4&&Ze<0.7854&&Age>0.4&&Age<1.4&&sqrt(Yc*Yc+Xc*Xc)<90)||(Fanka<4&&Zeg<0.7854&&Ageg>-0.385&&Ageg<1.485&&Hit7>0&&Xcg<-50&&Xcg>-420&&Ycg<-30&&Ycg>-550&&Sizmg>1111&&Sizeg>11111&&Idmx>0&&Nflg>0&&Ndtg>11&&log10(Nctot/8.5)>(1/4.2*(2.9*log10(Sizeg)-8.4)))");
    }  
    if(buf.compare("useKASCADE")==0) 
      cut = TCut("Fanka<4&&Ze<0.7854&&Age>0.4&&Age<1.4&&sqrt(Yc*Yc+Xc*Xc)<90");
    if(buf.compare("useGrande")==0) 
      cut =
      TCut("Fanka<4&&Zeg<0.7854&&Ageg>-0.385&&Ageg<1.485&&Hit7>0&&Xcg<-50&&Xcg>-420&&Ycg<-30&&Ycg>-550&&Sizmg>1111&&Sizeg>11111&&Idmx>0&&Nflg>0&&Ndtg>11&&log10(Nctot/8.5)>(1/4.2*(2.9*log10(Sizeg)-8.4))");
    if(buf.compare("createInfoFile")==0) 
      opt>>createInfoFile;
    if(buf.compare("createCopyScript")==0) 
      opt>>createCopyScript;
    if(buf.compare("fn2005")==0) 
      opt>>fn2005;
    if(buf.compare("fn2006")==0) 
      opt>>fn2006;
    if(buf.compare("fn2007")==0) 
      opt>>fn2007;
    if(buf.compare("fn2008")==0)
      opt>>fn2008;
    if(buf.compare("fn2009")==0)
      opt>>fn2009;
    if(buf.compare("namebase")==0) 
      opt>>namebase;
    if(buf.compare("KRETAversion")==0) 
      opt>>KRETAversion;
   
    buf="";
  }
  // add cut_string to cut
  cut += cut_str.c_str();
  
  // Decide which reconstruction should be used...
  if(string(cut).find("sqrt(Yc*Yc+Xc*Xc)<")!=string::npos) {
    cout << "\nUsing KASCADE cut..." << endl;
    preferGrande = false;
  } else if (string(cut).find("Hit7>0")!=string::npos) {
    cout << "\nUsing Grande cut..." << endl;
    preferGrande = true;
  } else
    cerr << "\n\nWARNING: Either KASCADE or Grande default cuts should be used!" << endl;

  if(argc==4)
    cut += argv[3];


  // Eventually some events get lost with this but the search is a lot of faster...
  opt.clear();
  opt.str("");
  if (minimumCuts) {
     // KASCADE miminum muon number
     double Kmumin = (energy_min * 6.0e5/2.0e17);
     double Gmumin = (energy_min * 5.0e5/2.5e17);
     opt<<"((Sizmg>"<<Kmumin<<")||(Nmu>"<<Gmumin<<"))";
     cut+= opt.str().c_str();
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
  
  // check if KRETA version was provided
  if (KRETAversion == "") {
    cerr << "\n\nERROR: Please define KRETA version of KASCADE reconstruction in option file." << endl;
    return 1;
  } else {
    cout << "\nImportant: Please confirm KRETA version of input file!\n"
         << "(if neccessary change KRETA version in option file)\n"
         << "KRETA version = " << KRETAversion 
         << "\nCorrect? --> if not, change config file and call genEventlist again.\n"
         << endl;
    strncpy(KRETAver,KRETAversion.c_str(),1023);     
  }

  cout <<"\nScanning for events..."<<endl
       <<"TCut = "<<cut<<endl
       <<"min. geomag. angel = " <<geomag_min <<"    max geomag. angel = "<<geomag_max<<endl
       <<"min. energy = " <<energy_min <<"    max energy = "<<energy_max<<endl<<endl
       <<"filename(event) azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]\n"
       <<"============================================================================================\n";

  int listCount=0;
  int cutCount=0;
  string gtstring;
  char eventname[64];
  bool found=0;
  fstream fin;

  TFile *fout = new TFile( (namebase+".root").c_str(),"recreate");
  TTree *k = new TTree("k","KASCADE-Information for eventlist");
  Double_t energy, geomag, cos_geomag;
  Double_t lgE, lgEg, lnA, lnAg, err_lgE, err_lgEg, err_lnA, err_lnAg;
  Double_t err_core, err_coreg, err_Az, err_Azg, err_Ze, err_Zeg;
  Double_t geomag_angle, geomag_angleg;
  
  k->Branch("Eventname",&eventname,"Eventname/C");
  k->Branch("Size",&Size,"Size/F");
  k->Branch("Sizeg",&Sizeg,"Sizeg/F");
  k->Branch("Age",&Age,"Age/F");
  k->Branch("Ageg",&Ageg,"Age/F");
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
  k->Branch("Gt",&Gt,"Gt/i");
  k->Branch("Mmn",&Mmn,"Mmn/i");

  // add energy, mass and errors
  k->Branch("lgE",&lgE,"lgE/D");
  k->Branch("err_lgE",&err_lgE,"err_lgE/D");
  k->Branch("lgEg",&lgEg,"lgEg/D");
  k->Branch("err_lgEg",&err_lgEg,"err_lgEg/D");
  k->Branch("lnA",&lnA,"lnA/D");
  k->Branch("err_lnA",&err_lnA,"err_lnA/D");
  k->Branch("lnAg",&lnAg,"lnAg/D");
  k->Branch("err_lnAg",&err_lnAg,"err_lnAg/D");

  // add error estimation for some KASCADE(-Grande) parameters
  k->Branch("err_core",&err_core,"err_core/D");
  k->Branch("err_coreg",&err_coreg,"err_coreg/D");
  k->Branch("err_Az",&err_Az,"err_Az/D");
  k->Branch("err_Azg",&err_Azg,"err_Azg/D");
  k->Branch("err_Ze",&err_Ze,"err_Ze/D");
  k->Branch("err_Zeg",&err_Zeg,"err_Zeg/D");
  
  // geomagnetic angle
  k->Branch("geomag_angle",&geomag_angle,"geomag_angle/D");
  k->Branch("geomag_angleg",&geomag_angleg,"geomag_angleg/D");

  // KRETA version
  k->Branch("KRETAver",&KRETAver,"KRETAver/C");
  

  // Additional eventlist info file *******************************************************************
  ofstream f1, f2, copyScript;

  f1.open( (namebase+".txt").c_str(), ios::out);

  // Write eventlist header, including the selection conditions ***************************************
  f1    <<"Automatic generated eventlist for the following conditions:"<<endl
        <<"TCut = "<<cut<<endl
        <<"min. geomag. angel = " <<geomag_min <<"    max geomag. angel = "<<geomag_max<<endl
        <<"min. energy = " <<energy_min <<"    max energy = "<<energy_max<<endl<<endl
        <<"filename(event) azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]\n"
        <<"============================================================================================\n";

  if (createInfoFile) {
       f2.open( (namebase+".info").c_str(), ios::out);
       f2<<"Filename                Gt        Mmn        Az        Ze        Geomag        Xc        Yc        Size        Sizeg         Sizmg        Nmu        Lmuo        Energy        Grande"<<endl
       <<"=================================================================================================================================="<<endl;
   }
   
  if (createCopyScript) {
    copyScript.open( (namebase+"-copy.sh").c_str(), ios::out);
    copyScript << "# bash script for copying event files" << endl
               << "# Go to destination directory and execute script." << endl
               << endl;
  }


  // get seperate strings for day month and year
  string day, month, year;
  
  // store GT and MMN of last event to get rid of double events;
  UInt_t lastGt = 0, lastMmn = 0;

  int nentries = t2->GetEntries();
  cout<<"Number of events that passed the cut conditions on the ROOT file: "<<nentries<<endl<<endl;

  // Loop over ROOT-File ******************************************************************************
  for (int i=0; i<nentries; i++) {
    t2->GetEntry(i);
    // continue if event is twice in KRETA reconstruction (happens in a few cases)
    if ((lastGt == Gt)&&(lastMmn==Mmn)) {
      cout << "Skipping event at GT " << Gt << " which seems to be twice in root file." << endl;
      continue;
    }  
    else {
      lastGt = Gt;
      lastMmn = Mmn;
    }
    
    // check for events which have neither KASCADE nor Grande reconstruction available
    if ((Sizeg<=0||Sizmg<=0) && (Size<=0||Nmu<=0)) {
      cout << "Skipping event at GT " << Gt << ", as neither KASCADE nor Grande reconstruction is available/complete." << endl;
      continue;
    } 
    
    // set the reconstruction to the prefered one
    bool Grande = preferGrande;
    
    found=0;
    time_t tm_s = Gt-1;
    tm * ptm = gmtime ( &tm_s );
    char tstr[32];
    strftime (tstr, 32, "%Y.%m.%d.%H:%M:%S", ptm);
    gtstring = tstr;
    
    // Looking for filenames in eventlists **********************************************************

    if( Gt >= 1104537600 && Gt < 1136073600 ) //Jahr 2005
      fin.open(fn2005.c_str(), ios::in);
    else if( Gt >= 1136073600 && Gt < 1167609600 ) //Jahr 2006
      fin.open(fn2006.c_str(), ios::in);
    else if( Gt >= 1167609600 && Gt < 1199145600 ) //Jahr 2007
      fin.open(fn2007.c_str(), ios::in);
    else if ( Gt >= 1199145600 && Gt < 1230768000 ) //Jahr 2008
      fin.open(fn2008.c_str(), ios::in);
    else if ( Gt >= 1230768000  && Gt < 1262390400 ) //Jahr 2009
      fin.open(fn2009.c_str(), ios::in);
    else {
      cerr<<"Not in timerange of 2005 to 2009. Skipping event...\n";
      continue;
    }

    // create substrings for day, month and year
    char temp[] = "XX";
    temp[0] = tstr[2]; temp[1] = tstr[3];
    year = string(temp);
    temp[0] = tstr[5]; temp[1] = tstr[6];
    month = string(temp);
    temp[0] = tstr[8]; temp[1] = tstr[9];
    day = string(temp);
    
    fin.clear();
    fin.seekg (0, ios::beg);
    //cout << "Looking for event: " << gtstring << endl;

    while(fin>>buf) {
      if(buf.find(gtstring)!=string::npos) {
        gtstring=buf;
        found=1;
        break;
      }
    }
    fin.close();
    if (!found) {
      cout << "Event: " << gtstring << "*.event could not be found in input eventlist." << endl;
      continue;
    }  

    // preparing filename for writing to root tree        
    gtstring.resize(64,char(0));
    strcpy (eventname, gtstring.c_str());

    // Calculating energy and geomagnetic angel for KASCADE and GRANDE data **************************
    cos_geomag=-1.;
    geomag=0;
    energy=0;

    //double errNe, errNmu, errTheta, errlog10Size,errlog10Lmuo,;
    double log10Size, log10Lmuo, log10Nmu, log10sizeg,log10sizmg;
    double FDeltaA, FDeltaTheta;  

    //errNe = (Size*20.)/100.;  //preliminar value!
    //errNmu = (Lmuo*20.)/100.; //preliminar value!
    //errTheta = 0.3;

    //transf into log10
    log10Size = Log10(Size);
    log10Lmuo = Log10(Lmuo);
    log10Nmu = Log10(Nmu);
    //errlog10Size = (errNe)/(Size*log(10.));
    //errlog10Lmuo = (errNmu)/(Lmuo*log(10.));
    log10sizeg = Log10(Sizeg);
    log10sizmg = Log10(Sizmg);

    ///********** Energy and primary Mass reconstructed by Grande (Wommer's formulas) *****///
    if( !(Azg==0||Zeg==0) && Sizeg>0 && Sizmg>0) { // Ze and Az in rad
      // calculate energy and mass
      lgEg=(0.3069*log10sizeg)+(0.7064*log10sizmg)+(1.2699/TMath::Cos(Zeg))+0.2931;
      lnAg=(-3.5822*log10sizeg)+(4.6829*log10sizmg)+(-6.3948*TMath::Cos(Zeg))+5.3495;
      // calculate energy and mass error
      if (exp(lnAg)<=1.5) {
        err_lgEg=0.1588;  //from Michael's plots
        err_lnAg=2.13;
      } else {
        err_lgEg=0.0922;
        err_lnAg=( ((1.17 - 1.26) / (60.-2.))*exp(lnAg) + 1.263); //from Michael's plots, linear dependence
      }  

      // use fixed error for core and angular uncertainties
      // (approximation for E > 10^17 eV)
      err_coreg = 7.;
      err_Azg = 0.006; // ~0.35°
      err_Zeg = 0.003; // ~0.17°

      // calculate geomagnetic angle
      cos_geomag =   TMath::Cos(Azg) * TMath::Sin(Zeg) * TMath::Cos(TMath::Pi()) * TMath::Sin(TMath::Pi()/180.0 * 25)
                   + TMath::Sin(Azg) * TMath::Sin(Zeg) * TMath::Sin(TMath::Pi()) * TMath::Sin(TMath::Pi()/180.0 * 25)
                   + TMath::Cos(Zeg) * TMath::Cos(TMath::Pi()/180.0 * 25);
      geomag_angleg = ACos(cos_geomag);
    } else {
      lgEg=0.;
      lnAg=0.;
      err_lgEg=0.;
      err_lnAg=0.;
      err_coreg = 0.;
      err_Azg = 0.;
      err_Zeg = 0.;
      geomag_angleg = 0.;
      if ((!useBothReconstructions) && (Grande))
        continue;
    }

    ///********** Energy and primary Mass reconstructed by KASCADE (Ralph's formulas) ******///
    //NB- here no log---no LgE and LnA
    if ( !(Az==0||Ze==0) && Size>0 && Nmu>0) {
      //Energy and Energy error
      lgE = lg_EnergyK(log10Size,log10Nmu,Ze);
      //err_lgE = err_lg_EnergyK(Ze,errlog10Size, errlog10Nmu, errTheta);
      //Mass and Mass error
      lnA=lg_A(log10Size,log10Nmu,Ze);
      //err_lnA=err_lg_A(Ze,errlog10Size, errlog10Nmu, errTheta);

      // mass dependent energy error from Michael's plots
      if(exp(lnA)<1.5)
        err_lgE = 0.17;
      else 
        err_lgE = 0.72;

      // mass and zenith dependent mass error from Michael's plots  
      if(Ze<(14./57.29578))     // Ze < 14°
        FDeltaTheta=1./1.265;
      else
        FDeltaTheta=(((2.05 - 1.1)/((40.-14.)/57.29578))*(Ze) + 0.6)/1.265;

      if (exp(lnA)<1.5)
        FDeltaA=2.12/1.269;
      else
        FDeltaA=(((0.58 - 1.226)/(60.-2.)) *(exp(lnA)) + 3.247)/1.269;

      err_lnA = 1.267*FDeltaA*FDeltaTheta;

      // use fixed error for core and azimuth uncertainties
      // (approximation for E > 10^17 eV)
      err_core = 4.;
      err_Az = 0.009; // ~0.5°

      // energy dependent zenith error
      if (lgE < 8.2)
        err_Ze = 0.003; // ~0.17°
      else   
        err_Ze = 0.007; // ~0.4°
  
      // calculate geomagnetic angle
      cos_geomag =   TMath::Cos(Az) * TMath::Sin(Ze) * TMath::Cos(TMath::Pi()) * TMath::Sin(TMath::Pi()/180.0 * 25)
                   + TMath::Sin(Az) * TMath::Sin(Ze) * TMath::Sin(TMath::Pi()) * TMath::Sin(TMath::Pi()/180.0 * 25)
                   + TMath::Cos(Ze) * TMath::Cos(TMath::Pi()/180.0 * 25);  
      geomag_angle = ACos(cos_geomag);
    } else {
      lgE = 0.;
      lnA = 0.;
      err_lgE = 0.;
      err_lnA = 0.;
      err_core = 0.;
      err_Az = 0.;
      err_Ze = 0.;
      geomag_angle = 0.;
      // if no KASCADE reconstruction is available, continue if KASCADE is wanted
      if ((!useBothReconstructions) && (!Grande))
        continue;
    }

    // calculate geomagnetic angle and determine energy for cutting
    // if both reconstructions should be used, now the one with the higher energy is chosen;
    if (useBothReconstructions) {
      if ((lgE<lgEg) && (Power(10.,lgEg)*1e9<energy_max)) {
        Grande = true;
        geomag = geomag_angleg*180./Pi();
        energy = Power(10.,lgEg)*1e9;   // energy in eV, but lgEg in GeV      
      } else {
        Grande = false;
        geomag = geomag_angle*180./Pi();
        energy = Power(10.,lgE)*1e9;   // energy in eV, but lgEg in GeV      
      }
    } else {
      if( Grande  && Azg!=0 && Zeg!= 0 && Sizeg>0 && Sizmg>0 ) {
        geomag = geomag_angleg*180./Pi();
        energy = Power(10.,lgEg)*1e9;   // energy in eV, but lgEg in GeV
      } else if (!Grande && Az!=0 && Ze!=0 && Size>0 && Lmuo>0) {
        geomag = geomag_angle*180./Pi();
        energy = Power(10.,lgE)*1e9;   // energy in eV, but lgEg in GeV
      } else
        continue;
    }       
    
    // Apply final cuts for energy and geomagnetic angle
    if ((energy<energy_min) || (energy>energy_max))
      continue;
    if ((geomag <= geomag_min) || (geomag >= geomag_max))
      continue;

    // increase counter of events which passed all cuts
    cutCount++;
       
    // Printing KASCADE or GRANDE data **************************************************************
    if(found) { 
      if( !Grande && (Az!=0 || Ze!=0) ) {
        if( createInfoFile )
          f2<<eventname<<"    "<< Gt<<"    "<< Mmn <<"    "<< Az*180./Pi()<<"    "<< Ze*180./Pi() <<"    "
            << geomag<<"    "<< Xc <<"    "<< Yc <<"    "<< Size <<"    0    "<< Nmu<<"    "<< Lmuo <<"    "<< energy  <<"    0"<<endl;
        if (createCopyScript) {
          copyScript << "cp /lxdata/lopes/" << year << "/" << month << "/" << day << "/" << eventname << "* ." << endl;
          copyScript << "bunzip2 " << eventname << "*bz2" << endl;
          copyScript << "rm " << eventname << "*bz2" << endl;
        }
        f1<<eventname<<"    "<<Az*180./TMath::Pi()<<"    "<<90.-Ze*180./TMath::Pi()<<"    "<<"2500"<<"    "<<Xc<<"    "<<Yc<<endl;
        cout<<eventname<<"    "<<Az*180./TMath::Pi()<<"    "<<90.-Ze*180./TMath::Pi()<<"    "<<"2500"<<"    "<<Xc<<"    "<<Yc<<endl;
        listCount++;
        k->Fill();
      } else if ( Grande && (Azg!=0 || Zeg!= 0) ) {
        if( createInfoFile )
          f2<<eventname<<"    "<< Gt<<"    "<< Mmn <<"    "<< Azg*180./Pi()<<"    "<< Zeg*180./Pi() 
            <<"    "<< geomag<<"    "<< Xcg <<"    "<<  Ycg <<"    0    "<< Sizeg <<"    "<< Sizmg<<"    0    "<< energy  <<"    1"<<endl;
        if (createCopyScript) {
          copyScript << "cp /lxdata/lopes/" << year << "/" << month << "/" << day << "/" << eventname << "* ." << endl;
          copyScript << "bunzip2 " << eventname << "*bz2" << endl;
          copyScript << "rm " << eventname << "*bz2" << endl;
        }
        f1<<eventname<<"    "<<Azg*180./TMath::Pi()<<"    "<<90.-Zeg*180./TMath::Pi()<<"    "<<"2500"<<"    "<<Xcg<<"    "<<Ycg<<endl;
        cout<<eventname<<"    "<<Azg*180./TMath::Pi()<<"    "<<90.-Zeg*180./TMath::Pi()<<"    "<<"2500"<<"    "<<Xcg<<"    "<<Ycg<<endl;
        listCount++;
        k->Fill();
      }
    }
  }

  cout << cutCount << " events passed final cuts." << endl;
  cout << listCount << " events put into eventlist (should be the same)." << endl;

  if (createCopyScript) {
    system (string("chmod 733 "+namebase+"-copy.sh").c_str());
    cout << "\nCopy script created." << endl;
    copyScript.close();
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
