// File rmNumUtils is a collection of numerical utilities for 
// performing RM synthesis.

/* Standard header files */
#include <complex>
#include <vector>
#include <stack>
#include <fstream>
#include <algorithm>			// for minimum element in vector
#include <limits>			// limit for S infinite
#include <iomanip>			// to set precision in cout
#include <fftw3.h>  // need for algrithm which 
#include <sstream>
/* RM header files */
#include "rmNumUtils.h"

using namespace std;

namespace RM {
  
  //_____________________________________________________________________________
  //                                                                     findGaps
  
  /*!
    \param freqs   -- Ordered double valued vector to find the gaps in
    \retval result -- 
  */
  void findGaps (vector<double> &freqs,
		 vector<uint> &result)
  {
    vector<double> mean(2) ;
    double epsilon = 1+5E-8 ;
    calcMeanVarDist(freqs,mean) ;
    double meanDist = mean[0] ;
    double varDist = mean[1] ;
    result.push_back(0) ; // always put the first element into the gaps 
    /* loop over all elements of the given vector, to look at the distances
       and to find the gaps, where the positions are stored into result */
    for (uint i=0; i<freqs.size()-1; i++) {
      /* a gap is detected for intervallength more than mean length plus 3 times 
	 the variance of the intervallenths */
      if ((freqs[i+1]-freqs[i]) > (epsilon*meanDist+3*varDist)) {
	result.push_back(i+1) ; 
      }
    }
    result.push_back(freqs.size()) ; // always put the first element into the gaps 
  }
  
  
  //_____________________________________________________________________________
  //                                                              calcMeanVarDist
  
  /*!
    Procedure to calculate the mean separation and varinace of the  
    separations of the element of an ordered  real valued vector 

    \param freqs ordered real valued vector (double precission ) 
    \param result vector conatains the mean [0] and the variance[1]
  */
  void calcMeanVarDist (vector<double> &freqs,
			vector<double> &result)
  {
    double mean = 0;
    double var  = 0;
    int num     = freqs.size()-1;
    for (int i=0; i<num ; i++) {
      double diff = freqs[i+1]-freqs[i] ;
      mean = mean+diff ;
      var = var+diff*diff ;
    }
    result[0] = mean/num ;
    result[1] = sqrt((var-mean*mean/num)/num);
  }
  
  complex<double> ehoch(double phi){
    complex<double> res=complex<double>(cos(phi), sin(phi));	
    return res ;
  }
  /*! procedure which performs the fast fourier 
    transformation on a complex vector of c++ stl .*/ 
  void FFT(vector<complex<double> > &input, vector<complex<double> > &result) {
    uint SIZE=input.size() ;
    double fak = 1.0/sqrt(SIZE) ;
    if (SIZE == result.size()) {
      fftw_complex    *data, *fft_result;
      fftw_plan       plan_forward;
      uint             i;
      
      data        = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * SIZE );
      fft_result  = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * SIZE );
      
      plan_forward  = fftw_plan_dft_1d( SIZE, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE );
      
      /* populate input data */
      for( i = 0 ; i < SIZE ; i++ ) {
        data[i][0] = input[i].real();
        data[i][1] = input[i].imag();
      }
      
      fftw_execute( plan_forward );
      
      /* store resutl to reslult vector */
      for( i = 0 ; i < SIZE ; i++ ) {
        result[i] = fak*complex<double>(fft_result[i][0],fft_result[i][1]) ;
      }
      
      /* free memory */
      fftw_destroy_plan( plan_forward );
      fftw_free( data );
      fftw_free( fft_result );
    }
    else {
      cerr << "FFT can not be performed, dimension error " << endl ;
      throw "FFT can not be performed, dimension error ";
    }
  }
  /*! procedure which performs the incerse fast fourier 
    transformation on a complex vector of c++ stl .*/ 
  void iFFT(vector<complex<double> > &input, vector<complex<double> > &result) {
    uint SIZE=input.size() ;
    double fak = 1.0/sqrt(SIZE) ;
    if (SIZE == result.size()) {
      fftw_complex    *fft_result, *ifft_result;
      fftw_plan        plan_backward;
      uint             i;
      
      
      fft_result  = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * SIZE );
      ifft_result = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * SIZE );
      
      plan_backward = fftw_plan_dft_1d( SIZE, fft_result, ifft_result, FFTW_BACKWARD, FFTW_ESTIMATE );
   
      /* populate input data */
      for( i = 0 ; i < SIZE ; i++ ) {
        fft_result[i][0] = input[i].real();
        fft_result[i][1] = input[i].imag();
      }  
 
      fftw_execute( plan_backward );

      /* store resutl to reslult vector */
      for( i = 0 ; i < SIZE ; i++ ) {
        result[i] = fak*complex<double>(ifft_result[i][0],ifft_result[i][1]) ;
      } 
      /* free memory */
      fftw_destroy_plan( plan_backward );
      fftw_free( fft_result );
      fftw_free( ifft_result );
   }
   else {
     cerr << "FFT can not be performed, dimension error " << endl ;
     throw "FFT can not be performed, dimension error ";
   }
}

/*!
  \brief Convert from frequencies to lambda squared
  \param frequency - frequency vector 
  \return lambda_sq - converted lambda squared vector
*/
void freqToLambdaSq(const vector<double> &center, const vector<double> &intervals, vector<double> &lcenter, vector<double> &linterv)
{
  // lambda squared to be calculated, same size as frequency vector
  lcenter.resize(center.size());
  linterv.resize(intervals.size());	
  // constants
  const double csq=89875517873681764.0;	// c^2  
  // loop over frequency vector and compute lambda squared values
  for(unsigned int i=center.size(); i>0; i--)
  {
     /* calc minmal and maximal frequency of the current interval */
     double minf= center[i-1]-0.5*intervals[i-1] ;
     double maxf= center[i-1]+0.5*intervals[i-1] ;
     double minl= csq/(maxf*maxf) ;
     double maxl= csq/(minf*minf) ;
     lcenter[center.size()-i] = 0.5*(maxl+minl) ;
     linterv[center.size()-i] = (maxl-minl) ;
  }
}

/*! procdure searchs the boundaries to analyse the maximal peak of the polarised 
    emission in dependence of the faraday depth.
    The procedure therfore searches for a position there curve of the absolute values of 
    the polarised emission becomes flat of shows a minimum.
    \param data polarised emisssion
    \param max index of the maximum of the intensity of the polarised emission 
    */
uint searchPeakBound(cvec &data, uint max) {
  uint up = 0 ;
  uint down = 0 ;
  double refu = abs(abs(data[max+1])-abs(data[max]))/abs(data[max]) ;
  bool found = false;
  bool start = false;
  /* search for nearly flat curve or minimum */
  for (uint i=max; (i<data.size()-2) && !found; i++) {
    double val1 = (abs(data[i+2])-abs(data[i+1]))*(abs(data[i+1])-abs(data[i])) ;
    double val2 = abs(abs(data[i+1])-abs(data[i]))/abs(data[max]) ;
    found = start && !((val1>0) && (val2>refu));    
    if (((val1<0) || (val2>refu))) start = true ;
    if (found) up=i-max ;
  }
  found = false ;
  start = false ;
  for (uint i=max; (i>2) && !found; i--) {
    double val1 = (abs(data[i-2])-abs(data[i-1]))*(abs(data[i-1])-abs(data[i])) ;
    double val2 = abs(abs(data[i-1])-abs(data[i]))/abs(data[max]) ;
    found = start && !((val1>0) && (val2>refu));
    if (((val1<0) || (val2>refu))) start = true ;
    if (found) down=max-i ;
  }
  uint erg = min(up,down) ;
  return erg ;
}

/*! procedure searchs the boundaries to analyse the maximal peak of the polarised 
    emission in dependence of the faraday depth.
    The procedure therfore searches for a position there curve of the absolute values of 
    the polarised emission becomes flat of shows a minimum.
    \param data polarised emisssion
    \param max index of the maximum of the intensity of the polarised emission 
    \param down return index for low interval boundary of the peak
    \param up return index for the upper interval boundary for the peak
    */
void searchPeakBound(cvec &data, uint max, uint &down, uint &up) {
  up = 0 ;
  down = 0 ;
  double refu = abs(abs(data[max+1])-abs(data[max]))/abs(data[max]) ;
  bool found = false;
  bool start = false;
  /* search for nearly flat curve or minimum */
  for (uint i=max; (i<data.size()-2) && !found; i++) {
    double val1 = (abs(data[i+2])-abs(data[i+1]))*(abs(data[i+1])-abs(data[i])) ;
    double val2 = abs(abs(data[i+1])-abs(data[i]))/abs(data[max]) ;
    found = start && !((val1>0) && (val2>refu));    
    if (((val1<0) || (val2>refu))) start = true ;
    if (found) up=i-max ;
  }
  found = false ;
  start = false ;
  for (uint i=max; (i>2) && !found; i--) {
    double val1 = (abs(data[i-2])-abs(data[i-1]))*(abs(data[i-1])-abs(data[i])) ;
    double val2 = abs(abs(data[i-1])-abs(data[i]))/abs(data[max]) ;
    found = start && !((val1>0) && (val2>refu));
    if (((val1<0) || (val2>refu))) start = true ;
    if (found) down=max-i ;
  }
}

/*! procedure integrates the function abs(data) from the index min,
    to the index max over the variable faras using the trapezoidal rule
    \param data values for the function to be integrated
    \param faras faraday depths to integrate over
    \param min index the integration starts
    \param max index the integration stops */

double integr(double sigma, vector<double> &data, vector<double> &faras, uint min, uint center, uint max) {
  double erg = 0 ;
  for (uint i=min; i<=max; i++) { // loop over the integration interval
    double var = (faras[i]-faras[center])*(faras[i]-faras[center]) ;
    double val1 = data[i-min]*exp(-(var)/(sigma*sigma))*var/(sigma*sigma*sigma) ;
    var = (faras[i+1]-faras[center])*(faras[i+1]-faras[center]) ;
    double val2 = data[i+1-min]*exp(-(var)/(sigma*sigma))*var/(sigma*sigma*sigma) ;
    erg = erg + 0.5*(val1+val2)*(faras[i+1]-faras[i]) ;
  }
  return erg ;
}

/*! procedure calculates the analytical components of the intgral to minimize 
    the L2 Norm of the difference from absolute square of the given function and 
    the fitting gauss.
    \param data values for the function to be integrated
    \param faras faraday depths to integrate over
    \param min index the integration starts
    \param max index the integration stops */
double calcAnalPart(double sigma, vector<double> &data, vector<double> &faras, uint min, uint center, uint max) {
  double f0 = data[center-min] ;
  double e =  0.5* (faras[max]-faras[min]) ;
  double erg = -2*f0*f0*e/sigma*exp(-2*e*e/(sigma*sigma)) ;
  erg = erg + sqrt(0.5*PI)*f0*f0 * gsl_sf_erf(sqrt(2)*e/sigma);
  return erg ;
} 

double calcAll(double sigma, vector<double> &data, vector<double> &faras, uint min, uint center, uint max) {
  return calcAnalPart(sigma, data, faras, min, center, max) -4*abs(data[center-min])*integr(sigma, data, faras, min, center, max);
}

/*! Procedure finds the optimal sigma for the best fit of a gauss distribution to the 
    absolute function near to it's maximum. This is done by finding the root of the 
    first derivation of the L2 Norm of the function: abs(data)- gauss(sigma)
    with respect to sigma. The root is found by a simple bisection method.
    \param sigma1 minimal possible sigma
    \param sigma2 maximal possible sigma 
    \param data vector of the data which have to be fitted
    \param faras vector of the fararaday depths which belong to the vector of data
    \param min minimal index where the fit of the data point is to be done
    \param center index of the maximum, where maximum of the gauss curve has to be set
    \param max maxima index where the fit is to be done  
    \param eps accuracy to find the optimal point */
double findOptSig(double sigma1, double sigma2, vector<double> &data, vector<double> &faras, uint min, uint center, uint max, double eps) {
  /* get the value for the two interval boundaries */
  double val1 = calcAll(sigma1,data,faras,min,center, max) ;
  double val2 = calcAll(sigma2,data,faras,min,center, max) ;
  if ((val1*val2) > 0) return sigma1 ; // now optimum can be found, so we assume a point in RM space
  /* continue to find the minimum */
  double sig1 = sigma1 ;
  double sig2 = sigma2 ; 
  double sigm = 0.5*(sig1+sig2) ;
  while ((sig2-sig1) > eps) {
    sigm = 0.5*(sig1+sig2) ; // get the next approximation for the optimum 
    double valm = calcAll(sigm,data,faras,min,center, max) ;
    if (valm*val1 < 0) {  // optimum is between valm and val1
      val2=valm ;
      sig2 = sigm ;  // sig1 stays the old value
    }
    else { // optimum is between valm and val2 
      val1=valm ;
      sig1 = sigm ; // sig2 stay the old value
    }
  }
  return sigm ;
}


/*! calculate the variance of the selected main peak using the
    estimator sum((x-x0)^2*f(x)) ;
    \param data vector to calculate the variance for 
    \param faraday independent (faradaydeth) for the peak to calculate the variance for */
double clacVar(vector<double> &data,vector<double> &faraday, uint lo, uint max, uint hi) {
  double var = 0 ;
  double center = faraday[max] ; 
  /* loop for calculating the integral,
     The data function is positive, because it comes from 
     an absolute function. So the simple integral is sufficient */
  double area = 0.0 ;
  for (uint i=0; i<data.size()-1; i++) {
    area = area+0.5*(data[i]+data[i+1])*(faraday[max-lo+i+1]-faraday[max-lo+i]);
  }
  /* loop for calculating the variance */
  for (uint i=0;i<data.size(); i++) {
    double dif = (faraday[max-lo+i]-center) ;
    var = var+dif*dif*data[i] ;
  }
  var = sqrt(var/area);
  return var ;
}
/*! procedure calculates the variance of a fitting gauss to the data vector 
    given by data. The fit is done to the absolute value of the datavector,
    at the position of the maximal absolute value of the data.
    \param data vector of the polarised emission at the faradaydepths
    \param faraday values of the faraday depths 
    \param useClean represents the used clean method
		1: just use a "point source" in faraday depth for cleaning 
		2: use a gauss with the same variance as the fitted peak
		3: use a gauss with the least square fit to the fitted peak 
    \param lo returns the lower end of the index interval which was taken into account
    \param index returns the index of the maximum
    \param hi returns the high index of the interval which was taken into account
    \return variance of the fitting gauss. */

