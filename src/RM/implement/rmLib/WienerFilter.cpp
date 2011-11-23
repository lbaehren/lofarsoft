//  	ITPP Test program and Wiener Filter test implementation
//
//	File:			wienerfilter.cpp
//	Author:			Sven Duscha (sduscha@mpa-garching.mpg.de)
//	Date:			09-06-2009
//	Last change:		18-10-2009

/* Standard header files */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <complex>
#include <vector>
#include <stack>
#include <fstream>
#include <algorithm>			// for minimum element in vector
#include <limits>			// limit for S infinite
#include <iomanip>			// to set precision in cout
/* RM header files */
#include "WienerFilter.h"

using namespace std;

namespace RM {
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  /*!
    \brief Default constructor, initializing with default parameters
    
    The default constructor sets internal variables for matrix filenames,
    and paramater values to defaults:
    
    variance_s=25
    lambda_phi=5
    maxiterations=1
    rmsthreshold=-1
    nu_0=1400000000
    epsilon_0=1e-26
  */
  wienerfilter::wienerfilter()
  {
    // Set default filenames
    Sfilename="Smatrix.it";				// filename for S matrix file
    Rfilename="Rmatrix.it";				// filename for R matrix file
    Nfilename="Nmatrix.it";				// filename for N matrix file
    Dfilename="Dmatrix.it";				// filename for D matrix file
    Qfilename="Qmatrix.it";  			// filename for Q matrix file
    
    variance_s=0;					// initialize variance of s
    lambda_phi=5;					// initialize lambda phi coherence length	maxiterations=100;				// default value for maximum number of iterations
    maxiterations=1;				// default value for maximum number of iterations
    rmsthreshold=-1;				// default, don't try to achieve rms threshold
    nu_0=1400000000;				// default value for nu_0=1.4GHz
    epsilon_0=1.0;				// epsilon zero (Jy conversion)
    R_ready=false ;
    D_ready=false ;
    WF_ready=false ;
  }

/*! 
   \brief Procedure to write the signal covariance matrix into a ascii file */
void wienerfilter::writeSignalMatrix(const string &filename){

  S.write(filename) ;
}
  
void wienerfilter::writeNoiseMatrix(const string &filename) {
  N.write(filename) ;
}

void wienerfilter::readResponseMatrix(const string &filename){

  R.write(filename) ;
}
  
  /*!
    \brief Constructor with frequency and Faraday depth dimenions
    
    This constructor sets values for the number of lambda squared (frequency) channels
    and the number of faraday depths to be probed for. This allows to set already
    appropriate lengths for both the data and the signal vector.
    
    \param nfreqs - number of frequency channels in data d
    \param nfaradays - number of faradaydepths to probe for, size of signal s
  */
  wienerfilter::wienerfilter(int nfreqs, int nfaradays)
  {
    d.set_size(nfreqs);				// set length of data vector to number of frequencies
    s.set_size(nfaradays);				// set length of signal vector to number of Faraday depths
    j.set_size(s.size());				// set j to size of s
    maperror.set_size(s.size());			// set size of map error the same as s
    m.set_size(nfaradays);				// set size of m to number of faraday depths
    faradaydepths.resize(nfaradays);		// set size of faradaydepths
    delta_faradaydepths.resize(nfaradays);		// resize vector containing delta Faraday depth distances
    frequencies.resize(nfreqs);			// set size of frequencies vector
    delta_frequencies.resize(nfreqs);		// set size of delta_frequencies_squareds
    lambdasquareds.resize(nfreqs);			// set size of lambdasquareds vector
    delta_lambdasquareds.resize(nfreqs);		// set size of delta_lambda_squareds
    bandpass.resize(nfreqs);			// bandpass vector for each frequency
    
    // Initialize spectral dependence parameters
    alpha=-0.7;					// spectral dependence power law factor alpha
    nu_0=1400000000;				// default value for nu_0=1.4GHz
    epsilon_0=1e-26;				// epsilon zero (Jy conversion)
    
    // Set default filenames
    Sfilename="Smatrix.it";				// filename for S matrix file
    Rfilename="Rmatrix.it";				// filename for R matrix file
    Nfilename="Nmatrix.it";				// filename for N matrix file
    Dfilename="Dmatrix.it";				// filename for D matrix file
    Qfilename="Qmatrix.it";  			// filename for Q matrix file
    
    variance_s=25;					// initialize variance of s
    lambda_phi=50;					// initialize lambda phi coherence length
    Sformula="";					// initial S guess formula;
    maxiterations=10;				// default value for maximum number of iterations
    rmsthreshold=-1;				// default, don't try to achieve rms threshold
    
    for(int i=0; i<nfreqs; i++)			// write default values into bandpass vector
      bandpass[i]=complex<double>(1,1);		// default is 1 both for Q and U
    
    //	cout << "wienerfilter():--frequencies.size() " << frequencies.size() << endl;				// debug
    //	cout << "wienerfilter():--delta_frequencies.size() " << delta_frequencies.size() << endl;	// debug
    R_ready=false ;
  }
  
/*! Procedure prepares the Wienerfilter object for a flat prior and no information
   about the noise. This means, that the resulting Matrix coresponds to the 
   Marcow- Guass- Regularisation of processing inverse problems. */

void wienerfilter::prepare(vector<double> &freqsC, vector<double> &freqsI, vector<double> &faras, double nu_0, double alpha, double epsilon, int method) {
  if((method!=1) &&(method !=4) && (!WF_ready)) {
    setFrequencies(freqsC) ;
    setFaradayDepths(faras) ;
    alpha = alpha;
    nu_0 = nu_0 ;
    epsilon_0 = epsilon ;
    if (method ==2) {  // use common Wiener filtering
      createResponseMatrix(freqsI,epsilon,0 );
      computeD_NoNoiseCov() ;
      computeW_noNoise() ;    
      WF = D*W;
      WF_ready = true ;
    }
    else if (method ==3) {  // use svd
      createResponseMatrix(freqsI,epsilon,1 );
      cmat RH = R.H() ;
//       cout << "RH calculated " << endl ;
      cmat RhR = RH*R ;
//       cout << "calculated Rh, calculate Propagator using eingenvalues" << endl ;
      D = RhR.calcPosEig() ;
//       cout << "Propagator calculated" << endl ;
      WF = D*RH ;
//       cout << "computed reconstruction matrix " << WF.rows() << " " << WF.cols() << endl ;
      WF_ready = true ;
    }
//     R.absOut("R.mat");
    R_ready = true ;

  }
}
  
