/*! Implementation of rm class methods

    Author:		Sven Duscha (sduscha@mpa-garching.mpg.de)
    Date:		18-12-2008
    Last change:	24-06-2009
*/


#include <iostream>				// C++/STL iostream
#include <fstream>				// file stream I/O
#include <math.h>				// mathematics library
#include <string.h>
#include <assert.h>

/*
#include <casa/Arrays.h>			// CASA library functions
#include <casa/Arrays/Array.h>
#include <casa/Utilities/DataType.h>
#include <tables/Tables/TiledFileAccess.h>
#include <lattices/Lattices/TiledShape.h>
*/
#include "rm.h"					// rm class declarations

using namespace std;

//===============================================================================
//
//  Constructions / Destruction
//
//===============================================================================

/*!
  \brief Default constructor
*/
rm::rm()
{
  // do nothing in particular
}


/*!
  \brief Declare a new RM cube of size x (right ascension), y (declination), and depth and stepsize in Faraday depth

  \param depth - total Faraday depth size 
  \param stepsize -- 
*/
rm::rm(int depth, double stepsize)
{
  // NOTE: This is now in rmCube class, soon depricated

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

  \param frequency - frequency vector 

  \return lambda_sq - converted lambda squared vector
*/
//vector<double>rm::freqToLambdaSq(vector<double> &frequency);
vector<double> rm::freqToLambdaSq(const vector<double> &frequency)
{
  // lambda squared to be calculated, same size as frequency vector
  vector<double> lambda_sq(frequency.size());	

  // constants
  const double csq=89875517873681764.0;	// c^2
 
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
  
  \param lambda_sq - lambda squared vector
  \return frequency - converted frequency vector
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

  \param frequencies - polarized intensities
  \param unit - unit of emission

  \return totalIntensity - calculated integrated total intensity
*/

vector<double> rm::totalIntensity(const vector<double> &intensities, int unit)
{
  vector<double> totalIntensity;

  // integrate over all frequencies in channel
  
  // convert to desired unit (given by string unit)

  return totalIntensity;
}



/*! Calculate the step size delta_lambda_sq for each lambda_squared channel
    
    \param freq_low - lower limit frequencies
    \param freq_high - upper limit frequencies
    \param freq - bool if vectors are given in frequency (=true) or in lambda squared
    
    \return delta_lambda_sq - computed delta lambda squared vector
*/
vector<double> rm::deltaLambdaSq( //vector<double> delta_lambda_sq, 
			const vector<double> &freq_low, 
			const vector<double> &freq_high,
			bool freq)
{
  // vector containing the converted lambda squared bins
  vector<double> delta_lambda_sq(freq_high.size());

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



/*!
	\brief Compute delta lambda squareds and lambda squared centres from image frequencies using a
	top hat bandpass assumption
	
	\param &frequencies - vector with imaged frequencies
	\param &bandwidths - delta frequencies
	\param &lambda_centre_squareds - vector for computed lambda squared centre frequencies
*/
vector<double> rm::deltaLambdaSqTopHat( const vector<double> &frequencies,
												const vector<double> &bandwidths,
												vector<double> &lambda_centre_squareds)
{
	// constants
	double csq=89875517873681764.0;				// c^2
	// temporary variables
	double frequencyquotient=0;					// temporary variable for frequency quotient
	
	vector<double> delta_lambda_squareds;		// delta lambda squared vector to be returned
	
	for(unsigned int i=0; i<frequencies.size(); i++)	// compute for all frequencies in vector
	{
		// lambda centre squareds
		frequencyquotient=bandwidths[i]/frequencies[i];
		lambda_centre_squareds[i]=csq/(pow(frequencies[i],2))*(1+0.75*pow(frequencyquotient, 2));
	
		// delta lambda squareds
		delta_lambda_squareds[i]=2*csq*bandwidths[i]/pow(frequencies[i], 3)*(1+0.5*pow(frequencyquotient, 2));
	}	
	
	return delta_lambda_squareds;
}


/*!
	\brief Compute the weighted average of the observed lambda squareds
	
	\param &lambda_squareds - vector of observed lambda squareds
	\param &weights - weighting vector for observed lambda squareds
	
	\return lambdaZero - weighted average of the observed lambda squareds
*/
double rm::weightedLambdaZero(vector<double> &lambda_squareds, vector<double> &weights)
{
	double lambdaZero=0;
	double sumWeights=0;
	double sumWeightedLambdas=0;
	
	//**********************************
	if(lambda_squareds.size()==0)			// if lambda_squareds vector has zero length
	{
		throw "rm::weightedLambdaZero lambda_squareds has 0 size";
	}
	if(weights.size()==0 || weights.size()!=lambda_squareds.size())	// if weights vector has wrong length
	{
		throw "rm::weightedLambdaZero weights has invalid size";
	}
	
	// lambdaZero=Sum(weights[i]*lambda_squareds[i]/weights[i]);
	for(unsigned int i=0; i<weights.size(); i++)		// loop over all lambda_squareds
	{
		sumWeights += weights[i]*lambda_squareds[i];
		sumWeightedLambdas += weights[i];
	}
	
	if(sumWeights==0)
	{
		throw "rm::weightedLambdaZero sum of weights is 0";
	}
	else
	{
		lambdaZero=sumWeightedLambdas/sumWeights;
	}
	
	return lambdaZero;
}


/*! 
	\brief Inverse Fourier Method for calculating the Rotation Measure at a single Faraday depth
    
    The inverse Fourier Transformation is the classical relation between the lambda squared space and Faraday
    space. Input images channel are not necessarily given in lambda squared wavelengths, but in frequency instead. 
	 If that is the case, a conversion function between lambda squared and frequency should be first used.
    
    \param phi - Faraday depth to compute RM for
    \param intensity - polarized intensities per channel
    \param lambda_squared - corresponding frequencies of channels
    \param weights - weigths for each channel
    \param delta_lambda_squared - delta lambda squared between channels
	 \param lambdaZero - wavelength to derotate polarization vectors to, default 0
    
    \return rm - Single RM value computed for Faraday depth phi
*/
complex<double> rm::inverseFourier(double phi,
				 const vector<complex<double> > &intensity,
				 const vector<double> &lambda_squared,
				 const vector<double> &weights,
				 const vector<double> &delta_lambda_squared,
				 const double lambdaZero)
{
//    vector<double> lambda_squared(frequency.size());
//    vector<double> delta_lambda_squared(delta_frequency.size());
    
    complex<double> exp_lambdafactor=0;				// exponential factor in direct FT
    complex<double> rmpolint;								// rm value
	 double lambdaZeroSq=0;									// derotating lambdaZeroSquared
    int chan=0; 												// loop variables
    const int numchannels=lambda_squared.size();	// number of lambda squared channels

    double K=0;					// K factor for RM-synthesis


    //-----------------------------------------------------------------------

    // Compute weighting factor from weights
    for (vector<double>::const_iterator it = weights.begin(); it!=weights.end(); ++it) 
    {
      K+=*it;
    }
    
    if(K!=0)	// Do not do a division by zero!
      K=1/K;	// take inverse
    else	// otherwise...
      K=1;	// ... use 1 as default
      
	 // If a derotating lambda Zero != 0 was given compute lambdaZeroSq
	 if(lambdaZero)
		lambdaZeroSq=lambdaZero*lambdaZero;

    // compute discrete Fourier sum by iterating over frequency vector
    //
    // P(phi) = K * expfactor * Sum_0^frequency.size() {P(lambda^2)*exp(lambda^2)}
    //
    for(chan=0; chan<numchannels; chan++)
    {
		// Use Euler formula for exp_lambdafactor
 		exp_lambdafactor=complex<double>(cos(-2.0*phi*(lambda_squared[chan]-lambdaZeroSq)), sin(-2.0*phi*(lambda_squared[chan]-lambdaZeroSq)));	
      // compute complex exponential factor
//      exp_lambdafactor=exp(complex<double>(0, -2*phi)*lambda_squared[chan]);
      rmpolint=rmpolint+(intensity[chan]*exp_lambdafactor)*delta_lambda_squared[chan];
    }

    rmpolint=K*rmpolint;	// multiply with weighting

    return rmpolint;		// return complex polarized intensities per Faraday depth
}


/*!
    \brief Inverse Fourier Method for calculating the Rotation Measure for a set of Faraday depths

    The inverse Fourier Transformation is the classical relation between the lambda squared space and Faraday
    space. Input images channel are not necessarily given in lambda squared wavelengths, but in frequency instead. 
	 If that is the case, a conversion function between lambda squared and frequency should be used first.

 	 If lambdaZero is given as an optional parameter (default 0) the polarization vectors are derotated to that
    wavelength.

    \param &phis - Faraday depth to compute RM for
    \param &intensity - Polarized intensities
    \param &lambda_squared - Lambda squareds of polarized intensities
    \param &weights - Weights associated with each frequency (or lambda squared)
    \param &delta_lambda_squared - Delta lambda squared distance between intensities
	 \param lambdaZero - lambda zero wavelength to derotate polarization vector to, default=0
    
    \return rm - vector of RM values computed for Faraday depths phi
*/
vector<complex<double> > rm::inverseFourier(const vector<double> &phis,
				 const vector<complex<double> > &intensity,
				 const vector<double> &lambda_squared,
				 const vector<double> &weights,
				 const vector<double> &delta_lambda_squared,
				 const double lambdaZero)
{
  vector<complex<double> > rmpolint(phis.size());		// polarized RM intensities per Faraday depth

  complex<double> exp_lambdafactor=0;						// exponential factor in direct FT
  double lambdaZeroSq=0;										// derotating lambdaZeroSquared
  double phi=0;													// single phi value to be computed
  const unsigned int numchannels=lambda_squared.size();			// number of frequency channels

  double K=0;														// K factor for RM-synthesis

  //-----------------------------------------------------------------------

  // Compute weighting factor from weights
  for (vector<double>::const_iterator it = weights.begin(); it!=weights.end(); ++it) 
  {
    K+=*it;
  }

  if(K!=0)		// Do not do a division by zero!
    K=1/K;		// take inverse
  else			// otherwise...
    K=1;			// ... use 1 as default    

  // If a derotating lambda Zero != Zero was given compute lambdaZeroSq
  if(lambdaZero)
     lambdaZeroSq=lambdaZero*lambdaZero;

  // compute discrete Fourier sum by iterating over frequency vector
  //
  // P(phi) = K * expfactor * Sum_0^frequency.size() {P(lambda^2)*exp(-2*i*phi*lambda^2)}
  //
  for(unsigned int i=0; i<phis.size(); i++)	// loop over Faraday depths given in phis vector 
  {
     for(unsigned int chan=0; chan<numchannels; chan++)
  	  {
		  phi=phis[i];				// select phi from Faraday depths vector
        // Use Euler formula for exp_lambdafactor
   // 	  ex_lambdafactor=(cos(-2*phi*(lambda_squared[chan]-lambdaZeroSq)), sin(-2*phi*(lambda_squared[chan]-lambdaZeroSq)));		// BUGGY! casa error?
		  exp_lambdafactor=complex<double>(cos(-2.0*phi*(lambda_squared[chan]-lambdaZeroSq)), sin(-2.0*phi*(lambda_squared[chan]-lambdaZeroSq)) );  
      rmpolint[i]=rmpolint[i]+(intensity[chan]*exp_lambdafactor*delta_lambda_squared[chan]);
     }

     rmpolint[i]=K*rmpolint[i];	// multiply with weighting
  }

  return rmpolint;	// return vector of complex polarized intensities per Faraday depth
}


/*! 
	\brief Single Polarization Inverse Fourier Method for calculating the Rotation Measure at a single Faraday depth
    
    The inverse Fourier Transformation is the classical relation between the lambda squared space and Faraday
    space. Input images channel are not necessarily given in lambda squared wavelengths, but in frequency instead. 
	 If that is the case, a conversion function between lambda squared and frequency should be first used.

    This method takes only a real intensity (Q or U) and computes the RM only for that Q and U.

    \param phi - Faraday depth to compute RM for
    \param intensity - intensity per channel for one polarization only
    \param lambda_squared - corresponding frequencies of channels
    \param weights - weigths for each channel
    \param delta_lambda_squared - delta lambda squared between channels
	 \param lambdaZero - wavelength to derotate polarization vectors to, default 0
    
    \return rm - Single RM value computed for Faraday depth phi
*/
complex<double> rm::inverseFourier(double phi,
				 const vector<double> &intensity,
				 const vector<double> &lambda_squared,
				 const vector<double> &weights,
				 const vector<double> &delta_lambda_squared,
				 const double lambdaZero)
{
//    vector<double> lambda_squared(frequency.size());
//    vector<double> delta_lambda_squared(delta_frequency.size());
    
    complex<double> exp_lambdafactor=0;				// exponential factor in direct FT
    complex<double> rmpolint;								// rm value
	 double lambdaZeroSq=0;									// derotating lambdaZeroSquared
    int chan=0; 												// loop variables
    const int numchannels=lambda_squared.size();	// number of lambda squared channels

    double K=0;					// K factor for RM-synthesis


    //-----------------------------------------------------------------------

    // Compute weighting factor from weights
    for (vector<double>::const_iterator it = weights.begin(); it!=weights.end(); ++it) 
    {
      K+=*it;
    }
    
    if(K!=0)	// Do not do a division by zero!
      K=1/K;	// take inverse
    else	// otherwise...
      K=1;	// ... use 1 as default
      
	 // If a derotating lambda Zero != 0 was given compute lambdaZeroSq
	 if(lambdaZero)
		lambdaZeroSq=lambdaZero*lambdaZero;

    // compute discrete Fourier sum by iterating over frequency vector
    //
    // P(phi) = K * expfactor * Sum_0^frequency.size() {P(lambda^2)*exp(lambda^2)}
    //
    for(chan=0; chan<numchannels; chan++)
    {
		// Use Euler formula for exp_lambdafactor
 		exp_lambdafactor=complex<double>(cos(-2.0*phi*(lambda_squared[chan]-lambdaZeroSq)), sin(-2.0*phi*(lambda_squared[chan]-lambdaZeroSq)));	
      // compute complex exponential factor
//      exp_lambdafactor=exp(complex<double>(0, -2*phi)*lambda_squared[chan]);
      rmpolint=rmpolint+(intensity[chan]*exp_lambdafactor)*delta_lambda_squared[chan];
    }

    rmpolint=K*rmpolint;	// multiply with weighting

    return rmpolint;		// return complex polarized intensities per Faraday depth
}



/*!
    \brief Inverse Fourier Method for calculating the Rotation Measure for a set of Faraday depths

    The inverse Fourier Transformation is the classical relation between the lambda squared space and Faraday
    space. Input images channel are not necessarily given in lambda squared wavelengths, but in frequency instead. 
	 If that is the case, a conversion function between lambda squared and frequency should be used first.

 	 If lambdaZero is given as an optional parameter (default 0) the polarization vectors are derotated to that
    wavelength.

	 This function computes the RM for one polarization (Q or U) only.

    \param &phis - Faraday depth to compute RM for
    \param &intensity - intensities in one single polarization
    \param &lambda_squared - Lambda squareds of polarized intensities
    \param &weights - Weights associated with each frequency (or lambda squared)
    \param &delta_lambda_squared - Delta lambda squared distance between intensities
	 \param lambdaZero - lambda zero wavelength to derotate polarization vector to, default=0
    
    \return rm - vector of RM values computed for Faraday depths phi
*/
vector<complex<double> > rm::inverseFourier(const vector<double> &phis,
				 const vector<double> &intensity,
				 const vector<double> &lambda_squared,
				 const vector<double> &weights,
				 const vector<double> &delta_lambda_squared,
				 const double lambdaZero)
{
  vector<complex<double> > rmpolint(phis.size());		// polarized RM intensities per Faraday depth

  complex<double> exp_lambdafactor=0;						// exponential factor in direct FT
  double lambdaZeroSq=0;										// derotating lambdaZeroSquared
  double phi=0;													// single phi value to be computed
  const unsigned int numchannels=lambda_squared.size();			// number of frequency channels

  double K=0;														// K factor for RM-synthesis

  //-----------------------------------------------------------------------

  // Compute weighting factor from weights
  for (vector<double>::const_iterator it = weights.begin(); it!=weights.end(); ++it) 
  {
    K+=*it;
  }

  if(K!=0)		// Do not do a division by zero!
    K=1/K;		// take inverse
  else			// otherwise...
    K=1;			// ... use 1 as default    

  // If a derotating lambda Zero != Zero was given compute lambdaZeroSq
  if(lambdaZero)
     lambdaZeroSq=lambdaZero*lambdaZero;

  // compute discrete Fourier sum by iterating over frequency vector
  //
  // P(phi) = K * expfactor * Sum_0^frequency.size() {P(lambda^2)*exp(-2*i*phi*lambda^2)}
  //
  for(unsigned int i=0; i<phis.size(); i++)	// loop over Faraday depths given in phis vector 
  {
     for(unsigned int chan=0; chan<numchannels; chan++)
  	  {
		  phi=phis[i];				// select phi from Faraday depths vector
        // Use Euler formula for exp_lambdafactor
   // 	  ex_lambdafactor=(cos(-2*phi*(lambda_squared[chan]-lambdaZeroSq)), sin(-2*phi*(lambda_squared[chan]-lambdaZeroSq)));		// BUGGY! casa error?
		  exp_lambdafactor=complex<double>(cos(-2.0*phi*(lambda_squared[chan]-lambdaZeroSq)), sin(-2.0*phi*(lambda_squared[chan]-lambdaZeroSq)) );  
      rmpolint[i]=rmpolint[i]+(intensity[chan]*exp_lambdafactor*delta_lambda_squared[chan]);
     }

     rmpolint[i]=K*rmpolint[i];	// multiply with weighting
  }

  return rmpolint;	// return vector of complex polarized intensities per Faraday depth
}



/*!
	 \brief Forward Fourier Transform - inverse operation to RM-Synthesis

    \param &freqs - Faraday depth to compute RM for
    \param &rmpolint - Polarized intensities for each Faraday depth
    \param &lambda_squared - Lambda squareds of polarized intensities
    \param &weights - Weights associated with each frequency (or lambda squared)
    \param &delta_phis - Delta phi distance between intensities in Faraday space
    
    \return intensities - vector of polarized intensity computed over vector freqs
*/
vector<complex<double> > rm::forwardFourier(const vector<double> &lambda_sqs,
				 const vector<complex<double> > &rmpolint,
				 const vector<double> &faradays,
				 const vector<double> &weights,
				 const vector<double> &delta_faradays,
				 const double lambdaZero)
{
  vector<complex<double> > intensities(lambda_sqs.size());	// polarized intensities for each frequency

  complex<double> exp_lambdafactor=0;								// exponential factor in direct FT
  double lambdasq=0;														// single lambda squared value to be computed
  const unsigned int numfaradays=faradays.size();				// number of Faraday depths
  const unsigned int numlambda_sqs=lambda_sqs.size();			// number of frequency channels to transform to

  //-----------------------------------------------------------------------

  // compute discrete Fourier sum by iterating over frequency vector
  //
  // P(phi) = K * expfactor * Sum_0^frequency.size() {P(lambda^2)*exp(-2*i*phi*lambda^2)}
  //
  for(unsigned int i=0; i<numlambda_sqs; i++)	// loop over lambda squareds given in phis vector 
  {
     for(unsigned int chan=0; chan<numfaradays; chan++)
  	  {
		  lambdasq=lambda_sqs[i];				// select lambda squared from lambda squareds vector
        // Use Euler formula for exp_lambdafactor
 		  exp_lambdafactor=complex<double>(cos(+2.0*lambdasq*faradays[chan]), sin(+2.0*lambdasq*faradays[chan]) );  
		  intensities[i]=intensities[i]+(rmpolint[chan]*exp_lambdafactor*delta_faradays[chan]);
     }
  }

  return intensities;	// return vector of complex polarized intensities per Faraday depth
}



/*!
	 \brief Q polarization only Forward Fourier Transform - inverse operation to RM-Synthesis

    \param &freqs - Faraday depth to compute RM for
    \param &rmpolint - Polarized intensities for each Faraday depth
    \param &lambda_squared - Lambda squareds of polarized intensities
    \param &weights - Weights associated with each frequency (or lambda squared)
    \param &delta_phis - Delta phi distance between intensities in Faraday space
    
    \return Qintensities - vector Q of polarized intensity computed over vector freqs
*/
vector<double> rm::forwardFourierQ(const vector<double> &lambda_sqs,
				 const vector<double> &rmpolint,
				 const vector<double> &faradays,
				 const vector<double> &weights,
				 const vector<double> &delta_faradays,
				 const double lambdaZero)
{
  vector<double> Qintensities(lambda_sqs.size());				// Q polarized intensities for each frequency

  double exp_lambdafactor=0;											// exponential factor in direct FT
  double lambdasq=0;														// single lambda squared value to be computed
  const unsigned int numfaradays=faradays.size();				// number of Faraday depths
  const unsigned int numlambda_sqs=lambda_sqs.size();			// number of frequency channels to transform to

  //-----------------------------------------------------------------------

  // compute discrete Fourier sum by iterating over frequency vector
  //
  // P(phi) = K * expfactor * Sum_0^frequency.size() {P(lambda^2)*exp(-2*i*phi*lambda^2)}
  //
  for(unsigned int i=0; i<numlambda_sqs; i++)	// loop over lambda squareds given in phis vector 
  {
     for(unsigned int chan=0; chan<numfaradays; chan++)
  	  {
		  lambdasq=lambda_sqs[i];				// select lambda squared from lambda squareds vector
        // Use Euler formula for exp_lambdafactor
 		  exp_lambdafactor=cos(+2.0*lambdasq*faradays[chan]);  
		  Qintensities[i]=Qintensities[i]+(rmpolint[chan]*exp_lambdafactor*delta_faradays[chan]);
     }
  }

  return Qintensities;	// return vector of complex polarized intensities per Faraday depth
}


/*!
	 \brief U polarization only Forward Fourier Transform - inverse operation to RM-Synthesis

    \param &freqs - Faraday depth to compute RM for
    \param &rmpolint - Polarized intensities for each Faraday depth
    \param &lambda_squared - Lambda squareds of polarized intensities
    \param &weights - Weights associated with each frequency (or lambda squared)
    \param &delta_phis - Delta phi distance between intensities in Faraday space
    
    \return Qintensities - vector U of polarized intensity computed over vector freqs
*/
vector<double> rm::forwardFourierU(const vector<double> &lambda_sqs,
				 const vector<double> &rmpolint,
				 const vector<double> &faradays,
				 const vector<double> &weights,
				 const vector<double> &delta_faradays,
				 const double lambdaZero)
{
  vector<double> Uintensities(lambda_sqs.size());				// Q polarized intensities for each frequency

  double exp_lambdafactor=0;											// exponential factor in direct FT
  double lambdasq=0;														// single lambda squared value to be computed
  const unsigned int numfaradays=faradays.size();				// number of Faraday depths
  const unsigned int numlambda_sqs=lambda_sqs.size();			// number of frequency channels to transform to

  //-----------------------------------------------------------------------

  // compute discrete Fourier sum by iterating over frequency vector
  //
  // P(phi) = K * expfactor * Sum_0^frequency.size() {P(lambda^2)*exp(-2*i*phi*lambda^2)}
  //
  for(unsigned int i=0; i<numlambda_sqs; i++)	// loop over lambda squareds given in phis vector 
  {
     for(unsigned int chan=0; chan<numfaradays; chan++)
  	  {
		  lambdasq=lambda_sqs[i];				// select lambda squared from lambda squareds vector
        // Use Euler formula for exp_lambdafactor
 		  exp_lambdafactor=sin(+2.0*lambdasq*faradays[chan]);  
		  Uintensities[i]=Uintensities[i]+(rmpolint[chan]*exp_lambdafactor*delta_faradays[chan]);
     }
  }

  return Uintensities;	// return vector of complex polarized intensities per Faraday depth
}



/*!
	\brief Perform RM clean on a line-of-sight Q or U only vector down to threshold
	
	\param rm - line-of-sight vector with intensity in Q or U
	\param threshold - threshold at which RM clean iterations stop
	
	\return int - number of clean iterations performed
*/
int rm::RMClean(vector<double> &rm, double threshold)
{
	int numiterations=0;
	
	
	return numiterations;
}


/*!
	\brief Perform RM clean on a line-of-sight P=Q + iU vector down to threshold
	
	\param rm - line-of-sight vector with complex polarized intensity P
	\param threshold - threshold at which RM clean iterations stop (same for Q and U)
	
	\return int - number of clean iterations performed
*/
int rm::RMClean(complex<vector<double> > &complxrm, double threshold)
{
	int numiterations=0;
	
	
	return numiterations;	
}


/*!
	\brief Perform RM clean on a line-of-sight P=Q + iU vector down to complex threshold
	
	\param rm - line-of-sight vector with complex polarized intensity P
	\param threshold - threshold at which RM clean iterations stop (complex threshold for Q and U separately)
	
	\return int - number of clean iterations performed
*/
int rm::RMClean(complex<vector<double> > &complxrm, complex<double> threshold)
{
	int numiterations=0;
	
	
	return numiterations;	
}


/*!
  \brief Compute the Rotation Measure Spread Function

  The Rotation Measure Spread Function is a measure for the quality of the observation
  to distinguish Faraday structures, similar to the PSF in conventional astronomy. In
  the rmclean function the calculated Rotation Measure is deconvoluted with the RMSF function. 

  \param &phis - Faraday depth range over which the RMSF is computed
  \param &lambda_squareds -  Lambda squareds of polarized intensities
  \param &weights - Weights associated with each frequency (or lambda squared)
  \param &delta_lambda_squared - Delta frequency (or delta lambda squared) between frequencies
  \param lambdaZero - lambda zero wavelength to derotate polarization vector to, default=0

  \return RMSF - vector with RMSF over range as specified
*/
vector<complex<double> > rm::RMSF(const vector<double> &phis,
				  const vector<double> &lambda_squared,
				  const vector<double> &weights,
				  const vector<double> &delta_lambda_squared,
				  const double lambdaZero)
{
  vector<complex<double> > rmsfvec(phis.size());					// calculated rmsf ATTENTION: rmCube has its own rmsf attribute
  vector<double> lambda_squared_pi(lambda_squared.size());  	// scale lambda squareds to 0 to 2*Pi
  unsigned int numchannels=lambda_squared.size();					// number of channels

  complex<double> exp_factor;							// complex exponential factor
  unsigned int weightssize=weights.size();		// size of weights vector
  unsigned int phissize=phis.size();				// size of phis vector

  //**************************************************
  // Consistency check of input data
  if(phis.size()==0)
     throw "rm::RMSF phis vector has length 0";
  if(lambda_squared.size()==0)
     throw "rm::RMSF lambda_squared vector has length 0";
  if(weights.size()==0)
     throw "rm::RMSF";
  if(delta_lambda_squared.size()==0)
	  throw "rm::RMSF delta_lambda_squared vector has length 0";
  // Doesn't check for all combinations of equality...
  if(phis.size()!=lambda_squared.size() || weights.size()!=delta_lambda_squared.size())
     throw "rm::RMSF input vectors differ in length";


  //***********************************************************
 // rmsf.resize(phissize);		// set rmsf length to that of phis

  // IMPORTANT: Map frequencies/lamdab to 2*Pi/N?
  for(unsigned int i=0; i<numchannels; i++)
  {
	lambda_squared_pi[i]=lambda_squared[i]*M_PI/numchannels;
  }	

  for(unsigned int iphi=0; iphi < phissize; iphi++)	// loop over all Faraday depths
  {
    // Since weights and lambda_sqs do correspond to each other and have the
    // same length, we can use iweight to index lambda_sqs and delta_lambda_sqs, too
    // Loop over all weights and compute Riemann's integral
    for(unsigned int iweight=0; iweight < weightssize; iweight++)
    {
   	 exp_factor=complex<double>(cos(-2*phis[iphi]*lambda_squared_pi[iweight]), sin(-2*phis[iphi]*lambda_squared_pi[iweight]));
	    rmsfvec[iphi]=rmsfvec[iphi] + weights[iweight] * exp_factor * delta_lambda_squared[iweight];					 
    }
  }
  
  return rmsfvec;		// return vector with calculated rmsf
}


/*!
  \brief Compute the Rotation Measure Spread Function from observed frequencies

  The Rotation Measure Spread Function is a measure for the quality of the observation
  to distinguish Faraday structures, similar to the PSF in conventional astronomy. 
  In the rmclean function the calculated Rotation Measure is deconvoluted with the RMSF function. 

  \param &phis - Faraday depth range over which the RMSF is computed
  \param &frequencies -  frequencies of polarized intensities
  \param &weights - Weights associated with each frequency (or lambda squared)
  \param &delta_frequencies - Delta frequency between frequencies
  \param freqZero - frequency zero wavelength to derotate polarization vector to, default=0

  \return RMSF - vector with RMSF over range as specified
*/
vector<complex<double> >rm::RMSFfreq(const vector<double> &phis,
				  const vector<double> &frequencies,
				  const vector<double> &weights,
				  const vector<double> &delta_frequencies,
				  const double freqZero)
{
  vector<complex<double> > rmsfvec(phis.size());				// calculated rmsf to be returned
  vector<double> lambda_sqs(frequencies.size());
  vector<double> delta_lambda_sqs(frequencies.size());
  double lambdaZero=0;						// lambdaZero to rotate polarized vector to (converted from freqZero)
  double lambdaZeroSq=0;					// derotated lambda squared
  vector<double> tempFreqZero(1);		// temporary frequency vector needed to convert single frequency (only implemented for vectors)

  unsigned int iphi=0, iweight=0;		// loop variables

  //************************************************
  lambda_sqs=freqToLambdaSq(frequencies);	// convert frequencies to lambda squareds
  delta_lambda_sqs=freqToLambdaSq(delta_frequencies);
  tempFreqZero[0]=freqZero;		// write derotating frequency into single temporary vector
  lambdaZero=freqToLambdaSq(tempFreqZero)[0];


  if(lambdaZero)
     lambdaZeroSq=lambdaZero*lambdaZero;

  for(iphi=0; iphi < phis.size(); iphi++)	// loop over all Faraday depths
  {
    // Since weights and lambda_sqs do correspond to each other and have the
    // same length, we can use iweight to index lambda_sqs and delta_lambda_sqs, too
    // Loop over all weights and compute Riemann's integral
    for(iweight=0; iweight < weights.size(); iweight++)
    {
      rmsfvec[iphi]=rmsfvec[iphi] + weights[iweight] * 
      exp(complex<double>(0,-2*phis[iphi]*(lambda_sqs[iweight]-lambdaZeroSq))) * delta_lambda_sqs[iweight];					 
    }
  }
  
  return rmsfvec;	// return vector with calculated rmsf
}


/*!
  \brief Wavelet Transform Method for calculating the Rotation Measure

  Normal case (bool freq=true, default): the input vector is a range of complex
  polarized intensities measured at different frequencies. Parameters a_min/a_max
  and b_min/b_max define ranges for wavelet parameter space probing.

  \param phi - Faraday depth to compute RM for
  \param intensity - Polarized intensities
  \param lambda_squared - Lambda squareds of polarized intensities
  \param weights - Weights associated with each frequency (or lambda squared)
  \param delta_lambda_squared - Delta lambda squared) between frequencies
  \param wavelet_parameters - vector with additional wavelet parameters (experimental)
  
  \return rm - vector of RM values computed for Faraday depths phi
*/
vector<double> rm::wavelet(vector<double> &phi,
			   vector<complex<double> > &intensity,
			   vector<double> &lambda_squared,
			   vector<double> &weights,
			   vector<double> &delta_lambda_squared,
			   vector<double> &wavelet_parameters)
{
  vector<double>rm;	// rm values along the line of sight
  
  
  return rm;
}


//********************************************************
//
//	Error estimation algorithms
//
//********************************************************


/*!
	\brief Compute lambda squared variance, used in rmErrorLsq function
	
	\param lambdaSq - vector with lambda squareds used in observation
	
	\return lambdaSqVariance - variance of lambda squared distribution
*/
double rm::lambdaSqVariance(vector<double> &lambdaSq)
{
	double templambdaSqVariance=0;		// variance of lambda squared distribution
	unsigned int N=lambdaSq.size();		// number of lambda squareds
	
	if(N==0)
		throw "rm::lambdaSqVariance lambdaSq has length 0";
	
	for(unsigned int i=0; i<N; i++)
	{	
		templambdaSqVariance = templambdaSqVariance + (1/(N-1)*(pow(lambdaSq[i], 2) - 1/N*pow(lambdaSq[i],2) ));
	}
	
	return templambdaSqVariance;
}


/*!
	\brief Calculate the RM error with the least squared fit error estimation
	
	\param rmsnoisechan - rms noise per channel
	\param lambdaSqVariance - variance of lambda squared distribution
	\param Nchan - number of channels in observation
	\param P - total polarized intensity integrated over all channels
	
	\return rmerror - error in polarized RM
*/
double rm::rmErrorLsq(double rmsnoisechan, double lambdaSqVariance, int Nchan, double P)
{
	double phiVariance=0;		// variance of phi
	double rmerror=0;				// rmerror = sqrt(phi); standard deviation

	phiVariance=pow(rmsnoisechan, 2)/(4*(Nchan-2)*pow(abs(P),2)*lambdaSqVariance);

	rmerror=sqrt(phiVariance);

	return rmerror;
}


double rm::rmErrorLsq(double rmsnoisechan, vector<double> &lambdaSqs, double P)
{
	double phiVariance=0;			// variance of phi
	double rmerror=0;					// rmerror = sqrt(phi); standard deviation
	double templambdaSqVariance=0;		// variance of lambda squareds computed with internal function
	int Nchan=0;						// number of lambda squared channels

	templambdaSqVariance=lambdaSqVariance(lambdaSqs);
	Nchan=lambdaSqs.size();

	if(Nchan==0)
		throw "rm::rmErrorLsq number of channels is 0";
	if(templambdaSqVariance < 0)
		throw "rm::rmErrorLsq lambdaSqVariance < 0";

	phiVariance=pow(rmsnoisechan, 2)/(4*(Nchan-2)*pow(abs(P),2)*templambdaSqVariance);

	rmerror=sqrt(phiVariance);

	return rmerror;	
}


/*! \brief Calculate RM error using Bayesian statistics

    Input parameters are the vector of polarized intensities, along with a vector describing
    their frequency distribution; if bool freq is set to false, these are assumed to be
    given in lambda squared values
    
    \param intensity - polarized intensities per channel
    \param lambda_sqs - corresponding lambda squared values of channels
    \param lambda_sq - bool if vectors are given in lambda squared (=true) or frequency
    
    \return rm_error - computed RM error estimation
*/

vector<double> rm::rmErrorBayes(vector<complex<double> > &intensity, vector<double> &lambda_sqs, bool lambda_sq)
{
  vector<double> rm_error(intensity.size());


  return rm_error;
}




/*!
  \brief Read the distribution of measured frequencies from a text file
  
  \param filename -- name of txt file with frequency distribution

  \return frequencies -- vector with frequencies
*/
vector<double> rm::readFrequencies(const std::string &filename)
{
  vector<double> frequencies;		// hold list of lambda squareds
  double frequency;			// individual lambda squared read per line

  //----------------------------------------------------------
  // Check if filename is text file FITS file or HDF5 file
  if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  {
    // TODO
    // use dal to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use dalFITS table
  {
    // TODO
    // use dalFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rm::readFrequencies failed to open file";
    }
  
    while(infile.good())	// as long as we can read from the file...
    {
      infile >> frequency;	// read double into temporary variable
      frequencies.push_back (frequency);	// store in lambdaSquareds vector
    }

    infile.close();		// close the text file
  }

  return frequencies;	 // return frequencies vector
}


/*!
  \brief Read the distribution of measured frequencies from a text file
  
  \param filename -- name of txt file with frequency distribution
  \param deltafreqs - vector to take delta frequencies (computed from difference)

  \return frequencies -- vector with frequencies
*/
vector<double> rm::readFrequencies(const std::string &filename, vector<double> &deltafreqs)
{
  vector<double> frequencies;		// hold list of lambda squareds
  double frequency=0;				// individual lambda squared read per line
  double prev_frequency=0;			// value of previous frequency read
  double difference=0;					// difference between current and previous frequency

  //----------------------------------------------------------
  // Check if filename is text file FITS file or HDF5 file
  if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  {
    // TODO
    // use dal to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use dalFITS table
  {
    // TODO
    // use dalFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rm::readFrequencies failed to open file";
    }
  
    while(infile.good())	// as long as we can read from the file...
    {
      infile >> frequency;	// read double into temporary variable
      frequencies.push_back (frequency);	// store in lambdaSquareds vector
   	
		if(prev_frequency!=0)
		{
	      difference=frequency-prev_frequency;
			deltafreqs.push_back (difference);			// write into delta vector
		}
		prev_frequency=frequency;							// keep as previous frequency
    }
	 deltafreqs.push_back (difference);					// write last diff into delta vector

    infile.close();		// close the text file
  }

  return frequencies;	 // return frequencies vector
}



/*!
  \brief Read the distribution of measured lambda squareds from a text file
  
  \param filename -- name of txt file with lambda squared distribution

  \return lambdaSquareds -- vector with lambda squareds
*/
vector<double> rm::readLambdaSquareds(const std::string &filename)
{
  vector<double> lambdaSquareds;	// hold list of lambda squareds
  double lambdaSq;			// individual lambda squared read per line

  //----------------------------------------------------------
  // Check if filename is text file FITS file or HDF5 file
  if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  {
    // TODO
    // use dal to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use dalFITS table
  {
    // TODO
    // use dalFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rm::readLambdaSquareds failed to open file";
    }
  
    while(infile.good())	// as long as we can read from the file...
    {
      infile >> lambdaSq;	// read double into temporary variable
      lambdaSquareds.push_back (lambdaSq);	// store in lambdaSquareds vector
      cout << lambdaSquareds[lambdaSquareds.size()-1] << endl;
    }

    infile.close();		// close the text file
  }

  return lambdaSquareds;	 // return frequencies vector
}



/*!
  \brief Read the distribution of measured lambdaSquareds AND deltaLambdaSquareds from a text file
  
  \param filename - name of txt file with lambda squared distribution
  \param deltaFrequencies - vector to keep delta Frequencies

  \return lambdaSquareds - vector with frequencies and delta frequencies
*/
vector<double> rm::readFrequenciesAndDeltaFrequencies(const std::string &filename, vector<double> &deltaFrequencies)
{
  double frequency=0;			// individual frequency read per line
  double deltaFrequency=0;		// individual delta frequency read per line  
  vector<double> frequencies;		// frequencies to be returned

  //----------------------------------------------------------
  // Check if filename is text file FITS file or HDF5 file
  if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  {
    // TODO
    // use dal to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use dalFITS table
  {
    // TODO
    // use dalFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rm::readFrequenciesAndDeltaFrequencies failed to open file";
    }
  
    while(infile.good())	// as long as we can read from the file...
    {
      infile >> frequency;			// read frequency (first column)
      infile >> deltaFrequency;			// read delta Frequency (2nd coloumn)
      frequencies.push_back (frequency);		// store in frequencies vector
      deltaFrequencies.push_back (deltaFrequency);	// store in delta frequencies vector
    }

    infile.close();		// close the text file
  }

  return frequencies;	  // return frequencies vector
}


/*!
  \brief Read the distribution of measured lambdaSquareds AND deltaLambdaSquareds from a text file
  
  \param filename - name of txt file with lambda squared and delta lambda squared distribution
  \param deltaLambdaSquareds - vector with delta lambda squareds

  \return lambdaSquareds - vector with lambda squareds
*/
vector<double> rm::readLambdaSquaredsAndDeltaSquareds(const std::string &filename,   vector<double> &deltaLambdaSquareds)
{
  vector<double> lambdaSquareds;	// lambda squareds to be returned
  double lambdaSq=0;			// individual frequency read per line
  double deltaLambdaSq=0;		// individual delta frequency read per line  

  //----------------------------------------------------------
  // Check if filename is text file FITS file or HDF5 file
  if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  {
    // TODO
    // use dal to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use dalFITS table
  {
    // TODO
    // use dalFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rm::readLambdaSquaredsAndDeltaSquareds failed to open file";
    }
  
    while(infile.good())	// as long as we can read from the file...
    {
      infile >> lambdaSq;			// read frequency (first column)
      infile >> deltaLambdaSq;			// read delta Frequency (2nd coloumn)
      lambdaSquareds.push_back (lambdaSq);		// store in frequencies vector
      deltaLambdaSquareds.push_back (deltaLambdaSq);	// store in delta frequencies vector
    }

    infile.close();		// close the text file
  }

  return lambdaSquareds;	  // return frequencies vector
}


/*!
  \brief Write a vector (RM) out to file on disk (mainly for debugging)
  
  \param rm - vector containing data (real double) to write to file
  \param filename - name of file to create or append to
  \param mode - write mode: overwrite, append
*/
void rm::writeRMtoFile(vector<double> rm, const std::string &filename)
{
  unsigned int i=0;	// loop variable
 
  ofstream outfile(const_cast<const char *>(filename.c_str()), ofstream::out);

  for(i=0; i<rm.size(); i++)		// loop over vector
  {
    outfile << rm[i];				// write out data
    outfile << endl;					// add endl
  }

  outfile.flush();					// flush output file
}


/*!
  \brief Write a vector (RM) out to file on disk (mainly for debugging)
  
  \param rm - vector containing data (real double) to write to file
  \param filename - name of file to create or append to
*/
void rm::writeRMtoFile(vector<complex<double> > rm, const std::string &filename)
{
  ofstream outfile(const_cast<const char *>(filename.c_str()), ofstream::out);

  for(unsigned int i=0; i<rm.size(); i++)		// loop over vector
  {
    outfile << rm[i].real() << "   ";			// write real part of data
	 outfile << rm[i].imag();						// write imaginary part of data
    outfile << endl;									// add endl
  }

  outfile.flush();						// flush output file
}


/*!
  \brief Write a vector (RM) out to file on disk (mainly for debugging)
  
  \param lambdasq - vector containing the lambda squared wavelengths
  \param rm - vector containing data (real double) to write to file
  \param filename - name of file to create or append to
*/
void rm::writeRMtoFile(vector<double> &lambdasq, vector<complex<double> > &rm, const std::string &filename)
{
  if(lambdasq.size()!=rm.size())
	 throw "rm::writeRMtoFile lambdasq and rm vector differ in size";
	
  ofstream outfile(const_cast<const char *>(filename.c_str()), ofstream::out);

  for(unsigned int i=0; i<rm.size(); i++)		// loop over vector
  {
	 outfile << lambdasq[i] << "   ";			// write lambda squareds 
    outfile << rm[i].real() << "   ";			// write real part of data
	 outfile << rm[i].imag();						// write imaginary part of data
    outfile << endl;									// add endl
  }

  outfile.flush();						// flush output file
}
