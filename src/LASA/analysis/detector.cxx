#include "detector.h"

void DETECTOR:: Read_Cord(char *filename,int n)
{
	char content[200] ;
	char det[50],x[50],y[50],z[50] ;
	float X,Y,Z ;
	int i=0 ;
	FILE *fp ;
	fp=fopen(filename,"r") ;
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
		X_Cor[i]=X ; 
		Y_Cor[i]=Y ; 
		Z_Cor[i]=Z ;
		if(i==0)
		{
			Xmin=X_Cor[i] ;
			Xmax=X_Cor[i] ;
			Ymin=Y_Cor[i] ;
			Ymax=Y_Cor[i] ;
		}
		if(X_Cor[i]<Xmin) Xmin=X_Cor[i] ;  
		if(X_Cor[i]>Xmax) Xmax=X_Cor[i] ;  
		if(Y_Cor[i]<Ymin) Ymin=Y_Cor[i] ;  
		if(Y_Cor[i]>Ymax) Ymax=Y_Cor[i] ; 
		i++ ;
		if(i==n) break ;
	}
	fclose(fp) ;
}

float DETECTOR:: Get_XCord(int n)
{
	return X_Cor[n] ;
}

float DETECTOR:: Get_YCord(int n)
{
	return Y_Cor[n] ;
}

float DETECTOR:: Get_ZCord(int n)
{
	return Z_Cor[n] ;
}

float DETECTOR:: Get_Xmin()
{
	return Xmin ;
}

float DETECTOR:: Get_Xmax()
{
	return Xmax ;
}

float DETECTOR:: Get_Ymin()
{
	return Ymin ;
}

float DETECTOR:: Get_Ymax()
{
	return Ymax ;
}
