//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Aris Noutsos
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Applications/njkk08/KaraFit.h,v $
   $Revision: 1.3 $
   $Date: 2009/08/22 13:42:31 $
   $Author: straten $ */

#ifndef _KaraFit_h
#define _KaraFit_h

#include <vector>
#include <math.h>

class KaraFit
{

public:

   KaraFit();
   ~KaraFit();
   
   
   void passData (const std::vector<double>& _pas, 
		  const std::vector<double>& _errs, 
		  const std::vector<double>& _freqs,
		  double _rm1, 
		  double _rm2, 
		  double _drm,
		  double _pa1, 
		  double _pa2, 
		  double _dpa)
   {

     rm1 = _rm1; rm2 = _rm2;
     drm = _drm; dpa = _dpa;
     pa1 = _pa1; pa2 = _pa2;

     pas = _pas;
     errs = _errs;
     freqs = _freqs;
     
   }
 
   int doFit();

   std::vector<double> fitValues;


private:


   void fitResult();



   double r2d(){return 180.0/M_PI;}
   double d2r(){return M_PI/180.0;}

   double rm;
   double drm;
   double pa;
   double dpa;
   
   double rm1,rm2;
   double pa1,pa2;
   
   double fu;
   double xmin,xmax;
   double expo;
   
   double prob;
   double intstep;
   
   double probmin,probmax;
   double bestrm,bestpa;
   double sigpa;
   
   std::vector<double> freqs;
   std::vector<double> pas;
   std::vector<double> errs;


}; 

#endif
