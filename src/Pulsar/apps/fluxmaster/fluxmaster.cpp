#include <time.h>                      // define time()

#include<iostream>
#include<fstream>
#include <sstream>
#include<cstdlib>
#include<cmath>
#include<iomanip>
#include<vector>
#include<algorithm>
#include<map>

#define PI 3.14159265358
#define c0 299.792458
#define KBOL 1.3806503e-23
#define d2r PI/180.0
#define r2d 180.0/PI

#include "cpgplot.h"


using namespace std;

class ProfileParameters
{ 
 public:
 
  void initialise();
  void passValues(char* &,bool &,float &,float &,float &,const float* ,const float* ,const float*, bool &, bool &, bool &);
  void process();
  void printValues();
  
  float fwhm;
  float s2n;
  float t_int;
  float flux;
 
 private:

  void parseTel();
  void read();
  void fitGauss();
  void calculateS2N();
  void radiometerEq();
  void altitudeCorrect();
  void plotProfile();
  void getRMS(vector<float> &);
  void freeVmem(vector<float>, const int);

  vector<float> freq;
  vector<float> aeff;
  vector<float> ssys;
  vector<float> noisePhaseBot;
  vector<float> noisePhaseTop;
  vector<float> function;
  vector<float> functionPhases;
  vector<float> signal;
  vector<float> posSignal;
  vector<float> bslSignal;
  vector<float> phases;
 
  float vmin(vector<float> &);
  float vmax(vector<float> &);
  float integrate(float, float);
  float func(float);

  vector<float> baseline(vector<float>&, float&);
  vector<float> makePosv(vector<float> &);

  char* fname;
  string psrname;
  
  float botPhaseLimit;
  float topPhaseLimit;
  float botNoiseLimit;
  float topNoiseLimit;
  float height;
  float centrePhase;
  float noise_rms;
  float noise_mean;
  float stdev;
  float bestDm;
  float pbary;
  float obsfreq;
  float zangle;
  float bw;
  float tsamp;

  int itel;
  int imax;
  long int nsamp;

  bool plot;
  bool mean;  
  bool verbose;  
  bool effelsberg;  
};


int main(int argc, char** argv)
{

   //Hardwired telescope parameters
   //LBA: 46 antennas
   //HBA: NL Core
   const float frequency[11]={  15.0,   30.0,   45.0,   60.0,   75.0,  120.0, 150.0, 180.0, 200.0, 210.0, 240.0 };
   const float area[11]={ 419.77, 419.77, 415.37, 347.37, 239.67, 600.0, 512.0, 356.0, 288.0, 261.0, 200.0 };
   const float sensitivity[11]={ 483.0,   89.0,   48.0,   32.0,   51.0,    3.6,   2.8,   3.2,   3.7,   3.7,   4.1 }; // in kJy
   //////////////////////////////////////////////////////////////////////////////////////////////

  if(argc==1)
  {
    cout <<endl<<endl<<endl;
    cout << "------------------"<<endl;
    cout << "FluxMaaasteeer (c)2009 ..." <<endl;
    cout <<endl<< "Usage: fluxmaster <filename> <observing frequency (MHz)> <bandwidth (MHz)> <source Z.A. (deg)> [options]" <<endl
         <<endl<< "Options: "<<endl
         <<       "         -plot    Plot profile with RMS(red error bar), noise windows (blue lines) and fitted Gaussian (green line)" <<endl
         <<       "         -mean    Calculate the mean flux instead of the (default) peak flux" <<endl
         <<       "         -eff     Use the sensitivity of Effelsberg instead of LOFAR" <<endl
         <<       "         -v       Verbose" <<endl;
	  
    cout << "------------------"<<endl<<endl;
    return 0;
  }

 

//Input parameters
        
        float observing_frequency= atof(argv[2]);
        float bandwidth= atof(argv[3]);
//        float t_int= atof(argv[4]);
        float zenith_angle=atof(argv[4]);

// Options
        bool plot=false;
        for(int i=0; i<argc; i++)
	  if(string(argv[i])=="-plot") plot=true;
        
        bool mean=false;
        for(int i=0; i<argc; i++)
	  if(string(argv[i])=="-mean") mean=true;

        bool verbose=false;
        for(int i=0; i<argc; i++)
	  if(string(argv[i])=="-v") verbose=true;

        bool effelsberg=false;
	for(int i=0; i<argc; i++)
	  if(string(argv[i])=="-eff") effelsberg=true;
                
	
    
//Begin computations
        ProfileParameters profile;
	
        profile.initialise();
	profile.passValues(argv[1],plot,observing_frequency,zenith_angle,bandwidth,frequency,area,sensitivity,mean,verbose,effelsberg);
	
        profile.process();
      
        cout <<endl<<  "------------------" <<endl;
	cout << "Flux: " << profile.flux << " mJy" <<endl; 
        cout <<        "------------------" <<endl<<endl;

        if(verbose) profile.printValues();

	return 0;
}



