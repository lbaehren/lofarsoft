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
#include "Auger.h"

int evtypes[3] ;
void gps_data() ;

int main(int argc, char **argv)
{
	FT_HANDLE ftHandle,ftHandle2 ;
	FT_STATUS ftStatus,ftStatus2 ;
	unsigned char dat[200],dat2[200],dat_rev[100000],dat_rev2[100000] ;
	DWORD dwBytesWritten, dwBytesRead, R_Bytes, T_Bytes, EventDWord ;
	DWORD dwBytesWritten2, dwBytesRead2, R_Bytes2, T_Bytes2, EventDWord2 ;

	if((ftStatus = FT_OpenEx((void*)"FTQZTB7R",FT_OPEN_BY_SERIAL_NUMBER, &ftHandle))==FT_OK) printf("\nMaster: open\n") ;
	if((ftStatus2 = FT_OpenEx((void*)"FTRCDY18",FT_OPEN_BY_SERIAL_NUMBER, &ftHandle2))==FT_OK) printf("\nSlave: open\n") ;
	/*if((ftStatus = FT_Open(0, &ftHandle))==FT_OK) printf("\nUSB 1 OK: Master opened\n") ;
	if((ftStatus2 = FT_Open(1, &ftHandle2))==FT_OK) printf("\nUSB 2 OK: Slave opened\n") ;
	FT_DEVICE ftDevice,ftDevice2 ;
	DWORD deviceID,deviceID2 ;
	char SerialNumber[16],SerialNumber2[16] ;
	char Description[64],Description2[64] ;
	ftStatus = FT_GetDeviceInfo(ftHandle,&ftDevice,&deviceID,SerialNumber,Description,NULL) ;
	ftStatus2 = FT_GetDeviceInfo(ftHandle2,&ftDevice2,&deviceID2,SerialNumber2,Description2,NULL) ;
	printf("HISPARC master Serial number=%s\n",SerialNumber) ;
	printf("HISPARC slave Serial number=%s\n",SerialNumber2) ;*/

	dat[0]=0x99 ;
	dat[1]=0xff ;
	dat[2]=0x66 ;
	if((ftStatus=FT_Write(ftHandle,dat,3,&dwBytesWritten))!=FT_OK) printf("Master: Err=%d\n",(int)ftStatus) ;
	if((ftStatus2=FT_Write(ftHandle2,dat,3,&dwBytesWritten))!=FT_OK) printf("Slave: Err=%d\n",(int)ftStatus2) ;
	
	sleep(1);
	FT_GetStatus(ftHandle,&R_Bytes,&T_Bytes,&EventDWord) ;
	FT_GetStatus(ftHandle2,&R_Bytes2,&T_Bytes2,&EventDWord2) ;
	FT_Purge(ftHandle,FT_PURGE_RX || FT_PURGE_TX);
	FT_Purge(ftHandle2,FT_PURGE_RX || FT_PURGE_TX);
  
	printf("Master: STATUS after Soft Reset: \tT_Bytes=%lu\tR_Bytes=%u\n",(unsigned long)T_Bytes,(int)R_Bytes) ;
	printf("Slave: STATUS after Soft Reset: \tT_Bytes=%lu\tR_Bytes=%u\n",(unsigned long)T_Bytes2,(int)R_Bytes2) ;
	sleep(1);
  
	dat[0] = 0x99 ;
	dat[1] = 0x35 ;
	dat[2] = 0x0 ;
	dat[3] = 0x0 ;
	dat[4] = 0x0 ;
	dat[5] = 0x1 ;
	dat[6] = 0x66 ;
	FT_Write(ftHandle,dat,7,&dwBytesWritten) ;
	dat[5] = 0x9 ;		//Setting Master->Slave
	FT_Write(ftHandle2,dat,7,&dwBytesWritten2) ;
	//sleep(2);
	FT_GetStatus(ftHandle,&R_Bytes,&T_Bytes,&EventDWord) ;
	FT_GetStatus(ftHandle2,&R_Bytes2,&T_Bytes2,&EventDWord2) ;
	printf("Master: STATUS after Writing mode: \tT_Bytes=%d\tR_Bytes=%d\n",(int)T_Bytes,(int)R_Bytes) ;
	printf("Slave: STATUS after Writing mode: \tT_Bytes=%d\tR_Bytes=%d\n",(int)T_Bytes2,(int)R_Bytes2) ;
	sleep(1);

	//-------------------Setting Trigger thresholds--------------
	unsigned short Low_thres ;
	Low_thres=40 ;	//Threshold in mv
	dat[0] = 0x99 ;
	dat[1] = 0x20 ;		//Channel 1 low threshold
	dat[2] = ((unsigned short)(Low_thres/0.4883) & 0xff00)>>8 ;
	dat[3] = ((unsigned short)(Low_thres/0.4883) & 0x00ff) ;
	dat[4] = 0x66 ;
	FT_Write(ftHandle,dat,5,&dwBytesWritten) ;

	dat[1] = 0x22 ;		//Channel 2 low threshold
	FT_Write(ftHandle,dat,5,&dwBytesWritten) ;
	//-------------------Setting Trigger condition--------------
	dat[1] = 0x30 ;
	//dat[2] = 0x10 ;		//atleast 2 high
	dat[2] = 0x02 ;			//atleast 2 low
	dat[3] = 0x66 ;
	FT_Write(ftHandle,dat,4,&dwBytesWritten) ;
	
	//-------------------Setting time windows-------------------
	unsigned short Pre_C_Time,Coin_Time,Post_C_Time ;
	Pre_C_Time=(unsigned short)(2.0*200) ;	//(x*200): in 5ns steps.
	Coin_Time=(unsigned short)(0.4*200) ;
	Post_C_Time=(unsigned short)(7.6*200) ;
	
	dat[0]=0x99 ;
	dat[1]=0x31 ;
	dat[2]=(Pre_C_Time&0xff00)>>8 ;
	dat[3]=(Pre_C_Time&0x00ff) ;
	dat[4]=0x66 ;
	FT_Write(ftHandle,dat,5,&dwBytesWritten) ;
	FT_Write(ftHandle2,dat,5,&dwBytesWritten2) ;

	dat[0]=0x99 ;
	dat[1]=0x32 ;
	dat[2]=(Coin_Time&0xff00)>>8 ;
	dat[3]=(Coin_Time&0x00ff) ;
	dat[4]=0x66 ;
	FT_Write(ftHandle,dat,5,&dwBytesWritten) ;
	FT_Write(ftHandle2,dat,5,&dwBytesWritten2) ;
	
	dat[0]=0x99 ;
	dat[1]=0x33 ;
	dat[2]=(Post_C_Time&0xff00)>>8 ;
	dat[3]=(Post_C_Time&0x00ff) ;
	dat[4]=0x66 ;
	FT_Write(ftHandle,dat,5,&dwBytesWritten) ;
	FT_Write(ftHandle2,dat,5,&dwBytesWritten2) ;
	//--------------------------xxx--------------------------
	//----------------------Setting HV-----------------------
	unsigned short HV ;
	HV=1500 ;	//PMT HV 
 	dat[0]=0x99 ;
	dat[1]=0x1E ;		//For channel 1
	dat[2]=(unsigned short)((HV+34.0714)/7.93155) & 0x00ff ;
	dat[3]=0x66 ;

	FT_Write(ftHandle,dat,4,&dwBytesWritten) ;	//USB 1 (Master)
	//FT_Write(ftHandle2,dat,4,&dwBytesWritten2) ;	//USB 2 (Slave)

	HV=1500 ;	//PMT HV 
	dat[1]=0x1F ;		//For channel 2
	dat[2]=(unsigned short)((HV+34.0714)/7.93155) & 0x00ff ;
	FT_Write(ftHandle,dat,4,&dwBytesWritten) ;	//USB 1
	//FT_Write(ftHandle2,dat,4,&dwBytesWritten2) ;	//USB 2
	//--------------------------xxx--------------------------

	dat[0]=0x99 ;
	dat[1]=0x55 ;
	dat[2]=0x66 ;
	FT_Write(ftHandle,dat,3,&dwBytesWritten) ;
	FT_Write(ftHandle2,dat,3,&dwBytesWritten2) ;

//-----------------ADC gain 
	dat[0]=0x99 ;
	dat[1]=0x14 ;
	dat[2]=0x0 ;
	dat[3]=0x66 ;
	FT_Write(ftHandle,dat,4,&dwBytesWritten) ;
	FT_Write(ftHandle2,dat,4,&dwBytesWritten2) ;
	dat[1]=0x15 ;
	FT_Write(ftHandle,dat,4,&dwBytesWritten) ;
	FT_Write(ftHandle2,dat,4,&dwBytesWritten2) ;
	dat[1]=0x16 ;
	FT_Write(ftHandle,dat,4,&dwBytesWritten) ;
	FT_Write(ftHandle2,dat,4,&dwBytesWritten2) ;
	dat[1]=0x17 ;
	FT_Write(ftHandle,dat,4,&dwBytesWritten) ;
	FT_Write(ftHandle2,dat,4,&dwBytesWritten2) ;

	//*************ADC offset adjust
	dat[0]=0x99 ;
	dat[1]=0x10 ;
	dat[2]=0x00 ;
	dat[3]=0x66 ;
	FT_Write(ftHandle,dat,4,&dwBytesWritten) ;
	FT_Write(ftHandle2,dat,4,&dwBytesWritten2) ;
	dat[1]=0x11 ;
	dat[2]=0x80;
	FT_Write(ftHandle,dat,4,&dwBytesWritten) ;
	FT_Write(ftHandle2,dat,4,&dwBytesWritten2) ;
	dat[1]=0x12 ;
	FT_Write(ftHandle,dat,4,&dwBytesWritten) ;
	FT_Write(ftHandle2,dat,4,&dwBytesWritten2) ;
	dat[1]=0x13 ;
	FT_Write(ftHandle,dat,4,&dwBytesWritten) ;
	FT_Write(ftHandle2,dat,4,&dwBytesWritten2) ;
//------------------
	sleep(1);
	FT_GetStatus(ftHandle,&R_Bytes,&T_Bytes,&EventDWord) ;
	FT_GetStatus(ftHandle2,&R_Bytes2,&T_Bytes2,&EventDWord2) ;
	printf("Master: STATUS after Ask Control: \tT_Bytes=%d\tR_Bytes=%d\n",(int)T_Bytes,(int)R_Bytes) ;
	printf("Slave: STATUS after Ask Control: \tT_Bytes=%d\tR_Bytes=%d\n",(int)T_Bytes2,(int)R_Bytes2) ;
	
	sleep(1);
	//FT_GetStatus(ftHandle,&R_Bytes,&T_Bytes,&EventDWord) ;
	//printf("STATUS after Ask Control: \tT_Bytes=%d\tR_Bytes=%d\n",(int)T_Bytes,(int)R_Bytes) ;
	//sleep(1);

	FT_Read(ftHandle,dat_rev,R_Bytes,&dwBytesRead) ;
	FT_Read(ftHandle2,dat_rev2,R_Bytes2,&dwBytesRead2) ;
	sleep(1) ;

	printf("\n") ;
	//int i ;
	for(int i=1;i<=(int)R_Bytes;i++)
		printf("Master: Received data[%2d] = 0x%x (%d)\n",i,dat_rev[i-1],dat_rev[i-1]) ;

	printf("\n********** PMT 1 HV=%u\t PMT 2 HV=%u ************\n\n",(unsigned int)(dat_rev[16]*7.93155-34.0714),(unsigned int)(dat_rev[17]*7.93155-34.0714)) ;	
	unsigned short pre_time = (dat_rev[27]<<8)+(dat_rev[28]) ;
	printf("Pre Coin time = %x (%d)\n",pre_time,pre_time);

	unsigned short coin_time = (dat_rev[29]<<8)+(dat_rev[30]) ;
	printf("Coincidence time = %x (%d)\n",coin_time,coin_time);

	unsigned short post_time = (dat_rev[31]<<8)+(dat_rev[32]) ;
	printf("Post Coin time = %x (%d)\n",post_time,post_time);
	
	for(int i=1;i<=(int)R_Bytes2;i++)
		printf("Slave: Received data[%2d] = 0x%x (%d)\n",i,dat_rev2[i-1],dat_rev2[i-1]) ;
  
	sleep(1) ;
	FT_GetStatus(ftHandle,&R_Bytes,&T_Bytes,&EventDWord) ;
	FT_GetStatus(ftHandle2,&R_Bytes2,&T_Bytes2,&EventDWord2) ;
	printf("\nMaster: Check Anything left in the Queue: \tT_Bytes=%d\tR_Bytes=%d\n",(int)T_Bytes,(int)R_Bytes) ;
	printf("Slave: Check Anything left in the Queue: \tT_Bytes=%d\tR_Bytes=%d\n\n",(int)T_Bytes2,(int)R_Bytes2) ;
	//----------------Asking 1 second messeges------------------------
	dat[0] = 0x99;
	dat[1] = 0x35;
	dat[2] = 0x0;
 	dat[3] = 0x0;
 	dat[4] = 0x0;
 	//dat[5] = 0x3;	//1 sec message
 	dat[5] = 0x1;	//only writing mode
 	dat[6] = 0x66;
	FT_Write(ftHandle,dat,7,&dwBytesWritten) ;
	sleep(1);
	FT_GetStatus(ftHandle,&R_Bytes,&T_Bytes,&EventDWord) ;
	printf("Master: STATUS after Asking for 1 sec. messages: \tT_Bytes=%d\tR_Bytes=%d\n",(int)T_Bytes,(int)R_Bytes) ;
	//FT_Read(ftHandle,dat_rev,R_Bytes,&dwBytesRead) ;
	//-----------------------xxx---------------------------------------
	/*sleep(1);
	FT_GetStatus(ftHandle,&R_Bytes,&T_Bytes,&EventDWord) ;
	printf("STATUS after Asking for 1 sec. messages: \tT_Bytes=%d\tR_Bytes=%d\n",(int)T_Bytes,(int)R_Bytes) ;
	FT_Read(ftHandle,dat_rev,R_Bytes,&dwBytesRead) ;
	
	sleep(1);
	FT_GetStatus(ftHandle,&R_Bytes,&T_Bytes,&EventDWord) ;
	FT_GetStatus(ftHandle2,&R_Bytes2,&T_Bytes2,&EventDWord2) ;
	printf("STATUS USB 1 after Asking for 1 sec. messages: \tT_Bytes=%d\tR_Bytes=%d\n",(int)T_Bytes,(int)R_Bytes) ;
	printf("STATUS USB 2 after Asking for 1 sec. messages: \tT_Bytes=%d\tR_Bytes=%d\n",(int)T_Bytes2,(int)R_Bytes2) ;
 
	FT_Read(ftHandle,dat_rev,R_Bytes,&dwBytesRead) ;
	for(int i=1;i<=(int)R_Bytes;i++)
		printf("Received data[%2d] = 0x%x (%d)\n",i,dat_rev[i-1],dat_rev[i-1]) ;
	//printf("\ndat_rev[4] = 0x%x (%d)",dat_rev[4],dat_rev[4]) ;
	//printf("\nAfter << 8") ;
 	//printf("\ndat_rev[4] = 0x%x (%d)\n",dat_rev[4]<<8,dat_rev[4]<<8) ;
	//printf("dat_rev[4]+dat_rev[5] = 0x%x (%d)\n",(dat_rev[4]<<8)+dat_rev[5],(dat_rev[4]<<8)+dat_rev[5]) ;
	gps_data(dat_rev) ;
	*/

	sleep(1) ;
	FT_GetStatus(ftHandle,&R_Bytes,&T_Bytes,&EventDWord) ;
	FT_GetStatus(ftHandle2,&R_Bytes2,&T_Bytes2,&EventDWord2) ;
	printf("\nMaster: Check Anything left in the Queue: \tT_Bytes=%d\tR_Bytes=%d\n",(int)T_Bytes,(int)R_Bytes) ;
	printf("Slave: Check Anything left in the Queue: \tT_Bytes=%d\tR_Bytes=%d\n\n",(int)T_Bytes2,(int)R_Bytes2) ;
	//printf("STATUS USB 2 after Asking for 1 sec. messages: \tT_Bytes=%d\tR_Bytes=%d\n",(int)T_Bytes2,(int)R_Bytes2) ;
	
	/*dat[0]=0x99 ;
	dat[1]=0x30 ;
	dat[2]=0x40 ;
	dat[3]=0x66 ;
	FT_Write(ftHandle,dat,4,&dwBytesWritten) ;
	printf("\nSUPPLY EXTERNAL TRIGGER\n") ;
	usleep(10000);*/
	//printf("\nGive pulse: \n") ;

	printf("\nWait for trigger: \n") ;	
	//sleep(5) ;
	sleep(1) ;
	FILE* evt_file_USB1,*evt_file_USB2 ;
	evt_file_USB1=fopen(argv[1],"w") ;
	evt_file_USB2=fopen(argv[2],"w") ;
	int out_USB1,out_USB2 ;
	out_USB1=open(argv[1],O_CREAT|O_WRONLY|S_IWRITE) ;
	out_USB2=open(argv[2],O_CREAT|O_WRONLY|S_IWRITE) ;

	for(int j=1;j<=2;j++)
	{	
	FT_GetStatus(ftHandle,&R_Bytes,&T_Bytes,&EventDWord) ;
	FT_GetStatus(ftHandle2,&R_Bytes2,&T_Bytes2,&EventDWord2) ;
	printf("j=%d\n",j) ;
	printf("Master: STATUS after trigger: \tT_Bytes=%d\tR_Bytes=%d\n",(int)T_Bytes,(int)R_Bytes) ;
	printf("Slave: STATUS after trigger: \tT_Bytes=%d\tR_Bytes=%d\n",(int)T_Bytes2,(int)R_Bytes2) ;
 
	FILE* evt_file_USB1,*evt_file_USB2 ;
	evt_file_USB1=fopen(argv[1],"w") ;
	evt_file_USB2=fopen(argv[2],"w") ;
	if((int)R_Bytes>0)
	{	
		FT_Read(ftHandle,dat_rev,R_Bytes,&dwBytesRead) ;
		printf("No. of Bytes read from USB1 = %d\n",(int)dwBytesRead) ;
		for(int i=1;i<=(int)R_Bytes;i++)
		{
			//printf("Received data[%2d] = 0x%x (%d)\n",i,dat_rev[i-1],dat_rev[i-1]) ;
			fprintf(evt_file_USB1,"%2d\t0x%02x\t%d\n",i,dat_rev[i-1],dat_rev[i-1]) ;
		}
		//write(out_USB1,dat_rev,R_Bytes) ;
	}
	//fclose(evt_file_USB1) ;
	if((int)R_Bytes2>0)
	{	
		FT_Read(ftHandle2,dat_rev2,R_Bytes2,&dwBytesRead2) ;
		printf("No. of Bytes read from USB2= %d\n",(int)dwBytesRead2) ;
		for(int i=1;i<=(int)R_Bytes2;i++)
		{
			//printf("Received data[%2d] = 0x%x (%d)\n",i,dat_rev[i-1],dat_rev[i-1]) ;
			fprintf(evt_file_USB2,"%2d\t%x\t%d\n",i,dat_rev2[i-1],dat_rev2[i-1]) ;
		}
		//write(out_USB2,dat_rev2,R_Bytes2) ;
	}
	sleep(1) ;
	}
	fclose(evt_file_USB1) ;	
	fclose(evt_file_USB2) ;
	close(out_USB1) ;
	close(out_USB2) ;
	
	FT_GetStatus(ftHandle,&R_Bytes,&T_Bytes,&EventDWord) ;
	FT_GetStatus(ftHandle2,&R_Bytes2,&T_Bytes2,&EventDWord2) ;
	printf("\nMaster: Check Anything left in the Queue: \tT_Bytes=%d\tR_Bytes=%d\n",(int)T_Bytes,(int)R_Bytes) ;
	printf("Slave: Check Anything left in the Queue: \tT_Bytes=%d\tR_Bytes=%d\n\n",(int)T_Bytes2,(int)R_Bytes2) ;

	//------------Reset HV to "0"--------------
 	dat[0]=0x99 ;
	dat[1]=0x1E ;		//For channel 1
	dat[2]=0x0 ;
	dat[3]=0x66 ;

	FT_Write(ftHandle,dat,4,&dwBytesWritten) ;	//USB 1 (Master)
	FT_Write(ftHandle2,dat,4,&dwBytesWritten2) ;	//USB 2 (Slave)

	dat[1]=0x1F ;		//For channel 2
	FT_Write(ftHandle,dat,4,&dwBytesWritten) ;	//USB 1
	FT_Write(ftHandle2,dat,4,&dwBytesWritten2) ;	//USB 2
	//--------------------------xxx--------------------------

	FT_Close(ftHandle) ;
	FT_Close(ftHandle2) ;
	return 0 ;
}
