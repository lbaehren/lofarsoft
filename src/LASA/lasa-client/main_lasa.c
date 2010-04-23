#include <signal.h>
#include <time.h>
#define	On	1	//On switch
#define	Off	0	//Off switch
#define	Done	2	//Done switch
#include "Constants.h"
unsigned int cal_type ;	//Type of calibration (0:Full Scale|1:Offset|2:Common Offset(for gain)|3:Gain|4:Final Common Offset)
#include "FUNCTION_Hisparc_Unit.h"

void BYE()
{
	printf("FINISH !!\n") ;
}

int main(int argc, char **argv)
{
	unsigned short dtime ;
	unsigned short Calibration ;

	HISPARC_UNIT *Master, *Slave ;
	Master= new HISPARC_UNIT(Master_Name,Master_Serial_No,Master_Port_No) ;
	Slave= new HISPARC_UNIT(Slave_Name,Slave_Serial_No,Slave_Port_No) ;

	Master->Open_Socket() ;
	Slave->Open_Socket() ;

	Calibration=Master->Get_Messages() ;
	Slave->Get_Messages() ;

	Master->Open_Hisparc_Unit() ;	
	Slave->Open_Hisparc_Unit() ;
	printf("\n") ;

	Master->Soft_Reset() ;
	Slave->Soft_Reset() ;

	time_t time1,time2 ;
	if(Calibration==Off)
	{
		dtime=Master->Set_Run_Controls() ;
		Slave->Set_Run_Controls() ;

		Master->Set_Master_Writing_Mode() ;
		Slave->Set_Slave_Writing_Mode() ;

		Master->Open_Output_Files(argv[1]) ;	
		Slave->Open_Output_Files(argv[2]) ;

		time1=time(NULL) ;
		Master->Ask_Control_Parameters() ;
		Slave->Ask_Control_Parameters() ;
		sleep(1) ;
	}
	if(Calibration==On)
	{
		cal_type=0 ;	//Start with Full scale adjust
		Master->Set_Calib_Controls() ;
		Slave->Set_Calib_Controls() ;

		Master->Set_Master_Writing_Mode() ;
		Slave->Set_Slave_Writing_Mode() ;

		Master->Open_Output_Files(argv[1]) ;	
		Slave->Open_Output_Files(argv[2]) ;
	}

	signal(SIGPIPE,SIG_IGN) ;
	int i=1 ;
	int c1,c2 ;
	while(1)
	{
		//printf("(%d)\n",i) ;
		if(Calibration==Off)
		{
			if(Master->Send_Data_To_Socket()==-1) break ;	
			if(Slave->Send_Data_To_Socket()==-1) break ;

			usleep(1000) ;
		//sleep(2) ;
			time2=time(NULL) ;
			if((unsigned int)(time2-time1)==(unsigned int)dtime)
			{
				Master->Ask_Control_Parameters() ;
				Slave->Ask_Control_Parameters() ;
				time1=time(NULL) ;
			}
		}
		if(Calibration==On)
		{
			c1=Master->Do_Calibration() ;
			c2=Slave->Do_Calibration() ; 
			if(c1==1 && c2==1) 
			{
				switch(cal_type)
				{
					case 0:
						cal_type=1 ;
						break ;
						
					case 1:
						cal_type=2 ;
						Master->Reset_Full_Scale() ;
						Slave->Reset_Full_Scale() ;
						break ;
					
					case 2:
						cal_type=3 ;
						break ;
					
					case 3:
						cal_type=4 ;
						Master->Reset_Common_Off() ;
						Slave->Reset_Common_Off() ;
						break ;
						
					case 4:
						printf("\n***** Calibration: DONE *****\n") ;
						if(Master->Send_Data_To_Socket()==-1) break ;	
						if(Slave->Send_Data_To_Socket()==-1) break ;
						Calibration=Done ;
						break ;

					default:
						printf("\n***** Select proper Calibration mode *****\n") ;
						break ;
				}
			}
			if(Calibration==Done) break ;
			printf("\n") ;
			Master->Set_Master_Writing_Mode() ;
			Slave->Set_Slave_Writing_Mode() ;
		}
		i++ ;
	}
	
	Master->Close_Output_Files() ;	
	Slave->Close_Output_Files() ;

	Master->Soft_Reset() ;
	Slave->Soft_Reset() ;

	Master->Close_Hisparc_Unit() ;	
	Slave->Close_Hisparc_Unit() ;

	Master->Close_Socket() ;
	Slave->Close_Socket() ;
	
	int k=atexit(BYE) ;
	if(k!=0) printf("atexit: Error\n") ;

	return 0 ;	
}