  /*!
    \brief Constructor with frequency and Faraday depth dimenions
  
  This constructor sets values for the number of lambda squared (frequency) channels
  and the number of faraday depths to be probed for. This allows to set already
  appropriate lengths for both the data and the signal vector.
  
  \param nfreqs - number of frequency channels in data d
  \param faradays - vector containing Faraday depths to be probed for
*/
wienerfilter::wienerfilter(int nfreqs, vector<double> faradays)
{
  d.set_size(nfreqs);				// set length of data vector to number of frequencies
  s.set_size(faradays.size());			// set length of signal vector to number of Faraday depths
  j.set_size(s.size());				// set j to size of s
  maperror.set_size(s.size());			// set size of map error the same as s
  m.set_size(faradays.size());			// set size of m
  faradaydepths=faradays;				// set faradaydepths to the content of the vector faradays provided as parameter
  delta_faradaydepths.resize(faradays.size());	// resize vector containing delta Faraday depth distances
  frequencies.resize(nfreqs);			// set length of vector for frequencies
  delta_frequencies.resize(nfreqs);		// set length of vector for delta frequencies
  lambdasquareds.resize(nfreqs);			// set size of lambdasquareds vector
  delta_lambdasquareds.resize(nfreqs);		// set size of delta_lambda_squareds	
  bandpass.resize(nfreqs);			// bandpass vector for each frequency
  
  // Set default filenames
  Sfilename="Smatrix.it";				// filename for S matrix file
  Rfilename="Rmatrix.it";				// filename for R matrix file
  Nfilename="Nmatrix.it";				// filename for N matrix file
  Dfilename="Dmatrix.it";				// filename for D matrix file
  Qfilename="Qmatrix.it";				// filename for Q matrix file
  
  variance_s=25;					// initialize variance of s
  lambda_phi=0.5;					// initialize lambda phi coherence length
  nu_0=1400000000;				// default value for nu_0=1.4GHz
  epsilon_0=1e-26;				// epsilon zero (Jy conversion)
  
  maxiterations=10;				// default value for maximum number of iterations
  rmsthreshold=-1;				// default, don't try to achieve rms threshold
  
  for(int i=0; i<nfreqs; i++)			// write default values into bandpass vector
    bandpass[i]=complex<double>(1,1);	// default is 1 both for Q and U
  R_ready=false ;
}


/*!
  \brief Constructor with frequency and Faraday depth dimenions and spectral dependency formula
  
  This constructor sets values for the number of lambda squared (frequency) channels
  and the number of faraday depths to be probed for. This allows to set already
  appropriate lengths for both the data and the signal vector. Additionaly, the formula
  for the initial guess of the signal covariance matrix S is given as a  parameter.
  
  \param frequencies - number of frequency channels in data d
  \param faradaydepths - number of faradaydepths to probe for, size of signal s
  \param formula - formula for spectral dependency in s
*/
wienerfilter::wienerfilter(int nfreqs, int nfaradays, std::string &formula)
{
  d.set_size(nfreqs);					// set length of data vector to number of frequencies
  s.set_size(nfaradays);				// set length of signal vector to number of Faraday depths
  j.set_size(s.size());				// set j to size of s
  maperror.set_size(s.size());		// set size of map error the same as s
  m.set_size(nfaradays);				// set size of m
  faradaydepths.resize(nfaradays);	// resize vector that contains Faraday depths to be probed for
  delta_faradaydepths.resize(nfaradays);	// resize vector containing delta Faraday depth distances
  frequencies.resize(nfreqs);		// set size of frequencies
  delta_frequencies.resize(nfreqs);	// set size of delta frequencies to that of number of frequencies
  lambdasquareds.resize(nfreqs);			// set size of lambdasquareds vector
  delta_lambdasquareds.resize(nfreqs);		// set size of delta_lambda_squareds
  
  
  // Set default filenames
	Sfilename="Smatrix.it";				// filename for S matrix file
	Rfilename="Rmatrix.it";				// filename for R matrix file
	Nfilename="Nmatrix.it";				// filename for N matrix file
	Dfilename="Dmatrix.it";				// filename for D matrix file
	Qfilename="Qmatrix.it";  			// filename for Q matrix file

	Sformula=formula;				// initial S guess formula;
	variance_s=25;					// initialize variance of s
	lambda_phi=50;					// initialize lambda phi coherence length
	nu_0=1400000000;				// default value for nu_0=1.4GHz
	epsilon_0=1e-26;				// epsilon zero (Jy conversion)	
	maxiterations=10;				// default value for maximum number of iterations
	rmsthreshold=-1;				// default, don't try to achieve rms threshold
  R_ready=false ;
}


/*!
	\brief Destructor
	
	No memory is allocated outside the normal variable creation, so there is
	no freeing of memory necessary.
*/
wienerfilter::~wienerfilter()
{
	// free allocated memory
}



complex<double> wienerfilter::integrand(double x,double phi_a, double phi_b,double alpha) {
  const double csq=89875517873681764.0;
  complex<double> I(0,1) ;
  double xsq = x*x ;
  double wa = 2*csq/xsq*phi_a ;
  double wb = 2*csq/xsq*phi_b ;
  complex<double> exa = complex<double>(cos(wa),sin(wa));
  complex<double> exb = complex<double>(cos(wb),sin(wb));
  complex<double> res = xsq*pow(x,-alpha)*(exb-exa) ;
  return res ;
}

/*! procedure for an adaptive integral evaluation of the function integrand which describes
    the response of the faraday rotation.
    \param nu_a lower integration bound 
    \param nu_b upper intefration bound 
    \param eps accuracy of the numerical integration 
    \param phi_a lower bound in Faraday space (analytical integration)
    \param phi_b upper bound in Faraday space (analytical integration) */

complex<double> wienerfilter::integral(double nu_a, double nu_b, double eps,double alpha, double phi_a, double phi_b, int level ) {
  complex<double> erg ;  
//  double h = nu_b-nu_a ;  
  double cent = (nu_a+nu_b)/2 ;  
  double epsilon = 5.9604644775390625e-08 ;  // machine accuracy for double precission
  complex<double> area_sing = 0.5*( integrand(nu_a,phi_a,phi_b,alpha)+ integrand(nu_b,phi_a,phi_b,alpha)) ;  // the factor h vanished by shortening
  complex<double> area_doub = 0.25*(integrand(nu_a,phi_a,phi_b,alpha)+2.0*integrand(cent,phi_a,phi_b,alpha)+integrand(nu_b,phi_a,phi_b,alpha)) ;
//   bool cont = (abs(area_doub-area_sing) < (eps*abs(area_sing)+epsilon)) ;
  if (abs(area_doub-area_sing) <= (eps)*abs(area_sing)+epsilon) {
    erg = area_doub ;
  }
  else {
    complex<double> left = integral(nu_a,cent,eps,alpha,phi_a,phi_b,level+1) ;
    complex<double> right = integral(cent,nu_b,eps,alpha,phi_a,phi_b,level+1) ;
    erg = left+right;
  }
  return erg ;
}

complex<double> null(double nu1,double nu2, double alpha) {
  //cg6 = ((1 / (3 - alpha) * (nu2 ^ (3 - alpha) - nu1 ^ (3 - alpha)) / (nu2 - nu1)) ^ 1 / (2 - alpha)) ^ (2 - alpha) * (nu2 - nu1);
  double cg6 = (pow(nu1,(3 - alpha)) - pow(nu2,(3 - alpha))) / (-3 + alpha);
  complex<double> res(cg6,0) ;
  return res ;
}

complex<double>low(double phi,double nu1,double nu2, double alpha, double c ) {
  complex<double> i(0.0,1.0) ;
  complex<double> cg2 = (-0.1e1 / 0.4e1*i) * nu1 * nu2 * sqrt(nu1 * nu2) * exp((2.0*i) * c*c * phi * (0.3e1 * sqrt(nu1 * nu2) - 0.2e1 * nu1 - 0.2e1 * nu2) / nu1 / nu2 * (pow((nu1 * nu2),(-0.1e1 / 0.2e1)))) * (exp((4.0*i) * c*c * phi / nu1 * (pow((nu1 * nu2),(-0.1e1 / 0.2e1)))) - exp((4.0*i) * c*c * phi / nu2 * (pow((nu1 * nu2),(-0.1e1 / 0.2e1))))) * (pow(((pow(nu2,(3 - alpha)) - pow(nu1,(3 - alpha))) / (alpha * nu1 - alpha * nu2 - 0.3e1 * nu1 + 0.3e1 * nu2)) , (-2 / (-2 + alpha)))) * (pow((pow(((pow(nu2, (3 - alpha)) - pow(nu1 , (3 - alpha))) / (alpha * nu1 - alpha * nu2 - 0.3e1 * nu1 + 0.3e1 * nu2)) , (-1 / (-2 + alpha)))) , (-alpha))) / (c*c) / phi;
  return cg2 ;
}

complex<double> hi(double phi,double nu1,double nu2, double alpha, double c )  {
  complex<double> i(0.0,1.0) ;
  complex<double> cg1 = (-0.1e1 / 0.4e1*i) * nu1 * nu2 * sqrt(nu1 * nu2) * exp((2.0*i) * c*c * phi * (0.3e1 * sqrt(nu1 * nu2) - 0.2e1 * nu1 - 0.2e1 * nu2) / nu1 / nu2 * (pow((nu1 * nu2),(-0.1e1 / 0.2e1)))) * (exp((4.0*i) * c*c * phi / nu1 * (pow((nu1 * nu2), (-0.1e1 / 0.2e1)))) - exp((4.0*i) * c*c * phi / nu2 * (pow((nu1 * nu2), (-0.1e1 / 0.2e1))))) * (pow(((pow(nu2 , (3 - alpha)) - pow(nu1, (3 - alpha))) / (alpha * nu1 - alpha * nu2 - 0.3e1 * nu1 + 0.3e1 * nu2)) , (-2 / (-2 + alpha)))) * (pow((pow(((pow(nu2, (3 - alpha)) - pow(nu1 , (3 - alpha))) / (alpha * nu1 - alpha * nu2 - 0.3e1 * nu1 + 0.3e1 * nu2)), (-1 / (-2 + alpha)))) , (-alpha))) / (c*c) / phi;
  return cg1 ;
}
/*! procedure for an adaptive integral evaluation of the function integrand which describes
    the response of the faraday rotation.
    \param nu_a lower integration bound 
    \param nu_b upper intefration bound 
    \param eps accuracy of the numerical integration 
    \param phi_a lower bound in Faraday space (analytical integration)
    \param phi_b upper bound in Faraday space (analytical integration) */

complex<double> wienerfilter::integral_approx(double nu_a, double nu_b, double eps,double alpha, double phi_a, double phi_b, int level ) {
//  const double csq=89875517873681764.0;		// c^2 constant
  const double c= 299792458.0 ;
  complex<double> erg ;  
//  double h = nu_b-nu_a ;
  complex<double> unt ;
  complex<double> ob ;
  /* the integral in \nu is different for phi == 0 and phi != null
     for phi == 0 the integral is exact
     for phi != 0 the integral is an approximation 
     The origin of the two terms is the integral over phi */
  if (phi_a == 0)   // case phi ==0
    unt = null(nu_a,nu_b,alpha);
  else
    unt = low(phi_a,nu_a,nu_b,alpha,c) ;
  if (phi_b == 0)  // case phi == 0 
    ob = null(nu_a,nu_b,alpha) ;
  else 
    ob = hi(phi_b,nu_a,nu_b,alpha,c) ;
  erg = (ob-unt)/(nu_a-nu_b) ;
  return erg ;
}


/*!
  \brief Apply the Wiener Filter operator to a data vector to reconstruct a map
  
  \param data - vector containing complex data
  \param map - reconstructed map
*/
void wienerfilter::applyWF(cvec &data, cvec &map)
{
  if(data.size()==0) {
    cerr << "Wiener Filtering can not be performed " << endl ;
    throw "wienerfilter::applyWF data vector has size 0";
  }

  //****************************************
  if(WF.cols()==0 && WF.rows()==0)	// if WF matrix does not exist
  {
    cerr << "Wiener Filtering can not be performed " << endl ;
    throw "wienerfilter::applyWF WF matrix does not exist";
  }

  map=WF*data;		// reconstruct map from data
}



vector<complex<double> > wienerfilter::applyWF(vector<complex<double> > datas) {
  vector<complex<double> > map=WF*datas ;
  return map ;
}


vector<complex<double> > wienerfilter::applyR(vector<complex<double> > &map) {
  vector<complex<double> > data=WF*map ;
  return data ;
}


/*!
  \brief functions generates the Matrix for performing the wiener filtering
*/
void wienerfilter::generateWienerFiltering(int noNoise, int flatPrior, double signal_var, double signal_corr, double noise_var, vector<uint> gaps, double eps, uint QR )
{
//    cout << "doing WienerFiltering" << endl;	// debug
  setVariance_s(signal_var) ;
  setLambdaPhi(signal_corr) ;
  if (noNoise == 0 )   // don't create noise matrix if noNoise is active 
    createNoiseMatrix(noise_var);
  string type= "gaussian";
  if (signal_corr==0) type = "white" ;
  if (flatPrior == 0) // don't create signal covariance matrix for flatPrior
    createSMatrix(type) ;
  // Set up matrices
  createResponseMatrix(gaps,eps,QR );
  R.realValOut("real.csv") ;
  R.imagValOut("imag.csv") ;
  R.absOut("Response.mat");
  cout << "noNoise=" << noNoise << " flatPrior=" <<  flatPrior << endl ;
  // Perform Wiener Filtering computations
  if (noNoise == 0) {
    NI=N.inv();
  }
  if ((noNoise ==0) && (flatPrior == 0 )) {
    computeD();
  }
  else if (noNoise == 0 ) {
    computeD_NoSignalCov() ;
  }
  else {
    computeD_NoNoiseCov() ;
  }
  cout <<  " Propagator fertig " << endl ;
//   cout << "W I E N E R : " << D(0,0)<< " " << D(10,0) << " " << D(0,10) << " " << D(10,10) << " " << D(0,20) << " " << D(20,0) << endl ;
  D.absOut("Propagator.mat") ;
//   computeQ();
  if (noNoise == 0)
    computeW();
  else 
    computeW_noNoise() ;
//   cout << "W I E N E R : " << W(0,0)<< " " << W(10,0) << " " << W(0,10) << " " << W(10,10) << " " << W(0,20) << " " << W(20,0) << endl ;
//  computeM();
  WF = D*W ;
  WF.absOut("Wiener.mat");
  cout << "wiener filter finished " << endl ;
//   cout << "W I E N E R : " << WF(0,0)<< " " << WF(10,0) << " " << WF(0,10) << " " << WF(10,10) << " " << WF(0,20) << " " << WF(20,0) << endl ;
}

/*!
  \brief functions generates the Matrix for performing the wiener filtering
*/
void wienerfilter::generateWienerFiltering(int noNoise, double noise_var, vector<uint> gaps, cvec power, double eps, uint QR)
{
  cout << "calculate Singnal Matrix from Powerspectrum" << endl ;
  calcSFromPowerspec(power) ;
  // Set up matrices
  if (!R_ready) {
    cout << "create Response Matrix " << R_ready << endl ;
    createResponseMatrix(gaps,eps,QR );
    cout << "response generated" << endl ;
  }
  else {
    cout << "response matrix used" << endl ;
  }
  R.absOut("Response.mat");
  // Perform Wiener Filtering computations
  if (noNoise == 0) {
    createNoiseMatrix(noise_var);
    NI=N.inv();
    NI.absOut("invNoise.csv");
  }
  if (noNoise ==0) {
    computeD();
  }
  else {
    computeD_NoNoiseCov() ;
  }
  cout <<  " Propagator fertig " << endl ;
//   cout << "W I E N E R : " << D(0,0)<< " " << D(10,0) << " " << D(0,10) << " " << D(10,10) << " " << D(0,20) << " " << D(20,0) << endl ;
  D.absOut("Propagator.mat") ;
//   computeQ();
  if (noNoise == 0)
    computeW();
  else 
    computeW_noNoise() ;
//   cout << "W I E N E R : " << W(0,0)<< " " << W(10,0) << " " << W(0,10) << " " << W(10,10) << " " << W(0,20) << " " << W(20,0) << endl ;
//  computeM();
  WF = D*W ;
  WF.absOut("Wiener.mat");
//   cout << "W I E N E R : " << WF(0,0)<< " " << WF(10,0) << " " << WF(0,10) << " " << WF(10,10) << " " << WF(0,20) << " " << WF(20,0) << endl ;
}

void wienerfilter::generateWienerFiltering(int noNoise, double noise_var, vector<uint> gaps, cmat covMat, double eps, uint QR )
{
  S=covMat ;
  S.absOut("origSignal.mat") ;
  cout << "create Response Matrix " << endl ;
  createResponseMatrix(gaps,eps,QR );
  cout << "response generated" << endl ;
  R.absOut("Response.mat");
  // Perform Wiener Filtering computations
  if (noNoise == 0) {
    createNoiseMatrix(noise_var);
    NI=N.inv();
  }
  if (noNoise ==0) {
    computeD();
  }
  else {
    computeD_NoNoiseCov() ;
  }
  cout <<  " Propagator fertig " << endl ;
//   cout << "W I E N E R : " << D(0,0)<< " " << D(10,0) << " " << D(0,10) << " " << D(10,10) << " " << D(0,20) << " " << D(20,0) << endl ;
  D.absOut("Propagator.mat") ;
//   computeQ();
  if (noNoise == 0)
    computeW();
  else 
    computeW_noNoise() ;
//   cout << "W I E N E R : " << W(0,0)<< " " << W(10,0) << " " << W(0,10) << " " << W(10,10) << " " << W(0,20) << " " << W(20,0) << endl ;
//  computeM();
  WF = D*W ;
  WF.absOut("Wiener.mat");
//   cout << "W I E N E R : " << WF(0,0)<< " " << WF(10,0) << " " << WF(0,10) << " " << WF(10,10) << " " << WF(0,20) << " " << WF(20,0) << endl ;
}

/*! procedure calculates the covarinace matrix, which is invariant angainst translation
    by using the powerspectrum of the corrlation function.
    \param vector which represents the powerspectrum 
   */
void wienerfilter::calcSFromPowerspec(cvec power) {
    uint dim = power.size() ;
    cmat t(dim,dim) ;
    S=t ;	
    cvec result(dim) ;
    power.fft(result) ;
   
    /* Set the Entries of the S-Matrix to the values of the fft of 
       the powerspectrum. Hangle the Matrix like a correlation function
       which only depends on the difference of its argument
       s11=s22=s33 .. s42=s53=s64 */
    for(uint i = 0 ; i < dim ; i++ ) {
	for (uint j=0; j<dim; j++) {
	  S.set_peropdic(j,j+i,result[i]);
	}
    }
    S.absOut("power.csv") ;
}

/*! procdure to iterate the signal powerspectrum and the reconstructed signal.
    Relaying on 
    Torsten Ensslin, Mona Frommert 
    Reconstruction of signals with unknown spectrain information field theory 
    with parameter uncertainty
    arXiv 1002.2928 using Jeffery Prior and critical filter*/
void wienerfilter::iteratePowerSpectrum(cvec freqs, cvec faradays, cvec power0, cvec data, vector<uint> gaps, cvec epsilon, cvec delta,  double aimDist) {
  double curDist = 2.0*aimDist ;//  variable for the current distance 
  complex<double> m1(-1,0) ;
  cvec power = power0 ;
  /* loop for iterating the map and the signal powerspectrum */
  while (curDist > aimDist) {
    cvec powerOld = power ;     // store the old powerspectrum for the use in comparison to the new one
    cvec map(faradays.size()) ; // create a vector for the estimated signal (poralized emmision in Faraday space)
    calcSFromPowerspec(power) ; // create a signal covariance matrix from the powerspectrum using fft
    double noiseVar = data.Abs()*0.1 ; //create an estimation of the noise matrix
    generateWienerFiltering(0, noiseVar, gaps, power, 0.001,2) ;  // create a wiener filter for the signal and noise covariance matrix
    applyWF(data, map) ; // apply the filer to the data vector to get the new signal vector 
    cvec maptr(faradays.size()) ; 
    map.fft(maptr) ;  // generate fft from map to make powerspectrum iteration in fourierspace
    cmat mmt(maptr) ;  // mmt = tensorproduct of map and complex adjoint of map map*map^t
    cmat prop = D.fft() ;
    /* create a vector for the prefactor 1/(1/2+epsilon_i) */
    oneOverEps *mapFunk = new oneOverEps(epsilon,1.0,0.5,0.0) ;
    cvec epsi(mapFunk,epsilon.size()) ;
    delete mapFunk ;
    prop.multToDia(delta) ; // multiply the diagonal of the fft of the propagator with deltas
    cmat sum = prop+mmt ;
    sum.multToDia(epsi) ;
    /* the correction terms for the powerspectrum are pointwise in the diagonal of the matrix sum */
    sum.copyToVec(power) ;  // dopy the powerspectrum into an Vector 
    powerOld.add(power,m1) ;
    curDist = powerOld.Abs() ; // calculate the changing of the powerspectrum to check for convergence
  }  // end of the loop over different powerspectra
}


/*! procdure to iterate the signal powerspectrum and the reconstructed signal
    for a couple of data sets, which are assumed to share the same signal covariance matrix
    which can be represented by a powerspectrum.
    Relaying on 
    Torsten Ensslin, Mona Frommert 
    Reconstruction of signals with unknown spectrain information field theory 
    with parameter uncertainty
    arXiv 1002.2928 using Jeffery Prior and critical filter
    page 8 formula 48 
    \param power0 initial guess for the powerspectrum
    \param datas vector of datas for which the common powerspectrum is to be generated 
    \param gaps is the vector of the gaps in the frequency achs
    \param epsilon vector of epsilons which are parameter of the filter
    \param delta vector of parameters of the filter 
    \param aimDis stop condition, if the condition is reached, the powerspectrum is considered as found
    \return true if the iteration is regarded as converged else false 
*/
void wienerfilter::iteratePowerSpectra(cvec power0, vector<cvec> datas, vector<uint> gaps, cvec epsilon, cvec delta,  double aimDist, uint smoothSteps) {
  double curDist = 2.0*aimDist ;//  variable for the current distance 
  complex<double> m1(-1,0) ;
  complex<double> zero(0,0) ;  
  cvec power = power0 ;  
  uint dim = datas.size() ;  
  complex<double> fak(1.0/dim,0) ;
  uint ite = 0 ;
  char buffer [33];
  double noiseVar = datas[0].Abs()*0.1 ; //create an estimation of the noise matrix
  bool res = false ;  // result value 
  /* loop for iterating the map and the signal powerspectrum */
  while ((curDist > aimDist) && (ite<100)) {
    ite++ ;
    cvec powerOld = power ;     // store the old powerspectrum for the use in comparison to the new one
    cvec powerSum(power.size(),zero) ; // init the sum of the powerspectra to make the average of all lines of sight
    cvec map(power0.size()) ; // create a vector for the estimated signal (poralized emmision in Faraday space)
    calcSFromPowerspec(power) ; // create a signal covariance matrix from the powerspectrum using fft
    generateWienerFiltering(0, noiseVar, gaps, power, 0.001,2) ;  // create a wiener filter for the signal and noise covariance matrix
    for (uint i=0; i<dim; i++) { // loop over all lines of sight, which are assumed to share the signal power spectrum       
      cvec data = datas[i] ;
      if (i % 15 ==0 ) cout << "Process line " << i << endl ;
      applyWF(data, map) ; // apply the filer to the data vector to get the new signal vector 
      cvec maptr(power0.size()) ; 
      map.fft(maptr) ;  // generate fft from map to make powerspectrum iteration in fourierspace
      cmat mmt(maptr) ;  // mmt = tensorproduct of map and complex adjoint of map map*map^t
      cmat prop = D.fft() ;
      /* create a vector for the prefactor 1/(1/2+epsilon_i) */
      oneOverEps *mapFunk = new oneOverEps(epsilon,1.0,0.5,0.0) ;
      cvec epsi(mapFunk,epsilon.size()) ;
      delete mapFunk ;
      prop.multToDia(delta) ; // multiply the diagonal of the fft of the propagator with deltas
      cmat sum = prop+mmt ;
      sum.multToDia(epsi) ; // diagonale of the matrix sum contains the new powerspectrum
    /* the correction terms for the powerspectrum are pointwise in the diagonal of the matrix sum */
      sum.copyToVec(power) ;  // dopy the powerspectrum into an Vector       
      powerSum.add(power,fak) ;
    } // end of the loop over all lines of sight
    /* prepare a controle output */
    sprintf(buffer,"%d",ite);
    string index(buffer) ;
    string name1 = "one"+index+".csv" ;
    string name2 = "sum"+index+".csv" ;
    string name3 = "smooth"+index+".csv" ;
    power.absOut(name1) ;  // print an example for an individuel found Powerspectrum
    powerSum.absOut(name2) ;  // print the averaged powerspectrum 
    smooth(powerSum,smoothSteps) ;
    powerSum.absOut(name3) ;  // print the averaged powerspectrum 
    double curNorm = powerSum.Abs() ;
    curDist = powerOld.Abs(powerSum)/curNorm ; // calculate the changing of the powerspectrum to check for convergence
    power.data = powerSum.data ;
    cerr << "Iteration: " << ite << "  distance: " << curDist << "  Norm: " << curNorm <<  endl ;
  }
  /* check, if the procedure is converged, than calculate the signal covariance matrix once again, and 
     set the result vector to true */
  if (curDist < aimDist) {
    calcSFromPowerspec(power) ;
    generateWienerFiltering(0, noiseVar, gaps, power, 0.001,2) ;  // create a wiener filter for the signal and noise covariance matrix
    res = true ;
  }
}

/*! procedure to smooth the powerspectrum by some iterations of the difusionequation.
    \param power powerspectrum to be smoothed 
    \param num number of iterations performed for smoothing */
void wienerfilter::smooth(cvec &power, uint num) {
  uint dim = power.size() ;
  for (uint j=0; j<num; j++) {
    vector<complex<double> > vek = power.data ;
    for (uint i=0; i<dim; i++) {
      complex<double> rval ;
      complex<double> lval ;
      if(i==0) {
	rval = vek[1];
	lval = vek[dim-1] ;
      }
      else if (i==dim-1) {
	rval = vek[0] ;
	lval = vek[i-1] ;
      } 
      else {
	rval = vek[i+1] ;
	lval = vek[i-1] ;
      }
      power.set(i,0.25*(2.0*vek[i]+rval+lval)) ;
    }
  }
}

/*! procdure to iterate the signal powerspectrum and the reconstructed signal
    for a couple of data sets, which are assume	d to share the same signal covariance matrix
    which can be represented by a powerspectrum.
    Relaying on 
    Torsten Ensslin, Mona Frommert 
    Reconstruction of signals with unknown spectrain information field theory 
    with parameter uncertainty
    arXiv 1002.2928 using Jeffery Prior and critical filter
    page 8 formula 48 */
void wienerfilter::iteratePowerSpectraNaive(cvec power0, vector<cvec> datas, vector<uint> gaps, cvec epsilon, cvec delta,  double aimDist) {
  double curDist = 2.0*aimDist ;//  variable for the current distance 
  complex<double> m1(-1,0) ;
  complex<double> zero(0,0) ;  
  cvec power = power0 ;  
  uint dim = datas.size() ;  
  complex<double> fak(1.0/dim,0) ;
  uint ite = 0 ;
  char buffer [33];
  /* loop for iterating the map and the signal powerspectrum */
  while ((curDist > aimDist) && (ite<50)) {
    ite++ ;
    cvec powerOld = power ;     // store the old powerspectrum for the use in comparison to the new one
    cvec powerSum(power.size(),zero) ; // init the sum of the powerspectra to make the average of all lines of sight
    cvec powerSq(power.size(),zero) ;  // init the sum of the powerspectra squared to get the diagonal covariance matrix
    cvec map(power0.size()) ; // create a vector for the estimated signal (poralized emmision in Faraday space)
    calcSFromPowerspec(power) ; // create a signal covariance matrix from the powerspectrum using fft
    double noiseVar = datas[0].Abs()/datas[0].size() ; //create an estimation of the noise matrix
    generateWienerFiltering(0, noiseVar, gaps, power, 0.001,2) ;  // create a wiener filter for the signal and noise covariance matrix
    for (uint i=0; i<dim; i++) { // loop over all lines of sight, which are assumed to share the signal power spectrum       
      cvec data = datas[i] ;
      if (i % 15 ==0 ) cout << "Process line " << i << endl ;
      applyWF(data, map) ; // apply the filer to the data vector to get the new signal vector 
      cvec maptr(power0.size()) ; 
      map.fft(maptr) ;  // generate fft from map to make powerspectrum iteration in fourierspace
      powerSum.add(maptr,1.0) ;
      powerSq.addSQ(maptr,1.0) ;
    } // end of the loop over all lines of sight
    powerSq.valOut("Vorher.csv");
    powerSq.addSQ(powerSum,-fak) ;
    powerSq.valOut("Nachher.csv");
    power.add(powerSq,1.0) ;
    power.mult(0.5) ;
    sprintf(buffer,"%d",ite);
    string index(buffer) ;
    string name = "power"+index+".csv" ;
    power.absOut(name) ;
    double cNorm = power.Abs() ;
//     powerOld.add(power,m1) ;
    curDist = powerOld.Abs(power) ; // calculate the changing of the powerspectrum to check for convergence
    cerr << "Iteration: " << ite << "  distance: " << curDist << "   Norm: " << cNorm <<  endl ;
  }
}
/*! procdure to iterate the signal signal covariance matrix and the reconstructed signal
    for a couple of data sets, which are assumed to share the same signal covariance matrix.
    Relaying on 
    Torsten Ensslin, Mona Frommert 
    Reconstruction of signals with unknown spectrain information field theory 
    with parameter uncertainty
    arXiv 1002.2928 using Jeffery Prior and critical filter
    page 8 formula 48 */
void wienerfilter::iterateCovMatrix(vector<cvec> &datas, vector<uint> &gaps, double aimDist) {
  double curDist = 2.0*aimDist ;//  variable for the current distance 
  complex<double> m1(-1,0) ;
  complex<double> zero(0,0) ;
  cmat covMat = S ;//covMat0 ;
  cvec map(covMat.rows()) ; // create a vector for the estimated signal (poralized emmision in Faraday space)
  uint dim = datas.size() ;  
  complex<double> fak(1.0/dim,0) ;
  uint ite = 0 ;
  char buffer [33];	
  /* loop for iterating the map and the signal powerspectrum */
  while ((curDist > aimDist) && (ite<10)) {
    cmat sumMat(covMat.rows(),covMat.cols(	),zero) ;
    ite++ ;
    double noiseVar = datas[0].Abs()*0.1 ; //create an estimation of the noise matrix
    generateWienerFiltering(0, noiseVar, gaps, covMat, 0.001,2) ;  // create a wiener filter for the signal and noise covariance matrix
 	cout << "1. filter generated" << endl ;
    for (uint i=0; i<dim; i++) { // loop over all lines of sight, which are assumed to share the signal power spectrum       
      cvec data = datas[i] ;
//       cout << "Process line " << i << endl ;
      applyWF(data, map) ; // apply the filer to the data vector to get the new signal vector 
      sumMat.add(map,map) ;
    } // end of the loop over all lines of sight
    sumMat.mult(fak) ;
    covMat = sumMat+D ;
    sprintf(buffer,"%d",ite);
    string index(buffer) ;
    string name = "test"+index+".csv" ;
    covMat.absOut(name.c_str());
  }
cout << "iteration ist fertig " << endl ;
}

/*! procdure to iterate the signal signal covariance matrix and the reconstructed signal
    for a couple of data sets, which are assumed to share the same signal covariance matrix.
    Relaying on 
    Torsten Ensslin, Mona Frommert 
    Reconstruction of signals with unknown spectrain information field theory 
    with parameter uncertainty
    arXiv 1002.2928 using Jeffery Prior and critical filter
    page 8 formula 48 */
void wienerfilter::iterateMatrixNaive(vector<cvec> &datas, vector<uint> &gaps, double aimDist) {
  double curDist = 2.0*aimDist ;//  variable for the current distance 
  complex<double> m1(-1,0) ;
  complex<double> zero(0,0) ;
  cmat covMat = S ;//covMat0 ;
  cvec map(covMat.rows()) ; // create a vector for the estimated signal (poralized emmision in Faraday space)
  uint dim = datas.size() ;  
  complex<double> fak(1.0/dim,0) ;
  uint ite = 0 ;
  char buffer [33];
  /* loop for iterating the map and the signal powerspectrum */
  while ((curDist > aimDist) && (ite<30)) {
    cmat sumMat(covMat.rows(),covMat.cols(),zero) ;
    cvec sumVec(covMat.rows(),zero) ;
    ite++ ;
    double noiseVar = datas[0].Abs()*0.1 ; //create an estimation of the noise matrix
    generateWienerFiltering(0, noiseVar, gaps, covMat, 0.001,2) ;  // create a wiener filter for the signal and noise covariance matrix
 	cout << "1. filter generated" << endl ;
    for (uint i=0; i<dim; i++) { // loop over all lines of sight, which are assumed to share the signal power spectrum       
      cvec data = datas[i] ;
//       cout << "Process line " << i << endl ;
      applyWF(data, map) ; // apply the filer to the data vector to get the new signal vector 
      sumMat.add(map,map) ;
      sumVec.add(map,1.0) ;
    } // end of the loop over all lines of sight
    
    sumMat.mult(fak) ;
    sumVec.mult(-1.0*fak) ;
    sumMat.add(sumVec,sumVec) ;
    covMat = covMat+sumMat;
    covMat.mult(0.5) ;
    sprintf(buffer,"%d",ite);
    string index(buffer) ;
    string nameM = "Mat"+index+".csv" ;
    string nameV = "Vec"+index+".csv" ;
    
    covMat.absOut(nameM.c_str());
    sumVec.absOut(nameV.c_str());
  }
cout << "iteration ist fertig " << endl ;
}

void wienerfilter::iterateForAll(rmCube &cubeIn, rmCube &cubeOut, double sigVar, double corLenght, double noiseVar,vector<uint> gaps, uint smoothSteps, double powerEps) {
  setVariance_s(sigVar) ;
  complex<double> zero(0,0) ;
  complex<double> one(1,0) ;
  setLambdaPhi(corLenght) ;
  createSMatrix("gaussian") ;   // generate the covariance matrix from the parameter values
  cmat Power = S.fft() ;        // generate the powerspectrum from the signal matrix
  cvec power(Power) ; 		// generate the powerspectrum vector from the matrix 
  uint dim = power.size() ;
  cvec epsilon(dim,zero) ;
  cvec delta(dim,1) ;
  uint nx = cubeIn.getXSize() ; ;
//  uint ny = cubeIn.getYSize() ;
  vector<cvec> datas ;
  for (uint ij=0 ; ij<1; ij++) 
  {
    cout << "ij " << ij << endl  ;
    for (uint ii=0; ii<nx; ii++) 
    {
      vector<complex<double> > P_lambda ; 
      cubeIn.getLineOfSight(ii,ij,P_lambda) ;
      cvec data(P_lambda) ;
      datas.push_back(data) ;
    }
  }
   cout << endl ;
   iteratePowerSpectra( power, datas, gaps, epsilon, delta, powerEps, smoothSteps) ;
  cout << "Zahl der Eintraege: " << datas.size() << "  Dimension: "<< power.size() << endl ;
}

void wienerfilter::iterateMatForAll(rmCube &cubeIn, rmCube &cubeOut, double sigVar, double corLenght, double noiseVar,vector<uint> gaps) {
  setVariance_s(sigVar) ;
  complex<double> zero(0,0) ;
  complex<double> one(1,0) ;
  setLambdaPhi(corLenght) ;
  createSMatrix("gaussian") ;   // generate the covariance matrix from the parameter values
  uint nx = cubeIn.getXSize() ;
//  uint ny = cubeIn.getYSize() ;
  vector<cvec> datas ;
  for (uint ij=0 ; ij<1; ij++) 
  {
    cout << "ij " << ij << endl  ;
    for (uint ii=0; ii<nx; ii++) 
    {
      vector<complex<double> > P_lambda ;
      cubeIn.getLineOfSight(ii,ij,P_lambda) ;
      cvec data(P_lambda) ;
      datas.push_back(data) ;
    }
  }
   cout << endl ;
   iterateMatrixNaive( datas, gaps, 1.0) ;
  cout << "Zahl der Eintraege: " << datas.size() <<  endl ;
}
/*!
	\brief Create a LOFAR (or simulated) Noise matrix
	
	The noise within each lambda squared channel is assumed to be independent
	of the noise of any other channel. Therefore the quadratic n x n matrix
	is a diagonal matrix. Initially the noise is assumed to be the rms noise 
	(taken from the input image) for all channels.
	
	\param noise - the noise in the data vector d
*/
void wienerfilter::createNoiseMatrix(double noise)
{
  if(noise < 0)	{	// only if positive rmsnoise is given
	cerr << "can not create noise matrix " << endl ;
	 throw "wienerfilter::createNoiseMatrix rmsnoise<0";
  }
  else
  {
	  if(frequencies.size()>0)		// only if frequencies vector has length > 0
	  {
		  uint size=frequencies.size(); // get size of data vector d
		 
		  N.set_size(size, size);			// set size of Noise matrix
		  N.zeros();					// initialize matrix to 0-matrix

		  for(uint i=0; i<size; i++)			// loop over rows (quadratic matrix)
		  {
		     // set rms^2 value to all diagonal elements (because it's the Noise Covariance matrix)
		     N.set(i, i, noise);
		  }
	  }
	  else {
	    cerr << "can not create noise matrix " << endl ;
	     throw "wienerfilter::createNoiseMatrix d vector has length 0";	
	  }
  }
}


/*!
	\brief Create a LOFAR (or simulated) Noise matrix
	
	The noise within each lambda squared channel is assumed to be independent
	of the noise of any other channel. Therefore the quadratic n x n matrix
	is a diagonal matrix. This overloaded function takes a vector<double>
	rmsnoiseperchan which gives the rms noise for each frequency channel
	individually.
		
	\param noiseperchan - the noise in the data vector d in each frequency channel
*/
void wienerfilter::createNoiseMatrix(vector<double> &noiseperchan)
{
  if(noiseperchan.size()==0) {			// only if positive rmsnoise is given
	cerr << "can not create noise matrix " << endl ;
    throw "wienerfilter::createNoiseMatrix noiseperchan has zero length";
  }
  else if(noiseperchan.size()!=(unsigned int) d.size()) {
    cerr << "can not create noise matrix " << endl ;
    throw "wienerfilter::createNoiseMatrix noiseperchan differs in length from data vector d";
  }
  else
  {
	  if(d.length()>0)				// only if d vector has length > 0
	  {
		  int size=d.length();				// get size of data vector d
		 
		  N.set_size(size, size);			// set size of Noise matrix
		  N.zeros();					// initialize matrix to 0-matrix

		  for(int i=0; i<size; i++)			// loop over rows (quadratic matrix)
		  {
		     N.set(i, i, noiseperchan[i]*noiseperchan[i]);			// set rms value to all diagonal elements
		  }
	  }
	  else
	     throw "wienerfilter::createNoiseMatrix d vector has length 0";	
  }
}


/*!
	\brief Create the Response matrix
	
	The Response matrix R is the Fourier Transform operator transforming image
	lambda squared wavelengths into Faraday space phi. It has m x n dimensions where m is the
	number of lambda squared channels and n the number of Faraday depths probed for.
*/
void wienerfilter::createResponseMatrix(vector<uint> gaps, double eps, uint singVal)
{
  const double csq=89875517873681764.0;		// c^2 constant

  complex<double> element=0;			// Matrix element in Vandermonde matrix for DFT
  complex<double> rowfactor=0;			// common factor for Matrix elements in the same row
//   vector<double> frequenciessquared(frequencies.size());	// vector to hold pre-computed frequencies squared
//  double Faradayexponent=0;			// Faraday rotation exponent
  //***********************************************************************
  // Parameter integrity checks
  //***********************************************************************
  if(frequencies.size()==0) {
    cerr << "can not create response matrix " << endl ;
     throw "wienerfilter::createResponseMatrix data vector d has size 0";
  }
  if(faradaydepths.size()==0) {
	cerr << "can not create response matrix " << endl ;
	  throw "wienerfilter::createResponseMatrix faradaydepths has invalid size";
  }
 
  // Create Response matrix with faradaydepths rows and frequencies columns 
  uint Nfaradaydepths=faradaydepths.size();
  uint Nfrequencies=frequencies.size(); 
  R.set_size(Nfrequencies, Nfaradaydepths);		// set size of response matrix

  // Check if spectral dependence parameters are valid
  if(nu_0==0)
     throw "wienerfilter::createResponseMatrix nu_0 is 0";  
//   if(alpha==0)	// check if alpha is valid
//      throw "wienerfilter::createResponseMatrix alpha is 0";


  // ************************************************************************
  // Calculate individual matrix elements
  // ************************************************************************
    
  // compute freq^2 vector first
/*  for(unsigned int i=0; i<frequencies.size(); i++)  
    frequenciessquared[i]=frequencies[i]*frequencies[i];*/
//  cout << "createResponseMatrix(): R.rows()=" << R.rows() << "\tR.cols()=" << R.cols() << endl;			// debug
//  cout << "alpha=" << alpha << endl;			// debug
//  cout << "epsilonZero = " << epsilon_0 << endl;	// debug
  
//  R.zeros();
  complex<double> I(0,1) ;
  complex<double> preFakt = epsilon_0*pow(nu_0, alpha)/(2.0*I*csq) ;
  uint curIndex = 0 ;
  cout << "start to create the Responsematrix" << endl ;
  for(uint k=0; k<Nfrequencies; k++) { // loop over all rows of R (i.e. frequencies) 
      // Convert Jy to SI unit
      // spectral dependence factor and bandpass common to all elements in the same row	
        rowfactor=preFakt;//*bandpass[k] ;	
	double nu_a ; // start of the integration interval for vu integration 
	double nu_b ; // end of the integration interval for vu integration 
	if (k==gaps[curIndex])  nu_a = frequencies[gaps[curIndex]] ;
	else nu_a = 0.5*(frequencies[k-1]+frequencies[k]) ;
	if (k==gaps[curIndex+1]-1) {
            nu_b = frequencies[gaps[curIndex+1]-1] ;
            if (curIndex != gaps.size()) curIndex++ ;
	}
	else nu_b= 0.5*(frequencies[k]+frequencies[k+1]) ;
//         cout << k << "\t" << rowfactor << "\t" << nu_a << "\t" << nu_b << "\t" << eps<< endl ;
        for(uint l=0; l<R.cols(); l++) { // loop over all columns of R (i.e. Faraday depths)	
	  double phi_a ;  // start of the integration interval for phi integration 
	  double phi_b ;  // end of the integration interval for phi integration 
	  // calculate the interval in faradaydepths
	  if (l==0)  phi_a = faradaydepths[0] ;          
	  else phi_a = 0.5*(faradaydepths[l-1]+faradaydepths[l]) ;
	  if (l==Nfaradaydepths-1) phi_b = faradaydepths[Nfaradaydepths-1] ;
	  else phi_b= 0.5*(faradaydepths[l]+faradaydepths[l+1]) ;
// 	  complex<double> integr = integral(nu_a,nu_b,eps,alpha,phi_a,phi_b,1);
	  complex<double> integr = integral_approx(nu_a,nu_b,eps,alpha,phi_a,phi_b,1);
	  element = rowfactor*integr ;
// 	  if (k==l) cout<< k << " " << l << " " << phi_a << " " << phi_b << " " << nu_a << " " << nu_b << " " << rowfactor << " " << integr << " " << element<< " " <<epsilon_0 << " " << alpha  << endl ;
// 	  cout << "k:" << k << "  l:" << l << "  num:" << integr << "  approx:" <<vergleich  << "  diff:" << (integr+vergleich)/integr << endl ;
	  R.set(k, l, element);	// write element to correct Matrix position	
        }
  }
  R_ready=true ;  
  cmat RR ;
  cout << "Responsematrix created" << endl ;
  if (singVal!=0) {
   cout << "start the singular value decomposition" << endl ;
   if (R.rows() >= R.cols())
     RR = R ;
   else 
     RR = R.H() ;
   RR.svd() ;
  }
//   cmat sing(R.singVals) ;
//   sing.absOut("sing.dat") ;
}
/*!
	\brief Create the Response matrix
	
	The Response matrix R is the Fourier Transform operator transforming image
	lambda squared wavelengths into Faraday space phi. It has m x n dimensions where m is the
	number of lambda squared channels and n the number of Faraday depths probed for.
*/
void wienerfilter::createResponseMatrix(vector<double> intervals, double eps, uint singVal)
{
  const double csq=89875517873681764.0;		// c^2 constant

  complex<double> element=0;			// Matrix element in Vandermonde matrix for DFT
  complex<double> rowfactor=0;			// common factor for Matrix elements in the same row
//   vector<double> frequenciessquared(frequencies.size());	// vector to hold pre-computed frequencies squared
//  double Faradayexponent=0;			// Faraday rotation exponent
  //***********************************************************************
  // Parameter integrity checks
  //***********************************************************************
  if(frequencies.size()==0) {
    cerr << "can not create response matrix " << endl ;
     throw "wienerfilter::createResponseMatrix data vector d has size 0";
  }
  if(faradaydepths.size()==0) {
	cerr << "can not create response matrix " << endl ;
	  throw "wienerfilter::createResponseMatrix faradaydepths has invalid size";
  }
 
  // Create Response matrix with faradaydepths rows and frequencies columns 
  cout << "start creating the response matrix" << endl ;
  uint Nfaradaydepths=faradaydepths.size();
  uint Nfrequencies=frequencies.size(); 
  R.set_size(Nfrequencies, Nfaradaydepths);		// set size of response matrix

  // Check if spectral dependence parameters are valid
  if(nu_0==0)
     throw "wienerfilter::createResponseMatrix nu_0 is 0";  

  // ************************************************************************
  // Calculate individual matrix elements
  // ************************************************************************
    
  complex<double> I(0,1) ;
  complex<double> preFakt = epsilon_0*pow(nu_0, alpha)/(2.0*I*csq) ;
//  uint curIndex = 0 ;
  for(uint k=0; k<Nfrequencies; k++) { // loop over all rows of R (i.e. frequencies) 
      // Convert Jy to SI unit
      // spectral dependence factor and bandpass common to all elements in the same row	
        rowfactor=preFakt;//*bandpass[k] ;	
	double nu_a ; // start of the integration interval for vu integration 
	double nu_b ; // end of the integration interval for vu integration 	
	nu_a = frequencies[k]-0.5*intervals[k] ;
	nu_b = frequencies[k]+0.5*intervals[k] ;

        for(uint l=0; l<R.cols(); l++) { // loop over all columns of R (i.e. Faraday depths)	
	  double phi_a ;  // start of the integration interval for phi integration 
	  double phi_b ;  // end of the integration interval for phi integration 
	  // calculate the interval in faradaydepths
	  if (l==0)  phi_a = faradaydepths[0] ;          
	  else phi_a = 0.5*(faradaydepths[l-1]+faradaydepths[l]) ;
	  if (l==Nfaradaydepths-1) phi_b = faradaydepths[Nfaradaydepths-1] ;
	  else phi_b= 0.5*(faradaydepths[l]+faradaydepths[l+1]) ;

	  complex<double> integr = integral_approx(nu_a,nu_b,eps,alpha,phi_a,phi_b,1);
	  element = rowfactor*integr ;

	  R.set(k, l, element);	// write element to correct Matrix position	
        }
  }
  R_ready=true ;
  cout << "creating response matrix finished" << endl ; 
}


/*!
	\brief Create the Response matrix with integral description
	
	The Response matrix R is the integral form Fourier Transform operator transforming image
	lambda squared wavelengths into Faraday space phi. It has m x n dimensions where m is the
	number of lambda squared channels and n the number of Faraday depths probed for.
*/
void wienerfilter::createResponseMatrixIntegral()
{
  const double csq=89875517873681764.0;		// c^2 constant

  complex<double> element=0;			// Matrix element in Vandermonde matrix for DFT
  complex<double> rowfactor=0;			// common factor for Matrix elements in the same row
  vector<double> frequenciessquared(frequencies.size());	// vector to hold pre-computed frequencies squared
  double Faradayexponent=0;			// Faraday rotation exponent

  vector<double> frequencies_low(frequencies.size());
  vector<double> frequencies_high(frequencies.size());

  //***********************************************************************
  // Parameter integrity checks
  //***********************************************************************
  if(d.size()==0) {
	cerr << "can not create response matrix " << endl ;
     throw "wienerfilter::createResponseMatrixIntegral data vector d has size 0";
  }
  if(s.size()==0) {
	cerr << "can not create response matrix " << endl ;
     throw "wienerfilter::createResponseMatrixIntegral signal vector s has size 0";
  }
  if(frequencies.size()!=(unsigned int) d.size()) {
	cerr << "can not create response matrix " << endl ;
     throw "wienerfilter::createResponseMatrixIntegral frequencies has invalid size";
  }
  if(delta_frequencies.size()!=(unsigned int) d.size()) {
	cerr << "can not create response matrix " << endl ;
     throw "wienerfilter::createResponseMatrixIntegral delta frequencies has invalid size";
  }
  if(faradaydepths.size()!=(unsigned int) s.size()) {
	cerr << "can not create response matrix " << endl ;
	  throw "wienerfilter::createResponseMatrixIntegral faradaydepths has invalid size";
  }
 
  // Create Response matrix with faradaydepths rows and frequencies columns 
  int Nfaradaydepths=s.length();
  int Nfrequencies=d.length(); 

  R.set_size(Nfrequencies, Nfaradaydepths);		// set size of response matrix

  // Check if spectral dependence parameters are valid
  if(nu_0==0) {
	cerr << "can not create response matrix " << endl ;
     throw "wienerfilter::createResponseMatrixIntegral nu_0 is 0";  
  }
  if(alpha==0)  {	// check if alpha is valid
	cerr << "can not create response matrix " << endl ;
     throw "wienerfilter::createResponseMatrixIntegral alpha is 0";
  }


  // ************************************************************************
  // Calculate individual matrix elements
  // ************************************************************************
    
  if(bandwidths.size()==0)
  {
 	for(unsigned int i=0; i<frequencies.size(); i++)  
  	{
    	frequenciessquared[i]=frequencies[i]*frequencies[i];	  	// Compute lower and upper frequencies// compute freq^2 vector first
    	frequencies_low[i]=frequencies[i]-delta_frequencies[i]/2.0;	 // "Bandwidth" is given by delta frequencies
    	frequencies_low[i]=frequencies[i]+delta_frequencies[i]/2.0;	 // "Bandwidth" is given by delta frequencies
  	}
  }
  else
  {
 	for(unsigned int i=0; i<frequencies.size(); i++)  
  	{
    	frequenciessquared[i]=frequencies[i]*frequencies[i];	 // Compute lower and upper frequencies// compute freq^2 vector first
    	frequencies_low[i]=frequencies[i]-bandwidths[i]/2.0;	 // "Bandwidth" is given by delta frequencies
    	frequencies_low[i]=frequencies[i]+bandwidths[i]/2.0;	 // "Bandwidth" is given by delta frequencies
  	}  
  }
 
 
  for(uint k=0; k<R.rows(); k++)		// loop over all rows of R (i.e. frequencies)
  {  	 
      // Convert Jy to SI unit
      // spectral dependence factor and bandpass common to all elements in the same row
      rowfactor=complex<double> ( bandpass[k] * epsilon_0 * pow(frequencies[k]/nu_0, -alpha) * delta_frequencies[k] );

     cout << "HIER " << k << "\t" << rowfactor << "\t";

      for(uint l=0; l<R.cols(); l++)	// loop over all columns of R (i.e. Faraday depths)
      {
	  Faradayexponent=-2.0 * (csq/frequenciessquared[k]) * faradaydepths[l];

	  element = complex<double>( cos(Faradayexponent), sin(Faradayexponent));
	  element = rowfactor*element*delta_faradaydepths[l];

// 	  cout << "\t" << l << "\t" << delta_faradaydepths[l] << endl;

	  R.set(k, l, element);	// write element to correct Matrix position
      }
  }
}


/*!
	\brief Compute variance of s (which is computed from s=R^{-1}d, taken as dispersion_s)
*/
void wienerfilter::computeVariance_s()
{
	complex<double> sum(0,0);		// temporary sum (for avg and dispersion_s)
	complex<double> cavg(0,0);		// complex average
	cmat Rinverse;				// inverse Response matrix
	cvec ds(s.size());			// d->s tranformed data
	double difference=0;			// difference between element and cavg
	int size=s.size();			// size of d=No. of elements, needed for average

	//-----------------------------------------------------
	if(d.size()==0)					// if d has no elements
	{
		cerr << "can not create signal covariance matrix " << endl ;
		throw "wienerfilter::computeVariance_s d has size 0";
	}
	if(R.rows()!=d.size() || R.cols()!=s.size())
	{
		cerr << "can not create signal covariance matrix" << endl ;
		throw "wienerfilter::computeVariance_s R has incorrect size";
	}
	
	//------------------------------------------------------		
	// Transform d -> s using R^{-1}
// 	cout << "ds.size() = " << ds.size() << endl;
	ds=R.inv()*(d);			// really need d-n and therefore a loop then!
// 	ds=d;

	for(int i=0; i<size; i++)		// Compute complex average
	{
	  sum=sum+ds[i];			// sum over all elements
	}
	cavg=(1.0/size)*sum;		// that size isnt 0 is checked for above
	cout << "cavg = " << cavg << endl;
	// compute dispersion_s of d vector
	// < | s-s_avg |^2 >
	for(int i=0; i<size; i++)			// complex dispersion_s
	{
	    difference=pow(abs(ds[i]-cavg), 2);		// compute over d->s transformed data
	    variance_s = variance_s + difference;		// deviation^2 of each element from the complex average
	//	cout << "variance_s[" << i << "] =" << variance_s << endl;	// debug
	}
	variance_s=variance_s/(size-1);						// -1 for unbiased deviation

// 	variance_s= 80; // debug
// 	cout << endl << "variance_s =" << variance_s << endl;	// debug
}


/*!
	\brief Guess an initial estimate of the Signal matrix S
	
	\param &type - type of initial spectral dependence "gaussian", "white" or "powerlaw"
*/
void wienerfilter::createSMatrix(const string &type)
{
	double divisor=1;						// multiplication factor in Gaussian exponential
	double dist=0;							// distance petween phi_i and phi_j
	
	// Make data integrity checks
	if(faradaydepths.size()==0)	{					// if s has zero length
	  cerr << "can not create signal covariance matrix " << endl ;
	  throw "wienerfilter::computeSMatrix s vector has 0 length";
	}

	// ******************************************************
	S.set_size(faradaydepths.size(), faradaydepths.size());		// set size of S matrix to dimensions of d-vector

	if(variance_s <= 0)			// if standard deviation is smaller or equal zero
	{
	        cout << "variance = " << variance_s << endl ;
		throw "wienerfilter::computeSMatrix dispersion_s <= 0";
	}
// 	if(lambda_phi<=0)					// if lambda_phi has an invalid value < 0
// 	{
// 	        cout << "lambda Phie = " << lambda_phi << endl ;
// 		throw "wienerfilter::computeSMatrix lambda_phi < 0";
// 	}


	if(type=="gaussian")					// if initial guess is a Gaussian decay:
	{
		vector<double> boundarycondition(5);		// vector with boundary conditions
		vector<double>::iterator pos;			// position index of minimum element
// 		double lambda_phi_corrected=0.0;		// corrected lambda_phi, we don't want to destroy our class attribute lambda_phi!
		mat tMat(S.rows(),S.cols());
		// Compute constant factors: 0.25*lambda_phi^2
//		divisor=0.25*lambda_phi*lambda_phi; // old: without peridic boundary condition

		// Create S matrix guess with Gaussian decay formula
		//
		// S(i,j)=dispersion_s^2*exp(0.5*abs(phi_i-phi_j)^2/(2*lambda_phi^2))
		//
		for(uint row=0; row<S.rows(); row++)			// loop over rows of S
		{
			for(uint col=0; col<S.cols(); col++)		// loop over columns of S
			{
//				dist=abs(row-col);			// compute absolute distance between phi_i and phi_j 
				dist=abs(faradaydepths[row]-faradaydepths[col]);
				dist*=dist;				// square absolute distance				
				// Periodic boundary condition for S matrix:
				// lambda=lambda_phi*min(faradaydepths[col]/lambda_phi, faradaydepths[row]/lambda_phi, 1)
			
				// write values into vector
// 				boundarycondition[0]=faradaydepths[row]/lambda_phi;
// 				boundarycondition[1]=faradaydepths[col]/lambda_phi; 
// 				boundarycondition[2]=(faradaydepths[S.size()-1]-faradaydepths[row])/lambda_phi;
// 				boundarycondition[3]=(faradaydepths[S.size()-1]-faradaydepths[col])/lambda_phi; 
// 				boundarycondition[4]=1.0;
			
// 					cout << "fd[" << row << "]=" << faradaydepths[row] << "\tfd[" << col << "]=" << faradaydepths[col] << "\tlambda_phi=" << lambda_phi << endl;
// 				cout << "b[0]=" << boundarycondition[0] << "\tb[1]=" << boundarycondition[1] << "\tb[2]=" << boundarycondition[2];
			
				// find minimum in vector
// 				pos = min_element (boundarycondition.begin(), boundarycondition.end());
// 				lambda_phi_corrected=*pos;	// set to corrected lambda_phi
				
// 				if(lambda_phi_corrected != lambda_phi)
// 				  cout << "\tlambda_phi_corrected (" << row << "," << col << ") = " << lambda_phi_corrected << endl;
				
				divisor=0.25*lambda_phi*lambda_phi;	// compute divisor
				
				
// 				divisor=0.25*lambda_phi*lambda_phi

				S.set(row, col, variance_s*exp(-dist/divisor) );	// Write element into S
				tMat.set(row,col,variance_s*exp(-dist/divisor) );
			}
		}
		mat tr(tMat.rows(),tMat.cols()) ;
// 		bool sucess = tMat.JacobiIteration(tr,0.00000000001,2000000) ;
//   		if (sucess) {
// 		  vec eigenVals(tMat.rows()) ;
// 		  tMat.copyToVec(eigenVals) ;
// 		  cerr << eigenVals.data << endl ; 
// 		}
		// Write Gaussian decay formula into Sformula string
		Sformula="dispersion_s^2*exp(0.5*abs(phi_i-phi_j)^2/(2*lambda_phi^2))";
		Stype="gaussian";
	}
	else if(type=="white")
	{
		// S(i,j)=dispersion_s*Kronecker(i,j) diagonal!
		for(uint row=0; row<S.rows(); row++)			// loop over rows of S
		{
			for(uint col=0; col<S.cols(); col++)		// loop over columns of S
			{
/*				if(col==row)						// Diagonal matrix!
					S.set(row, col, variance_s);	// set to variance_s
				else
					S.set(row, col, 0);				// elsewhere set it to 0*/

				if(col==row)						// Diagonal matrix!
					S.set(row, col, variance_s);	// set to variance_s
				else
					S.set(row, col, 0);				// elsewhere set it to 0

//				cout << "S(" << row << "," << col << ") = " << S.get(row, col) << endl;		// debug
			}
		}			
		
		// Write White noise formula into Sformula string
		Sformula="dispersion_s*diagS(i,j)?";
		Stype="whitesignal";
	}
	else if(type=="powerlaw")
	{
		// S(i,j)=dispersion_s*exp(-abs(phi_i-phi_j)^2/lambda_phi)?
		for(uint row=0; row<S.rows(); row++)				// loop over rows of S
		{
			for(uint col=0; col<S.cols(); col++)			// loop over columns of S
			{
// 					dist=abs(row-col);	// compute absolute distance between phi_i and phi_j 
					dist=abs(faradaydepths[row]-faradaydepths[col]);
					dist*=dist;									// square absolute distance				

					S.set(row, col, complex<double>(variance_s*exp(-dist/lambda_phi)));
			}
		}			
	
		// Write White noise formula into Sformula string	
		Sformula="dispersion_s*exp(-abs(phi_i-phi_j)^2/lambda_phi)";
		Stype="powerlaw";
	}
	else if(type=="deconvolution")		// S matrix for deconvolution is infinite
	{
		// S(i,j)=inf
		for(uint row=0; row<S.rows(); row++)
		{
			for(uint col=0; col<S.cols(); col++)						// set each element of S to:
			{
//			    S.set(row, col, numeric_limits<double>::max( ));	// maximum double value available on this platform
			    S.set(row, col, complex<double>(100000,100000));		// 100000 is sufficiently high
			}
		}
		
		Sformula="infinity";
		Stype="infinite";
	}
	else
	{
		cerr << "can not create signal covariance matrix" << endl ;
		throw "wienerfilter::computeSMatrix unknown spectral dependency function";
	}
}


//**********************************
//
// Algorithm functions
//
//**********************************

/*!
	\brief Compute information source vector j = R^(dagger)N^(inverse)
*/
void wienerfilter::computej()								
{
	#ifdef debug_
	cout << "d.length = " << d.length() << "  s.length = " << s.length() << endl;
	cout << "R.rows = " << R.rows() << "  R.cols = " << R.cols() << endl;
	#endif

	// check that R and N have the correct size
	if(R.rows()==0 || R.cols()==0)
		throw "wienerfilter::computej R has size 0";
	if( !(R.rows()==d.length() && R.cols()==s.length()))						
		throw "wienerfilter::computej R has invalid dimensions";
	else if( !( N.rows()==d.length() && N.cols()==d.length()))
		throw "wienerfilter::computej N has invalid dimensions";
	else if(d.length()==0)
		throw "wienerfilter::computej data vector d is empty";
	else
	{
		if(W.rows()!=0 && W.cols()!=0)		// If we already computed W
		{
			#ifdef debug_
			cout << "compute: j = W*d" << endl;			// debug
			#endif
			j=W*d;							// compute j
		}
		else		// otherwise compute it from individual matrices
		{
			#ifdef debug_
			cout << "compute: j=R.H()*(inv(N)*d)" << endl;		// debug
			#endif
			cmat RH = R.H() ;
			mat NI = N.inv() ;
			cvec Nd = NI*d ;
			j=RH*(Nd);					// compute j
		}
	}
}


/*!
	\brief Computes the Propagator D,  D=R^(dagger)N^(inverse)R=j*R
*/
void wienerfilter::computeD()								
{
	if(R.rows()==0 || R.cols()==0) {
		cerr << "wienerfilter::computeD R has size 0" << endl ,
		throw "wienerfilter::computeD R has size 0";
	}
	else if(S.cols()==0 || S.rows()==0) {
		cerr << "wienerfilter::computeD S has invalid size"<< endl ;
		throw "wienerfilter::computeD S has invalid size";
	}
	else
	{

// 		#ifdef debug_
		cout << "compute: D = inv(S) + R.H()*inv(N)*R" << endl;
// 		#endif
		S.absOut("signal.mat");
		cmat SI = S.inv() ;
		cmat RH = R.H() ;
		mat NI=N.inv();
		N.absOut("noise.mat");
		cmat RHN = RH*NI ;
		cmat RHNR = RHN*R ;
                RHNR.absOut("RHNIR.mat") ;		
		cmat zw = SI + RHNR;		// D^-1=S^-1+R^{dagger}*inv(N)*R
		D=zw.inv();					// compute inverse
		cmat Eins = SI*S ;
    		SI.absOut("sigInv.mat");
	}
}

/*!
	\brief Computes the Propagator D,  D=R^(dagger)N^(inverse)R=j*R
	for the case, that the inverse of the Signalcovariance matrix = 0
*/
void wienerfilter::computeD_NoSignalCov()								
{
        if(R.rows()==0 || R.cols()==0) {
		cerr <<"wienerfilter::computeD R has size 0" << endl ;
		throw "wienerfilter::computeD R has size 0";
	}
	else if(S.cols()==0 || S.rows()==0) {
		cerr <<"wienerfilter::computeD S has invalid size" << endl;
		throw "wienerfilter::computeD S has invalid size";
	}
	else
	{
		mat NI=N.inv();
// 		#ifdef debug_
		cout << "compute: D = inv(S) + R.H()*inv(N)*R" << endl;
// 		#endif
		cmat RH = R.H() ;
		
		cmat RHN = RH*NI ;
		cmat RHNR = RHN*R ;		
		D = RHNR;		// D^-1=R^{dagger}*inv(N)*R
		
		D=D.inv();			// compute inverse
		computeMapError();		// Store diagonal of D in maperror
	}
}

/*!
	\brief Computes the Propagator D,  D=R^(dagger)N^(inverse)R=j*R
	for the case, that the inverse of the Noisecovariance matrix = 0
        For this case we use the Gauss-Markow-Regularisation, which can 
        be regarded as a limit of Wiener Filterung for no noise and unknown 
        powerspectrum of the signal.
*/
void wienerfilter::computeD_NoNoiseCov()  // is currently called								
{
       if(R.rows()==0 || R.cols()==0) {
		cerr << "wienerfilter::computeD R has size 0" << endl ;
		throw "wienerfilter::computeD R has size 0";
		}
	else
	{
// 		#ifdef debug_
		cout << "compute: D = R.H()*R" << endl;
		cmat RH = R.H() ;
		cout << "Rh calculated " << RH.rows() << " " << RH.cols() <<  endl ;
		cmat RHR = RH*R ;
		cout << "RhR calculated " << endl ;
// 		clock_t t3 = clock() ;
// 		cout << "gls: " << t2-t1 << "  std: " << t3-t2 << endl ;
		cmat RHRI = RHR.inv() ;
		cout << "RhR^(-1) calculated " << endl ;
		D=RHRI;//*RH ;					// compute inverse
// 		computeMapError();				// Store diagonal of D in maperror
	}
}


/*!
	\brief Computer intermediate matrix W
*/
void wienerfilter::computeW()
{
	cout << "Aufruf der Routine computeW" << endl ;
	// Consistency checks
	if(R.cols()==0) {
		cerr << "wienerfilter::computeW R has wrong columns" << endl ;
		throw "wienerfilter::computeW R has wrong columns";
	}
	if(R.rows()==0) {
		cerr << "wienerfilter::computeW R has wrong number of rows" << endl ;
		throw "wienerfilter::computeW R has wrong number of rows";
	}
	if(N.rows()==0) {
		cerr << "wienerfilter::computeW N has wrong number of rows" << endl ;
		throw "wienerfilter::computeW N has wrong number of rows";
	 }
	if(N.rows()==0) {
		cerr << "wienerfilter::computeW N is not quadratic" << endl ;
		throw "wienerfilter::computeW N is not quadratic";
	}
	cmat RH = R.H() ;
	W=RH*NI;		// compute intermediate product W=R^{dagger}*inv(N)
}

void wienerfilter::computeW_noNoise() {

  W = R.H();
}

/*!
	\brief Computer intermediate matrix M
*/
void wienerfilter::computeM()
{
	// Consistency checks TODO
	
	M=W*R;
}


/*!
	\brief Compute the final Wiener Filter matrix which can then be applied to data vectors
*/
void wienerfilter::computeWF()
{
	// Do consistency checks
	if(S.cols()!=S.rows() || S.cols()!=s.size())
	{
		cerr << "wienerfilter::computeWF S has invalid size" << endl ;
		throw "wienerfilter::computeWF S has invalid size";
	}
	if(M.cols()!=0 && W.cols()!=0)		// if M and W were computed
	{
		cmat SI = S.inv() ;
		cmat SM = SI+M ;
		WF=SM*W;
	}
	else if(M.cols()!=0 && W.cols()==0)	// if we only have M
	{
		cmat SI = S.inv() ;
		cmat SM = SI+M ;
		cmat SIM = SM.inv();
		WF=SIM*W;				// correct?
	}
	else				// ... otherwise compute it from individual matrices
	{
		cmat SI = S.inv() ;
		cmat RH = R.H() ;
		mat NI = N.inv() ;
		cmat RNI = RH*NI ;
		cmat RNR = RNI*R ;
		cmat SPR = SI + RNR ;
		cmat SPRR = SPR*RH ;
		WF=SPRR*NI;
	}
}



//******************************************************************
//
// Delete Matrices that are not needed anymore
//
//******************************************************************

/*!
	\brief Delete Noise matrix N
*/
void wienerfilter::deleteN()
{
	// TODO: Free memory taken by N
}


/*!
	\brief Delete Response matrix R 
*/
void wienerfilter::deleteR()
{
	// TODO: Free memory taken by R
}


//*******************************************************************
//
// Member access functions (D and Q are results and handeled below)
//
//*******************************************************************

/*!
	\brief Get lambda nu parameter for S matrix
*/
double wienerfilter::getLambdaPhi()
{
	return lambda_phi;
}


/*!
	\brief Set lambda phi parameter for S matrix, coherence length

	\param lambdaPhi - lambda_phi parameter for spectral dependence of s
*/
void wienerfilter::setLambdaPhi(double lambdaphi)
{
// 	if(lambdaphi)
	  this->lambda_phi=lambdaphi;
// 	else {
// 		cerr << "wienerfilter::setLambdaPhi lambda_phi is 0" << endl ;
// 		throw "wienerfilter::setLambdaPhi lambda_phi is 0";
// 	}
}


/*!
	\brief Get lambdas vector used in data
*/
vector<double> wienerfilter::getLambdas()
{
	return lambdas;
}


/*!
	\brief Set lambdas vector
	
	\param lambdas - lambda vector used in data
*/
void wienerfilter::setLambdas(vector<double> &lambdas)
{
	this->lambdas=lambdas;
}


/*!
	\brief Get lambda squareds used in data
*/
vector<double> wienerfilter::getLambdaSquareds()
{
	return lambdasquareds;
}


/*!
	\brief Set lambda squareds used in data
	
	\param lambdasquareds - lambda squareds used in the data
*/
void wienerfilter::setLambdaSquareds(vector<double> &lambdasquareds)
{
	this->lambdasquareds=lambdasquareds;
}


/*!
	\brief Get delta lambda squareds used in data
*/
vector<double> wienerfilter::getDeltaLambdaSquareds()
{
	return delta_lambdasquareds;
}


/*!
	\brief Set delta lambda squareds used in data
	
	\param delta_lambda_squareds - vector containing the 
*/
void wienerfilter::setDeltaLambdaSquareds(vector<double> &delta_lambdasquareds)
{
	if(delta_lambdasquareds.size()==0) {
		cerr << "wienerfilter::setDeltaLambdaSquareds delta_lambda_squareds has size 0" << endl ;
		throw "wienerfilter::setDeltaLambdaSquareds delta_lambda_squareds has size 0";
	}
	else
		this->delta_lambdasquareds=delta_lambdasquareds;
}


/*!
	\brief Get frequencies vector wth observed frequencies
	
	\return frequencies - frequencies vector with observed frequencies
*/
vector<double> wienerfilter::getFrequencies()
{
	return frequencies;
}


/*!
	\brief Set frequencies vector with to observed frequencies
	
	\param freqs - vector containing observed frequencies
*/
void wienerfilter::setFrequencies(vector<double> &freqs)
{
	if(freqs.size()==0) {
	   cerr << "wienerfilter::setFrequencies freqs vector is 0" << endl ;
	   throw "wienerfilter::setFrequencies freqs vector is 0";
	}
	else
	   frequencies=freqs;
}


/*!
	\brief Get delta frequencies distances between observed frequencies

	\return delta_frequencies - vector with delta distances between observed frequencies
*/
vector<double> wienerfilter::getDeltaFrequencies()
{
	return delta_frequencies;
}


/*!
	\brief Set delta frequencies distances between observed frequencies

	\param deltafreqs - vector containing delta frequencies distances between observed frequencies
*/
void wienerfilter::setDeltaFrequencies(vector<double> &deltafreqs)
{
	if(deltafreqs.size()==0) {
		cerr << "wienerfilter::setDeltaFrequencies deltafreqs vector has size 0" << endl ;
		throw "wienerfilter::setDeltaFrequencies deltafreqs vector has size 0";
	}
	else
		delta_frequencies=deltafreqs;			// set internal delta frequencies to given vector
}


/*!
	\brief Get Faraday depths to be probed for
*/
vector<double> wienerfilter::getFaradayDepths()
{
	return faradaydepths;
}


/*!
	\brief Set Faraday depths to be probed for
*/
void wienerfilter::setFaradayDepths(vector<double> &faradaydepths)
{
	if(faradaydepths.size()==0)  {
		cerr << "wienerfilter::setFaradayDepths faradaydepths has size 0"<< endl ;
		throw "wienerfilter::setFaradayDepths faradaydepths has size 0";
	}
	else
		this->faradaydepths=faradaydepths;
}


/*!
	\brief Get bandpass for each frequency
	
	\return bandpass - vector containing the bandpass setting for each frequency
*/
vector<complex<double> > wienerfilter::getBandpass()
{
	return bandpass;
}

 
/*!
	\brief Set bandpass for each frequency
	
	\param bandpass - vector containing the bandpass setting for each frequency
*/
void wienerfilter::setBandpass(vector<complex<double> > &bandpass)
{
	if(bandpass.size()==0) {
		cerr << "wienerfilter::setBandpass bandpass vector has size 0" << endl ;
		throw "wienerfilter::setBandpass bandpass vector has size 0";
	}
	else
		this->bandpass=bandpass;
}

void wienerfilter::createUnitBandpass() {
  uint nfreqs = frequencies.size() ;
  bandpass.resize(nfreqs);
  for (uint i=0; i<nfreqs;i++) {
    bandpass[i]=complex<double>(1,0);	
  }
}

/*!
	\brief Return dispersion_s

	\return dispersion_s - the dispersion of the signal in Faraday space phi
*/
double wienerfilter::getVariance_s()
{
	return variance_s;
}


/*!
	\brief Return epsilon zero
	
	\return epsilon_0 - epsilon zero emission at nu zero frequency
*/
double wienerfilter::getEpsilonZero()						
{
	return epsilon_0;
}


/*!
	\brief Set epsilon zero value of spectral dependence
*/
void wienerfilter::setEpsilonZero(double epsilonzero)
{
	if(epsilonzero==0) {
		cerr << "wienerfilter::setEpsilonZero epsilonzero is 0" << endl ;
		throw "wienerfilter::setEpsilonZero epsilonzero is 0";
	}
	else if(epsilonzero< 0) {
		cerr << "wienerfilter::setEpsilonZero epsilonzero is 0" << endl ;
		throw "wienerfilter::setEpsilonZero epsilonzero is 0";
	}
	else
		epsilon_0=epsilonzero;
}


/*!
  \brief Set variance_s - variance of signal s
*/
void wienerfilter::setVariance_s(double variance_s)
{
//   if(variance_s <= 0) {
// 	cerr << "wienerfilter::setVariance_s variance_s <= 0" << endl ;
//     throw "wienerfilter::setVariance_s variance_s <= 0";
//   }

  this->variance_s=variance_s;
}


/*!
	\brief Return nu zero
	
	\return nu_0 - nu zero frequency for which epsilon zero emission is defined
*/
double wienerfilter::getNuZero()						
{
	return nu_0;
}


/*!
	\brief Set nu zero value of spectral dependence
*/
void wienerfilter::setNuZero(double nu)
{
	if(nu==0) {
	   cerr << "wienerfilter::setNuZero nu is 0" << endl ;
	   throw "wienerfilter::setNuZero nu is 0";
	}
	else if(nu < 0) { 
	   cerr << "wienerfilter::setNuZero nu < 0" << endl ; 
	   throw "wienerfilter::setNuZero nu < 0";
	}
	else
	   nu_0=nu;
}
  

/*!
	\brief Get alpha power law exponent of spectral dependence
	
	\return alpha - alpha power law exponent of spectral dependence
*/
double wienerfilter::getAlpha()
{
	return alpha;
}
 
 
/*!
	\brief Set alpha power law exponent of spectral dependence 
*/
void wienerfilter::setAlpha(double alpha)
{
	if(alpha==0) {
		cerr << "wienerfilter::setAlpha alpha is 0" << endl ;
		throw "wienerfilter::setAlpha alpha is 0";
	}
//	else if(alpha < 0)
// 		throw "wienerfilter::setAlpha alpha < 0";
	else
		this->alpha=alpha;
}


/*!
	\brief Return the Signal covariance matrix S
	
	\return cmat - signal covariance matrix S
*/
cmat wienerfilter::getSignalMatrix()
{	
	return S;
}


/*!
	\brief Return the Noise matrix N
	
	\return cmat - noise matrix
*/
mat wienerfilter::getNoiseMatrix()
{	
	return N;
}


/*!
	\brief Return the Response  matrix R
	
	\return cmat - instrumental Response matrix
*/
cmat wienerfilter::getResponseMatrix()
{	
	return R;
}

					
/*!
	\brief Return the Information source vector j
	
	\return cmat - noise matrix
*/
cvec wienerfilter::getInformationSourceVector()
{	
	return j;
}					


//*************************************************
//
// Results of Wiener Filter
//
//************************************************

/*!
	\brief Compute the reconstructed map m
*/
void wienerfilter::reconstructm()
{
	if(m.size()==0) {
		cerr <<"wienerfilter::reconstructm m has size 0" << endl;
		throw "wienerfilter::reconstructm m has size 0";
	}
	if(m.size()!=s.size()) {
		cerr << "wienerfilter::reconstructm m has wrong size" << endl ;
		throw "wienerfilter::reconstructm m has wrong size";
	}

	if(WF.rows()!=0 && WF.cols()!=0)		// if we have the Wiener Filter matrix
	{
// 	    cout << "We have WF matrix." << endl;	// debug
	    m = WF*d;				
	}
	else
	{
// 	    cout << "m = D*j" << endl;		// debug
	    m = D*j;					// m=D*j(d)
	}
}


/*!
	\brief Compute the map error, diagonal elements of D

	\return cmat - map error / diagonal of propagator matrix D
*/
cvec wienerfilter::computeMapError()
{
	if(D.rows()==0 || D.cols()==0) {
		cerr <<"wienerfilter::getMapError D has size 0" << endl ;
		throw "wienerfilter::getMapError D has size 0";
	}
	if(D.rows()!=D.cols()) {
		cerr << "wienerfilter::getMapError D is not quadratic" << endl ;
		throw "wienerfilter::getMapError D is not quadratic";
	}
		
	for(uint k=0; k < D.rows(); k++)
	{
	    // get sqrt of diagonal elements of D (since D is also covariance matrix of propagator matrix D)
	    maperror[k]=sqrt(D.get(k,k));
	}

	return maperror;
}



/*!
	\brief Return the map error D, diagonal elements of D

	\return rmerror - map error
*/
/*
cvec wienerfilter::getMapError()
{
   return maperror;
}
*/


/*!
	\brief Return the map error D, diagonal elements of D

	\return rmerror - map error
*/
vector<complex<double> > wienerfilter::getMapError()
{
  unsigned int size=D.size();
  vector<complex<double> > rmerror(size);

  if(size==0) {
	cerr << "wienerfilter::getMapError D has size 0" << endl ;
    throw "wienerfilter::getMapError D has size 0";
  }

  for(unsigned int i=0; i < size; i++)
  {
    rmerror[i]=maperror[i];
  }

  return rmerror;
}


/*!
	\brief Return the propagator matrix D

	\return cmat - propagator matrix D
*/
cmat wienerfilter::getD()
{
	return D;
}

/*!
	\brief Return the instrumental fidelity matrix Q

	\return cmat - instrumental fidelity Q
*/
cmat wienerfilter::getInstrumentFidelity()
{
	return Q;
}

/*!
	\brief Return the Wiener Filter matrix WF
	
	\return cmat - Wiener Filter matrix WF
*/
cmat wienerfilter::getWienerFilter()
{
	return WF;
}


/*!
	\brief Return the data vector d
*/
cvec wienerfilter::getDataVector()
{
	return d;
}


/*!
	\brief Set the data vector cvec d to values given in vector

	\param data - STL data vector
*/



/*!
 	\brief Get the limit of maximum iterations
	
	\return long - limit of maximum iterations
*/
unsigned long wienerfilter::getMaxIterations()
{
	return maxiterations;
}


/*!
 	\brief Set the limit of maximum iterations
*/	
void wienerfilter::setMaxIterations(unsigned long max)
{
	if(max)								// only if greater than zero
		this->maxiterations=max;
	else
		throw "wienerfilter::setMaxIterions max <= 0";
}


/*!
	\brief Get the number of iterations that already have been computed
*/
unsigned long wienerfilter::getNumberOfIterations()
{
	return number_of_iterations;
}


/*!
	\brief Get the formula (string) that was used to initially the signal 
	covariance matrix S
	
	\return string - formula expression used for spectral dependence
*/
string wienerfilter::getSformula()
{
 	return Sformula;
}


/*!
	\brief Get the type of spectral dependence that was used to initially guess the signal 
	covariance matrix S
	
	\return string - gaussian, whitenoise or powerlaw
*/
string wienerfilter::getStype()
{
	return Stype;
}


/*!
	\brief Get RMS limit threshold

	\return double - rms threshold in Jy
*/
double wienerfilter::getRMSthreshold()
{
	return rmsthreshold;
}


/*!
	\brief Set RMS limit threshold
*/
void wienerfilter::setRMSthreshold(double rms)
{
	if(rms)
		rmsthreshold=rms;
	else
		throw "wienerfilter::setRMSthreshold rms=0";
}


/*!
	\brief Read configuration from a file
	
	\param filename - name of config file in ASCII format
*/


/*!
	\brief Write configuration to a file
	
	\param filename - name of config file in ASCII format
*/
void wienerfilter::writeConfigFile(const string &filename)
{
  if(filename.length()==0)
     throw "wienerfilter::writeConfigFile invalid filename";	
  
  // File contains list of Matrix filenames in ITPP format?
  ofstream outfile(const_cast<const char *>(filename.c_str()), ofstream::out);

  if(outfile.fail())
  {
    throw "wienerFilter::writeConfigFile failed to open file";
  }

  // Write out Matrix file names
  if(Sfilename.length())
  	 outfile << "S=" << Sfilename << endl;			
  if(Rfilename.length())
    outfile << "R=" << Rfilename << endl;
  if(Nfilename.length())
    outfile << "N=" << Nfilename << endl;
  if(Dfilename.length())
    outfile << "D=" << Dfilename << endl;
  if(Qfilename.length())
    outfile << "Q=" << Qfilename << endl;

  // Write out S initial guess formula
  if(Sformula.length())
    outfile << "Sformula=" << Sformula << endl;

  // Write out lambda_phi
  if(lambda_phi)
     outfile << "lambda_phi=" << lambda_phi << endl;

  // Write out maximum number of iterations
  if(maxiterations)
    outfile << "MaxIterations=" << maxiterations << endl;

  // Write out rmsthreshold
  if(rmsthreshold)
    outfile << "rmsthreshold=" << rmsthreshold << endl;
	
  outfile.flush();			// flush output file	
  outfile.close();			// close output file
}



//**********************************************************************
//
// ASCII write functions (useful if plotting will be done with GNUplot)
//
//**********************************************************************

/*!
  \brief Write out a real vector in ASCII format
  
  \param v - real vector to write out
  \param filename - filename of ASCII text file to write
*/
void wienerfilter::writeASCII(vector<double> v, const string &filename)
{
  unsigned int length=v.size();
  ofstream outfile(const_cast<const char*>(filename.c_str()), ofstream::out);

  // Check input parameters
  if(v.size()==0)
    throw "wienerfilter::writeASCII vector v has size 0";
  if(filename=="")  
    throw "wienerfilter::writeASCII no filename given";
    
  //-----------------------------------------------------
  outfile << "#value" << endl;				// comment line
  for(unsigned int i=0; i < length; i++)
  {
    outfile << v[i] << endl;				// write out vector element
  }
}


/*!
  \brief Write out a real ITPP vec vector in ASCII format
  
  \param v - real vector to write out
  \param filename - filename of ASCII text file to write
*/
void wienerfilter::writeASCII(vec v, const string &filename)
{
  unsigned int length=v.size();
  ofstream outfile(const_cast<const char*>(filename.c_str()), ofstream::out);

  // Check input parameters
  if(v.size()==0)
    throw "wienerfilter::writeASCII vector v has size 0";
  if(filename=="")  
    throw "wienerfilter::writeASCII no filename given";
    
  //-----------------------------------------------------
  outfile << "#value" << endl;				// comment line
  for(unsigned int i=0; i < length; i++)
  {
    outfile << v[i] << endl;				// write out vector element
  }
}


/*!
  \brief Write out a real STL vector and a complex STL vector in ASCII format
  
  \param v1 - real STL vector to write out
  \param v2 - real STL vector to write out
  \param filename - filename of ASCII text file to write
*/
void wienerfilter:: writeASCII(vector<double> v1, vector<double> v2, const std::string &filename)
{
  unsigned int length=v1.size();
  ofstream outfile(const_cast<const char*>(filename.c_str()), ofstream::out);

  // Check input parameters
  if(v1.size()==0)
    throw "wienerfilter::writeASCII vector v1 has size 0";
  if(v2.size()==0)
    throw "wienerfilter::writeASCII vector v2 has size 0";  
  if(v1.size()!=v2.size())
    throw "wienerfilter::writeASCII vector v1 and v2 differ in size";
  if(filename=="")  
    throw "wienerfilter::writeASCII no filename given";
    
  //-----------------------------------------------------
  outfile << "#value" << endl;				// comment line
  for(unsigned int i=0; i < length; i++)
  {
    outfile << v1[i] << "\t" << v2[i];
    outfile << endl;				// write out vector element
  }
}

/*!
  \brief Write out a real STL vector and a complex STL vector in ASCII format
  
  \param v - real STL vector to write out
  \param cv - complex STL vector to write out
  \param filename - filename of ASCII text file to write
*/
void wienerfilter:: writeASCII(vector<double> v, vector<complex<double> > cv, const std::string &filename)
{
  unsigned int length=v.size();
  ofstream outfile(const_cast<const char*>(filename.c_str()), ofstream::out);

  // Check input parameters
  if(v.size()==0)
    throw "wienerfilter::writeASCII vector v has size 0";
  if(filename=="")  
    throw "wienerfilter::writeASCII no filename given";
    
  //-----------------------------------------------------
  outfile << "#value" << endl;				// comment line
  for(unsigned int i=0; i < length; i++)
  {
    outfile << v[i] << "\t" << cv[i].real() << "\t" << cv[i].imag();
    outfile << endl;				// write out vector element
  }
}


/*!
  \brief Write out a complex vector in ASCII format
  
  \param v - complex vector to write out
  \param filename - filename of ASCII text file to write
*/
void wienerfilter::writeASCII(cvec v, const string &filename)
{
  unsigned int length=v.size();
  ofstream outfile(const_cast<const char*>(filename.c_str()), ofstream::out);

  // Check input parameters
  if(v.size()==0)
    throw "wienerfilter::writeASCII vector v has size 0";
  if(filename=="")  
    throw "wienerfilter::writeASCII no filename given";
    
  //-----------------------------------------------------
  outfile << "#index\treal" << "\t" << "imag" << endl;			// comment line
  for(unsigned int i=0; i < length; i++)
  {
    outfile << i << "\t" << v[i].real() << "\t" << v[i].imag() << endl;	// write out real and imaginary part of vector
  }
}


/*!
  \brief Write out a coordinate and a real vector in ASCII format
  
  \param coord - real coordinate STL vector
  \param v - complex vector to write out
  \param filename - filename of ASCII text file to write
*/
void wienerfilter::writeASCII(vector<double> coord, cvec v, const string &filename)
{
  unsigned int length=v.size();
  ofstream outfile(const_cast<const char*>(filename.c_str()), ofstream::out);

  // Check input parameters
  if(coord.size()==0)
    throw "wienerfilter::writeASCII vector coord has size 0";
  if(v.size()==0)
    throw "wienerfilter::writeASCII vector v has size 0";
  if(coord.size() != (unsigned int) v.size())
    throw "wienerfilter::writeASCII coord vector and data vector differ in size";
  if(filename=="")  
    throw "wienerfilter::writeASCII no filename given";
    
  //-----------------------------------------------------
  outfile << "#value\treal\t\timag" << endl;				// comment line
  for(unsigned int i=0; i < length; i++)
  {
    outfile << coord[i] << "\t";			// write out coord vector element and a tab
    outfile << v[i].real() << "\t" << v[i].imag() << endl;				// write out vector element
  }
}


/*!
  \brief Write out a coordinate and a real vector in ASCII format
  
  \param coord - coordinate vector
  \param v - real vector to write out
  \param filename - filename of ASCII text file to write
*/
void wienerfilter::writeASCII(vec coord, vec v, const string &filename)
{
  unsigned int length=v.size();
  ofstream outfile(const_cast<const char*>(filename.c_str()), ofstream::out);

  // Check input parameters
  if(coord.size()==0)
    throw "wienerfilter::writeASCII vector coord has size 0";
  if(v.size()==0)
    throw "wienerfilter::writeASCII vector v has size 0";
  if(coord.size()!=v.size())
    throw "wienerfilter::writeASCII";
  if(filename=="")  
    throw "wienerfilter::writeASCII no filename given";
    
  //-----------------------------------------------------
  outfile << "#value" << endl;				// comment line
  for(unsigned int i=0; i < length; i++)
  {
    outfile << coord[i] << "\t";			// write out coord vector element and a tab
    outfile << v[i] << endl;				// write out vector element
  }
}


/*!
  \brief Write out a coordinate and a complex vector in ASCII format
  
  \param coord - coordinate vector
  \param v - complex vector to write out
  \param filename - filename of ASCII text file to write
*/
void wienerfilter::writeASCII(vec coord, cvec v, const string &filename)
{
  unsigned int length=v.size();
  ofstream outfile(const_cast<const char*>(filename.c_str()), ofstream::out);

  // Check input parameters
  if(v.size()==0)
    throw "wienerfilter::writeASCII vector v has size 0";
  if(filename=="")  
    throw "wienerfilter::writeASCII no filename given";
    
  //-----------------------------------------------------
  outfile << "#index\treal" << "\t" << "imag" << endl;			// comment line
  for(unsigned int i=0; i < length; i++)
  {
    outfile << coord[i] << "\t" << v[i].real(); 	// write out coord and real part of vector
    outfile << "\t" << v[i].imag() << endl;		// write out imaginary part of vector
  }
}


/*!
  \brief Write out a complex matrix in ASCII format
  
  \param M - complex matrix to write out
  \param filename - filename of ASCII text file to write
*/
void wienerfilter::writeASCII(cmat M, const string &filename)
{
  unsigned int rows=M.rows(), cols=M.cols();
  ofstream outfile(const_cast<const char*>(filename.c_str()), ofstream::out);

  //-----------------------------------------------------
  // Check input parameters
  if(rows==0 || cols==0)
    throw "wienerfilter::writeASCII Matrix M has size 0";
  if(filename=="")  
    throw "wienerfilter::writeASCII no filename given";

  //-----------------------------------------------------
  for(unsigned int i=0; i < rows; i++)				// loop over rows
  {
	 for(unsigned int j=0; j < cols; j++)			// loop over columns
	 {
	    outfile << M(i,j).real() << "\t" << M(i,j).imag() << "\t";	// write out real and imaginary part of vector
	 }
	 outfile << endl;			// at end of each row add a newline
  }

}


/*!
  \brief Write out a real matrix in ASCII format
  
  \param M - real matrix to write out
  \param filename - filename of ASCII text file to write
*/
void wienerfilter::writeASCII(mat M, const string &filename)
{
  unsigned int rows=M.rows(), cols=M.cols();
  ofstream outfile(const_cast<const char*>(filename.c_str()), ofstream::out);

  //-----------------------------------------------------
  // Check input parameters
  if(rows==0 || cols==0)
    throw "wienerfilter::writeASCII Matrix M has size 0";
  if(filename=="")  
    throw "wienerfilter::writeASCII no filename given";

  //-----------------------------------------------------
  for(unsigned int i=0; i < rows; i++)										// loop over rows
  {
	 for(unsigned int j=0; j < cols; j++)			// loop over columns
	 {
	    outfile << M(i,j) << "\t";	// write out real and imaginary part of vector
	 }
	 outfile << endl;		// at end of each row add a newline
  }

}


/*!
  \brief Write out the real or imaginary part of a complex matrix
  
  \param M - complex matrix
  \param part - string defining "real" or "imaginary" part to write out
  \param filename - name of ASCII file to write to
*/
void wienerfilter::writeASCII(cmat M, const std::string &part, const std::string &filename)
{
  unsigned int rows=M.rows(), cols=M.cols();
  ofstream outfile(const_cast<const char*>(filename.c_str()), ofstream::out);

  //-----------------------------------------------------
  // Check input parameters
  if(rows==0 || cols==0)
    throw "wienerfilter::writeASCII Matrix M has size 0";
  if(filename=="")  
    throw "wienerfilter::writeASCII no filename given";
  if(part!="real" && part!="imaginary")
    throw "wienerfilter::writeASCII wrong part identifier";

  //-----------------------------------------------------
  if(part=="real")
  {
    for(unsigned int i=0; i < rows; i++)	// loop over rows
    {
	 for(unsigned int j=0; j < cols; j++)	// loop over columns
	 {
	    outfile << M(i,j).real() << "\t";	// write out real and imaginary part of vector
	 }
	 outfile << endl;			// at end of each row add a newline
    }
  }
  else
  {
    for(unsigned int i=0; i < rows; i++)	// loop over rows
    {
      for(unsigned int j=0; j < cols; j++)	// loop over columns
      {
	outfile << M(i,j).imag() << "\t";	// write out real and imaginary part of vector
      }
      outfile << endl;				// at end of each row add a newline
    }  
  }
}


/*!
	\brief Save all matrices to their .it files
*/
void wienerfilter::saveAllMatrices()
{
	// TODO: High-level functio to save away all Matrices

}


// Data I/O functions to convert to STL vector<double> and vector<complex<double> >

/*!
	\brief Get data from STL complex vector
*/
void wienerfilter::getData(vector<complex<double> > &cmplx)
{
	if(cmplx.size()==0)
		throw "wienerfilter::getData cmplx vector has size 0";
	
	d.set_size(cmplx.size());		// resize d
	
	for(unsigned int i=0; i<cmplx.size(); i++)
	{
		d[i]=cmplx[i];
	}
}


/*!
	\brief Get data from real and imaginary STL vectors and copy to internal cvec data vector

	\param &real - vector containing the real part
	\param &imag - vector containing the imaginary part
*/
void wienerfilter::getData(vector<double> &real, vector<double> &imag)
{
	if(real.size()==0)
		throw "wienerfilter::getData real vector has length 0";
	if(imag.size()==0)
	   throw "wienerfilter::getData imaginary vector has length 0";
	if(real.size()!=imag.size())
		throw "wienerfilter:getData re and im vector differ in length";
	
	cvec tempcvec;						// temporary cvec vector needed for conversion
	int size=real.size();			// get size from vector parameters
	
	
	d.set_size(size);					// set size initially to size of real vector
	for(int i=0; i<size; i++)
	{
		// This must be possible to be implemented more efficiently!
//		tempcvec=to_cvec(real[i], imag[i]);		
//		d.ins(i, tempcvec);			// append temporary vector to d
		d[i] = complex<double>(real[i],imag[i]);
	}
}	


/*!
  \brief Read in a real signal from a file
  
  \param vector - vector to read into (reads only real part)
  \param filename - name of file to read from
*/
void wienerfilter::readSignalFromFile(const std::string &filename)
{
  double value=0;
  vector<double> signal;
  ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	

  if(infile.fail())
  {
     throw "wienerfilter::readSignalFromFile failed to open file";
  }	
  
  signal.clear();
  while(infile.good())
  {
    infile >> value;
    signal.push_back (value);
  }
  
  // Copy STL C++ vectors into ITPP vectors in class
  for(unsigned int i=0; i<signal.size(); i++)
    this->s[i].real()=signal[i];
  
  infile.close();
}


/*!
	\brief Export signal to STL complex vector
	
	\param &cmplx - STL complex double vector
*/
void wienerfilter::exportSignal(vector<complex<double> > &cmplx)
{
	if(s.size()==0)
		throw "wienerfilter::exportSignal s has length 0";
	
	for(uint i=0; i<s.size(); i++)
	{
		cmplx[i]=s[i];
	}
}


/*!
	\brief Export signal to real and imaginary STL vectors

	\param &re - vector with real part of signal
	\param &im - vector with imaginary part of signal
*/
void wienerfilter::exportSignal(vector<double> &re, vector<double> &im)
{
	cvec sconjugate(s.size());			// conjugate of s vector
	
	for(uint i=0; i<s.length(); i++)
	{
		// TODO: This doesn't work
//		re[i]=static_cast<double>( 0.5*(s[i]-sconjugate[i]) );
//		im[i]=static_cast<double>( 0.5*(s[i]+sconjugate[i]) );
	}
}



/*!
	\brief Read lambda squareds and delta lambda squareds from a text file
	
	\param filename - name of text file
*/
void wienerfilter::readLambdaSqAndDeltaLambdaSqFromFile(const std::string &filename)
{
  ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);		// file with lambda squareds
	
