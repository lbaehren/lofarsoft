/***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sven Duscha (sduscha@mpa-garching.mpg.de)                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*!
  \class rmclean
  \ingroup RM
  \brief RM Clean class containing different CLEAN algorithms

  \author Sven Duscha (sduscha@mpa-garching.mpg.de)
  \date 29-07-2009 (Last change: 17-08-2009)
*/

#include <vector>
#include <complex>
#include <fftw3.h>
#include "rmIO.h"

using namespace std;

class rmclean : public RM::rmIO
{
  vector<complex<double> > dirtyMap;					//! dirty Map on which is worked on
  //	vector<complex<double> > minorCleanComponent;	//! vector with a set of minor cycle CLEAN components
  vector<complex<double> > cleanComponents;			//! vector containing CLEAN components
  //	vector<complex<double> > cleanedimage;				//! final cleaned image, this will be returned by the methods, so no need to keep that as attribute in the class
  double fullwidthhalfmaximum;							//! FWHM needs to be computed only once
  double gain;												//! gainfactor for CLEAN iterations
  vector<double> gaussianSubstraction;				//! Gaussian used in substraction from dirty map
  double *gaussian;											//! non Fourier tranformed function (e.g. Gaussian / Dirac)
  double *cleanComponent;									//! single CLEAN component(s) found in minor cycle
  double *restoredComponent;								//! inverse Fourier Transformed restored Component
  
  unsigned int numIterations;							//! current number of iterations
  
  // FFTW related attributes for (inverse) Fast Fourier Transforms
  fftw_complex *FTGaussian;								//! Fourier Transform of Gaussian (with RMSF' FWHM)
  fftw_complex *FTcleanComponent;						//! Fourier Transform of a set of CLEAN components
  fftw_complex *FTrestoredComponent;					//! Fourier Transform (i.e. Convolution) of restored component
  
  fftw_plan planGaussian;									//! FFTW3 plan for Fourier Transform of Gaussian
  fftw_plan planCleanComponent;							//! FFTW3 plan for Fourier Transform of CleanComponent
  fftw_plan planRestoredComponent;						//! FFTW3 plan for Inverse Fourier Transform back into data space
  
  //	vector<complex<double> > RMSF;						//! RMSF, must be at least computed over twice the range as the data RM to be cleaned
  vector<complex<double> > FTRMSF;						//! Fourier Transform of RMSF
  vector<vector<complex<double> > > preshiftedRMSF;			//! vector containing a set of preshifted RMSFs
  
  // private helper functions
  
  bool keepshiftedRMSF;									//! keep shifted RMSF true or false
  void copyDataToDirtyMap(const vector<complex<double> > &data);							//! make a copy of data in dirtyMap vector
  
  void shiftRMSF(const unsigned int maxpos, vector<complex<double> > &shiftedRMSF);				//! shift RMSF to one phi_max position
  void computePreshiftedRMSF();							//! compute shifted RMSF for positions 0 to length of dirtyMap
  void computePreshiftedRMSF(const unsigned int length);	//! compute shifted RMSF for positions 0 to length
  
 public:
  
  // === Construction ===========================================================

  //! Create a rmclean object with length line-of-sight length
  rmclean (const unsigned int length);

  // === Destruction ============================================================
  
  ~rmclean(void);
  
  // Test functions...
  vector<complex<double> > RMSF;						//! RMSF, must be at least computed over twice the range as the data RM to be cleaned	
  //	vector<vector<complex<double> > > preshiftedRMSF;			//! vector containing a set of preshifted RMSFs
  //...//
  
  // Hogbom CLEAN algorithms, this perforrms only the Major cycle on all components
  vector<complex<double> > hogbom(const vector<complex<double> > &data, const complex<double> threshold, const unsigned int maxiter);			// RM Hogbom CLEAN function, returns the cleaned map
  vector<double> hogbom(const vector<double> &data, const double threshold, const unsigned int maxiter);												// RM Hogbom CLEAN function, returns the cleaned map, overloaded with No. of iterations instead of threshold
  
  // Clark CLEAN algorithms, this perforrms only the Major cycle on all components
  vector<complex<double> > clark(const vector<complex<double> > &data, const double threshold, const unsigned int nmaxiter);						// RM Clark CLEAN function, returns the cleaned map
  //	vector<complex<double> > clark(const vector<complex<double> > &data, const unsigned int interations);			// RM Clark CLEAN function, returns the cleaned map, overloaded with No. of iterations instead of threshold
  
  // Brentjens RM CLEAN algorithms, convolve with full RMSF function
  void rmsfClean(const vector<complex<double> > &, vector<complex<double> > &, const double threshold, const unsigned int maxiter);
  
  // Helper functions
  void createGaussian(vector<double> &, const double peak, const double fwhm, const unsigned int peakpos);					// create a Gaussian with equivalent FWHM and peak height shifted to peakpos
  void createGaussian(vector<complex<double> > &, const double peak, const double fwhm, const unsigned int peakpos);	// create a complex Gaussian with equivalent FWHM and peak height shifted to peakpos
  void createGaussian(vector<double>  &, const double peak, const double fwhm);											// create a Gaussian with equivalent FWHM and peak height
  void createGaussian(vector<complex<double> > &, const double peak, const double fwhm);											// create a complex Gaussian with equivalent FWHM and peak height
	double* createGaussianArray(const int length, const double peak, const double fwhm);								// create a double array with a Gaussian function

	double FWHM(const vector<double> &data);								// determine the FWHM of a data vector
	complex<double> FWHM(const vector<complex<double> > &data);		// determine the real and imaginary FWHM of a complex data vector

	void computePreshiftedRMSF(vector<int> shiftpositions);			// compute
	
	// FFT helper functions with vectors
	void fft_real(vector<double> &, vector<double> &);
	void fft(vector<double> &, vector<complex<double> > &);	
	void fft(vector<complex<double> > &, vector<complex<double> > &);

	void ifft_real(vector<double> &, vector<double> &);
	void ifft(vector<complex<double> > &, vector<double> &);
	void ifft(vector<complex<double> > &, vector<complex<double> > &);
	
	void convolution(vector<complex<double> > &a , vector<complex<double> > &b, vector<complex<double> > &c);

	void setPlan();
	
	// Member access functions
//	vector<complex<double> > getCleanedMap();				// return the final cleaned image
	vector<complex<double> > getCleanComponents();		// return vector with CLEAN components
	double getFWHM();												// get the calculated FHWM
	void setFWHM(double fwhm);									// set the FWHM value
	double getGain();												// get the currently set gain for cleaning
	void setGain(double gain);									// set the gain for cleaning
	unsigned int getNumIterations();							// get current number of iterations

	bool getKeepShiftedRMSF();									// get truth variable if shifted RMSF should be kept in vector
	void setKeepShiftedRMSF(bool);							// set truth variable if shifted RMSF should be kept in vector
};
