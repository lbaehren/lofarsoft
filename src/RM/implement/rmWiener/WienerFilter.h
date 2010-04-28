

// Standard header files
#include <iostream>
#include <string>
#include <vector>


#define _ITPP_	// set for the moment compilation to _ITPP_


#ifdef _ITPP_
// IT++ header files
#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#elif _ARMADILLO_
#include <armadillo>
#endif

// RM header files
#include "rmNoise.h"

using namespace std;
using namespace itpp;

#define DEBUG 1
//! statistical limit for number of line of sights before attempting a S(i,j) iteration
#define LOSLIMIT 100


namespace RM {  //  namespace RM
  
  /*!
    \class wienerfilter  
    \ingroup RM
    \brief Wiener Filter class
    
    \author Sven Duscha (sduscha@mpa-garching.mpg.de)
    
    \date 15-06-2009 (Last change: 09-11-2009)
  */
  class wienerfilter : public rmNoise
  {
    
  public:

#ifdef _ITPP_
    //! Signal matrix (complex double)
    cmat S;
    //! System response matrix (complex double)
    cmat R;
    //! (rms) noise matrix (real double)
    mat N;
    //   cmat N;	// (rms) noise matrix (complex double)
	cvec j;	// information source vector complex double vector ITTP type
    //! Data vector
    cvec d;
    //! Signal vector
    cvec s;
  
    // Intermediate products
    cmat W;	// W = R.H()*N
    cmat M;	// M = W*R
    //! Final Wiener filter matrix: WF=(inv(S) + M)*W
    cmat WF;
    //! Propagator = error of final map, intermediate result (complex double) 
    cmat D;
    //! Map error (each Faraday Depth)
    cvec maperror;
    //! Instrumental fidelity (complex double)
    cmat Q;
    //! Reconstructed map vector m
    cvec m;
#elif _ARMADILLO_
	cx_mat S;
	cx_mat R;
	mat N;
	vector<complex<double>> j;
	vector<complex<double>> d;
	vector<complex<double>> s;
	cx_mat W
	cx_mat M;
	cx_mat WF;
	cx_mat D;
	vector<complex<double>> maperror;
	cx_mat Q;
	vector<complex<double>> m;
#endif	  
    // Observational parameters
    
    //! Epsilon zero emissivity at frequency nu zero
  double epsilon_0;
  //! Frequency nu for which emissivity epsilon zero applies
  double nu_0;
  //! Spectral dependence power law factor alpha
  double alpha;
  double variance_s;							// standard deviation of s, complex (since both for Q and U)
  double lambda_phi;							// lambda nu for spectral dependence coherence length
  vector<double> lambdas;						// lambdas in data
  vector<double> lambdasquareds;				// lambda squareds in data
  vector<double> delta_lambdasquareds;				// delta lambda squareds distance between lambda squareds
  vector<double> frequencies;					// frequencies present in observation
  vector<double> delta_frequencies;				// delta frequencies distances between frequencies
  vector<double> bandwidths;					// if we have bandwidths given, different from delta frequencies
  vector<double> faradaydepths;					// Faraday depths to probe for
  vector<double> delta_faradaydepths;			// delta faradays distance between Faraday depths
  
  // Instrumental parameters
  vector<complex<double> > bandpass;			// bandpass factor for each frequency

  unsigned long maxiterations;					// maximum number of iterations to perform
  unsigned long number_of_iterations; 			// number of iterations in this Wiener filtering
  double rmsthreshold;							// rms threshold till iterations stop

  // String constants for memmber attributes (mainly file access names, initialized with default names)
  string Sfilename;			// filename for S matrix file
  string Rfilename;			// filename for R matrix file
  string Nfilename;			// filename for N matrix file
  string Dfilename;			// filename for D matrix file
  string Qfilename;  			// filename for Q matrix file
  string Sformula;			// formula for initial S guess
  string Stype;				// type of spectral dependency

  //*****************************************************
  //
  // Wiener filtering algorithm functions
  //
  //*****************************************************
  void createNoiseMatrix(double noise);		// create an instrument/simulated noise matrix, complex due to P=Q+iU and noise might differ in Q and U
  void createNoiseMatrix(vector<double> &noiseperchan);	// create a noise matrix with rms noise for each frequency channel
  void createResponseMatrix();	 							// create the Response matrix frequencies as No. of columns, Faraday depths as No. of rows
 
