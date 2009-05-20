/*! Implementation of rm class methods

    Author:		Sven Duscha (sduscha@mpa-garching.mpg.de)
    Date:		18-12-2008
    Last change:	19-05-2009
*/


#include <iostream>				// C++/STL iostream
#include <math.h>				// mathematics library
#include <string.h>


#include <casa/Arrays.h>			// CASA library functions
#include <casa/Arrays/Array.h>
#include <casa/Utilities/DataType.h>
#include <tables/Tables/TiledFileAccess.h>
#include <lattices/Lattices/TiledShape.h>

#include "rm.h"					// rm class declarations

using namespace std;
using namespace casa;		// namespace for functions from casacore

//===============================================================================
//
//  Constructions / Destruction
//
//===============================================================================

/*!
  \brief Declare a new RM cube of size x (right ascension), y (declination), and depth and stepsize in Faraday depth

  \param depth --
  \param stepsize -- 
*/
rm::rm(int depth, double stepsize)
{
	// declare RM cube of desired size and assign output stream
	
	
}

/*!
  \brief rm class destructor
*/

rm::~rm()
{ 
  // Free memory? Currently nothing to be done
  cout << "rm destructor called" << endl;
}

//===============================================================================
//
//  Methods
//
//===============================================================================

/*!
  \brief Convert from frequencies to lambda squared

  \param frequency - Frequency vector 

  \return success - 1 on successful execution, 0 on error
*/
//vector<double>rm::freqToLambdaSq(vector<double> &frequency);
vector<double> rm::freqToLambdaSq(const vector<double> &frequency)
{
  // lambda squared to be calculated, same size as frequency vector
  vector<double> lambda_sq(frequency.size());	

  // constants
  double csq=89875517873681764.0;	// c^2
 
  // Check for data consistency
  if(frequency.size()==0)
  {
    throw "rm::freqToLambdaSq: frequency vector size 0";
  }

  // loop over frequency vector and compute lambda squared values
  for(unsigned int i=0; i<frequency.size(); i++)
  {
     lambda_sq[i]=csq/(frequency[i]*frequency[i]);
     //cout << "frequency = " << frequency[i] << "  lambda^2 = " << lambda_sq[i] << endl;
  }
  
  return lambda_sq;	// return computed lambda squareds
}


/*!
  \brief Convert from lambda squared to frequencies
  
  \param lambda_sq -- 
  \param frequency --
  
  \return succes -- 
*/
vector<double> rm::lambdaSqToFreq(const vector<double> &lambda_sq)
{
    // frequencies to be calculated from lambda squared, same size as lambda squared vector
    vector<double> frequency(lambda_sq.size());		

    // constants
    double csq=89875517873681764.0;	// c^2

    // Check for data consistency
    if(lambda_sq.size()==0)
    {
      throw "rm::lambdaSqToFreq: lambda_sq vector size 0";
    }

    // loop over frequency vector and compute lambda squared values
    for(unsigned int i=0; i<lambda_sq.size(); i++)
    {
      frequency[i]=sqrt(csq/(lambda_sq[i]));
      //cout << "lambda^2 = " << lambda_sq[i] << "  frequency = " << frequency[i]  << endl;
    }
    
    return frequency;	// return computed frequencies
}


/*!
    Unit conversion functions are needed:
    
    Jy/beam, total integrated flux, flux per channel
    need to know delta f of each channel for integration
    RM-Imager: uv data is V^2 (Voltage squared)
*/


/*! Calculate the total intensity integrated over all channels

  \param frequencies --
  \param unit --

  \return totalIntensity --
*/

vector<double> rm::totalIntensity(vector<double> &frequencies, int unit)
{
  vector<double> totalIntensity;

  // integrate over all frequencies in channel
  
  // convert to desired unit (given by string unit)

  return totalIntensity;
}



