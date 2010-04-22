#include <TApplication.h>
#include <TFile.h>
#include <TSystem.h>
#include <TTree.h>
#include <stdio.h>
#include <signal.h>
#include "Constants.h"
#include "monitoring.h"
#include "arrival_direction.h"
#include "Global_Variables.h"

void Read_Detector_Cord() ;
void Initialise_Variables() ;
void Create_ROOT_File() ;
void Write_ROOT_File() ;
void Get_Data() ;
void Store_Sec_Data() ;
void Store_Control_Data() ;
void Check_Coincidence() ;
void Sort_Event_Times(int) ;

MONITORING *Monitoring ;
ARRIVAL_DIRECTION *Arrival_Direction ;
#include "FUNCTION_Device.h"
#include "Get_Data.h"

/*int handler=0 ;
void signal_handler(int unnused)
{
	printf("You pressed Ctrl-C\n") ;
	handler=1 ;
}*/
int main(int argc, char **argv)
{
	TApplication theApp("App", &argc, argv);
	Monitoring=new MONITORING(gClient->GetRoot(),200,200) ;
	Monitoring->Initialisation() ;
	
	while(1)
	{
		printf("LASA 1: Select %d\n",Monitoring->Select_Lasa1_return()) ;
		if(Monitoring->Select_Lasa1_return()==1) lasa_select[0]=1 ;
		else lasa_select[0]=0 ;
		printf("LASA 2: Select %d\n",Monitoring->Select_Lasa2_return()) ;
		if(Monitoring->Select_Lasa2_return()==1) lasa_select[1]=1 ;
		else lasa_select[1]=0 ;
		printf("LASA 3: Select %d\n",Monitoring->Select_Lasa3_return()) ;
		if(Monitoring->Select_Lasa3_return()==1) lasa_select[2]=1 ;
		else lasa_select[2]=0 ;
		printf("LASA 4: Select %d\n",Monitoring->Select_Lasa4_return()) ;
		if(Monitoring->Select_Lasa4_return()==1) lasa_select[3]=1 ;
		else lasa_select[3]=0 ;
		printf("LASA 5: Select %d\n",Monitoring->Select_Lasa5_return()) ;
		if(Monitoring->Select_Lasa5_return()==1) lasa_select[4]=1 ;
		else lasa_select[4]=0 ;

		printf("\n") ;
		if(Monitoring->Select_Done_return()==1) break ;
		sleep(1) ;
    		gSystem->ProcessEvents() ;	
	}
	while(1)
	{
		if(Monitoring->Start_return()==1)
		{	Calibration=0 ;
			printf("Starting Normal RUN\n") ;
			break ;
		}
		if(Monitoring->Start_return()==2)
		{	Calibration=1 ;
			printf("Starting Calibration RUN\n") ;
			break ;
		}
		sleep(1) ;
		printf("Select Run\n") ;
    		gSystem->ProcessEvents() ;	
	}

	char command[200],name[200] ;
	sockfd_Max=0 ;
	for(int i=0;i<NO_LASAs;i++)
	{
		if(lasa_select[i]==0) continue ;
		strcpy(command,"") ;
		strcpy(name,"") ;
		sprintf(name,"xterm -T LASA%d",i+1) ;
		sprintf(command," -e ./start_lasa %d &",i+1) ;
		strcat(name,command) ;
		printf("Now starting LASA %d\n",i+1) ;
		system(name) ;

		lasa[i].Master=new DEVICE() ;
		lasa[i].Slave=new DEVICE() ;
	
		printf("-------------------------\n") ;
		lasa[i].Master->Read_Message_File("./data/Lasa_Settings.dat",2*i+1) ;
		printf("-------------------------\n") ;
		lasa[i].Slave->Read_Message_File("./data/Lasa_Settings.dat",2*i+2) ;
	
		lasa[i].Master->Open_Socket() ;
		lasa[i].Slave->Open_Socket() ;

		lasa[i].sockfd_master=lasa[i].Master->Accept_Socket() ;
		if(lasa[i].sockfd_master>sockfd_Max) sockfd_Max=lasa[i].sockfd_master ;
		lasa[i].sockfd_slave=lasa[i].Slave->Accept_Socket() ;
		if(lasa[i].sockfd_slave>sockfd_Max) sockfd_Max=lasa[i].sockfd_slave ;

		if(Calibration==0)
		{
			lasa[i].Master->Build_Hisparc_Messages() ;
			lasa[i].Slave->Build_Hisparc_Messages() ;
		}
	}

	Monitoring->DRAW_canvas() ;
	Monitoring->DEFINE_Histo() ;

	int no_pcs=0 ;	
	for(int i=0;i<NO_LASAs;i++)
	{
		if(lasa_select[i]==0) continue ;
		no_pcs++ ;
		lasa[i].Master->Send_Messages() ;
		lasa[i].Slave->Send_Messages() ;
	}

	Initialise_Variables() ;
	Arrival_Direction->Read_Detector_Cord() ;
	Create_ROOT_File() ;
	//signal(SIGINT,signal_handler) ;

	int j ;
	j=1 ;
	timeA=time(NULL) ;
	while(1)
	{
		//if(handler==1) break ;
		//printf("(%6d)\n",j) ;
    		gSystem->ProcessEvents() ;
		if(Monitoring->Exit_return()==1) break ;

		Monitor2=Monitoring->Event_Display_interval_return() ;	//event display interval
		Get_Data() ;
		if(Calib_Done==2*no_pcs)
		{
			FILE *calib ;
			calib=fopen("./data/Calibration.dat","w") ;
			fprintf(calib,"LASA\t\tOffset[1+]\tOffset[1-]\tOffset[2+]\tOffset[2-]\tGain[1+]\tGain[1-]\tGain[2+]\tGain[2-]\tCommon Offset\n") ;
			for(int i=0;i<NO_LASAs;i++)
			{
				fprintf(calib,"%d (Master)\t%d\t\t%d\t\t%d\t\t%d",i+1,lasa[i].data_master[0],lasa[i].data_master[1],lasa[i].data_master[2],lasa[i].data_master[3]) ;
				fprintf(calib,"\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n",lasa[i].data_master[4],lasa[i].data_master[5],lasa[i].data_master[6],lasa[i].data_master[7],lasa[i].data_master[8]) ;
				fprintf(calib,"%d (Slave)\t%d\t\t%d\t\t%d\t\t%d",i+1,lasa[i].data_slave[0],lasa[i].data_slave[1],lasa[i].data_slave[2],lasa[i].data_slave[3]) ;
				fprintf(calib,"\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n",lasa[i].data_slave[4],lasa[i].data_slave[5],lasa[i].data_slave[6],lasa[i].data_slave[7],lasa[i].data_slave[8]) ;
			}
			fclose(calib) ;
			printf("\nCALIBRATION: DONE\n") ;
			while(1)
			{
				gSystem->ProcessEvents() ;
				if(Monitoring->Exit_return()==1) break ;
			}	
		}
		j++ ;
	}
	Write_ROOT_File() ;

	for(int i=0;i<NO_LASAs;i++)
	{
		if(lasa_select[i]==0) continue ;

		close(lasa[i].sockfd_master) ;
		close(lasa[i].sockfd_slave) ;
	}
	
	printf("\n******** EXIT *******\n\n") ;
	gApplication->Terminate(0) ;
	theApp.Run() ; 
	return 0 ;
}
