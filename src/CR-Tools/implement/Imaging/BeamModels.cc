/*-------------------------------------------------------------------------*
 | $Id:: Skymapper.cc 1013 2007-11-05 10:35:15Z baehren                  $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <Imaging/BeamModels.h>

namespace CR { // Namespace CR -- begin

  const complex<double> _Complex_I = complex<double>(0,1);

  // ---------------------------------------------------------------- test_double
  
  double BeamModels::test_double (const double *par,
				  const double *x)
  {
    // check if we try to point below the horizon
    if (x[3]<=0) {
      std::cerr << "Error: tried to point below the horizon!" << std::endl;
      return 0;
    }
    
    // speed of light
    const double c=3.e8;
    const double theta=M_PI/2-x[3];
    const double phi=par[1]+x[2];
    const double result=(1-sqr(sin(theta))*sqr(sin(phi)))*sqr(sin(2*M_PI*x[1]*par[2]*cos(theta)/c));
    return (par[0]*sqrt(result));
    
  }

  // --------------------------------------------------------------------- Gamma1

  complex<double> BeamModels::Gamma1 (double tan_al,
				      double sin_al,
				      double sin_th,
				      double cos_th,
				      double cos_ph,
				      double k,
				      double h,
				      double L)
  { 
    double A=sin_th*cos_ph-cos_th/tan_al;
    double inv_sin_al=1/sin_al;
    double tmp=k*A*L;
    double Re1=cos(tmp)*inv_sin_al;
    double Im1=sin(tmp)*inv_sin_al;
    
    Re1 =Re1-inv_sin_al*cos(k*L*inv_sin_al);
    Im1 =Im1-A*sin(k*L*inv_sin_al);
    
    complex<double> C1=Re1+Im1*_Complex_I;
    
    complex<double> C2=cos(k*h*cos_th)+sin(k*h*cos_th)*_Complex_I;
    
    C1=-C1*C2/(A*A-inv_sin_al*inv_sin_al);
    return C1;
  }

  // --------------------------------------------------------------------- Gamma2

  complex<double> BeamModels::Gamma2 (double tan_al,
				      double sin_al,
				      double sin_th,
				      double cos_th,
				      double cos_ph,
				      double k,
				      double h,
				      double L)
  {
   double A=-sin_th*cos_ph-cos_th/tan_al;
   double inv_sin_al=1/sin_al;
   double tmp=k*A*L;
   double Re1=cos(tmp)*inv_sin_al;
   double Im1=-sin(tmp)*inv_sin_al;

   Re1 =Re1-inv_sin_al*cos(k*L*inv_sin_al);
   Im1 =Im1+A*sin(k*L*inv_sin_al);

   complex<double> C1=Re1+Im1*_Complex_I;

   complex<double> C2=cos(k*h*cos_th)+sin(k*h*cos_th)*_Complex_I;
   
   C1=-C1*C2/(A*A-inv_sin_al*inv_sin_al);

   return C1;
}

  // --------------------------------------------------------------------- Gamma3

  complex<double> BeamModels::Gamma3 (double tan_al,
				      double sin_al,
				      double sin_th,
				      double cos_th,
				      double cos_ph,
				      double k,
				      double h,
				      double L)
  {
    double A=-sin_th*cos_ph+cos_th/tan_al;
    double inv_sin_al=1/sin_al;
    double tmp=k*A*L;
    double Re1=cos(tmp)*inv_sin_al;
    double Im1=-sin(tmp)*inv_sin_al;
    
    Re1 =Re1-inv_sin_al*cos(k*L*inv_sin_al);
    Im1 =Im1+A*sin(k*L*inv_sin_al);
    
    complex<double> C1=Re1+Im1*_Complex_I;
    
    complex<double> C2=cos(k*h*cos_th)-sin(k*h*cos_th)*_Complex_I;
    
    C1=-C1*C2/(A*A-inv_sin_al*inv_sin_al);

    return C1;
  }
  
  // --------------------------------------------------------------------- Gamma4

  complex<double> BeamModels::Gamma4 (double tan_al,
				      double sin_al,
				      double sin_th,
				      double cos_th,
				      double cos_ph,
				      double k,
				      double h,
				      double L)
  {
    double A=sin_th*cos_ph+cos_th/tan_al;
    double inv_sin_al=1/sin_al;
    double tmp=k*A*L;
    double Re1=cos(tmp)*inv_sin_al;
    double Im1=sin(tmp)*inv_sin_al;
    
    Re1 =Re1-inv_sin_al*cos(k*L*inv_sin_al);
    Im1 =Im1-A*sin(k*L*inv_sin_al);
    
    complex<double> C1=Re1+Im1*_Complex_I;
    
    complex<double> C2=cos(k*h*cos_th)-sin(k*h*cos_th)*_Complex_I;
    
    C1=-C1*C2/(A*A-inv_sin_al*inv_sin_al);

    return C1;
  }

  // -------------------------------------------------------------- droopy_dipole

  double BeamModels::droopy_dipole (const double *par,
				    const double *x)
  {
    // Check if we are below the horizon
    if (x[3]<=0.0) {
      return 0;
    }

    const double c=3.e8;
    double theta=M_PI/2-x[3];
    //if (x[3]<=0.2) theta=M_PI/2-0.2; /* below horizon */
    const double phi=par[3]+x[2]; /* take orientation into account */
    
    /* some essential constants */
    double k=2*M_PI*x[1]/c;
    
    /* calculate needed trig functions */
    double tan_al=tan(par[2]);
    double sin_al=sin(par[2]);
    double cos_al=cos(par[2]);
    double sin_th=sin(theta);
    double cos_th=cos(theta);
    double sin_ph=sin(phi);
    double cos_ph=cos(phi);
    
    /* mu/4PI=10e-7  x omega/sin(alpha)*/
    const double A=(1e-7)*2*M_PI*x[1]/sin_al;
    
    complex<double> tmp=Gamma1(tan_al,sin_al,sin_th,cos_th,cos_ph,k,par[0],par[1]);
    complex<double> Eth=A*tmp*(cos_al*sin_th-sin_al*cos_th*cos_ph);
    complex<double> Eph=A*tmp*(sin_al*sin_ph);
    
    tmp=Gamma2(tan_al,sin_al,sin_th,cos_th,cos_ph,k,par[0],par[1]);
    Eth+=A*tmp*(-cos_al*sin_th-sin_al*cos_th*cos_ph);
    Eph+=A*tmp*(sin_al*sin_ph);
    
    tmp=Gamma3(tan_al,sin_al,sin_th,cos_th,cos_ph,k,par[0],par[1]);
    Eth+=A*tmp*(-cos_al*sin_th+sin_al*cos_th*cos_ph);
    Eph+=A*tmp*(-sin_al*sin_ph);
    
    tmp=Gamma4(tan_al,sin_al,sin_th,cos_th,cos_ph,k,par[0],par[1]);
    Eth+=A*tmp*(cos_al*sin_th+sin_al*cos_th*cos_ph);
    Eph+=A*tmp*(-sin_al*sin_ph);

    /*
      The final lines of the routine had to be modified w.r.t. the original
      code to make it work:
      
      const double result=Eth*Eth+Eph*Eph;
      return (sqrt(cabs(result)));
    */
    
    double result=abs(Eth*Eth+Eph*Eph);
    return (sqrt(result));
  }
  
} // Namespace CR -- end
