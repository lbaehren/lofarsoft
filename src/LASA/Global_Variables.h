#include <string.h>

char* Hostname="dwingeloo" ;
int lasa_select[NO_LASAs] ;
int bytes_read ;
int sockfd_Max ;
TFile *f ;
TTree *tree_event,*tree_log,*tree_sec ;
float X_Cor[NO_LASAs*4],Y_Cor[NO_LASAs*4],Z_Cor[NO_LASAs*4] ;	//X & Y coordinates of the detectors
unsigned short counter_Monitor2,counter_Event_Size,counter_Monitor_rate ;
long long unsigned time1,time2 ;	//To calculate event rate
double event_rate ;			//Event rate
int Calib_Master_Done[NO_LASAs] ;	//Calibration flag for Master
int Calib_Slave_Done[NO_LASAs] ;	//Calibration flag for Slave
int Calib_Done=0 ;			//Total calibration flag
TH2F* h2 ;				//2D distribution for arrival direction
int Calibration ;			//[1:on and 0:off]
int Monitor2 ;				//No. of events Interval for monitoring event display
time_t timeA,timeB ;			//To display no. of trigger events/channel

struct ONE_SEC_STRUCTURE		//One sec message structure to be stored in ROOT file
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
} sec_data[NO_LASAs] ;
long unsigned counter_sec ;		//Counter used in storing one sec messages

//------------------------------------------
struct CONTROL_STRUCTURE		//Structure of control parameters (returned by the Hisparc units) + headers to be stored in the log book
{
	unsigned int Detector ;	//Detector no
	unsigned int YMD ;		//YearMonthDay (E.g: 20090325 for 25/03/2009)
	unsigned int HMS ;		//HourMinSec (E.g: 105415 for 10hrs:54mins:15secs)
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
} ;

struct LOG_BOOK
{
	CONTROL_STRUCTURE master[2] ;	//For 2 detectors
	CONTROL_STRUCTURE slave[2] ;	//For 2 detectors
} log_book[NO_LASAs] ;
short unsigned counter_log ;		//Counter used in storing control parameters

//------------------------------------------
struct DATA_STRUCTURE			//Event data structure to be stored in ROOT file
{
	unsigned int detector ;		//Detector number
	unsigned int YMD ;		//YearMonthDay (E.g: 20090325 for 25/03/2009)
	unsigned int GPS_time_stamp ;	//Event time stamp in secs since the Epoch (00:00:00 UTC, Jan 1 1970)
	unsigned int CTD ;	
	unsigned int nsec ;		//An apprx. value
	unsigned int Trigg_condition ;	//Trigger condition
	unsigned int Trigg_pattern ;	//Trigger pattern
	unsigned int Total_counts ;	//Total ADC counts of the signal
	unsigned int Pulse_height ;	//Pulse height of the signal (in ADC counts)
	unsigned int Pulse_width ;	//Pulse width of the signal (in nsec)
	short unsigned counts[4000] ;	//Traces (ADC counts)
} ;

struct FINAL_DATA
{
	DATA_STRUCTURE master[2] ;	//For 2 detectors
	DATA_STRUCTURE slave[2] ;	//For 2 detectors
	unsigned int Trigg_Status ;	//Station trigger status (1:Yes & 0:No)
} final_data[NO_LASAs] ;
//-------------------------------------------

struct EVENT_TIME
{
	unsigned int YMD ;
	unsigned int GPS_time_stamp ;
	unsigned int CTD ;
	unsigned int nsec ;
} event_time[NO_LASAs] ;

struct ARRAY_EVENT_TIMES		//Store event times together from all the LASAs
{
	short unsigned lasa ;		//LASA identifier: 0--->1 for lasa[0]--->lasa[4]
	short unsigned event_no ;	//Event no. in lasa[lasa]
	long long unsigned time_stamp ;		//Event time stamp in nanosecs
} array_event_times[Max_Event] ;
ARRAY_EVENT_TIMES last_trigger_events[NO_LASAs] ;	//Store events for the last trigger
ARRAY_EVENT_TIMES temp_array[Spy_Event+NO_LASAs] ;
int x[Spy_Event+NO_LASAs] ;

short unsigned pointer_event ;		//Pointer for array_event_times[1000]
long long unsigned counter ;		//Counter used in spying the events

struct TIME0				//Ref. time for the RUN (:-> the 1st one sec time)
{
	short unsigned int sec ;
	short unsigned int min ;
	short unsigned int hour ;
	short unsigned int day ;
	short unsigned int month ;
	short unsigned int year ;
} time0 ;

class DEVICE ;
struct LASA
{
	DEVICE *Master ;
	DEVICE *Slave ;
	int sockfd_master ;
	int sockfd_slave ;
	unsigned char data_master[50000] ;
	unsigned char data_slave[50000] ;
	int pointer_data_master ;
	int pointer_data_slave ;
	int tot_bytes_read_master ;
	int tot_bytes_read_slave ;
	int flag_master ;
	int flag_slave ;
} ;
LASA lasa[NO_LASAs] ;
