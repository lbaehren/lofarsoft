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
int lasa_select[NO_LASAs] ;
int bytes_read ;
int sockfd_Max ;
TFile *f ;
TTree *tree_event,*tree_log,*tree_sec ;
float X_Cor[NO_LASAs*4],Y_Cor[NO_LASAs*4],Z_Cor[NO_LASAs*4] ;	//X & Y coordinates of the detectors
unsigned short counter_Monitor2,counter_Event_Size,counter_Monitor_rate ;
long long unsigned time1,time2 ;		//To calculate event rate
double event_rate ;				//Event rate
int Calib_Master_Done[NO_LASAs] ;		//Calibration flag for Master
int Calib_Slave_Done[NO_LASAs] ;		//Calibration flag for Slave
int Calib_Done=0 ;				//Total calibration flag
TH2F* h2 ;					//2D distribution for arrival direction
