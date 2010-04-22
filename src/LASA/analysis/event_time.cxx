#include "event_time.h"

void EVENT_TIME:: Sort_Increase_Order(unsigned long* buf)
{
	for(int j=0;j<=3;j++)
	{
		corrected[j]=*buf ;
		uncorrected[j]=*buf ;
		buf++ ;
	}

	unsigned long temp_time ;
	for(int j=0;j<=3;j++)
	{
		temp_time=uncorrected[j] ;
		for(int i=j+1;i<=3;i++)
		{
			if(uncorrected[i]<temp_time)
			{
				uncorrected[j]=uncorrected[i] ;
				uncorrected[i]=temp_time ;
				temp_time=uncorrected[j] ;
			}
		}
		//printf("[%2d] uncorrected=%lu\t corrected=%lu\n",j+1,uncorrected[j],corrected[j]) ;
	}
}

void EVENT_TIME:: Cal_Event_Time(unsigned int trigg_condi,unsigned long time_stamp)
{
	unsigned long trigg_at ;	//event time for the one which decides the trigger
	unsigned int n=0 ;
	for(int i=0;i<=3;i++)
	{
		if(uncorrected[i]==0) continue ;
		else n++ ;
		if(n==trigg_condi)
		{
			trigg_at=uncorrected[i] ;
			//printf("\tTime stamp=%lu\ttrigg_at=%lu\n",time_stamp,trigg_at) ;
			break ;
		}
	}
	for(int i=0;i<=3;i++)
	{
		if(n==trigg_condi && corrected[i]>0) 
		{
			corrected[i]=corrected[i]-trigg_at+time_stamp ;
			//printf("\t[%2d] corrected time=%lu\tcorrected time=%lu\n",i+1,corrected[i],corrected[i]) ;
		}
		else corrected[i]=0 ;
	}
}

unsigned long EVENT_TIME:: Get_Event_Time(int i)
{
	return corrected[i] ;
}