double getVarForGauss(cvec data, vector<double> &faraday, int useClean, uint &lo, uint &index, uint &hi, double &max) {
  index = data.maxAbs() ; // return faraday depth for maximal absolute value 
//   cout << "test: " << index << endl ;
  max = faraday[index] ;  // return index for maximal absolute value
  hi=0 ; 
  lo=0 ;
  double sigma ;
  if (useClean == 1) {
//    searchPeakBound(data,index,lo,hi) ;
    sigma = 0.25*(faraday[index]-faraday[index-1]) ;
  }
  else if (useClean == 2) {
    searchPeakBound(data,index,lo,hi) ;
    vector<double> redData(lo+hi+1) ;
    /* get the values at the interval boundaries */
    double val1 = abs(data[index-lo]) ;
    double val2 = abs(data[index+hi]) ;
    double fak = 1.0/(hi+lo) ;  // faktor for linear interpolation between val1 and val2
    uint count = 0 ;
    /* get the values of the data at the boundaries, for subtracting them */
    for (uint i=index-lo; i<=index+hi; i++) {
      double fak2 = fak*count ;
      double fak1 = 1-fak2 ;
      count++ ; 
      redData[i-index+lo] = abs(data[i])-(fak1*val1+fak2*val2) ;
    }
    sigma = clacVar(redData,faraday,lo,index,hi) ;
//     cout << "test: " << index-lo << " " << index+hi << endl ;
//     cout << "sigma: " << sigma  << endl ;
  }
  else if (useClean == 3) {
    uint var = searchPeakBound(data,index) ;
    lo = var ;
    hi = var ;
    /* extract the peak data only, and make them be zero at it boundary */
    vector<double> redData(2*var+1) ;
    /* get the values at the interval boundaries */
    double val1 = abs(data[index-var]) ;
    double val2 = abs(data[index+var]) ;
    double fak = 1.0/(2*var) ;  // faktor for linear interpolation between val1 and val2
    uint count = 0 ;
    /* get the values of the data at the boundaries, for subtracting them */
    for (uint i=index-var; i<=index+var; i++) {
      double fak2 = fak*count ;
      double fak1 = 1-fak2 ;
      count++ ; 
      redData[i-index+var] = abs(data[i])-(fak1*val1+fak2*val2) ;
    }
    double sigma1 = faraday[index]-faraday[index-1] ;  
    double sigma2 = faraday[index+var] - faraday[index] ;
    sigma = findOptSig(sigma1,sigma2,redData,faraday,index-var,index, index+var,sigma1) ;
//     cout << "test: " << index-lo << " " << index+hi << endl ;
//     cout << "sigma1: " << sigma1 << "  sigma: " << sigma << "  sigma2: " << sigma2 << endl ;
  }
  else {
    sigma = (faraday[index]-faraday[index-1]) ;
  }
//   for (uint i=index-lo; i<index+hi; i++) {
//     cout << faraday[i] << " " << abs(data[i]) << " " << abs(data[index])*exp(-(faraday[i]-faraday[index])*(faraday[i]-faraday[index])/(sigma*sigma)) << endl ;
//   }
  return sigma;
}

/*! procedure makes a convolution of the point like source of the polarised emission (in 
    faradaydeth, with the center beam form. The center beam we assume to be a gauss like 
    beam, with a variance generated out of the observed differences in lambda squared direction */ 

void smoothPeak(cvec &peak, uint index, vector<double> &lcenter,vector<double> &linterv,vector<double> &faras, uint useClean) {
  if (useClean==1) { // smoothing only used for useClean method 1
    if (faras.size() != peak.size()) {
      cerr << "no matching dimensions in smoothPeak detected" << endl ;
      throw "no matching dimensions in smoothPeak detected" ;
    }
    /* calculate the variance for the gauss beam shape */ 
    double lmin = lcenter[0]-linterv[0];
    double lmax = lcenter[lcenter.size()-1]+linterv[linterv.size()];
    double faras0=faras[index] ;  // get the faraday depth of the point like source of polarised emission
    double df=2*sqrt(3)/(lmax-lmin) ;
    double var = df/(2*sqrt(log(2)));
    complex<double> peak0 = peak[index] ;  // value of the point like polarised emission
    /* loop for calculating the smoothed point source for adding up the clean rm image */
    for (uint i=0; i<faras.size();i++) {
      double far = (faras[i]-faras0)/var;
      peak[i] = exp(-(far*far))*peak0 ;
    }
  }
}

/*! proceure calculates the factor, which is lost by a forward and a bakward transformation
    of the rm-synthesis .
    \param peak is the peak, where the loss factor is calculated for
    \param faras vector for the faraday dephts
    \param lcenter vector for the centers of the lambda squared axis
    \param linterv vector for the intervallengths of the lambda squared axis
    \param nu_0 parameter nu_0
    \param alpha parameter alpha
    \param epsilon parameter epsilon
    \param method used method for the rm synthesis 
    \param index index of the maximal value of the peak */
complex<double> calcFakt(cvec &peak,vector<double> &faras, vector<double> &lcenter,vector<double> linterv,double nu_0, double alpha, double epsilon, uint method, uint index) {
  complex<double> res(1.0,0) ; // for methods 2 return a faktor of one
  if (method==1) { // only use for fourier rm synthesis 
    vector<complex<double> > test1 = performFarraday(peak.data, lcenter, faras,  nu_0, alpha, epsilon) ;
    vector<complex<double> > test2 = performRMSynthesis(test1,lcenter,linterv,faras, nu_0, alpha, epsilon) ;
    res = peak[index]/test2[index];
  }
  return res ;
}
/*! procedure calculates the complex peak for the current data. The absolutevalue
    of this peak is a gaussian function, it's mean and it's variance is given as a
    parameter to this function. The phase is used directly from the data set.
    \param data dirty reconstruction of the signal
    \param max  mean (maximum) value for the gauss function
    \param sigma variance for the gauss function
    \param faras vector of the values of the faraday depths for the data function
    \param index index of the maximum in the data vectors data, faras and peak
    \param peak complex vector which return the peak function in faraday depth to preform cleaning 
    \param useClean flag for the method of cleaning, which is useds */

void createPeak(vector<complex<double> > &data, double max, double sigma, vector<double> &faras,uint lo, uint index, uint hi, cvec &peak, uint useClean) {
  double t=0 ;
  double m=0 ;
  if ((useClean>1) && (lo+hi > 0 )) {
    /* start calculating the phase for the generated peak for the rm clean algorithm */
    vector<double> phi(lo+hi+1) ;
    double x=0 ; // init mean for faraday depths
    double y=0 ; // initi mean for phi (phase of peak) 
    double shift=0;
    double alt = 0 ;
    /* loop to calculate the mean of the faradaydepths and of the phases of the polarised emission */
    for (uint i=0; i<lo+hi+1; i++) {
      phi[i] = atan2(imag(data[i+index-lo]),real(data[i+index-lo])) ;
      /* avoid a changing of the sign for the angular phi */
      if (phi[i]-alt<0) shift = shift+2*PI;
      alt = phi[i] ;
      phi[i] = phi[i]+shift ;
      x=x+faras[i+index-lo] ;
      y=y+phi[i] ;
    }
    x= x/(lo+hi+1) ;
    y= y/(lo+hi+1) ;
    /* loop to calculate the needed values for least square fit for phis */
    double num = 0 ;
    double den = 0 ;
    for (uint i=0; i<lo+hi+1; i++) {
      num = num+(faras[i+index-lo]-x)*(phi[i]-y) ;
      den = den+(faras[i+index-lo]-x)*(faras[i+index-lo]-x) ;
    }
    m = num/den ; // slope of the line 
    t = y-m*x ;
//     cout << "m: " <<m << "  t: " << t << endl ;
    
    for (uint i=0; i<data.size(); i++) {
      double alpha = t+m*faras[i] ;
      double abso = abs(data[index])*exp(-(faras[i]-faras[index])*(faras[i]-faras[index])/(sigma*sigma)) ;
      complex<double> val(abso*cos(alpha),abso*sin(alpha)) ;
      peak.set(i,val) ;
    }
  }
  else {
    peak.set(index,data[index]) ;
  }
}


/*! integration over the lamda squared of the instrument. In the case of gaps the lamda 
    squared intervals are splited into two separat intervals */

complex<double> integrateLamdaSq(vector<complex<double> > &QU,vector<double> &lambC, vector<double> &lambI,double faraday, double nu_0, double alpha, double epsilon, bool debug) {
    double lam_a ;  // start of the integration interval for phi integration 
    double lam_b ;  // end of the integration interval for phi integration 
    uint Nlambdas = lambC.size() ;
    double K=1.0/Nlambdas;
    complex<double> exp_lambdafactor;
    complex<double> erg = 0.0 ;
   // calculate the interval in faradaydepths
    /* loop for the porduct of one row of the transformation matrix with the vector
     * which must be transformed */
    for	(uint j=0; j<Nlambdas; j++) { 
        /* get the boarders of the current lamda interval */
        lam_a = lambC[j]-0.5*lambI[j] ;
        lam_b = lambC[j]+0.5*lambI[j] ;
        double arg = 2.0*faraday*lambC[j] ;
        exp_lambdafactor=complex<double>(cos(arg), sin(arg));	
        erg = erg+exp_lambdafactor*QU[j];//*(lam_b-lam_a) ;
        if (debug) cout << j << " " << erg << " " << lam_b-lam_a << " " <<exp_lambdafactor <<" " <<  QU[j] << endl ;
    }
    if(debug) cout << endl << endl << endl ;
    return K*erg ;
}

/*! integration over the lamda squared of the instrument. In the case of gaps the lamda 
    squared intervals are splited into two separat intervals */

complex<double> integrateLamdaSqInv(vector<complex<double> > &QU,vector<double> &faras,double lambda, double nu_0, double alpha, double epsilon, bool debug) {
    double fara_a ;  // start of the integration interval for phi integration 
    double fara_b ;  // end of the integration interval for phi integration 
    uint Nfaras = faras.size() ;
    complex<double> exp_lambdafactor;
    complex<double> erg = 0.0 ;
   // calculate the interval in faradaydepths
    /* loop for the porduct of one row of the transformation matrix with the vector
     * which must be transformed */
    for	(uint j=0; j<Nfaras; j++) { 
        /* get the boarders of the current lamda interval */
        if (j==0) fara_a=faras[0] ;
        else fara_a=0.5*(faras[j]+faras[j-1]);
        if (j==Nfaras-1) fara_b=faras[j];
        else fara_b=0.5*(faras[j+1]+faras[j]);
        double arg = -2.0*lambda*faras[j] ;
        exp_lambdafactor=complex<double>(cos(arg), sin(arg));	        
        erg = erg+exp_lambdafactor*QU[j];//*(fara_b-fara_a) ;
        if (debug) cout << j << " / " << Nfaras << " " << erg << " " << fara_b-fara_a << " " <<exp_lambdafactor <<" " <<  QU[j] << endl ;
    }
    if(debug) cout << endl << endl << endl ;
    return erg ;
}

/*! Procedure performs a simple RM-Synthesis, which just makes an diskrete 
  * "innverse Fourier" transformation. The matrix for this transformation is \
  * intended to be maximal similar to an unitary matrix. 
  * \param	QU		: QU values of lambdadependece which must be transformed
  * \param	lambdas		: lambda^2-lambda0^2 of the QU values
  * \param	faradays	: faraday depths for which the new QU values are calculated */
vector<complex<double> > performRMSynthesis(vector<complex<double> > &QU, vector<double> &lambC,vector<double> &lambI, vector<double> &faradays,  double nu_0, double alpha, double epsilon) {
  uint N = faradays.size() ;  
  double fak = 1.0;// /(faradays[N-1]-faradays[0]);
  vector<complex<double> > res(N) ;	
  /* loop over all faraday depths, which is the dimension of the result.
     in every periode of the loop one component of the result is written */
  for (uint i=0; i<N ; i++ ) {
    bool debug=false ;
    complex<double> erg = integrateLamdaSq(QU,lambC,lambI,faradays[i],nu_0,alpha, epsilon, debug) ;
//     if(abs(erg) > 1) cout << "index: " << i << "  Wert: " << erg << endl ;
    res[i] = fak*erg ;
  }  
  return res ;
}

/*! Procedure performs a simple simulation of polarised emission and faradayrotation, 
  *  which just makes an diskrete Fourier" transformation. The matrix for this transformation is 
  * intended to be maximal similar to an unitary matrix. 
  * \param	QU		: QU values of lambdadependece which must be transformed
  * \param	lambdas		: lambda^2-lambda0^2 of the QU values
  * \param	faradays	: faraday depths for which the new QU values are calculated */
vector<complex<double> > performFarraday(vector<complex<double> > &QU, vector<double> &lambC, vector<double> &faradays,  double nu_0, double alpha, double epsilon) {
  uint N = lambC.size() ; 
  double fak = 1.0;// /(lambC[N-1]-lambC[0]);
  vector<complex<double> > res(N) ;	
  bool debug=false ;
  /* loop over all faraday depths, which is the dimension of the result.
     in every periode of the loop one component of the result is written */
  for (uint i=0; i<N ; i++ ) {
    complex<double> erg = integrateLamdaSqInv(QU,faradays,lambC[i],nu_0,alpha, epsilon, debug) ;
    res[i] = fak*erg ;    
  }  
  return res ;
}

/*! procedure creates a complex matrix as the tensor product from two complex vectors 
    \param vek1 first vector
    \param vek2 seccond vector 
    \return new Matrix, which is the tensor product of the two vectors */
complex<double> cmat::prod(vector<complex<double> > &vek1, vector<complex<double> > &vek2) {
  uint s1 = vek1.size() ;
  uint s2 = vek2.size() ;
  complex<double> erg = 0 ;
  if (s1 != s2) {
   cerr << "imcomplatible dimensions " << endl ;
   throw "imcomplatible dimensions \n"; 
  }
  else {
    for (uint i=0; i<s1; i++) {
      erg = erg + vek1[i]*vek2[i] ;
    }
  }
  return erg ;
}

/*! copies the diagonal of the current matrix to a vector
   \param vector, in which the values of the diagonal of the matrix are copied. */
   
void cmat::copyToVec(cvec &vec)  {
  uint dim = vec.size() ;
  uint dim1 = rows() ;
  uint dim2 = cols() ;
  /* check compatibility */
  if ((dim1 !=dim) || (dim2 !=dim)) {
    cerr << "imcomplatible dimensions " << endl ;
    throw "imcomplatible dimensions \n"; 
  }
  /* loop over all elements of the diagonal of the current matrix, which must be quadratic
     because dim1 == dim == dim2 */
  for (uint i=0; i<dim; i++ ) {
    vec.set(i,data[i][i]) ;
  }
}

/*! Implementation of a matrix multiplication of the current complex matrix to a 
    real matrix.
    \param real matrix to multiply to */