// Passes values to the class from the main programme
void ProfileParameters::passValues(char* &_fname,bool &_plot,
                                   float &_obsfreq,float &_zangle, float &_bandwidth,
				   const float* _frequency,const float* _area,const float* _sensitivity, bool &_mean, bool &_verbose, bool &_effelsberg)
{
  fname=_fname;
  plot=_plot;
  obsfreq=_obsfreq;
  zangle=_zangle;
  bw=_bandwidth;
  mean=_mean;
  verbose=_verbose;
  effelsberg=_effelsberg;
  
  for(int i=0; i<11; i++)
  {
    freq.push_back(_frequency[i]);
    aeff.push_back(_area[i]);
    ssys.push_back(_sensitivity[i]);
  }
  
}



//Initialises the passed values
void ProfileParameters::initialise()
{
  tsamp=0.0;
  nsamp=0;
  stdev=0.0;
  fwhm=0.0;
  height=0.0;
  centrePhase=0.0;
  s2n=0.0;
  noise_rms=0.0;
  noise_mean=0.0;
  t_int=0.0;

  botPhaseLimit=0.0;
  topPhaseLimit=0.0;

  botNoiseLimit=0.0;
  topNoiseLimit=0.0;

  noisePhaseBot.clear();
  noisePhaseTop.clear();

  signal.clear();
  posSignal.clear();
  bestDm=0.0;
  pbary=0.0;
  obsfreq=0.0; 
}



/// MAIN PROCESSING QUEUE /////
///////////////////////////////////////////////////////////
void ProfileParameters::process()
{
  parseTel();

  read();

  fitGauss();

  calculateS2N();

  radiometerEq();
   
  if(plot) plotProfile();
}
///////////////////////////////////////////////////////////



//DETERMINES THE MATRIX POSITION FOR THE SENSITIVITY FROM THE USER INPUT 
void ProfileParameters::parseTel()
{
  vector<float> diff;

  for(int i=0; i<11; i++)
    diff.push_back(fabs(obsfreq-freq[i]));

  itel=0;
  float diffmin=1e30;
  for(int i=0; i<11; i++)
    if(diff[i]<=diffmin)
    {
      diffmin=diff[i];
      itel=i;
    }

}


// READS THE PROVIDED PROFILE DATA
void ProfileParameters::read()
{

  ifstream datafile;
  datafile.open(fname);

  signal.clear();
  freeVmem(signal,2048);

  float dummy,f;
  char buffer[256];

  while(1)
  {
     datafile.getline(buffer,100);
     
     if(buffer[0]=='#')
       continue;
     else
       datafile >> dummy >> f;
     
     if(datafile.eof()) break;
     
     signal.push_back(f); 
  }

  
  datafile.clear();
  datafile.seekg(0,ios::beg);

  
  while(1)
  {
     datafile.getline(buffer,100);

     if(datafile.eof()) break;  

     if(buffer[0]=='#')
     {
       string buffer_str=buffer;
       
       if(buffer_str.substr(2,9)=="Candidate")
          psrname=buffer_str.substr(22).c_str();
       if(buffer_str.substr(2,7)=="Best DM")
          bestDm=atof(buffer_str.substr(22).c_str());
       if(buffer_str.substr(2,11)=="P_bary (ms)")
          pbary=atof(buffer_str.substr(22,15).c_str());
       if(buffer_str.substr(2,8)=="T_sample")
          tsamp=atof(buffer_str.substr(22).c_str());
       if(buffer_str.substr(2,11)=="Data Folded")
          nsamp=atoi(buffer_str.substr(22).c_str());
         
       t_int=float(tsamp*nsamp);
     }
  }  
  
  datafile.close();

}


