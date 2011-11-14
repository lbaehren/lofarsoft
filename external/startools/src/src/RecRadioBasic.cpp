#include "RecRadioBasic.hh"

#include <iostream>
#include <time.h>
#include <math.h>
#include <Riostream.h>
#include <cstdlib>


//!Fast Fourier Transform of the West Version 3
#include <fftw3.h>


//!root 
#include <TROOT.h>
#include <TSystem.h>

//!RecRadio headers
#include "RecRadioCalib.hh"
#include "RecRadioTrigger.hh"
#include "RecRadioUtil.hh"


using namespace std;
using namespace TMath;



/**********************************************************************************************/

void SubtractPedestal(int window_size, short int *trace, int start, int stop){

   int TraceMean=0;
   if(stop==-1) stop = window_size/2-100;
   
   if(stop>window_size){
     cerr << "SubstractPedestal: stop > window_size" << endl;
     exit(1);
   }
   if(start<0){
     cerr << "SubstractPedestal: start < 0" << endl;
     exit(1);
   }
   
   for(int j=start; j<stop; j++) TraceMean += trace[j];
   TraceMean /= (stop-start);
   for(int j=0; j<window_size; j++) trace[j] -= TraceMean;
   

}

/**********************************************************************************************/

void SubtractPedestal(int window_size, float *trace, int start, int stop){

   float TraceMean=0;
   if(stop==-1) stop = window_size/2-100;
   
   if(stop>window_size){
     cerr << "SubstractPedestal: stop > window_size" << endl;
     exit(1);
   }
   if(start<0){
     cerr << "SubstractPedestal: start < 0" << endl;
     exit(1);
   }

   for(int j=start; j<stop; j++) TraceMean += trace[j];
   TraceMean /= (stop-start);
   for(int j=0; j<window_size; j++) trace[j] -= TraceMean;
   

}


/**********************************************************************************************/