cmat cmat::operator* (mat &matr) {
  uint nrow = rows() ;
  uint ncol = matr.cols() ;
  mat temp = matr.H() ;
  cmat erg(nrow,ncol) ;	
  for (uint i=0; i<nrow;i++) {
      vector<complex<double> > &row (data[i]) ;
      for (uint j=0;j<ncol;j++) {
        vector<double> &col(temp.data[j]) ;
        erg.set(i,j,mult(row,col)) ;
	
      }
  }   
  return erg ;
}

/*! performs a full Jacobistep to the current matrix object, and changes the current Matrix in this way 
   by multiplying it with UH*cmat*U where U is defined by the two paramters phi and psi
   \param phi argument of the trigonometric part of the untrary transformation U
   \param psi argument of the exponential part of the unitary Matrix U */
void cmat::fullJacobiStep(double phi,double psi, uint j, uint k) {
  rightJacobi(phi,psi,j,k) ;
  leftJacobi(phi,psi,j,k) ;
}

/** Performs Jocobiiterations until the matrix has nearly diagonal form.
   This means no auter diagonal element has a larger absolute value tahsn "aim".
   \param trans matrix which accmulates the transformation matrix, and so represents 
      the eigenvectors of the matrix 
   \param aim maximal off diagonal value of the approximated diagonal matrix 
   \param maxite maximal number of iterations, if the aim is not reached then, the iterarion failed 
   \return true if the iteration was sucessfull false if it failed */

bool cmat::JacobiIteration(cmat &trans, double aim, uint maxite) {
  double maxVal = getMaxOfDiag() ;
  double minVal = getMinDiag() ;
  uint ite = 0 ;
  cout << "Jacobi: " << ite << " " << minVal << " " << maxVal << endl ;
  if (rows() != cols()) {
    cerr << "dimensions of the matrix do not match, matrix must be a squre matrix " << endl ;
    throw "dimensions of the matrix do not match, matrix must be a squre matrix";
  }
  while ((maxVal > aim) && (ite<=maxite)) {
    ite++ ;
    uint j=maxCol ;
    uint k=maxRow ;
    if (maxCol > maxRow) {
      swap(k,j);
    } 
//     cout << "Information " << j << " "  << k << endl ;
    complex<double> ajk = data[j][k] ;
    /* get the values of ajj and akk wich are real */
    double ajj = data[j][j].real() ;
    double akk = data[k][k].real() ;
    double phi = 0;
    double psi = -atan2(ajk.imag(),ajk.real()) ;
    complex<double> zw = ehoch(psi)*ajk ;
    double wer = zw.real() ;
  /*if (ajj-akk>0)*/ phi= atan2(2*wer,akk-ajj) ;
//   else if (ajk >0) phi = atan2(2*ajk,ajj-akk)-PI ;
//   else if (ajk <0) phi = atan2(2*ajk,ajj-akk)+PI ;

    phi = -0.5*phi;
  
    fullJacobiStep(phi,psi,j,k) ;
    trans.rightJacobi(phi,psi,j,k) ;
    if (ite%5000 == 0) { 
      maxVal = getMaxOfDiag() ;
      minVal = getMinDiag() ;
      cout << "Jacobi: " << ite << "  maxVal:" << maxVal << "  minVal:" << minVal << "  aim:" << aim << endl ;
    }
  }
  return (ite <= maxite) ;
}

/*! Procedure performs a transformation of the current matrix into a tridiagonal form
    using the method of householder.
    This function only works, if the matrix is hermitian. */
void cmat::HouseholderTrans(cmat &trans) {
}

/*! Procedure calculates the sigma factor of the i-th transformation to get the 
   housefolder transformation and the tridiagonal form of the matrix 
   \param index index of the transformation for getting the tridiagonal form 
                The index runs from 0 to dim-1 */

double cmat::sigmaHouseholder(uint index)  {
   double erg=0 ;
   uint dim = rows() ;
   for (uint i=index+1; i<dim; i++) {
     double re = data[i][index].real() ;
     double im = data[i][index].imag() ;
     erg = erg + re*re+im*im;
   }
   return sqrt(erg);
}

/*! Procedure calculates the sigma factor of the i-th transformation to get the 
   housefolder transformation and the tridiagonal form of the matrix 
   \param index index of the transformation for getting the tridiagonal form 
                The index runs from 0 to dim-1 */

double cmat::sigmaHouseholder2(uint index)  {
   double erg=0 ;
   uint dim = rows() ;
   for (uint i=index; i<dim; i++) {
     double re = data[i][index].real() ;
     double im = data[i][index].imag() ;
     erg = erg + re*re+im*im;
   }
   return sqrt(erg);
}
/*! Procedure calculates the beta factor of the i-th transformation to
    get the householder transformation 
    \param index index of the transformation for getting the tridiagonal form 
                The index runs from 0 to dim-1 
    \param sig  sigmavalue to avoid double computation of it */
double cmat::betaHouseholder(uint index,double sig) {
  double erg = abs(data[index+1][index]);
  erg = 1.0/(sig*(sig+erg)); 
  return erg ;
}

/*! Procedure calculates the beta factor of the i-th transformation to
    get the householder transformation 
    \param index index of the transformation for getting the tridiagonal form 
                The index runs from 0 to dim-1 
    \param sig  sigmavalue to avoid double computation of it */
double cmat::betaHouseholder2(uint index,double sig) {
  double erg = abs(data[index][index]);
  erg = 1.0/(sig*(sig+erg)); 
  return erg ;
}

/*! performs a multiplication of the current matrix with a householdermatrix which is 
   described by a vector u and beta (Matrix = E-beta*u*u^t). The vector u does not 
   have same dimension as the matrix itself. The same is true for the unity matrix E.
   So the multiplication does not effect the complete collumn but only the last Elements.
   \param u vector which describes the  householder matrix
   \param beta real parameter which specifies the householder matrix
   \param ind index of the collumn which is affected */
   
void cmat::multTFromLeftToCol(vector<complex<double> > &u, double beta, uint ind) {
  uint dim=u.size() ;  // dimension of the vector u, which is different from the matrix
  uint index = rows()- dim ; // get the samllest index of the collumn which is affected
  vector<complex<double> > col(dim); 
  /* loop to get the values of the collumn, which has to be bultiplied */
  for (uint i=index; i<rows(); i++ ) {
    col[i-index] = data[i][ind] ;
  }
  /* loop to perform the multiplication of the vector u with the col => scalar */
  complex<double> utc(0.0) ; // result for the product 
  for (uint i=0; i<dim; i++) {
    utc = utc+conj(u[i])*col[i] ;
  }
  complex<double> fakt = beta*utc ;  // calculat the whole factor 
  /* loop to perform the multiplication */
  for (uint i=index; i<rows(); i++ ) {
    data[i][ind] = col[i-index]-fakt*u[i-index] ;
  }
}

/*! performs a multiplication of the current matrix with a householdermatrix which is 
   described by a vector u and beta (Matrix = E-beta*u*u^t). The vector u does not 
   have same dimension as the matrix itself. The same is true for the unity matrix E.
   So the multiplication does not effect the complete collumn but only the last Elements.
   \param u vector which describes the  householder matrix
   \param beta real parameter which specifies the householder matrix
   \param ind index of the collumn which is affected */
   
void cmat::multTFromRightToRow(vector<complex<double> > &u, double beta, uint ind) {
  uint dim=u.size() ;  // dimension of the vector u, which is different from the matrix
  uint index = rows()- dim ; // get the samllest index of the collumn which is affected
  vector<complex<double> > row(dim); 
  /* loop to get the values of the collumn, which has to be bultiplied */
  for (uint i=index; i<cols(); i++ ) {
    row[i-index] = data[ind][i] ;
  }
  /* loop to perform the multiplication of the vector u with the col => scalar */
  complex<double> utc(0.0) ; // result for the product 
  for (uint i=0; i<dim; i++) {
    utc = utc+u[i]*row[i] ;
  }
  complex<double> fakt = beta*utc ;  // calculat the whole factor 
  /* loop to perform the multiplication */
  for (uint i=index; i<cols(); i++ ) {
    data[ind][i] = row[i-index]-fakt*conj(u[i-index]) ;
  }
}

/*! performs a multiplication of the current matrix with a householdermatrix which is 
   described by a vector u and beta (Matrix = E-beta*u*u^t). The vector u does not 
   have same dimension as the matrix itself. The same is true for the unity matrix E.
   So the multiplication does not effect the complete collumn but only the last Elements.
   \param u vector which describes the  householder matrix
   \param beta real parameter which specifies the householder matrix
   \param ind index of the collumn which is affected */
   
void cmat::multTFromLeft(vector<complex<double> > &u, double beta, bool all) {
  uint dim=u.size() ;  // dimension of the vector u, which is different from the matrix
  uint index = rows()- dim ; // get the samllest index of the collumn which is affected
  uint start = 0;  // starposition if the first collumns are not zeros
  if (!all) start = index-1 ; // first columns are zeros
  for (uint ind =start ; ind < cols() ; ind++) {
    multTFromLeftToCol(u,beta,ind) ;
  }
}

/*! performs a multiplication of the current matrix with a householdermatrix which is 
   described by a vector u and beta (Matrix = E-beta*u*u^t). The vector u does not 
   have same dimension as the matrix itself. The same is true for the unity matrix E.
   So the multiplication does not effect the complete collumn but only the last Elements.
   \param u vector which describes the  householder matrix
   \param beta real parameter which specifies the householder matrix
   \param ind index of the collumn which is affected */
   
void cmat::multTFromLeft2(vector<complex<double> > &u, double beta, bool all) {
  uint dim=u.size() ;  // dimension of the vector u, which is different from the matrix
  uint index = rows()- dim ; // get the samllest index of the collumn which is affected
  uint start = 0;  // starposition if the first collumns are not zeros
  if (!all) start = index ; // first columns are zeros
  for (uint ind =start ; ind < cols() ; ind++) {
    multTFromLeftToCol(u,beta,ind) ;
  }
}

/*! performs a multiplication of the current matrix with a householdermatrix which is 
   described by a vector u and beta (Matrix = E-beta*u*u^t). The vector u does not 
   have same dimension as the matrix itself. The same is true for the unity matrix E.
   So the multiplication does not effect the complete collumn but only the last Elements.
   \param u vector which describes the  householder matrix
   \param beta real parameter which specifies the householder matrix
   \param ind index of the collumn which is affected */

void cmat::multTFromRight(vector<complex<double> > &u, double beta, bool all) {
  uint dim=u.size() ;  // dimension of the vector u, which is different from the matrix
  uint index = cols()- dim ; // get the samllest index of the collumn which is affected
  uint start = 0; // startposition if the first rows are not zeros
  if (!all) start = index-1 ;  // first rows are zeros
  for (uint ind =start ; ind < rows() ; ind++) {
    multTFromRightToRow(u,beta,ind) ;
  }
}

/*! performs a multiplication of the current matrix with a householdermatrix which is 
   described by a vector u and beta (Matrix = E-beta*u*u^t). The vector u does not 
   have same dimension as the matrix itself. The same is true for the unity matrix E.
   So the multiplication does not effect the complete collumn but only the last Elements.
   \param u vector which describes the  householder matrix
   \param beta real parameter which specifies the householder matrix
   \param ind index of the collumn which is affected */

void cmat::multTFromRight2(vector<complex<double> > &u, double beta, bool all) {
  uint dim=u.size() ;  // dimension of the vector u, which is different from the matrix
  uint index = cols()- dim ; // get the samllest index of the collumn which is affected
  uint start = 0; // startposition if the first rows are not zeros
  if (!all) start = index ;  // first rows are zeros
  for (uint ind =start ; ind < rows() ; ind++) {
    multTFromRightToRow(u,beta,ind) ;
  }
}
/*! procedure checks, whether a matrix is hermitian, to an accuracy of eps.
    \param wanted accuracy
    \return true, if the matrix is hermitian otherwise false */

bool cmat::isHermitian(double eps) {
  if (rows() != cols()) {
     cerr << "isHermitian: Matrix must be quadratic" << endl ;
     throw "isHermitian: Matrix must be quadratic";
  }
  bool erg = true ;
  for (uint i=0; (i<rows()) && (erg); i++) {
    for (uint j=i; (j<cols()) && (erg); j++) {
      erg = (abs(data[i][j]-conj(data[j][i]))<eps) ;
      if (!erg) cout << "found: " << i << " " << j << " " << data[i][j] << " " << data[j][i] << endl ;
    }
  }
  return erg ;
}
/*! preforms the complete Householder transformation to bring the current matrix
    into tridiagonal form
    \param unity matrix reports all used transformations to have the full transformation
    needed to the eigenvectors.*/
void cmat::performHouseholder(cmat &unity) {
  for (uint index=0; index <rows()-2; index++) {
    double sig= sigmaHouseholder(index) ;
    vector<complex<double> > u = uHouseholder(index,sig) ;
    double beta = betaHouseholder(index,sig) ;
//     cout << "householder: " << index+1 << "/" << rows()-1 << endl ;
    multTFromLeft(u,beta,false) ;
    multTFromRight(u,beta,false) ;
    unity.multTFromLeft(u,beta,true) ;
  }
}

/*! preforms the complete Householder transformation to bring the current matrix
    intor tridiagonal form
    \param unity matrix reports all used transformations to have the full transformation
    needed to the eigenvectors.*/
void cmat::performQR_step(cmat &unity) {
  for (uint index=0; index <rows()-1; index++) {
    double sig= sigmaHouseholder2(index) ;
    vector<complex<double> > u = uHouseholder2(index,sig) ;
    double beta = betaHouseholder2(index,sig) ;
//     cout << "householder: " << index+1 << "/" << rows()-1 << endl ;
    multTFromLeft2(u,beta,false) ;
    multTFromRight2(u,beta,false) ;
    unity.multTFromLeft2(u,beta,true) ;
  }
}

bool cmat::performQR(cmat &unity, double aim, uint maxite) {
  double eps = 1e-10 ;
  if (rows() != cols()) {
    cerr << "dimensions of the matrix do not match, matrix must be a squre matrix " << endl ;
    throw "dimensions of the matrix do not match, matrix must be a squre matrix";
  }
  double maxVal = getMaxOfDiag() ;
  double minVal = getMinDiag() ;
  uint ite = 0 ;
  performHouseholder(unity) ;
  while ((maxVal > aim*minVal+eps) && (ite<=maxite)) {
    performQR_step(unity) ;
    if (ite==99) valOut("test1.mat");
    if (ite==100) valOut("test2.mat");
    ite++ ;
    maxVal = getMaxOfDiag() ;
    minVal = getMinDiag() ;
//     cout << "QR: " << ite << "   res: " << maxVal << "  aim: " << aim*minVal+eps << endl   ;
    cout << "QR: " << ite << "  res: " << maxVal << "  minVal: " << minVal << "  aim: "  << aim*minVal+eps << endl ;
  }
  
  return (ite <= maxite) ;
}
/*! Procedure calculates the phase factor of the i-th transformation to
    get the householder transformation 
    \param index index of the transformation for getting the tridiagonal form 
                The index runs from 0 to dim-1 */