// FITS A GAUSSIAN TO THE PROFILE
void ProfileParameters::fitGauss()
{

  posSignal=makePosv(signal);

  height=-1e3;
  centrePhase=0.0;

  for(int i=0; i<phases.size(); i++)
    if(posSignal[i]>=height)
    {
      height=posSignal[i];
      centrePhase=phases[i];
      imax=i;
    }

  botPhaseLimit=min(float(centrePhase-0.2),float(centrePhase));
  topPhaseLimit=min(float(centrePhase+0.2),float(1.0));

//  ofstream profiledata;
//  profiledata.open("profile.dat");

  float x=0.0;
  float expo=0.0;
  float probMax=-1e30;

  for(int i=0; i<30; i++)
  {
     float s=0.01+float(i)*0.01;
     
     float prob=0.0;
     for(int j=0; j<phases.size(); j++)
     {
        float phase=phases[j];
        float fu=height*exp(-0.5*(phase-centrePhase)*(phase-centrePhase)/(s*s));
        
        if(phase>=botPhaseLimit&&phase<=topPhaseLimit)
        {
          x=fu-posSignal[j];
    
//          if(i==0) profiledata << phases[j] << "    " << posSignal[j] << "  " << fu <<endl;

          expo=-x*x/2.0;
        }
        else expo=0.0;
        
        prob += expo;
     }

     if(prob>probMax)
     {
       probMax=prob;
       stdev=s;
       fwhm=2.0*sqrt(2.0*log(2.0))*s;
     }
  }
//  profiledata.close();


}




// CALCULATES THE MEAN OR PEAK S/N OF THE PROFILE 
void ProfileParameters::calculateS2N()
{
   
///    NOISE-LEVELS COMPUTATIONS /////   
   vector<float> noiseSignal;
   s2n=0.0;

   botNoiseLimit=max(float(centrePhase-1.5*fwhm),float(0.0));
   topNoiseLimit=min(float(centrePhase+1.5*fwhm),float(1.0));


   for(int i=0; i<phases.size(); i++)
   {

     if(phases[i]>=0.0 && phases[i]<=botNoiseLimit)
     {
        noiseSignal.push_back(posSignal[i]);
	noisePhaseBot.push_back(phases[i]);
     }
     if(phases[i]>=topNoiseLimit && phases[i]<1.0)
     {
        noiseSignal.push_back(posSignal[i]);
	noisePhaseTop.push_back(phases[i]);
     }
   }

   getRMS(noiseSignal);

   bslSignal=baseline(posSignal,noise_mean);
   
   

//  SIGNAL-TO-NOISE COMPUTATIONS  //
   if(mean)
   {
     float phaseZero[2];
     phaseZero[0]=centrePhase;
     phaseZero[1]=centrePhase;

     float dphi=fabs(phases[imax]-phases[imax+1]);

     float ampSum=0.0;
     for(int i=0; i<2; i++)
     {
	int ibit=-1+2*i;
	int k=imax;
	while(bslSignal[k]>0.0)
	{
            ampSum+=bslSignal[k];//*dphi;
	    k+=ibit;
        }
     }


     if(verbose)
     {
     	cout <<endl<<endl;
	cout << "S/N-computation parameters:"<<endl;
	cout << "    Area under profile:        " << ampSum <<endl;
	cout << "    Noise RMS:                 " << noise_rms <<endl;
	cout << "    Peak signal:               " << height-noise_mean <<endl;
	cout <<endl<<endl;
     }

     s2n=sqrt(ampSum*(height-noise_mean))/noise_rms;
     
   }
   else
     s2n=(height-noise_mean)/noise_rms;

}



// THE RADIOMETER EQUATION
void ProfileParameters::radiometerEq()
{
//Fixed parameters
   float np=2.0;
   float effsys=16.6;

   if(effelsberg)
     flux=s2n*effsys*sqrt(fwhm)/(sqrt(t_int*np*bw*(1.0-fwhm)));
   else
     flux=s2n*ssys[itel]*1e3*sqrt(fwhm)/(sqrt(t_int*np*bw*(1.0-fwhm)));
       
   float s2nExp=1.6/(effsys*sqrt(fwhm)/(sqrt(t_int*np*bw*(1.0-fwhm))));
   
   cout <<endl <<endl << "Expected S/N: " << s2nExp <<endl<<endl;    
       
   altitudeCorrect();   
}


