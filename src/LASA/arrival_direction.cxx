#include "arrival_direction.h"

float Det_X[NO_LASAs*4],Det_Y[NO_LASAs*4],Det_Z[NO_LASAs*4],cdt[NO_LASAs*4],weight[NO_LASAs*4] ;
float X0,Y0,Z0 ;	//Origin of (X',Y',Z'): shower plane 

double Sub_Func(float x,float y,float z,double *par)
{
	double value=x*cos(par[0]*3.14/180)*sin(par[1]*3.14/180)+y*sin(par[0]*3.14/180)*sin(par[1]*3.14/180)+z*cos(par[1]*3.14/180) ;
 	return value ;
}

void Func_To_Fit(int &npar,double *gin,double &f,double *par,int iflag)
{
	const int nbins = 4*NO_LASAs ;
	int i ;
	double chisq = 0 ;
	double delta ;
	for(i=0;i<nbins;i++)	//calculate chisquare
	{
		delta=cdt[i]-Sub_Func(Det_X[i]-X0,Det_Y[i]-Y0,Det_Z[i]-Z0,par) ;
		chisq+=weight[i]*delta*delta ;
	}
	f=chisq ;
}

void ARRIVAL_DIRECTION:: Read_Detector_Cord()
{
	char content[200] ;
	char det[50],x[50],y[50],z[50] ;
	float X,Y,Z ;
	int i=0 ;
	FILE *fp ;
	fp=fopen("./data/Detector_Cord.dat","r") ;
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

void ARRIVAL_DIRECTION:: Get_Origin(int i,float *cdt,float *weight)
{
	X0=Det_X[i-1] ;
	Y0=Det_Y[i-1] ;
	Z0=Det_Z[i-1] ;
}

TH2F* ARRIVAL_DIRECTION:: Do_Fit()
{
	TMinuit *gMinuit = new TMinuit(2) ;  //initialize TMinuit
	gMinuit->SetFCN(Func_To_Fit) ;
	double arglist[10] ;
	int ierflg = 0 ;
	arglist[0] = 1 ;
	gMinuit->SetPrintLevel(-1) ;
	gMinuit->mnexcm("SET ERR",arglist,1,ierflg) ;

	// Set starting values and step sizes for parameters
	static Double_t vstart[2] = {30,30} ;
	static Double_t step[2] = {0.1,0.1} ;
	gMinuit->mnparm(0,"phi",vstart[0],step[0],0,0,ierflg) ;
	gMinuit->mnparm(1,"theta",vstart[1],step[1],0,0,ierflg) ;

	// Now ready for minimization step
	arglist[0] = 500 ;
	arglist[1] = 1.0 ;
	gMinuit->mnexcm("MIGRAD",arglist,1,ierflg) ;
	gMinuit->SetErrorDef(1) ;
   	
	// get the fitted values
	double PHI,PHI_err,THETA,THETA_err ;
	gMinuit->GetParameter(0,PHI,PHI_err) ;
	gMinuit->GetParameter(1,THETA,THETA_err) ;

	h_theta_phi->Fill(THETA,PHI) ;
	return h_theta_phi ;
}
