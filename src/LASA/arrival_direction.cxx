#include "arrival_direction.h"

float Det_X[NO_LASAs*4];
float Det_Y[NO_LASAs*4];
float Det_Z[NO_LASAs*4];
float cdt[NO_LASAs*4];
float weight[NO_LASAs*4];
float X0,Y0,Z0 ;	//Origin of (X',Y',Z'): shower plane 

// ==============================================================================
//
//  Construction
//
// ==============================================================================

//_______________________________________________________________________________
//                                                              ARRIVAL_DIRECTION

ARRIVAL_DIRECTION::ARRIVAL_DIRECTION ()
{
  init ();
}

//_______________________________________________________________________________
//                                                              ARRIVAL_DIRECTION

/*!
  \param histogram -- Name of the histogram
  \param tite      -- Title of the histogram
*/
ARRIVAL_DIRECTION::ARRIVAL_DIRECTION (std::string const &histogram,
				      std::string const &title)
{
  init (histogram,
	title);
}

//_______________________________________________________________________________
//                                                                           init

void ARRIVAL_DIRECTION::init (std::string const &histogram,
			      std::string const &title)
{
  // Store basic information
  histogram_p = histogram;
  title_p     = title;
  // Set up the histogram
  h_theta_phi = new TH2F(histogram.c_str(),title.c_str(),18,0,90,36,0,360) ;	
}

// ==============================================================================
//
//  Methods
//
// ==============================================================================

//_______________________________________________________________________________
//                                                                       Sub_Func

/*!
  \param x -- x-coordinate
  \param y -- y-coordinate
  \param z -- z-coordinate
  \param par -- Shift parameter
  \return value
*/
double Sub_Func (float x,
		 float y,
		 float z,
		 double *par)
{
  double value=x*cos(par[0]*3.14/180)*sin(par[1]*3.14/180)+y*sin(par[0]*3.14/180)*sin(par[1]*3.14/180)+z*cos(par[1]*3.14/180) ;
  return value ;
}

//_______________________________________________________________________________
//                                                                    Func_To_Fit

/*!
  \param npar -- Number of paameters
  \param gin 
  \param f
  \param par
  \param iflag
*/
void Func_To_Fit (int &npar,
		  double *gin,
		  double &f,
		  double *par,
		  int iflag)
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

//_______________________________________________________________________________
//                                                             Read_Detector_Cord

void ARRIVAL_DIRECTION::Read_Detector_Cord ()
{
  char content[200] ;
  char det[50],x[50],y[50],z[50] ;
  float X,Y,Z ;
  int i=0 ;
  FILE *fp ;
  fp=fopen("Detector_Cord.dat","r") ;
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

//_______________________________________________________________________________
//                                                                     Get_Origin

/*!
  \param i
  \param cdt
  \param weight
*/
void ARRIVAL_DIRECTION::Get_Origin (int i,
				    float *cdt,
				    float *weight)
{
  X0=Det_X[i-1] ;
  Y0=Det_Y[i-1] ;
  Z0=Det_Z[i-1] ;
}

//_______________________________________________________________________________
//                                                                         Do_Fit

TH2F* ARRIVAL_DIRECTION::Do_Fit()
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