  // temporary variables
  double lambdasq=0;				// temporary variable for lambda squared
  double deltalambdasq=0;			// temporary variable for delta lambda squared
  vector<double> lambdaSqs;
  vector<double> deltaLambdaSqs;

  if(infile.fail())
  {
     throw "wienerfilter::readLambdaSqAndDeltaLambdaSqFromFile failed to open file";
  }	

  while(infile.good())
  {
	  infile >> lambdasq >> deltalambdasq;
	  lambdaSqs.push_back (lambdasq);
	  deltaLambdaSqs.push_back (deltalambdasq);
  }
  
  // Copy STL C++ vectors into ITPP vectors in class
  lambdasquareds=lambdaSqs;
  delta_lambdasquareds=deltaLambdaSqs;
  
  infile.close();
}


/*!
	\brief Read lambda squareds and complex data vector from a text file
	
	\param filename - name of text file
*/
void wienerfilter::readSimDataFromFile(const std::string &filename)
{
	ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);		// file with lambda squareds
	
	double lambdasq=0;		// temporary variable for lambda sqaured per line
	double real=0;				// temporary variable for real part per line
	double imag=0;				// temporary variable for imaginary part per line
	complex<double> intensity;					// temporary complex intensity
	
	// Temporary vectors
	vector<complex<double> > intensities;
	cvec tempcvec;

   if(infile.fail())
   {
      throw "wienerfilter::readSimDataFromFile failed to open file";
   }
	uint i=0;
	while(infile.good())
	{
		infile >> lambdasq >> real >> imag;
		lambdasquareds.push_back(lambdasq);
		d[i] = complex<double>(real,imag) ;
//		tempcvec=to_cvec(real, imag);
	}
	
	// Write vectors to wienerfilter class