  void createResponseMatrixIntegral();	// integral form of Response matrix
  void createSMatrix(const string &);						// initial guess for the S matrix, Types: "whitesignal", "gaussian", "powerlaw", "deconvolution"
  void iterateSMatrix();									// adjust S matrix from result of Wiener filtering
  
  // Algorithm functions
  void computeVariance_s();				// compute dispersion_s (taken from data vector d)
  void computej();						// R^(dagger)N^(inverse)*d
  void computeD();						// D=R^(dagger)N^(inverse)R
  void computeQ();						// compute Q=RSR^(dagger)

  // Compute intermediate product matrices
  void computeW();						// W = R^{dagger}*N^{inverse}
  void computeM();						// M = W*R

  void reconstructm();					// function to reconstruct the map m

  // Functions to free memory of matrices that are not needed anymore
  void deleteN();
  void deleteR();

  void doWienerFiltering();				// do internal Wiener filtering (D, j etc.)
#ifdef _ITPP_
  void applyWF(const cvec &data, cvec &map);	// apply WF-Matrix to data vector
#elif _ARMADILLO_
  void applyWF(const std::vector<std::complex<double> > &data, std::vector<std::complex<double> > &map);
#endif
	  
public:	
  wienerfilter();						// default constructor
  wienerfilter(int nfreqs, int nfaradays);
  wienerfilter(int nfreqs, vector<double> &faradays);
  wienerfilter(int nfreqs, int nfaradays, std::string &formula);
  ~wienerfilter();							// destructor
	
  // perform Wiener Filtering on the wienerfilter object
  void wienerFiltering(double noise, const std::string &type="white");
  void wienerFiltering(double noise, double lambda_phi, double alpha=0.7);
  void wienerFiltering(double noise, double lambda_phi, double alpha, const std::string &type="white");
  void wienerFiltering(double noise, double
   lambda_phi, double epsilon_zero, double alpha=0.7);
  void wienerFiltering(double noise, double lambda_phi, double epsilon_zero, double alpha, const std::string &type="white");  
  void wienerFiltering(const vector<double> &noise, const std::string &type="white");
  void wienerFiltering(const vector<double> &noise, double lambda_phi, double alpha=0.7);
  void wienerFiltering(const vector<double> &noise, double lambda_phi, double alpha, const std::string &type="white");
  void wienerFiltering(const vector<double> &noise, double epsilon_zero, double lambda_phi, double alpha=0.7);
  void wienerFiltering(const vector<double> &noise, double epsilon_zero, double lambda_phi, double alpha, const std::string &type);

  void computeWF();	//! compute Wiener Filter operator
  
  
  // Member access functions (D and Q are results and handeled below)
//   double getVariance_s(void);					// for debugging get variance_s
  void setVariance_s(double s);						// debug: set variance_s

  double getLambdaPhi();										// get lambda_phi
  void setLambdaPhi(double lambdaphi);							// set lambda_phi
  vector<double> getLambdas();									// get lambdas vector
  void setLambdas(const vector<double> &lambdas);					// set lambdas vector
  vector<double> getLambdaSquareds();							// get lambda squareds used in data
  void setLambdaSquareds(vector<double> &lambdasquareds);		// set lambda squareds used in data
  vector<double> getDeltaLambdaSquareds();		// get the delta Lambda Squareds associated with the data
  void setDeltaLambdaSquareds(const vector<double> &delta_lambdasqs);	// set the delta Lambda Squareds associated with the data
  vector<double> getFrequencies();								// get frequencies vector wth observed frequencies
  void setFrequencies(const vector<double> &freqs);				// set frequencies vector with to observed frequencies
  vector<double> getDeltaFrequencies();							// get delta frequencies distances between observed frequencies
  void setDeltaFrequencies(const vector<double> &deltafreqs);	// set delta frequencies distances between observed frequencies
  vector<double> getFaradayDepths();							// get the Faraday depths to be probed for
  void setFaradayDepths(const vector<double> &faraday_depths);	// set the Faraday depths to be probed for

  vector<complex<double> > getBandpass();		// get bandpass for each frequency
  void setBandpass(const vector<complex<double> > &);	// set bandpass for each frequency

  double getVariance_s();					// get dispersion_s (no set function only compute above)