/*! Calculate the step size delta_lambda_sq for each lambda_squared channel
    
    \param freq_low --
    \param freq_high --
    \param freq
    
    \return delta_lambda_sq --
*/
vector<double> rm::deltaLambdaSq( //vector<double> delta_lambda_sq, 
			const vector<double> &freq_low, 
			const vector<double> &freq_high,
			bool freq)
{
  // vector containing the converted lambda squared bins
  vector<double> delta_lambda_sq;

  // lambda vectors for conversion
  vector<double> lambda_low(freq_high.size()), lambda_high(freq_low.size());


  // check for valid parameters
  if(freq_low.size()==0 || freq_high.size()==0)		// either of the limit vectors is zero
  {
    throw "rm::deltaLambdaSq: freq_low or freq_high vector 0";
  }
  else if(freq_low.size()!=freq_high.size())		// freq_low and freq_high differ in size
  {
    throw "rm::deltaLambdaSq: freq_low and freq_high vector differ in size";
  }
  else
  {
    // if frequencies are given (i.e. bool freq=true, default)
    // convert lower and higher frequencies to lambda squared
    if(freq)
    {
      lambda_low=freqToLambdaSq(freq_high);	// freq_high corresponds to low lambda
      lambda_high=freqToLambdaSq(freq_low);	// freq_low corresponds to high lambda
    }
    else	// if limits were actually already given as lambda squareds just use these
    {
      lambda_low=freq_low;
      lambda_high=freq_high; 
    }

    // compute difference between higher lambda and lower lambda (other way around than frequencies)
    for(unsigned int i=0; i < freq_low.size(); i++)
    {
      delta_lambda_sq[i]=lambda_high[i]-lambda_low[i];	// compute difference
      
      #ifdef _debug
      cout << "delta_lambda_sq[" << i << "] = " << delta_lambda_sq[i] << endl;	// debug output
      #endif
    }

  }

  return delta_lambda_sq;		// return delta lambda squared vector
}


/*! \brief Inverse Fourier Method for calculating the Rotation Measure at a single Faraday depth
    
    The inverse Fourier Transformation is the classical relation between the lambda squared space and Faraday
    space. Input images channel are not necessarily given in lambda squared wavelengths, but in frequency instead. If that is the case, a conversion function between lambda squared and frequency is used.
    
    Normal case (bool freq=true, default): the input vector is a range of complex polarized 
    intensities measured at different frequencies 
    Special case (bool freq=false): the input vector is already given as a range of polarized intensities given at different lambda squareds

    \param phi - Faraday depth to compute RM for
    \param intensity --
    \param frequency --
    \param weights --
    \param delta_freq --
    \param freq --
    
    \return rm - Single RM value computed for Faraday depth phi
*/
complex<double> rm::inverseFourier(double &phi,
				 vector<complex<double> > &intensity,
				 vector<double> &frequency,
				 vector<double> &weights,
				 vector<double> &delta_frequency,
				 bool freq)
{
    vector<double> lambda_squared(frequency.size());
    vector<double> delta_lambda_squared(delta_frequency.size());
    
    complex<double> exp_lambdafactor=0;		// exponential factor in direct FT
    complex<double> rmpolint;			// rm value
    int chan=0; 				// loop variables
    const int numchannels=frequency.size();	// number of frequency channels

    double K=0;					// K factor for RM-synthesis

    if(freq)		// if given as frequency, convert to lambda squareds
    {
      lambda_squared=freqToLambdaSq(frequency);			// convert frequency vector    
      delta_lambda_squared=freqToLambdaSq(delta_frequency);  	// convert delta step vector   
    }
    else
    {
      lambda_squared=frequency;
      delta_lambda_squared=delta_frequency;
    }

    // Compute weighting factor from weights
    for (vector<double>::iterator it = weights.begin(); it!=weights.end(); ++it) 
    {
      cout << *it << endl;
      K+=*it;
    }
    
    if(K!=0)	// Do not do a division by zero!
      K=1/K;	// take inverse


    // compute discrete Fourier sum by iterating over frequency vector
    //
    // P(phi) = K * expfactor * Sum_0^frequency.size() {P(lambda^2)*exp(lambda^2)}
    //
    for(chan=0; chan<numchannels; chan++)
    {
      // compute exponential factor
      exp_lambdafactor=exp(-2*phi*lambda_squared[chan])*delta_frequency[chan];	// exp lambda_squared factor
      rmpolint=rmpolint+(intensity[chan]*exp_lambdafactor);
    }

    rmpolint=K*rmpolint;	// multiply with weighting

    return rmpolint;
}