//	d.ins(d.size(), tempcvec);
	
	infile.close();
}


/*!
	\brief Read lambda squareds, delta lambda squareds and complex data vector from a text file
	
	\param filename - name of text file
*/
void wienerfilter::read4SimDataFromFile(const std::string &filename)
{
	ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);		// file with lambda squareds
	
	double lambdasq=0;		// temporary variable for lambda sqaured per line
   	double deltalambdasq=0;	// temporary variable for delta lambda squared per line
	double real=0;				// temporary variable for real part per line
	double imag=0;				// temporary variable for imaginary part per line
	complex<double> intensity;					// temporary complex intensity
	unsigned int i=0;			// loop variable
	
	// Temporary vectors
	vector<complex<double> > intensities;
	cvec tempcvec;

   	if(infile.fail())
   	{
      throw "wienerfilter::read4SimDataFromFile failed to open file";
   	}
	
	while(infile.good())
	{
		infile >> lambdasq >> deltalambdasq >> real >> imag;

	   // Write complex vector to wienerfilter class data vector d (no easier way found than this)
	//	tempcvec=to_cvec(real, imag);				

		if(lambdasquareds.size() > i)		// if there is size left in lambdasquareds and delta_lambda_squareds vectors
		{
			lambdasquareds[i]=lambdasq;
			delta_lambdasquareds[i]=deltalambdasq;
//			d.set_subvector(i, tempcvec);		// set i-1 position to tempcvec (conversion currently does not work)		
			d[i]=complex<double>(real,imag);
		}
		else		// otherwise use push back function to append at the end of the vector
		{
		   	lambdasquareds.push_back(lambdasq);
			delta_lambdasquareds.push_back(deltalambdasq);			
//			d.ins(d.size(), tempcvec);			// insert tempcvec at end of d
			d[i] = complex<double>(real,imag); 
		}

		i++;											// increment index into data vector
	}
	
	// For some reason the push_back creates an empty element both in
	// lambdasquareds and delta_lambda_squareds which has to be popped_back (removed)
	//lambdasquareds.pop_back();
	//delta_lambda_squareds.pop_back();
	
	infile.close();
}


