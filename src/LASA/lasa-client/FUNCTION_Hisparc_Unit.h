#include "CLASS_Hisparc_Unit.h"

void HISPARC_UNIT:: Open_Socket()
{
	sockfd=Socket->Open_Socket(Port_Num) ;
	Socket->Connect_Socket(Port_Num) ;
}

unsigned short HISPARC_UNIT:: Get_Messages()
{
	int bytes_read ;
	fd_set readfds ;
	FD_ZERO(&readfds) ;
	FD_SET(sockfd,&readfds) ;
	select(sockfd+1,&readfds,NULL,NULL,NULL) ;

	if(FD_ISSET(sockfd,&readfds))
	{
		if((bytes_read = read(sockfd,read_data,200))==-1)
			perror("read(): Error\n") ;

		printf("%s: Bytes read=%d\n",Device_Name,bytes_read) ;
	}
	/*for(int i=0;i<=40;i++)
		printf("Message[%d]=0x%x\t%d\n",i,read_data[i],read_data[i]) ;*/

	if(read_data[1]==0x30 && read_data[2]==0x80) calibration=1 ;
	else calibration=0 ;
	return(calibration) ;
}

void HISPARC_UNIT:: Open_Hisparc_Unit()
{
	if((ft_status = FT_OpenEx((void*) Device_Serial_No,FT_OPEN_BY_SERIAL_NUMBER, &ft_handle))==FT_OK) printf("%s(%s): open\n",Device_Name,Device_Serial_No) ;
}

void HISPARC_UNIT:: Soft_Reset()
{
	dat[0]=0x99 ;
	dat[1]=0xff ;
	dat[2]=0x66 ;
	if((ft_status=FT_Write(ft_handle,dat,3,&dwBytesWritten))!=FT_OK) printf("%s: Err=%d\n",Device_Name,(int)ft_status) ;
	
	sleep(1);
	FT_Purge(ft_handle,FT_PURGE_RX || FT_PURGE_TX);
	FT_GetStatus(ft_handle,&R_Bytes,&T_Bytes,&EventDWord) ;
  
	printf("%s: STATUS after Soft Reset: \tT_Bytes=%lu\tR_Bytes=%u\n",Device_Name,(unsigned long)T_Bytes,(int)R_Bytes) ;
	sleep(1) ;
}

unsigned short HISPARC_UNIT:: Set_Run_Controls()
{
	dat[0]=0x99 ;
	dat[1]=0x50 ;

	for(int i=0;i<=30;i++)
		dat[2+i]=read_data[i] ;

	dat[33]=0x0 ;
	dat[34]=0x0 ;
	dat[35]=0x0 ;
	dat[36]=0x0 ;
	dat[37]=0x66 ;
	FT_Write(ft_handle,dat,38,&dwBytesWritten) ;
	
	unsigned short dtime ;
	dtime=(unsigned short)((read_data[31]<<8)+(read_data[32])) ;
	return dtime ;
	sleep(1) ;
}

void HISPARC_UNIT:: Set_Full_Scale(unsigned char fs)	//Setting internal voltage
{
	dat[0] = 0x99 ;	
	dat[1] = 0x19 ;
	dat[2] = fs ;
	dat[3] = 0x66 ;
	FT_Write(ft_handle,dat,4,&dwBytesWritten) ;
}

void HISPARC_UNIT:: Reset_Full_Scale()	//Reset internal voltage
{
	dat[0] = 0x99 ;	
	dat[1] = 0x19 ;
	dat[2] = 0x0 ;
	dat[3] = 0x66 ;
	FT_Write(ft_handle,dat,4,&dwBytesWritten) ;
	Offset_done[0]=Offset_done[1]=Offset_done[2]=Offset_done[3]=0 ;
	Gain_done[0]=Gain_done[1]=Gain_done[2]=Gain_done[3]=0 ;
	Full_Scale=Common_Off=0x0 ;
	Full_Scale_done=Common_Off_done=0 ;
	sum_av=0 ;
}

void HISPARC_UNIT:: Reset_Common_Off()	//Reset internal voltage
{
	dat[0] = 0x99 ;	
	dat[1] = 0x18 ;
	dat[2] = 0x0 ;
	dat[3] = 0x66 ;
	FT_Write(ft_handle,dat,4,&dwBytesWritten) ;
	Common_Off=0x0 ;
	Common_Off_done=0 ;
}