  double getEpsilonZero();					// get epsilon zero value of spectral dependence
  void setEpsilonZero(double);				// set epsilon zero value of spectral dependence
  double getNuZero();						// get nu zero value of spectral dependence
  void setNuZero(double);					// set nu zero value of spectral dependence
  double getAlpha();						// get alpha power law exponent of spectral dependence
  void setAlpha(double);					// set alpha power law exponent of spectral dependence
 
#ifdef _ITPP_
  cmat getSignalMatrix();					// get S signal covariance matrix
  cmat getResponseMatrix();					// get R response matrix
  mat getNoiseMatrix();						// get N noise matrix
  cvec getInformationSourceVector();				// get j information source vector

  cmat getD();								// D
  cvec computeMapError();					// Diagonal elements of D matrix
//   cvec getMapError();					// return maperror from class attributes
  vector<complex<double> > getMapError();
  cmat getInstrumentFidelity();					// Q
  cmat getWienerFilter();						// WF final Wiener Filter matrix operator

  cvec getDataVector();							// get data in data vector d
  void setDataVector(cvec &);					// set data vector to content of cvec &data
  void setDataVector(vector<complex<double> > &);			// allows to input external data into data vector
  cvec getSignalVector();									// get the signal vector
  void setSignalVector(cvec &);								// DEBUG: this function is only for debugging!
#elif _ARMADILLO_
  cx_mat getSignalMatrix();						// get S signal covariance matrix
  cx_mat getResponseMatrix();					// get R response matrix
  mat getNoiseMatrix();							// get N noise matrix
  vector<complex<double> > getInformationSourceVector();	// get j information source vector
	  
   cx_mat getD();											// D
   vector<complex<double> > computeMapError();				// Diagonal elements of D matrix
   vector<complex<double> > getMapError();
   cx_mat getInstrumentFidelity();							// Q
   cx_mat getWienerFilter();								// WF final Wiener Filter matrix operator
	  
   vector<complex<double> > getDataVector();				// get data in data vector d
   void setDataVector(vector<complex<double> > &);			// set data vector to content of cvec &data
   void setDataVector(vector<complex<double> > &);			// allows to input external data into data vector
   vector<complex<double> > getSignalVector();				// get the signal vector
   void setSignalVector(vector<complex<double> > &);							  
#endif

	  
  unsigned long getMaxIterations();				 			// get the limit of maximum iterations
  void setMaxIterations(unsigned long maxiterations);		// set the limit of maximum iterations

  unsigned long getNumberOfIterations();	// get current number of iterations

  string getSformula();						// get S matrix formula used for spectral dependence					
  string getStype();						// get type of spectral dependence

  double getRMSthreshold();					// get RMS limit threshold
  void setRMSthreshold(double rms);			// set RMS limit threshold

  // File access functions for configuration and algorithm parameters etc.
  void readConfigFile(const string &filename);					// read configuration from file
  void writeConfigFile(const string &filename);					// write configuration to file

  void readSignalMatrix(const string &filename);				// read Signal matrix from .it file
  void writeSignalMatrix(const string &filename);				// write Signal matrix to .it file
  void readNoiseMatrix(const string &filename);					// read Noise matrix from .it file
  void writeNoiseMatrix(const string &filename);				// write Noise matrix to .it file
  void readResponseMatrix(const string &filename);				// read Response matrix from .it file
  void writeResponseMatrix(const string &filename);				// write Response matrix to .it file
  void readPropagatorMatrix(const string &filename);			// read Propagator matrix from .it file
  void writePropagatorMatrix(const string &filename);			// write Propagator matrix to .it file
  void readInstrumentFidelityMatrix(const string &filename);	// read Instrument fidelity matrix from .it file
  void writeInstrumentFidelityMatrix(const string &filename);	// write Instrument fidelity matrix to .it file
  void saveAllMatrices();										// save all matrices to .it files

  //************************************************************************************
  //
  // Data I/O functions to convert to STL vector<double> and vector<complex<double> >
  //
  //************************************************************************************
  void getData(vector<complex<double> > &cmplx);			// get data from STL complex vector
  void getData(vector<double> &re, vector<double> &im);			// get data from real and imaginary STL vectors
  void exportSignal(vector<complex<double> > &cmplx);			// export signal to STL complex vector
  void exportSignal(vector<double> &re, vector<double> &im);		// export signal to real and imaginary STL vectors