/*!
  \brief Read Faraday depths from a file

  \param filename - name of text file containing Faraday depths
*/
void wienerfilter::readFaradayDepthsFromFile(const std::string &filename)
{



}


/*!
  \brief Read frequencies from a file
  
  \param filename - name of text file containing frequency list
*/

/*!
  \brief Read polarized data from a file

  \param filename - name of text file containing polarized data
*/
void wienerfilter::readDataFromFile(const std::string &filename)
{
	ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);		// file with lambda squareds
	
	double real=0;				// temporary variable for real part per line
	double imag=0;				// temporary variable for imaginary part per line
	complex<double> intensity;	// temporary complex intensity
	unsigned int i=0;			// loop variable
	
	// Temporary vectors
	vector<complex<double> > intensities;
	cvec tempcvec;

   	if(infile.fail())
   	{
	  throw "wienerfilter::read4SimFreqDataFromFile failed to open file";
   	}
	
	while(infile.good())
	{
		infile >> real >> imag;

	   // Write complex vector to wienerfilter class data vector d (no easier way found than this)
//		tempcvec=to_cvec(real, imag);				

		if(frequencies.size() > i)		// if there is size left in lambdasquareds and delta_lambda_squareds vectors
		{
//			d.set_subvector(i, tempcvec);	// set i-1 position to tempcvec (conversion currently does not work)		
			d[i] = complex<double>(real,imag);
		}
		else		// otherwise use push back function to append at the end of the vector
		{
//		   d.ins(d.size(), tempcvec);		// insert tempcvec at end of d
		   d[i] = complex<double>(real,imag);
		}

		i++;					// increment index into data vector
	}
	
	infile.close();
}


