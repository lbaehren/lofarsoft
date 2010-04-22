#include <TH1.h>
#include <TF1.h>
#include <fcntl.h>
#include "/usr/include/sys/types.h"
#include "/usr/include/sys/stat.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "FUNCTION_Sockets.h"

double Background(double *x,double *par)
{
	return par[0] ;
}

class DEVICE
{
	private:
		char message[650] ;
		char keyword[50] ;
		char value1[50] ;
		char value2[50] ;
		char value3[50] ;
		char value4[50] ;
		char value5[50] ;
		char value6[50] ;
		char value7[50] ;
		char value8[50] ;
		char value9[50] ;
		char value10[50] ;
		char defn[100] ;
		char value[50] ;
		struct SEND_MESSAGES
		{
			char SYS_NAME[50] ;
			char UNIT_STATE[50] ;
			char UNIT_SERIAL_NO[50] ;
			unsigned short PORT_NO ;
			unsigned short Control_Parameters[100] ;
		} messages ;
		unsigned short VALUE ;
		unsigned char Control_Messages[200] ;

		char* Device_Name ;
		int sockfd,newsockfd ;
		SOCKET_CALLS *Socket ;

		struct EVENT_TIME
		{
			short unsigned int sec ;
			short unsigned int min ;
			short unsigned int hour ;
			short unsigned int day ;
			short unsigned int month ;
			short unsigned int year ;
			unsigned int CTD ;
			unsigned int nsec ;
			long long unsigned time_stamp ;	//in nanosecs.
		} ;

		struct HANDLE_MESSAGES
		{
			unsigned char Header,Identifier ;
			unsigned char One_Sec_Message[NO_One_Sec][One_Sec_Bytes] ;
			unsigned char Event_Message[NO_Event][Event_Bytes] ;	//For holding up to "NO_Event" events	
			EVENT_TIME event_time[NO_Event] ;
			unsigned char Control_Parameters[Control_Bytes] ;
			int pointer_sec_message ;
			int pointer_data_message ;
			int pointer_control_parameters ;
			int sec_no ;		//One sec message serial no.
			int event_no ;		//Event serial no.
			short int waiting ;	//(1: sec message) (2: event data) 
						//(3: control parameters)
		} ;
		HANDLE_MESSAGES handle_messages ;

		struct ONE_SEC_TIME
		{
			short unsigned int sec ;
			short unsigned int min ;
			short unsigned int hour ;
			short unsigned int day ;
			short unsigned int month ;
			short unsigned int year ;
			short unsigned int sync ;
			unsigned int CTP ;
			float quant ;
		} one_sec_time[NO_One_Sec] ;
		unsigned int current_CTP ;

		struct CONTROL_PARAMETERS	//Structure of control parameters (returned by the Hisparc units) + headers to be stored in ROOT file
		{
			unsigned int Detector ;	//Detector no
			unsigned int YMD ;	//YearMonthDay (E.g: 20090325 for 25/03/2009)
			unsigned int HMS ;	//HourMinSec (E.g: 105415 for 10hrs:54mins:15secs)
			unsigned int Time_stamp ;	//Time stamp in secs since the Epoch (00:00:00 UTC, Jan 1 1970)
			unsigned short Channel_offset_pos ;
			unsigned short Channel_offset_neg ;
			unsigned short Channel_gain_pos ;
			unsigned short Channel_gain_neg ;
			unsigned short Common_offset_adj ;
			unsigned short Full_scale_adj ;
			unsigned short Channel_inte_time ;
			unsigned short Comp_thres_low ;
			unsigned short Comp_thres_high ;
			unsigned short Channel_HV ;
			unsigned short Channel_thres_low ;
			unsigned short Channel_thres_high ;
			unsigned short Trigg_condition ;
			unsigned short Pre_coin_time ;	//in ns
			unsigned short Coin_time ;	//in ns
			unsigned short Post_coin_time ;	//in ns
		} control_parameters[2] ;

		TH1F *h_BG_1,*h_BG_2 ;		//Histograms for the background traces
		TF1 *BG_1,*BG_2 ;		//Fitted function to the background
		float BG_mean_1,BG_mean_2 ;
		int Bin_Start,Bin_End ;
		TH1F *Trace_1,*Trace_2 ;	//Histograms representing the traces in detectors 1&2
		TH1F *Landau_1,*Landau_2 ;	//Histograms for landau distribution in ,,	,,
		TF1 *Fit_1,*Fit_2 ;		//Fitted function ,,	,,	,,	,,	,,
		unsigned short monitoring_1,monitoring_2 ;	//Counters for monitoring ,,	,,
		unsigned short counter_traces_1,counter_traces_2 ;//Counters for monitoring traces
	
	public:
		void Open_Socket() ;
		int Accept_Socket() ;
		void Read_Message_File(char*,int) ;
		void Build_Hisparc_Messages() ;
		void Send_Messages() ;
		void Initialise_Variables() ;
		void Get_Sec_Time(int,int,int) ;
		void Store_Sec_Messages(int) ;
		void Store_Control_Parameters(int,int) ;
		void Get_Event_Time(int) ;
		int Data_Processing(unsigned char*,int,int,int flag) ;
		void Build_Final_Events(int,int,int) ;
		void Close_Socket() ;

	DEVICE()
	{
		Socket= new SOCKET_CALLS ;
		h_BG_1 = new TH1F("BG 1","BG_1",20,0,100) ;
		h_BG_2 = new TH1F("BG 2","BG_2",20,0,100) ;
		Trace_1= new TH1F("Channel 1","Traces",4000,0,10000) ;
		Trace_2= new TH1F("Channel 2","Traces",4000,0,10000) ;
		Landau_1= new TH1F("Channel 1","Landau",250,0,4000) ;
		Landau_2= new TH1F("Channel 1","Landau",250,0,4000) ;
		//Fit_1= new TF1("Fit_1","landau",0,15000) ;
		//Fit_2= new TF1("Fit_2","landau",0,15000) ;
	}
} ;