  //************************************************************************************
  //
  // Functions to read in simulated data from files
  //
  //************************************************************************************
  void readLambdaSqFromFile(const std::string &filename);		// read lambda squareds from a file

  void readLambdaSqAndDeltaLambdaSqFromFile(const std::string &filename);
  void readSimDataFromFile(const std::string &filename);		// read lambda squareds and complex intensity from file
  void read4SimDataFromFile(const std::string &filename);		// read complete set of simulated data: lambdasq, delta lambda sq and complex intensity from file
  void read4SimFreqDataFromFile(const std::string &filename);		// read complete frequency (!) set of simulated data: frequencies, delta frequencies and complex intensity
  void readSignalFromFile(const std::string &filename);			// read a real signal from file
  void readFaradayDepthsFromFile(const std::string &filename);
  void readRealDataFromFile(const std::string &filename);		// read real data from a file
  void readDataFromFile(const std::string &filename);			// read a data from a file and write it into the data vector

  //************************************************************************************ 
  //
  // ASCII (GNUplot) format writing functions
  //
  //************************************************************************************

#ifdef _ITPP_
  void writeASCII(const vector<double>, const cvec &v, const std::string &filename);	// STL vector and a complex ITPP vector
  void writeASCII(const vec &v, const std::string &filename);							// output a vector to an ASCII file
  void writeASCII(const cvec &v, const std::string &filename);							// output a complex vector to an ASCII file

  // ASCII (GNUplot) with coordinates
  void writeASCII(const vec &coord, const vec &v, const std::string &filename);			// output a coordinate vector and a vector to an ASCII file
  void writeASCII(const vec &coord, const cvec &v, const std::string &filename);		// output a coordinate vector and a vector to an ASCII file

  void writeASCII(const cmat M, const std::string &filename);		// output a complex matrix M to GNUplot ASCII format
  void writeASCII(const cmat M, const std::string &part, const std::string &filename);	// output the real or imaginary part of a complex matrix to GNUplot ASCII format
	  
  // === Mathematica text format output routines ================================

  void writeMathematica (cmat M,
			 const std::string &part,
			 const std::string &filename);
#elif _ARMADILLO_
  void writeASCII(vector<double> &v, vector<complex<double> > &cv, const std::string &filename);	// output a STL vector
  // ASCII (GNUplot) with coordinates
  void writeASCII(cx_mat M, const std::string &filename);		// output a complex matrix M to GNUplot ASCII format
	  
  // === Mathematica text format output routines ================================
  void writeMathematica (cx_mat M,
							 const std::string &part,
							 const std::string &filename);	  
#endif

  void writeASCII(vector<double> &v, vector<complex<double> > &cv, const std::string &filename);	// output a STL vector
  void writeASCII(vector<double> &v, const std::string &filename);	// output a STL vector
  void writeASCII(vector<double> &v1, vector<double> &v2, const std::string &filename);	// output a STL
  void writeASCII(mat M, const std::string &filename);		// output a real matrix M to GNUplot ASCII format
  void writeMathematica (mat M, const string &filename);
  void writeASCII(vector<double> &v, const std::string &filename);	// output a STL vector
  void writeASCII(vector<double> &v1, vector<double> &v2, const std::string &filename);	// output a STL
	  
	  
  // High-level output functions working directly on class attributes: data map etc.
  // In the implementation these distinguish internally between _ITPP_ and _ARMADILLO_
  // but the function prototype stays the same of course
  //
  //! Write data vector d to an ASCII file
  void writeDataToFile(const std::string &filename);
  //! Write signal vector s to an ASCII file
  void writeSignalToFile(const std::string &filename);
  //! Write map vector m to an ASCII file
  void writeMapToFile(const std::string &filename);

  //! Create simple data
  void createData (double max,
		   double stretch,
		   double shift=0);
  //! Create a simple signal
  void createSignal (double max,
		     double stretch, 
		     double shift=0);
  //! Simple Response Matrix for testing
  void createSimpleResponseMatrix (int shift,
				   double scale);
  
  // === Helper functions =======================================================

  //! Compute P=sqrt(Q+iU)
  void complexPower(const vector<complex<double> > &signal,
		    vector<double> &power);

#ifdef _ITPP_
  //! Compute P=sqrt(Q+iU)
  void complexPower(const cvec &signal,
		    vector<double> &power);	
#endif
	  
  };
  
}  //  end namespace RM