complex<double> cmat::phaseHouseholder(uint index) {
  complex<double> val = data[index+1][index] ;
  double x1=abs(val) ;
  complex<double> erg = val/x1 ;
  return erg ;
}

/*! Procedure calculates the phase factor of the i-th transformation to
    get the householder transformation 
    \param index index of the transformation for getting the tridiagonal form 
                The index runs from 0 to dim-1 */
complex<double> cmat::phaseHouseholder2(uint index) {
  complex<double> val = data[index][index] ;
  double x1=abs(val) ;
  complex<double> erg = val/x1 ;
  return erg ;
}

/*! calculates the Vector for the elementary housholder transformation:
    T_i = E-u*u^t
    \param index for the index of the elementary transformation 
     \param sig  sigmavalue to avoid double computation of it */
vector<complex<double> > cmat::uHouseholder(uint index, double sig) {
  uint dim = rows() ;
  uint newDim = dim-index-1 ;  // dimension of the restvector
  vector<complex<double> > erg(newDim) ;
  complex<double> phas = phaseHouseholder(index) ;
  /* set the firts component */
  erg[0] = data[index+1][index] + phas*sig ;
  /* loop to update all values of the result vector */
  for (uint i=1; i<newDim; i++) {
    erg[i] = data[index+1+i][index] ;
  }
  return erg ;
}

/*! calculates the Vector for the elementary housholder transformation:
    T_i = E-u*u^t
    \param index for the index of the elementary transformation 
     \param sig  sigmavalue to avoid double computation of it */
vector<complex<double> > cmat::uHouseholder2(uint index, double sig) {
  uint dim = rows() ;
  uint newDim = dim-index ;  // dimension of the restvector
  vector<complex<double> > erg(newDim) ;
  complex<double> phas = phaseHouseholder2(index) ;
  /* set the firts component */
  erg[0] = data[index][index] + phas*sig ;
  /* loop to update all values of the result vector */
  for (uint i=1; i<newDim; i++) {
    erg[i] = data[index+i][index] ;
  }
  return erg ;
}

/*! half of the Jacobi step cmat*U */
void cmat::rightJacobi(double phi,double psi, uint j, uint k) {
  /* precompute the sine and cosine for the transformation matrix */
  double s = sin(phi) ;
  double c = cos(phi) ;
  complex<double> e=ehoch(psi) ;
  complex<double> ee=conj(e) ;  // exp(-psi)
  complex<double> se = s*e ;
  complex<double> see = -s*ee ;
   
  for (uint i=0; i<rows() ; i++) {
    /* fetch needed datas - will be changed */
    complex<double> dij = data[i][j] ;
    complex<double> dik = data[i][k] ;
    data[i][j] = c*dij+se*dik ;
    data[i][k] = c*dik+see*dij ;
  }
}
/*! half of the Jacobi step UH*cmat */
void cmat::leftJacobi(double phi,double psi, uint j, uint k) {
 /* precompute the sine and cosine for the transformation matrix */
  double s = sin(phi) ;
  double c = cos(phi) ;
  complex<double> e=ehoch(psi) ;
  complex<double> ee=conj(e) ;  // exp(-psi)
  complex<double> se = s*ee ;
  complex<double> see = -s*e ;
  for (uint i=0; i<cols(); i++) {
    complex<double> dji = data[j][i] ;
    complex<double> dki = data[k][i] ;
    data[j][i] = c*dji+se*dki ;
    data[k][i] = c*dki+see*dji ;
  }
}

void cmat::mult(complex<double> fak) {
  uint nrow = rows() ;
  uint ncol = cols() ;
  for (uint i=0; i<nrow;i++) {
      for (uint j=0;j<ncol;j++) {
	data[i][j] = fak*data[i][j] ;	
      }
  }   
//   for (uint i=0; i<nrow;i++) {
//       for (uint j=0;j<ncol;j++) {
// 	cout << data[i][j] << endl ;	
//       }
//   }   
}

void cvec::mult(complex<double> fak) {
  uint dim = size() ;
  for (uint i=0; i<dim;i++) {
    data[i] = fak*data[i] ;	
  }   
}

//! flips the order of the current vector
void cvec::flip() {
   uint last = data.size()-1 ;
   uint half = data.size()/2 ;
  /* loop the half of the vector */
  for (uint i=0; i<half; i++) {
    complex<double> zw = data[i] ;
    data[i] = data[last-i] ;
    data[last-i] = zw ;
  }
}

//! flips the order of the current vector
void vec::flip() {
   uint last = data.size()-1 ;
   uint half = data.size()/2 ;
  /* loop the half of the vector */
  for (uint i=0; i<half; i++) {
    double zw = data[i] ;
    data[i] = data[last-i] ;
    data[last-i] = zw ;
  }
}

void mat::mult(double fak) {
  uint nrow = rows() ;
  uint ncol = cols() ;
  for (uint i=0; i<nrow;i++) {
      for (uint j=0;j<ncol;j++) {
	data[i][j] = fak*data[i][j] ;	
      }
  }   
}
/*! procedure writing out the martix in rectangular way 
    \param fileName Name of the file to write the data into */
void cmat::write(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<rows(); i++) {
    vector<complex<double> > &aus = data[i] ;
    for (uint j=0; j<aus.size() ; j++) {
      outfile << aus[j] << " " ;
    }
    outfile << endl ;
  }
  outfile.close();

}
/*! writing out absolute values of a Matrix with row and collumn index.
    Each value gets a own line, one line skip for a new row 
   \param fileName Name of the file to write the data into */
void cmat::absOut(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<rows(); i++) {
    vector<complex<double> > &aus = data[i] ;
    for (uint j=0; j<aus.size() ; j++) {
      outfile << i << "\t" << j << "\t" << abs(aus[j]) << endl  ;
    }
    outfile << endl ;
  }
  outfile.close();

}

/*! writing out absolute values of a Matrix with row and collumn index.
    Each value gets a own line, one line skip for a new row 
   \param fileName Name of the file to write the data into */
void cmat::valOut(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<rows(); i++) {
    vector<complex<double> > &aus = data[i] ;
    for (uint j=0; j<aus.size() ; j++) {
      outfile << i << "\t" << j << "\t" << aus[j] << endl  ;
    }
    outfile << endl ;
  }
  outfile.close();

}
/*! writing out absolute values of a complex vector into a named file */
void cvec::absOut(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<size(); i++) {
    complex<double> aus = data[i] ;
    outfile << i << " " << abs(aus) << endl ;
  }
  outfile.close();
}

/*! writing out absolute values of a complex vector into a named file */
void cvec::absOut(string fileName, uint start, uint end) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=max(start,(uint)0); (i<size()) && (i<=end)  ; i++) {
    complex<double> aus = data[i] ;
    outfile << i << " " << abs(aus) << endl ;
  }
  outfile.close();
}

/*! writing out absolute values of a complex vector into a named file */
void vec::absOut(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<size(); i++) {
    double aus = data[i] ;
    outfile << i << " " << fabs(aus) << endl ;
  }
  outfile.close();
}

/*! caluculates the inverse of the current matrix without changing it.
    The calculation uses a LU decomposition of the matrix */
cmat cmat::inv() {
  cmat zw ;
  zw.data = data ;
  return zw.GaussEle() ;
}
            /**
     * Returns sqrt(x<sup>2</sup>+y<sup>2</sup>).
     */
    double hypote(double x,double y) {
        double xAbs=fabs(x);
        double yAbs=fabs(y);
        if(xAbs==0.0 && yAbs==0.0)
            return 0.0;
        else if(xAbs<yAbs)
            return yAbs*sqrt(1.0+(xAbs/yAbs)*(xAbs/yAbs));
        else
            return xAbs*sqrt(1.0+(yAbs/xAbs)*(yAbs/xAbs));
    }

/*! copies the entries of the gsl-matrix to the data elements
   of the current matrix 
   \param gsl_matrix to take the values from 
*/
void mat::copyGSLMat2data(gsl_matrix *gsl_mat, vector<vector<double> > &dat) {
/* gt number of rows and columns of gsl matrix */
   uint row = gsl_mat->size1 ;
   uint col = gsl_mat->size2 ;
   /* reserve memory */
   dat.resize(row) ;
   for (uint i=0; i<row; i++) {
     dat[i].resize(col) ;
     for (uint j=0; j<col; j++) {
	/* set value */
	double val = gsl_matrix_get(gsl_mat,i,j);
	dat[i][j] = val ;
     }
   }
}

/*! copies the entries of the gsl-matrix to the data elements
   of the current matrix 
   \param gsl_matrix to take the values from 
*/
void cmat::copyGSLMat2data(gsl_matrix *gsl_mat, vector<vector<complex<double> > > &dat) {
/* gt number of rows and columns of gsl matrix */
   uint row = gsl_mat->size1 ;
   uint col = gsl_mat->size2 ;
   /* reserve memory */
   dat.resize(row) ;
   for (uint i=0; i<row; i++) {
     dat[i].resize(col) ;
     for (uint j=0; j<col; j++) {
	/* set value */
	double val = gsl_matrix_get(gsl_mat,i,j);
	dat[i][j] = val ;
     }
   }
}
/*! copies the entries of the gsl-matrix to the data elements
   of the current matrix 
   \param gsl_matrix to take the values from 
*/
void cmat::copyGSLMat2data(gsl_matrix *gsl_mat, vector<vector<double> > &dat) {
/* gt number of rows and columns of gsl matrix */
   uint row = gsl_mat->size1 ;
   uint col = gsl_mat->size2 ;
   /* reserve memory */
   dat.resize(row) ;
   for (uint i=0; i<row; i++) {
     dat[i].resize(col) ;
     for (uint j=0; j<col; j++) {
	/* set value */
	double val = gsl_matrix_get(gsl_mat,i,j);
	dat[i][j] = val ;
     }
   }
}

/*! copies the entries of the gsl-matrix to the data elements
   of the current matrix 
   \param gsl_matrix to take the values from 
*/
void cmat::copyGSLcomplMat2data(gsl_matrix *gsl_mat, vector<vector<complex<double> > > &dat, vector<vector<int> > &test) {
/* gt number of rows and columns of gsl matrix */
   uint row = gsl_mat->size1/2 ;
   uint col = gsl_mat->size2/2 ; 
   /* reserve memory */
   dat.resize(row) ;
   int count1 = 0 ;
   int count2 = 0 ;
   int count3 = 0 ;
   int count4 = 0 ;
   for (uint i=0; i<row; i++) {
     dat[i].resize(col) ;
     vector<int> line ;
     for (uint j=0; j<col; j++) {
	/* set value */
	complex<double> val(gsl_matrix_get(gsl_mat,2*i,2*j),gsl_matrix_get(gsl_mat,2*i+1,2*j)) ;
        double diff1 = gsl_matrix_get(gsl_mat,2*i,2*j)-gsl_matrix_get(gsl_mat,2*i+1,2*j+1);
        double summ1 = gsl_matrix_get(gsl_mat,2*i,2*j)+gsl_matrix_get(gsl_mat,2*i+1,2*j+1);
	if ((diff1 > 0.00001) && (summ1 > 0.00001)) {
	   count1++;
        }
        double diff2 = gsl_matrix_get(gsl_mat,2*i+1,2*j)-gsl_matrix_get(gsl_mat,2*i,2*j+1);
        double summ2 = gsl_matrix_get(gsl_mat,2*i+1,2*j)+gsl_matrix_get(gsl_mat,2*i,2*j+1);
	if ((diff2 > 0.00001) && (summ2 > 0.00001)) {
	   count2++;
        }
	if ((diff1 > 0.00001) && (diff2 > 0.00001)) {
	   count3++;
        }
	if ((summ1 > 0.00001) && (summ2 > 0.00001)) {
	   count4++;
        }
	dat[i][j] = val ;
        if (diff1>0.00001) line.push_back(1) ;
        else line.push_back(-1) ;
     }
     test.push_back(line) ;
   }
   cout << "gesamtzahl: " << count1 << " " << count2<< " " << count3<< " " << count4 << " " << row*col << endl ;
}

/*! copies the entries of the gsl-matrix to the data elements
   of the current matrix 
   \param gsl_matrix to take the values from 
*/
// void cmat::copyGSLVec2dat(gsl_vector *gsl_vec, vector<double>  &dat) {
// /* gt number of rows and columns of gsl matrix */
//    uint row = gsl_vec->size/2 ;
//    /* reserve memory */
//    dat.resize(row) ;
//    for (uint i=0; i<row; i++) {
//  	double val = gsl_vector_get(gsl_vec,2*i);
// 	dat[i] = val ;
//    }
// }

/*! copies the entries of the entries the singular vaue gsl vector
    to the data elements for the singular values, given as real vector dat.
    In the singular value decomposition of the gsl, each value is 
    shown twice, so every seccond value muat be omitted
   \param gsl_matrix to take the values from 
*/
void cmat::copyGSLVec2data(gsl_vector *gsl_vec, vector<double>  &dat) {
/* gt number of rows and columns of gsl matrix */
   uint row = gsl_vec->size ;
   /* reserve memory */
   dat.resize(row) ;
   for (uint i=0; i<row; i++) {
 	double val = gsl_vector_get(gsl_vec,i);
	dat[i] = val ;
   }
}

/*! extracts the real part of the current complex matrix */

mat cmat::real() {
  mat res(rows(),cols()) ;
  for (uint i=0; i<rows(); i++) {
    for (uint j=0; j<cols(); j++) {
      res.set(i,j,data[i][j].real());
    }
  }
  return res ;
}
/*! extracts the imaginary part of the current complex matrix */
mat cmat::imag() {
  mat res(rows(),cols()) ;
  for (uint i=0; i<rows(); i++) {
    for (uint j=0; j<cols(); j++) {
      res.set(i,j,data[i][j].imag());
    }
  }
  return res ;
}
/*! copies the entries of the gsl-matrix to the data elements
   of the current matrix 
   \param gsl_matrix to take the values from 
*/
void mat::copyGSLVec2data(gsl_vector *gsl_vec, vector<double>  &dat) {
/* gt number of rows and columns of gsl matrix */
   uint row = gsl_vec->size ;
   /* reserve memory */
   dat.resize(row) ;
   for (uint i=0; i<row; i++) {
 	double val = gsl_vector_get(gsl_vec,i);
	dat[i] = val ;
   }
}

gsl_matrix *mat::createGSL_Mat() {
   uint n1 = rows() ;
   uint n2 = cols() ;
   gsl_matrix *m = gsl_matrix_alloc(n1, n2) ;
   for (uint i=0; i<n1;i++) {
	for (uint j=0; j<n2; j++) {
	   gsl_matrix_set (m, i, j, data[i][j]);
	}
   }
   return m ;
}

