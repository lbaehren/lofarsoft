
#include <iostream>
#include <cmath>
#include <Math/NR.h>

namespace LOPES {  // NAMESPACE LOPES -- BEGIN

  double NRerfcc (double x)
  {
    double t (0.0);
    double z (0.0);
    double ans (0.0);
    
    z=fabs(x);
    t=1.0/(1.0+0.5*z);
    ans=t*exp(-z*z-1.26551223+t*(1.00002368+t*(0.37409196+t*(0.09678418+
	    t*(-0.18628806+t*(0.27886807+t*(-1.13520398+t*(1.48851587+
            t*(-0.82215223+t*0.17087277)))))))));
    return x >= 0.0 ? ans : 2.0-ans;
  }
  
  double NRgammln (double xx)
  {
    int j (0);
    double x (0.0);
    double y (0.0);
    double tmp (0.0);
    double ser (0.0);
    static double cof[6]={76.18009172947146,-86.50532032941677,
			  24.01409824083091,-1.231739572450155,
			  0.1208650973866179e-2,-0.5395239384953e-5};  
    y=x=xx;
    tmp=x+5.5;
    tmp -= (x+0.5)*log(tmp);
    ser=1.000000000190015;
    for (j=0;j<=5;j++) ser += cof[j]/++y;
    return -tmp+log(2.5066282746310005*ser/x);
  }
  
  double NRbetacf (double a,
		   double b,
		   double x)
  {
    int m,m2;
    double aa,c,d,del,h,qab,qam,qap;
    
    qab=a+b;
    qap=a+1.0;
    qam=a-1.0;
    c=1.0;
    d=1.0-qab*x/qap;
    if (fabs(d) < FPMIN) d=FPMIN;
    d=1.0/d;
    h=d;
    for (m=1;m<=MAXIT;m++) {
      m2=2*m;
      aa=m*(b-m)*x/((qam+m2)*(a+m2));
      d=1.0+aa*d;
      if (fabs(d) < FPMIN) d=FPMIN;
      c=1.0+aa/c;
      if (fabs(c) < FPMIN) c=FPMIN;
      d=1.0/d;
      h *= d*c;
      aa = -(a+m)*(qab+m)*x/((a+m2)*(qap+m2));
      d=1.0+aa*d;
      if (fabs(d) < FPMIN) d=FPMIN;
      c=1.0+aa/c;
      if (fabs(c) < FPMIN) c=FPMIN;
      d=1.0/d;
      del=d*c;
      h *= del;
      if (fabs(del-1.0) < EPS) break;
    }
    if (m > MAXIT) {
      std::cerr << "[betacf] ";
      std::cerr << "a or b too big, or MAXIT too small in betacf" << std::endl;
    }
    return h;
  }
  
  double NRbetai(double a,
		 double b,
		 double x)
  {
    double bt;
    
    if (x < 0.0 || x > 1.0) {
      std::cerr << "[NRbetai] Bad x in routine betai" << std::endl;
    }
    if (x == 0.0 || x == 1.0) bt=0.0;
    else
      bt=exp(NRgammln(a+b)-NRgammln(a)-NRgammln(b)+a*log(x)+b*log(1.0-x));
    if (x < (a+1.0)/(a+b+2.0))
      return bt*NRbetacf(a,b,x)/a;
    else
      return 1.0-bt*NRbetacf(b,a,1.0-x)/b;
  }
  
} // NAMESPACE LOPES -- END