/*!
    \brief Inverse Fourier Method for calculating the Rotation Measure for a set of Faraday depths

    The inverse Fourier Transformation is the classical relation between the lambda squared space and Faraday
    space. Input images channel are not necessarily given in lambda squared wavelengths, but in frequency instead. If that is the case, a conversion function between lambda squared and frequency is used.
    
    Normal case (bool freq=true, default): the input vector is a range of complex polarized intensities measured at different frequencies 
    Special case (bool freq=false): the input vector is already given as a range of polarized intensities given at different lambda squareds

    \param phi - Faraday depth to compute RM for
    \param intensity - Polarized intensities
    \param frequency - Frequencies (or lambda squareds) of polarized intensities
    \param weights - Weights associated with each frequency (or lambda squared)
    \param delta_freq - Delta frequency (or delta lambda squared) between frequencies
    \param freq - If intensities are given as frequencies (true) or as lambda squared (false)
    
    \return rm - vector of RM values computed for Faraday depths phi
*/
vector<double>rm::inverseFourier(vector<double> &phi,
				 vector<complex<double> > &intensity,
				 vector<double> &frequency,
				 vector<double> &weights,
				 vector<double> &delta_freq,
				 bool freq)
{
  vector<double> lambda_squared(frequency.size());
  vector<double> delta_lambda_squared(delta_freq.size());
  vector<double> rm;



  return rm;
}


/*!
  \brief Wavelet Transform Method for calculating the Rotation Measure

  Normal case (bool freq=true, default): the input vector is a range of complex
  polarized intensities measured at different frequencies. Parameters a_min/a_max
  and b_min/b_max define ranges for wavelet parameter space probing.

  \param phi - Faraday depth to compute RM for
  \param intensity - Polarized intensities
  \param frequency - Frequencies (or lambda squareds) of polarized intensities
  \param weights - Weights associated with each frequency (or lambda squared)
  \param delta_freq - Delta frequency (or delta lambda squared) between frequencies
  \param freq - If intensities are given as frequencies (true) or as lambda squared (false)
  
  \return rm - vector of RM values computed for Faraday depths phi
*/
vector<double> rm::wavelet(vector<double> &phi,
			   vector<complex<double> > &intensity,
			   vector<double> &frequencies,
			   vector<double> &weights,
			   vector<double> &delta_freqs,
			   vector<double> &wavelet_parameters,
			   bool freq)
{
  vector<double>rm;	// rm values along the line of sight
  
  
  return rm;
}


// High-Level access functions to compute RM tiles/cubes

// Wrapper function to perform RM synthesis on a tile of an image:
// A section (or whole) image is taken as input, using the casacore LatticeBase class,
// the algorithm used to compute the RM cube is specified via a function pointer
// also implement multithreading into that?
//

//Lattice<double> rm::rmOnLattice(Lattice<double> imagetile,  algorithm)
//{
//	LatticeBase *lattice;
//
//
//
//	return *lattice;
//}


// Error calculation algorithms

/*! \brief Calculate RM error using LSQ approximation
    
    Input parameters are the vector of polarized intensities, along with a vector describing
    their frequency distribution; if bool freq is set to false, these are assumed to be
    given in lambda squared values
    
    \param intensity --
    \param lambda_sqs --
    \param lambda_sq --
    
    \return rm_error --
*/

vector<double> rm::rmErrorLsq(vector<double> intensity, vector<double> lambda_sqs, bool lambda_sq)
{
	vector<double> rm_error;


	return rm_error;
}


/*! \brief Calculate RM error using Bayesian statistics

    Input parameters are the vector of polarized intensities, along with a vector describing
    their frequency distribution; if bool freq is set to false, these are assumed to be
    given in lambda squared values
    
    \param intensity --
    \param lambda_sqs --
    \param lambda_sq --
    
    \return rm_error --
*/

vector<double> rm::rmErrorBayes(vector<double> intensity, vector<double> lambda_sqs, bool lambda_sq)
{
	vector<double> rm_error;


	return rm_error;
}