gsl_matrix *cmat::createRealGSL_Mat() {
   uint n1 = rows() ;
   uint n2 = cols() ;
   gsl_matrix *m = gsl_matrix_alloc(2*n1, 2*n2) ;
   for (uint i=0; i<n1;i++) {
	for (uint j=0; j<n2; j++) {
	   gsl_matrix_set (m, 2*i, 2*j, data[i][j].real());
	   gsl_matrix_set (m, 2*i+1, 2*j+1, data[i][j].real());
	}
   }
   for (uint i=0; i<n1;i++) {
	for (uint j=0; j<n2; j++) {
	   gsl_matrix_set (m, 2*i, 2*j+1, data[i][j].imag());
	   gsl_matrix_set (m, 2*i+1, 2*j, -data[i][j].imag());
	}
   }
   return m ;
}

gsl_matrix *cmat::createGSL_Mat(bool real) {
   uint n1 = rows() ;
   uint n2 = cols() ;
   gsl_matrix *m = gsl_matrix_alloc(n1, n2) ;
   for (uint i=0; i<n1;i++) {
	for (uint j=0; j<n2; j++) {
	   if (real) {
	     gsl_matrix_set (m, i, j, data[i][j].real());
           }
	   else {
	     gsl_matrix_set (m, i, j, data[i][j].imag());
	   }
	}
   }
   return m ;
}

gsl_matrix_complex *cmat::createGSL_Mat() {
   uint n1 = rows() ;
   uint n2 = cols() ;
   gsl_matrix_complex *m = gsl_matrix_complex_alloc(n1, n2) ;
   for (uint i=0; i<n1;i++) {
	for (uint j=0; j<n2; j++) {
	   complex<double> cval = data[i][j];
	   gsl_complex val ;
	   val.dat[0] = cval.real() ;
	   val.dat[1] = cval.imag() ;
	   gsl_matrix_complex_set (m, i, j,val);
	}
   }
   return m ;
}
void mat::svd() {
//   uint n1 = rows() ;
   uint n2 = cols() ;
   gsl_matrix *m = createGSL_Mat() ;
   gsl_matrix *v = gsl_matrix_alloc(n2, n2) ;
   gsl_vector *s = gsl_vector_alloc (n2) ;
   gsl_vector *w = gsl_vector_alloc (n2) ;
   cout << "all gsl matrixes are generated" << endl ;
   /* store current matrix into gsl object */
   
   gsl_linalg_SV_decomp(m,v,s,w);
   vec sing(s) ;
   sing.valOut("singTest.dat");
   copyGSLMat2data(m,unitary1) ;
   copyGSLMat2data(v,unitary2) ;
   copyGSLVec2data(s,singVals);
   gsl_matrix_free(m) ;
   gsl_matrix_free(v) ;
   cout << "u: " << unitary1.size() <<"  v: " << unitary2.size() << endl ; 
}

void cmat::svd() {
      
}
double sig(double val1, double val2) {
    double res = 0.0 ;
    if (val2>=0) res = fabs(val1) ;
    else res = -fabs(val1);
    return res ;
}

/*! creates pseudo inverse of the current matrix, using the singular value
    decomposition. */
void cmat::creatPseudoInverse() {

}
/*! procedure calculates the (pseudo) inverse of the current positive (semi) definite
    matrix. The result is returned inside the parameter erg
    \param U untirary transformation for the diagonalisation of the current matrix
    \param eig eigenvalues of the current matrix
    \param erg matrix to return the result for the inverse matrix */

void makeInverse(cmat &U,vec &eig, cmat &erg) {
   double eps = sqrt(numeric_limits<double>::epsilon( )) ;
   uint dim = U.rows();
   for (uint i1=0; i1<dim; i1++) {
     for (uint i2=0; i2<dim; i2++) {
       complex<double> zw(0,0) ;
       for (uint i3=0; i3<dim; i3++) {
         if (eig[i3] > eps)
           zw = zw+U.data[i1][i3]*conj(U.data[i2][i3])/eig[i3] ;
       }
       erg.set(i1,i2,zw) ;
     }
   }
}
/*! Procedure for calculating the pseudoinverse of the current matrix 
    by calculating the eigenvalues and eigenvectors. The matrix must be hermitian
    but is also intended to be positive definite. 
    \return The pseudoinvers Matrix */

cmat cmat::calcPosEig() {
  if (rows() != cols()) {
     cerr << "CalcPosEig error: matrix is not a square matrix." << endl ;
     throw "CalcPosEig error: matrix is not a square matrix." ;
  }
  gsl_matrix_complex *A=createGSL_Mat() ; //create a gsl matrix from the current matrix
  size_t dim = A->size1 ;
  gsl_matrix_complex  *eigVecs = gsl_matrix_complex_alloc(dim,dim) ;
  gsl_vector *eigVals= gsl_vector_alloc(dim) ;
  gsl_eigen_hermv_workspace *work = gsl_eigen_hermv_alloc (dim)  ;
  if ((eigVecs==0)||(eigVals==0)|| (work==0) ) {
    cerr << "cmat calculating eigenvectors: not enough memory" << endl ;
    throw "cmat calculating eigenvectors: not enough memory" ;
  }
  cout << "start gsl procedure to calculate eigenvalues" << endl ;
  int res = gsl_eigen_hermv (A, eigVals, eigVecs, work);
  cout << "gsl procedure for eigenvalues finished: " << res  << endl ;
  cmat U(eigVecs) ;
  cmat Uh=U.H() ;
  vec eig(eigVals) ;
  eig.valOut("eigen.dat") ;
  cmat inv(rows(),rows());
  makeInverse(U, eig,inv) ;
  gsl_eigen_hermv_free(work) ;
  gsl_matrix_complex_free(A) ;
  gsl_matrix_complex_free(eigVecs) ;
  gsl_vector_free(eigVals) ;
  return inv;
}

/*! Procedure calculates the Eigenvectors and Eigenvalues for the current matrix */
void cmat::calcEigenSys() {
  if (rows() != cols()) {
     cerr << "CalcPosEig error: matrix is not a square matrix." << endl ;
     throw "CalcPosEig error: matrix is not a square matrix." ;
  }
  gsl_matrix_complex *A=createGSL_Mat() ; //create a gsl matrix from the current matrix
  size_t dim = A->size1 ;
  gsl_matrix_complex  *eigVecs = gsl_matrix_complex_alloc(dim,dim) ;
  gsl_vector *eigVals= gsl_vector_alloc(dim) ;
  gsl_eigen_hermv_workspace *work = gsl_eigen_hermv_alloc (dim)  ;
  if ((eigVecs==0)||(eigVals==0)|| (work==0) ) {
    cerr << "cmat calculating eigenvectors: not enough memory" << endl ;
    throw "cmat calculating eigenvectors: not enough memory" ;
  }
  cout << "start gsl procedure to calculate eigenvalues" << endl ;
  int res = gsl_eigen_hermv (A, eigVals, eigVecs, work);
  cout << "gsl procedure for eigenvalues finished: " << res  << endl ;
  cmat U(eigVecs,true) ;
  vec eig(eigVals) ;
  eigenValues=eig.data ;
  Q = U.data ;
  gsl_eigen_hermv_free(work) ;
  gsl_matrix_complex_free(A) ;
  gsl_matrix_complex_free(eigVecs) ;
  gsl_vector_free(eigVals) ;
}

vec mat::operator* (vec &vek) {
  uint nRow = rows() ;
  vec erg(nRow) ;
  for (uint i=0; i<nRow; i++) {
    vector<double> &zw(data[i]) ;
    erg[i] = vek*zw ;
  }
  return erg ;
} 

/*! procedure tries to solve the linear system, using the current matrix as the system matrix and the parameter rs 
   as the right side vector */
cvec cmat::solveWithSVD(cvec rs) {
  cvec erg(cols()) ;
  return erg ;
}
void createArray(uint row, uint col, double val, vector<vector<double> > &mat) {
	mat.resize(row) ;
	for (uint i=0; i<row; i++) {
		mat[i].resize(col) ;
		for (uint j=0; j<col; j++) {
			mat[i][j] = val ;
		}
	}
}


/*! Test whether the current matrix is unitary or not
   true if matrix is (almost) unitary
   \param eps degree of accuracy for performing the test */
bool cmat::isUnitary(double eps) {
  bool erg = true ;
  complex<double> eins(1,0) ;
  complex<double> null(0,0) ;
  cmat zw1 = this->H() ;
  cmat zw2 = zw1*(*this) ;
  /* in the following, the procedure tests whether the matrix zw2 is the unit matrix */
  for (uint i=0; (i<zw2.rows()) && erg ; i++) {
    erg = abs(zw2.get(i,i) - eins) < eps ;
    for (uint j=i+1; (j<zw2.cols()) && erg ; j++) {
	erg = (abs(zw2.get(i,j)) < eps) && (abs(zw2.get(j,i)) < eps) ;
    } 
  }
  return erg ;
}

/*! caluculates the pseudoinverse of the current matrix without changing it.
    This procedure only works with Hermitian matrixes. 
    It performs a diagonalisation of the matrix, and calculates the inverse
    by simply inversing diagonal elements (which are real). Then the eigenvectors
    whos eigenvalues are to close to zero are removed from the image space of the 
    matrix. Now the pseudoinverse is calculated by using the Projector */
cmat cmat::pseudoinverse() {
  if (rows() != cols()) {
    cerr << "pseudoinverse: matrix must be quadratic for this procedure " << endl ;
    throw "pseudoinverse: matrix must be quadratic for this procedure " ;
  }
  cmat ra ;
  double eps = 1e-10 ;
  ra.data = data ;
  cmat rMat(rows(),cols()) ;
  rMat.fillUnit() ;
  bool success = ra.JacobiIteration(rMat,eps,1000000) ;  
  if (success) { // Jacobi iteration returns successfully
    cmat diaInv(rows(),cols()) ;
    diaInv.fillUnit() ;
    cmat zw = rMat.H() ;
    for (uint i=0; i< rows() ; i++) {
	if (abs(ra.data[i][i]) > eps)  
	  diaInv.set(i,i,1/ra.data[i][i].real()) ;  // only use real part, diagonal element is real
	else 
	  diaInv.set(i,i,0) ;  // set to zero to integrate the projection into the image of the matrix into the pseudoinverse
    }
     cmat ra2 = rMat*ra*zw ;
     cmat res = zw*diaInv*rMat ;
     return res ;
  }
  else {  // Jacobi iteration failed 
    cerr << "pseudoinverse: Jacobi iteration failed  " << endl ;
    throw "pseudoinverse: Jacobi iteration failed  " ;
  }
  /* calculate the inverse of the diagonal elements */
}


/*! caluculates the pseudoinverse of the current matrix without changing it.
    This procedure only works with Hermitian matrixes. 
    It performs a diagonalisation of the matrix, and calculates the inverse
    by simply inversing diagonal elements (which are real). Then the eigenvectors
    whos eigenvalues are to close to zero are removed from the image space of the 
    matrix. Now the pseudoinverse is calculated by using the Projector */
mat mat::pseudoinverse() {
  if (rows() != cols()) {
    cerr << "pseudoinverse: matrix must be quadratic for this procedure " << endl ;
    throw "pseudoinverse: matrix must be quadratic for this procedure " ;
  }
  mat ra ;
  double eps = 1e-10 ;
  ra.data = data ;
  mat rMat(rows(),cols()) ;
  rMat.fillUnit() ;
  bool success = ra.JacobiIteration(rMat,eps,1000000) ;  
  if (success) { // Jacobi iteration returns successfully
    mat diaInv(rows(),cols()) ;
    diaInv.fillUnit() ;
    mat zw = rMat.H() ;
    for (uint i=0; i< rows() ; i++) {
	if (fabs(ra.data[i][i]) > eps)  
	  diaInv.set(i,i,1/ra.data[i][i]) ;  // only use real part, diagonal element is real
	else 
	  diaInv.set(i,i,0) ;  // set to zero to integrate the projection into the image of the matrix into the pseudoinverse
    }
     mat ra2 = rMat*ra*zw ;
     mat res = zw*diaInv*rMat ;
     return res ;
  }
  else {  // Jacobi iteration failed 
    cerr << "pseudoinverse: Jacobi iteration failed  " << endl ;
    throw "pseudoinverse: Jacobi iteration failed  " ;
  }
  /* calculate the inverse of the diagonal elements */
}

/*! writing out absolute values of a complex vector into a named file */
void cvec::valOut(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<size(); i++) {
    complex<double> aus = data[i] ;
    outfile << i << " " << aus << endl ;
  }
  outfile.close();
}

/*! function returns the real part of the complex inside a real vector
*/
vec cvec::real() {
  vec res(size()) ;
  for (uint i=0; i<size(); i++) {
    res.set(i,data[i].real());
  }
  return res ;
}
/*! function returns the imaginary part of the complex inside a real vector
*/
vec cvec::imag() {
  vec res(size()) ;
  for (uint i=0; i<size(); i++) {
    res.set(i,data[i].imag());
  }
  return res ;
}

/*! writing out the two components of the complex numbers in two separate columns
    of a text file to plot the file by gnuplot */
void cvec::sepOut(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<size(); i++) {
    complex<double> aus = data[i] ;
    outfile << i << " " << aus.real() << " " << aus.imag() << endl ;
  }
  outfile.close();
}

/*! writing out absolute values of a complex vector into a named file */
void cvec::realOut(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<size(); i++) {
    double aus = data[i].real() ;
    outfile << i << " " << aus << endl ;
  }
  outfile.close();
}

/*! writing out absolute values of a complex vector into a named file */
void cvec::imagOut(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<size(); i++) {
    double aus = data[i].imag() ;
    outfile << i << " " << aus << endl ;
  }
  outfile.close();
}

/*! writing out absolute values of a complex vector into a named file */
void vec::valOut(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<size(); i++) {
    double aus = data[i] ;
    outfile << i << " " << aus << endl ;
  }
  outfile.close();
}


/*! Procedure returns the absolute bigest number of the matrix.
   The procedure also sets the attributes maxCol, and maxRow to the 
   indexes, where the absolute maximal value was found */ 
double cmat::getMaxOfDiag() {
  double maxVal = 0 ;
  for (uint i=0; i<rows(); i++) {
    for (uint j=0; j<cols() ; j++) {
	if ((i!=j) && (maxVal < abs(data[i][j]))) {
	  maxVal = abs(data[i][j]) ;
	  maxCol = j ;
	  maxRow = i ;
        }
    }
  }
  return maxVal ;
}

/*! Procedure returns the absolute smallest number of the diagonal
    of the matrix 
    The procedure is only usable for quadratic matrices */
