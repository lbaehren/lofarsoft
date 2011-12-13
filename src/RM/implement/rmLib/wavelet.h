#ifndef WV_H
#define WV_H
#include <stdlib.h>
#include <vector>
//#include <iostream>			// streamed input output for rm cubes
#include <complex>			// complex math functions

using namespace std;

// ==============================================================================
//
//  Class: wavelet
//
// ==============================================================================

/*!
  \class wavelet
  
  \ingroup RM
  
  \brief Wavelet algorithms class
  
  \author Anton Chupin (chupin@icmm.ru)
  
  \date 27.06.2011
  
  \test twavelet.cc
      
  <h3>Synopsis</h3>

  The wavelet class provides algorithms for computing the direct and inverse
  wavelet transform from an input one-dimensional array. 
*/ 

double cabs(complex<double> z);

class wavelet {
  
  //! Number of wavelet
  unsigned nw;
  //! Normalisation constant for wavelet
  double Cpsi;
  
  complex<double> Haar(double x);			// real wavelet - Haar
  complex<double> MexicanHat(double x);	// real wavelet - Mexican Hat
  complex<double> Morlet(double x);		// complex wavelet - Morlet
  complex<double> wvf(double x);			// used wavelet function
  // their fourier transforms:
  complex<double> Haar_f(double x);
  complex<double> MexicanHat_f(double x);
  complex<double> Morlet_f(double x);
  complex<double> wvf_f(double x);
  
 public:
  // parameters and calculated data
  double amin, amax, adelta;			// Boundaries and delta in logarithmic scale (base 2)
  double log2;
  unsigned an;						// Number of scales (computable)
  //! Boundaries and delta in shift
  double bmin, bmax, bdelta;
  //! Number of shifts (computable)
  unsigned bn;
  //! 2d-array of wavelet coefficients
  complex<double> **wc;
  //! Scales
  vector<double> as;
  //! Shifts
  vector<double> bs;
  
  // === Procedures =============================================================

  //! Simple constructor, needs initialisation of array to use
  wavelet();
  //! Constructor with main parameters
  wavelet (double amin,
	   double amax, 
	   double adelta, 
	   double bmin,
	   double bmax,
	   double bdelta);
  //! Choose the number of using wavelet
  double useWavelet(int number);
  void transform(vector<complex<double> > y);					// direct wavelet transform, result in the wc array
  void transform(vector<complex<double> > y,vector<double> x);	// direct wavelet transform, result in the wc array
  void ftransform(vector<complex<double> > y,vector<double> x);// direct wavelet transform of fourier image, result in the wc array
  vector<complex<double> > invTransform(vector<double> x);		// inverse wavelet transform from the wc array
  ~wavelet();						// destructor, releases used memory
};

// ==============================================================================
//
//  Class: WaveletSynthesis
//
// ==============================================================================

/*!
  \class WaveletSynthesis
  
  \ingroup RM
  
  \brief Wavelet RM-synthesis
  
  \author Anton Chupin (chupin@icmm.ru)
  
  \date 28.06.2011
  
  \test twvsynt.cc
  
  <h3>Synopsis</h3>
  
  The wavelet synthesis class provides algorithms for RM-synthesis of polarization data
  from an input one-dimensional array. 
    
*/ 

class WaveletSynthesis
{
private:
	bool dataReady;					// has data been read from file?

public:
	wavelet wplus, wminus, wres;	// arrays of wavelet coefficients (direct, symmetry, result)
	complex<double>*** data;		// input data from FITS files
	vector<double> freqs;			// list of observed frequences
	vector<double> lamsq;			// list of lambda-square

	WaveletSynthesis();
	WaveletSynthesis(double amin, double amax, double adelta, 	// constructor with main parameters
		double bmin, double bmax, double bdelta);
	void dataLoad(string fname);								// reads data from FITS file (or call another utility)
	vector<complex<double> > getLine(int i, int k);				// extract 1 line of sight
	void doSymmetry(int b0, int size);							// perform symmetry reflection
	void analys(vector<complex<double> > line, wavelet &wres);	// wavelet analysis stage
	vector<complex<double> > synth(wavelet &w, vector<double>fs);// wavelet synthesis stage
	~WaveletSynthesis();										// destructor, releases memory
};

// ==============================================================================
//
//  Class: WaveletClean
//
// ==============================================================================

/*!
  \class WaveletClean, derived from WaveletSynthesis
  
  \ingroup RM
  
  \brief Wavelet RM-cleaning procedure
  
  \author Anton Chupin (chupin@icmm.ru)
  
  \date 29.06.2011
  
  \test twvclean.cc
      
  <h3>Synopsis</h3>

  The wavelet cleaning class provides algorithm for RM-cleaning of polarization data
  from an input one-dimensional array. It finds a maximum in the wavelet plane, then
  subtracts ("cleans") RMSF corresponding to the source point positioned in this maximum,
  and calculates some characteristics in order to know if the residue is underlevel noise.
    
*/ 

class WaveletClean : public WaveletSynthesis
{
 private:
  double wmaxinit;
  void perform();						// one step of cleaning
  vector<complex <double> > signal;

 public:

  //! Wavelet coefficients of initial Q+iU
  wavelet data;
  //! Wavelet coefficients of point source
  wavelet RMSF;
  //! Faraday depths for restored (cleaned) signal
  vector<double> depths;

  //! Restored (cleaned) signal
  vector<complex <double> > res;
  
  //! Constructor automatically do wavelet transform
  WaveletClean (vector<complex<double> > s,
		vector<double> lam);
  //! Constructor, with default amin=-10,amax=15,adelta=1, bmin=-20, bmax=150, bdelta=1
  WaveletClean (vector<complex<double> > s,
		vector<double> lam,
		double amin,
		double amax,
		double adelta, 
		double bmin,
		double bmax,
		double bdelta);
  void init(vector<complex<double> > s, vector<double> lam, vector<double> fd, int numw);
  // calculates wavelet coefficients of signal
  void perform(int times);			// main intro point - calls "times" steps of cleaning
  unsigned int getMaxPos(unsigned int &a, unsigned int &b);	// finds position of maximum in data by default
  unsigned int getMaxPos(wavelet &w, unsigned int &a, unsigned int &b);	// finds position of maximum
  double chi2();						// chi-square criterion (requires inverse transform!)
  double ratio();						// ratio of residue and initial signal
};

#endif		// _WV_H_
