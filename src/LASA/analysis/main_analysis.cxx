#include <stdlib.h>
#include <string.h>
#include <TH1.h>
#include <TF1.h>
#include <TROOT.h>
#include <TObjArray.h>
#include <TF1.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TFrame.h>
#include <TStyle.h>

#define	NO_TRACES	100
#define	NO_DETs		20
#define	REF_DET		1
#define H_Landau_Min	0
#define H_Landau_Max	6000//1000//8000
#define H_Landau_No_Bin	400//100//300

#define BG_Min		0			//t_min (nsec) for background calculation
#define BG_Max		400//1500//400			//t_max (nsec) for background calculation
#define BG_No_Bin	(BG_Max-BG_Min)/2.5	//No. of bins for background calculation

#define	Window_Open	50		//START of the signal time window [: T_peak-Window_open] nsecs
#define	Window_Close	100		//END of the signal time window [: T_peak+Window_close] nsecs

#define H_PH_Min	0
#define H_PH_Max	500
#define H_PH_No_Bin	300

#define H_PW_Min	0
#define H_PW_Max	50
#define H_PW_No_Bin	20

double background(double *x,double *par)	//return the background level
{
	return par[0] ;
}

TDirectory *Dir_Tel[NO_DETs] ;
TFile *out ;
void Create_ROOT_File()			
{
	out=new TFile("output.root","recreate") ;
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
	TH1F *h_TC[NO_DETs],*h_BG[NO_DETs],*h_PH[NO_DETs],*h_PW[NO_DETs],*h_Tr[NO_DETs],*h3 ;
	TF1 *BG ;
	
	struct EVENT_DATA			//Event data structure stored in the ROOT file
	{
		unsigned int detector ;		//Detector number
		unsigned int YMD ;		//YearMonthDay (E.g: 20090325 for 25/03/2009)
		unsigned int GPS_time_stamp ;	//Event time stamp in secs since the Epoch (00:00:00 UTC, Jan 1 1970)
		unsigned int CTD ;	
		unsigned int nsec ;		//An apprx. value
		unsigned int Trigg_pattern ;	//Trigger pattern
		unsigned int Total_counts ;	//Total counts in a signal
		unsigned int Pulse_height ;	//Pulse height of a signal
		unsigned int Pulse_width ;	//Pulse width of a signal
		short unsigned counts[4000] ;	//Counts (arb. units)
	} event_data[NO_DETs];
	
	TFile *f=new TFile(argv[1]) ;
	TTree *event=(TTree*)f->Get("Tree_event") ;
	char detector[50],fitting[20],trace[20] ; 
	Create_ROOT_File() ;
	for(int i=0;i<NO_DETs;i++)
	{
		strcpy(detector,"") ;
		sprintf(detector,"Det%d",i+1) ;
		h_TC[i] = new TH1F(detector,"Total Counts",H_Landau_No_Bin,H_Landau_Min,H_Landau_Max) ;
		h_BG[i] = new TH1F(detector,"BG",20,0,100) ;
		h_PH[i] = new TH1F(detector,"Pulse height",H_PH_No_Bin,H_PH_Min,H_PH_Max) ;
		h_PW[i] = new TH1F(detector,"Pulse width",H_PW_No_Bin,H_PW_Min,H_PW_Max) ;
		event->SetBranchAddress(detector,&event_data[i]) ;
		strcpy(detector,"") ;
	}
	TObjArray output_traces[NO_DETs] ;	
	
	unsigned long no_entries=(unsigned long)event->GetEntries() ;
	float BG_mean ;
	BG_mean=0 ;			//Initializing background mean value.
	int BIN_S,BIN_E ;		//START and END bin nos. of the signal time window
	for(unsigned long i=0;i<no_entries;i++)
	{
		event->GetEntry(i) ;
		for(int j=0;j<NO_DETs;j++)
		{
			strcpy(detector,"") ;
			sprintf(detector,"Det%d",j+1) ;
			h_Tr[j] = new TH1F(detector,"Traces-BG",4000,0,10000) ;
			h_Tr[j]->SetXTitle("Time (nsecs)") ;	
			h_Tr[j]->GetXaxis()->CenterTitle() ;	
			h_Tr[j]->SetYTitle("ADC counts") ;	
			h_Tr[j]->GetYaxis()->CenterTitle() ;	
			h_BG[j]->Reset() ;
			for(int k=1;k<=4000;k++)
			{
				if(k<=BG_No_Bin) h_BG[j]->Fill(event_data[j].counts[k-1]) ;
				h_Tr[j]->SetBinContent(k,event_data[j].counts[k-1]) ;
			}
			BG_mean=h_BG[j]->GetMean() ;
			BG = new TF1("BG",background,0,10000,1) ;
			BG->SetParameter(0,BG_mean) ;
			h_Tr[j]->Add(BG,-1) ;	//Substracting background
			h_PH[j]->Fill(event_data[j].Pulse_height) ;
			h_PW[j]->Fill(event_data[j].Pulse_width) ;
			BIN_S=(h_Tr[j]->GetMaximumBin()-(int)(Window_Open/2.5)) ;
			BIN_E=(h_Tr[j]->GetMaximumBin()+(int)(Window_Close/2.5)) ;
			h_TC[j]->Fill(h_Tr[j]->Integral(BIN_S,BIN_E)) ;

			//-------------------Plotting upto NO_TRACES----------------
			if(i<NO_TRACES)
			{
				strcpy(detector,"") ;
				strcpy(trace,"") ;
				sprintf(detector,"Det%d",j+1) ;
				h3 = new TH1F(detector,"Traces+BG",4000,0,10000) ;
				for(int k=1;k<=4000;k++) h3->SetBinContent(k,event_data[j].counts[k-1]) ;
				h3->SetXTitle("Time (nsecs)") ;	
				h3->GetXaxis()->CenterTitle() ;	
				h3->SetYTitle("ADC counts") ;	
				h3->GetYaxis()->CenterTitle() ;	
				h3->Draw("C") ;
				output_traces[j].Add(h3) ;

				h_Tr[j]->Draw("C") ;
				output_traces[j].Add(h_Tr[j]) ;
				strcpy(detector,"") ;
			}
			//-------------------------xxx------------------------------
			delete gDirectory->FindObject(detector) ;	
			strcpy(detector,"") ;
		}
	}
	TF1 *fit[NO_DETs] ;
	FILE *fp ;
	fp=fopen("test_calib.dat","w") ;
	float ref_gain ;
	for(int i=0;i<NO_DETs;i++)
	{
		Create_ROOT_SubDirectories1(i+1) ;
		strcpy(fitting,"") ;
		sprintf(fitting,"Fit%d",i+1) ;
		fit[i]=new TF1(fitting,"landau",H_Landau_Min,H_Landau_Max) ;
		gStyle->SetOptFit(1111) ;
		h_TC[i]->Draw("HIST") ;
		h_TC[i]->Fit(fitting,"Q") ;
		h_TC[i]->SetXTitle("Total ADC count") ;	
		output_traces[i].Add(h_TC[i]) ;
	//gStyle->SetOptFit(1111) ;
	//gStyle->SetOptStat(0) ;
		h_PH[i]->Draw("HIST") ;	
		h_PH[i]->SetXTitle("Pulse height (ADC count)") ;	
		output_traces[i].Add(h_PH[i]) ;
		h_PW[i]->Draw("HIST") ;	
		h_PW[i]->SetXTitle("Pulse width (nsec)") ;
		output_traces[i].Add(h_PW[i]) ;
		Dir_Tel[i]->cd() ;
		output_traces[i].Write() ;
		
		if((i+1)==REF_DET) ref_gain=fit[i]->GetParameter(1) ;	
		fprintf(fp,"%d\t%f\t%f\t%f\n",i+1,fit[i]->GetParameter(1),fit[i]->GetParameter(2),fit[i]->GetParameter(1)/ref_gain) ;
		strcpy(fitting,"") ;
		strcpy(detector,"") ;
	}
	fclose(fp) ;
}