double cmat::getMinDiag() {
  if (rows() != cols()) {
    cerr << "getMInDiag: Matrix must be a square matrix " << endl ;
    throw "getMInDiag: Matrix must be a square matrix " ;
  }
  double minVal = 0 ;
  for (uint i=0; i<rows(); i++) {
    double test = abs(data[i][i]) ;
    if ((test < minVal) || (i==0)) {
	minVal = test ;
    }
  }
  return minVal ;
}

complex<double> cmat::mult(vector<complex<double> > &v1,vector<complex<double> > &v2) {
  complex<double> erg(0,0) ;
  for (uint i=0; i<v1.size(); i++) {
    erg = erg+v1[i]*v2[i] ;
  }
  return erg ;
}

complex<double> cmat::mult(vector<complex<double> > &v1,vector<double> &v2) {
  complex<double> erg(0,0) ;
  for (uint i=0; i<v1.size(); i++) {
    erg = erg+v1[i]*v2[i] ;
  }
  return erg ;
}

double cmat::maxi() {
  double erg = 0 ;
  for (uint i=0; i<rows();i++) {
    vector<complex<double> > row = data[i] ;
    for (uint j=0; j< cols() ; j++) {
      if (erg < abs(row[j])) erg = abs(row[j]) ;
    }
  } 
  return erg;
}

void cmat::swapCols(uint ind1, uint ind2) {
  if (ind1 != ind2) {
    if(max(ind1,ind2)>rows()) {
	 throw "index out of Range in swapCols\n" ;
    }
    for (uint i=0; i<rows(); i++) {
	swap(data[i][ind1],data[i][ind2]);
    }
  }
}

/*! fill a symetric ramdom matrix with all elements from 0 to 1 */
void cmat::fillSymRand() {
  double imax = 1.0/RAND_MAX ;
  /* loop to fill array */
  for (uint i=0; i<rows() ; i++) {
    for (uint j=i; j<cols() ; j++) {
      double psi = 0 ;
      if (i!=j) psi = rand()*imax ;
      data[i][j] = rand()*imax*ehoch(psi) ;
      if (i!= j) data[j][i] = conj(data[i][j]) ;
    }
  }
}

/*! Procedure performs an complete elimination schema including the following steps:
    1. Forward elimination
    2. backward elimination
    3. normalisation */

cmat cmat::GaussEle() {
  if (rows() != cols()) {
    throw "no Elimination implemented for non quadratic matrixes\n" ;
  }
  stack<permEntry> perm ;
  cmat unity(rows(),rows()) ;
  unity.fillUnit() ;
  /* forward elemination */
  for (uint i=0; i<rows()-1; i++) {
//     cout << "forward: " << i << "/" << rows() << endl ;
    uint ro = getMaxPivot(i) ;
    if (ro != i) {
      data[ro].swap(data[i]);
      permEntry eint(ro,i) ;
      perm.push(eint) ;
    }
    if (data[i][i] == 0.0 ) {
      cout << "index " << i << "  diagonal " << data[i][i] << endl ;
      throw "Matrix is not invertible \n";
    }
    for (uint j= i+1; j< rows(); j++ ) {
      complex<double> fak = -data[j][i]/data[i][i];
      add(j,i,fak);
      unity.add(j,i,fak);
    }
  }
  /* backward elemination */
    for (uint i=rows()-1; i>0; i--) {    
//       cout << "backward: " << i << "/" << rows() << endl ;
      for (uint j= 0; j< i; j++ ) {
        complex<double> fak = -data[j][i]/data[i][i];
        add(j,i,fak);
        unity.add(j,i,fak);
      }
    }
    /* normalisation */
    for (uint i=0; i<rows(); i++) {
	complex<double> fak = 1.0/data[i][i] ;
	unity.mult(fak,i) ;
    }
    /* swap collumns of result */
    while(!perm.empty()) {
	    
      permEntry per = perm.top() ;
      perm.pop() ;
      unity.swapCols(per.pos1,per.pos2) ;
    }
    return unity;
}

void cmat::mult(complex<double> fak, uint row) {
  for (uint i=0; i<data.size(); i++) {
    data[row][i] = fak*data[row][i] ;
  }
}

void mat::mult(double fak, uint row) {
  for (uint i=0; i<data.size(); i++) {
    data[row][i] = fak*data[row][i] ;
  }
}

void mat::rightJacobi(double phi, uint j, uint k) {\
  double s = sin(phi) ;
  double c = cos(phi) ;
  for (uint i=0; i<rows() ; i++) {
    /* fetch needed datas - will be changed */
    double dij = data[i][j] ;
    double dik = data[i][k] ;
    data[i][j] = c*dij-s*dik ;
    data[i][k] = c*dik+s*dij ;
  }
}

void mat::leftJacobi(double phi, uint j, uint k) {
  double s = sin(phi) ;
  double c = cos(phi) ;
  for (uint i=0; i<cols() ; i++) {
    /* fetch needed datas - will be changed */
    double dji = data[j][i] ;
    double dki = data[k][i] ;
    data[j][i] = c*dji-s*dki ;
    data[k][i] = c*dki+s*dji ;
  }
}

/*! fill a symetric ramdom matrix with all elements from 0 to 1 */
void mat::fillSymRand() {
  double imax = 1.0/RAND_MAX ;
  /* loop to fill array */
  for (uint i=0; i<rows() ; i++) {
    for (uint j=i; j<cols() ; j++) {
      data[i][j] = rand()*imax ;
      data[j][i] = data[i][j] ;
    }
  }
}

void mat::copyToVec(vec &vals)  {
  uint dim = vals.size() ;
  uint dim1 = rows() ;
  uint dim2 = cols() ;
  /* check compatibility */
  if ((dim1 !=dim) || (dim2 !=dim)) {
    cerr << "imcomplatible dimensions " << endl ;
    throw "imcomplatible dimensions \n"; 
  }
  /* loop over all elements of the diagonal of the current matrix, which must be quadratic
     because dim1 == dim == dim2 */
  for (uint i=0; i<dim; i++ ) {
    vals.set(i,data[i][i]) ;
  }
}

void mat::fullJacobiStep(double phi, uint j, uint k) {
  rightJacobi(phi,j,k) ;
  leftJacobi(phi,j,k) ;
}

/** Performs Jocobiiterations until the matrix has nearly diagonal form.
   This means no auter diagonal element has a larger absolute value tahsn "aim".
   \param trans matrix which accmulates the transformation matrix, and so represents 
      the eigenvectors of the matrix 
   \param aim maximal off diagonal value of the approximated diagonal matrix 
   \param maxite maximal number of iterations, if the aim is not reached then, the iterarion failed 
   \return true if the iteration was sucessfull false if it failed */

bool mat::JacobiIteration(mat &trans, double aim, uint maxite) {
  double maxVal = getMaxOfDiag() ;
  double minVal = getMinDiag() ;
  uint ite = 0 ;
  double eps = 1e-10 ;
  cout << "Jacobi: " << ite << " " << maxVal << endl ;
  if (rows() != cols()) {
    cerr << "dimensions of the matrix do not match, matrix must be a squre matrix " << endl ;
    throw "dimensions of the matrix do not match, matrix must be a squre matrix";
  }
  while ((maxVal > aim*minVal+eps) && (ite<=maxite)) {
    ite++ ;
    uint j=maxCol ;
    uint k=maxRow ;
    if (maxCol > maxRow) {
      swap(k,j);
    } 
    double ajk = data[j][k] ;
    double ajj = data[j][j] ;
    double akk = data[k][k] ;
    double phi = 0;
  /*if (ajj-akk>0)*/ phi= atan2(2*ajk,ajj-akk) ;
//   else if (ajk >0) phi = atan2(2*ajk,ajj-akk)-PI ;
//   else if (ajk <0) phi = atan2(2*ajk,ajj-akk)+PI ;

    phi = -0.5*phi;
  
    fullJacobiStep(phi,j,k) ;
    trans.rightJacobi(phi,j,k) ;
    maxVal = getMaxOfDiag() ;
    minVal = getMinDiag() ;
    cout << "Jakobi: " << ite << "  pos: " << maxVal << "  minVal: " << minVal << "  aim: "  << aim*minVal+eps << endl ;
  }
  return (ite <= maxite) ;
}

/*! Procedure returns the absolute bigest number of the matrix.
   The procedure also sets the attributes maxCol, and maxRow to the 
   indexes, where the absolute maximal value was found */ 
double mat::getMaxVal() {
  double maxVal = 0 ;
  for (uint i=0; i<rows(); i++) {
    for (uint j=0; j<cols() ; j++) {
	if (maxVal < fabs(data[i][j])) {
	  maxVal = fabs(data[i][j]) ;
	  maxCol = j ;
	  maxRow = i ;
        }
    }
  }
  return maxVal ;
}

/*! Procedure returns the absolute bigest number of the matrix.
   The procedure also sets the attributes maxCol, and maxRow to the 
   indexes, where the absolute maximal value was found */ 
double mat::getMaxOfDiag() {
  double maxVal = 0 ;
  for (uint i=0; i<rows(); i++) {
    for (uint j=0; j<cols() ; j++) {
	if ((i!=j) && (maxVal < fabs(data[i][j]))) {
	  maxVal = fabs(data[i][j]) ;
	  maxCol = j ;
	  maxRow = i ;
        }
    }
  }
  return maxVal ;
}

/*! Procedure returns the absolute smallest number of the diagonal
    of the matrix 
    The procedure is only usable for quadratic matrices */
double mat::getMinDiag() {
  if (rows() != cols()) {
    cerr << "getMInDiag: Matrix must be a square matrix " << endl ;
    throw "getMInDiag: Matrix must be a square matrix " ;
  }
  double minVal = 0 ;
  for (uint i=0; i<rows(); i++) {
    double test = fabs(data[i][i]) ;
    if ((test < minVal) || (i==0)) {
	minVal = test ;
    }
  }
  return minVal ;
}

void mat::swapCols(uint ind1, uint ind2) {
  if (ind1 != ind2) {
    if(max(ind1,ind2)>rows()) {
	 throw "index out of Range in swapCols\n" ;
    }
    for (uint i=0; i<rows(); i++) {
	swap(data[i][ind1],data[i][ind2]);
    }
  }
}

/*! writing out absolute values of a Matrix with row and collumn index.
    Each value gets a own line, one line skip for a new row 
   \param fileName Name of the file to write the data into */
void mat::absOut(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<rows(); i++) {
    vector<double> &aus = data[i] ;
    for (uint j=0; j<aus.size() ; j++) {
      outfile << i << "\t" << j << "\t" << fabs(aus[j]) << endl  ;
    }
    outfile << endl ;
  }
  outfile.close();

}

/*! writing out absolute values of a Matrix with row and collumn index.
    Each value gets a own line, one line skip for a new row 
   \param fileName Name of the file to write the data into */
void mat::valOut(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<rows(); i++) {
    vector<double> &aus(data[i]) ;
    for (uint j=0; j<aus.size() ; j++) {
      outfile /*<< i << "\t" << j*/ << "\t" << aus[j]  ;
    }
    outfile << endl ;
  }
  outfile.close();

}

/*! writing the real part of a matrix into an array */
void cmat::realValOut(string fileName) {
  ofstream outfile (fileName.c_str());
  outfile.precision(15);
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<rows(); i++) {
    vector<complex<double> > &aus(data[i]) ;
    for (uint j=0; j<aus.size() ; j++) {
      outfile /*<< i << "\t" << j*/ << "\t" << aus[j].real()  ;
    }
    outfile << endl ;
  }
  outfile.close();

} 

/*! writing the imaginary part of a matrix into an array */
void cmat::imagValOut(string fileName) {
  ofstream outfile (fileName.c_str());
  outfile.precision(15);
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<rows(); i++) {
    vector<complex<double> > &aus(data[i]) ;
    for (uint j=0; j<aus.size() ; j++) {
      outfile /*<< i << "\t" << j*/ << "\t" << aus[j].imag()  ;
    }
    outfile << endl ;
  }
  outfile.close();

} 
void cmat::fillUnit() {
  uint nCol = cols() ;
  uint nRow = rows() ;
  if (nRow != nCol) {
    cerr << "no unit for non quadratic matrixes\n" ;
    throw "no unit for non quadratic matrixes\n" ;
  }
  for (uint i=0; i< nRow ; i++) {
     for (uint j=0; j<nRow; j++ ) {
	 data[i][j] =(i == j) ? 1 : 0; // kronecer delta
     }
  }
}

void cmat::add(uint addto, uint addfrom, complex<double> fak) {
  for (uint i=0; i<data.size(); i++) {
    data[addto][i] = data[addto][i]+fak*data[addfrom][i] ;
  }	
}

void cmat::add(cvec &vec1, cvec &vec2) {
  if ((vec1.size() != rows()) || (vec2.size() != cols())) {
    cerr << "add: dimensions of vectors and matrix do not fit" << endl ;
    throw "dimensions of vectors and matrix do not fit" ;
  }
  for (uint i =0; i<rows(); i++) {
    for (uint j=0; j<cols(); j++) {
       data[i][j] = data[i][j]+vec1[i]*vec2[j] ;
    }
  }
}

void mat::add(uint addto, uint addfrom, double fak) {
  for (uint i=0; i<data.size(); i++) {
    data[addto][i] = data[addto][i]+fak*data[addfrom][i] ;
  }	
}

/*! procedure performs the multiplication of two Matrices represented by
    stl vector of vectors. In this case the multiplication is performed on complex matrix with
    the complex adjunct of the seccond one. The number of cols of matrices 
    must be the same. The calculating of the adjunct is not done explicitly 
    The result is also vector of vectors and the entries of the matrix are complex */
vector<vector<complex<double> > > cmat::mult_M2MH(vector<vector<complex<double> > > &m1, vector<vector<complex<double> > > &m2) {
  if ((m1.size() ==0) || (m2.size()==0) || (m1[0].size()==0) || (m2[0].size()==0)) {
     cerr << "error in matrixmultiplication, one dimension is 0" << endl ;
     throw "error in matrixmultiplication, one dimension is 0" ;
  }
  if (m1[0].size() != m2[0].size()) {
    cerr << "error in matrixmultiplication, dimensions of the matrix do mot fit" << endl ;
    throw "error in matrixmultiplication, dimensions of the matrix do mot fit";
  }
  /* calculate the dimensions of the result matrix */
  uint rows=m1.size() ;
  uint cols=m2.size() ;
  uint mults=m1[0].size() ;
  /* create the result matrix */
  vector<vector<complex<double> > > res ;
  res.resize(rows) ;
  for (uint i=0; i<rows; i++) {
    res[i].resize(cols) ;
  }
  /*perform the multiplication  */
  for (uint i1=0; i1<rows; i1++) {
    for (uint i2=0; i2<cols; i2++) {
      complex<double> val(0,0);
      for(uint i3=0; i3<mults; i3++) {
        val = val+m1[i1][i3]*conj(m2[i2][i3]) ;
      }
      res[i1][i2] = val ;
    }
  }
  return res;
}


