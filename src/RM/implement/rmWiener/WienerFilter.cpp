//  	ITPP Test program and Wiener Filter test implementation
//
//	File:			wienerfilter.cpp
//	Author:			Sven Duscha (sduscha@mpa-garching.mpg.de)
//	Date:			09-06-2009
//	Last change:		18-10-2009

/* Standard header files */
#include <complex>
#include <vector>
#include <fstream>
#include <algorithm>			// for minimum element in vector
#include <limits>			// limit for S infinite
#include <iomanip>			// to set precision in cout
/* RM header files */
#include "WienerFilter.h"

using namespace std;
using namespace itpp;

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
    epsilon_0=1e-26;				// epsilon zero (Jy conversion)
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
  }
  
  
  /*!
    \brief Constructor with frequency and Faraday depth dimenions
  
  This constructor sets values for the number of lambda squared (frequency) channels
  and the number of faraday depths to be probed for. This allows to set already
  appropriate lengths for both the data and the signal vector.
  
  \param nfreqs - number of frequency channels in data d
  \param faradays - vector containing Faraday depths to be probed for
*/
wienerfilter::wienerfilter(int nfreqs, const vector<double> &faradays)
{
  d.set_size(nfreqs);					// set length of data vector to number of frequencies
  s.set_size(faradays.size());			// set length of signal vector to number of Faraday depths
  j.set_size(s.size());					// set j to size of s
  maperror.set_size(s.size());			// set size of map error the same as s
  m.set_size(faradays.size());			// set size of m
  faradaydepths=faradays;				// set faradaydepths to the content of the vector faradays provided as parameter
  delta_faradaydepths.resize(faradays.size());	// resize vector containing delta Faraday depth distances
  frequencies.resize(nfreqs);			// set length of vector for frequencies
  delta_frequencies.resize(nfreqs);		// set length of vector for delta frequencies
  lambdasquareds.resize(nfreqs);		// set size of lambdasquareds vector
  delta_lambdasquareds.resize(nfreqs);	// set size of delta_lambda_squareds	
  bandpass.resize(nfreqs);				// bandpass vector for each frequency
  
  // Set default filenames
  Sfilename="Smatrix.it";				// filename for S matrix file
  Rfilename="Rmatrix.it";				// filename for R matrix file
  Nfilename="Nmatrix.it";				// filename for N matrix file
  Dfilename="Dmatrix.it";				// filename for D matrix file
  Qfilename="Qmatrix.it";				// filename for Q matrix file
  
  variance_s=25;						// initialize variance of s
  lambda_phi=0.5;						// initialize lambda phi coherence length
  nu_0=1400000000;						// default value for nu_0=1.4GHz
  epsilon_0=1e-26;						// epsilon zero (Jy conversion)
  
  maxiterations=10;						// default value for maximum number of iterations
  rmsthreshold=-1;						// default, don't try to achieve rms threshold
  
  for(int i=0; i<nfreqs; i++)			// write default values into bandpass vector
    bandpass[i]=complex<double>(1,1);	// default is 1 both for Q and U
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



//*****************************************************
//
// Wiener filtering high-level calling functions
//
//*****************************************************
/*!
  \brief Perform Wiener filtering
  
  \param noise - noise value (the same for all channels)
  \param type - type of Signal covariance matrix S ("white"=default, "gaussian", "powerlaw")
*/
void wienerfilter::wienerFiltering(double noise, const std::string &type)
{
  cout << "1 is called" << endl;
  createNoiseMatrix(noise);
  createSMatrix(type);
  doWienerFiltering();
}


/*!
  \brief Perform Wiener filtering with "white" Signal covariance matrix assumed
  
  \param noise - noise value to be set for all channels
  \param lambda_phi - lambda phi coherence length of signal
  \param alpha - spectral index (default=-0.7)
*/
void wienerfilter::wienerFiltering(double noise, double lambda_phi, double alpha)
{
  cout << "2 is called" << endl;
  createNoiseMatrix(noise);
  setLambdaPhi(lambda_phi);
  setAlpha(alpha);
  createSMatrix("white");

  doWienerFiltering();
}


/*!
  \brief Perform Wiener filtering with Signal covariance specified
  
  \param noise - noise value to be set for all channels
  \param lambda_phi - lambda phi coherence length of signal
  \param alpha - spectral index
  \param type - type of Signal covariance ("white"=default, "gaussian" or "powerlaw")
*/
void wienerfilter::wienerFiltering(double noise, double lambda_phi, double alpha, const std::string &type)
{
  cout << "3 is called" << endl;
  createNoiseMatrix(noise);

  setLambdaPhi(lambda_phi);
  setAlpha(alpha);
  createSMatrix(type);

  doWienerFiltering();
}


/*!
  \brief Perform Wiener filtering with "white" signal assumed
  
  \param noise - noise value to be set for all channels
  \param lambda_phi - lambda phi coherence length of signal
  \param epsilon_zero - emissivity at nu_0
  \param alpha - spectral index (default=-0.7)
*/
void wienerfilter::wienerFiltering(double noise, double lambda_phi, double epsilon_zero, double alpha)
{
  cout << "4 is called" << endl;
  createNoiseMatrix(noise);
  setLambdaPhi(lambda_phi);
  setEpsilonZero(epsilon_zero);
  setAlpha(alpha);  

  
  createSMatrix("white");

  doWienerFiltering();
}


/*!
  \brief Perform Wiener filtering with "white" signal assumed
  
  \param noise - noise value to be set for all channels
  \param lambda_phi - lambda phi coherence length of signal
  \param epsilon_zero - emissivity at nu_0
  \param alpha - spectral index
  \param type - type of Signal covariance ("white"=default, "gaussian" or "powerlaw")  
*/
void wienerfilter::wienerFiltering(double noise, double lambda_phi, double epsilon_zero, double alpha, const std::string &type)
{
  cout << "5 is called" << endl;
  createNoiseMatrix(noise);
  setLambdaPhi(lambda_phi);
  setEpsilonZero(epsilon_zero);
  setAlpha(alpha);  
  setVariance_s(20);
  createSMatrix(type);

  doWienerFiltering();
}


/*!
  \brief Perform Wiener filtering
  
  \param noise - vector with noise value for each channel
  \param type - Type of Signal covariance matrix S ("white"=default, "gaussian", "powerlaw")
*/
void wienerfilter::wienerFiltering(const vector<double> &noise, const std::string &type)
{
  cout << "6 is called" << endl;
  createNoiseMatrix(noise);
  createSMatrix(type);

  doWienerFiltering();
}


/*!
  \brief Perform Wiener filtering

  \param noise - noise vector to be set for each individual channels
  \param lambda_phi - lambda phi coherence length of signal
  \param alpha - spectral index (default=-0.7)
*/
void wienerfilter::wienerFiltering(const vector<double> &noise, double lambda_phi, double alpha)
{
  cout << "7 is called" << endl;
  createNoiseMatrix(noise);
  setLambdaPhi(lambda_phi);
  setAlpha(alpha);
  createSMatrix("white");  

  doWienerFiltering();
}


/*!
  \brief Perform Wiener filtering

  \param noise - noise vector to be set for each individual channels
  \param lambda_phi - lambda phi coherence length of signal
  \param alpha - spectral index (default=-0.7)
  \param type - type of Signal covariance matrix ("white"=default, "gaussian" or "powerlaw")
*/
void wienerfilter::wienerFiltering(const vector<double> &noise, double lambda_phi, double alpha, const std::string &type)
{
  cout << "8 is called" << endl;
  createNoiseMatrix(noise);
  setLambdaPhi(lambda_phi);
  setAlpha(alpha);
  createSMatrix(type);  

  doWienerFiltering();
}


/*!
  \brief Perform Wiener filtering

  \param noise - noise vector to be set for all channels
  \param lambda_phi - lambda phi coherence length of signal
  \param alpha - spectral index (default=-0.7)
*/
void wienerfilter::wienerFiltering(const vector<double> &noise, double epsilon_zero, double lambda_phi, double alpha)
{
  cout << "9 is called" << endl;
  createNoiseMatrix(noise);
  setLambdaPhi(lambda_phi);
  setEpsilonZero(epsilon_zero);
  setAlpha(alpha);
  createSMatrix("white");

  doWienerFiltering();
}


/*!
  \brief Perform Wiener filtering

  \param noise - noise value to be set for all channels
  \param lambda_phi - lambda phi coherence length of signal
  \param alpha - spectral index
  \param type - type of Signal covariance matrix ("white"=default, "gaussian" or "powerlaw")
*/
void wienerfilter::wienerFiltering(const vector<double> &noise, double epsilon_zero, double lambda_phi, double alpha, const std::string &type)
{
  cout << "10 is called" << endl;
  createNoiseMatrix(noise);
  setLambdaPhi(lambda_phi);
  setEpsilonZero(epsilon_zero);
  setAlpha(alpha);
  createSMatrix(type);


  cout << "variance_s = " << getVariance_s() << endl;
  cout << "lambda_phi = " << getLambdaPhi() << endl;
  cout << "epsilon_0 = " << getEpsilonZero() << endl;
  cout << "alpha = " << getAlpha() << endl;


  doWienerFiltering();
}


/*!
  \brief Apply the Wiener Filter operator to a data vector to reconstruct a map
  
  \param data - vector containing complex data
  \param map - reconstructed map
*/
void wienerfilter::applyWF(const cvec &data, cvec &map)
{
  if(data.size()==0)
    throw "wienerfilter::applyWF data vector has size 0";

  //****************************************
  if(WF.cols()==0 && WF.rows()==0)	// if WF matrix does not exist
  {
    throw "wienerfilter::applyWF WF matrix does not exist";
  }

  map=WF*data;		// reconstruct map from data
}



	
	
/*!
  \brief Apply the Wiener Filter operator to a data vector to reconstruct a map
  
  \param data - vector containing complex data
  \param map - reconstructed map
*/
void wienerfilter::applyWF(const std::vector<std::complex<double> > &data, std::vector<std::complex<double> > &map)
{
  if(data.size()==0)
    throw "wienerfilter::applyWF data vector has size 0";

  // ****************************************
  if(WF.cols()==0 && WF.rows()==0)	// if WF matrix does not exist
  {
    throw "wienerfilter::applyWF WF matrix does not exist";
  }

  map=WF*data;		// reconstruct map from data
}


//*****************************************************
//
// Internal Wiener filtering algorithm functions
//
//*****************************************************

/*!
  \brief internal function that does actual Wiener filtering
*/
void wienerfilter::doWienerFiltering()
{
  createResponseMatrix();	  // Set up matrices

  // Perform Wiener Filtering computations
  computej(); 
  computeD();

  computeQ();
  computeW();
  computeM();


  if(_DEBUG_)	// If _DEBUG_ is set, output matrices to files
  {
    writeASCII(N, "./intermediate/N.dat");
    writeASCII(D, "real" ,"./intermediate/D_r.dat");
    writeASCII(D, "imaginary" ,"./intermediate/D_i.dat");

    writeASCII(S, "real" ,"./intermediate/S_r.dat");
    writeASCII(S, "imaginary" ,"./intermediate/S_i.dat");

    writeASCII(R, "real" ,"./intermediate/R_r.dat");
    writeASCII(R, "imaginary" ,"./intermediate/R_i.dat");

    writeASCII(M, "real" ,"./intermediate/M_r.dat"); 
    writeASCII(M, "imaginary" ,"./intermediate/M_i.dat"); 
  }
  
  reconstructm();
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
  if(noise < 0)		// only if positive rmsnoise is given
	 throw "wienerfilter::createNoiseMatrix rmsnoise<0";
  else
  {
	  if(d.length()>0)					// only if d vector has length > 0
	  {
		  int size=d.length();				// get size of data vector d
		 
		  N.set_size(size, size);			// set size of Noise matrix
		  N.zeros();					// initialize matrix to 0-matrix

		  for(int i=0; i<size; i++)			// loop over rows (quadratic matrix)
		  {
		     // set rms^2 value to all diagonal elements (because it's the Noise Covariance matrix)
		     N.set(i, i, noise*noise);
		  }
	  }
	  else
	     throw "wienerfilter::createNoiseMatrix d vector has length 0";	
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
  if(noiseperchan.size()==0)			// only if positive rmsnoise is given
    throw "wienerfilter::createNoiseMatrix noiseperchan has zero length";
  else if(noiseperchan.size()!=(unsigned int) d.size())
    throw "wienerfilter::createNoiseMatrix noiseperchan differs in length from data vector d";
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
void wienerfilter::createResponseMatrix()
{
  const double csq=89875517873681764.0;		// c^2 constant

  complex<double> element=0;			// Matrix element in Vandermonde matrix for DFT
  complex<double> rowfactor=0;			// common factor for Matrix elements in the same row
  vector<double> frequenciessquared(frequencies.size());	// vector to hold pre-computed frequencies squared
  double Faradayexponent=0;			// Faraday rotation exponent
  
  //***********************************************************************
  // Parameter integrity checks
  //***********************************************************************
  if(d.size()==0)
     throw "wienerfilter::createResponseMatrix data vector d has size 0";
  if(s.size()==0)
     throw "wienerfilter::createResponseMatrix signal vector s has size 0";
  if(frequencies.size()!=(unsigned int) d.size())
     throw "wienerfilter::createResponseMatrix frequencies has invalid size";
  if(delta_frequencies.size()!=(unsigned int) d.size())
     throw "wienerfilter::createResponseMatrix delta frequencies has invalid size";
  if(faradaydepths.size()!=(unsigned int) s.size())
	  throw "wienerfilter::createResponseMatrix faradaydepths has invalid size";
 
  // Create Response matrix with faradaydepths rows and frequencies columns 
  int Nfaradaydepths=s.length();
  int Nfrequencies=d.length(); 

  R.set_size(Nfrequencies, Nfaradaydepths);		// set size of response matrix

  // Check if spectral dependence parameters are valid
  if(nu_0==0)
     throw "wienerfilter::createResponseMatrix nu_0 is 0";  
  if(alpha==0)	// check if alpha is valid
     throw "wienerfilter::createResponseMatrix alpha is 0";


  // ************************************************************************
  // Calculate individual matrix elements
  // ************************************************************************
    
  // compute freq^2 vector first
  for(unsigned int i=0; i<frequencies.size(); i++)  
    frequenciessquared[i]=frequencies[i]*frequencies[i];
  
//  cout << "createResponseMatrix(): R.rows()=" << R.rows() << "\tR.cols()=" << R.cols() << endl;			// debug
//  cout << "alpha=" << alpha << endl;			// debug
//  cout << "epsilonZero = " << epsilon_0 << endl;	// debug
  
//  R.zeros();
  for(int k=0; k<R.rows(); k++)		// loop over all rows of R (i.e. frequencies)
  {  	 
      // Convert Jy to SI unit
      // spectral dependence factor and bandpass common to all elements in the same row
      rowfactor=complex<double> ( bandpass[k] * epsilon_0 * pow(frequencies[k]/nu_0, -alpha) * delta_frequencies[k] );

  //    cout << k << "\t" << rowfactor << "\t";

      for(int l=0; l<R.cols(); l++)	// loop over all columns of R (i.e. Faraday depths)
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
  if(d.size()==0)
     throw "wienerfilter::createResponseMatrixIntegral data vector d has size 0";
  if(s.size()==0)
     throw "wienerfilter::createResponseMatrixIntegral signal vector s has size 0";
  if(frequencies.size()!=(unsigned int) d.size())
     throw "wienerfilter::createResponseMatrixIntegral frequencies has invalid size";
  if(delta_frequencies.size()!=(unsigned int) d.size())
     throw "wienerfilter::createResponseMatrixIntegral delta frequencies has invalid size";
  if(faradaydepths.size()!=(unsigned int) s.size())
	  throw "wienerfilter::createResponseMatrixIntegral faradaydepths has invalid size";
 
  // Create Response matrix with faradaydepths rows and frequencies columns 
  int Nfaradaydepths=s.length();
  int Nfrequencies=d.length(); 

  R.set_size(Nfrequencies, Nfaradaydepths);		// set size of response matrix

  // Check if spectral dependence parameters are valid
  if(nu_0==0)
     throw "wienerfilter::createResponseMatrixIntegral nu_0 is 0";  
  if(alpha==0)	// check if alpha is valid
     throw "wienerfilter::createResponseMatrixIntegral alpha is 0";


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
 
 
  for(int k=0; k<R.rows(); k++)		// loop over all rows of R (i.e. frequencies)
  {  	 
      // Convert Jy to SI unit
      // spectral dependence factor and bandpass common to all elements in the same row
      rowfactor=complex<double> ( bandpass[k] * epsilon_0 * pow(frequencies[k]/nu_0, -alpha) * delta_frequencies[k] );

  //    cout << k << "\t" << rowfactor << "\t";

      for(int l=0; l<R.cols(); l++)	// loop over all columns of R (i.e. Faraday depths)
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
		throw "wienerfilter::computeVariance_s d has size 0";
	}
	if(R.rows()!=d.size() || R.cols()!=s.size())
	{
		throw "wienerfilter::computeVariance_s R has incorrect size";
	}
	
	//------------------------------------------------------		
	// Transform d -> s using R^{-1}
// 	cout << "ds.size() = " << ds.size() << endl;
	ds=inv(R)*(d);			// really need d-n and therefore a loop then!
// 	ds=d;

	for(int i=0; i<size; i++)		// Compute complex average
	{
	  sum=sum+ds[i];			// sum over all elements
	}
	cavg=sum/size;		// that size isnt 0 is checked for above
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
	if(s.size()==0)						// if s has zero length
	  throw "wienerfilter::computeSMatrix s vector has 0 length";
	if(faradaydepths.size()!=(unsigned int)s.size())
	  throw "wienerfilter::computeSMatrix";
	
	// ******************************************************
	S.set_size(s.size(), s.size());		// set size of S matrix to dimensions of d-vector

	if(variance_s <= 0)			// if standard deviation is smaller or equal zero
	{
		throw "wienerfilter::computeSMatrix dispersion_s <= 0";
	}
	if(lambda_phi<=0)					// if lambda_phi has an invalid value < 0
	{
		throw "wienerfilter::computeSMatrix lambda_phi < 0";
	}


	if(type=="gaussian")					// if initial guess is a Gaussian decay:
	{
		vector<double> boundarycondition(5);		// vector with boundary conditions
		vector<double>::iterator pos;			// position index of minimum element
		double lambda_phi_corrected=0.0;		// corrected lambda_phi, we don't want to destroy our class attribute lambda_phi!
	
		// Compute constant factors: 0.25*lambda_phi^2
//		divisor=0.25*lambda_phi*lambda_phi; // old: without peridic boundary condition

		// Create S matrix guess with Gaussian decay formula
		//
		// S(i,j)=dispersion_s^2*exp(0.5*abs(phi_i-phi_j)^2/(2*lambda_phi^2))
		//
		for(int row=0; row<S.rows(); row++)			// loop over rows of S
		{
			for(int col=0; col<S.cols(); col++)		// loop over columns of S
			{
//				dist=abs(row-col);			// compute absolute distance between phi_i and phi_j 
				dist=abs(faradaydepths[row]-faradaydepths[col]);
				dist*=dist;				// square absolute distance				
				// Periodic boundary condition for S matrix:
				// lambda=lambda_phi*min(faradaydepths[col]/lambda_phi, faradaydepths[row]/lambda_phi, 1)
			
				// write values into vector
				boundarycondition[0]=faradaydepths[row]/lambda_phi;
				boundarycondition[1]=faradaydepths[col]/lambda_phi; 
				boundarycondition[2]=(faradaydepths[S.size()-1]-faradaydepths[row])/lambda_phi;
				boundarycondition[3]=(faradaydepths[S.size()-1]-faradaydepths[col])/lambda_phi; 
				boundarycondition[4]=1.0;
			
				cout << "fd[" << row << "]=" << faradaydepths[row] << "\tfd[" << col << "]=" << faradaydepths[col] << "\tlambda_phi=" << lambda_phi << endl;
				cout << "b[0]=" << boundarycondition[0] << "\tb[1]=" << boundarycondition[1] << "\tb[2]=" << boundarycondition[2];
			
				// find minimum in vector
				pos = min_element (boundarycondition.begin(), boundarycondition.end());
				lambda_phi_corrected=*pos;	// set to corrected lambda_phi
				
				if(lambda_phi_corrected != lambda_phi)
				  cout << "\tlambda_phi_corrected (" << row << "," << col << ") = " << lambda_phi_corrected << endl;
				
				divisor=0.25*lambda_phi_corrected*lambda_phi_corrected;	// compute divisor
				
				
// 				divisor=0.25*lambda_phi*lambda_phi

				S.set(row, col, variance_s*exp(-dist/divisor) );	// Write element into S
			}
		}

		// Write Gaussian decay formula into Sformula string
		Sformula="dispersion_s^2*exp(0.5*abs(phi_i-phi_j)^2/(2*lambda_phi^2))";
		Stype="gaussian";
	}
	else if(type=="white")
	{
		// S(i,j)=dispersion_s*Kronecker(i,j) diagonal!
		for(int row=0; row<S.rows(); row++)			// loop over rows of S
		{
			for(int col=0; col<S.cols(); col++)		// loop over columns of S
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
		for(int row=0; row<S.rows(); row++)				// loop over rows of S
		{
			for(int col=0; col<S.cols(); col++)			// loop over columns of S
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
		for(int row=0; row<S.rows(); row++)
		{
			for(int col=0; col<S.cols(); col++)						// set each element of S to:
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
		throw "wienerfilter::computeSMatrix unknown spectral dependency function";
	}
}


/*!
	\brief Iterate signal matrix S with results from Wiener filtering of statistically enough s vectors
*/
void wienerfilter::iterateSMatrix()
{
	// Check if we have enough s vectors for statistical guess
	if(number_of_iterations<LOSLIMIT)
		throw "wienerfilter::iterateSMatrix() number_of_los calculated < LOSLIMIT";

	// Adjust s and create new autocorrelation matrix S (k+1) compute
	// over all line of sights
	// S(i,j)=1/N_los*Sum_l=1^N_los(<m_i^(k), m_j^(k)> + D(i,j)^(k))
	for(unsigned int l=0; l<number_of_iterations; l++)
	{
		// TODO!
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
			cout << "computej: j = W*d" << endl;			// debug
			#endif
			j=W*d;							// compute j
		}
		else		// otherwise compute it from individual matrices
		{
			#ifdef debug_
			cout << "computej: j=R.H()*(inv(N)*d)" << endl;		// debug
			#endif
			j=R.H()*(inv(N)*d);					// compute j
		}
	}
}


/*!
	\brief Computes the Propagator D,  D=R^(dagger)N^(inverse)R=j*R
*/
void wienerfilter::computeD()								
{
	if(j.length()==0)
		throw "wienerfilter::computeD j has zero length";
	else if(R.rows()==0 || R.cols()==0)
		throw "wienerfilter::computeD R has size 0";
	else if(R.cols()!=s.length() || R.rows()!=d.length())
		throw "wienerfilter::computeD R has invalid size";
	else if(S.cols()!=s.length() || S.rows()!=s.length())
		throw "wienerfilter::computeD S has invalid size";
	else if(N.rows()!=d.length() || N.cols()!=d.length())
		throw "wienerfilter::computeD N has invalid size";
	else
	{
		if(W.rows()!=0 && W.cols()!=0)			// if we already have W
		{
			#ifdef debug_
			cout << "computeD: D = inv(S) + W *R" << endl;
			#endif
			D = inv(S) + W * R;					// compute with W and simplified formula
		}
		else
		{
			#ifdef debug_
			cout << "computeD: D = inv(S) + R.H()*inv(N)*R" << endl;
			#endif		
			D = inv(S) + R.H()*inv(N) * R;		// D^-1=S^-1+R^{dagger}*inv(N)*R
		}
		
		D=inv(D);					// compute inverse
		computeMapError();				// Store diagonal of D in maperror
	}
}


/*!
	\brief Compute instrument fidelity Q=RSR^(inverse)
*/
void wienerfilter::computeQ()
{
	if(N.rows()!=d.length() || N.cols()!=d.length())
		throw "wienerfilter::computeQ N has invalid size";
	else if(R.rows()!=d.length() || R.cols()!=s.length())
		throw "wienerfilter::computeQ R has invalid size";
	else if(S.rows()!=s.length() || S.cols()!=s.length())
		throw "wienerfilter::computeQ S has invalid size";
	else
	{
		if(M.cols()!=0 && M.rows()!=0)		// if M was computed
		{
			Q=S*M;
		}
		else if(M.rows()!=0 && M.cols()!=0)	// if we have already M
		{
			Q=S*M;				// compute directly
		}
		else					// compute it from individual matrices
		{
			Q=R*S*R.H()*inv(N);		// compute instrument fidelity
		}
	}
}


/*!
	\brief Computer intermediate matrix W
*/
void wienerfilter::computeW()
{
	// Consistency checks
	if(R.cols()!=s.size())
		throw "wienerfilter::computeW R has wrong number columns";
	if(R.rows()!=d.size())
		throw "wienerfilter::computeW R has wrong number of rows";
	if(N.rows()!=d.size())
		throw "wienerfilter::computeW N has wrong number of rows";
	if(N.rows()!=N.cols())
		throw "wienerfilter::computeW N is not quadratic";
	
	W=R.H()*inv(N);		// compute intermediate product W=R^{dagger}*inv(N)
}


/*!
	\brief Computer intermediate matrix M
*/
void wienerfilter::computeM()
{
	// Consistency checks TODO
	if(W.cols()==0 || W.rows()==0)
		throw "wienerfilter::computeM W has size 0 rows/columns";
	if(Rcols()==0 || R.rows()==0)
		throw "wienerfilter::computeM R has size 0 rows/columns";
	else 
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
		throw "wienerfilter::computeWF S has invalid size";
	}
	if(M.cols()!=0 && W.cols()!=0)		// if M and W were computed
	{
		WF=(inv(S)+M)*W;
	}
	else if(M.cols()!=0 && W.cols()==0)	// if we only have M
	{
		WF=(inv(inv(S) + M))*W;				// correct?
	}
	else				// ... otherwise compute it from individual matrices
	{
		WF=(inv(S) + R.H()*inv(N)*R)*R.H()*inv(N);
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
	if(lambdaphi)
	  this->lambda_phi=lambdaphi;
	else
		throw "wienerfilter::setLambdaPhi lambda_phi is 0";
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
void wienerfilter::setLambdaSquareds(const vector<double> &lambdasquareds)
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
void wienerfilter::setDeltaLambdaSquareds(const vector<double> &delta_lambdasquareds)
{
	if(delta_lambdasquareds.size()==0)
		throw "wienerfilter::setDeltaLambdaSquareds delta_lambda_squareds has size 0";
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
void wienerfilter::setFrequencies(const vector<double> &freqs)
{
	if(freqs.size()==0)
	   throw "wienerfilter::setFrequencies freqs vector is 0";
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
void wienerfilter::setDeltaFrequencies(const vector<double> &deltafreqs)
{
	if(deltafreqs.size()==0)
		throw "wienerfilter::setDeltaFrequencies deltafreqs vector has size 0";
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
void wienerfilter::setFaradayDepths(const vector<double> &faradaydepths)
{
	if(faradaydepths.size()==0)
		throw "wienerfilter::setFaradayDepths faradaydepths has size 0";
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
void wienerfilter::setBandpass(const vector<complex<double> > &bandpass)
{
	if(bandpass.size()==0)
		throw "wienerfilter::setBandpass bandpass vector has size 0";
	else
		this->bandpass=bandpass;
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
	if(epsilonzero==0)
		throw "wienerfilter::setEpsilonZero epsilonzero is 0";
	else if(epsilonzero< 0)
		throw "wienerfilter::setEpsilonZero epsilonzero is 0";
	else
		epsilon_0=epsilonzero;
}


/*!
  \brief Set variance_s - variance of signal s
*/
void wienerfilter::setVariance_s(double variance_s)
{
  if(variance_s <= 0)
    throw "wienerfilter::setVariance_s variance_s <= 0";

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
	if(nu==0)
	   throw "wienerfilter::setNuZero nu is 0";
	else if(nu < 0)
	   throw "wienerfilter::setNuZero nu < 0";
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
	if(alpha==0)
		throw "wienerfilter::setAlpha alpha is 0";
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
	if(m.size()==0)
		throw "wienerfilter::reconstructm m has size 0";
	if(m.size()!=s.size())
		throw "wienerfilter::reconstructm m has wrong size";

	if(WF.rows()!=0 && WF.cols()!=0)		// if we have the Wiener Filter matrix
	{
// 	    cout << "We have WF matrix." << endl;	// debug
	    m = WF*d;				
	}
	else
	{
// 	    cout << "m = D*j" << endl;		// debug
		
		if(D.rows()==0 || D.rows()==0)
			throw "wienerfilter::reconstructm D matrix is 0";
		else
			m = D*j;					// m=D*j(d)
	}
}


/*!
	\brief Compute the map error, diagonal elements of D

	\return cmat - map error / diagonal of propagator matrix D
*/
cvec wienerfilter::computeMapError()
{
	if(D.rows()==0 || D.cols()==0)
		throw "wienerfilter::getMapError D has size 0";
	if(D.rows()!=D.cols())
		throw "wienerfilter::getMapError D is not quadratic";
		
	for(int k=0; k < D.rows(); k++)
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
  #ifdef _ITPP_
  unsigned int size=D.size();
  vector<complex<double> > rmerror(size);

  if(size==0)
    throw "wienerfilter::getMapError D has size 0";

  for(unsigned int i=0; i < size; i++)
  {
    rmerror[i]=maperror[i];
  }
 
  return rmerror;
  #elif _ARMADILLO_
  return this->maperror;
  #endif
}


/*!
	\brief Return the propagator matrix D

	\return cmat - propagator matrix D
*/
#ifdef _ITPP_
cmat wienerfilter::getD()
{
	return D;
}
#elif _ARMADILLO_
cx_mat wienerfilter::getD()
{
	return D;
}	
#endif

	
/*!
	\brief Return the instrumental fidelity matrix Q

	\return cmat - instrumental fidelity Q
*/
#ifdef _ITPP_
cmat wienerfilter::getInstrumentFidelity()
{
	return Q;
}
#elif _ARMADILLO_
cx_mat wienerfilter::getInstrumentFidelity()
{
	return Q;
}
#endif
	
	
/*!
	\brief Return the Wiener Filter matrix WF
	
	\return cmat - Wiener Filter matrix WF
*/
#ifdef _ITPP_
cmat wienerfilter::getWienerFilter()
{
	return WF;
}
#elif _ARMADILLO_
cx_mat wienerfilter::getWienerFilter()
{
	return WF;
}	
#endif
	

/*!
	\brief Return the data vector d
*/
#ifdef _ITPP_
cvec wienerfilter::getDataVector()
{
	return d;
}
#elif _ARMADILLO_
vector<complex<double> > wienerfilter::getDataVector()
{
	return d;
}	
#endif
	

/*!
	\brief Set the data vector cvec d to values given in vector

	\param data - STL data vector
*/
#ifdef _ITPP_
void wienerfilter::setDataVector(vector<complex<double> > &data)
{
	cvec tempcvec;				// temporary vector for conversion
	unsigned int size=0;		// to hold size of complex data vector
	
	if(data.size()==0)
	{
		throw "wienerfilter::setDataVector data has size 0";
	}
	else
	{
		size=data.size();
		
		for(unsigned int i=0; i<size; i++)
		{
			d[i]=complex<double>(data[i]);		// convert STL vector to temporary cvec
				
			tempcvec=to_cvec(data[i].real(), data[i].imag());
			d.set_subvector(i, tempcvec);			// conversion currently does not work
		}
	}
}
#elif _ARMADILLO_
void wienerfilter::setDataVector(vector<complex<double> > &data)
{
	if(data.size()==0)
	{
		throw "wienerfilter::setDataVector data has size 0";
	}
	else
	{
		this->d=data;
	}
#endif	


#ifdef _ITPP_
/*!
	\brief Set the data vector cvec d to values given in ITPP cvec
	
	\param &data - ITPP cvec containing data
*/
void wienerfilter::setDataVector(cvec &data)
{
	if(data.size()==0)
	{
		throw "wienerfilter::setDataVector data has size 0";
	}
	else
		d=(cvec) data;
}

/*!
	\brief Set the signal vector cvec d to values given in ITPP cvec (only for debugging purposes!)
	
	\param &signal - ITPP cvec containing the signal
*/
void wienerfilter::setSignalVector(cvec &signalvec)
{
	if(signalvec.size()==0)
	{
		throw "wienerfilter::setSignalVector signalvec has size 0";
	}
	else
		s=signalvec;
}
#endif
	

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
void wienerfilter::readConfigFile(const string &filename)
{
	// use itpp::parser(filename) to parse config file, makes life easier
	Parser p;							// ITPP file parser
	p.init(string(filename));		// initialize parser with filename
	
	if(p.exist("Sfilename"))
		Sfilename=p.get_string("Sfilename");
	else
		cerr << "wienerfilter::readConfigfile Sfilename doesn't exist" << endl;
	if(p.exist("Rfilename"))
		Rfilename=p.get_string("Rfilename");
	else
		cerr << "wienerfilter::readConfigfile Rfilename doesn't exist" << endl;
	if(p.exist("Nfilename"))	
		Nfilename=p.get_string("Nfilename");
	else
		cerr << "wienerfilter::readConfigfile Nfilename doesn't exist" << endl;
	if(p.exist("Nfilename"))	
		Dfilename=p.get_string("Dfilename");
	else
		cerr << "wienerfilter::readConfigfile Dfilename doesn't exist" << endl;
	if(p.exist("Qfilename"))
		Qfilename=p.get_string("Qfilename");
	else
		cerr << "wienerfilter::readConfigfile Qfilename doesn't exist" << endl;
	if(p.exist("Sformula"))
		Sformula=p.get_string("Sformula");
	else
		cerr << "wienerfilter::readConfigfile Sformula doesn't exist" << endl;	
	if(p.exist("lambda_phi"))
		lambda_phi=p.get_double("lambda_phi");
	else
		cerr << "wienerfilter::readConfigFile lambda_phi doesn't exist" << endl;
	if(p.exist("maxiterations"))
		maxiterations=p.get_int("maxiterations");
	else
		cerr <<"wienerfilter::readConfigfile maxiterations doesn't exist" << endl;
	if(p.exist("rmsthreshold"))
		rmsthreshold=p.get_double("rmsthreshold");
	else
		cerr << "wienerfilter::readConfigfile rmsthreshold doesn't exist" << endl;
	
	
	/*
	unsigned int position=0;		// position token was found in string
	string substring;					// temporary substring

	if(filename.length()==0)
		throw "wienerfilter::readConfigFile invalid filename";
	
	// File contains list of Matrix filenames in ITPP format?
   ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
 
   if(infile.fail())
   {
     throw "wienerFilter::readConfigFile failed to open file";
   }
   if ( infile.is_open() )
   {
      string line;			// buffer line to read from config file

      while ( getline ( infile, line ) ) 
      {
         string::size_type i = line.find_first_not_of ( " \t\n\v" );

         if ( i != string::npos && line[i] == '#' )	// if it is a comment
            continue;

         // Process non-comment lines
    		if((position=line.find("Sfilename="))!=string::npos)				// if Sfilename is found
			{
				Sfilename=line.substr(position);
			}
			else if((position=line.find("Rfilename="))!=string::npos)		// if Rfilename is found
			{
				Rfilename=line.substr(position);
			}
			else if((position=line.find("Nfilename="))!=string::npos)		// if Nfilename is found
			{
				Nfilename=line.substr(position);
			}
    		else if((position=line.find("Dfilename="))!=string::npos)		// if Dfilename is found
			{
				Dfilename=line.substr(position);
			}
    		else if((position=line.find("Qfilename="))!=string::npos)		// if Qfilename is found
			{
				Qfilename=line.substr(position);
			}			
    		else if((position=line.find("Sformula="))!=string::npos)			// if Sformula is found
			{
				Sformula=line.substr(position);
			}			
    		else if((position=line.find("maxiterations="))!=string::npos)		// if maxiterations is found
			{
				substring=line.substr(position);
				maxiterations=atoi(substring.c_str());
			}			
    		else if((position=line.find("rmsthreshold="))!=string::npos)		// if rmsthreshold is found
			{
				substring=line.substr(position);
				rmsthreshold=atoi(substring.c_str());
			}			
			else
			{
				throw "wienerfilter::readConfigFile file contains an unknown paramter";
			}
		}
	
		infile.close();
	}
	*/
}


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



//***************************************************************************
//
// IT++ input/output routines
//
//****************************************************************************
#ifdef _ITPP_
/*!
	\brief Read Signal Matrix from a ITPP file
	
	\param filename - name of ITPP file
*/
void wienerfilter::readSignalMatrix(const string &filename)
{
	if(filename.length())
	{
		Sfilename=filename;			// Update Nfilename attribute

		it_file Sfile(filename);	// file stream for N matrix	
		Sfile >> Name("S") >> S;

		Sfile.close();
	}
	else
		throw "wienerfilter::readSignalMatrix invalid filename";
}


/*!
	\brief Write Signal matrix from a ITPP file
	
	\param filename - name of ITPP file
*/
void wienerfilter::writeSignalMatrix(const string &filename)
{
	if(filename.length())
	{
		Sfilename=filename;		// Update Sfilename attribute	

		it_file Sfile(filename);	// file stream for S matrix	
		Sfile << Name("S") << S;	

		Sfile.flush();
		Sfile.close();
	}
	else
		throw "wienerfilter::writeSignalMatrix invalid filename";
}



/*!
	\brief Read Noise Matrix from a ITPP file
	
	\param filename - name of ITPP file
*/
void wienerfilter::readNoiseMatrix(const string &filename)
{
	if(filename.length())
	{
		Nfilename=filename;			// Update Nfilename attribute

		it_file Nfile(filename);	// file stream for N matrix	
		Nfile >> Name("N") >> N;

		Nfile.close();
	}
}


/*!
	\brief Write Noise matrix from a ITPP file
	
	\param filename - name of ITPP file
*/
void wienerfilter::writeNoiseMatrix(const string &filename)
{
	if(filename.length())
	{
		Nfilename=filename;			// Update Nfilename attribute	

		it_file Nfile(filename);	// file stream for N matrix	
		Nfile << Name("N") << N;	

		Nfile.flush();
		Nfile.close();
	}
}


/*!
	\brief Read Response matrix from a ITPP file
	
	\param filename - name of ITPP file
*/
void wienerfilter::readResponseMatrix(const string &filename)
{
   if(filename.length())			// only if a valid filename was given
	{
		Rfilename=filename;			// Update Rfilename attribute

		it_file Rfile(filename);	// file stream for R matrix	
		Rfile >> Name("R") >> R;	// read into wienerfilter class member

		Rfile.close();
	}
}


/*!
	\brief Write Response matrix from a ITPP file
	
	\param filename - name of ITPP file
*/
void wienerfilter::writeResponseMatrix(const string &filename)
{
   if(filename.length())			// only if a valid filename was given
	{
		Rfilename=filename;			// Update Rfilename attribute
	
		it_file Rfile(filename);	// file stream for R matrix	
		Rfile << Name("R") << R;	// read into wienerfilter class member
		
		Rfile.flush();
		Rfile.close();
	}
	else
		throw "wienerfilter::writePropagatorMatrix invalid filename";
}


/*!
	\brief Read Propagator mtrix from a ITPP file
	
	\param filename - name of ITPP file
	
	\return mat - propagator matrix
*/
void wienerfilter::readPropagatorMatrix(const string &filename)
{
	if(filename.length())			// only if a valid filename was given
	{
		Dfilename=filename;			// Update Dfilename attribute

		it_file Dfile(filename);	// file stream for D matrix	
		Dfile >> Name("D") >> D;	// read into wienerfilter class member

		Dfile.close();
	}
	else
		throw "wienerfilter::readPropagatorMatrix invalid filename";
}


/*!
	\brief Write Propagator matrix from a ITPP file
	
	\param filename - name of ITPP file
*/
void wienerfilter::writePropagatorMatrix(const string &filename)
{
	if(filename.length())			// only if a valid filename was given
	{
		Dfilename=filename;			// Update Dfilename attribute
		
		it_file Dfile(filename);	// file stream for D matrix	
		Dfile << Name("D") << D;	

		Dfile.flush();
		Dfile.close();
	}
	else
		throw "wienerfilter::writePropagatorMatrix invalid filename";
}


/*!
	\brief Read Instrument Fidelity matrix from a ITPP file
	
	\param filename - name of ITPP file
	
	\return mat - instrumental fidelity matrix
*/
void wienerfilter::readInstrumentFidelityMatrix(const string &filename)
{
	if(filename.length())			// only if a valid filename was given
	{
		Qfilename=filename;			// Update Qfilename attribute

		it_file Qfile(filename);	// file stream for Q matrix	
		Qfile >> Name("Q") >> Q;

		Qfile.close();
	}
	else
		throw "wienerfilter::readInstrumentFidelityMatrix invalid filename";
}


/*!
	\brief Write Instrumental Fidelity matrix from a ITPP file
	
	\param filename - name of ITPP file
*/
void wienerfilter::writeInstrumentFidelityMatrix(const string &filename)
{
	if(filename.length())		// only if a valid filename was given
	{
		Qfilename=filename;			// Update Qfilename attribute	

		it_file Qfile(filename);		
		Qfile << Name("Q") << Q;				

		Qfile.flush();
		Qfile.close();
	}
	else
	{
		throw "wienerfilter::writeInstrumentFidelityMatrix invalid filename";
	}
}
#endif


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

#ifdef _ITPP_
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
#endif
	

/*!
  \brief Write out a real STL vector and a complex STL vector in ASCII format
  
  \param v1 - real STL vector to write out
  \param v2 - real STL vector to write out
  \param filename - filename of ASCII text file to write
*/
void wienerfilter:: writeASCII(const vector<double> &v1, const vector<double> &v2, const std::string &filename)
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
void wienerfilter:: writeASCII(const vector<double> &v, const vector<complex<double> > &cv, const std::string &filename)
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


#ifdef _ITPP_
/*!
  \brief Write out a complex vector in ASCII format
  
  \param v - complex vector to write out
  \param filename - filename of ASCII text file to write
*/
void wienerfilter::writeASCII(const cvec &v, const string &filename)
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
void wienerfilter::writeASCII(const vector<double> &coord, const cvec &v, const string &filename)
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
void wienerfilter::writeASCII(const vec &coord, const vec &v, const string &filename)
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
void wienerfilter::writeASCII(const vec &coord, const cvec &v, const string &filename)
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
#endif


/*!
  \brief Write out a complex matrix in ASCII format
  
  \param M - complex matrix to write out
  \param filename - filename of ASCII text file to write
*/
#ifdef _ITPP_
void wienerfilter::writeASCII(const cmat M, const string &filename)
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
#elif _ARMADILLO_
void wienerfilter::writeASCII(const cx_mat M, const string &filename)
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
#endif


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
void wienerfilter::getData(const vector<double> &real, const vector<double> &imag)
{
	if(real.size()==0)
		throw "wienerfilter::getData real vector has length 0";
	if(imag.size()==0)
	   throw "wienerfilter::getData imaginary vector has length 0";
	if(real.size()!=imag.size())
		throw "wienerfilter:getData re and im vector differ in length";
	
	cvec tempcvec;						// temporary cvec vector needed for conversion
	int size=real.size();				// get size from vector parameters
	
	
	d.set_size(size);					// set size initially to size of real vector
	for(int i=0; i<size; i++)
	{
		// This must be possible to be implemented more efficiently!
		tempcvec=to_cvec(real[i], imag[i]);		
		d.ins(i, tempcvec);			// append temporary vector to d
	}
}	


/*!
  \brief Read in a real signal from a file
  
  \param vector - vector to read into (reads only real part)
  \param filename - name of file to read from
*/
#ifdef _ITPP_
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
	
	for(int i=0; i<s.size(); i++)
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
	
	for(int i=0; i<s.length(); i++)
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
	
	while(infile.good())
	{
		infile >> lambdasq >> real >> imag;
		lambdasquareds.push_back(lambdasq);
		
		tempcvec=to_cvec(real, imag);
	}
	
	// Write vectors to wienerfilter class
	d.ins(d.size(), tempcvec);
	
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
		tempcvec=to_cvec(real, imag);				

		if(lambdasquareds.size() > i)		// if there is size left in lambdasquareds and delta_lambda_squareds vectors
		{
			lambdasquareds[i]=lambdasq;
			delta_lambdasquareds[i]=deltalambdasq;
			d.set_subvector(i, tempcvec);		// set i-1 position to tempcvec (conversion currently does not work)		
		}
		else		// otherwise use push back function to append at the end of the vector
		{
		   	lambdasquareds.push_back(lambdasq);
			delta_lambdasquareds.push_back(deltalambdasq);			
			d.ins(d.size(), tempcvec);			// insert tempcvec at end of d
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
		tempcvec=to_cvec(real, imag);				

		if(frequencies.size() > i)		// if there is size left in lambdasquareds and delta_lambda_squareds vectors
		{
			d.set_subvector(i, tempcvec);	// set i-1 position to tempcvec (conversion currently does not work)		
		}
		else		// otherwise use push back function to append at the end of the vector
		{
		   d.ins(d.size(), tempcvec);		// insert tempcvec at end of d
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
		tempcvec=to_cvec(real, imag);				

		if(frequencies.size() > i)				// if there is size left in lambdasquareds and delta_lambda_squareds vectors
		{
			frequencies[i]=freq;
			delta_frequencies[i]=deltafreq;
			d.set_subvector(i, tempcvec);		// set i-1 position to tempcvec (conversion currently does not work)		
		}
		else		// otherwise use push back function to append at the end of the vector
		{
		   frequencies.push_back(freq);
		   delta_frequencies.push_back(deltafreq);			
		   d.ins(d.size(), tempcvec);			// insert tempcvec at end of d
		}

		i++;											// increment index into data vector
	}
	
	// For some reason the push_back creates an empty element both in
	// lambdasquareds and delta_lambda_squareds which has to be popped_back (removed)
	//frequencies.pop_back();
	//delta_frequencies.pop_back();
	
	infile.close();
}
#elif _ARMADILLO_
	
	// TODO with Armadillo-implementation
	//void wienerfilter::getData(const vector<double> &real, const vector<double> &imag)	
	//void wienerfilter::readSignalFromFile(const std::string &filename)
	//void wienerfilter::exportSignal(vector<complex<double> > &cmplx)
	//void wienerfilter::exportSignal(vector<double> &re, vector<double> &im)	
	//void wienerfilter::readLambdaSqAndDeltaLambdaSqFromFile(const std::string &filename)	
	//void wienerfilter::readSimDataFromFile(const std::string &filename)
	//void wienerfilter::read4SimDataFromFile(const std::string &filename)	
	//void wienerfilter::readDataFromFile(const std::string &filename)
	//void wienerfilter::read4SimFreqDataFromFile(const std::string &filename)
#endif


/*!
 \brief Read Faraday depths from a file
 
 \param filename - name of text file containing Faraday depths
 */
void wienerfilter::readFaradayDepthsFromFile(const std::string &filename)
{
	
	
	
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


#ifdef _ITPP_
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
  int i=0;     // loop variable

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
#elif _ARMADILLO_
	
	// All output functions need to be adjusted to Armadillo...
#endif


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
  for(int row=0; row<R.rows(); row++)
  {
    for(int col=0; col<R.cols(); col++)
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
void wienerfilter::complexPower(const vector<complex<double> > &signal, vector<double> &power)
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
void wienerfilter::complexPower(const cvec &signal, vector<double> &power)
{
   if(signal.size()==0)
      throw "wienerfilter::complexPower signal has size 0";

   if(power.size()!=(unsigned int)signal.size())		// if power vector has not the same length as signal
      power.resize(signal.size());		// resize power vector

   for(unsigned int i=0; i < (unsigned int)signal.size(); i++)
      power[i]=sqrt(signal[i].real()*signal[i].real()+signal[i].imag()*signal[i].imag());
}

}  // end namespace RM
