#include <strstream>
#include <stdlib.h>
#include <TMath.h>
#include <TF2.h>
#include <TH2F.h>
#include <TROOT.h>
#include <TMinuit.h>
#include "Constants.h"
class ARRIVAL_DIRECTION
{
	private:
		TH2F *h_theta_phi ;
		struct AD	//For arrival direction calculation 
		{
			unsigned int Max_Count ;	//Detector having the Maximum count in an event
			float cdt[NO_LASAs*4] ;
			float weight[NO_LASAs*4] ;
		} ad;
	
	
	public:
		void Read_Detector_Cord() ;
		void Get_Origin(int,float *,float *) ;
		TH2F* Do_Fit() ;

	ARRIVAL_DIRECTION()
	{
		h_theta_phi = new TH2F("h_theta_phi","Arrival Direction",18,0,90,36,0,360) ;	
	}
} ;
