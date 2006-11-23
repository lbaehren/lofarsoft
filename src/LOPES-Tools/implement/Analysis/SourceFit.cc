/***************************************************************************
 *   Copyright (C) 2005 by Sven Lafebre                                    *
 *   s.lafebre@astro.ru.nl                                                 *
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

#include <fstream>
#include <sstream>
#include <iostream>

#include <Analysis/SourceFit.h>

#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}
#define c 3.7474057
#define pi 3.1415926536

using std::cout;
using std::cerr;
using std::endl;

namespace LOPES { // Namespace LOPES -- begin
  
  void SourceFit::minimize(PeakList *p) {
    
    cout << "+ minimize" << endl;
    init();
    int i = 0;
    do {    
      chiSqrOld = chiSqr;
      cout << "Iteration " << (++i) << endl;
      marquardtMinimize(p);
    } while ( chiSqr != 0 &&
	      ( chiSqrOld < chiSqr ||
		( chiSqrOld-chiSqr  > 0.01 &&
		  chiSqrOld/chiSqr > 1.001
		  ) ) );
    cout << "- minimize" << endl;
  }
  
  void SourceFit::init() {
    cout << "+ init" << endl;
    noFitParam = 0;
    
    r0.resize(4);
    r0(0) = 0;      // x coordinate
    r0(1) = 0;      // y coordinate
    r0(2) = 1000;   // z coordinate
    r0(3) = 1000;   // distance to detector
    
    doFitParam.resize(r0.shape().asVector()(0));
    for (int j = 0; j < r0.shape().asVector()(0); j ++) {
      doFitParam(j) = true;
      noFitParam ++;
    }
    
    covar.resize(noFitParam,noFitParam);
    alpha.resize(noFitParam,noFitParam);
    
    r.resize(8,3);
    r(0,0) =   0          ; r(0,1) =   0         ; r(0,2) =  0             ;
    r(1,0) =  18.36400032 ; r(1,1) =  31.92000008; r(1,2) = -0.06199645996 ;
    r(2,0) = -13.57299995 ; r(2,1) =  50.19499969; r(2,2) = -0.1319961548  ;
    r(3,0) =   4.58799934 ; r(3,1) =  82.16400146; r(3,2) = -0.002998352051;
    r(4,0) = -34.13400269 ; r(4,1) = 125.6149979 ; r(4,2) = -0.07699584961 ;
    r(5,0) =  -1.940999985; r(5,1) = 107.3110046 ; r(5,2) = -0.05499267578 ;
    r(6,0) =  36.43399811 ; r(6,1) =  63.68000031; r(6,2) = -0.1549987793  ;
    r(7,0) =  54.70999908 ; r(7,1) =  95.71699524; r(7,2) = -0.2219924927  ;
    //r(8,0) = -70.66899872 ; r(8,1) =  61.49099731; r(8,2) = -0.03699493408 ;
    //r(9,0) = -38.63100052 ; r(9,1) =  43.24399948; r(9,2) = -0.1079940796  ;
    
    alamda = -1;
    
    cout << "- init" << endl;
  }
  
  /*void SourceFit::init() {
    cout << "+ init" << endl;
    noFitParam = 0;
    
    r0.resize(4);
    r0(0) = 0;
    r0(1) = 0;
    r0(2) = 100;
    r0(3) = 100;
    
    doFitParam.resize(r0.shape().asVector()(0));
    for (int j = 0; j < r0.shape().asVector()(0); j ++) {
    doFitParam(j) = true;
    noFitParam ++;
    }
    
    covar.resize(noFitParam,noFitParam);
    alpha.resize(noFitParam,noFitParam);
    
    r.resize(5,3);
    r(0,0) =    0 ; r(0,1) =    0 ; r(0,2) =  0;
    r(1,0) =  200 ; r(1,1) =  200 ; r(1,2) =  0;
    r(2,0) = -200 ; r(2,1) =  200 ; r(2,2) =  0;
    r(3,0) =  200 ; r(3,1) = -200 ; r(3,2) =  0;
    r(4,0) = -200 ; r(4,1) = -200 ; r(4,2) =  0;
    
    alamda = -1;
    
    cout << "- init" << endl;
    }*/
  
  void SourceFit::fitFunction(int k,
			      Vector<double> r0,
			      double& t,
			      Vector<double>& dtdr0)
  {
    cout << "+ fitFunction" << endl;
    
    double dist = (r(k,0)-r0(0))*(r(k,0)-r0(0)) +
      (r(k,1)-r0(1))*(r(k,1)-r0(1)) +
      (r(k,2)-r0(2))*(r(k,2)-r0(2));
    //cout << r(k,0) << " " << r(k,1) << " " << r(k,2) << endl;
    //cout << r0(0)  << " " << r0(1)  << " " << r0(2)  << endl;
    dist = sqrt(dist);
    t = (dist-r0(3))/c;
    
    for (int i = 0; i < 3; i ++) {
      dtdr0(i) = (r0(i)-r(k,i))/(c*dist);
    }
    dtdr0(3)    = -1/c;
    
    cout << "- fitFunction" << endl;
  }
  
  void SourceFit::marquardtMinimize(PeakList *p) {
    int j,k,l;
    int n = r0.shape().asVector()(0);
    
    cout << "+ marquardtMinimize" << endl;
    if (alamda < 0.0) {
      r0Try.resize(n);
      beta .resize(n);
      da   .resize(n);
      for (noFitParam = 0, j = 0; j < n; j ++) {
	if (doFitParam(j)) noFitParam ++;
      }
      oneda.resize(noFitParam,1);
      alamda = 0.001;
      marquardtMatrix(p,r0,alpha,beta);
      chiSqrOld = chiSqr;
      for (j = 0; j < n; j ++) {
	r0Try(j) = r0(j);
      }
    }
    
    for (j = 0; j < noFitParam; j ++) {
      for (k = 0; k < noFitParam; k ++) {
	covar(j,k) = alpha(j,k);
      }
      covar(j,j) = alpha(j,j)*(1.0+alamda);
      oneda(j,0) = beta(j);
    }
    
    gaussJordan(covar, oneda);
    for (j = 0; j < noFitParam; j ++) {
      da(j)    = oneda(j,0);
    }
    
    cout << "1 marquardtMinimize" << endl;
    if (alamda == 0.0) {
      expandCovariance(covar);
      expandCovariance(alpha);
      return;
    }
    
    for (j = -1, l = 0; l < n; l ++)
      if (doFitParam(l)) r0Try(l) = r0(l) + da(++j);
    
    marquardtMatrix(p,r0Try,covar,da);
    
    if (chiSqr < chiSqrOld) {
      alamda *= 0.1;
      chiSqrOld = chiSqr;
      for (j = 0; j < noFitParam; j ++) {
	for (k = 0; k < noFitParam; k ++)
	  alpha(j,k) = covar(j,k);
	beta(j) = da(j);
      }
      r0 = r0Try;
    } else {
      alamda *= 10.0;
      chiSqr = chiSqrOld;
    }
    cout << "r0    = [" << r0(0) << ", " << r0(1) << ", " << r0(2) << "]" << endl;
    double ds = sqrt(r0(0)*r0(0)+r0(1)*r0(1)+r0(2)*r0(2));
    double az = -atan(r0(0)/r0(1));
    double el = asin(r0(2)/ds);
    cout << "az,el,r = [" << az/pi*180 << ", " << el/pi*180 << ", " << ds << "]" << endl;
    cout << "- marquardtMinimize" << endl;
  }
  
  // mrqcof
  void SourceFit::marquardtMatrix(PeakList*       p,
				  Vector<double>& a,
				  Matrix<double>& al,
				  Vector<double>& be)
  {
    int j,k,l,m;
    int n = a.shape().asVector()(0);
    double tmod,wt,dt;
    Vector<double> dtdr0(n);
    
    cout << "+ marquardtMatrix" << endl;
    for (j = 0; j < noFitParam; j ++) {
      for (k = 0; k < noFitParam; k ++) {
	al(j,k) = 0.0;
      }
      be(j) = 0.0;
    }
    
    chiSqr = 0.0;
    for (uint i = 0; i < p->length(); i ++) {
      if (p->peak(i)->significance() > 0) {
	fitFunction(i, a, tmod, dtdr0);
	//sig2i = 1.0/(sig(i)*sig(i));
	cout << "tmod = " << tmod << endl;
	dt = p->peak(i)->time() - tmod;
	for (j = -1, l = 0; l < n; l ++) {
	  if (doFitParam(l)) {
	    //wt = dtdr0(l)*sig2i;
	    wt = dtdr0(l);
	    for (j ++, k = -1, m = 0; m <= l; m ++) {
	      if (doFitParam(m)) {
		al(j,++k) += wt*dtdr0(m);
	      }	}
	    be(j) += dt*wt;
	  }
	}
	//chiSqr += dt*dt*sig2i;
	chiSqr += dt*dt;
      }	}
    for (j = 1; j < noFitParam; j ++) {
      for (k = 0; k <= j; k ++) {
	al(k,j) = al(j,k);
      }	}
    cout << "- marquardtMatrix" << endl;
  }
  
  void SourceFit::expandCovariance(Matrix<double>& covar)
  {
    int i, j, k;
    int n = covar.nrow();
    
    double temp;
    
    cout << "+ expandCovariance" << endl;
    for (i = noFitParam; i < n; i ++) {
      for (j = 0; j <= i; j ++) {
	covar(i,j) = 0.0;
	covar(j,i) = 0.0;
      }	}
    
    k = noFitParam;
    for (j = n - 1; j >= 0; j --) {
      if (doFitParam(j)) {
	for (i = 0; i < n; i ++)
	  SWAP(covar(i,k), covar(i,j))
	    for (i = 0; i < n; i ++)
	      SWAP(covar(k,i), covar(j,i))
		k --;
      }
    }
    cout << "- expandCovariance" << endl;
  }
  
  void SourceFit::gaussJordan(Matrix<double>& a,
			      Matrix<double>& b)
  {
    int n = a.nrow();
    int m = b.ncolumn();
    int i,j,k,l,ll;
    int icol (0), irow(0);
    double big (0.0);
    double dum (0.0);
    double pivinv (0.0);
    double temp (0.0);
    
    Vector<int> indxc(n);
    Vector<int> indxr(n);
    Vector<int> ipiv (n);
    
    cout << "+ gaussJordan" << endl;
    cout << "n = " << n << endl;
    for (j = 0; j < n; j ++) {
      ipiv(j) = 0;
    }
    cout << ". gaussJordan" << endl;
    for (i = 0; i < n; i ++) {
      big = 0.0;
      for (j = 0; j < n; j ++) {
	if (ipiv(j) != 1) {
	  for (k = 0; k < n; k ++) {
	    if (ipiv(k) == 0) {
	      if (fabs(a(j,k)) >= big) {
		big  = fabs(a(j,k));
		irow = j;
		icol = k;
	      }
	    } else if (ipiv(k) > 1) {
	      throw string("GaussJordan: Singular Matrix-1");
	    }
	  }	}	}
      
      cout << "  " << i << " gaussJordan" << endl;
      ++ (ipiv(icol));
      if (irow != icol) {
	for (l = 0; l < n; l ++) SWAP(a(irow,l), a(icol,l))
	  for (l = 0; l < m; l ++) SWAP(b(irow,l), b(icol,l))
	    }
      cout << "  " << i << " gaussJordan" << endl;
      indxr(i) = irow;
      indxc(i) = icol;
      if (a(icol,icol) == 0.0)
	throw string("GaussJordan: Singular Matrix-2");
      
      cout << "a(icol,icol) = " << a(icol,icol) << endl;
      pivinv = 1.0/a(icol,icol);
      a(icol,icol) = 1.0;
      cout << "  " << i << " gaussJordan" << endl;
      for (l  = 0; l  < n; l  ++) a(icol,l) *= pivinv;
      for (l  = 0; l  < m; l  ++) b(icol,l) *= pivinv;
      for (ll = 0; ll < n; ll ++) {
	if (ll != icol) {
	  dum = a(ll,icol);
	  a(ll,icol) = 0.0;
	  for (l = 0; l < n; l ++) a(ll,l) -= a(icol,l)*dum;
	  for (l = 0; l < m; l ++) b(ll,l) -= b(icol,l)*dum;
	}
      }
      cout << "  " << i << " gaussJordan" << endl;
    }
    cout << ". gaussJordan" << endl;
    for (l = n-1; l >= 0; l --) {
      if (indxr(l) != indxc(l))
	for (k = 0; k < n; k ++)
	  SWAP(a(k,indxr(l)), a(k,indxc(l)));
    }
    cout << "- gaussJordan" << endl;
  }
  
} // Namespace LOPES -- end
