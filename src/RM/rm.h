/*!
  \class rm
  
  \ingroup RM
  
  \brief Brief description for class rm
  
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
#include <iostream>			// streamed input output for rm cubes
#include <complex>			// complex math functions
#include <casa/Arrays/Array.h>		// use CASA Arrays as temporary buffers
#include <casa/Quanta/Unit.h>		// use CASA Unit class for unit handling
#include <casa/Quanta.h>		// ... and unit conversion


// Namespace usage
using namespace std;
using namespace casa;

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
  casa::Unit raUnit;
  casa::Unit decUnit;

  //! Units of input vectors (lamba squared or frequency) the cube was created from
  casa::Unit spectralUnit;

  //! Units of Faraday intensity in output cube
  casa::Unit faradayUnit;
  
  //! Convert frequency vector to lambda squared vector
  vector<double> freqToLambdaSq(const vector<double> &frequency);
  //! Convert lambda squared vector to frequency vector
  vector<double> lambdaSqToFreq(const vector<double> &lambda_sq);

  //! Calculate delta lambda squared steps from lower and higher frequency limits
  vector<double> deltaLambdaSq(	const vector<double> &freq_low, 
			const vector<double> &freq_high,
			bool freq=true);

  // Public functions.
public:
  //! Calculate integrated total intensity along one line of sight
  vector<double> totalIntensity(vector<double> &, int);

  //! Default constructor.(external buffer handling independent of cube object)
  rm(int depth, double stepsize);

  //! Constructor with associated output stream 
  rm(int, int, int, double, ofstream &);

  /*! \brief RM cube with associated Faraday buffer. One line of sight or two dimensional
  
      create a RM cube object with one (or a times b) associated Faraday line(s) (defaults: axis a=1 axis b=0)
      of sight, buffer is only given as reference
  */
  rm(int, int, int, double, Array<double> &faradaybuffer, int x=1, int y=0);
  
  
  /*! \brief RM cube with a full Faraday plane (complete RA and Dec coverage of the image)
  
      create a RM cube object with one associated Faraday plane buffer of dimensions x and y, and
      third dimension z (default 1)
 */
 rm(int, int, int, double, Array<double> &faradayplanes, int x, int y, int z=1);
  
  
  /*!
   \brief Default destructor
  */
  ~rm();
		
  //! Rotation Measure computing algorithms
  //
  //! Brentjens and de Bruyn with the Inverse Fourier Transform for a single RM value
  complex<double> inverseFourier(double &,
				 vector<complex<double> > &,
				 vector<double> &,
				 vector<double> &,
				 vector<double> &,
				 bool freq=true);

  //! Brentjens and de Bruyn with the Inverse Fourier Transform for complete set of RMs
  vector<double> inverseFourier(vector<double> &,
				vector<complex<double> > &,
				vector<double> &,
				vector<double> &,
				vector<double> &,
				bool freq=true);


  //! Frick and Stepanov wavelet algorithm
  vector<double> wavelet(vector<double> &,
			 vector<complex<double> > &,
			 vector<double> &,
			 vector<double> &,
			 vector<double> &,
			 vector<double> &,
			 bool freq=true);

  //! En&szlig;lin Information Theory Based algorithm
  vector<double> ift(vector<double>,
		     vector<double>,
		     bool freq=true);

  //! (Forward) Fourier Transform to get an image from an RM cube
  vector<double> fourierTransform(vector<double>, vector<double>, bool freq=true);

  // Atmospheric correction of RM (contribution through atmosphere)
  // ??
	
  //! Calculcate Faraday rotation RM contribution by atmosphere
  double atmosphericFaradayRotation(double tec);
  
  //! Rotation Measure error estimation algorithms

  //! (assuming) a least squares error approximation
  vector<double> rmErrorLsq(vector<double> intensities,
			    vector<double> lambda_sqs,
			    bool lambda_sq=true);
  //! Error estimate based on Bayesian statistics
  vector<double> rmErrorBayes(vector<double> intensities,
			      vector<double> lambda_sqs,
			      bool lambda_sq=true);
};

#endif		// _RM_H_
