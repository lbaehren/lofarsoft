#include "analysis.h"

double background(double *x,double *par)	//return the background level
{
	return par[0] ;
}

void ANALYSIS:: Create_Histograms(char *name)
{
	//printf("%s\n",name) ;
	h_TC = new TH1F(name,"Total Counts",H_Landau_No_Bin,H_Landau_Min,H_Landau_Max) ;
	h_BG = new TH1F(name,"BG",20,0,100) ;
	h_PH = new TH1F(name,"Pulse height",H_PH_No_Bin,H_PH_Min,H_PH_Max) ;
	h_PW = new TH1F(name,"Pulse width",H_PW_No_Bin,H_PW_Min,H_PW_Max) ;
}
		
void ANALYSIS:: Process_Traces(char *name)
{
	h_Tr = new TH1F(name,"Traces-BG",4000,0,10000) ;
	h_Tr->SetXTitle("Time (nsecs)") ;	
	h_Tr->GetXaxis()->CenterTitle() ;	
	h_Tr->SetYTitle("ADC counts") ;	
	h_Tr->GetYaxis()->CenterTitle() ;
	for(int k=1;k<=4000;k++)
	{
		if(k<=BG_No_Bin) h_BG->Fill(event_data.counts[k-1]) ;
		h_Tr->SetBinContent(k,event_data.counts[k-1]) ;
	}
	BG_mean=h_BG->GetMean() ;
	BG = new TF1("BG",background,0,10000,1) ;
	BG->SetParameter(0,BG_mean) ;
	h_Tr->Add(BG,-1) ;	//Substracting background
	h_PH->Fill(event_data.Pulse_height) ;
	h_PW->Fill(event_data.Pulse_width) ;
	BIN_S=(h_Tr->GetMaximumBin()-(int)(Window_Open/2.5)) ;
	BIN_E=(h_Tr->GetMaximumBin()+(int)(Window_Close/2.5)) ;
	h_TC->Fill(h_Tr->Integral(BIN_S,BIN_E)) ;
	total_count=(float)(h_Tr->Integral(BIN_S,BIN_E)) ;
}
		
void ANALYSIS:: Draw_Traces(char *name)
{
	h3 = new TH1F(name,"Traces+BG",4000,0,10000) ;
	for(int k=1;k<=4000;k++) h3->SetBinContent(k,event_data.counts[k-1]) ;
	h3->SetXTitle("Time (nsecs)") ;	
	h3->GetXaxis()->CenterTitle() ;	
	h3->SetYTitle("ADC counts") ;	
	h3->GetYaxis()->CenterTitle() ;	
	h3->Draw("C") ;
	h_Tr->Draw("C") ;
}
		
TH1F* ANALYSIS:: Get_Traces1()
{
	return h3 ;
}
		
TH1F* ANALYSIS:: Get_Traces2()
{
	return h_Tr ;
}
		
void ANALYSIS:: Draw_Histograms(char *name)
{
	fit=new TF1(name,"landau",H_Landau_Min,H_Landau_Max) ;
	gStyle->SetOptFit(1111) ;
	h_TC->Draw("HIST") ;
	h_TC->Fit(name,"Q") ;
	h_TC->SetXTitle("Total ADC count") ;	
	h_TC->GetXaxis()->CenterTitle() ;	
	//gStyle->SetOptFit(1111) ;
	//gStyle->SetOptStat(0) ;
	h_PH->Draw("HIST") ;	
	h_PH->SetXTitle("Pulse height (ADC count)") ;	
	h_PH->GetXaxis()->CenterTitle() ;	
	h_PW->Draw("HIST") ;	
	h_PW->SetXTitle("Pulse width (nsec)") ;
	h_PW->GetXaxis()->CenterTitle() ;	
}
		
TH1F* ANALYSIS:: Get_Total_Count()
{
	return h_TC ;
}

TH1F* ANALYSIS:: Get_Pulse_Height()
{
	return h_PH ;
}

TH1F* ANALYSIS:: Get_Pulse_Width()
{
	return h_PW ;
}

float ANALYSIS:: Get_Signal_Count()
{
	return total_count ;
}

unsigned int ANALYSIS:: Get_Trigg_Condition()
{
	return event_data.Trigg_condition ;
} 

void ANALYSIS:: Get_Fit_Parameters(float *p0,float *p1,float *p2)
{
	*p0=fit->GetParameter(0) ;
	*p1=fit->GetParameter(1) ;
	*p2=fit->GetParameter(2) ;
}
		
void ANALYSIS:: Reset_BG()
{
	h_BG->Reset() ;
}
		
void ANALYSIS:: Evt_Branch_Address(char *name,TTree *event)
{
	event->SetBranchAddress(name,&event_data) ;
}
		
void ANALYSIS:: Sec_Branch_Address(char *name,TTree *sec)
{
	sec->SetBranchAddress(name,&sec_data) ;
}
		
void ANALYSIS:: Read_Sec_Data(unsigned long n)
{
	time[n].YMD=sec_data.YMD ;
	time[n].GPS_time_stamp=sec_data.GPS_time_stamp ;
	time[n].sync=sec_data.sync ;
	time[n].CTP=sec_data.CTP ;
	time[n].quant=sec_data.quant ;
}

unsigned int ANALYSIS:: Get_Sec_YMD1()
{
	return time[0].YMD ;
}
		
unsigned int ANALYSIS:: Get_Evt_GPS_Time()
{
	return event_data.GPS_time_stamp ;
}

unsigned int ANALYSIS:: Get_Sec_GPS_Time1()
{
	return time[0].GPS_time_stamp ;
}

unsigned int ANALYSIS:: Get_Sec_GPS_Time2()
{
	return time[1].GPS_time_stamp ;
}

void ANALYSIS:: Rearrange_Sec_Data()
{
	time[0].GPS_time_stamp=time[1].GPS_time_stamp ;
	time[0].sync=time[1].sync ;
	time[0].CTP=time[1].CTP ;
	time[0].quant=time[1].quant ;

	time[1].GPS_time_stamp=time[2].GPS_time_stamp ;
	time[1].sync=time[2].sync ;
	time[1].CTP=time[2].CTP ;
	time[1].quant=time[2].quant ;

	Read_Sec_Data(2) ;
}
		
unsigned int ANALYSIS:: Get_nsec()
{
	return event_data.nsec ;
}
		
unsigned long ANALYSIS:: Get_EventTime_Stamp()
{
	evt_time_stamp=(unsigned long)(time[0].sync+time[1].quant+(1.0*event_data.CTD/time[0].CTP)*(1000000000.0-time[1].quant+time[2].quant)) ;
	return evt_time_stamp ;
}

unsigned long ANALYSIS:: Get_Arrival_Time(float thres)
{
	/*int bin ;
	bin=h_Tr->FindFirstBinAbove((double)thres,1) ;
	return bin*2.5 ;*/
	int i ;
	for(i=1;i<=4000;i++)
	{
		if(h_Tr->GetBinContent(i)>=thres) return (unsigned long)(i*2.5*10) ;	//returns in units of 0.1ns
	}
	return 0 ;
}