/*!
	\brief Read frequencies, delta frequencies and complex data vector from a text file
	
	\param filename - name of text file
*/
void wienerfilter::read4SimFreqDataFromFile(const std::string &filename)
{
	ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);		// file with lambda squareds
	
	double freq=0;				// temporary variable for lambda sqaured per line
   	double deltafreq=0;			// temporary variable for delta lambda squared per line
	double real=0;				// temporary variable for real part per line
	double imag=0;				// temporary variable for imaginary part per line
	complex<double> intensity;	// temporary complex intensity
	unsigned int i=0;			// loop variable
	
	// Temporary vectors
	vector<complex<double> > intensities;
	cvec tempcvec;

   	if(infile.fail())
   	{
	  throw "wienerfilter::read4SimFreqDataFromFile failed to open file";
   	}
	
	while(infile.good())
	{
		infile >> freq >> deltafreq >> real >> imag;

	   // Write complex vector to wienerfilter class data vector d (no easier way found than this)
//		tempcvec=to_cvec(real, imag);				

		if(frequencies.size() > i)				// if there is size left in lambdasquareds and delta_lambda_squareds vectors
		{
			frequencies[i]=freq;
			delta_frequencies[i]=deltafreq;
//			d.set_subvector(i, tempcvec);		// set i-1 position to tempcvec (conversion currently does not work)		
			d[i]=complex<double>(real,imag);
		}
		else		// otherwise use push back function to append at the end of the vector
		{
		   frequencies.push_back(freq);
		   delta_frequencies.push_back(deltafreq);			
//		   d.ins(d.size(), tempcvec);			// insert tempcvec at end of d
  		   d[i]=complex<double>(real,imag);		   
		}

		i++;											// increment index into data vector
	}
	
	// For some reason the push_back creates an empty element both in
	// lambdasquareds and delta_lambda_squareds which has to be popped_back (removed)
	//frequencies.pop_back();
	//delta_frequencies.pop_back();
	
	infile.close();
}



