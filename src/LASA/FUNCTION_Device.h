#include <strstream>
#include <stdio.h> 
#include "Hisparc_Trigger_Condition.h"
#include "CLASS_Device.h"

void DEVICE:: Open_Socket()
{
	sockfd=Socket->Open_Socket(messages.PORT_NO) ;
	Socket->Bind_Socket() ;
	Socket->Listen_Socket() ;
}

int DEVICE:: Accept_Socket()
{
	newsockfd=Socket->Accept_Socket() ;
	printf("%s (%s): Connected\n",messages.SYS_NAME,messages.UNIT_STATE) ;
	return newsockfd ;
}

//void DEVICE:: Read_Message_File(FILE* fp,int col)
void DEVICE:: Read_Message_File(char* filename,int col)
{
	FILE *fp=fopen(filename,"r") ;
	while(fgets(message,200,fp)!=NULL)
	{
		if(strncmp(message,"//",strlen("//"))==0) continue ;
		//printf("%s",message) ;
		std::istrstream M (message) ;
		//M>>keyword>>value>>defn ;
		M>>keyword>>value1>>value2>>value3>>value4>>value5>>value6>>value7>>value8>>value9>>value10>>defn ;
		//printf("# %s\t%s\t%s\n",keyword,value,defn) ;
		if(col==1) strcpy(value,value1) ;
		if(col==2) strcpy(value,value2) ;
		if(col==3) strcpy(value,value3) ;
		if(col==4) strcpy(value,value4) ;
		if(col==5) strcpy(value,value5) ;
		if(col==6) strcpy(value,value6) ;
		if(col==7) strcpy(value,value7) ;
		if(col==8) strcpy(value,value8) ;
		if(col==9) strcpy(value,value9) ;
		if(col==10) strcpy(value,value10) ;

		if(strncmp(keyword,"SYS_NAME",strlen("SYS_NAME"))==0)
		{
			strcpy(messages.SYS_NAME,value) ;
			printf("* %s\n",messages.SYS_NAME) ;
		}
		if(strncmp(keyword,"UNIT_STATE",strlen("UNIT_STATE"))==0)
		{
			strcpy(messages.UNIT_STATE,value) ;
			printf("* %s\n",messages.UNIT_STATE) ;
		}
		if(strncmp(keyword,"UNIT_SERIAL_NO",strlen("UNIT_SERIAL_NO"))==0)
		{
			strcpy(messages.UNIT_SERIAL_NO,value) ;
			printf("* %s\n",messages.UNIT_SERIAL_NO) ;
		}
		if(strncmp(keyword,"PORT_NO",strlen("PORT_NO"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.PORT_NO=VALUE ;
			printf("* PORT_NO=%d\n",messages.PORT_NO) ;
		}
		if(strncmp(keyword,"CHANNEL_1_OFFSET+",strlen("CHANNEL_1_OFFSET+"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[0]=VALUE ;
			//printf("CHANNEL_1_OFFSET+=%d\n",messages.Control_Parameters[0]) ;
		}
		if(strncmp(keyword,"CHANNEL_1_OFFSET-",strlen("CHANNEL_1_OFFSET-"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[1]=VALUE ;
			//printf("CHANNEL_1_OFFSET-=%d\n",messages.Control_Parameters[1]) ;
		}
		if(strncmp(keyword,"CHANNEL_2_OFFSET+",strlen("CHANNEL_2_OFFSET+"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[2]=VALUE ;
			//printf("CHANNEL_2_OFFSET+=%d\n",messages.Control_Parameters[2]) ;
		}
		if(strncmp(keyword,"CHANNEL_2_OFFSET-",strlen("CHANNEL_2_OFFSET-"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[3]=VALUE ;
			//printf("CHANNEL_2_OFFSET-=%d\n",messages.Control_Parameters[3]) ;
		}
		if(strncmp(keyword,"CHANNEL_1_GAIN+",strlen("CHANNEL_1_GAIN+"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[4]=VALUE ;
			//printf("CHANNEL_1_GAIN+=%d\n",messages.Control_Parameters[4]) ;
		}
		if(strncmp(keyword,"CHANNEL_1_GAIN-",strlen("CHANNEL_1_GAIN-"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[5]=VALUE ;
			//printf("CHANNEL_1_GAIN-=%d\n",messages.Control_Parameters[5]) ;
		}
		if(strncmp(keyword,"CHANNEL_2_GAIN+",strlen("CHANNEL_2_GAIN+"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[6]=VALUE ;
			//printf("CHANNEL_2_GAIN+=%d\n",messages.Control_Parameters[6]) ;
		}
		if(strncmp(keyword,"CHANNEL_2_GAIN-",strlen("CHANNEL_2_GAIN-"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[7]=VALUE ;
			//printf("CHANNEL_2_GAIN-=%d\n",messages.Control_Parameters[7]) ;
		}
		if(strncmp(keyword,"COMMON_OFFSET_ADJ",strlen("COMMON_OFFSET_ADJ"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[8]=VALUE ;
			//printf("COMMON_OFFSET_ADJ=%d\n",messages.Control_Parameters[8]) ;
		}
		if(strncmp(keyword,"FULL_SCALE_ADJ",strlen("FULL_SCALE_ADJ"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[9]=VALUE ;
			//printf("FULL_SCALE_ADJ=%d\n",messages.Control_Parameters[9]) ;
		}
		if(strncmp(keyword,"CHANNEL_1_INTE_TIME",strlen("CHANNEL_1_INTE_TIME"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[10]=VALUE ;
			//printf("CHANNEL_1_INTE_TIME=%d\n",messages.Control_Parameters[10]) ;
		}
		if(strncmp(keyword,"CHANNEL_2_INTE_TIME",strlen("CHANNEL_2_INTE_TIME"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[11]=VALUE ;
			//printf("CHANNEL_2_INTE_TIME=%d\n",messages.Control_Parameters[11]) ;
		}
		if(strncmp(keyword,"COMP_THRES_LOW",strlen("COMP_THRES_LOW"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[12]=VALUE ;
			//printf("COMP_THRES_LOW=%d\n",messages.Control_Parameters[12]) ;
		}
		if(strncmp(keyword,"COMP_THRES_HIGH",strlen("COMP_THRES_HIGH"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[13]=VALUE ;
			//printf("COMP_THRES_HIGH=%d\n",messages.Control_Parameters[13]) ;
		}
		if(strncmp(keyword,"CHANNEL_1_HV",strlen("CHANNEL_1_HV"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			if(VALUE>HV_Upp_Limit)
			{
				printf("Supplied voltage exceed Maximum voltage (%d)\n",HV_Upp_Limit) ;
				printf("Voltage set to Maximum voltage (%d)\n",HV_Upp_Limit) ;
				VALUE=HV_Upp_Limit ;
			}
			messages.Control_Parameters[14]=VALUE ;
			//printf("CHANNEL_1_HV=%d\n",messages.Control_Parameters[14]) ;
		}
		if(strncmp(keyword,"CHANNEL_2_HV",strlen("CHANNEL_2_HV"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			if(VALUE>HV_Upp_Limit)
			{
				printf("Supplied voltage exceed Maximum voltage (%d)\n",HV_Upp_Limit) ;
				printf("Voltage set to Maximum voltage (%d)\n",HV_Upp_Limit) ;
				VALUE=HV_Upp_Limit ;
			}
			messages.Control_Parameters[15]=VALUE ;
			//printf("CHANNEL_2_HV=%d\n",messages.Control_Parameters[15]) ;
		}
		if(strncmp(keyword,"CHANNEL_1_THRES_LOW",strlen("CHANNEL_1_THRES_LOW"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[16]=VALUE ;
			//printf("CHANNEL_1_THRES_LOW=%d\n",messages.Control_Parameters[16]) ;
		}
		if(strncmp(keyword,"CHANNEL_1_THRES_HIGH",strlen("CHANNEL_1_THRES_HIGH"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[17]=VALUE ;
			//printf("CHANNEL_1_THRES_HIGH=%d\n",messages.Control_Parameters[17]) ;
		}
		if(strncmp(keyword,"CHANNEL_2_THRES_LOW",strlen("CHANNEL_2_THRES_LOW"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[18]=VALUE ;
			//printf("CHANNEL_2_THRES_LOW=%d\n",messages.Control_Parameters[18]) ;
		}
		if(strncmp(keyword,"CHANNEL_2_THRES_HIGH",strlen("CHANNEL_2_THRES_HIGH"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[19]=VALUE ;
			//printf("CHANNEL_2_THRES_HIGH=%d\n",messages.Control_Parameters[19]) ;
		}
		if(strncmp(keyword,"TRIGG_CONDITION",strlen("TRIGG_CONDITION"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[20]=VALUE ;
			//printf("TRIGG_CONDITION=%d\n",messages.Control_Parameters[20]) ;
		}
		if(strncmp(keyword,"PRE_COIN_TIME",strlen("PRE_COIN_TIME"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[21]=VALUE ;
			//printf("PRE_COIN_TIME=%d\n",messages.Control_Parameters[21]) ;
		}
		if(strncmp(keyword,"COIN_TIME",strlen("COIN_TIME"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[22]=VALUE ;
			//printf("COIN_TIME=%d\n",messages.Control_Parameters[22]) ;
		}
		if(strncmp(keyword,"POST_COIN_TIME",strlen("POST_COIN_TIME"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[23]=VALUE ;
			//printf("POST_COIN_TIME=%d\n",messages.Control_Parameters[23]) ;
		}
		if(strncmp(keyword,"LOG_BOOK",strlen("LOG_BOOK"))==0)
		{
			std::istrstream V(value) ;
			V>>VALUE ;
			messages.Control_Parameters[24]=VALUE ;
			//printf("LOG_BOOK=%d\n",messages.Control_Parameters[24]) ;
		}
		strcpy(value,"") ;	
	}
	fclose(fp) ;	
}

void DEVICE:: Build_Hisparc_Messages() 
{
	Control_Messages[0]=(messages.Control_Parameters[0]) & 0x00ff ;
	Control_Messages[1]=(messages.Control_Parameters[1]) & 0x00ff ;
	Control_Messages[2]=(messages.Control_Parameters[2]) & 0x00ff ;
	Control_Messages[3]=(messages.Control_Parameters[3]) & 0x00ff ;
	Control_Messages[4]=(messages.Control_Parameters[4]) & 0x00ff ;
	Control_Messages[5]=(messages.Control_Parameters[5]) & 0x00ff ;
	Control_Messages[6]=(messages.Control_Parameters[6]) & 0x00ff ;
	Control_Messages[7]=(messages.Control_Parameters[7]) & 0x00ff ;
	Control_Messages[8]=(messages.Control_Parameters[8]) & 0x00ff ;
	Control_Messages[9]=(messages.Control_Parameters[9]) & 0x00ff ;
	Control_Messages[10]=(messages.Control_Parameters[10]) & 0x00ff ;
	Control_Messages[11]=(messages.Control_Parameters[11]) & 0x00ff ;
	Control_Messages[12]=(messages.Control_Parameters[12]) & 0x00ff ;
	Control_Messages[13]=(messages.Control_Parameters[13]) & 0x00ff ;
	Control_Messages[14]=(unsigned short)((messages.Control_Parameters[14]+34.0714)/7.93155) & 0x00ff ;
	Control_Messages[15]=(unsigned short)((messages.Control_Parameters[15]+34.0714)/7.93155) & 0x00ff ;
	Control_Messages[16]=((unsigned short)(messages.Control_Parameters[16]/0.4883) & 0xff00)>>8 ;
	Control_Messages[17]=((unsigned short)(messages.Control_Parameters[16]/0.4883) & 0x00ff) ;
	Control_Messages[18]=((unsigned short)(messages.Control_Parameters[17]/0.4883) & 0xff00)>>8 ;
	Control_Messages[19]=((unsigned short)(messages.Control_Parameters[17]/0.4883) & 0x00ff) ;
	Control_Messages[20]=((unsigned short)(messages.Control_Parameters[18]/0.4883) & 0xff00)>>8 ;
	Control_Messages[21]=((unsigned short)(messages.Control_Parameters[18]/0.4883) & 0x00ff) ;
	Control_Messages[22]=((unsigned short)(messages.Control_Parameters[19]/0.4883) & 0xff00)>>8 ;
	Control_Messages[23]=((unsigned short)(messages.Control_Parameters[19]/0.4883) & 0x00ff) ;
	Control_Messages[24]=Trigger_Condition(messages.Control_Parameters[20]) ;

	unsigned short Pre_Coin_Time=(unsigned short)(messages.Control_Parameters[21]/5) ;	//no. of 5ns steps
	Control_Messages[25]=(Pre_Coin_Time & 0xff00)>>8 ;	
	Control_Messages[26]=(Pre_Coin_Time & 0x00ff) ;	

	unsigned short Coin_Time=(unsigned short)(messages.Control_Parameters[22]/5) ;		//no. of 5ns steps
	Control_Messages[27]=(Coin_Time & 0xff00)>>8 ;	
	Control_Messages[28]=(Coin_Time & 0x00ff) ;	
	
	unsigned short Post_Coin_Time=(unsigned short)(messages.Control_Parameters[23]/5) ;	//no. of 5ns steps
	Control_Messages[29]=(Post_Coin_Time & 0xff00)>>8 ;	
	Control_Messages[30]=(Post_Coin_Time & 0x00ff) ;

	Control_Messages[31]=((unsigned short)(messages.Control_Parameters[24]) & 0xff00)>>8 ;
	Control_Messages[32]=((unsigned short)(messages.Control_Parameters[24]) & 0x00ff) ;
	/*for(int i=0;i<=40;i++)
		printf("Message[%d]=0x%x\t%d\n",i,Control_Messages[i],Control_Messages[i]) ;*/
}

/*void DEVICE:: Open_Output_File(char* filename)
{
	out_handle=open(filename,O_CREAT|O_WRONLY,0444) ;
}*/

void DEVICE:: Send_Messages()
{
	if(Calibration==On)
	{
		unsigned char dat[4] ;
		dat[0]=0x99 ;
		dat[1]=0x30 ;
		dat[2]=0x80 ;
		dat[3]=0x66 ;
		int bytes_send=write(newsockfd,dat,4) ;
		printf("%s (%s): Bytes send=%d\n",messages.SYS_NAME,messages.UNIT_STATE,bytes_send) ;
	}
	if(Calibration==Off)
	{
		int bytes_send=write(newsockfd,Control_Messages,40) ;
		printf("%s (%s): Bytes send=%d\n",messages.SYS_NAME,messages.UNIT_STATE,bytes_send) ;
	}
}

void DEVICE:: Initialise_Variables()
{
	handle_messages.pointer_sec_message=0 ;
	handle_messages.pointer_data_message=0 ;
	handle_messages.pointer_control_parameters=0 ;
	handle_messages.sec_no=0 ;
	handle_messages.event_no=0 ;
	handle_messages.waiting=0 ;
	monitoring_1=1 ;
	monitoring_2=1 ;
	counter_traces_1=1 ;
	counter_traces_2=1 ;

	for(int i=0;i<NO_Event;i++)
		memset(handle_messages.Event_Message[i],0,sizeof(handle_messages.Event_Message[i])) ;
}

void DEVICE:: Get_Sec_Time(int no,int Lasa,int flag)		//Flag=0(slave) or 1(master)
{
	one_sec_time[no].sec=handle_messages.One_Sec_Message[no][8] ;
	one_sec_time[no].min=handle_messages.One_Sec_Message[no][7] ;
	one_sec_time[no].hour=handle_messages.One_Sec_Message[no][6] ;
	one_sec_time[no].day=handle_messages.One_Sec_Message[no][2] ;
	one_sec_time[no].month=handle_messages.One_Sec_Message[no][3] ;
	one_sec_time[no].year=(handle_messages.One_Sec_Message[no][4]<<8)+handle_messages.One_Sec_Message[no][5] ;
	one_sec_time[no].sync=(handle_messages.One_Sec_Message[no][9]>>7)&0x1 ;
	one_sec_time[no].CTP=((handle_messages.One_Sec_Message[no][9]&0x7f)<<24)+(handle_messages.One_Sec_Message[no][10]<<16)+(handle_messages.One_Sec_Message[no][11]<<8)+handle_messages.One_Sec_Message[no][12] ;

	current_CTP=one_sec_time[no].CTP ;
	if(time0.year==0)	//Initialising start time for the RUN:-> the 1st one sec message time
	{
		time0.sec=one_sec_time[no].sec ;
		time0.min=one_sec_time[no].min ;
		time0.hour=one_sec_time[no].hour ;
		time0.day=one_sec_time[no].day ;
		time0.month=one_sec_time[no].month ;
		time0.year=one_sec_time[no].year ;
	}
	
	unsigned char buf[4] ;
	for(int i=0;i<=3;i++)
		buf[i]=handle_messages.One_Sec_Message[no][16-i] ;
	one_sec_time[no].quant=*(float*)buf ;

	printf("one sec[%2d] %d/%d/%d\t%d:%d:%d\tsync=%d\tCTP=%lu\tquant=%f\n",no,one_sec_time[no].day,one_sec_time[no].month,one_sec_time[no].year,one_sec_time[no].hour,one_sec_time[no].min,one_sec_time[no].sec,one_sec_time[no].sync,one_sec_time[no].CTP,one_sec_time[no].quant) ;
	
	if(no>=(Store_One_Sec-1) && Lasa==0 && flag==1) Store_Sec_Data() ;
}

void DEVICE:: Store_Sec_Messages(int Lasa)
{
	struct tm t ;
	t.tm_sec=one_sec_time[counter_sec].sec ;
	t.tm_min=one_sec_time[counter_sec].min ;
	t.tm_hour=one_sec_time[counter_sec].hour ;
	t.tm_mday=one_sec_time[counter_sec].day ;
	t.tm_mon=one_sec_time[counter_sec].month ;
	t.tm_year=one_sec_time[counter_sec].year-1900 ;
	sec_data[Lasa].YMD=(t.tm_year+1900)*10000+t.tm_mon*100+t.tm_mday ;
	sec_data[Lasa].GPS_time_stamp=(unsigned int)timegm(&t) ;
	sec_data[Lasa].sync=one_sec_time[counter_sec].sync ;
	sec_data[Lasa].CTP=one_sec_time[counter_sec].CTP ;
	sec_data[Lasa].quant=one_sec_time[counter_sec].quant ;
	sec_data[Lasa].Channel_1_Thres_count_high=(handle_messages.One_Sec_Message[counter_sec][17]<<8)+handle_messages.One_Sec_Message[counter_sec][18] ;
	sec_data[Lasa].Channel_1_Thres_count_low=(handle_messages.One_Sec_Message[counter_sec][19]<<8)+handle_messages.One_Sec_Message[counter_sec][20] ;
	sec_data[Lasa].Channel_2_Thres_count_high=(handle_messages.One_Sec_Message[counter_sec][21]<<8)+handle_messages.One_Sec_Message[counter_sec][22] ;
	sec_data[Lasa].Channel_2_Thres_count_low=(handle_messages.One_Sec_Message[counter_sec][23]<<8)+handle_messages.One_Sec_Message[counter_sec][24] ;
	memmove(sec_data[Lasa].Satellite_info,handle_messages.One_Sec_Message[counter_sec]+25,61) ;
}

void DEVICE:: Store_Control_Parameters(int Lasa,int flag)	//Flag=0(slave) or 1(master)
{
	struct tm* t ;
	time_t tt ;
	tt=time(NULL) ;
	t=gmtime(&tt) ;
	control_parameters[0].YMD=(t->tm_year+1900)*10000+t->tm_mon*100+t->tm_mday ;
	control_parameters[1].YMD=(t->tm_year+1900)*10000+t->tm_mon*100+t->tm_mday ;
	control_parameters[0].HMS=t->tm_hour*10000+t->tm_min*100+t->tm_sec ;
	control_parameters[1].HMS=t->tm_hour*10000+t->tm_min*100+t->tm_sec ;
	control_parameters[0].Time_stamp=(unsigned int)timegm(t) ;
	control_parameters[1].Time_stamp=(unsigned int)timegm(t) ;
	control_parameters[0].Channel_offset_pos=handle_messages.Control_Parameters[2] ;
	control_parameters[0].Channel_offset_neg=handle_messages.Control_Parameters[3] ;	
	control_parameters[1].Channel_offset_pos=handle_messages.Control_Parameters[4] ;	
	control_parameters[1].Channel_offset_neg=handle_messages.Control_Parameters[5] ;	
	control_parameters[0].Channel_gain_pos=handle_messages.Control_Parameters[6] ;	
	control_parameters[0].Channel_gain_neg=handle_messages.Control_Parameters[7] ;	
	control_parameters[1].Channel_gain_pos=handle_messages.Control_Parameters[8] ;	
	control_parameters[1].Channel_gain_neg=handle_messages.Control_Parameters[9] ;	
	control_parameters[0].Common_offset_adj=handle_messages.Control_Parameters[10] ;	
	control_parameters[1].Common_offset_adj=handle_messages.Control_Parameters[10] ;	
	control_parameters[0].Full_scale_adj=handle_messages.Control_Parameters[11] ;	
	control_parameters[1].Full_scale_adj=handle_messages.Control_Parameters[11] ;	
	control_parameters[0].Channel_inte_time=handle_messages.Control_Parameters[12] ;	
	control_parameters[1].Channel_inte_time=handle_messages.Control_Parameters[13] ;	
	control_parameters[0].Comp_thres_low=handle_messages.Control_Parameters[14] ;	
	control_parameters[1].Comp_thres_low=handle_messages.Control_Parameters[14] ;	
	control_parameters[0].Comp_thres_high=handle_messages.Control_Parameters[15] ;	
	control_parameters[1].Comp_thres_high=handle_messages.Control_Parameters[15] ;	
	control_parameters[0].Channel_HV=(unsigned short)(handle_messages.Control_Parameters[16]*7.93155-34.0714) ;
	control_parameters[1].Channel_HV=(unsigned short)(handle_messages.Control_Parameters[17]*7.93155-34.0714) ;
	control_parameters[0].Channel_thres_low=(unsigned short)(((handle_messages.Control_Parameters[18]<<8)+handle_messages.Control_Parameters[19])*0.4883) ;	
	control_parameters[0].Channel_thres_high=(unsigned short)(((handle_messages.Control_Parameters[20]<<8)+handle_messages.Control_Parameters[21])*0.4883) ;	
	control_parameters[1].Channel_thres_low=(unsigned short)(((handle_messages.Control_Parameters[22]<<8)+handle_messages.Control_Parameters[23])*0.4883) ;	
	control_parameters[1].Channel_thres_high=(unsigned short)(((handle_messages.Control_Parameters[24]<<8)+handle_messages.Control_Parameters[25])*0.4883) ;	
	control_parameters[0].Trigg_condition=handle_messages.Control_Parameters[26] ;	
	control_parameters[1].Trigg_condition=handle_messages.Control_Parameters[26] ;	
	control_parameters[0].Pre_coin_time=((handle_messages.Control_Parameters[27]<<8)+handle_messages.Control_Parameters[28])*5 ;	
	control_parameters[1].Pre_coin_time=((handle_messages.Control_Parameters[27]<<8)+handle_messages.Control_Parameters[28])*5 ;	
	control_parameters[0].Coin_time=(unsigned short)(((handle_messages.Control_Parameters[29]<<8)+handle_messages.Control_Parameters[30])*5) ;	
	control_parameters[1].Coin_time=(unsigned short)(((handle_messages.Control_Parameters[29]<<8)+handle_messages.Control_Parameters[30])*5) ;	
	control_parameters[0].Post_coin_time=(unsigned short)(((handle_messages.Control_Parameters[31]<<8)+handle_messages.Control_Parameters[32])*5) ;
	control_parameters[1].Post_coin_time=(unsigned short)(((handle_messages.Control_Parameters[31]<<8)+handle_messages.Control_Parameters[32])*5) ;

	if(flag==0)
	{
		control_parameters[0].Detector=(unsigned int)(4*Lasa+3) ;
		control_parameters[1].Detector=(unsigned int)(4*Lasa+4) ;
		memcpy(&log_book[Lasa].slave[0],&control_parameters[0],sizeof(control_parameters[0])) ;
		memcpy(&log_book[Lasa].slave[1],&control_parameters[1],sizeof(control_parameters[1])) ;
	}
	if(flag==1)
	{
		control_parameters[0].Detector=(unsigned int)(4*Lasa+1) ;
		control_parameters[1].Detector=(unsigned int)(4*Lasa+2) ;
		memcpy(&log_book[Lasa].master[0],&control_parameters[0],sizeof(control_parameters[0])) ;
		memcpy(&log_book[Lasa].master[1],&control_parameters[1],sizeof(control_parameters[1])) ;
	}
	if(counter_log==(NO_LASAs*2)-1)
	{
		Store_Control_Data() ;
		counter_log=0 ;
	}
	counter_log++ ;
}

void DEVICE:: Get_Event_Time(int no)
{
	handle_messages.event_time[no].sec=handle_messages.Event_Message[no][17] ;
	handle_messages.event_time[no].min=handle_messages.Event_Message[no][16] ;
	handle_messages.event_time[no].hour=handle_messages.Event_Message[no][15] ;
	handle_messages.event_time[no].day=handle_messages.Event_Message[no][11] ;
	handle_messages.event_time[no].month=handle_messages.Event_Message[no][12] ;
	handle_messages.event_time[no].year=(handle_messages.Event_Message[no][13]<<8)+handle_messages.Event_Message[no][14] ;
	handle_messages.event_time[no].CTD=(handle_messages.Event_Message[no][18]<<24)+(handle_messages.Event_Message[no][19]<<16)+(handle_messages.Event_Message[no][20]<<8)+handle_messages.Event_Message[no][21] ;
	handle_messages.event_time[no].nsec=(long unsigned int)((1.0*handle_messages.event_time[no].CTD/current_CTP)*(1000000000)) ;//This relation is not exact
	//--------Event time stamp in nanosecs (w.r.t time0 and is used only for checking the coincidences)--------
	//The calculation below assumes:-> 1 year=365 days; 1 month=30 days; 1 day=24 hrs; 1 hour=60 mins; 1 min=60 sec  
	handle_messages.event_time[no].time_stamp=(long long unsigned)(((handle_messages.event_time[no].year-time0.year)*31536000+(handle_messages.event_time[no].month-time0.month)*2592000+(handle_messages.event_time[no].day-time0.day)*86400+(handle_messages.event_time[no].hour-time0.hour)*3600+(handle_messages.event_time[no].min-time0.min)*60+(handle_messages.event_time[no].sec-time0.sec))*(1000000000.0)+handle_messages.event_time[no].nsec) ;
	//-----------xxx--------------
	
	//printf("event [%2d] %d/%d/%d\t%d:%d:%d\tCTD=%lu\tnsec=%lu\ttime=%lf",no,handle_messages.event_time[no].day,handle_messages.event_time[no].month,handle_messages.event_time[no].year,handle_messages.event_time[no].hour,handle_messages.event_time[no].min,handle_messages.event_time[no].sec,handle_messages.event_time[no].CTD,handle_messages.event_time[no].nsec,handle_messages.event_time[no].time_stamp) ;
	printf("event [%2d] %d/%d/%d\t%d:%d:%d CTD=%u ns=%u ",no,handle_messages.event_time[no].day,handle_messages.event_time[no].month,handle_messages.event_time[no].year,handle_messages.event_time[no].hour,handle_messages.event_time[no].min,handle_messages.event_time[no].sec,handle_messages.event_time[no].CTD,handle_messages.event_time[no].nsec) ;
}

int DEVICE:: Data_Processing(unsigned char* buf,int bytes,int lasa,int flag)	//Flag=0(slave) or 1(master) 
{
	int pointer=0 ;
	int tot_bytes_left=bytes ;
	while(tot_bytes_left>0)
	{
		if(buf[pointer]==0x99 && handle_messages.waiting==0)	//If the data starts with a header & "waiting=0"
		{
			//printf("\tHeader detected\n") ;
			if(tot_bytes_left==1) return 1 ;
			switch(buf[pointer+1])
			{
			case 0xA4:	//1 sec message
				if(tot_bytes_left<One_Sec_Bytes)
				{
					memmove(handle_messages.One_Sec_Message[handle_messages.sec_no]+handle_messages.pointer_sec_message,buf+pointer,tot_bytes_left) ;
					//printf("\tCase Header:1 sec message: No. of bytes read=%d\n",tot_bytes_left) ;
					handle_messages.pointer_sec_message=tot_bytes_left ;
					tot_bytes_left=0 ;
					handle_messages.waiting=1 ;
					//printf("\tCase Header: waiting=%2d\n",handle_messages.waiting) ;
				}
				else
				{
					memmove(handle_messages.One_Sec_Message[handle_messages.sec_no],buf+pointer,One_Sec_Bytes) ;
					Get_Sec_Time(handle_messages.sec_no,lasa,flag) ;
					
					//printf("\tCase Header: 1 sec message: No. of bytes read=%d\n",One_Sec_Bytes) ;
					tot_bytes_left=tot_bytes_left-One_Sec_Bytes ;
					pointer=pointer+One_Sec_Bytes ;
					if(handle_messages.sec_no==(NO_One_Sec-1)) handle_messages.sec_no=0 ;
					else handle_messages.sec_no++ ;
					handle_messages.waiting=0 ;
				}
				break ;

			case 0xA0:	//Event data
				if(tot_bytes_left<Event_Bytes)
				{
					memmove(handle_messages.Event_Message[handle_messages.event_no]+handle_messages.pointer_data_message,buf+pointer,tot_bytes_left) ;
					//printf("\tCase Header: Event data: No. of bytes read=%d\n",tot_bytes_left) ;
					handle_messages.pointer_data_message=tot_bytes_left ;
					tot_bytes_left=0 ;
					handle_messages.waiting=2 ;
					//printf("\tCase Header: waiting=%d\n",handle_messages.waiting) ;
				}
				else
				{
					memmove(handle_messages.Event_Message[handle_messages.event_no],buf+pointer,Event_Bytes) ;
					//Get_Event_Time(handle_messages.event_no) ;

					//if(flag==0) printf("\n") ;
					if(flag==1)
					{
						Get_Event_Time(handle_messages.event_no) ;
						array_event_times[pointer_event].lasa=lasa ;
						array_event_times[pointer_event].event_no=handle_messages.event_no ;
						array_event_times[pointer_event].time_stamp=handle_messages.event_time[handle_messages.event_no].time_stamp ;
						printf(" Ev_no=%d\tlasa=%d\ttime=%llu\n",pointer_event,array_event_times[pointer_event].lasa,array_event_times[pointer_event].time_stamp) ;

						if((((counter*Max_Event)+pointer_event+1)/Spy_Event)*Spy_Event==((counter*Max_Event)+pointer+1))
						{
							printf("Spying events\n") ;
							Sort_Event_Times(pointer_event+1-(Spy_Event+Spy_Delay)) ;
							Check_Coincidence() ;
						}
						if(pointer_event==(Max_Event-1))
						{
							counter++ ;
							pointer_event=0 ;
						}
						else pointer_event++ ;
					}
					//printf("\tCase Header: Event data: No. of bytes read=%d\n",Event_Bytes) ;
					tot_bytes_left=tot_bytes_left-Event_Bytes ;
					pointer=pointer+Event_Bytes ;
					if(handle_messages.event_no==(NO_Event-1)) handle_messages.event_no=0 ;
					else handle_messages.event_no++ ;
					handle_messages.waiting=0 ;
				}
				break ;

			case 0x55:	//Control parameters
				if(tot_bytes_left<Control_Bytes)
				{
					memmove(handle_messages.Control_Parameters+handle_messages.pointer_control_parameters,buf+pointer,tot_bytes_left) ;
					//printf("\tCase Header: Control parameters: No. of bytes read=%d\n",tot_bytes_left) ;
					handle_messages.pointer_control_parameters=tot_bytes_left ;
					tot_bytes_left=0 ;
					handle_messages.waiting=3 ;
					//printf("\tCase Header: waiting=%d\n",handle_messages.waiting) ;
				}
				else
				{
					memmove(handle_messages.Control_Parameters,buf+pointer,Control_Bytes) ;
					Store_Control_Parameters(lasa,flag) ;
					//printf("\tCase Header: Control parameters: No. of bytes read=%d\tlasa=%d\tflag=%d\n",Control_Bytes,lasa,flag) ;
					tot_bytes_left=tot_bytes_left-Control_Bytes ;
					pointer=pointer+Control_Bytes ;
					handle_messages.waiting=0 ;
				}
				break ;

			default:	//Others
				int i=0 ;
				while(i<tot_bytes_left)
				{
					pointer++ ;
					if(buf[pointer]==0x99) break ;
					i++ ;
				}
				tot_bytes_left=tot_bytes_left-i ;
				break ;	
			}
		}
		else	//If the data does NOT start with a header or with a header and "waiting!=0"
		{
			switch(handle_messages.waiting)
			{
			case 1:		//1 sec message
				if(tot_bytes_left<(One_Sec_Bytes-handle_messages.pointer_sec_message))
				{
					memmove(handle_messages.One_Sec_Message[handle_messages.sec_no]+handle_messages.pointer_sec_message,buf+pointer,tot_bytes_left) ;
					//printf("1 sec message: No. of bytes read=%d\n",tot_bytes_left) ;
					handle_messages.pointer_sec_message=handle_messages.pointer_sec_message+tot_bytes_left ;
					tot_bytes_left=0 ;
					handle_messages.waiting=1 ;
					//printf("\twaiting=%2d\n",handle_messages.waiting) ;
				}
				else
				{
					memmove(handle_messages.One_Sec_Message[handle_messages.sec_no]+handle_messages.pointer_sec_message,buf+pointer,One_Sec_Bytes-handle_messages.pointer_sec_message) ;
					Get_Sec_Time(handle_messages.sec_no,lasa,flag) ;
					//printf("\t1 sec message: No. of bytes read=%d\n",One_Sec_Bytes-(handle_messages.pointer_sec_message)) ;
					tot_bytes_left=tot_bytes_left-One_Sec_Bytes+(handle_messages.pointer_sec_message) ;
					pointer=pointer+One_Sec_Bytes-(handle_messages.pointer_sec_message) ;
					handle_messages.pointer_sec_message=0 ;
					if(handle_messages.sec_no==(NO_One_Sec-1)) handle_messages.sec_no=0 ;
					else handle_messages.sec_no++ ;
					handle_messages.waiting=0 ;
				}
				break ;

			case 2:		//Event data
				if(tot_bytes_left<(Event_Bytes-handle_messages.pointer_data_message))
				{
					memmove(handle_messages.Event_Message[handle_messages.event_no]+handle_messages.pointer_data_message,buf+pointer,tot_bytes_left) ;
					//printf("\tEvent data: No. of bytes read=%d\n",tot_bytes_left) ;
					handle_messages.pointer_data_message=handle_messages.pointer_data_message+tot_bytes_left ;
					tot_bytes_left=0 ;
					handle_messages.waiting=2 ;
					//printf("\twaiting=%2d\n",handle_messages.waiting) ;
				}
				else
				{
					memmove(handle_messages.Event_Message[handle_messages.event_no]+handle_messages.pointer_data_message,buf+pointer,Event_Bytes-handle_messages.pointer_data_message) ;
					//Get_Event_Time(handle_messages.event_no) ;

					//if(flag==0) printf("\n") ;
					if(flag==1)
					{
						Get_Event_Time(handle_messages.event_no) ;
						array_event_times[pointer_event].lasa=lasa ;
						array_event_times[pointer_event].event_no=handle_messages.event_no ;
						array_event_times[pointer_event].time_stamp=handle_messages.event_time[handle_messages.event_no].time_stamp ;
						printf(" Ev_no=%d\tlasa=%d\ttime=%llu\n",pointer_event,array_event_times[pointer_event].lasa,array_event_times[pointer_event].time_stamp) ;

						if((((counter*Max_Event)+pointer_event+1)/Spy_Event)*Spy_Event==((counter*Max_Event)+pointer_event+1))
						{
							printf("spy event\n") ;
							Sort_Event_Times(pointer_event+1-(Spy_Event+Spy_Delay)) ;
							Check_Coincidence() ;
						}
						if(pointer_event==(Max_Event-1))
						{
							counter++ ;
							pointer_event=0 ;
						}
						else pointer_event++ ;
					}
					//printf("\tEvent data: No. of bytes read=%d\n",Event_Bytes-(handle_messages.pointer_data_message)) ;
					tot_bytes_left=tot_bytes_left-Event_Bytes+(handle_messages.pointer_data_message) ;
					pointer=pointer+Event_Bytes-(handle_messages.pointer_data_message) ;
					handle_messages.pointer_data_message=0 ;
					if(handle_messages.event_no==(NO_Event-1)) handle_messages.event_no=0 ;
					else handle_messages.event_no++ ;
					handle_messages.waiting=0 ;
				}
				break ;

			case 3:		//Control parameters
				if(tot_bytes_left<(Control_Bytes-handle_messages.pointer_control_parameters))
				{
					memmove(handle_messages.Control_Parameters+handle_messages.pointer_control_parameters,buf+pointer,tot_bytes_left) ;
					//printf("\tControl parameters: No. of bytes read=%d\n",tot_bytes_left) ;
					handle_messages.pointer_control_parameters=handle_messages.pointer_control_parameters+tot_bytes_left ;
					tot_bytes_left=0 ;
					handle_messages.waiting=3 ;
				}
				else
				{
					memmove(handle_messages.Control_Parameters+handle_messages.pointer_control_parameters,buf+pointer,Control_Bytes-handle_messages.pointer_control_parameters) ;
					Store_Control_Parameters(lasa,flag) ;
					//printf("\tControl parameters: No. of bytes read=%d\n",Control_Bytes-(handle_messages.pointer_control_parameters)) ;
					tot_bytes_left=tot_bytes_left-Control_Bytes+(handle_messages.pointer_control_parameters) ;
					pointer=pointer+Control_Bytes-(handle_messages.pointer_control_parameters) ;
					handle_messages.pointer_control_parameters=0 ;
					handle_messages.waiting=0 ;
				}
				break ;
		
			default:	//Others
				int i=0 ;
				while(i<tot_bytes_left)
				{
					if(buf[pointer]==0x99) break ;
					pointer++ ;
					i++ ;
				}
				tot_bytes_left=tot_bytes_left-i ;
				break ;
			}
		}
	}
	return 0 ;
}

void DEVICE:: Build_Final_Events(int Lasa,int flag,int no)	//flag=1(master) or 0(slave)
{
	if(flag==1)
	{
		struct tm t ;
		t.tm_sec=handle_messages.event_time[no].sec ;
		t.tm_min=handle_messages.event_time[no].min ;
		t.tm_hour=handle_messages.event_time[no].hour ;
		t.tm_mday=handle_messages.event_time[no].day ;
		t.tm_mon=handle_messages.event_time[no].month ;
		t.tm_year=handle_messages.event_time[no].year-1900 ;
		event_time[Lasa].YMD=(t.tm_year+1900)*10000+t.tm_mon*100+t.tm_mday ;
		event_time[Lasa].GPS_time_stamp=(unsigned int)timegm(&t) ;
		event_time[Lasa].CTD=handle_messages.event_time[no].CTD ;
		event_time[Lasa].nsec=handle_messages.event_time[no].nsec ;
		for(int j=0;j<=1;j++)
		{
			final_data[Lasa].master[j].YMD=event_time[Lasa].YMD ;
			final_data[Lasa].master[j].GPS_time_stamp=event_time[Lasa].GPS_time_stamp ;
			final_data[Lasa].master[j].CTD=event_time[Lasa].CTD ;
			final_data[Lasa].master[j].nsec=event_time[Lasa].nsec ;
			final_data[Lasa].master[j].Trigg_pattern=(handle_messages.Event_Message[no][3]<<8)+handle_messages.Event_Message[no][4] ;
		}
	}
	if(flag==0)
	{
		for(int j=0;j<=1;j++)
		{
			final_data[Lasa].slave[j].YMD=event_time[Lasa].YMD ;
			final_data[Lasa].slave[j].GPS_time_stamp=event_time[Lasa].GPS_time_stamp ;
			final_data[Lasa].slave[j].CTD=event_time[Lasa].CTD ;
			final_data[Lasa].slave[j].nsec=event_time[Lasa].nsec ;
			final_data[Lasa].slave[j].Trigg_pattern=(handle_messages.Event_Message[no][3]<<8)+handle_messages.Event_Message[no][4] ;
		}
	}
	short unsigned a1,b1,a2,b2,a3,b3,data1,data2 ;	//For channel 1
	short unsigned A1,B1,A2,B2,A3,B3,DATA1,DATA2 ;	//For channel 2
	int k=0 ;
	for(int j=22;j<=5998+22;j+=3)
	{
		a1=(handle_messages.Event_Message[no][j]>>4)&0xf ;
		b1=handle_messages.Event_Message[no][j]&0xf ;
		a2=(handle_messages.Event_Message[no][j+1]>>4)&0xf ;
		data1=(a1<<8)+(b1<<4)+a2 ;
		
		b2=handle_messages.Event_Message[no][j+1]&0xf ;
		a3=(handle_messages.Event_Message[no][j+2]>>4)&0xf ;
		b3=handle_messages.Event_Message[no][j+2]&0xf ;
		data2=(b2<<8)+(a3<<4)+b3 ;
		
		A1=(handle_messages.Event_Message[no][j+6000]>>4)&0xf ;
		B1=handle_messages.Event_Message[no][j+6000]&0xf ;
		A2=(handle_messages.Event_Message[no][j+6000+1]>>4)&0xf ;
		DATA1=(A1<<8)+(B1<<4)+A2 ;
		
		B2=handle_messages.Event_Message[no][j+6000+1]&0xf ;
		A3=(handle_messages.Event_Message[no][j+6000+2]>>4)&0xf ;
		B3=handle_messages.Event_Message[no][j+6000+2]&0xf ;
		DATA2=(B2<<8)+(A3<<4)+B3 ;

		if(flag==1)
		{
			final_data[Lasa].master[0].counts[k]=data1 ;
			Trace_1->SetBinContent(k+1,data1) ;
			final_data[Lasa].master[1].counts[k]=DATA1 ;
			Trace_2->SetBinContent(k+1,DATA1) ;
			if(k+1<=BG_No_Bin)
			{
				h_BG_1->Fill(data1) ;
				h_BG_2->Fill(DATA1) ;
			}
			k++ ;

			final_data[Lasa].master[0].counts[k]=data2 ;
			Trace_1->SetBinContent(k+1,data2) ;
			final_data[Lasa].master[1].counts[k]=DATA2 ;
			Trace_2->SetBinContent(k+1,DATA2) ;
			if(k+1<=BG_No_Bin)
			{
				h_BG_1->Fill(data2) ;
				h_BG_2->Fill(DATA2) ;
			}
		}
		if(flag==0)
		{
			final_data[Lasa].slave[0].counts[k]=data1 ;
			Trace_1->SetBinContent(k+1,data1) ;
			final_data[Lasa].slave[1].counts[k]=DATA1 ;
			Trace_2->SetBinContent(k+1,DATA1) ;
			if(k+1<=BG_No_Bin)
			{
				h_BG_1->Fill(data1) ;
				h_BG_2->Fill(DATA1) ;
			}
			k++ ;

			final_data[Lasa].slave[0].counts[k]=data2 ;
			Trace_1->SetBinContent(k+1,data2) ;
			final_data[Lasa].slave[1].counts[k]=DATA2 ;
			Trace_2->SetBinContent(k+1,DATA2) ;
			if(k+1<=BG_No_Bin)
			{
				h_BG_1->Fill(data2) ;
				h_BG_2->Fill(DATA2) ;
			}
		}
		k++ ;
	}
	if(flag==1)
	{
		BG_mean_1=h_BG_1->GetMean() ;
		BG_1 = new TF1("BG_1",Background,0,10000,1) ;
		BG_1->SetParameter(0,BG_mean_1) ;
		Trace_1->Add(BG_1,-1) ;		//Substracting background	
		/*for(int k=1;k<=4000;k++)
		{
			if((Trace_1->GetBinContent(k))<0) Trace_1->SetBinContent(k,0) ;
		}*/
		Bin_Start=(Trace_1->GetMaximumBin()-(int)(Window_Open/2.5)) ;
		Bin_End=(Trace_1->GetMaximumBin()+(int)(Window_Close/2.5)) ;

		final_data[Lasa].master[0].Total_counts=(unsigned int)(Trace_1->Integral(Bin_Start,Bin_End)) ;
		final_data[Lasa].master[0].Pulse_height=(unsigned int)(Trace_1->GetMaximum()+BG_mean_1) ;
		final_data[Lasa].master[0].Pulse_width=(unsigned int)(Trace_1->GetRMS()) ;
		//if(final_data[Lasa].master[0].Pulse_height>(unsigned int)mVolts2ADC*messages.Control_Parameters[16])
		//{
			if(4*Lasa+1==Monitoring->Click_Traces_return() && counter_traces_1==Monitor2 && Monitoring->Click1_return()==1)
			{
				Monitoring->DRAW_Traces(Trace_1,4*Lasa+1) ;	//Display traces
				counter_traces_1=1 ;
			}
			else counter_traces_1++ ;
			if(counter_traces_1>Monitor2) counter_traces_1=1 ;

		if(final_data[Lasa].master[0].Pulse_height>(unsigned int)mVolts2ADC*messages.Control_Parameters[16]) Monitoring->FILL_Hits(final_data[Lasa].master[0].detector) ;
			Landau_1->Fill(final_data[Lasa].master[0].Total_counts) ;
			if(monitoring_1==Monitor1 && Monitoring->Click1_return()==1)
			{
				Monitoring->DRAW_Hits(0) ;	//Drawing online histogram
				timeB=time(NULL) ;
				if((unsigned int)(timeB-timeA)>=(unsigned int)(Hits_Display*60))
				{
					Monitoring->DRAW_Hits(1) ;
					timeA=time(NULL) ;
				}
				Fit_1= new TF1("Fit1","landau",0,15000) ;
				Landau_1->Fit("Fit1","Q0") ;
				Monitoring->Fill_MPV_Sigma(final_data[Lasa].master[0].detector,Fit_1->GetParameter(1),Fit_1->GetParameter(2)) ;
				printf("Det=%d\tConst=%lf\tMPV=%lf\tSigma=%lf\n",final_data[Lasa].master[0].detector,Fit_1->GetParameter(0),Fit_1->GetParameter(1),Fit_1->GetParameter(2)) ;
				Monitoring->Draw_MPV_Sigma() ;
				if(4*Lasa+1==Monitoring->Click_Landau_return()) Monitoring->DRAW_Landau(Landau_1,4*Lasa+1,Fit_1->GetParameter(0),Fit_1->GetParameter(1),Fit_1->GetParameter(2)) ;

				Landau_1->Reset() ;
				delete Fit_1 ;
				monitoring_1=1 ;	
			}
			else monitoring_1++ ;
			if(monitoring_1>Monitor1) monitoring_1=1 ;
		//}
		h_BG_1->Reset() ;
		Trace_1->Reset() ;

		BG_mean_2=h_BG_2->GetMean() ;
		BG_2 = new TF1("BG_2",Background,0,10000,1) ;
		BG_2->SetParameter(0,BG_mean_2) ;
		Trace_2->Add(BG_2,-1) ;		//Substracting background	
		/*for(int k=1;k<=4000;k++)
		{
			if((Trace_2->GetBinContent(k))<0) Trace_2->SetBinContent(k,0) ;
		}*/
		Bin_Start=(Trace_2->GetMaximumBin()-(int)(Window_Open/2.5)) ;
		Bin_End=(Trace_2->GetMaximumBin()+(int)(Window_Close/2.5)) ;

		final_data[Lasa].master[1].Total_counts=(unsigned int)(Trace_2->Integral(Bin_Start,Bin_End)) ;
		final_data[Lasa].master[1].Pulse_height=(unsigned int)(Trace_2->GetMaximum()+BG_mean_2) ;
		final_data[Lasa].master[1].Pulse_width=(unsigned int)(Trace_2->GetRMS()) ;
		//if(final_data[Lasa].master[1].Pulse_height>(unsigned int)mVolts2ADC*messages.Control_Parameters[18])
		//{
			if(4*Lasa+2==Monitoring->Click_Traces_return() && counter_traces_2==Monitor2 && Monitoring->Click1_return()==1)
			{
				Monitoring->DRAW_Traces(Trace_2,4*Lasa+2) ;	//Display traces
				counter_traces_2=1 ;
			}
			else counter_traces_2++ ;
			if(counter_traces_2>Monitor2) counter_traces_2=1 ;

		if(final_data[Lasa].master[1].Pulse_height>(unsigned int)mVolts2ADC*messages.Control_Parameters[18]) Monitoring->FILL_Hits(final_data[Lasa].master[1].detector) ;
			Landau_2->Fill(final_data[Lasa].master[1].Total_counts) ;
			if(monitoring_2==Monitor1 && Monitoring->Click1_return()==1)
			{
				Monitoring->DRAW_Hits(0) ;	//Drawing online histogram
				timeB=time(NULL) ;
				if((unsigned int)(timeB-timeA)>=(unsigned int)(Hits_Display*60))
				{
					Monitoring->DRAW_Hits(1) ;
					timeA=time(NULL) ;
				}
				Fit_2= new TF1("Fit2","landau",0,15000) ;
				Landau_2->Fit("Fit2","Q0") ;
				Monitoring->Fill_MPV_Sigma(final_data[Lasa].master[1].detector,Fit_2->GetParameter(1),Fit_2->GetParameter(2)) ;
				printf("Det=%d\tMPV=%f\tSigma=%f\n",final_data[Lasa].master[1].detector,Fit_2->GetParameter(1),Fit_2->GetParameter(2)) ;
				Monitoring->Draw_MPV_Sigma() ;
				if(4*Lasa+2==Monitoring->Click_Landau_return()) Monitoring->DRAW_Landau(Landau_2,4*Lasa+2,Fit_2->GetParameter(0),Fit_2->GetParameter(1),Fit_2->GetParameter(2)) ;

				Landau_2->Reset() ;
				delete Fit_2 ;
				monitoring_2=1 ;	
			}
			else monitoring_2++ ;
			if(monitoring_2>Monitor1) monitoring_2=1 ;
		//}
		h_BG_2->Reset() ;
		Trace_2->Reset() ;
	}
	if(flag==0)
	{
		BG_mean_1=h_BG_1->GetMean() ;
		BG_1 = new TF1("BG_1",Background,0,10000,1) ;
		BG_1->SetParameter(0,BG_mean_1) ;
		Trace_1->Add(BG_1,-1) ;		//Substracting background	
		/*for(int k=1;k<=4000;k++)
		{
			if((Trace_1->GetBinContent(k))<0) Trace_1->SetBinContent(k,0) ;
		}*/
		Bin_Start=(Trace_1->GetMaximumBin()-(int)(Window_Open/2.5)) ;
		Bin_End=(Trace_1->GetMaximumBin()+(int)(Window_Close/2.5)) ;
		final_data[Lasa].slave[0].Total_counts=(unsigned int)(Trace_1->Integral(Bin_Start,Bin_End)) ; 
		final_data[Lasa].slave[0].Pulse_height=(unsigned int)(Trace_1->GetMaximum()+BG_mean_1) ;
		final_data[Lasa].slave[0].Pulse_width=(unsigned int)(Trace_1->GetRMS()) ;
		//if(final_data[Lasa].slave[0].Pulse_height>(unsigned int)mVolts2ADC*messages.Control_Parameters[16])
		//{
			if(4*Lasa+3==Monitoring->Click_Traces_return() && counter_traces_1==Monitor2 && Monitoring->Click1_return()==1)
			{
				Monitoring->DRAW_Traces(Trace_1,4*Lasa+3) ;	//Display traces
				counter_traces_1=1 ;
			}
			else counter_traces_1++ ;
			if(counter_traces_1>Monitor2) counter_traces_1=1 ;

		if(final_data[Lasa].slave[0].Pulse_height>(unsigned int)mVolts2ADC*messages.Control_Parameters[16]) Monitoring->FILL_Hits(final_data[Lasa].slave[0].detector) ;
			Landau_1->Fill(final_data[Lasa].slave[0].Total_counts) ;
			if(monitoring_1==Monitor1 && Monitoring->Click1_return()==1)
			{
				Monitoring->DRAW_Hits(0) ;	//Drawing online histogram
				timeB=time(NULL) ;
				if((unsigned int)(timeB-timeA)>=(unsigned int)(Hits_Display*60))
				{
					Monitoring->DRAW_Hits(1) ;
					timeA=time(NULL) ;
				}
				Fit_1= new TF1("Fit1","landau",0,15000) ;
				Landau_1->Fit("Fit1","Q0") ;
				Monitoring->Fill_MPV_Sigma(final_data[Lasa].slave[0].detector,Fit_1->GetParameter(1),Fit_1->GetParameter(2)) ;
				printf("Det=%d\tMPV=%f\tSigma=%f\n",final_data[Lasa].slave[0].detector,Fit_1->GetParameter(1),Fit_1->GetParameter(2)) ;
				Monitoring->Draw_MPV_Sigma() ;
				if(4*Lasa+3==Monitoring->Click_Landau_return()) Monitoring->DRAW_Landau(Landau_1,4*Lasa+3,Fit_1->GetParameter(0),Fit_1->GetParameter(1),Fit_1->GetParameter(2)) ;

				Landau_1->Reset() ;
				delete Fit_1 ;
				monitoring_1=1 ;
			}
			else monitoring_1++ ;
			if(monitoring_1>Monitor1) monitoring_1=1 ;
		//}
		h_BG_1->Reset() ;
		Trace_1->Reset() ;

		BG_mean_2=h_BG_2->GetMean() ;
		BG_2 = new TF1("BG_2",Background,0,10000,1) ;
		BG_2->SetParameter(0,BG_mean_2) ;
		Trace_2->Add(BG_2,-1) ;		//Substracting background	
		/*for(int k=1;k<=4000;k++)
		{
			if((Trace_2->GetBinContent(k))<0) Trace_2->SetBinContent(k,0) ;
		}*/
		Bin_Start=(Trace_2->GetMaximumBin()-(int)(Window_Open/2.5)) ;
		Bin_End=(Trace_2->GetMaximumBin()+(int)(Window_Close/2.5)) ;

		final_data[Lasa].slave[1].Total_counts=(unsigned int)(Trace_2->Integral(Bin_Start,Bin_End)) ;
		final_data[Lasa].slave[1].Pulse_height=(unsigned int)(Trace_2->GetMaximum()+BG_mean_2) ;
		final_data[Lasa].slave[1].Pulse_width=(unsigned int)(Trace_2->GetRMS()) ;
		//if(final_data[Lasa].slave[1].Pulse_height>(unsigned int)mVolts2ADC*messages.Control_Parameters[18])
		//{
			if(4*Lasa+4==Monitoring->Click_Traces_return() && counter_traces_2==Monitor2 && Monitoring->Click1_return()==1)
			{
				Monitoring->DRAW_Traces(Trace_2,4*Lasa+4) ;	//Display traces
				counter_traces_2=1 ;
			}
			else counter_traces_2++ ;
			if(counter_traces_2>Monitor2) counter_traces_2=1 ;

		if(final_data[Lasa].slave[1].Pulse_height>(unsigned int)mVolts2ADC*messages.Control_Parameters[18]) Monitoring->FILL_Hits(final_data[Lasa].slave[1].detector) ;
			Landau_2->Fill(final_data[Lasa].slave[1].Total_counts) ;
			if(monitoring_2==Monitor1 && Monitoring->Click1_return()==1)
			{
				Monitoring->DRAW_Hits(0) ;	//Drawing online histogram
				timeB=time(NULL) ;
				if((unsigned int)(timeB-timeA)>=(unsigned int)(Hits_Display*60))
				{
					Monitoring->DRAW_Hits(1) ;
					timeA=time(NULL) ;
				}
				Fit_2= new TF1("Fit2","landau",0,15000) ;
				Landau_2->Fit("Fit2","Q0") ;
				Monitoring->Fill_MPV_Sigma(final_data[Lasa].slave[1].detector,Fit_2->GetParameter(1),Fit_2->GetParameter(2)) ;
				printf("Det=%d\tMPV=%f\tSigma=%f\n",final_data[Lasa].slave[1].detector,Fit_2->GetParameter(1),Fit_2->GetParameter(2)) ;
				Monitoring->Draw_MPV_Sigma() ;
				if(4*Lasa+4==Monitoring->Click_Landau_return()) Monitoring->DRAW_Landau(Landau_2,4*Lasa+4,Fit_2->GetParameter(0),Fit_2->GetParameter(1),Fit_2->GetParameter(2)) ;
				
				Landau_2->Reset() ;
				delete Fit_2 ;
				monitoring_2=1 ;	
			}
			else monitoring_2++ ;
			if(monitoring_2>Monitor1) monitoring_2=1 ;
		//}
		h_BG_2->Reset() ;
		Trace_2->Reset() ;
	}
}

void DEVICE:: Close_Socket()
{
	Socket->Close_Socket() ;
}
