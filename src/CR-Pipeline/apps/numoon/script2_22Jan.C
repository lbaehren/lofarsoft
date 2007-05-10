#include "TROOT.h"
#include "TSystem.h"
#include "TApplication.h"
#include <iostream>


Double_t time = 0;
Double_t TEC1=9.8;
Double_t TEC2=9.8;
Double_t TEC3=10.2;
Double_t TEC4=9.7;
Double_t TEC5=8.2;
Double_t TEC6=11.5;
Double_t TEC7=16.76;
Double_t TEC;


void script2_22Jan()
{
    if (time < 361)
	TEC = (TEC2-TEC1)/360.* time + TEC1;
    else if(time <721)
	TEC = (TEC3-TEC2)/360.*(time-360) + TEC2;
    else if(time <1081)
	TEC = (TEC4-TEC3)/360.*(time-720) + TEC3;
    else if(time <1441)
	TEC = (TEC5-TEC4)/360.*(time-1080) + TEC4;
    else if(time <1801)
	TEC = (TEC6-TEC5)/360.*(time-1440) + TEC5;
    else if(time <2190)
	TEC = (TEC7-TEC6)/360.*(time-1800) + TEC6;
  
    
    gROOT->LoadMacro("/home/numoon/analyses_final_v20.C+");
    
    analyses_final_v20(TEC,110.0,140.0,230.0,60.0,1.5,1.5,1.5,1.5,0.6,1.0,0.9,1.1);
    //analyses_final_v20(TEC,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0);
    //gROOT->LoadMacro("/home/numoon/analyses_gaintest.C+");
    
    //analyses_gaintest(TEC,10000.0);
}