void HISPARC_UNIT:: Set_Common_Off(unsigned char fs)	//Setting common offset
{
	dat[0] = 0x99 ;	
	dat[1] = 0x18 ;
	dat[2] = fs ;
	dat[3] = 0x66 ;
	FT_Write(ft_handle,dat,4,&dwBytesWritten) ;
}

void HISPARC_UNIT:: Set_Calib_Controls()
{
	ADC_Offset[0]=ADC_Offset[1]=ADC_Offset[2]=ADC_Offset[3]=0x80 ;
	ADC_Gain[0]=ADC_Gain[1]=ADC_Gain[2]=ADC_Gain[3]=0x80 ;

	//------------------ADC Offset/Gain initial settings------------------
	for(int j=0;j<4;j++)
	{
		dat[0] = 0x99 ;
		dat[1] = 0x10+j ;
		dat[2]= ADC_Offset[j] ;
		dat[3] = 0x66 ;
		FT_Write(ft_handle,dat,4,&dwBytesWritten) ;

		dat[1] = 0x14+j ;
		dat[2]= ADC_Gain[j] ; 
		FT_Write(ft_handle,dat,4,&dwBytesWritten) ;
	}
	dat[1]=0x18 ;		//Common offset adjust
	dat[2]=0x0 ;
	FT_Write(ft_handle,dat,4,&dwBytesWritten) ;
	Reset_Full_Scale() ;	//Reset (full scale adjust) internal voltage

	//-------------------Setting time windows (10us)-------------------
	unsigned short Pre_C_Time,Coin_Time,Post_C_Time ;
	Pre_C_Time=(unsigned short)(2.0*200) ;	//(x*200): in 5ns steps.
	Coin_Time=(unsigned short)(0.4*200) ;
	Post_C_Time=(unsigned short)(7.6*200) ;
	
	dat[0]=0x99 ;
	dat[1]=0x31 ;
	dat[2]=(Pre_C_Time&0xff00)>>8 ;
	dat[3]=(Pre_C_Time&0x00ff) ;
	dat[4]=0x66 ;
	FT_Write(ft_handle,dat,5,&dwBytesWritten) ;

	dat[0]=0x99 ;
	dat[1]=0x32 ;
	dat[2]=(Coin_Time&0xff00)>>8 ;
	dat[3]=(Coin_Time&0x00ff) ;
	dat[4]=0x66 ;
	FT_Write(ft_handle,dat,5,&dwBytesWritten) ;
	
	dat[0]=0x99 ;
	dat[1]=0x33 ;
	dat[2]=(Post_C_Time&0xff00)>>8 ;
	dat[3]=(Post_C_Time&0x00ff) ;
	dat[4]=0x66 ;
	FT_Write(ft_handle,dat,5,&dwBytesWritten) ;
	//--------------------------xxx--------------------------
	dat[1]=0x30 ;	//Trigger condition
	dat[2]=0x80 ;
	dat[3]=0x66 ;
	FT_Write(ft_handle,dat,4,&dwBytesWritten) ;
}

void HISPARC_UNIT:: Set_Master_Writing_Mode()
{
	dat[0] = 0x99 ;
	dat[1] = 0x35 ;
	dat[2] = 0x0 ;
 	dat[3] = 0x0 ;
 	dat[4] = 0x0 ;
 	dat[5] = 0x3 ;
 	dat[6] = 0x66 ;

	if(calibration==On) dat[5]=0x1 ;
	FT_Write(ft_handle,dat,7,&dwBytesWritten) ;
	FT_GetStatus(ft_handle,&R_Bytes,&T_Bytes,&EventDWord) ;
	//printf("%s: STATUS after writing mode: \tT_Bytes=%d\tR_Bytes=%d\n",Device_Name,(int)T_Bytes,(int)R_Bytes) ;
	sleep(1);
}

