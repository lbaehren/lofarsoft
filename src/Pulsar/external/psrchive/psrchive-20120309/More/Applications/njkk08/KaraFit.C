/***************************************************************************
 *
 *   Copyright (C) 2008 by Aris Noutsos
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "KaraFit.h"
#include "Physical.h"

#include <iostream>

using namespace std;

KaraFit::KaraFit()
{

   rm  = 0.0; drm = 0.0;
   pa  = 0.0; dpa = 0.0;
   
   rm1 = 0.0; rm2 = 0.0;
   pa1 = 0.0; pa2 = 0.0;
   
   fu = 0.0;
   xmin = 0.0; xmax = 0.0;
   expo = 0.0;
   
   prob = 0.0;
   intstep = 0.0;
   
   freqs.clear(); 
     pas.clear(); 
       errs.clear();

   fitValues.clear();

   probmin = 1e30,
   probmax = -1e30;

   bestrm = 0.0,
   bestpa = 0.0;

   sigpa = 0.0;

}

   
KaraFit::~KaraFit()
{
}




int KaraFit::doFit()
{
  
    int nsteps = int(ceil(fabs(rm2-rm1)/drm));

    cout <<endl;
    cout << " Doing fit inside RM = [" << rm1 << ":" <<rm2<<"] in "<< nsteps << " steps."<<endl;
    cout <<endl;
   
    double fu = 0.0;
    
    int count = 0;
    
    cerr << "[";   

    const double c0 = Pulsar::speed_of_light * 1e-6;

    for (rm = rm1 ; rm < rm2 ; rm += drm){ 

         if(count%int(ceil(0.1*nsteps))==0) cerr << "."; 

	 for (pa = pa1 ; pa < pa2 ; pa += dpa){

	     prob = 0.0;

	     for (unsigned i = 0; i < pas.size(); i++){

                 fu = pa + r2d()*c0*c0*rm*(1./pow(freqs[i],2)-1./pow(freqs.front(),2));
		 
		 fu = modf(fu/180.0,&intstep);
		 fu *= 180.0;

		 if (fu < 0.0) fu += 180.0;

		 xmin = min(pas[i],fu);
		 xmax = max(pas[i],fu);

		 double x = min(xmax-xmin,180.0-(xmax-xmin));

		 expo = -x*x/(2.*errs[i]*errs[i]); 

        	 prob += expo;

             }

	     double proba = prob;
	     
	     if (proba > probmax){ 

                     bestrm = rm;
		     bestpa = pa;
		     sigpa = errs[0];
		     
		     probmax = proba;

	     }

	     if (proba < probmin)
        	     probmin = prob;

	 } 
	 
	 count++;
    } 

    cerr <<"]"<<endl;


    if(bestrm >= rm2-drm && bestpa >= pa2-dpa)
    {
      cerr << " *** Fit failed: no convergence or bad ranges. *** " <<endl;
      return 1;
    }
    else
         fitResult();
	

    return 0;

    
}


void KaraFit::fitResult()
{

  fitValues.push_back(bestrm);
  fitValues.push_back(bestpa);
  fitValues.push_back(sigpa);
  fitValues.push_back(probmax);

}
