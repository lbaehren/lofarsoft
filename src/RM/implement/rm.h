/*!
  \class rm
  
  \ingroup RM
  
  \brief Brief description for class rm
  
  \author Sven Duscha
  
  \date 28.01.09.
  
  \test trm.cc
  
  <h3>Prerequisite</h3>
  
  <ul type="square">
  <li> None
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
#include <iostream>			// streamed input output for rm cubes
#include <complex>			// complex math functions

#include <casa/Arrays/Array.h>		// use CASA Arrays as temporary buffers
#include <casa/Quanta/Unit.h>		// use CASA Unit class for unit handling
#include <casa/Quanta.h>		// ... and unit conversion


#include "../../DAL/implement/dalFITS.h"	// dalFITS file access

// Namespace usage
using namespace std;
//using namespace casa;

class rm
{
  // Private variables
private:
  //! Cached line of sight rm values if already computed
  vector<double> cached_los;
  //! Pixel coords of cached rm los values
  int cached_x, cached_y;
  //! Right Ascension and Declination coords of cached los rm values
  double cached_ra, cached_dec;

  //! Units of RA and Dec
 // casa::Unit raUnit;
 // casa::Unit decUnit;

  //! Units of input vectors (lamba squared or frequency) the cube was created from
  //casa::Unit spectralUnit;

  //! Units of Faraday intensity in output cube
  //casa::Unit faradayUnit;
  
  //! Compute delta lambda squareds from frequencies using tophat function
  vector<double> deltaLambdaSqTopHat(const vector<double> &frequency,
												 const vector<double> &bandwidth,
												 vector<double> &lambda_centre);

  //! Calculate integrated total intensity along one line of sight
  vector<double> totalIntensity(const vector<double> &, int);

	//! Compute the weighted average lambda zero from the observed lambda squareds
  double weightedLambdaZero(vector<double> &lambda_squareds, vector<double> &weights);

  // Public functions.
public:
  //! Calculate delta lambda squared steps from lower and higher frequency limits
  vector<double> deltaLambdaSq(	const vector<double> &freq_low, 
											const vector<double> &freq_high,
											bool freq=true);
	
  //! Convert frequency vector to lambda squared vector
  vector<double> freqToLambdaSq(const vector<double> &frequency);
  //! Convert lambda squared vector to frequency vector
  vector<double> lambdaSqToFreq(const vector<double> &lambda_sq);
	
	
  //! Default constructor
  rm();

  //! Constructor.(external buffer handling independent of cube object)
  rm(int depth, double stepsize);

  //! Constructor with associated output stream 
  rm(int, int, int, double, ofstream &);

  /*! \brief RM cube with associated Faraday buffer. One line of sight or two dimensional
  
      create a RM cube object with one (or a times b) associated Faraday line(s) (defaults: axis a=1 axis b=0)
      of sight, buffer is only given as reference
  */
  rm(int, int, int, double, casa::Array<double> &faradaybuffer, int x=1, int y=0);
  
  rm(int, int, int, double, casa::Array<double> &faradayplanes, int x, int y, int z=1);
  
  ~rm();
		
  //! Rotation Measure computing algorithms
  //
  //! Brentjens and de Bruyn with the Inverse Fourier Transform for a single RM value
  complex<double> inverseFourier(const double,
				 const vector<complex<double> > &,
				 const vector<double> &,
				 const vector<double> &,
				 const vector<double> &,
				 const double lambdaZero=0);

  //! Brentjens and de Bruyn with the Inverse Fourier Transform for complete set of RMs
  vector<complex<double> > inverseFourier(const vector<double> &,
														const vector<complex<double> > &,
														const vector<double> &,
														const vector<double> &,
														const vector<double> &,
														const double lambdaZero=0);
	
	//! Inverse Fourier transform for one polarization (Q or U) only to compute a single Faraday Depth													
	complex<double> inverseFourier(double phi,
											 const vector<double> &intensity,
											 const vector<double> &lambda_squared,
											 const vector<double> &weights,
											 const vector<double> &delta_lambda_squared,
											 const double lambdaZero);

	//! Inverse Fourier transform for one polarization (Q or U) only to compute for a vector of Faraday Depths	
	vector<complex<double> > inverseFourier(const vector<double> &phis,
								  				 		const vector<double> &intensity,
						  							   const vector<double> &lambda_squared,
													   const vector<double> &weights,
														const vector<double> &delta_lambda_squared,
														const double lambdaZero);

  // Compute the Rotation Measure Spread Function (RMSF)
  vector<complex<double> > RMSF( const vector<double> &,
			        						const vector<double> &,
											const vector<double> &,
											const vector<double> &,
											const double lambdaZero=0);
											
	// Compute the Roation Measure Spread Function from input frequencies
   vector<complex<double> > RMSFfreq(const vector<double> &phis,    
									  			 const vector<double> &frequencies,                     
									  			 const vector<double> &weights,                         
									  			 const vector<double> &delta_frequencies,               
									  			 const double freqZero=0);			
									
	//! Forward Fourier Transform to compute polarized intensities from RM for a selection of lambdas
	vector<complex<double> > forwardFourier(const vector<double> &lambda_sqs,
														 const vector<complex<double> > &rmpolint,
														 const vector<double> &faradays,
														 const vector<double> &weights,
														 const vector<double> &delta_faradays,
														 const double lambdaZero);					   

	//! Forward Fourier Transform for Q only
	vector<double> forwardFourierQ( const vector<double> &lambda_sqs,
										     const vector<double> &rmpolint,
											  const vector<double> &faradays,
											  const vector<double> &weights,
											  const vector<double> &delta_faradays,
											  const double lambdaZero);


	//! Forward Fourier Transform for U only
	vector<double> forwardFourierU( const vector<double> &lambda_sqs,
										     const vector<double> &rmpolint,
											  const vector<double> &faradays,
											  const vector<double> &weights,
											  const vector<double> &delta_faradays,
											  const double lambdaZero);

  // Clean a RM vector line-of-sight down to threshold
  int RMClean(vector<double> &phis, double threshold);

  // Clean a RM vector line-of-sight down to complex threshold (for Q and U)
  int RMClean(complex<vector<double> > &complxrm, complex<double> threshold);

  // Clean a complex RM vector line-of-sight down to threshold (threshold is the same for Q and U)
  int RMClean(complex<vector<double> > &complxphis, double threshold);

  // Frick and Stepanov wavelet algorithm
  vector<double> wavelet(vector<double> &,
			 vector<complex<double> > &,
			 vector<double> &,
			 vector<double> &,
			 vector<double> &,
			 vector<double> &);

  // En&szlig;lin Information Theory Based algorithm	(will be in separate wienerfilter class)
  vector<double> ift(vector<double> &,
		     vector<double> &);

  //! (Forward) Fourier Transform to get an image from an RM cube
  //vector<double> fourierTransform(vector<double> &, vector<double> &, bool freq=true);

  
  // Read frequency / lambda squared distributions from text files
  //! read frequency distribution from a text file
  vector<double> readFrequencies(const std::string &);

  //! read lambda squareds from a text file
  vector<double> readLambdaSquareds(const std::string &);

  //! read frequencies from file and compute delta frequencies from differences
  vector<double> readFrequencies(const std::string &filename, vector<double> &deltafreqs);

  //! read frequencies and delta frequencies from a text file
  vector<double> readFrequenciesAndDeltaFrequencies(const std::string &, vector<double> &);

  //! read lambda squareds and delta squareds from a text file
  vector<double> readLambdaSquaredsAndDeltaSquareds(const std::string &, vector<double> &);

  //! Write a vector out to file
  void writeRMtoFile(vector<double>, const std::string &filename);

  //! Write a complex vector out to a file
  void writeRMtoFile(vector<complex<double> > rm, const std::string &filename);

  //! Write the lambda squareds ann the complex vector rm out to a file
  void writeRMtoFile(vector<double> &lambdasq, vector<complex<double> > &rm, const std::string &filename);


  // Atmospheric correction of RM (contribution through atmosphere)
  // ??
	
  //! Calculcate Faraday rotation RM contribution by atmosphere
  double atmosphericFaradayRotation(vector<double> &tec);
  
  //! Rotation Measure error estimation algorithms


  //! Variance of the lambda squared distribution (helper function for rmErrorLsq)
  double lambdaSqVariance(vector<double> &lambdaSq);

  //! (assuming) a least squares error approximation
  double rmErrorLsq(double rmsnoisechan, double lambdaSqVariance, int Nchan, double P);

  //! overloaded ErrorLsq function calling lambdaSqVariance internally
  double rmErrorLsq(double rmsnoisechan, vector<double> &lambdaSqs, double P);

  //! Error estimate based on Bayesian statistics
  vector<double> rmErrorBayes(vector<complex<double> > &intensities,
			      vector<double> &lambda_sqs,
			      bool lambda_sq=true);
};

#endif		// _RM_H_
