#include <TH1.h>
#include <TH2F.h>
#include <TF1.h>
#include <TObjArray.h>
#include <TF1.h>
#include <TTree.h>
#include <TFrame.h>
#include <TStyle.h>

#include "constants.h"

/*!
	\class ANALYSIS
	\ingroup LASA
*/
class ANALYSIS
{
	private:
		struct EVENT_DATA			//Event data structure stored in the ROOT file
		{
			unsigned int detector ;		//Detector number
			unsigned int YMD ;		//YearMonthDay (E.g: 20090325 for 25/03/2009)
			unsigned int GPS_time_stamp ;	//Event time stamp in secs since the Epoch (00:00:00 UTC, Jan 1 1970)
			unsigned int CTD ;	
			unsigned int nsec ;		//An apprx. value
			unsigned int Trigg_condition ;	//Trigger condition
			unsigned int Trigg_pattern ;	//Trigger pattern
			unsigned int Total_counts ;	//Total counts in a signal
			unsigned int Pulse_height ;	//Pulse height of a signal
			unsigned int Pulse_width ;	//Pulse width of a signal
			short unsigned counts[4000] ;	//Counts (arb. units)
		} event_data;

		struct ONE_SEC_STRUCTURE		//One sec message structure stored in ROOT file
		{
			unsigned int Lasa ;
			unsigned int YMD ;
			unsigned int GPS_time_stamp ;	//Time stamp in secs since the Epoch (00:00:00 UTC, Jan 1 1970)
			unsigned int sync ;
			unsigned int CTP ;
			float quant ;
			unsigned short Channel_1_Thres_count_high ;
			unsigned short Channel_1_Thres_count_low ;
			unsigned short Channel_2_Thres_count_high ;
			unsigned short Channel_2_Thres_count_low ;
			unsigned char Satellite_info[61] ;
		} sec_data ;

		struct TO_FIND_EVT_TIME		//Structure use to calculate event time (nsecs)
		{
			unsigned int YMD ;
			unsigned int GPS_time_stamp ;	//Time stamp in secs since the Epoch (00:00:00 UTC, Jan 1 1970)
			unsigned int sync ;
			unsigned int CTP ;
			float quant ;
		} time[3] ;	//Can store up to 10 one sec events
		unsigned int evt_time_stamp ;	//Event time stamp in nsecs
		
		TH1F *h_TC,*h_BG,*h_PH,*h_PW,*h_Tr,*h3 ;
		TF1 *BG,*fit ;
		float BG_mean  ;
		int BIN_S,BIN_E ;	//START and END bin nos. of the signal time window
		float total_count ;	//Total count of the signal

	public:	
		void Create_Histograms(char*) ;
		void Process_Traces(char*) ;
		void Draw_Traces(char*) ;		
		TH1F* Get_Traces1() ;		//Return "Traces+BG" distribution
		TH1F* Get_Traces2() ;		//Return "Traces-BG"	,,
		void Draw_Histograms(char*) ;	
		TH1F* Get_Total_Count() ;	//Return total count distribution	
		TH1F* Get_Pulse_Height() ;	//Return pulse height 	,,
		TH1F* Get_Pulse_Width() ;	//Return pulse width 	,,
		float Get_Signal_Count() ;	//Return total ADC count of a signal
		unsigned int Get_Trigg_Condition() ;	//Return the trigger condition
		void Get_Fit_Parameters(float*,float*,float*) ;		
		void Reset_BG() ;
		void Evt_Branch_Address(char*,TTree*) ;		
		void Sec_Branch_Address(char*,TTree*) ;
		void Read_Sec_Data(unsigned long) ;
		unsigned int Get_Sec_YMD1() ;		//Return 1st one sec YMD		
		unsigned int Get_Evt_GPS_Time() ;	//Return event GPS time stamp (secs)		
		unsigned int Get_Sec_GPS_Time1() ;	//Return 1st one sec GPS time stamp (secs)		
		unsigned int Get_Sec_GPS_Time2() ;	//Return 2nd one sec GPS time stamp (secs)		
		void Rearrange_Sec_Data() ;	//Rearranging the 3 one sec times		
		unsigned int Get_nsec() ;	//Return approx. event time stamp		
		unsigned long Get_EventTime_Stamp() ;	//Return exact time stamp
		unsigned long Get_Arrival_Time(float) ;	//return uncorrected event arrival time (in units of 0.1ns)
		
	ANALYSIS()
	{
		BG_mean=0 ;	//Initializing background mean value.
	}
} ;