/*! procedure performs the multiplication of two Matrices represented by
    stl vector of vectors. In this case the multiplication is performed on complex matrix with
    the complex adjunct of the seccond one. The number of cols of matrices 
    must be the same. The calculating of the adjunct is not done explicitly 
    The result is also vector of vectors and the entries of the matrix are complex */
vector<vector<complex<double> > > cmat::mult_M2MH(vector<vector<complex<double> > > &m1, vector<vector<double> >  &m2) {
  if ((m1.size() ==0) || (m2.size()==0) || (m1[0].size()==0) || (m2[0].size()==0)) {
     cerr << "error in matrixmultiplication, one dimension is 0" << endl ;
     throw "error in matrixmultiplication, one dimension is 0" ;
  }
  if (m1[0].size() != m2[0].size()) {
    cerr << "error in matrixmultiplication, dimensions of the matrix do mot fit" << endl ;
    throw "error in matrixmultiplication, dimensions of the matrix do mot fit";
  }
  /* calculate the dimensions of the result matrix */
  uint rows=m1.size() ;
  uint cols=m2.size() ;
  uint mults=m1[0].size() ;
  /* create the result matrix */
  vector<vector<complex<double> > > res ;
  res.resize(rows) ;
  for (uint i=0; i<rows; i++) {
    res[i].resize(cols) ;
  }
  /*perform the multiplication  */
  for (uint i1=0; i1<rows; i1++) {
    for (uint i2=0; i2<cols; i2++) {
      complex<double> val(0,0);
      for(uint i3=0; i3<mults; i3++) {
        val = val+m1[i1][i3]*m2[i2][i3] ;
      }
      res[i1][i2] = val ;
    }
  }
  return res;
}

/*! procedure performs the multiplication  of two Matrices represented by
    stl vector of vectors. In this case the multiplication is performed on complex matrix complex adjunct
    with a seccond one. The number of rows of matrices 
    must be the same. The calculating of the adjunct is not done explicitly 
    The result is also vector of vectors and the entries of the matrix are complex */
vector<vector<complex<double> > > cmat::mult_MH2M(vector<vector<complex<double> > > &m1, vector<vector<complex<double> > > &m2) {
  if ((m1.size() ==0) || (m2.size()==0) || (m1[0].size()==0) || (m2[0].size()==0)) {
     cerr << "error in matrixmultiplication, one dimension is 0" << endl ;
     throw "error in matrixmultiplication, one dimension is 0" ;
  }
  if (m1.size() != m2.size()) {
    cerr << "error in matrixmultiplication, dimensions of the matrix do mot fit" << endl ;
    throw "error in matrixmultiplication, dimensions of the matrix do mot fit";
  }
  /* calculate the dimensions of the result matrix */
  uint rows=m1[0].size() ;
  uint cols=m2[0].size() ;
  uint mults=m1.size() ;
  /* create the result matrix */
  vector<vector<complex<double> > > res ;
  res.resize(rows) ;
  for (uint i=0; i<rows; i++) {
    res[i].resize(cols) ;
  }
  /*perform the multiplication  */
  for (uint i1=0; i1<rows; i1++) {
    for (uint i2=0; i2<cols; i2++) {
      complex<double> val(0,0);
      for(uint i3=0; i3<mults; i3++) {
        val = val+conj(m1[i3][i1])*m2[i3][i2] ;
      }
      res[i1][i2] = val ;
    }
  }
  return res;
}

/*! procedure performs the multiplication  of two Matrices represented by
    stl vector of vectors. In this case the multiplication is performed on complex matrix complex adjunct
    with a seccond one. The number of rows of matrices 
    must be the same. The calculating of the adjunct is not done explicitly 
    The result is also vector of vectors and the entries of the matrix are complex */
vector<vector<complex<double> > > cmat::mult_MH2M(vector<vector<complex<double> > > &m1, vector<vector<double> > &m2) {
  if ((m1.size() ==0) || (m2.size()==0) || (m1[0].size()==0) || (m2[0].size()==0)) {
     cerr << "error in matrixmultiplication, one dimension is 0" << endl ;
     throw "error in matrixmultiplication, one dimension is 0" ;
  }
  if (m1.size() != m2.size()) {
    cerr << "error in matrixmultiplication, dimensions of the matrix do mot fit" << endl ;
    throw "error in matrixmultiplication, dimensions of the matrix do mot fit";
  }
  /* calculate the dimensions of the result matrix */
  uint rows=m1[0].size() ;
  uint cols=m2[0].size() ;
  uint mults=m1.size() ;
  /* create the result matrix */
  vector<vector<complex<double> > > res ;
  res.resize(rows) ;
  for (uint i=0; i<rows; i++) {
    res[i].resize(cols) ;
  }
  /*perform the multiplication  */
  for (uint i1=0; i1<rows; i1++) {
    for (uint i2=0; i2<cols; i2++) {
      complex<double> val(0,0);
      for(uint i3=0; i3<mults; i3++) {
        val = val+conj(m1[i3][i1])*m2[i3][i2] ;
      }
      res[i1][i2] = val ;
    }
  }
  return res;
}
/*! procedure performs the multiplication of two Matrices represented by
    stl vector of vectors. In this case the multiplication is performed on complex matrix with
    the complex adjunct of the seccond one. The number of cols of matrices 
    must be the same. The calculating of the adjunct is not done explicitly 
    The result is also vector of vectors and the entries of the matrix are complex */
vector<vector<double> > cmat::mult_M2MH(vector<vector<double> > &m1, vector<vector<double> > &m2) {
  if ((m1.size() ==0) || (m2.size()==0) || (m1[0].size()==0) || (m2[0].size()==0)) {
     cerr << "error in matrixmultiplication, one dimension is 0" << endl ;
     throw "error in matrixmultiplication, one dimension is 0" ;
  }
  if (m1[0].size() != m2[0].size()) {
    cerr << "error in matrixmultiplication, dimensions of the matrix do mot fit" << endl ;
    throw "error in matrixmultiplication, dimensions of the matrix do mot fit";
  }
  /* calculate the dimensions of the result matrix */
  uint rows=m1.size() ;
  uint cols=m2.size() ;
  uint mults=m1[0].size() ;
  /* create the result matrix */
  vector<vector<double> > res ;
  res.resize(rows) ;
  for (uint i=0; i<rows; i++) {
    res[i].resize(cols) ;
  }
  /*perform the multiplication  */
  for (uint i1=0; i1<rows; i1++) {
    for (uint i2=0; i2<cols; i2++) {
      double val=0.0;
      for(uint i3=0; i3<mults; i3++) {
        val = val+m1[i1][i3]*m2[i2][i3] ;
      }
      res[i1][i2] = val ;
    }
  }
  return res;
}

/*! procedure performs the multiplication  of two Matrices represented by
    stl vector of vectors. In this case the multiplication is performed on complex matrix complex adjunct
    with a seccond one. The number of rows of matrices 
    must be the same. The calculating of the adjunct is not done explicitly 
    The result is also vector of vectors and the entries of the matrix are complex */
vector<vector<double> > cmat::mult_MH2M(vector<vector<double> > &m1, vector<vector<double> > &m2) {
  if ((m1.size() ==0) || (m2.size()==0) || (m1[0].size()==0) || (m2[0].size()==0)) {
     cerr << "error in matrixmultiplication, one dimension is 0" << endl ;
     throw "error in matrixmultiplication, one dimension is 0" ;
  }
  if (m1.size() != m2.size()) {
    cerr << "error in matrixmultiplication, dimensions of the matrix do mot fit" << endl ;
    throw "error in matrixmultiplication, dimensions of the matrix do mot fit";
  }
  /* calculate the dimensions of the result matrix */
  uint rows=m1[0].size() ;
  uint cols=m2[0].size() ;
  uint mults=m1.size() ;
  /* create the result matrix */
  vector<vector<double> > res ;
  res.resize(rows) ;
  for (uint i=0; i<rows; i++) {
    res[i].resize(cols) ;
  }
  /*perform the multiplication  */
  for (uint i1=0; i1<rows; i1++) {
    for (uint i2=0; i2<cols; i2++) {
      double val = 0.0;
      for(uint i3=0; i3<mults; i3++) {
        val = val+m1[i3][i1]*m2[i3][i2] ;
      }
      res[i1][i2] = val ;
    }
  }
  return res;
}

/*! procedure performs the multiplication s muliplivation of two Matrices represented by
    stl vector of vectors. In this case the multiplication is performed on complex matrix with 
    a real matrix. The complex matrix is the left one the real is the right one. The
    number of cols of the complex matrix must agree to the number of rows of the real matrix.
    The result is also vector of vectors and the entries of the matrix are complex */
vector<vector<complex<double> > > cmat::mult(vector<vector<complex<double> > > &m1, vector<vector<double> > &m2) {
  if ((m1.size() ==0) || (m2.size()==0) || (m1[0].size()==0) || (m2[0].size()==0)) {
     cerr << "error in matrixmultiplication, one dimension is 0" << endl ;
     throw "error in matrixmultiplication, one dimension is 0" ;
  }
  if (m1[0].size() != m2.size()) {
    cerr << "error in matrixmultiplication, dimensions of the matrix do mot fit" << endl ;
    throw "error in matrixmultiplication, dimensions of the matrix do mot fit";
  }
  /* calculate the dimensions of the result matrix */
  uint rows=m1.size() ;
  uint cols=m2[0].size() ;
  uint mults=m2.size() ;
  /* create the result matrix */
  vector<vector<complex<double> > > res ;
  res.resize(rows) ;
  for (uint i=0; i<rows; i++) {
    res[i].resize(cols) ;
  }
  /*perform the multiplication  */
  for (uint i1=0; i1<rows; i1++) {
    for (uint i2=0; i2<cols; i2++) {
      complex<double> val(0,0);
      for(uint i3=0; i3<mults; i3++) {
        val = val+m1[i1][i3]*m2[i3][i2] ;
      }
      res[i1][i2] = val ;
    }
  }
  return res;
}

/*! procedure performs the multiplication s muliplivation of two Matrices represented by
    stl vector of vectors. In this case the multiplication is performed on complex matrix with 
    a real matrix. The complex matrix is the right one the real is the left one. The
    number of cols of the real matrix must agree to the number of rows of the complex matrix.
    The result is also vector of vectors and the entries of the matrix are complex */
vector<vector<complex<double> > > cmat::mult(vector<vector<double> > &m1,vector<vector<complex<double> > > &m2) {
  if ((m1.size() ==0) || (m2.size()==0) || (m1[0].size()==0) || (m2[0].size()==0)) {
     cerr << "error in matrixmultiplication, one dimension is 0" << endl ;
     throw "error in matrixmultiplication, one dimension is 0" ;
  }
  if (m1[0].size() != m2.size()) {
    cerr << "error in matrixmultiplication, dimensions of the matrix do mot fit" << endl ;
    throw "error in matrixmultiplication, dimensions of the matrix do mot fit";
  }
  /* calculate the dimensions of the result matrix */
  uint rows=m1.size() ;
  uint cols=m2[0].size() ;
  uint mults=m2.size() ;
  /* create the result matrix */
  vector<vector<complex<double> > > res ;
  res.resize(rows) ;
  for (uint i=0; i<rows; i++) {
    res[i].resize(cols) ;
  }
  /*perform the multiplication  */
  for (uint i1=0; i1<rows; i1++) {
    for (uint i2=0; i2<cols; i2++) {
      complex<double> val(0,0);
      for(uint i3=0; i3<mults; i3++) {
        val = val+m1[i1][i3]*m2[i3][i2] ;
      }
      res[i1][i2] = val ;
    }
  }
  return res;
}

/*! procedure peforms a multiplication of two matrices, where the left of them is a
    diagonal matrix represented by a vector of the diagonal elements only.
    The right matrix is assumed to be a quadratic real matrix */
vector<vector<double> > cmat::mult(vec &diag, vector<vector<double> > &mat) {
  if ((mat.size()==0) || (diag.size() == 0) || (mat[0].size() == 0)) {
     cerr << "error in matrixmultiplication, one dimension is 0" << endl ;
     throw "error in matrixmultiplication, one dimension is 0" ;
  }
  if ((mat.size() != mat[0].size()) || (mat.size() != diag.size())) {
    cerr << "error in matrixmultiplication, dimensions of the matrix do mot fit" << endl ;
    throw "error in matrixmultiplication, dimensions of the matrix do mot fit";
  }
  /* calculate the dimensions of the result matrix */
  uint num=diag.size() ;
  /* create the result matrix */
  vector<vector<double> >  res ;
  res.resize(num) ;
  for (uint i=0; i<num; i++) {
    res[i].resize(num) ;
  }
  /*perform the multiplication  */
  for (uint i1=0; i1<num; i1++) {
    for (uint i2=0; i2<num; i2++) {
      res[i1][i2] = diag[i1]*mat[i1][i2] ;
    }
  }
  return res;

}

/*! procedure peforms a multiplication of two matrices, where the right of them is a
    diagonal matrix represented by a vector of the diagonal elements only.
    The left matrix is assumed to be a quadratic real matrix */
vector<vector<double> > cmat::mult(vector<vector<double> > &mat, vec &diag) {
  if ((mat.size()==0) || (diag.size() == 0) || (mat[0].size() == 0)) {
     cerr << "error in matrixmultiplication, one dimension is 0" << endl ;
     throw "error in matrixmultiplication, one dimension is 0" ;
  }
  if ((mat.size() != mat[0].size()) || (mat.size() != diag.size())) {
    cerr << "error in matrixmultiplication, dimensions of the matrix do mot fit" << endl ;
    throw "error in matrixmultiplication, dimensions of the matrix do mot fit";
  }
  /* calculate the dimensions of the result matrix */
  uint num=diag.size() ;
  /* create the result matrix */
  vector<vector<double> >  res ;
  res.resize(num) ;
  for (uint i=0; i<num; i++) {
    res[i].resize(num) ;
  }
  /*perform the multiplication  */
  for (uint i1=0; i1<num; i1++) {
    for (uint i2=0; i2<num; i2++) {
      res[i1][i2] = diag[i2]*mat[i1][i2] ;
    }
  }
  return res;

}
/*!calculates the comples adjoint of the current matrix */
cmat cmat::H() {
  cmat erg(cols(),rows()) ;
  for (uint i =0; i<rows(); i++) {
    vector<complex<double> > &u(data[i]) ;
    for (uint j=0; j<cols(); j++) {
       erg.set(j,i,conj(u[j])) ;
    }
  }
  return erg ;
}
/*! calculates the fft of the current matrix. Which means:
    result = fft*matrix*fft^-1 where fft is the matrix which represents
    the fft and fft^1 the inverse of it. Since fft is orthogonal 
    fft^-1 = adj(fft) */
