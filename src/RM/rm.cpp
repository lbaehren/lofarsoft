/*
 *  rm.cpp
 *  rmsynth
 *
 *  Created by Sven Duscha on 28.01.09.
 * 
 *	The rm class provides algorithms for computing the Faraday Rotation Measure
 *	from an input image cube. The image cube should be provided in a 3D-"cube"
 *	array and can be read as a whole or in a tiled manner.
 *
 *	The RM is computed along the frequency axis of each line of sight. Since RM is the
 *	Inverse Fourier Transform of the lambda squared polarized intensities along the line
 *	of sight, input cube frequency planes are converted to lambda squared planes.
 */

#include "rm.h"

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
vector<double>rm::freq_to_lambda_sq(vector<double> frequency)
{
	vector<double> lambda_sq;	// lambda squareds of measured channels
	
	
	
	return lambda_sq;
}


// Convert from lambda squared to frequencies
//
vector<double>rm::lambda_sq_to_freq(vector<double> lambda_sq)
{
	vector<double> freq;	// frequencies of measured channels

	return freq;
}


// Inverse Fourier Method for calculating the Rotation Measure
// Normal case (bool freq=true, default): the input vector is a range of complex polarized intensities
// measured at different frequencies
// Special case (bool freq=false): the input vector is already given as a range of polarized intensities
// given at different lambda squareds
//
vector<double>rm::inversefourier(vector<double> intensity, vector<double> frequency, bool freq)
{
	vector<double>rm;	// rm values along the line of sight

	return rm;
}





// Wavelet Transform Method for calculating the Rotation Measure
// Normal case (bool freq=true, default): the input vector is a range of complex polarized intensities
// measured at different frequencies
//
vector<double> rm::wavelet(vector<double> intensity, vector<double> frequencies, bool freq)
{
	vector<double>rm;	// rm values along the line of sight


	return rm;
}

