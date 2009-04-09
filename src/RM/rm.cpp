/*! Implementation of rm class methods

    Author:		Sven Duscha (sduscha@mpa-garching.mpg.de)
    Date:		18-12-2008
    Last change:	06-04-2009
*/


#include <iostream>				// C++/STL iostream

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

  \param x, y, depth, stepsize, os -- 
 
  \return none --
*/

rm::rm(int x, int y, int depth, double stepsize, ofstream &file)	// pass only reference to output stream!
{
	// declare RM cube of desired size and assign output stream
	
	
}

/*!
  \brief rm class destructor

*/

rm::~rm()
{
  // Free memory? Currently nothing to be done
}

//===============================================================================
//
//  Methods
//
//===============================================================================

/*!
  \brief Convert from frequencies to lambda squared

  \param frequency -- 

  \return lambda_sq -- 
*/
vector<double>rm::freqToLambdaSq(vector<double> &frequency)
{
  // constants
  double c=299792458.0;			// vacuum speed of light, c, in metres per second 
//  int csq=299792458*299792458;	// c^2
  double lambda=0.0;			// temporary variable to hold lambda (non-squared!) after initial conversion
 
  /* lambda squareds of measured channels */
  vector<double> lambda_sq;
  /* conversion factor to convert from freq to lambda squared values */
  double conversion_factor=0;


  for (vector<double>::iterator it = frequency.begin();it != frequency.end(); it++)   // loop through vector and convert to lambdaSquared
  {
    lambda=c/(*it);
 
    cout << "frequency = " << *it << "  lambda = " << lambda << endl;
  }
  
  lambda=10;
  return lambda_sq;
}


/*!
  \brief Convert from lambda squared to frequencies
  
  \param lambda_sq -- 

  \return freq -- 
*/
vector<double>rm::lambdaSqToFreq(vector<double> &lambda_sq)
{
	vector<double> freq;	// frequencies of measured channels

	return freq;
}


// Convert intensities to Jy/beam
// is this needed????



/*! \brief Inverse Fourier Method for calculating the Rotation Measure
    
    The inverse Fourier Transformation is the classical relation between the lambda squared space and Faraday
    space. Input images channel are not necessarily given in lambda squared wavelengths, but in frequency instead. If that is the case, a conversion function between lambda squared and frequency is used.
    
    Normal case (bool freq=true, default): the input vector is a range of complex polarized intensities
    measured at different frequencies
    Special case (bool freq=false): the input vector is already given as a range of polarized intensities
    given at different lambda squareds

    \param intensity --
    \param frequency --
    \param freq --
    
    \return rm --
*/
vector<double>rm::inverseFourier(vector<double> intensity,
				 vector<double> frequency,
				 bool freq)
{
	vector<double>rm;	// rm values along the line of sight


	return rm;
}



/*!
  \brief Wavelet Transform Method for calculating the Rotation Measure

  Normal case (bool freq=true, default): the input vector is a range of complex
  polarized intensities measured at different frequencies. Parameters a_min/a_max
  and b_min/b_max define ranges for wavelet parameter space probing.

  \param intensity -- 
  \param frequencies -- 
  \param wavelet_parameters -- 
  \param freq -- 
  
  \return rm --
 */
vector<double> rm::wavelet(vector<double> intensity,
			   vector<double> frequencies,
			   vector<double> wavelet_parameters,
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
