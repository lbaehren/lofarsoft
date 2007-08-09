#include "TH2.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TNtuple.h"
#include "MyObject_newdata.h"
#include "TMath.h"
#include "TSpectrum.h"
#include "TStopwatch.h"
#include "TVirtualFFT.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
const Long_t numberToAnalyse = 10001, //const Long_t numberToAnalyse = 1024*8 + 1;
             TotalNumbers = numberToAnalyse*2 - 2;
const Int_t NumberOfSpectra = 200, //const Int_t NumberOfSpectra = 1024*40/numberToAnalyse;
            FileTotal = 100, FreqBands = 4;

Double_t Signalx[TotalNumbers], Signaly[TotalNumbers];

void readdata()
{
    int filestep=1, step_aver_fft=1;
    
     MyObject_newdata myMoon("/raid/pv2/numoon/freq1.data", 4096 + 2*NumberOfSpectra*TotalNumbers*filestep+(2*TotalNumbers*step_aver_fft));
     Bool_t onereadfailed = 0;
     Double_t re_x=0., im_x=0., re_y=0., im_y=0.;
	  
     for (Long_t event = 0; event < TotalNumbers; event++)
     {
	      
	if (myMoon.unpackEventC())
        {
 		 Signalx[event]=(Double_t) (myMoon.getSubEventX());
		 Signaly[event]=(Double_t) (myMoon.getSubEventY());
	}
	else onereadfailed = 1;
	
    }
    	
}	
