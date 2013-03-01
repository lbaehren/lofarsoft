/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "linchi2.h"
#include <cmath>
#include <iostream>

using namespace std;

static double gammln(const double xx)
{
  int j;
  double x,y,tmp,ser;
  static const double cof[6] = {76.18009172947146, -86.50532032941677,
			       24.01409824083091, -1.231739572450155,
			       0.1208650973866179e-2, -0.5395239384953e-5};

  y = x = xx;
  tmp = x + 5.5;
  tmp -= (x+0.5)*log(tmp);
  ser = 1.000000000190015;
  
  for (j = 0; j < 6; j++)
    ser += cof[j]/++y;
  
  return -tmp+log(2.5066282746310005*ser/x);
}

static void gser(double& gamser, const double a, const double x, double& gln)
{
  const int ITMAX = 100;
  const double EPS = 0.0001;

  int n;
  double sum,del,ap;

  gln = gammln(a);

  if (x <= 0.0) {
    if (x < 0.0)
      cerr << "gser x less than zero" << endl;
    gamser = 0.0;
    return;
  }
  else {
    ap = a;
    del = sum = 1.0/a;
    for (n = 0; n < ITMAX; n++) {
      ++ap;
      del += x/ap;
      sum += del;
      if (fabs(del) < fabs(sum)*EPS) {
	gamser = sum*exp(-x+a*log(x)-gln);
	return;
      }
    }
    cerr << "gser a too large, ITMAX too small" << endl;
    return;
  }
}

static void gcf(double& gammcf, const double a, const double x, double& gln)
{
  const int ITMAX = 100;
  const double EPS = 0.0001;
  const double FPMIN = 0.0000001;
  
  // ITMAX is the maximum allowed number of iterations; EPS is the
  // relative accuracy; FPMIN is a number near the smallest
  // representable double
  
  int i;
  double an,b,c,d,del,h;
  
  gln = gammln(a);
  b = x + 1.0 - a;
  c = 1.0/FPMIN;
  d = 1.0/b;
  h = d;

  for (i = 1; i <= ITMAX; i++) {
    an = -i*(i-a);
    b += 2.0;
    d = an*d + b;
    if (fabs(d) < FPMIN)
      d = FPMIN;
    c = b + an/c;
    if (fabs(c) < FPMIN)
      c = FPMIN;
    d = 1.0/d;
    del = d*c;
    h *= del;
    if (fabs(del-1.0) <= EPS)
      break;
  }

  if (i > ITMAX) {
    cerr << "gcf a too large, ITMAX too small" << endl;
    return;
  }
  
  gammcf = exp(-x+a*log(x)-gln)*h;
  
}

static double gammq(const double a, const double x)
{
  double gamser=0, gammcf=0, gln=0;
  
  if (x < 0.0 || a <= 0.0) {
    cerr << "gammq invalid arguments" << endl;
    return 0.0;
  }
  if (x < a+1.0) {
    gser(gamser, a, x, gln);
    return 1.0-gamser;
  }
  else {
    gcf(gammcf, a, x, gln);
    return gammcf;
  }
}

void lin_chi2_fit(const vector<double>& x, const vector<double>& y,
                  const vector<double>& sig, 
		  bool mwt, double& a, double& b, double& siga, double& sigb, 
		  double& chi2, double& q)
{
  int i;
  double wt, t, sxoss, sx = 0.0, sy= 0.0, st2 = 0.0, ss, sigdat;

  int ndata = x.size();
  b = 0.0;

  if (mwt) {
    ss = 0.0;
    for (i = 0; i < ndata; i++) {
      wt = 1.0/(sig[i]*sig[i]);
      ss += wt;
      sx += x[i]*wt;
      sy += y[i]*wt;
    }
  }
  else {
    for (i = 0; i < ndata; i++) {
      sx += x[i];
      sy += y[i];
    }
    ss = ndata;
  }
  
  sxoss = sx/ss;

  if (mwt) {
    for (i = 0; i < ndata; i++) {
      t = (x[i] - sxoss) / sig[i];
      st2 += t*t;
      b += t*y[i] / sig[i];
    }
  }
  else {
    for (i = 0; i < ndata; i++) {
      t = x[i] - sxoss;
      st2 += t*t;
      b += t*y[i];
    }
  }

  b /= st2;
  a = (sy-sx*b)/ss;
  siga = sqrt((1.0+sx*sx/(ss*st2))/ss);
  sigb = sqrt(1.0/st2);
  chi2 = 0.0;
  q = 1.0;

  if (!mwt) {
    for (i = 0; i < ndata; i++)
      chi2 += ((y[i] - a - b*x[i])*(y[i] - a - b*x[i]));
    sigdat = sqrt(chi2/(ndata-2));
    siga *= sigdat;
    sigb *= sigdat;
  }
  else {
    for (i = 0; i < ndata; i++)
      chi2 += ((y[i] - a - b*x[i])/sig[i])*((y[i] - a - b*x[i])/sig[i]);
    if (ndata > 2)
      q = gammq(0.5*(ndata - 2), 0.5*chi2);
  }
}

