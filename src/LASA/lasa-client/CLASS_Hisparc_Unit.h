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
#include "ftd2xx.h"
#include "FUNCTION_Sockets.h"

class HISPARC_UNIT
{
	private:
		FT_HANDLE ft_handle ;
		FT_STATUS ft_status ;
		unsigned short calibration ;
		FILE *fp ;
		unsigned char dat[200],dat_rev[100000],read_data[200] ;
		short unsigned Channel_1_Counts[4000],Channel_2_Counts[4000],Offset_done[4],Gain_done[4] ;
		short unsigned Full_Scale_done,Common_Off_done ;
		float sum_av ;		//Sum of the 4 ADCs
		unsigned char ADC_Offset[4],ADC_Gain[4],Full_Scale,Common_Off ;
		DWORD dwBytesWritten, dwBytesRead, R_Bytes, T_Bytes, EventDWord ;
		int out_handle,send_bytes ;
		char *Device_Name, *Device_Serial_No ;
		int sockfd ;
		SOCKET_CALLS *Socket ;
		int Port_Num ;

	public:
		void Open_Socket() ;
		unsigned short Get_Messages() ;
		void Open_Hisparc_Unit() ;
		void Soft_Reset() ;
		unsigned short Set_Run_Controls() ;	//Normal run control settings
		void Set_Full_Scale(unsigned char) ;	//ADC full scale adjust
		void Reset_Full_Scale() ;		//ADC full scale reset
		void Reset_Common_Off() ;		//ADC common offset reset
		void Set_Common_Off(unsigned char) ;	//Common Offset adjust
		void Set_Calib_Controls() ;		//ADC calibration settings
		void Set_Master_Writing_Mode() ;	//This also ask for 1 sec messages
		void Set_Slave_Writing_Mode() ;		//This also sets the original Master->Slave and also ask for 1 sec messages
		void Ask_Control_Parameters() ; 
		void Open_Output_Files(char*) ;
		int Send_Data_To_Socket() ;
		int Build_Calib_Event(unsigned char*,int) ;
		int Do_Calibration() ;
		void Close_Output_Files() ;
		void Close_Hisparc_Unit() ;
		void Close_Socket() ;

		HISPARC_UNIT()
		{
		}

		HISPARC_UNIT(char* Device,char* Serial_No,int PORT_NUM)
		{
			Device_Name=Device ;
			Device_Serial_No=Serial_No ;
			Socket= new SOCKET_CALLS ;
			Port_Num=PORT_NUM ;
		}
} ;
