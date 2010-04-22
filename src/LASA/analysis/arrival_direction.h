#include <strstream>
#include <stdlib.h>
#include <TMath.h>
#include <TF2.h>
#include <TH2F.h>
#include <TROOT.h>
#include <TMinuit.h>
#include "constants.h"

class ARRIVAL_DIRECTION
{
	private:
		float Det_X[NO_DETs],Det_Y[NO_DETs],Det_Z[NO_DETs],cdt[NO_DETs],weight[NO_DETs] ;
		float X0,Y0,Z0 ;	//Ref Det. coord (x,y,z) 
		long long unsigned T0 ;	//Ref Det. event arrival time (t) 
		double PHI,THETA ;	//Arrival direction (theta,phi)
		int Det0 ;		//Detector no.: Representing the origin of the shower plane
		int Flag_Event ;	//1:Accept and 0:Reject
	
	public:
		void Read_Detector_Cord() ;
		void Do_Ref_Det(int) ;		//Calculating values like the (x,y,z) coord. for the Ref. det
		float Get_X0() ;		//Return x coord. for the ref. detector
		float Get_Y0() ;		//Return y coord. for the ref. detector
		float Get_Z0() ;		//Return z coord. for the ref. detector
		long long unsigned Get_T0() ;	//Return event arrival time for the ref. detector
		int Accept_Event() ;
		void Do_Arrival_Direction(long long unsigned *,float*) ;//Calculate arrival direction
		void Do_Best_Fit() ;
		float Get_cdt(int) ;		//Return cdt
		double Get_Theta() ;		//Return theta in degrees
		double Get_Phi() ;		//Return phi in degrees
} ;