void HISPARC_UNIT:: Set_Slave_Writing_Mode()
{
	dat[0] = 0x99 ;
	dat[1] = 0x35 ;
	dat[2] = 0x0 ;
	dat[3] = 0x0 ;
	dat[4] = 0x0 ;
	dat[5] = 0x1 ;//To be used for Slave without GPS
	//dat[5] = 0x9 ;//Only setting Master->Slave
	//dat[5] = 0xB ;//Setting Master->Slave as well as asking for 1 sec messages
	dat[6] = 0x66 ;

	FT_Write(ft_handle,dat,7,&dwBytesWritten) ;
	FT_GetStatus(ft_handle,&R_Bytes,&T_Bytes,&EventDWord) ;
	//printf("%s: STATUS after Writing mode: \tT_Bytes=%d\tR_Bytes=%d\n",Device_Name,(int)T_Bytes,(int)R_Bytes) ;
	sleep(1);
}

void HISPARC_UNIT:: Ask_Control_Parameters()
{
	dat[0] = 0x99 ;	
	dat[1] = 0x55 ;	
	dat[2] = 0x66 ;	
	FT_Write(ft_handle,dat,3,&dwBytesWritten) ;
	sleep(1);
	FT_GetStatus(ft_handle,&R_Bytes,&T_Bytes,&EventDWord) ;
	printf("%s: STATUS after asking control parameters: \tT_Bytes=%d\tR_Bytes=%d\n",Device_Name,(int)T_Bytes,(int)R_Bytes) ;
}

void HISPARC_UNIT:: Open_Output_Files(char* filename)
{
	if(calibration==Off) out_handle=open(filename,O_CREAT|O_WRONLY,0444) ;
	if(calibration==On) fp=fopen(filename,"w") ;
}

int HISPARC_UNIT:: Send_Data_To_Socket()
{
	int total_send_bytes=0 ;	
	if(calibration==On)
	{
		read_data[0]=ADC_Offset[0] ;
		read_data[1]=ADC_Offset[1] ;
		read_data[2]=ADC_Offset[2] ;
		read_data[3]=ADC_Offset[3] ;
		read_data[4]=ADC_Gain[0] ;
		read_data[5]=ADC_Gain[1] ;
		read_data[6]=ADC_Gain[2] ;
		read_data[7]=ADC_Gain[3] ;
		read_data[8]=Common_Off ;
		total_send_bytes=write(sockfd,read_data,9) ;
		printf("ADC Offset + Gain + Common Offset: Bytes sent=%d\n",total_send_bytes) ;
	}
	
	if(calibration==Off)
	{
		FT_GetStatus(ft_handle,&R_Bytes,&T_Bytes,&EventDWord) ;
		printf("%s: STATUS after trigger: \tT_Bytes=%d\tR_Bytes=%d\n",Device_Name,(int)T_Bytes,(int)R_Bytes) ;
	
		if((int)R_Bytes>0)
		{
			FT_Read(ft_handle,dat_rev,R_Bytes,&dwBytesRead) ;
			printf("No. of Bytes read from %s = %d\n",Device_Name,(int)dwBytesRead) ;

			int total_left_bytes=(int)dwBytesRead ;
			while(total_left_bytes>0)
			{	
				//write(out_handle,dat_rev+total_send_bytes,total_left_bytes) ;
				send_bytes=write(sockfd,dat_rev+total_send_bytes,total_left_bytes) ;
				printf("Bytes sent=%d\n",send_bytes) ;
				if(send_bytes==-1) return send_bytes ;
				total_send_bytes=total_send_bytes+send_bytes ;
				total_left_bytes=total_left_bytes-send_bytes ;
			}
		}
	}
	return total_send_bytes ;
}

