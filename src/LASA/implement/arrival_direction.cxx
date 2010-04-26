#include "arrival_direction.h"

void ARRIVAL_DIRECTION:: Read_Detector_Cord()
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
		i++ ;
		if(i==NO_LASAs*4) break ;
	}
	fclose(fp) ;
}

void ARRIVAL_DIRECTION:: Do_Ref_Det(int i)
{
	X0=Det_X[i] ;
	Y0=Det_Y[i] ;
	Z0=Det_Z[i] ;
}

float ARRIVAL_DIRECTION:: Get_X0()
{
	return X0 ;
}

float ARRIVAL_DIRECTION:: Get_Y0()
{
	return Y0 ;
}

float ARRIVAL_DIRECTION:: Get_Z0()
{
	return Z0 ;
}

long long unsigned ARRIVAL_DIRECTION:: Get_T0()
{
	return T0 ;
}

int ARRIVAL_DIRECTION:: Accept_Event()
{
	return Flag_Event ;
}

void ARRIVAL_DIRECTION:: Do_Arrival_Direction(long long unsigned* time,float* count)
{
	long long unsigned event_time[NO_DETs] ;
	Det0=-1 ;	//Ref detector no. (the one which receives the first trigger)
	int flag=0 ;
	long long unsigned temp ;
	for(int j=0;j<NO_DETs;j++)
	{
		event_time[j]=*time ;
		weight[j]=*count ;
		if(event_time[j]>0)
		{
			if(flag==0)
			{
				temp=event_time[j] ;
				Det0=j ;
			}
			else
			{
				if(event_time[j]<temp)
				{
					temp=event_time[j] ;
					Det0=j ;
				}
			}
			flag++ ;
		}
		time++ ;
		count++ ;
	}
	printf("Det0=%2d\n",Det0) ;
	Do_Ref_Det(Det0) ;
	T0=event_time[Det0] ;

	for(int j=0;j<NO_DETs;j++)	//Calculating the arrival time differences w.r.t the ref. detector
	{
		if(event_time[j]>0) cdt[j]=(float)(event_time[j]-event_time[Det0])*0.1*(1.e-9*vel_light) ;	//in meters
		else cdt[j]=-1 ;
		printf("[Det=%2d] Time=%16llu\tcount=%f\tcdt=%f\n",j,event_time[j],weight[j],cdt[j]) ;
	}
}

void ARRIVAL_DIRECTION:: Do_Best_Fit()
{
	float P,Q,R,S,W,T1,S1,S2,S3,S4,S5,S6 ;
	P=Q=R=S=W=T1=S1=S2=S3=S4=S5=S6=0 ;
	int counter=0 ;
	for(int j=0;j<NO_DETs;j++)
	{	
		weight[j]=1 ;		//Ignoring event weights
		if(cdt[j]>=0)
		{
			counter++ ;
			S=S+weight[j]*Det_X[j]*Det_X[j] ;
			W=W+weight[j]*Det_Y[j]*Det_Y[j] ;
			T1=T1+weight[j]*Det_X[j] ;
			S1=S1+weight[j]*Det_X[j]*cdt[j] ;
			S2=S2+weight[j]*Det_X[j]*Det_Y[j] ;
			S3=S3+weight[j]*Det_Y[j]*cdt[j] ;
			S4=S4+weight[j]*Det_Y[j] ;
			S5=S5+weight[j]*cdt[j] ;
			S6=S6+weight[j] ;
			P=(S1*S2)/S ;
			Q=(T1*S2)/S ;
			R=-((S2*S2)/S)+W ;
		}
	}
	float t0,l,m ;
	//Here, "t0" is actually "ct0 in meters"
	t0=(-T1*S1*R+R*S*S5-T1*P*S2+T1*S2*S3+S*S4*P-S*S4*S3)/(-T1*Q*S2-T1*S2*S4-R*T1*T1+S*S4*Q+S*S4*S4+R*S*S6) ;
	m=(-P+t0*Q+S3+t0*S4)/R ;
	l=(S1/S)+((P*S2)/(R*S))-((t0*Q*S2)/(R*S))-((S2*S3)/(R*S))-((t0*S2*S4)/(R*S))-((t0*T1)/S) ;

	THETA=(asin(sqrt(l*l+m*m)))*(180.0/pi) ;	//in degrees
	PHI=(acos(m/sqrt(l*l+m*m)))*(180.0/pi) ;	//in degrees
	if(l<0) PHI=360.0-PHI ;
	if(counter<TRIGG_COND)
	{
		THETA=-1 ;
		PHI=-1 ;
		Flag_Event=0 ;
	}
	else Flag_Event=1 ;
	printf("ct0=%f\tl=%f\tm=%f\tTHETA=%lf\tPHI=%lf\n",t0,l,m,THETA,PHI) ;
}

float ARRIVAL_DIRECTION:: Get_cdt(int i)
{
	return cdt[i] ;
}

double ARRIVAL_DIRECTION:: Get_Theta()
{
	return (double)THETA ;
}

double ARRIVAL_DIRECTION:: Get_Phi()
{
	return (double)PHI ;
}