/*!
  \brief Read a data vector from a text file (mainly for debugging)

  \param filename - name of file to read from
*/
void wienerfilter::readRealDataFromFile(const std::string &filename)
{
  ifstream infile(const_cast<const char *>(filename.c_str()), ifstream::in);

  unsigned int i=0;
  while(infile.good())
  {
  	infile >> d[i++];
  }
}


/*!
  \brief Write a data vector out to file on disk (mainly for debugging)

  \param filename - name of file to create or append to
*/
void wienerfilter::writeDataToFile(const std::string &filename)
{
  ofstream outfile(const_cast<const char *>(filename.c_str()), ofstream::out);

  for(unsigned i=0; i < (unsigned) d.length(); i++) 	// loop over vector
  {
	outfile << d[i];
    outfile << endl;             // add endl
  }

  outfile.flush();               // flush output file
}


/*!
  \brief Write a signal vector out to file on disk (mainly for debugging)

  \param filename - name of file to create or append to
*/
void wienerfilter::writeSignalToFile(const std::string &filename)
{
  if(s.size()==0)
    throw "wienerfilter::writeSignalToFile signal has size 0";
  if(filename=="")
    throw "wienerfilter::writeSignalToFile no filename given";

  ofstream outfile(const_cast<const char *>(filename.c_str()), ofstream::out);

  for(unsigned int i=0; i < (unsigned int) s.size(); i++)	// loop over vector
  {
    outfile << faradaydepths[i] << "\t";	// write faradaydepth
    outfile << s[i];				// write element to file
    outfile << endl;             		// add endl
  }

  outfile.flush();               		// flush output file
}


