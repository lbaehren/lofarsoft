/*! Implementation of rm class methods

    Author:		Sven Duscha (sduscha@mpa-garching.mpg.de)
    Date:		18-12-2008
    Last change:	19-10-2009
*/


//#include <iostream>				// C++/STL iostream
//#include <fstream>				// file stream I/O
#include <math.h>				// mathematics library
#include <string.h>
#include <assert.h>
#include <algorithm>				// max_lement

#ifdef HAVE_CASA
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Utilities/DataType.h>
#include <tables/Tables/TiledFileAccess.h>
#include <lattices/Lattices/TiledShape.h>
#endif

#include "rm.h"					// rm class declarations

using namespace std;

//===============================================================================
//
//  Constructions / Destruction
//
//===============================================================================

rm::rm ()
{
  // do nothing in particular
}


rm::~rm ()
{ 
  // Free memory? Currently nothing to be done
  //  cout << "rm destructor called" << endl;
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

double rm::totalIntensity(const vector<double> &intensities, int unit=0)
{
  	double totalIntensity=0;

	if(intensities.size()==0)
		throw "rm::totalIntensity intensities has size 0";

  	// integrate over all frequencies in channel
	for(unsigned int i=0; i < intensities.size(); i++)
	{
		totalIntensity+=intensities[i];
	  	// convert to desired unit (given by string unit)
	}

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
	\param &delta_lambda_squareds - vector to hold delta lambda squareds
*/
void rm::deltaLambdaSqTopHat( const vector<double> &frequencies,
												const vector<double> &bandwidths,
												vector<double> &lambda_centre_squareds,
												vector<double> &delta_lambda_squareds)
{
	// constants
	double csq=89875517873681764.0;				// c^2
	// temporary variables
	double frequencyquotient=0;					// temporary variable for frequency quotient
	
//vector<double> delta_lambda_squareds;		// delta lambda squared vector to be returned (now parameter)
	
	for(unsigned int i=0; i<frequencies.size(); i++)	// compute for all frequencies in vector
	{
		// lambda centre squareds
		frequencyquotient=bandwidths[i]/frequencies[i];
		lambda_centre_squareds[i]=csq/(pow(frequencies[i],2))*(1+0.75*pow(frequencyquotient, 2));
	
		// delta lambda squareds
		delta_lambda_squareds[i]=2*csq*bandwidths[i]/pow(frequencies[i], 3)*(1+0.5*pow(frequencyquotient, 2));
	}	
	
//	return delta_lambda_squareds;
}


/*!
	\brief Compute the delta lambda squareds from a vector of lambda squareds

	\param lambdaSq - vector containing lambda squareds
*/
vector<double> rm::deltaLambdaSq(const vector<double> &lambdaSq)
{
	double previousLambdaSq=0;				// previous lambdaSq
	double previousDeltaLambdaSq=0;		// previous computed delta lambda squared
	
	if(lambdaSq.size()==0)
		throw "rm::deltaLambdaSq lambdaSq vector is 0";
	
	vector<double> deltaLambdaSq(lambdaSq.size());		// vector to contain computed delta lambda squareds
	
	previousLambdaSq=lambdaSq[0];								// first lambda squared kept as previous
	for(unsigned int i=1; i<lambdaSq.size(); i++)
	{
		if(i < lambdaSq.size()-1)
		{
			deltaLambdaSq[i]=lambdaSq[i]-previousLambdaSq;
			previousDeltaLambdaSq=deltaLambdaSq[i];
		}
		else
		{
			deltaLambdaSq[i]=previousDeltaLambdaSq;		// last delta should be set to the same as previous (a bit crude)
		}
	}
	
	return deltaLambdaSq;										// return vector with computed delta lambda squareds
}


/*!
	\brief Compute delta lambda squareds differences from lambda squareds
	
	\param values - vector with lambda squareds
	\param deltas - vector to hold computed delta lambda squareds
*/
void rm::computeDeltas(const vector<double> &values, vector<double> &deltas)
{	
	double previous=values[0];		// variable to hold value of previous lambda squared

	if(values.size()==0)
	  throw "rm::computeDeltas values has size 0";

	if(deltas.size()!=values.size())
	  deltas.resize(values.size());		// resize delta lambda squared vector
	
	for(unsigned int i=1; i < values.size(); i++)
	{
		deltas[i-1]=abs(values[i]-previous);
		previous=values[i];
	}	
	deltas[values.size()-1]=deltas[values.size()-2];	// fill last value
}


/*!
	\brief Compute weights with inverse weighting from noise vector
	
	\param noisevec - vector containing noise per channel
	\param weights - vector with computed weights
*/
void rm::computeWeights(const vector<double> &noisevec, vector<double> &weights)
{
	if(noisevec.size()==0)
		throw "rm::computeWeights noise vector has size 0";
	
	for(unsigned int i=0; i < noisevec.size(); i++)
	{
		weights[i]=1/noisevec[i];
	}
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
    complex<double> rmpolint;						// rm value
	 double lambdaZeroSq=0;							// lambdaZeroSquared
    int chan=0; 									// variables
    const int numchannels=lambda_squared.size();	// number of lambda squared channels

    double K=1;										// K weighting factor for RM-synthesis


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
    // P(phi) = K * expfactor * Sum_0^frequency.size() {weights[i]*P(lambda^2)*exp(lambda^2)}
    //
    for(chan=0; chan<numchannels; chan++)
    {
	// Use Euler formula for exp_lambdafactor
 	exp_lambdafactor=complex<double>(cos(-2.0*phi*(lambda_squared[chan]-lambdaZeroSq)), sin(-2.0*phi*(lambda_squared[chan]-lambdaZeroSq)));	
      // compute complex exponential factor
//      exp_lambdafactor=exp(complex<double>(0, -2*phi)*lambda_squared[chan]);
      rmpolint=rmpolint+(weights[chan]*intensity[chan]*exp_lambdafactor)*delta_lambda_squared[chan];
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
  vector<complex<double> > rmpolint;				// polarized RM intensities per Faraday depth

  complex<double> exp_lambdafactor=0;				// exponential factor in direct FT
  double lambdaZeroSq=0;					// derotating lambdaZeroSquared
  double phi=0;							// single phi value to be computed
  const unsigned int numchannels=lambda_squared.size();		// number of frequency channels

  double K=1;							// K weighting factor for RM-synthesis

  //-----------------------------------------------------------------------
  // Consistency checks
  //-----------------------------------------------------------------------  
  if(lambda_squared.size() != weights.size())
    throw "rm::inverseFourier lambda squareds and weights vector differ in size";
  if(lambda_squared.size() != delta_lambda_squared.size())
    throw "rm::inverseFourier lambda squareds and delta lambda squareds vector differ in size";  
  if(intensity.size() != lambda_squared.size())
    throw "rm::inverseFourier lambda squareds and intensity vector differ in size";  

  //-----------------------------------------------------------------------
  if(rmpolint.size() != phis.size())
    rmpolint.resize(phis.size());
  
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

  rmpolint.resize(phis.size());

  // compute discrete Fourier sum by iterating over frequency vector
  //
  // P(phi) = K * expfactor * Sum_0^frequency.size() {P(lambda^2)*exp(-2*i*phi*lambda^2)}
  //
  for(unsigned int i=0; i<phis.size(); i++)	// loop over Faraday depths given in phis vector 
  {
     phi=phis[i];				// select phi from Faraday depths vector
 
//      cout << "rm:inverseFourier" << i << "\t" << phi << endl;
     for(unsigned int chan=0; chan<numchannels; chan++)
     {
// 	cout <<	chan << "\t" << lambda_squared[chan] << "\t" << intensity[chan] << endl;
      
	// Use Euler formula for exp_lambdafactor
 	exp_lambdafactor=complex<double>(cos(-2.0*phi*(lambda_squared[chan]-lambdaZeroSq)), sin(-2.0*phi*(lambda_squared[chan]-lambdaZeroSq)) );  
	rmpolint[i]=rmpolint[i]+(weights[i]*intensity[chan]*exp_lambdafactor*delta_lambda_squared[chan]);

// 	cout << chan << "\t" << delta_lambda_squared[chan] << endl;
// 	cout << chan << "\t" << weights[chan] << endl;
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
      rmpolint=rmpolint+(weights[chan]*intensity[chan]*exp_lambdafactor)*delta_lambda_squared[chan];
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

  complex<double> exp_lambdafactor=0;				// exponential factor in direct FT
  double lambdaZeroSq=0;					// derotating lambdaZeroSquared
  double phi=0;													// single phi value to be computed
  const unsigned int numchannels=lambda_squared.size();		// number of frequency channels

  double K=1;							// K factor for RM-synthesis

  //-----------------------------------------------------------------------

  // Compute weighting factor from weights
  for (vector<double>::const_iterator it = weights.begin(); it!=weights.end(); ++it) 
  {
    K+=*it;
  }

  if(K!=0)		// Do not do a division by zero!
    K=1/K;		// take inverse
  else			// otherwise...
    K=1;		// ... use 1 as default    

  // If a derotating lambda Zero != Zero was given compute lambdaZeroSq
  if(lambdaZero)
     lambdaZeroSq=lambdaZero*lambdaZero;

  // compute discrete Fourier sum by iterating over frequency vector
  //
  // P(phi) = K * expfactor * Sum_0^frequency.size() {P(lambda^2)*exp(-2*i*phi*lambda^2)}
  //
  for(unsigned int i=0; i<phis.size(); i++)	// loop over Faraday depths given in phis vector 
  {
    phi=phis[i];				// select phi from Faraday depths vector
    for(unsigned int chan=0; chan<numchannels; chan++)
    {
        // Use Euler formula for exp_lambdafactor
   // 	  ex_lambdafactor=(cos(-2*phi*(lambda_squared[chan]-lambdaZeroSq)), sin(-2*phi*(lambda_squared[chan]-lambdaZeroSq)));		// BUGGY! casa error?
      exp_lambdafactor=complex<double>(cos(-2.0*phi*(lambda_squared[chan]-lambdaZeroSq)), sin(-2.0*phi*(lambda_squared[chan]-lambdaZeroSq)) );  
      rmpolint[i]=rmpolint[i]+(weights[i]*intensity[chan]*exp_lambdafactor*delta_lambda_squared[chan]);
    }

    rmpolint[i]=K*rmpolint[i];	// multiply with weighting
  }

  return rmpolint;	// return vector of complex polarized intensities per Faraday depth
}



/*!
    \brief Forward Fourier Transform - inverse operation to RM-Synthesis

    \param &lambda_sqs - Lambda squareds to compute RM for
    \param &rmpolint - complex polarized intensities for each Faraday depth
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
//   double K=1;																// weighting factor per faraday depth

  //-----------------------------------------------------------------------
  /*
  // Compute weighting factor from weights
  for (vector<double>::const_iterator it = weights.begin(); it!=weights.end(); ++it) 
  {
    K+=*it;
  }

  if(K!=0)		// Do not do a division by zero!
    K=1/K;		// take inverse
  else			// otherwise...
    K=1;			// ... use 1 as default    
  */

  for(unsigned int i=0; i < intensities.size(); i++)
    intensities[i]=0;					// initialize intensities with zeros

  // compute discrete Fourier sum by iterating over frequency vector
  //
  // P(phi) = K * expfactor * Sum_0^frequency.size() {P(lambda^2)*exp(-2*i*phi*lambda^2)}
  //
  for(unsigned int i=0; i<numlambda_sqs; i++)	// loop over lambda squareds given in phis vector 
  {
     for(unsigned int depth=0; depth<numfaradays; depth++)
     {
		  lambdasq=lambda_sqs[i];				// select lambda squared from lambda squareds vector
        // Use Euler formula for exp_lambdafactor
 		  exp_lambdafactor=complex<double>(cos(+2.0*lambdasq*faradays[depth]), sin(+2.0*lambdasq*faradays[depth]) );  
		  intensities[i]=intensities[i]+(weights[depth]*rmpolint[depth]*exp_lambdafactor*delta_faradays[depth]);
     }
  
 //      intensities[i]=K*intensities[i];		// multiply by weighting factor
     intensities[i]=intensities[i];		// multiply by weighting factor 
  }

//   cout << "K = " << K << endl;

  return intensities;	// return vector of complex polarized intensities per Faraday depth
}


/*!
    \brief Forward Fourier Transform - inverse operation to RM-Synthesis

    \param &lambda_sqs - Lambda squareds to compute RM for
    \param &rmpolint - Polarized intensities for each Faraday depth
    \param &lambda_squared - Lambda squareds of polarized intensities
    \param &weights - Weights associated with each frequency (or lambda squared)
    \param &delta_phis - Delta phi distance between intensities in Faraday space
    
    \return intensities - vector of polarized intensity computed over vector freqs
*/
vector<complex<double> > rm::forwardFourier(const vector<double> &lambda_sqs,
				 const vector<double> &rmpolint,
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
//   double K=1;																// weighting factor per faraday depth

  //-----------------------------------------------------------------------

  /*
  // Compute weighting factor from weights
  for (vector<double>::const_iterator it = weights.begin(); it!=weights.end(); ++it) 
  {
    K+=*it;
  }

  if(K!=0)		// Do not do a division by zero!
    K=1/K;		// take inverse
  else			// otherwise...
    K=1;			// ... use 1 as default    
  */

  for(unsigned int i=0; i < intensities.size(); i++)
    intensities[i]=0;					// initialize intensities with zeros

  // compute discrete Fourier sum by iterating over frequency vector
  //
  // P(phi) = K * expfactor * Sum_0^frequency.size() {P(lambda^2)*exp(-2*i*phi*lambda^2)}
  //
  for(unsigned int i=0; i<numlambda_sqs; i++)	// loop over lambda squareds given in phis vector 
  {
     for(unsigned int depth=0; depth<numfaradays; depth++)
     {
		  lambdasq=lambda_sqs[i];				// select lambda squared from lambda squareds vector
        // Use Euler formula for exp_lambdafactor
 		  exp_lambdafactor=complex<double>(cos(+2.0*lambdasq*faradays[depth]), sin(+2.0*lambdasq*faradays[depth]) );  
		  intensities[i]=intensities[i]+(weights[depth]*rmpolint[depth]*exp_lambdafactor*delta_faradays[depth]);
     }
  
//      intensities[i]=K*intensities[i];		// multiply by weighting factor
       intensities[i]=intensities[i];		// multiply by weighting factor
  }

//   cout << "K = " << K << endl;

  return intensities;	// return vector of complex polarized intensities per Faraday depth
}


/*!
	 \brief Q polarization only Forward Fourier Transform - inverse operation to RM-Synthesis

    \param &lambda_sqs - lambda squareds to compute Q intensity for
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
     for(unsigned int depth=0; depth<numfaradays; depth++)
  	  {
		  lambdasq=lambda_sqs[i];				// select lambda squared from lambda squareds vector
        // Use Euler formula for exp_lambdafactor
 		  exp_lambdafactor=cos(+2.0*lambdasq*faradays[depth]);  
		  Qintensities[i]=Qintensities[i]+(weights[depth]*rmpolint[depth]*exp_lambdafactor*delta_faradays[depth]);
     }
  }

  return Qintensities;	// return vector of complex polarized intensities per Faraday depth
}


/*!
	 \brief U polarization only Forward Fourier Transform - inverse operation to RM-Synthesis

    \param &lambda_sqs - lambda squareds to compute U intensity for
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
     for(unsigned int depth=0; depth<numfaradays; depth++)
  	  {
		  lambdasq=lambda_sqs[i];				// select lambda squared from lambda squareds vector
        // Use Euler formula for exp_lambdafactor
 		  exp_lambdafactor=sin(+2.0*lambdasq*faradays[depth]);  
		  Uintensities[i]=Uintensities[i]+(weights[depth]*rmpolint[depth]*exp_lambdafactor*delta_faradays[depth]);
     }
  }

  return Uintensities;	// return vector of complex polarized intensities per Faraday depth
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
  if(weights.size()!=lambda_squared.size() || weights.size()!=delta_lambda_squared.size())
     throw "rm::RMSF input vectors differ in length";
  if(lambda_squared.size() != delta_lambda_squared.size())
	  throw "rm::RMSF lambda_squareds and delta_lambda_squareds vector differ in size";

  //***********************************************************

  for(unsigned int iphi=0; iphi < phissize; iphi++)	// loop over all Faraday depths
  {
    // Since weights and lambda_sqs do correspond to each other and have the
    // same length, we can use iweight to index lambda_sqs and delta_lambda_sqs, too
    // Loop over all weights and compute Riemann's integral
    for(unsigned int iweight=0; iweight < weightssize; iweight++)
    {
   	 exp_factor=complex<double>(cos(-2*phis[iphi]*lambda_squared[iweight]), sin(-2*phis[iphi]*lambda_squared[iweight]));
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
vector<complex<double> > rm::RMSFfreq(const vector<double> &phis,
				  const vector<double> &frequencies,
				  const vector<double> &weights,
				  const vector<double> &delta_frequencies,
				  const double freqZero)
{
  vector<complex<double> > rmsfvec(phis.size());				// calculated rmsf to be returned
  vector<double> lambda_sqs(frequencies.size());				// vector to keep to lambda squared converted frequencies
  vector<double> delta_lambda_sqs(frequencies.size());		// vector to keep to delta lambda squared converted frequencies
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
	\brief Scale the RMSF to a maximum of 1 (optional a different max value)
	
	\param rmsf - vector containing the rmsf
	\param max - maximum (at 0) to scale RMSF to
*/
void rm::normalizeRMSF(vector<complex<double> > &rmsf, const double max)
{
	double currentMax=0;						// current maxmimum value of RMSF at 0
	double scaleFactor=0;					// resulting scaling factor to be used
	unsigned int maxpos=0;					// position of maximum in vector
	vector<double> power(rmsf.size());	// vector to hold power of rmsf vector

	// First compute power of complex rmsf
	for(unsigned int i=0; i < rmsf.size(); i++)
		power[i]=sqrt(rmsf[i].real()*rmsf[i].real() + rmsf[i].imag()*rmsf[i].imag());

	// Get current maximum value at 0	
	maxpos=max_element(power.begin(), power.end())-power.begin();
	currentMax=power[maxpos];
	scaleFactor=max/currentMax;	// compute scale factor from 1 to currentMax
	
	// Scale the whole complex rmsf vector with scaling factor
	for(unsigned int i=0; i < rmsf.size(); i++)
	{
		rmsf[i]=scaleFactor*rmsf[i];
	}
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


/*!
	\brief Calculate the RM error with the least squared fit error estimation
	
	\param rmsnoisechan - rms noise per channel
	\param lambdaSqVariance - variance of lambda squared distribution
	\param Nchan - number of channels in observation
	\param P - total polarized intensity integrated over all channels
	
	\return rmerror - error in polarized RM
*/
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


/*! 
    \brief Calculate RM error using Bayesian statistics

    Input parameters are the vector of polarized intensities, along with a vector describing
    their frequency distribution; if bool freq is set to false, these are assumed to be
    given in lambda squared values
    
    \param error - computed RM error estimation
    \param intensity - polarized intensities per channel
    \param lambda_sqs - corresponding lambda squared values of channels
    \param lambda_sq - bool if vectors are given in lambda squared (=true) or frequency
*/
void rm::rmErrorBayes(vector<double> &error, const vector<complex<double> > &intensity, const vector<double> &lambda_sqs, const bool lambda_sq)
{
  vector<double> rmerror(intensity.size());


}



/*!
  \brief Compute the Power of a complex vector
  
  \param signal - STL vector signal to compute P=sqrt(Q^2 + U^2)
  \param power - vector to hold resulting P
*/
void rm::complexPower(const vector<complex<double> > &signal, vector<double> &power)
{
   if(signal.size()==0)
      throw "wienerfilter::complexPower signal has size 0";

   if(power.size()!=(unsigned int)signal.size())		// if power vector has not the same length as signal
      power.resize(signal.size());		// resize power vector

   for(unsigned int i=0; i < (unsigned int)signal.size(); i++)
      power[i]=sqrt(signal[i].real()*signal[i].real()+signal[i].imag()*signal[i].imag());
}


// Now (02.08.2009) all I/O functions are in the rmio class

///*!
//  \brief Read the distribution of measured frequencies from a text file
  
//  \param filename -- name of txt file with frequency distribution

// \return frequencies -- vector with frequencies
//*/
/*
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
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
  {
    // TODO
    // use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rm::readFrequencies failed to open file";
    }
  
	 unsigned int i=0;
    while(infile.good())	// as long as we can read from the file...
    {
      infile >> frequency;	// read double into temporary variable

		if(frequencies.size() > i)					// if there is sufficient space in frequencies vector...
			frequencies[i]=frequency;				// write to index i
		else												// otherwise		
      	frequencies.push_back (frequency);	// push back into lambdaSquareds vector
    }

    infile.close();		// close the text file
  }

  return frequencies;	 // return frequencies vector
}
*/

///*!
//  \brief Read the distribution of measured frequencies from a text file
//  
//  \param filename -- name of txt file with frequency distribution
//  \param deltafreqs - vector to take delta frequencies (computed from difference)
//
//  \return frequencies -- vector with frequencies
//*/
/*
vector<double> rm::readFrequenciesDiffFrequencies(const std::string &filename, vector<double> &deltafreqs)
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
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
  {
    // TODO
    // use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rm::readFrequencies failed to open file";
    }
  
	 unsigned int i=0;					// vector index variable
    while(infile.good())				// as long as we can read from the file...
    {
      infile >> frequency;				// read double into temporary variable

		if(frequencies.size() > i)					// if there is sufficient space in frequencies vector...
			frequencies[i]=frequency;				// write to index i
		else												// otherwise
      	frequencies.push_back (frequency);	// push back into lambdaSquareds vector
   	
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
*/


///*!
//  \brief Read the distribution of measured lambda squareds from a text file
// 
//  \param filename -- name of txt file with lambda squared distribution
//
//  \return lambdaSquareds -- vector with lambda squareds
//*/
/*
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
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
  {
    // TODO
    // use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rm::readLambdaSquareds failed to open file";
    }
  
	 unsigned int i=0;		// vector index variable
    while(infile.good())	// as long as we can read from the file...
    {
      infile >> lambdaSq;	// read double into temporary variable
		
		if(lambdaSquareds.size() > i)					// if lambdaSquareds has sufficient size
			lambdaSquareds[i]=lambdaSq;				// write to index i in vector
		else
      	lambdaSquareds.push_back (lambdaSq);	// push back into lambdaSquareds vector
    }

    infile.close();		// close the text file
  }

  return lambdaSquareds;	 // return frequencies vector
}
*/


///*!
//  \brief Read the distribution of measured lambdaSquareds AND deltaLambdaSquareds from a text file
//  
//  \param filename - name of txt file with lambda squared distribution
//  \param deltaFrequencies - vector to keep delta Frequencies
//
//  \return lambdaSquareds - vector with frequencies and delta frequencies
//*/
/*
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
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
  {
    // TODO
    // use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rm::readFrequenciesAndDeltaFrequencies failed to open file";
    }

    unsigned int i=0;						// vector index variable if vector has sufficient size
    while(infile.good())					// as long as we can read from the file...
    {
      infile >> frequency;					// read frequency (first column)
      infile >> deltaFrequency;			// read delta Frequency (2nd coloumn)
 
		if(frequencies.size() > i)								// if frequencies vector has sufficient size
			frequencies[i]=frequency;							// store at index i
		else
     		frequencies.push_back (frequency);				// if delta frequencies vector has sufficient size
		if(deltaFrequencies.size() > i)						// store at index i
			deltaFrequencies[i]=deltaFrequency;
		else
      	deltaFrequencies.push_back (deltaFrequency);		// store in delta frequencies vector
    }

    infile.close();		// close the text file
  }

  return frequencies;	  // return frequencies vector
}
*/

///*!
//  \brief Read the distribution of measured lambdaSquareds AND deltaLambdaSquareds from a text file
//  
//  \param filename - name of txt file with lambda squared and delta lambda squared distribution
//  \param deltaLambdaSquareds - vector with delta lambda squareds
//
//  \return lambdaSquareds - vector with lambda squareds
//*/
/*
vector<double> rm::readLambdaSquaredsAndDeltaSquareds(const std::string &filename,   vector<double> &deltaLambdaSquareds)
{
  vector<double> lambdaSquareds;		// lambda squareds to be returned
  double lambdaSq=0;						// individual frequency read per line
  double deltaLambdaSq=0;				// individual delta frequency read per line  

  //----------------------------------------------------------
  // Check if filename is text file FITS file or HDF5 file
  if(filename.find(".hdf5", 1)!=string::npos)	// if HDF5 use dal
  {
    // TODO
    // use dal to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".fits", 1)!=string::npos)	// if FITS file  use rmFITS table
  {
    // TODO
    // use rmFITSTable to read lambda Squareds and deltaLambdaSquareds from file
  }
  else if(filename.find(".txt", 1)!=string::npos)	// if it is text file
  {
    ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);	// open file for reading
  
    if(infile.fail())
    {
      throw "rm::readLambdaSquaredsAndDeltaSquareds failed to open file";
    }
  
	 int i=0;									// vector index variable if vector has sufficient size
    while(infile.good())					// as long as we can read from the file...
    {
      infile >> lambdaSq;					// read frequency (first column)
      infile >> deltaLambdaSq;			// read delta Frequency (2nd coloumn)

		if(static_cast<int>(lambdaSquareds.size()) > i)		// if there is size left in lambdasquareds and delta_lambda_squareds vectors
		{ 
			lambdaSquareds[i]=lambdaSq;
		}
		else
		{
			lambdaSquareds.push_back (lambdaSq);		// store in frequencies vector
		}	
		if(static_cast<int>(deltaLambdaSquareds.size()) > i)
		{
			deltaLambdaSquareds[i]=deltaLambdaSq;	
		}	
		else
		{
	      deltaLambdaSquareds.push_back (deltaLambdaSq);	// store in delta frequencies vector
		}
    }

    infile.close();		// close the text file
  }

  return lambdaSquareds;	  // return frequencies vector
}
*/

///*!
//	\brief Read lambda squareds, delta lambda squareds and complex data vector from a text file
//	
//	\param &filename - name of text file with simulated polarized emission data
//	\param &lambdasquareds - vector to store lambda squared values in
//	\param &delta_lambda_squareds - vector to store delta lambda squared values in
//	\param &intensities				-	vector<complex<double> > to store complex polarized intensities
//*/
/*
void rm::readSimDataFromFile(const std::string &filename, vector<double> &lambdasquareds, vector<double> &delta_lambda_squareds, vector<complex<double> > &intensities)
{
	ifstream infile(const_cast<const char*>(filename.c_str()), ifstream::in);		// file with lambda squareds
	
	double lambdasq=0;		// temporary variable for lambda sqaured per line
   double deltalambdasq=0;	// temporary variable for delta lambda squared per line
	double real=0;				// temporary variable for real part per line
	double imag=0;				// temporary variable for imaginary part per line
	complex<double> intensity;					// temporary complex intensity
	int i=0;					// loop variable
	
	// Temporary vectors
//	vector<complex<double> > intensities;

   if(infile.fail())
   {
      throw "rn::readSimDataFromFile failed to open file";
   }
	
	while(infile.good())
	{
		infile >> lambdasq >> deltalambdasq >> real >> imag;


		if(static_cast<int>(lambdasquareds.size()) > i)		// if there is size left in lambdasquareds and delta_lambda_squareds vectors
		{
			lambdasquareds[i]=lambdasq;
			delta_lambda_squareds[i]=deltalambdasq;
			intensities[i]=complex<double>(real, imag);		
		}
		else		// otherwise use push back function to append at the end of the vector
		{
		   lambdasquareds.push_back(lambdasq);
			delta_lambda_squareds.push_back(deltalambdasq);			
			intensities.push_back(complex<double>(real, imag));		
		}

		i++;										// increment index into data vector
	}
	
	infile.close();
}
*/

///*!
//  \brief Write a vector (RM) out to file on disk (mainly for debugging)
//  
//  \param rm - vector containing data (real double) to write to file
//  \param filename - name of file to create or append to
//  \param mode - write mode: overwrite, append
//*/
/*
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
*/

///*!
//  \brief Write a vector (RM) out to file on disk (mainly for debugging)
//  
//  \param rm - vector containing data (real double) to write to file
//  \param filename - name of file to create or append to
//*/
/*
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
*/

///*!
//  \brief Write a vector (RM) out to file on disk (mainly for debugging)
//  
//  \param lambdasq - vector containing the lambda squared wavelengths
//  \param rm - vector containing data (real double) to write to file
//  \param filename - name of file to create or append to
//*/
/*
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
*/
