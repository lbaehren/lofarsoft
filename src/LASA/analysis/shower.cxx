#include "shower.h"

void SHOWER:: Read_Detector_Cord()
{
	char content[200] ;
	char det[50],x[50],y[50],z[50] ;
	float X,Y,Z ;
	int i=0 ;
	FILE *fp ;
	fp=fopen("../data/Detector_Cord.dat","r") ;
	while(fgets(content,200,fp)!=NULL)
	{
		if(strncmp(content,"//",strlen("//"))==0) continue ;
		std::istrstream M(content) ;
		M>>det>>x>>y>>z ;
		std::istrstream XX(x) ;
		std::istrstream YY(y) ;
		std::istrstream ZZ(z) ;
		XX>>X ;
		YY>>Y ;
		ZZ>>Z ;
		Det_X[i]=X ; 
		Det_Y[i]=Y ;
		Det_Z[i]=Z ;
		//printf("%d\t%f\t%f\t%f\n",i,Det_X[i],Det_Y[i],Det_Z[i]) ;
		i++ ;
		if(i==NO_LASAs*4) break ;
	}
	fclose(fp) ;
}

void SHOWER:: Find_Core(long long unsigned *time,float *count)
{
	Event_Size=0 ;
	float SumX=0 ;
	float SumY=0 ;
	for(int i=0;i<NO_DETs;i++)
	{
		if(*time>0)
		{
			Event_Size=Event_Size+(*count) ;
			SumX=SumX+(*count)*Det_X[i] ;
			SumY=SumY+(*count)*Det_Y[i] ;
		}
		time++ ;
		count++ ;
	}
	x_core=SumX/Event_Size ;
	y_core=SumY/Event_Size ;
}

float SHOWER:: Get_XCore()
{
	return x_core ;
}

float SHOWER:: Get_YCore()
{
	return y_core ;
}

float SHOWER:: Get_Size()
{
	return Event_Size ;
}
