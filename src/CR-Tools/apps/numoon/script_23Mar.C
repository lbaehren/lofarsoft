#include "TROOT.h"
#include "TSystem.h"
#include "TApplication.h"
#include <iostream>


Double_t time = 10;
Double_t TEC1=11.4;
Double_t TEC2=9.9;
Double_t TEC3=9.8;
Double_t TEC4=9.4;
Double_t TEC5=0.0;
Double_t TEC6=0.0;
Double_t TEC7=0.0;
Double_t TEC;


void script_23Mar()
{
    if (time < 361)
	TEC = (TEC2-TEC1)/360.*time + TEC1;
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
  

    //gROOT->LoadMacro("/home/numoon/analyses_final_v20.C+");
    gROOT->LoadMacro("/home/stijnbui/numoon/analysis_v21.C+");

    analyses_new_v20(TEC,90.0,160.0,90.0,30.0,1.4,1.4,1.2,1.2,1.0,1.0,1.0,1.0);
    //analyses_final_v20(TEC,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0);
    //gROOT->LoadMacro("/home/numoon/analyses_gaintest.C+");
    
    //analyses_gaintest(TEC,10000.0);
}
