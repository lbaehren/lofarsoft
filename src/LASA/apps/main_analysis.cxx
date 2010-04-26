#include <strstream>
#include <stdlib.h>
#include <string.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraphPolar.h>
#include <TF1.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TObjArray.h>
#include <TFile.h>
#include <TTree.h>
#include <TLatex.h>
#include "constants.h"
#include "shower.h"
#include "detector.h"
#include "analysis.h"
#include "event_time.h"
#include "arrival_direction.h"

double fit_theta(double *x,double *par)			//Function to be fitted for theta distribution
{
	return par[0]*sin(x[0]*pi/180)*pow(cos(x[0]*pi/180),par[1]) ;	//f=A*sin(x)*(cos(x)^n)
}

TDirectory *Dir_Tel[NO_DETs] ;
TFile *out ;
void Create_ROOT_File()			
{
	out=new TFile("../output/output.root","recreate") ;
}

void Create_ROOT_SubDirectories1(int det)	//creating subdirectories for the "Detectors"
{
	char det_no[30] ;
	sprintf(det_no,"Det%d",det) ;
	Dir_Tel[det-1]=out->mkdir(det_no) ;
	strcpy(det_no,"") ;
}

int main(int argc, char** argv)
{
	float x_cor[NO_DETs],y_cor[NO_DETs],z_cor[NO_DETs] ;	//Detector coordinates
	float total_count[NO_DETs] ;			//Store total count of a signal (ADC counts)
	unsigned long Event_Arr_Time[NO_DETs] ;		//Store event arrival time (in units of 0.1 nsecs) 
	long long unsigned Final_Event_Time[NO_DETs] ;	//Store final event arrival time (in units of 0.1 nsecs)
	float dt ;			//time difference between 2 successive events in millisecs 
	long long unsigned t0 ;		//in 0.1 nsecs (use for calculating dt)
	float R_Core2 ;			//Square of event core distance from the origin in meters
	float Run_Time=0 ;		//Total run time (in secs)
	long long unsigned Run0 ;	//use in calculating Run_Time
	
	for(int i=0;i<NO_LASAs;i++)
	{
		Final_Event_Time[4*i]=0 ;
		Final_Event_Time[4*i+1]=0 ;
		Final_Event_Time[4*i+2]=0 ;
		Final_Event_Time[4*i+3]=0 ;
	}
	
	//---------------------------------
	SHOWER *shower ;
	shower=new SHOWER() ;
	shower->Read_Detector_Cord() ;
	DETECTOR *det ;
	det=new DETECTOR() ;
	det->Read_Cord("../data/Detector_Cord.dat",NO_DETs) ;
	for(int i=0;i<NO_DETs;i++)
	{
		x_cor[i]=det->Get_XCord(i) ;
		y_cor[i]=det->Get_YCord(i) ;
		z_cor[i]=det->Get_ZCord(i) ;
		printf("(%2d)\t%5.1f\t%5.1f\t%5.1f\n",i,x_cor[i],y_cor[i],z_cor[i]) ;
	}

	TH1F *h_theta,*h_phi,*h1_lateral,*h_temp,*h_dt,*h_hits ;
	TH2F *h_core,*h_theta_phi,*h2_lateral ;
	TGraphPolar *g_theta_phi ;
	h_core = new TH2F("h_core","Core distribution",(int)(((det->Get_Xmax()+10)-(det->Get_Xmin()-10))/Bin_Size_X),det->Get_Xmin()-10.0,det->Get_Xmax()+10.0,(int)(((det->Get_Ymax()+10)-(det->Get_Ymin()-10))/Bin_Size_Y),det->Get_Ymin()-10.0,det->Get_Ymax()+10.0) ;
	h_theta = new TH1F("h_theta","#theta distribution",Theta_No_Bin,0,90) ;	
	h_phi = new TH1F("h_phi","#phi distribution",Phi_No_Bin,0,360) ;	
	h_theta_phi = new TH2F("h_theta_phi","2D Arrival Direction",Theta_No_Bin,0,90,Phi_No_Bin,0,360) ;	
	h2_lateral = new TH2F("h2_lateral","2D lateral distribution",Lateral_No_Bin,0,300,H_Landau_No_Bin,H_Landau_Min,H_Landau_Max) ;	
	h1_lateral = new TH1F("h1_lateral","1D lateral distribution",Lateral_No_Bin,0,300) ;	
	h_temp = new TH1F("","",H_Landau_No_Bin,H_Landau_Min,H_Landau_Max) ;	//temp. histo. use for finding lateral distribution	
	h_dt = new TH1F("h_dt","Time difference",dt_No_Bin,dt_Min,dt_Max) ;	
	h_hits = new TH1F("h_hits","No. of hits",Hits_No_Bin,Hits_Min,Hits_Max) ;	

	EVENT_TIME *event_time[NO_LASAs] ;
	//---------------------------------
	char detector[50],lasa[50],fitting[20],trace[20] ; 
	ANALYSIS *analysis[NO_DETs] ;
	TObjArray output_traces[NO_DETs] ;	
	TObjArray output_core ;	
	TObjArray output_direction ;
	ARRIVAL_DIRECTION *arrival_direction ;
	arrival_direction=new ARRIVAL_DIRECTION ;
	arrival_direction->Read_Detector_Cord() ;
	double theta[N_Max],phi[N_Max] ;
	int n=0 ;
	int flag_traces=0 ;
	
	FILE *gp ;
	gp=fopen(argv[1],"r") ;
	char content[200],path[150],filename[50],input[200] ;

	for(int i=0;i<NO_DETs;i++)
	{
		analysis[i]=new ANALYSIS ;
		strcpy(detector,"") ;
		sprintf(detector,"Det%d",i+1) ;
		analysis[i]->Create_Histograms(detector) ;
		strcpy(detector,"") ;
	}
	while(fgets(content,200,gp)!=NULL)	//Main loop running over the data files
	{
		if(strncmp(content,"//",strlen("//"))==0) continue ;
		std::istrstream M(content) ;
		M>>path>>filename ;
		strcpy(input,path) ;
		strcat(input,filename) ;
		printf("\n%s\n\n",input) ;
		
	TFile *f=new TFile(input) ;
	TTree *event=(TTree*)f->Get("Tree_event") ;
	TTree *sec=(TTree*)f->Get("Tree_sec") ;
	unsigned int time0=0 ;				//Represents the start time of the run (1st one sec GPS time in secs)

	for(int i=0;i<NO_DETs;i++)
	{
		strcpy(detector,"") ;
		sprintf(detector,"Det%d",i+1) ;
		analysis[i]->Evt_Branch_Address(detector,event) ;
		strcpy(detector,"") ;
	}

	for(int i=0;i<NO_LASAs;i++)
	{
		event_time[i]=new EVENT_TIME ;
		strcpy(lasa,"") ;
		sprintf(lasa,"Lasa%d",i+1) ;
		analysis[4*i]->Sec_Branch_Address(lasa,sec) ;
		strcpy(lasa,"") ;
	}
	unsigned long no_entries_event=(unsigned long)event->GetEntries() ;
	unsigned long no_entries_sec=(unsigned long)sec->GetEntries() ;
	//----------- Reading sec data ------------------
	unsigned long m=0 ;
	for(m=0;m<=2;m++)
	{
		sec->GetEntry(m) ;
		for(int j=0;j<NO_LASAs;j++)
		{
			analysis[4*j]->Read_Sec_Data(m) ;
			if(time0==0 && (analysis[4*j]->Get_Sec_YMD1())!=0)
			{
			time0=analysis[4*j]->Get_Sec_GPS_Time1() ;	//Setting staring time for the run
			printf("time0=%u\n",time0) ;
			}
		}
	}
	//-----------------------------------------------
	int flag=1 ;		//Analysis processing flag
	int flag_dt=0 ;		//flag use in calulating dt
	int flag_dRun=0 ;	//flag use in calulating dRun
	for(unsigned long i=0;i<no_entries_event;i++)	//Loop running inside individual data file
	{
		if(m==no_entries_sec) break ;		//coming out of the main loop (end reading the files)
		event->GetEntry(i) ;
		flag=1 ;
		for(int j=0;j<NO_DETs;j++)
		{
			if(analysis[0]->Get_Evt_GPS_Time()<analysis[0]->Get_Sec_GPS_Time1())
			{
				flag=0 ;
				break ;		//Checking events arriving before the one sec data.
			}
			if(analysis[0]->Get_Evt_GPS_Time()>analysis[0]->Get_Sec_GPS_Time1())
			{	m++ ;	
				sec->GetEntry(m) ;
				for(int n=0;n<NO_LASAs;n++) analysis[4*n]->Rearrange_Sec_Data() ;
				i-- ;
				flag=0 ;
				break ;		//Coming out of the detector loop
			}
			if(flag_dRun==0 && analysis[j]->Get_Evt_GPS_Time()>0)	//Run time calculation
			{
				Run0=analysis[j]->Get_Evt_GPS_Time() ;
				flag_dRun=1 ;
			}
			
			strcpy(detector,"") ;
			sprintf(detector,"Det%d",j+1) ;
			analysis[j]->Reset_BG() ;
			analysis[j]->Process_Traces(detector) ;
			total_count[j]=analysis[j]->Get_Signal_Count() ;
			Event_Arr_Time[j]=analysis[j]->Get_Arrival_Time(Det_Thres) ;//(Get event arrival time from the traces)
			//-------------- Get event time stamp (nsecs) for each LASA -----------
			if(j==0)
			{
				printf("[%lu]Approx=%u\tExact=%lu\n",i,analysis[0]->Get_nsec(),analysis[0]->Get_EventTime_Stamp()) ;
			}
			//-------------------Plotting upto NO_TRACES----------------
			if(flag_traces==0)
			{
				printf("[Ev=%lu]\t[Det=%d]\tT=%lu\n",i+1,j+1,Event_Arr_Time[j]) ;
				strcpy(detector,"") ;
				strcpy(trace,"") ;
				sprintf(detector,"Det%d",j+1) ;
				analysis[j]->Draw_Traces(detector) ;
				output_traces[j].Add(analysis[j]->Get_Traces1()) ;
				output_traces[j].Add(analysis[j]->Get_Traces2()) ;
				strcpy(detector,"") ;
				if(i==NO_TRACES-1) flag_traces=1 ;
			}
			//-------------------------xxx------------------------------
			delete gDirectory->FindObject(detector) ;	
			strcpy(detector,"") ;
		}
		if(flag==1)
		{
			for(int k=0;k<NO_LASAs;k++)
			{
				event_time[k]->Sort_Increase_Order(&Event_Arr_Time[4*k]) ;
				event_time[k]->Cal_Event_Time(analysis[4*k]->Get_Trigg_Condition(),analysis[4*k]->Get_EventTime_Stamp()) ;

				if(event_time[k]->Get_Event_Time(0)!=0)
				Final_Event_Time[4*k]=(long long unsigned)((analysis[k]->Get_Evt_GPS_Time()-time0))*1000000000*10+(event_time[k]->Get_Event_Time(0)) ;
				if(event_time[k]->Get_Event_Time(1)!=0)
				Final_Event_Time[4*k+1]=(long long unsigned)((analysis[k]->Get_Evt_GPS_Time()-time0))*1000000000*10+(event_time[k]->Get_Event_Time(1)) ;
				if(event_time[k]->Get_Event_Time(2)!=0)
				Final_Event_Time[4*k+2]=(long long unsigned)((analysis[k]->Get_Evt_GPS_Time()-time0))*1000000000*10+(event_time[k]->Get_Event_Time(2)) ;
				if(event_time[k]->Get_Event_Time(3)!=0)
				Final_Event_Time[4*k+3]=(long long unsigned)((analysis[k]->Get_Evt_GPS_Time()-time0))*1000000000*10+(event_time[k]->Get_Event_Time(3)) ;
			}
			arrival_direction->Do_Arrival_Direction(&Final_Event_Time[0],&total_count[0]) ;
			arrival_direction->Do_Best_Fit() ;
			if(arrival_direction->Get_Theta()>=0 && arrival_direction->Get_Theta()<=90 && arrival_direction->Get_Phi()>=0 && arrival_direction->Get_Phi()<=360 && n<N_Max)
			{
				theta[n]=(double)arrival_direction->Get_Theta() ;	//in degrees
				phi[n]=((double)arrival_direction->Get_Phi())*pi/180 ;	//in radians
				n++ ;
			}
			h_theta->Fill(arrival_direction->Get_Theta()) ;
			h_phi->Fill(arrival_direction->Get_Phi()) ;
			h_theta_phi->Fill(arrival_direction->Get_Theta(),arrival_direction->Get_Phi()) ;

			shower->Find_Core(&Final_Event_Time[0],&total_count[0]) ;
			printf("x_core=%f\ty_core=%f\n",shower->Get_XCore(),shower->Get_YCore()) ;
			h_core->Fill(shower->Get_XCore(),shower->Get_YCore()) ;
			
			for(int k=0;k<NO_DETs;k++)
			{
				if(arrival_direction->Get_cdt(k)>=0)
				{
					R_Core2=pow(shower->Get_XCore()-x_cor[k],2)+pow(shower->Get_YCore()-y_cor[k],2) ;
					h2_lateral->Fill(sqrt(R_Core2)*cos(arrival_direction->Get_Theta()*pi/180),total_count[k]) ;
					h_hits->Fill(k+1) ;
				}
			}
			if(flag_dt==0 && arrival_direction->Accept_Event()==1)
			{
				t0=arrival_direction->Get_T0() ;
				flag_dt=1 ;
			}
			else
			{
				if(arrival_direction->Accept_Event()==1)
				{
					dt=(float)(arrival_direction->Get_T0()-t0)/10.0/pow(10,6) ;	//in millisecs
					h_dt->Fill(dt) ;
					t0=arrival_direction->Get_T0() ;
				}
			}
			for(int k=0;k<NO_LASAs;k++)
			{
				Final_Event_Time[4*k]=0 ;
				Final_Event_Time[4*k+1]=0 ;
				Final_Event_Time[4*k+2]=0 ;
				Final_Event_Time[4*k+3]=0 ;
			}
			printf("--------------------xxx-----------------\n") ;
		}
	}//end reading 1 input data file
	f->Close() ;
	for(int j=0;j<NO_DETs;j++)	//Run time calculation
	{
		if(analysis[j]->Get_Evt_GPS_Time()>0)
		{
			Run_Time=Run_Time+(float)(analysis[j]->Get_Evt_GPS_Time()-Run0) ;
			break ;
		}
	}
	}//end reading all input data files
	fclose(gp) ;

	Create_ROOT_File() ;
	if(n<N_Max) g_theta_phi = new TGraphPolar(n,phi,theta) ;
	else g_theta_phi = new TGraphPolar(N_Max,phi,theta) ;
	g_theta_phi->SetTitle("#theta - #phi polar plot") ;
	g_theta_phi->SetMarkerColor(4); 
	g_theta_phi->SetMarkerStyle(20); 
	g_theta_phi->SetMarkerSize(0.5);
	g_theta_phi->Draw("ALP") ;
	output_direction.Add(g_theta_phi) ;

	TF1 *fit1 ;
	fit1 = new TF1("fit1",fit_theta,0,90,2) ;
	fit1->SetParameter(0,h_theta->GetMaximum()) ;	//Setting starting values
	fit1->SetParameter(1,8) ;
	h_theta->Draw("HIST") ;				//Drawing theta distribution
	//h_theta->SetStats(kFALSE) ;
	h_theta->Fit("fit1","V+") ;			//Fitting theta distribution		
	//gStyle->SetOptStat(0) ;
	fit1->Draw("SAME") ;
	gStyle->SetOptFit(1111) ;
	h_theta->SetXTitle("#theta (deg.)") ;
	h_theta->GetXaxis()->CenterTitle() ;
	h_theta->SetYTitle("dN/d#theta") ;
	h_theta->GetYaxis()->CenterTitle() ;
	output_direction.Add(h_theta) ;

	h_phi->SetXTitle("#phi (deg.)") ;
	h_phi->GetXaxis()->CenterTitle() ;
	h_phi->SetYTitle("dN/d#phi") ;
	h_phi->GetYaxis()->CenterTitle() ;
	h_phi->Draw("HIST") ;			//Drawing phi distribution
	output_direction.Add(h_phi) ;
	
	h_theta_phi->SetXTitle("#theta (deg.)") ;
	h_theta_phi->GetXaxis()->CenterTitle() ;
	h_theta_phi->SetYTitle("#phi (deg.)") ;
	h_theta_phi->GetYaxis()->CenterTitle() ;
	h_theta_phi->Draw("LEGO2Z") ;		//Drawing theta-phi distribution
	output_direction.Add(h_theta_phi) ;

	h_dt->SetXTitle("Time difference #Delta (millisecs)") ;
	h_dt->GetXaxis()->CenterTitle() ;
	h_dt->SetYTitle("dN/d#Delta") ;
	h_dt->GetYaxis()->CenterTitle() ;
	h_dt->Draw("HIST") ;			//Drawing phi distribution
	output_direction.Add(h_dt) ;

	h_hits->SetNdivisions(NO_DETs+2,"X") ;
	h_hits->SetXTitle("Detector no.") ;
	h_hits->GetXaxis()->CenterTitle() ;
	h_hits->SetFillColor(kRed) ;
	h_hits->SetStats(kFALSE) ;
	h_hits->SetBarWidth(0.3) ;
	h_hits->Draw("bar4") ;			//Drawing phi distribution
	output_direction.Add(h_hits) ;

	for(int i=1;i<=Lateral_No_Bin;i++)
	{
		for(int j=1;j<=H_Landau_No_Bin;j++)
			h_temp->SetBinContent(j,h2_lateral->GetBinContent(i,j)) ;
		h1_lateral->SetBinContent(i,h_temp->GetMean()/Muon_Count/Det_Area) ;
	}
	h1_lateral->SetXTitle("R (m)") ;
	h1_lateral->GetXaxis()->CenterTitle() ;
	h1_lateral->SetYTitle("Particle density (m^{-2})") ;
	h1_lateral->GetYaxis()->CenterTitle() ;
	h1_lateral->SetStats(kFALSE) ;
	h1_lateral->Draw("HIST") ;		//Drawing 1D lateral distribution
	output_direction.Add(h1_lateral) ;
	h2_lateral->SetXTitle("R (m)") ;
	h2_lateral->GetXaxis()->CenterTitle() ;
	h2_lateral->SetYTitle("Total counts") ;
	h2_lateral->GetYaxis()->CenterTitle() ;
	h2_lateral->Draw("LEGO2Z") ;		//Drawing 2D lateral distribution
	output_direction.Add(h2_lateral) ;
	output_direction.Write() ;

	h_core->SetXTitle("X (meters)") ;
	h_core->GetXaxis()->CenterTitle() ;
	h_core->SetYTitle("Y (meters)") ;
	h_core->GetYaxis()->CenterTitle() ;
	h_core->Draw("LEGO2Z") ;		//Drawing core distribution
	output_core.Add(h_core) ;
	output_core.Write() ;
	
	for(int i=0;i<NO_DETs;i++)
	{
		Create_ROOT_SubDirectories1(i+1) ;
		strcpy(fitting,"") ;
		sprintf(fitting,"Fit%d",i+1) ;
		analysis[i]->Draw_Histograms(fitting) ;
		output_traces[i].Add(analysis[i]->Get_Total_Count()) ;
		output_traces[i].Add(analysis[i]->Get_Pulse_Height()) ;
		output_traces[i].Add(analysis[i]->Get_Pulse_Width()) ;
		
		Dir_Tel[i]->cd() ;
		output_traces[i].Write() ;
		
		strcpy(fitting,"") ;
		strcpy(detector,"") ;
	}
	printf("\n\n\t Total Run Time=%e secs\t=%f hrs\t=%f days\n\n",Run_Time,Run_Time/3600.0,Run_Time/3600.0/24.0) ;
}

