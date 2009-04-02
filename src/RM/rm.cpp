
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Utilities/DataType.h>
#include <tables/Tables/TiledFileAccess.h>
#include <lattices/Lattices/TiledShape.h>

#include "rm.h"

using namespace std;
using namespace casa;		// namespace for functions from casacore

//===============================================================================
//
//  Constructions / Destruction
//
//===============================================================================

rm::rm()
{
	// Do nothing in particular
}


// rm class destructor
//
rm::~rm()
{
	// Free memory?
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
vector<double>rm::freqToLambdaSq(vector<double> frequency)
{
  /* lambda squareds of measured channels */
  vector<double> lambda_sq;
  /* loop variable */
  int i=0;
  /* conversion factor to convert from freq to lambda squared values */
  double conversion_factor=0;


  for (vector<double>::iterator i = frequency.begin();i != frequency.end(); i++)   // loop through vector and convert to lambdaSquared
  {
//    cout << "frequency[" << i << "] = " << (*i) << endl;	// debug
  
  }
  
  
  return lambda_sq;
}


/*!
  \brief Convert from lambda squared to frequencies
  
  \param lambda_sq -- 

  \return freq -- 
*/
vector<double>rm::lambdaSqToFreq(vector<double> lambda_sq)
{
	vector<double> freq;	// frequencies of measured channels

	return freq;
}


// Convert intensities to Jy/beam
// is this needed????



// Inverse Fourier Method for calculating the Rotation Measure
// Normal case (bool freq=true, default): the input vector is a range of complex polarized intensities
// measured at different frequencies
// Special case (bool freq=false): the input vector is already given as a range of polarized intensities
// given at different lambda squareds
//
vector<double>rm::inverseFourier(vector<double> intensity,
				 vector<double> frequency,
				 bool freq)
{
	vector<double>rm;	// rm values along the line of sight


	return rm;
}



/*!
  Wavelet Transform Method for calculating the Rotation Measure

  Normal case (bool freq=true, default): the input vector is a range of complex
  polarized intensities measured at different frequencies. Parameters a_min/a_max
  and b_min/b_max define ranges for wavelet parameter space probing.

  \param intensity -- 
  \param frequencies -- 
  \param wavelet_parameters -- 
  \param freq -- 
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

// Calculate RM error using LSQ approximation
// Input parameters are the vector of polarized intensities, along with a vector describing
// their frequency distribution; if bool freq is set to false, these are assumed to be
// given in lambda_squared values
//
vector<double> rm::rmErrorLsq(vector<double> intensity, vector<double> lambda_sqs, bool lambda_sq)
{
	vector<double> rm_error;


	return rm_error;
}


// Calculate RM error using Bayesian statistics
//
//
vector<double> rm::rmErrorBayes(vector<double> intensity, vector<double> lambda_sqs, bool lambda_sq)
{
	vector<double> rm_error;


	return rm_error;
}
