/*!
  \class rm
  
  \ingroup RM
  
  \brief Brief description for class <newClass>
  
  \author Sven Duscha
  
  \date 28.01.09.
  
  \test trm.cc
  
  <h3>Prerequisite</h3>
  
  <ul type="square">
  <li>[start filling in your text here]
  </ul>
    
  <h3>Synopsis</h3>

  The rm class provides algorithms for computing the Faraday Rotation Measure
  from an input image cube. The image cube should be provided in a 3D-"cube"
  array and can be read as a whole or in a tiled manner.
  
  The RM is computed along the frequency axis of each line of sight. Since RM is the
  Inverse Fourier Transform of the lambda squared polarized intensities along the line
  of sight, input cube frequency planes are converted to lambda squared planes.
  
  <h3>Example(s)</h3>
  
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
		
		vector<double> freqToLambdaSq(vector<double> frequency);		// convert frequency vector to lambda squared vector
		vector<double> lambdaSqToFreq(vector<double> lambda_sq);		// convert frequency vector to lambda squared vector

	public:
		rm();							// constructor:
		~rm();							// destructor: frees memory?
		
		// Rotation Measure computing algorithms
		vector<double> inverseFourier(vector<double>, vector<double>, bool freq=true);		// Brentjens and de Bruyn with the Inverse Fourier Transform
		vector<double> wavelet(vector<double>, vector<double>, vector<double> wavelet_parameters, bool freq=true);		// Frick and Stepanov wavelet algorithm
		//! Rotation Measure computing algorithms
		vector<double> inverseFourier(vector<double>, vector<double>, bool freq=true);		//! Brentjens and de Bruyn with the Inverse Fourier Transform
		vector<double> wavelet(vector<double>, vector<double>, vector<double> wavelet_parameters, bool freq=true);				//! Frick and Stepanov wavelet algorithm
		vector<double> ift(vector<double>, vector<double>, bool freq=true);					// Enßlin Information Theory Based algorithm
		vector<double> fourierTransform(vector<double>, vector<double>, bool freq=true);	// (Forward) Fourier Transform to get an image from an RM cube
		
		// Atmospheric correction of RM (contribution through atmosphere)
		// ??
		double atmosphericFaradayRotation(double tec);		// calculcate Faraday rotation RM contribution by atmosphere
		
		// Rotation Measure error estimation
		vector<double> rmErrorLsq(vector<double> intensities, vector<double> lambda_sqs, bool lambda_sq=true);		// (assuming) a least squares error approximation
		vector<double> rmErrorBayes(vector<double> intensities, vector<double> lambda_sqs, bool lambda_sq=true);	// error estimate based on Bayesian statistics
};

#endif		// _RM_H_