// ALTITUDE CORRECTIONS
void ProfileParameters::altitudeCorrect()
{
   zangle *= PI/180.0;

   float wavelength=c0/freq[itel];
   float x=sin(zangle);

   float correction_factor=pow(wavelength*sin(1.25*PI*x/wavelength)/(1.25*PI*x),4);

   if(zangle==0.0 || effelsberg) correction_factor=1.0;

   flux /= correction_factor;
}


/////////////////  GRAPHICS  ////////////////////////////////////////
void ProfileParameters::plotProfile()
{

   for(int i=0; i<1000; i++)
   {
     float phs=botPhaseLimit+i*(topPhaseLimit-botPhaseLimit)/1000.0;
     function.push_back(func(phs));
     functionPhases.push_back(phs);
   }


   cpgbeg(0,"/xs",1,1);	

   cpgpap(8.0,0.7);

   cpgswin(phases.front(), phases.back(), vmin(bslSignal), 1.1*vmax(bslSignal));

//   cpgenv(phases.front(), phases.back(), 0.0, 1.1*height,1,1);

   cpgbox("bcnst",0.0,0,"bcnvst",0.0,0);


///  FLUXMASTER LABEL
   cpgscf(2);
   cpgsch(0.85);
   cpgmtxt("LV",-56,-0.11,0.5,"FluxMaster (c)2009");

///  Y-AXIS LABEL
   cpgscf(1);
   cpgsch(1);

   cpgmtxt("L",3.5,0.5,0.5,"Flux (a.u.)");

///  X-AXIS LABEL
   cpglab("Phase","","");            

// BASELINE
   cpgsci(2);
   cpgslw(1);
   cpgsls(4);
   cpgmove(0.0,0.0);
   cpgdraw(phases.back(),0.0);

///  FLUX PROFILE
   cpgsls(1);
   cpgsci(1);
   cpgline(bslSignal.size(),&phases.front(),&bslSignal.front());

   cpgsci(3);//green	  
   cpgmove(phases.front(), bslSignal.front());

///  GAUSSIAN FUNCTION
   cpgline(function.size(),&functionPhases.front(),&function.front());



//  RMS ERRORBAR
   cpgsci(2);
   cpgsls(2);
   cpgmove(phases.front(), bslSignal.front());

   int maxNoiseBins=max(noisePhaseBot.size(),noisePhaseTop.size());
   float rmsBarPhase=0.0;

   if(noisePhaseBot.size()==maxNoiseBins)
     rmsBarPhase=noisePhaseBot[maxNoiseBins/2];
   else
     rmsBarPhase=noisePhaseTop[maxNoiseBins/2];
   
   cpgslw(10);
   cpgsls(1);
   
   cpgerr1(6,rmsBarPhase,0.0,noise_rms,5.0);

   

//NOISE WINDOWS
   cpgslw(4);
   cpgsci(4);
   cpgsls(1);
   cpgmove(phases.front(), bslSignal.front());

   float noiseLineBotPhase=0.5*(noisePhaseBot.front()+noisePhaseBot.back());
   float noiseLineBotHWidth=0.5*(noisePhaseBot.back()-noisePhaseBot.front());

   float noiseLineTopPhase=0.5*(noisePhaseTop.front()+noisePhaseTop.back());
   float noiseLineTopHWidth=0.5*(noisePhaseTop.back()-noisePhaseTop.front());

   cpgerr1(5,noiseLineBotPhase,0.98*vmax(bslSignal),noiseLineBotHWidth,3.0);

//   cpgmove(noisePhaseTop.front(),0.98*height);
   
   cpgerr1(5,noiseLineTopPhase,0.98*vmax(bslSignal),noiseLineTopHWidth,3.0);


/// VARIOUS LABELS
   cpgsci(1);
   cpgslw(3);
   cpgscf(2);
   cpgsch(1.2);
   cpgmtxt("LV",-5.0,1.05,0.5,psrname.c_str());  //pulsar name
   
   ostringstream dmString_stm;
   dmString_stm << bestDm;
   string dmString = "DM: "+dmString_stm.str()+" pc cm-3";
      
   cpgmtxt("LV",-35.0,1.05,0.5,dmString.c_str()); //pulsar dm   

   ostringstream pbaryString_stm;
   pbaryString_stm << pbary;
   string pbaryString = "Period(bary): "+pbaryString_stm.str()+" ms";
      
   cpgmtxt("LV",-19.0,1.05,0.5,pbaryString.c_str()); //pulsar period
   
   ostringstream s2nString_stm;
   s2nString_stm << s2n;
   string s2nString = "S/N: "+s2nString_stm.str();
      
   cpgmtxt("LV",-37.0,0.97,0.5,s2nString.c_str()); //pulsar period
     

}


