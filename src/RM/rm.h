/*
 *  rm.h
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
 *
 */

#ifndef RM_H
#define RM_H


#include <vector>


using namespace std;


//	RM class
//
//
class rm
{
	private:
		vector<double> cached_los;		// cached line of sight rm values if already computed
		int cached_x, cached_y;			// pixel coords of cached rm los values
		double cached_ra, cached_dec;	// Right Ascension and Declination coords of cached los rm values
		
		vector<double> freq_to_lambda_sq(vector<double> frequency);	// convert frequency vector to lambda squared vector
		vector<double> lambda_sq_to_freq(vector<double> lambda_sq);		// convert frequency vector to lambda squared vector

	public:
		rm();							// constructor:
		~rm();							// destructor: frees memory?
		
		// Rotation Measure computing algorithms
		vector<double> inversefourier(vector<double>, vector<double>, bool freq=true);		// Brentjens and de Bruyn with the Inverse Fourier Transform
		vector<double> wavelet(vector<double>, vector<double>, bool freq=true);				// Frick and Stepanov wavelet algorithm
		vector<double> ift(vector<double>, vector<double>, bool freq=true);					// Enßlin Information Theory Based algorithm
		vector<double> fouriertransform(vector<double>, vector<double>, bool freq=true);	// (Forward) Fourier Transform to get an image from an RM cube
		
		// Atmospheric correction of RM (contribution through atmosphere)
		// ??

		
		// Rotation Measure error estimation
		double lsq(vector<double> lambda_squared);			// (assuming) a least squares error
		double bayesian(vector<double> lambda_squared);		// error estimate based on Bayesian statistics
};

#endif		// _RM_H_