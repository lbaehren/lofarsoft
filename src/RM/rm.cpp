/*
 *  rm.cpp
 *  rmsynth
 *
 *  Created by Sven Duscha on 28.01.09.
 * 
 *	The rm class provides algorithms for computing the Faraday Rotation Measure
 *	from an input image cube. The image cube should be provided in a 3D-"cube"
 *	array and can be read as a whole or in a tiled manner, using casacore lattice classes.
 *
 *	The RM is computed along the frequency axis of each line of sight. Since RM is the
 *	Inverse Fourier Transform of the lambda squared polarized intensities along the line
 *	of sight, input cube frequency planes are converted to lambda squared planes.
 */


#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Utilities/DataType.h>
#include <tables/Tables/TiledFileAccess.h>
#include <lattices/Lattices/TiledShape.h>


#include "rm.h"


using namespace std;
using namespace casa;		// namespace for functions from casacore


// rm class constructor
//
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



// Helper functions
//
// Convert from frequencies to lambda squared
//
vector<double>rm::freqToLambdaSq(vector<double> frequency)
{
	vector<double> lambda_sq;	// lambda squareds of measured channels
	
	
	
	return lambda_sq;
}


// Convert from lambda squared to frequencies
//
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
vector<double>rm::inverseFourier(vector<double> intensity, vector<double> frequency, bool freq)
{
	vector<double>rm;	// rm values along the line of sight


	return rm;
}



// Wavelet Transform Method for calculating the Rotation Measure
// Normal case (bool freq=true, default): the input vector is a range of complex polarized intensities
// measured at different frequencies
// Parameters a_min/a_max and b_min/b_max define ranges for wavelet parameter space probing
//
vector<double> rm::wavelet(vector<double> intensity, vector<double> frequencies, vector<double> wavelet_parameters, bool freq)
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