// PRINTS USEFUL INFORMATION UPON VERBOSITY REQUEST
void ProfileParameters::printValues()
{
    cout <<endl<<endl<<endl;
    cout << "-------------------------"<<endl;

    cout  << "Observing frequency:     " << obsfreq << "  MHz" <<endl 
          << "Filename:                " << fname   << "     " <<endl 
          << "Number of samples:       " << nsamp   << "     " <<endl 
          << "Sample length:           " << tsamp   << "  sec" <<endl 
          << "Observation length:      " << t_int   << "  sec" <<endl 
          << "Gaussian-fit properties: "                   <<endl 
          << "       Fit range: ["<< botPhaseLimit << "-" << topPhaseLimit  << "]" <<endl 
	  << "       Mean:      "                << centrePhase        <<endl 
          << "       Height:    "                << height-noise_mean  <<endl 
          << "       Stdev:     "                << stdev              <<endl 
          << "       FWHM:      "                 << fwhm               <<endl 
          << "Noise windows: [0.0-"  << botNoiseLimit   << "],["<< topNoiseLimit <<"-1.0]"<<endl 
          << "Noise RMS:     "           << noise_rms       <<endl 
          << "Profile S/N:   "         << s2n             <<endl 

          << "-------------------------"<<endl
          <<endl<<endl<<endl;

}




///////////// TOOLS //////////////////////


// COMPUTES THE RMS & MEAN OF A VECTOR
void ProfileParameters::getRMS(vector<float> &v)
{
  float mean=0.0;
  float rms=0.0;

  for(int i=0; i<v.size(); i++)
      mean+=v[i]/float(v.size());

  for(int i=0; i<v.size(); i++)
     rms+=(v[i]-mean)*(v[i]-mean)/float(v.size());

  noise_mean=mean;
  noise_rms=sqrt(rms);


}


// BASELINES A PROFILE STORED IN A VECTOR
vector<float> ProfileParameters::baseline(vector<float> &v,float &mean)
{
  vector<float> bv;
  
  for(int i=0; i<v.size(); i++)
    bv.push_back(v[i]-mean);

  return bv;
}


// SHIFTS THE PROFILE TO POSITIVE VALUES (MIN=0)
vector<float> ProfileParameters::makePosv(vector<float> &v)
{
  float minval=1e30;
  vector<float> posv;
  
  for(int i=0; i<v.size(); i++)
    if(v[i]<=minval)
      minval=v[i];

  freeVmem(posv,2048);
  phases.clear();
  
  for(int i=0; i<v.size(); i++)
  {
    posv.push_back(v[i]-minval);
    phases.push_back(float(i)/float(v.size()-1));
  }

  return posv;
}


// MINIMUM VALUE OF A VECTOR
float ProfileParameters::vmin(vector<float> &v)
{
  float min=1e20;
  
  for(int i=0; i<v.size(); i++)
    if(v[i]<=min) min=v[i];

  return min;
}


// MAXIMUM VALUE OF A VECTOR
float ProfileParameters::vmax(vector<float> &v)
{
  float max=-1e20;
  
  for(int i=0; i<v.size(); i++)
    if(v[i]>=max) max=v[i];

  return max;
}

// FREES MEMORY FOR A VECTOR
void ProfileParameters::freeVmem(vector<float> v, int n)
{
  v.reserve(n);
}


// INTEGRATES A FUNCTION F(X) 
float ProfileParameters::integrate(float a, float b) {

	double I=0.0;
	float dx=0.0001;
	float x=a+dx/2.0;
	do
	{
		I += func(x)*dx;
		x+=dx;
		
	}while(x<b);

	return I;
}

// THE FUNCTION (A GAUSSIAN)
float ProfileParameters::func(float x) {return height*exp(-0.5*(x-centrePhase)*(x-centrePhase)/(stdev*stdev))-noise_mean;}