int HISPARC_UNIT:: Build_Calib_Event(unsigned char* buf,int bytes)
{
	unsigned char data[Event_Bytes] ;
	short unsigned a1,b1,a2,b2,a3,b3,data1,data2 ;	//For channel 1
	short unsigned A1,B1,A2,B2,A3,B3,DATA1,DATA2 ;	//For channel 2
	if(bytes>=Event_Bytes)
	{
		dat[0] = 0x99 ;
		dat[1] = 0x35 ;
		dat[2] = 0x0 ;
 		dat[3] = 0x0 ;
 		dat[4] = 0x0 ;
 		dat[5] = 0x0 ;
 		dat[6] = 0x66 ;
		FT_Write(ft_handle,dat,7,&dwBytesWritten) ;//Stop output

		int k ;
		float sum[4] ;
		memmove(data,buf,Event_Bytes) ;
		//float t=0 ;
		if(data[0]==0x99)
		{
			switch(data[1])
			{
				case 0xA0:
				k=0 ;
				for(int j=22;j<=5998+22;j+=3)
				{
					a1=(data[j]>>4)&0xf ;
					b1=data[j]&0xf ;
					a2=(data[j+1]>>4)&0xf ;
					data1=(a1<<8)+(b1<<4)+a2 ;
		
					b2=data[j+1]&0xf ;
					a3=(data[j+2]>>4)&0xf ;
					b3=data[j+2]&0xf ;
					data2=(b2<<8)+(a3<<4)+b3 ;
		
					A1=(data[j+6000]>>4)&0xf ;
					B1=data[j+6000]&0xf ;
					A2=(data[j+6000+1]>>4)&0xf ;
					DATA1=(A1<<8)+(B1<<4)+A2 ;
		
					B2=data[j+6000+1]&0xf ;
					A3=(data[j+6000+2]>>4)&0xf ;
					B3=data[j+6000+2]&0xf ;
					DATA2=(B2<<8)+(A3<<4)+B3 ;

					/*fprintf(fp,"%f\t%d\t%d\n",t,data1,DATA1) ;
					t=t+2.5 ;
					fprintf(fp,"%f\t%d\t%d\n",t,data2,DATA2) ;
					t=t+2.5 ;*/
					Channel_1_Counts[k]=data1 ;
					Channel_2_Counts[k]=DATA1 ;
					k++ ;

					Channel_1_Counts[k]=data2 ;
					Channel_2_Counts[k]=DATA2 ;
					k++ ;
				}
				//t=0 ;
				//fclose(fp) ;
				sum[0]=sum[1]=sum[2]=sum[3]=0 ;
				for(int j=0;j<4000;j=j+2)
				{
					sum[0]=sum[0]+Channel_1_Counts[j] ;	//Channel 1 offset +
					sum[1]=sum[1]+Channel_1_Counts[j+1] ;	//Channel 1 offset -
					sum[2]=sum[2]+Channel_2_Counts[j] ;	//Channel 2 offset +
					sum[3]=sum[3]+Channel_2_Counts[j+1] ;	//Channel 2 offset -
				}
				sum[0]=sum[0]/2000 ;
				sum[1]=sum[1]/2000 ;
				sum[2]=sum[2]/2000 ;
				sum[3]=sum[3]/2000 ;
				sum_av=(sum[0]+sum[1]+sum[2]+sum[3])/4 ; 
				printf("%7s: (Sum)[1+]=%5.1f [1-]=%5.1f [2+]=%5.1f [2-]=%5.1f av=%5.1f\t",Device_Name,sum[0],sum[1],sum[2],sum[3],sum_av) ;

				if(cal_type==0 && Full_Scale_done==0)
				{
					printf("%7s: Full scale adjust: =%d\n",Device_Name,Full_Scale) ;
					if(sum_av>ADC_Full_Scale_High) 
					{
						Full_Scale-- ;
						Full_Scale_done=0 ;
					}
					else
					{
						if(sum_av<ADC_Full_Scale_Low)
						{
							Full_Scale++ ;
							Full_Scale_done=0 ;
						}
						else Full_Scale_done=1 ;
					}
					if(Full_Scale_done==0) Set_Full_Scale(Full_Scale) ;
				}
				else
				{
					if(cal_type==0)
					{ 
						printf("%7s: Full scale adjust DONE: =%d\n",Device_Name,Full_Scale) ;
						return 1 ;
					}
				}

				if(Offset_done[0]+Offset_done[1]+Offset_done[2]+Offset_done[3]<4 && cal_type==1)
				{
					printf("*Offset* (%7s): [1+]=%d [1-]=%d [2+]=%d [2-]=%d\n",Device_Name,ADC_Offset[0],ADC_Offset[1],ADC_Offset[2],ADC_Offset[3]) ;
					for(int j=0;j<4;j++)
					{
						if(sum[j]>ADC_Off_High) 
						{
							ADC_Offset[j]++ ;
							Offset_done[j]=0 ;
						}
						else
						{
							if(sum[j]<ADC_Off_Low)
							{
								ADC_Offset[j]-- ;
								Offset_done[j]=0 ;
							}
							else Offset_done[j]=1 ;
						}
						if(Offset_done[j]==0)
						{	
							dat[0] = 0x99 ;
							dat[1] = 0x10+j ;
							dat[2]= ADC_Offset[j] ;
							dat[3] = 0x66 ;
							FT_Write(ft_handle,dat,4,&dwBytesWritten) ;
						}
					}
				}
				else
				{
					if(cal_type==1)
					{ 
						printf("%s: Offset DONE: [1+]=%d [1-]=%d [2+]=%d [2-]=%d\n",Device_Name,ADC_Offset[0],ADC_Offset[1],ADC_Offset[2],ADC_Offset[3]) ;
						return 1 ;
					}
				}

				if(cal_type==2 && Common_Off_done==0)
				{
					printf("%7s: Common Offset: =%d\n",Device_Name,Common_Off) ;
					if(sum_av>ADC_Common_Off_High) 
					{
						Common_Off-- ;
						Common_Off_done=0 ;
					}
					else
					{
						if(sum_av<ADC_Common_Off_Low)
						{
							Common_Off++ ;
							Common_Off_done=0 ;
						}
						else Common_Off_done=1 ;
					}
					if(Common_Off_done==0) Set_Common_Off(Common_Off) ;
				}
				else
				{
					if(cal_type==2)
					{ 
						printf("%7s: Common Offset DONE: =%d\n",Device_Name,Common_Off) ;
						return 1 ;
					}
				}

				if(Gain_done[0]+Gain_done[1]+Gain_done[2]+Gain_done[3]<4 && cal_type==3)
				{
					printf("*GAIN* (%7s): [1+]=%d [1-]=%d [2+]=%d [2-]=%d\n",Device_Name,ADC_Gain[0],ADC_Gain[1],ADC_Gain[2],ADC_Gain[3]) ;
					for(int j=0;j<4;j++)
					{
						if(sum[j]>ADC_Gain_High)
						{
							ADC_Gain[j]++ ;
							Gain_done[j]=0 ;
						}
						else
						{
							if(sum[j]<ADC_Gain_Low)
							{
								ADC_Gain[j]-- ;
								Gain_done[j]=0 ;
							}
							else Gain_done[j]=1 ;
						}
						if(Gain_done[j]==0)
						{	
							dat[0] = 0x99;
							dat[1] = 0x14+j;
							dat[2]= ADC_Gain[j];
							dat[3] = 0x66;
							FT_Write(ft_handle,dat,4,&dwBytesWritten) ;
						}
					}
				}		
				else
				{
					if(cal_type==3)
					{
						printf("%7s: Gain DONE: [1+]=%d [1-]=%d [2+]=%d [2-]=%d\n",Device_Name,ADC_Gain[0],ADC_Gain[1],ADC_Gain[2],ADC_Gain[3]) ;
						return 1 ;
					}
				}

				if(cal_type==4 && Common_Off_done==0)
				{
					printf("%7s: Final Common Offset: =%d\n",Device_Name,Common_Off) ;
					if(sum_av>Final_Common_Off_High) 
					{
						Common_Off-- ;
						Common_Off_done=0 ;
					}
					else
					{
						if(sum_av<Final_Common_Off_Low)
						{
							Common_Off++ ;
							Common_Off_done=0 ;
						}
						else Common_Off_done=1 ;
					}
					if(Common_Off_done==0) Set_Common_Off(Common_Off) ;
				}
				else
				{
					if(cal_type==4)
					{ 
						printf("%7s: Final Common Offset DONE: =%d\n",Device_Name,Common_Off) ;
						return 1 ;
					}
				}
				break ;
			
				default:
					break ;
			}
		}
	}
	return 0 ;
}

int HISPARC_UNIT:: Do_Calibration()
{
	FT_GetStatus(ft_handle,&R_Bytes,&T_Bytes,&EventDWord) ;
	int cal ;	
	if((int)R_Bytes>0)
	{
		FT_Read(ft_handle,dat_rev,R_Bytes,&dwBytesRead) ;
		//printf("No. of Bytes read from %s = %d\n",Device_Name,(int)dwBytesRead) ;
		cal=Build_Calib_Event(dat_rev,(int)dwBytesRead) ;
	}
	return cal ;
}

void HISPARC_UNIT:: Close_Output_Files()
{
	close(out_handle) ;
}

void HISPARC_UNIT:: Close_Hisparc_Unit()
{
	FT_Close(ft_handle) ;
}

void HISPARC_UNIT:: Close_Socket()
{
	Socket->Close_Socket() ;
}