/*!
  \brief Write a map vector out to file on disk (mainly for debugging)

  \param filename - name of file to create
*/
void wienerfilter::writeMapToFile(const std::string &filename)
{
  uint i=0;     // loop variable

  if(faradaydepths.size()==0)			// if Faradaydepths have not been set
    throw "wienerfilter::writeMapToFile faradaydepths has size 0";
  if(faradaydepths.size()!=(unsigned int) m.size())		// if number of Faraday depths differs from length of map
    throw "wienerfilter::writeMapToFile faradaydepths differ from map in size";
  if(filename=="")
    throw "wienerfilter::writeMapToFile no filename given";

  ofstream outfile(const_cast<const char *>(filename.c_str()), ofstream::out);

  for(i=0; i<m.size(); i++) 	   		// loop over vector
  {
    outfile << faradaydepths[i] << "\t";
    outfile << m[i] << "\t";	 		// write element to file
    outfile << endl;           			// add endl
  }

  outfile.flush();               		// flush output file
}


//***************************************************************************
//
// Mathematica output routines
//
//****************************************************************************


/*!
  \brief Write out a real matrix in Mathematica format
  
  \param M - real matrix to write out
  \param filename - filename of Mathematica text file to write
*/
void wienerfilter::writeMathematica(mat M, const string &filename)
{
  unsigned int rows=M.rows(), cols=M.cols();
  ofstream outfile(const_cast<const char*>(filename.c_str()), ofstream::out);

  //-----------------------------------------------------
  // Check input parameters
  if(rows==0 || cols==0)
    throw "wienerfilter::writeASCII Matrix M has size 0";
  if(filename=="")  
    throw "wienerfilter::writeASCII no filename given";

  //-----------------------------------------------------
  outfile << "{";
  for(unsigned int i=0; i < rows; i++)			// loop over rows
  {
	 outfile << "{";
	 for(unsigned int j=0; j < cols; j++)			// loop over columns
	 {
	    if(j!=cols-1)
	      outfile << M(i,j) << ", ";	// write out real with colon
	    else
	      outfile << M(i,j);		// no colon after last value
	 }
	 if(i!=rows-1)
	    outfile << "}," << endl;		// at end of each row add "}," and a newline
	 else
	    outfile << "}" << endl;		// at last row only add "} "a newline
  }
  outfile << "}" << endl;
}


/*!
  \brief Write out in Mathematica format the real or imaginary part of a complex matrix
  
  \param M - complex matrix
  \param part - string defining "real" or "imaginary" part to write out
  \param filename - name of ASCII file to write to
*/
void wienerfilter::writeMathematica(cmat M, const std::string &part, const std::string &filename)
{
  unsigned int rows=M.rows(), cols=M.cols();
  ofstream outfile(const_cast<const char*>(filename.c_str()), ofstream::out);

  //-----------------------------------------------------
  // Check input parameters
  if(rows==0 || cols==0)
    throw "wienerfilter::writeASCII Matrix M has size 0";
  if(filename=="")  
    throw "wienerfilter::writeASCII no filename given";
  if(part!="real" && part!="imaginary")
    throw "wienerfilter::writeASCII wrong part identifier";

  //-----------------------------------------------------
  if(part=="real")
  {
    outfile << "{";
    for(unsigned int i=0; i < rows; i++)	// loop over rows
    {
	 for(unsigned int j=0; j < cols; j++)	// loop over columns
	 {
	    if(j!=cols-1)
	      outfile << M(i,j).real() << ", ";	// write out real and imaginary part of vector
	    else
	      outfile << M(i,j).real();		// no colon after last value
	 }
	 if(i!=rows-1)
	    outfile << "}," << endl;		// at end of each row add "}" and a newline
	 else
	    outfile << "}" << endl;		// at end of last row add only a "}" and a newline
    }
    outfile << "}" << endl;
  }
  else
  {
    outfile << "{";
    for(unsigned int i=0; i < rows; i++)	// loop over rows
    {
      for(unsigned int j=0; j < cols; j++)	// loop over columns
      {
	outfile << M(i,j).imag() << ",\t";	// write out real and imaginary part of vector
      }
      outfile << "}," << endl;				// at end of each row add a newline
    }
    outfile << "}" << endl;
  }
  
}



//***************************************************************************
//
// Debug signal mock routines
//
//****************************************************************************

/*!
  \brief Create simple complex data (sinus for real and imaginary part)

  \param max - maximum amplitude
  \param stretch - factor by which to stretch sinus
*/
void wienerfilter::createData(double max, double stretch, double shift)
{
  if(max==0)
    throw "wienerfilter::createData max is 0";
  if(frequencies.size()==0)
    throw "wienerfilter::createData frequencies vector is 0";

  for(unsigned int i=0; i < frequencies.size(); i++)
  {
//    cout << 2*M_PI*i/stretch << endl;			// debug
    d[i].real()=shift+max*sin(i*stretch*M_PI/frequencies.size());
    d[i].imag()=shift+max*sin(i*stretch*M_PI/frequencies.size());
  }
}


/*!
  \brief Create simple complex input signal (sinus for real and imaginary part)

  \param max - maximum amplitude
  \param stretch - factor by which to stretch sinus
*/
void wienerfilter::createSignal(double max, double stretch, double shift)
{
  if(max==0)
    throw "wienerfilter::createSignal max is 0";
  if(faradaydepths.size()==0)
    throw "wienerfilter::createSignal frequencies vector is 0";

  for(unsigned int i=0; i < faradaydepths.size(); i++)
  {
//    cout << 2*M_PI*i/stretch << endl;			// debug
    s[i].real()=shift+max*sin(i*stretch*M_PI/faradaydepths.size());
 //   s[i].imag()=shift+max*sin(i*stretch*M_PI/faradaydepths.size());
  }
}



/*!
  \brief Create a simple response Matrix for testing

  \param shift - value to shift signal by
  \param scale - value to scale signal by
*/
void wienerfilter::createSimpleResponseMatrix(int shift, double scale)
{
  if(d.size()==0)
     throw "wienerfilter::createSimpleResponseMatrix data vector d has size 0";
  if(s.size()==0)
	  throw "wienerfilter::createSimpleResponseMatrix signal vector s has size 0";

  // Create Response matrix with faradaydepths rows and frequencies columns 
  int Nfaradaydepths=s.length();
  int Nfrequencies=d.length(); 

  R.set_size(Nfrequencies, Nfaradaydepths);		// set size of response matrix


  // Simple shift and scale the original signal
  for(uint row=0; row<R.rows(); row++)
  {
    for(uint col=0; col<R.cols(); col++)
    {
      if(row==col+shift)
      {
	R.set(row, col, 1.0/scale);		// scale and shift the signal
 //	cout << "R.set(row, col, scale) = " << R(row, col) << endl;
      }
      else
      {
	R.set(row, col, 0);
 //	cout << "R.set(row, col, scale) = " << R(row, col) << endl;	
      }
    }
  }
}


/*!
  \brief Compute the Power of a complex vector
  
  \param signal - STL vector signal to compute P=sqrt(Q^2 + U^2)
  \param power - vector to hold resulting P
*/
void wienerfilter::complexPower(vector<complex<double> > &signal, vector<double> &power)
{
   if(signal.size()==0)
      throw "wienerfilter::complexPower signal has size 0";

   if(power.size()!=(unsigned int)signal.size())		// if power vector has not the same length as signal
      power.resize(signal.size());		// resize power vector

   for(unsigned int i=0; i < (unsigned int)signal.size(); i++)
      power[i]=sqrt(signal[i].real()*signal[i].real()+signal[i].imag()*signal[i].imag());
}


/*!
  \brief Compute the Power of a complex vector
  
  \param signal - ITPP vector signal to compute P=sqrt(Q^2 + U^2)
  \param power - vector to hold resulting P
*/
void wienerfilter::complexPower( cvec &signal, vector<double> &power)
{
   if(signal.size()==0)
      throw "wienerfilter::complexPower signal has size 0";

   if(power.size()!=(unsigned int)signal.size())		// if power vector has not the same length as signal
      power.resize(signal.size());		// resize power vector

   for(unsigned int i=0; i < (unsigned int)signal.size(); i++)
      power[i]=sqrt(signal[i].real()*signal[i].real()+signal[i].imag()*signal[i].imag());
}



}  // end namespace RM