cmat cmat::fft() {
  if (rows() != cols()) {
    cerr << "no fft for non quadratic matrixes\n" ;
    throw "no fft for non quadratic matrixes\n" ;
  }
  uint dim = rows() ;
  cmat erg(dim,dim) ;
  for (uint i =0; i<rows(); i++) {
    vector<complex<double> > &u(data[i]) ;
    vector<complex<double> > v(dim) ;
    /* perform inverse fourier transformation to each row of the matrix */
    iFFT(u,v) ;
    erg.data[i] = v ;    
  }
  cmat res = erg.t() ; // transpone the matrix 
  /* fourier transformation of the collums of trasformed matrix (here we are 
     transforming the rows, but we transponed the Matrix before */
  for (uint i =0; i<rows(); i++) {
    vector<complex<double> > &u(res.data[i]) ;
    vector<complex<double> > v(dim) ;
    /* perform inverse fourier transformation to each row of the matrix */
    FFT(u,v) ;
    erg.data[i] = v ;
  }
  res = erg.t() ; // transpone the matrix  to get the original order  
  return res ;
}
cmat cmat::t() {
  cmat erg(cols(),rows()) ;
  for (uint i =0; i<rows(); i++) {
    vector<complex<double> > &u(data[i]) ;
    for (uint j=0; j<cols(); j++) {
       erg.set(j,i,u[j]) ;
    }
  }
  return erg ;
}

complex<double> cmat::det() {
  GaussEle() ;
  complex<double> erg = 1.0 ;
  for (uint i=0; i<rows();i++) {
     erg = erg*get(i,i) ;
  }
  return erg ;
}

complex<double> cmat::diaDet() {
  complex<double> erg = 1.0 ;
  for (uint i=0; i<rows();i++) {
     erg = erg*get(i,i) ;
  }
  return erg ;
}

double cmat::hermDiaDet() {
  double erg = 1.0 ;
  for (uint i=0; i<rows();i++) {
     erg = erg*get(i,i).real() ;
  }
  return erg ;
}

mat mat::inv() {
  mat zw ;
  zw.data = data ;
  return zw.GaussEle() ;
}

double mat::det() {
  GaussEle() ;
  double erg = 1.0 ;
  for (uint i=0; i<rows();i++) {
     erg = erg*get(i,i) ;
  }
  return erg ;
}

double mat::diaDet() {
  double erg = 1.0 ;
  for (uint i=0; i<rows();i++) {
     erg = erg*get(i,i) ;
  }
  return erg ;
}
/*! Procedure performs an complete elimination schema including the following steps:
    1. Forward elimination
    2. backward elimination
    3. normalisation */
mat mat::GaussEle() {
  if (rows() != cols()) {
    throw "no Elimination implemented for non quadratic matrixes\n" ;
  }
  stack<permEntry> perm ;
  mat unity(rows(),rows()) ;
  unity.fillUnit() ;
  /* forward elemination */
  for (uint i=0; i<rows()-1; i++) {
    uint ro = getMaxPivot(i) ;
    if (ro != i) {
	data[ro].swap(data[i]);
	permEntry eint(ro,i) ;
	perm.push(eint) ;
    }    	
    if (data[i][i] == 0 ) {
      throw "Matrix is not invertible \n";
    }
    for (uint j= i+1; j< rows(); j++ ) {
      double fak = -data[j][i]/data[i][i];
// if (j==i+1) cout << "Eliimnation: " << j << " " << fak << " " 	<< data[j][i] << endl ;	
      add(j,i,fak);
      unity.add(j,i,fak);
    }
  }
  /* backward elemination */
    for (uint i=rows()-1; i>0; i--) {    
      for (uint j= 0; j< i; j++ ) {
        double fak = -data[j][i]/data[i][i];
        add(j,i,fak);
        unity.add(j,i,fak);
      }
    }
    /* noralisation */
    for (uint i=0; i<rows(); i++) {
	double fak = 1.0/data[i][i] ;
	unity.mult(fak,i) ;
    }
    /* swap collumns of result */
    while(!perm.empty()) {
      permEntry per = perm.top() ;
      perm.pop() ;
      unity.swapCols(per.pos1,per.pos2) ;
    }
  return unity;
}

void mat::write(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<rows(); i++) {
    vector<double> &aus(data[i]) ;
    for (uint j=0; j<aus.size() ; j++) {
      outfile << aus[j] << " " ;
    }
    outfile << endl ;
  }
  outfile.close();

}

void mat::fillUnit() {
  uint nCol = cols() ;
  uint nRow = rows() ;
  if (nRow != nCol) {
    throw "no unit for non quadratic matrixes\n" ;
  }
  for (uint i=0; i< nRow ; i++) {
     for (uint j=0; j<nRow; j++ ) {
	 data[i][j] =(i == j) ? 1 : 0; // kronecer delta
     }
  }
}

double mat::maxi() {
  double erg = 0 ;
  for (uint i=0; i<rows();i++) {
    vector<double>  &row = data[i] ;
    for (uint j=0; j< cols() ; j++) {
      if (erg < fabs(row[j])) erg = fabs(row[j]) ;
    }
  } 
  return erg;
}

mat mat::H() {
  mat erg(cols(),rows()) ;
  for (uint i =0; i<rows(); i++) {
    vector<double>  &u(data[i]) ;
    for (uint j=0; j<cols(); j++) {
       erg.set(j,i,u[j]) ;
    }
  }
  return erg ;
}

double mat::prod(vector<double> &vek1, vector<double> &vek2) {
  uint s1 = vek1.size() ;
  uint s2 = vek2.size() ;
  double erg = 0 ;
  if (s1 != s2) {
   throw "imcomplatible dimensions \n"; 
  }
  else {
    for (uint i=0; i<s1; i++) {
      erg = erg + vek1[i]*vek2[i] ;
    }
  }
  return erg ;
}

double mat::getPivot(vector<double> &vek, uint start) {
  double erg = fabs(vek[start]) ;
  
  erg = erg/Abs(vek,start) ;
  return erg ;
}

uint mat::getMaxPivot(uint start) {
  double pivot = 0 ;
  uint erg = start ;
  for (uint i=start; i<data.size(); i++) {
    double test = getPivot(data[i],start) ;
    if (test>pivot) {
	pivot = test ;
	erg = i ;
    }
  }
  return erg ;
}

uint cmat::getMaxPivot(uint start) {
  double pivot = 0 ;
  uint erg = start ;  
  for (uint i=start; i<data.size(); i++) {
    double test = getPivot(data[i],start) ;
    if (test>pivot) {
	pivot = test ;
	erg = i ;
    }
  }
  return erg ;
}

double mat::Abs(vector<double> &vek, uint anf) {
  double erg = 0 ;
  for (uint i=anf; i<vek.size(); i++) {
    erg = erg + vek[i]*vek[i] ;
  }
  return sqrt(erg) ;
}

double cmat::Abs(vector<complex<double> > &vek, uint anf) {
  double erg = 0 ;
  for (uint i=anf; i<vek.size(); i++) {
    erg = erg + (vek[i]*conj(vek[i])).real() ;
  }
  return sqrt(erg) ;
}

double cmat::getPivot(vector<complex<double> > &vek, uint start) {
  double erg = abs(vek[start]) ;
  erg = erg/Abs(vek,start) ;
  return erg ;
}

      /*! procedure which returns the fft of an vector */
void cvec::fft(cvec &result) {
  FFT(data,result.data) ;
}
void cvec::fftshift(cvec &reslut) {
}

/*! constructor needs a quadratic matric and get the vector
   object from the diagonal elements of the matrix */
cvec::cvec(cmat &mat) {
  uint dim1 = mat.rows() ;
  uint dim2 = mat.cols() ;
  if (dim1 != dim2) {
    cerr << "Matrix must be quadratic ! " << endl ;
    throw "Matrix must be quadratic ! " ;
  }
  data.resize(dim1) ;  // define dimension of the new vector 
  for (uint i=0; i<dim1; i++) {  // loop over all diagonal elements of the matrix
    data[i] = mat.get(i,i) ;
  }
}
      /*! procedure which returns the inverse fft of an vector */
void cvec::ifft(cvec &result) {
  iFFT(data,result.data) ;
}

void grid::arange(double start, double stop, double step) {
  uint ind = 0 ;
  double cur = start ;
  cout << "test: " << start << " " << stop <<  " " << step << endl ;
  while  (cur < stop) {
    points.data.push_back(cur) ;
    ind++ ;
    cur = cur+step ;
  }
  cout << "Zahl: " << points.size() << endl ; 
}

double grid::gcf_expsinc(double x, double dx){
    // GCF using an exponentially damped sinc 

    double w1 = 2.52 ;
    double w2 = 1.55 ; // see Briggs et al, 1999
    double alpha = 2.0 ;
    double C ;
    if (x!= 0.) 
        C = exp(-1.*pow((abs(x)/w1/dx),alpha))*sin(PI*x/w2/dx)/(x/w2/dx)/PI ;
    else
        C = exp(-1.*pow((abs(x)/w1/dx),alpha)) ;
    
    return C;
}

void grid::make_x_grid(vec &x, uint m, double dy, uint ny) {
    // REMEMBER that I have to scale the gridded x axis (multiply by pi) if using 
    // this for Fourier synthesis.
    // I should probably split this into it's own function
    double dx = 1./dy/ny ;
    uint nx = ny ; 
    double deltax = x[x.size()-1] - x[0] ;
    cout << "ny= " << ny  << "  nx*dx= " << nx*dx <<"  2*m*dx= " << 2*m*dx << "  deltax+2*m*dx= " << deltax+2*m*dx << endl ;
    //EXCEPT
    if (nx*dx < deltax+2*m*dx) {
        cerr << "ERROR: Invalid y axis requested, aliasing will occur" << endl ;
        throw "ERROR: Invalid y axis requested, aliasing will occur";
    }
    
    double xstart = x[0]-m*dx ;
    arange(xstart, xstart + nx*dx, dx);
    
    // sometimes the arange produces one too many values due to 
    // numerical accuracy problems
    if (points.size() == nx+1) {
        points.data.pop_back() ;
    }
  }

/*! rewrite so this only returns the gridded f
    user uses the make_x_grid function to actually get the grid, only needs be done once
    change to pass xgrid here as arg rather than ungridded x*/
 void grid::pre_fft_gridding(cvec &f, vec &x, double dy, uint ny){

    /* Gridding routine used to prepare a function for the FFT algorithm.  Some 
    function f which is to be Fourier transformed is defined at irregularly spaced 
    locations x.  The grid size in x is determined from the requested Fourier space grid 
    defined by dy and ny.  This function returns the x grid and the gridded (and 
    smoothed) version of f.  x must be in order from low to high values.  */

    uint m = 6; // number of grid cells to use for convolution
    make_x_grid(x, m, dy, ny) ;
    complex<double> zero(0,0) ;
    fgrid.set_vals(points.size(), zero) ;
    double dx = points[1] - points[0] ; 
    // do the convolution
    for (uint indx=0; indx<x.size() ; indx++) {
        double xmin = x[indx] - 0.5*m*dx ;
        double xmax = x[indx] + 0.5*m*dx ;
        
        // compute range overwhich we should be convolving
        uint xmindx = (uint) ceil((xmin - points[0])/dx) ;
        uint xmaxdx = (uint) floor((xmax - points[0])/dx) ;
        
        if (xmindx < 0)
            xmindx = 0 ;
            
        if (xmaxdx >= points.size()) 
            xmaxdx = points.size() ;
        
        for (uint jndx = xmindx ; jndx < xmaxdx; jndx++ ) {
//             gcfval = gcf_pswf(abs(xgrid[jndx] - x[indx]), m, dx)
            double gcfval = gcf_expsinc(points[jndx] - x[indx], dx) ;
            fgrid[jndx] = fgrid[jndx] + f[indx]*gcfval ;
       }
    } 
//     return xgrid, fgrid
  }

/*!        RMSynth.compute_dirty_image(pol)
           Inverts the given polarization vector to arrive at the dirty dispersion 
           function.  
         
        Inputs:
            pol-    An Nx1 numpy array containing the complex polarized emission 
                    as a function of frequency (or lambda^2 if that is how the 
                    freq. axis has been defined).  Must be in order from low to 
                    high along freq. axis.  
        Outputs:
            1-      A complex valued map of the dirty dispersion function.
        */
  void grid::compute_dirty_image(cvec &pol, vec &x, double delta, uint num){
        dphi = delta ;
        nphi = num ;
//       Flipping pol vector
//         pol.flip() ;
        pre_fft_gridding(pol, x, dphi, nphi);
        // EXCEPT... although probably not necessary
//         if(l2grid.size  != len(self.l2):
//             print 'Inconsistency in calculation of l2 grid' 
        cvec residual_map(pol.size());
	FFT(pol.data, residual_map.data) ;
        residual_map.realOut("rmR.dat");
        residual_map.imagOut("rmI.dat");
//         residual_map = self.K*numpy.fft.fftshift(numpy.fft.fft(polgrid))
        for (uint indx =0; indx<nphi ; indx++) { 
//             rot = 2.*(self.l20 - self.l2i)*self.phi[indx]
//              shift results by the L20 and by the initial l2 value to account for 
//              the fact that we start at L2!=0 as FFT expects
//             residual_map[indx] = numpy.complex(math.cos(rot), math.sin(rot))*residual_map[indx]
        }
        
//         grid_norm = G.gridding_norm(self.dphi, self.nphi)
  }
/*! procedure returns the index of the maximal abslute valued element of the 
    complex valued vector. There is always a result returned, if more elements have the same 
    absolute value, the lowest index is returned.
    \param vekt vector to find the element for 
    \return index of the maximal abslute valued element */
uint maxAbs(vector<complex<double> > vekt) {
  double max = 0 ; // absolute value >= 0
  uint res = 0 ;  // default result is 0
  for (uint i=0; i<vekt.size(); i++) { // loop to find the maximal value and its index
    if (abs(vekt[i]) > max) { // current element is larger than the current maximal value
      max = abs(vekt[i]) ;
      res = i ;
    }
  }
  return res ;
}

/*! procedure returns the mean value of all local maxima of the  
    absolute values of the complex vector.
    \param vekt vector to find the element for 
    \return mean value of all local maxima of the polarized intensity */
double meanPeak(vector<complex<double> > vekt) {
  double res = 0.0 ;
  uint num=0;
  for (uint i=1; i<vekt.size()-1; i++) {
    if((abs(vekt[i])>abs(vekt[i-1])) && (abs(vekt[i])>abs(vekt[i+1]))) {
       res+=abs(vekt[i]);
       num++ ;
    }
  }
  res = res/num ;
  return res ;
}
        
//         return residual_map/abs(grid_norm) 

}
