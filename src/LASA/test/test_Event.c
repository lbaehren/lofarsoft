#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	char out_file[100],xx[10] ;
	int Sec_no=1 ;
	int event_no=1 ;
	FILE *out_event, *out_One_Sec ;

	float time=0 ;
	unsigned short a1,b1,a2,b2,a3,b3,data1,data2 ;	//For channel 1
	unsigned short A1,B1,A2,B2,A3,B3,DATA1,DATA2 ;	//For channel 2

	strcpy(out_file,argv[2]) ;
	sprintf(xx,"%d",event_no) ;	
	strcat(out_file,"_Event_") ;
	strcat(out_file,xx) ;
	out_event=fopen(out_file,"w") ;
	
	strcpy(out_file,argv[2]) ;
	sprintf(xx,"%d",Sec_no) ;	
	strcat(out_file,"_Sec_") ;
	strcat(out_file,xx) ;
	out_One_Sec=fopen(out_file,"w") ;

	unsigned char Header,Identifier ;
	struct ONE_SEC_MESSAGE
	{
		unsigned char GPS_Time_Stamp[7] ;
		unsigned char CTP[4] ;
		unsigned char Quan_Error[4] ;
		unsigned char Thres_Counters[8] ;
		unsigned char Satellite[61] ;
		unsigned char End ;
	} One_Sec_Message ;

	struct DATA_MESSAGE
	{
		unsigned char Trigg_Condition ;
		unsigned char Trigg_Pattern[2] ;
		unsigned char Pre_Coin_Time[2] ;
		unsigned char Coin_Time[2] ;
		unsigned char Post_Coin_Time[2] ;
		unsigned char GPS_Time_Stamp[7] ;
		unsigned char CTP[4] ;
		unsigned char Data[12000] ;
		unsigned char End ;
	} Data_Message ;

	int inhandle ;
	inhandle=open(argv[1],O_RDONLY) ;	//opening input file containing messages
	int bytes ;
	while(1)
	{
	bytes=read(inhandle,&Header,1) ;
	printf("No. of bytes read=%d\n",bytes) ;
	if(bytes<=0) break ;
	read(inhandle,&Identifier,1) ;

	switch(Identifier)
	{
		case 0xA4:	//1 second message
			bytes=read(inhandle,&One_Sec_Message,85) ;

			printf("No. of bytes read=%d\n",bytes) ;
			fwrite(&One_Sec_Message,85,1,out_One_Sec) ;
			fclose(out_One_Sec) ;
			Sec_no++ ;

			strcpy(out_file,argv[2]) ;
			sprintf(xx,"%d",Sec_no) ;	
			strcat(out_file,"_Sec_") ;
			strcat(out_file,xx) ;
			out_One_Sec=fopen(out_file,"w") ;
			break ;

		case 0xA0:	//Data Message
			bytes=read(inhandle,&Data_Message,12021) ;
		
			printf("No. of bytes read=%d\n",bytes) ;
			for(int j=0;j<=5998;j+=3)
			{
				a1=(Data_Message.Data[j]>>4)&0xf ;
				b1=Data_Message.Data[j]&0xf ;
				a2=(Data_Message.Data[j+1]>>4)&0xf ;
				data1=(a1<<8)+(b1<<4)+a2 ;
		
				b2=Data_Message.Data[j+1]&0xf ;
				a3=(Data_Message.Data[j+2]>>4)&0xf ;
				b3=Data_Message.Data[j+2]&0xf ;
				data2=(b2<<8)+(a3<<4)+b3 ;
		
				A1=(Data_Message.Data[j+6000]>>4)&0xf ;
				B1=Data_Message.Data[j+6000]&0xf ;
				A2=(Data_Message.Data[j+6000+1]>>4)&0xf ;
				DATA1=(A1<<8)+(B1<<4)+A2 ;
		
				B2=Data_Message.Data[j+6000+1]&0xf ;
				A3=(Data_Message.Data[j+6000+2]>>4)&0xf ;
				B3=Data_Message.Data[j+6000+2]&0xf ;
				DATA2=(B2<<8)+(A3<<4)+B3 ;

				fprintf(out_event,"%f\t%d\t%d\n",time,data1,DATA1)  ;
				time=time+2.5 ;
				fprintf(out_event,"%f\t%d\t%d\n",time,data2,DATA2)  ;
				time=time+2.5 ;
			}
			fclose(out_event) ;
			
			event_no++ ;
			time=0 ;
			strcpy(out_file,argv[2]) ;
			sprintf(xx,"%d",event_no) ;	
			strcat(out_file,"_Event_") ;
			strcat(out_file,xx) ;
			out_event=fopen(out_file,"w") ;
			break ;

		default:
			break ;
	}
	}
	close(inhandle) ;
	fclose(out_One_Sec) ;	
	fclose(out_event) ;
}
