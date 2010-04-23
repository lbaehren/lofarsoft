#include <stdio.h>
#include "Auger.h"
 
void gps_data(unsigned char *buf)
{
  int i;
  unsigned short t,t2,SYCN;
  unsigned int ctp,ctd,CTP;
	float QUANT ;
	char quant[4] ;
  
  //printf("GPS: ");
  //for(i=0;i<2;i++) printf(" %u ",buf[2+i]);
	printf("\nHeader: \t\t0x%x (%u)\n",buf[0],buf[0]) ;
	printf("Identifier: \t\t0x%x (%u)\n",buf[1],buf[1]) ;
	printf("Day: \t\t0x%x (%u)\n",buf[2],buf[2]) ;
	printf("Month: \t\t0x%x (%u)\n",buf[3],buf[3]) ;
	t=(buf[4]<<8)+buf[5];
	printf("Year: \t\t0x%x (%u)\n",t,t);
	printf("Hour: \t\t0x%x (%u)\n",buf[6],buf[6]) ;
	printf("Minutes: \t\t0x%x (%u)\n",buf[7],buf[7]) ;
	printf("Seconds: \t\t0x%x (%u)\n",buf[8],buf[8]) ;
  //for(i=4;i<7;i++) printf(" %u ",buf[2+i]);	
  //printf("\n");
  t = *((int *)&(buf[9]));
  SWAP(t);
  t2 = *((int *)&(buf[11]));
  SWAP(t2);
  ctp = (t<<16)+t2;
	CTP = (buf[9]<<24)+(buf[10]<<16)+(buf[11]<<8)+buf[12] ;
	SYCN = (buf[9]>>7)&0x01 ;
	for(i=0;i<4;i++) quant[i]=buf[13+i] ;
	QUANT = *(float*)quant;
  t = *((int *)&(buf[13]));
  SWAP(t);
  t2 = *((int *)&(buf[15]));
  SWAP(t2);
  ctd = (t<<16)+t2;
	printf("ctp: \t\t0x%x (%u)\n",ctp,ctp) ;
	printf("CTP: \t\t0x%x (%u)\n",CTP,CTP) ;
	printf("SYCN: \t\t0x%x (%u)\n",SYCN,SYCN) ;
	///printf("QUANT: \t\t0x%x (%e)\n",QUANT,QUANT) ;
	printf("ctd: \t\t0x%x (%u)\n",ctd,ctd) ;
  //printf("CTP %u CTD %u\n",ctp,ctd);
  t = *((int *)&(buf[17]));
  SWAP(t);
  t2 = *((int *)&(buf[19]));
  SWAP(t2);
	printf("Ch 2 Low: \t0x%x (%d)\n",t,t) ;
	printf("Ch 2 High: \t0x%x (%d)\n",t2,t2) ;
  //printf("Ch 2 high %u, low %u\n",t,t2);
  t = *((int *)&(buf[21]));
  SWAP(t);
  t2 = *((int *)&(buf[23]));
  SWAP(t2);
	printf("Ch 1 Low: \t0x%x (%d)\n",t,t) ;
	printf("Ch 1 High: \t0x%x (%d)\n",t2,t2) ;
  //printf("Ch 1 high %u, low %u\n",t,t2);
  //printf("End 0x%02x\n",buf[25]);
}
