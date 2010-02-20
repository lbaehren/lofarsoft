#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <TH1.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TFrame.h>
#include <TStyle.h>
#include <TPostScript.h>

#define	H_count_Min	0
#define	H_count_Max	10000
#define	H_count_No_Bin	4000

#define	INTE_LOW_LIMIT	1	//700
#define	INTE_UPP_LIMIT	4000	//1000

#define H_Landau_Chan_1_Min	0
#define H_Landau_Chan_1_Max	25000
#define H_Landau_Chan_1_No_Bin	50
 
#define H_Landau_Chan_2_Min	0
#define H_Landau_Chan_2_Max	10000
#define H_Landau_Chan_2_No_Bin	50

int main(int argc, char** argv)
{
	TCanvas *c1 ;
	c1 = new TCanvas("c","G",10,40,800,600) ;
	c1->GetFrame()->SetFillColor(10) ;
	c1->GetFrame()->SetBorderSize(2) ;
	c1->SetBorderMode(1) ;
	c1->SetFillColor(10) ;

	TH1F *H_count_Chan_1, *H_count_Chan_2, *H_Landau_Chan_1, *H_Landau_Chan_2 ;
	H_count_Chan_1 = new TH1F("Channel 1","Count distribution",H_count_No_Bin,H_count_Min,H_count_Max) ;
	H_count_Chan_2 = new TH1F("Channel 2","Count distribution",H_count_No_Bin,H_count_Min,H_count_Max) ;
	H_Landau_Chan_1 = new TH1F("Channel 1","Landau distribution",H_Landau_Chan_1_No_Bin,H_Landau_Chan_1_Min,H_Landau_Chan_1_Max) ;
	H_Landau_Chan_2 = new TH1F("Channel 2","Landau distribution",H_Landau_Chan_2_No_Bin,H_Landau_Chan_1_Min,H_Landau_Chan_2_Max) ;

	int k=1 ;
	unsigned short a1,b1,a2,b2,a3,b3,data1,data2 ;	//For channel 1
	unsigned short A1,B1,A2,B2,A3,B3,DATA1,DATA2 ;	//For channel 2

	unsigned char data,Header,Identifier ;
	struct ONE_SEC_MESSAGE
	{
		unsigned char GPS_Time_Stamp[7] ;
		unsigned char CTP[4] ;
		unsigned char Quan_Error[4] ;
		unsigned char Thres_Counters[8] ;
		unsigned char Satellite[61] ;
		unsigned char End ;
	} One_Sec_Message ;

	struct DATA_MESSAGE
	{
		unsigned char Trigg_Condition ;
		unsigned char Trigg_Pattern[2] ;
		unsigned char Pre_Coin_Time[2] ;
		unsigned char Coin_Time[2] ;
		unsigned char Post_Coin_Time[2] ;
		unsigned char GPS_Time_Stamp[7] ;
		unsigned char CTP[4] ;
		unsigned char Data[12000] ;
		unsigned char End ;
	} Data_Message ;

	int inhandle ;
	inhandle=open(argv[1],O_RDONLY) ;
	int bytes ;
	while(1)
	{
	bytes=read(inhandle,&data,1) ;
	printf("No. of bytes read=%d\n",bytes) ;
	if(bytes<=0) break ;

	if(data==0x99)
	{
		printf("\tHeader detected\n") ;
		//read(inhandle,&Header,1) ;
		read(inhandle,&Identifier,1) ;

		switch(Identifier)
		{
		case 0xA4:	//1 second message
			bytes=read(inhandle,&One_Sec_Message,85) ;

			printf("1 sec message: No. of bytes read=%d\n",bytes) ;
			break ;

		case 0xA0:	//Data message
			bytes=read(inhandle,&Data_Message,12021) ;
		
			printf("Event data: No. of bytes read=%d\n",bytes) ;
			for(int j=0;j<=5998;j+=3)
			{
				a1=(Data_Message.Data[j]>>4)&0xf ;
				b1=Data_Message.Data[j]&0xf ;
				a2=(Data_Message.Data[j+1]>>4)&0xf ;
				data1=(a1<<8)+(b1<<4)+a2 ;
		
				b2=Data_Message.Data[j+1]&0xf ;
				a3=(Data_Message.Data[j+2]>>4)&0xf ;
				b3=Data_Message.Data[j+2]&0xf ;
				data2=(b2<<8)+(a3<<4)+b3 ;
		
				A1=(Data_Message.Data[j+6000]>>4)&0xf ;
				B1=Data_Message.Data[j+6000]&0xf ;
				A2=(Data_Message.Data[j+6000+1]>>4)&0xf ;
				DATA1=(A1<<8)+(B1<<4)+A2 ;
		
				B2=Data_Message.Data[j+6000+1]&0xf ;
				A3=(Data_Message.Data[j+6000+2]>>4)&0xf ;
				B3=Data_Message.Data[j+6000+2]&0xf ;
				DATA2=(B2<<8)+(A3<<4)+B3 ;

				H_count_Chan_1->SetBinContent(k,data1) ;
				H_count_Chan_2->SetBinContent(k,DATA1) ;
				k++ ;
				H_count_Chan_1->SetBinContent(k,data2) ;
				H_count_Chan_2->SetBinContent(k,DATA2) ;
				k++ ;
			}
			H_Landau_Chan_1->Fill(H_count_Chan_1->Integral(INTE_LOW_LIMIT,INTE_UPP_LIMIT)) ;
			H_Landau_Chan_2->Fill(H_count_Chan_2->Integral(INTE_LOW_LIMIT,INTE_UPP_LIMIT)) ;
			
			k=1 ;
			break ;	

		default:
			break ;
		}
	}
	}
	close(inhandle) ;

	TF1 *fit1,*fit2 ;
	fit1=new TF1("fit1","landau",H_Landau_Chan_1_Min,H_Landau_Chan_1_Max) ;
	TPostScript *ps1,*ps2 ;
	gStyle->SetOptFit(1111) ;
	H_Landau_Chan_1->Draw("HIST") ;
	H_Landau_Chan_1->Fit("fit1","V+") ;
	//H_Landau_Chan_1->Fit("fit1","N") ;
	H_Landau_Chan_1->SetYTitle("f(c)") ;
	H_Landau_Chan_1->SetXTitle("Total count c") ;	
	//gStyle->SetOptFit(1111) ;
	//gStyle->SetOptStat(0) ;	
	ps1=new TPostScript("Channel_1_distribution.ps",112) ;
	ps1->Close() ;
	printf("\nChannel 1: Constant=%f\tMPV=%f\tSigma=%f\n",fit1->GetParameter(0),fit1->GetParameter(1),fit1->GetParameter(2)) ;

	fit2=new TF1("fit2","landau",H_Landau_Chan_2_Min,H_Landau_Chan_2_Max) ;
	H_Landau_Chan_2->Draw("HIST") ;
	H_Landau_Chan_2->Fit("fit2","V+") ;
	//gStyle->SetOptFit(1111) ;
	//gStyle->SetOptStat(0) ;
	H_Landau_Chan_2->SetYTitle("f(c)") ;
	H_Landau_Chan_2->SetXTitle("Total count c") ;
	ps2=new TPostScript("Channel_2_distribution.ps",112) ;
	ps2->Close() ;	
	printf("\nChannel 2: Constant=%lf\tMPV=%lf\tSigma=%lf\n",fit2->GetParameter(0),fit2->GetParameter(1),fit2->GetParameter(2)) ;
}

