// Test and development of an RM CLEAN to later clean RM cubes
//
//	File:				rmclean.cpp
//	Author:			Sven Duscha (sduscha@mpa-garching.mpg.de)
//	Date:				05-07-2009
// Last change:	22-08-2009


#include <iostream>
#include <algorithm>
#include <math.h>
#include <limits>				// maximum value for variables on architecture/compiler
#include <fftw3.h>
#include "rmclean.h"

using namespace std;


/*!
	\brief Default constructor
	
	\param length - length of FFTs, i.e. number of lambda squared channels
*/
rmclean::rmclean(const unsigned int length)
{
	numIterations=0;			// initialize current number of iterations

	if(length==0)
		throw "rmclean::rmclean length is 0";
	
	//-----------------------------------------------------
	
	// Both input and output arrays have the same length, this corresponds to the number
	// of Faraday Depths calculated along the line of sight
	// allocate memory for algorithm variables
	gaussian = static_cast<double*> (calloc(sizeof(double), length));
	cleanComponent = static_cast<double*> (calloc(sizeof(double), length));
	restoredComponent = static_cast<double*> (calloc(sizeof(double), length));
	dirtyMap.resize(length);
	
	FTGaussian = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*length);
	FTcleanComponent = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*length);
	FTrestoredComponent = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*length);

	// Use FFTW_MEASURE, because this plan will be executed millions of times (for every
	// line-of-sight) and we want to have the best performing FFT
	planGaussian=fftw_plan_dft_r2c_1d(length, gaussian, FTGaussian, FFTW_MEASURE);
	planCleanComponent=fftw_plan_dft_r2c_1d(length, cleanComponent, FTcleanComponent, FFTW_MEASURE);
	planRestoredComponent=fftw_plan_dft_c2r_1d(length, FTrestoredComponent, restoredComponent, FFTW_MEASURE);
}


/*!
	\brief Destructor of rmclean, frees memory of CLEAN variables
*/
rmclean::~rmclean(void)
{
	
	// clean up FFTW variables
	fftw_destroy_plan(planGaussian);
	fftw_destroy_plan(planCleanComponent);
	fftw_destroy_plan(planRestoredComponent);
	fftw_free(FTGaussian);
	fftw_free(FTcleanComponent);
	
	
	free(gaussian);
	free(cleanComponent);
	free(FTrestoredComponent);
}


//*****************************************************************
//
// 						Algorithm functions
//
//*****************************************************************


/*
	\brief Hogbom CLEAN on a complex RM data vector, uses either threshold or number of iterations as break condition
	
	\param data - RM data to be cleaned
	\param threshold - noise threshold after which CLEAN stops
	\param maxiter - or maximum number of iterations after CLEAN stops (default=0 i.e. work down to noise level)
	
	\return cleaned RM vector - complex vector of cleaned palarized RM
*/
vector<complex<double> > rmclean::hogbom(const vector<complex<double> > &data, const complex<double> threshold, const unsigned int maxiter=0)
{
	vector<complex<double> > cleanedImage(data.size());
	
	//-------------------------------------------------------------
	// Data integrity checks
	//
	if(data.size()==0)
		throw "rmclean::hogbom data has size 0";
	if(threshold.real()<=0 || threshold.imag()<=0)
		throw "rmclean::hogbom threshold <= 0";
	if(cleanedImage.size()!=data.size())
		throw "rmclean::hogbom cleanedimage size is different from data size";
		
	//-------------------------------------------------------------
	if(maxiter==0)			// if no maximum number of iterations was given...
	{
		// clean down to threshold level
	}
	else
	{
		for(numIterations=0; numIterations < maxiter; numIterations++)
		{
			
		}
	}
	
	// create Gaussian with that FWHM
	
	
	// Major cycle: Hogbom only has major cycle
	
	// convolve clean components with Gaussian
	


	return cleanedImage;						// return fully cleaned image
}


/*!
	\brief Hogbom CLEAN on a (Q or U) RM data vector, uses either threshold or number of iterations as break condition

	\param data - RM data to be cleaned
	\param threshold - noise threshold after which CLEAN stops
	\param maxiter - or maximum number of iterations after CLEAN stops (default=0 i.e. work down to noise level)

	\return cleaned RM vector - vector of cleaned single (Q or U) palarized RM
*/
vector<double> rmclean::hogbom(const vector<double> &data, const double threshold, const unsigned int maxIterations=0)
{	
	vector<double> cleanedMap(data.size());	// vector that will hold the final cleaned map
//	vector<double> dirtyMap(data);				// create a copy of data, called the dirty map, to work on
//	vector<double> cleanComp(data);				// vector containing single CLEAN component of cycle
//	vector<double> gaussian(data.size());		// vector to hold Gaussian to convolve CLEAN components with
//	vector<double> gaussianFT(data.size());			// Fourier transformed gaussian
//	vector<double> cleancomponentFT(data.size());	// Fourier transformed CLEAN component
	vector<double> restoredComponent(data.size());

	// Helper variables
	unsigned int length=data.size();				// length of data vector
	double fwhm=0;										// FWHM of RMSF used to create Gaussian
	double max=numeric_limits<double>::max( );		// initiliaze with architecture/compiler maximum value
	unsigned int maxpos=0;							// position of maximum found
	
	double *Gauss=NULL, *Dirac=NULL;				// input arrays for FFTW FT
	fftw_complex *fftwGauss, *fftwDirac;		// output arrays for FFTW FT
	fftw_plan fftwplanGauss, fftwplanDirac;	// fftw_plans
	
	//-------------------------------------------------------------
	// Data integrity checks
	//
	if(data.size()==0)
		throw "rmclean::hogbom data has size 0";
	if(threshold<=0)
		throw "rmclean::hogbom threshold <= 0";
	if(gain<=0)
		throw "rmclean:hogbom gain <= 0";
	if(gain>=2)
		throw "rmclean::hogbom gain > 2";
	
	//-------------------------------------------------------------
	//
	// Work on only one part of the cleaned Image
	//
	//-------------------------------------------------------------
	
	// Major cycle: Hogbom only has major cycle:
	while(max > threshold)		// as long as the maximum found is above the threshold
	{
		// Maximum number of iterations break condition, if these were set
		if(maxIterations!=0 && numIterations > maxIterations)
			break;													// stop cleaning loop
		
		// Find peak in dirty image, break when peak intensity < threshold (1.2*noise level)
		maxpos=max_element(data.begin(), data.end())-data.begin();
		max=data[maxpos];

		// multiply peak intensity with reduction factor gamma = 0.4
		cleanComponent[maxpos]=gain*max;									// single CLEAN component for FFT
		cleanComponents[maxpos]=gain*max;								// Set clean component in cleanmap
		
		gaussian=createGaussianArray(length, max, fwhm);			// create Gaussian (Array) with that FWHM

		fftw_execute(planGaussian);										// Fourier transform Gaussian
		fftw_execute(planCleanComponent);								// Fourier transform CLEAN component
 

		// Convolve Gaussian with Dirac in Fourier Space (Multiplication)
		for(unsigned int i=0; i < length; i++)			// element-wise multiplication
		{
			restoredComponent[i]=FTcleanComponent[i][0]*FTGaussian[i][0];		// multiply only real part
		}
		
		numIterations++;			// increment iterations counter
		
		writeRMtoFile(restoredComponent, "fftrestored.dat");
		// Inverse Fourier Transform back into data space:
		fftw_execute(planRestoredComponent);
		
		writeRMtoFile(restoredComponent, "restored.dat");	
	}	// CLEAN algortihm loop over all found peaks
	

		
	// convolve clean componentswith Gaussian
	
	// Add convolved function to dirty image 
	
	// multiply peak intensity with reduction factor gamma = 0.4
	
	// set clean component at peak position in cleaned image	
	
	
	return cleanedMap;
}


/*!
	\brief CLEANing by convolving with the full RMSF, in a hogböm-like minor-only cycle
	
	\param data - complex RM vector to be cleaned
	\param cleanedMap - complex vector of cleaned RM
	\param threshold - threshold to clean down to
	\param maxinterations - optional parameter to limit algorithm to a maximum number of iterations	
*/
void rmclean::rmsfClean(const vector<complex<double> > &data, vector<complex<double> > &cleanedMap, const double threshold, const unsigned int maxIterations=0)
{
	double max;									// initiliaze with architecture/compiler maximum value
	unsigned int maxpos=0;					// position of maximum found
	unsigned int length=data.size();
	vector<double> power(data.size());	// vector to contain absolute power of RM data vector
	vector<complex<double> > shiftedRMSF(data.size());
	
	//------------------------------------------------------
	// Data integrity checks
	if(data.size()==0)
		throw "rmclean::rmsfClean data vector has size 0";
	if(RMSF.size()<2*data.size())
		throw "rmclean::rmsfClean RMSF should be at least twice the size of data vector";
	if(threshold==0)
		throw "rmclean::rmsfClean threshold is 0";
		
	//------------------------------------------------------
	max=numeric_limits<double>::max( );		// set maximum to local numerical max value

	copyDataToDirtyMap(data);

	// Check for both real and imaginary (Q and U) components to be above threshold
	//while(max > threshold)
	for(unsigned int i=0; i < maxIterations; i++)
	{	
		// Compute absolute power of dirtyMap vector to look for maximum
		for(unsigned int i=0; i < length; i++)
		{
			power[i]=sqrt(dirtyMap[i].real()*dirtyMap[i].real() + dirtyMap[i].imag()*dirtyMap[i].imag());
		}
		
		// Find peak in dirty image, break when peak intensity < threshold (1.2*noise level)
		maxpos=max_element(power.begin(), power.end())-power.begin();
		max=power[maxpos];

		// Maximum number of iterations break condition, if these were set
		if(max < threshold)
			break;													// stop cleaning loop		
		
		// Shift RMSF to peak position of maxpos: R(phi-phi_max,k)
		// If a preshifted entry for that maxpos exists use preshiftedRMSF table
		if(preshiftedRMSF[maxpos].size()==RMSF.size())
		{
			shiftedRMSF=preshiftedRMSF[maxpos];
		}
		else	// if there is no preshifted RMSF at that position, compute it now (and store it in preshiftedRMSF vector)
		{
			shiftRMSF(maxpos, shiftedRMSF);

			if(keepshiftedRMSF==true)						// if variable to precompute RMSF is set to true
				preshiftedRMSF[maxpos]=shiftedRMSF;		// shiftRMSF and store it in vector of precomputed RMSFs
		}
		
		
		// Add newly computed dirty map component to cleanedMap (="Model Map")
		// M[phi_max,k] = M[phi_max,k] + gain * dirtyMap[phi_max,k]
		for(unsigned int i=0; i < length; i++)
		{
			cleanedMap[maxpos]=cleanedMap[maxpos]+gain*dirtyMap[maxpos];
		}
		
		cout << "shiftedRMSF.size() = " << shiftedRMSF.size() << endl;
		
		// Substract RMSF scaled by maximum peak times gain factor from the dirtyMap
		// F_{k+1}[phi] =  F_{k}[phi] - gain*F_{k}[phi]*shiftedRMSF[i]		
		for(unsigned int i=0; i < length; i++)
		{
			dirtyMap[i]=dirtyMap[i]-gain*dirtyMap[i]*shiftedRMSF[i];
		}
	}
}


/*!
	\brief Copy data to dirtyImage
	
	\param data - RM data vector to be copied to the dirtyMap
*/
void rmclean::copyDataToDirtyMap(const vector<complex<double> > &data)
{
	dirtyMap=vector<complex<double> >(data);
}


/*
	\brief Determine the FWHM of a data vector
	
	\param data - Data to determine the FWHM for
	
	\return fwhm - the Full Width Half Maximum as a double
*/
double rmclean::FWHM(const vector<double> &data)
{
	double fwhm=0;								// FWHM to be returned
	double max=0;								// peak value
	int maxpos=0;								// peak value index position
	unsigned int currleftpos=0;			// current left position in data vector
	unsigned int currrightpos=0;			// current right position in data vector	
	
	maxpos=max_element(data.begin(), data.end())-data.begin();
	max=data[maxpos];
	
	// walk in both direction till half-maximum value is reached
	currleftpos=maxpos;							// initialize position to peak
	currrightpos=maxpos;							// initialize position to peak
	while(data[currleftpos] > 0.5*max)		// as long as we haven't reached half maximum value
	{
		currleftpos--;								// move to the left
	}
	while(data[currrightpos] > 0.5*max)
	{
		currrightpos++;							// move to the right
	}
	
	// FWHM = 0.5*((maxpos-currleftpos)+(currrightpos-maxpos))	
	fwhm=(maxpos-currleftpos)+(currrightpos-maxpos);
	
	if(fwhm==0)
		throw "rmclean::FWHM fwhm resulted in 0";
	if(fwhm<0)
		throw "rmclean::FWHM fwhm resulted in < 0";
		
	return fwhm;
}


/*
	\brief Determine the FWHM of a complex data vector separately for real and imaginary part
	
	\param data - Data to determine the FWHM both in real and imaginary part separately
	
	\return fwhm - the Full Width Half Maximum as a double
*/
complex<double> rmclean::FWHM(const vector<complex<double> > &data)
{
	complex<double> fwhm;			// FWHM to be returned
//	double maximum=0;					// peak value
//	int maxpos=0;						// index position of the peak value

	// Data integrity checks
	if(data.size()==0)
		throw "rmclean::FWHM data vector has size 0";

	//-----------------------------------------
	vector<double> data_real(data.size());		// real part of data vector
	vector<double> data_imag(data.size());		// imaginary part of data vector

	// Copy data over to local real and imaginary copies
	for(unsigned int i=0; i<data.size(); i++)
	{
		data_real[i]=data[i].real();
		data_imag[i]=data[i].imag();
	}

	// determine FWHM of PSF for real and imaginary part separately
	fwhm.real()=FWHM(data_real);
	fwhm.imag()=FWHM(data_imag);
	
	return fwhm;
}


/*
	\brief Create a Gaussian over length with given peak value and FWHM, divided to one half
	at the beginning and another half and the end of the vector (used for Fourier Transform)
	
	\param gaussian - gaussian vector to hold Gaussian created
	\param peak - peak value of Gaussian
	\param fwhm - Full Width Half Maximum of Gaussian to create
	
	\return gaussian - gaussian with equivalent FWHM and peak value
*/
/* Later migrate this to casacore Gaussian1D function?
	casa::Gaussian1D< T >::Gaussian1D	(	const T & 	height,
	const T & 	center,
	const T & 	width	 
	)			 [inline]
*/
void rmclean::createGaussian(vector<double> &gaussian, const double peak, const double fwhm)
{
//	vector<double> gaussian(length);			// vector to keep Gaussian data
	double length=gaussian.size();			// length of Gaussian to be created
	double sigma=fwhm/(2*sqrt(2*log(2)));	// standard deviation sigma dependence on fwhm
	double normalizationfactor=1, factor=0;				// normalization factor to scale Gaussian to desired peak value
	double sigmafactor=0;						// 2*sigma*sigma division in exponential

	// Check for data consistency
	if(gaussian.size()==0)
		throw "rmclean::createGaussian gaussian vector has length 0";
	if(peak<=0)
		throw "rmclean::createGaussian peak=0";
	if(fwhm<=0)
		throw "rmclean::createGaussian fwhm<=0";
		
	//----------------------------------------
	// Create Gaussian with specified FWHM, peak and peakpos (=mean)
	//
	// Compute normalization and common factor first
	normalizationfactor=peak/(1/(sigma*sqrt(2*M_PI)));
	factor=normalizationfactor*(1/(sigma*sqrt(2*M_PI)));
	sigmafactor=1/(2*sigma*sigma);

//	cout << "normalizationfactor = " << normalizationfactor << "   factor = " << factor << endl;
//	cout << "sigma = " << sigma << endl;

	factor=normalizationfactor*(1/(sigma*sqrt(2*M_PI)));	// part of formula independent of i
	for(int i=0; i < floor(length/2); i++)
	{
		gaussian[i]=factor*exp(-sigmafactor*(i*i));	
//		gaussian[i]=factor*exp(-pow((double)i,2)/(2*pow(sigma,2)));
//		cout << gaussian[i] << endl;
	}
	for(int i=ceil(length/2); i < length; i++)
	{
		gaussian[i]=factor*exp(-sigmafactor*((length-i)*(length-i)));
//		cout << gaussian[i] << endl;
	}
	
	//------------------------------------------------------------
	// Period Gaussian with specified FWHM and peak
	//
	// Compute normalization and common factor first
	/* This was a Fourier-periodicity shifted Gaussian
	normalizationfactor=peak/(1/(sigma*sqrt(2*M_PI)));
	factor=normalizationfactor*(1/(sigma*sqrt(2*M_PI)));

	// Due to peridiocity in Fourier space, split Gaussian and go down from peakvalue to 0
	// on the left and up at the end of the interval
	for(unsigned int i=0; i < floor(length/2); i++)
	{
		gaussian[i]=factor*exp(-pow((double)i,2)/(2*pow(sigma,2)));
	}
	for(unsigned int i=ceil(length/2); i < length; i++)
	{
		gaussian[i]=factor*exp(-pow((double)(length-i),2)/(2*pow(sigma,2)));
	}
	*/
}


/*
	\brief Create a Gaussian over length with given peak value and FWHM at peakpos position
	
	\param gaussian - gaussian vector to hold Gaussian created
	\param peak - peak value of Gaussian
	\param fwhm - Full Width Half Maximum of Gaussian to create
	\param peakpos - position of Gaussian peak
	
	\return gaussian - gaussian with equivalent FWHM and peak value
*/
void rmclean::createGaussian(vector<double> &gaussian, const double peak, const double fwhm, 
	const unsigned int peakpos)
{
//	vector<double> gaussian(length);			// vector to keep Gaussian data
	double length=gaussian.size();			// length of Gaussian to be created
	double sigma=fwhm/(2*sqrt(2*log(2)));	// standard deviation sigma dependence on fwhm
	double normalizationfactor=1, factor=0;				// normalization factor to scale Gaussian to desired peak value
	double sigmafactor=0;						// 2*sigma*sigma division in exponential
	int mean=peakpos;

	// Check for data consistency
	if(gaussian.size()==0)
		throw "rmclean::createGaussian gaussian vector has length 0";
	if(peak<=0)
		throw "rmclean::createGaussian peak=0";
	if(fwhm<=0)
		throw "rmclean::createGaussian fwhm<=0";
	if(peakpos>length)
		throw "rmclean::createGaussian peak position > length";	
		
	//----------------------------------------
	// Create Gaussian with specified FWHM, peak and peakpos (=mean)
	//
	// Compute normalization and common factor first
	normalizationfactor=peak/(1/(sigma*sqrt(2*M_PI)));
	factor=normalizationfactor*(1/(sigma*sqrt(2*M_PI)));
	sigmafactor=1/(2*sigma*sigma);

//	cout << "normalizationfactor = " << normalizationfactor << "   factor = " << factor << "   sigmafactor = " << sigmafactor << endl;
//	cout << "mean = " << mean << endl;
	for(int i=0; i < length; i++)
	{
		gaussian[i]=factor*exp(-sigmafactor*(i-mean)*(i-mean));
	}
}


/*
	\brief Create a complex Gaussian over length with given peak value and FWHM at peakpos position
	
	\param gaussian - gaussian complex vector to hold Gaussian created for Q and U
	\param peak - peak value of Gaussian
	\param fwhm - Full Width Half Maximum of Gaussian to create
	\param peakpos - position of Gaussian peak
	
	\return gaussian - gaussian with equivalent FWHM and peak value
*/
void rmclean::createGaussian(vector<complex<double> > &gaussian, const double peak, const double fwhm, 
	const unsigned int peakpos)
{
//	vector<double> gaussian(length);			// vector to keep Gaussian data
	double length=gaussian.size();			// length of Gaussian to be created
	double sigma=fwhm/(2*sqrt(2*log(2)));	// standard deviation sigma dependence on fwhm
	double normalizationfactor=1, factor=0;				// normalization factor to scale Gaussian to desired peak value
	double sigmafactor=0;						// 2*sigma*sigma division in exponential
	unsigned int mean=peakpos;

	// Check for data consistency
	if(gaussian.size()==0)
		throw "rmclean::createGaussian gaussian vector has length 0";
	if(peak<=0)
		throw "rmclean::createGaussian peak=0";
	if(fwhm<=0)
		throw "rmclean::createGaussian fwhm<=0";
	if(peakpos>length)
		throw "rmclean::createGaussian peak position > length";	
		
	//----------------------------------------
	// Create Gaussian with specified FWHM, peak and peakpos (=mean)
	//
	// Compute normalization and common factor first
	normalizationfactor=peak/(1/(sigma*sqrt(2*M_PI)));
	factor=normalizationfactor*(1/(sigma*sqrt(2*M_PI)));
	sigmafactor=1/(2*sigma*sigma);

	factor=normalizationfactor*(1/(sigma*sqrt(2*M_PI)));						// part of formula independent of i
	for(int i=0; i < length; i++)
	{
		gaussian[i].real()=factor*exp(-sigmafactor*(i-mean)*(i-mean));		// real part of Gaussian
//		gaussian[i].imag()=factor*exp(-sigmafactor*(i-mean)*(i-mean));		// imaginary part of Gaussian
		gaussian[i].imag()=gaussian[i].real();
	}
}


/*
	\brief Create a double array with a Gaussian over length with given peak value and FWHM
	
	\param length - length of data vector to create
	\param peak - peak value of Gaussian
	\param fwhm - Full Width Half Maximum of Gaussian to create
	
	\return *gaussian - double array with gaussian with equivalent FWHM and peak value
*/
double* rmclean::createGaussianArray(const int length, const double peak, const double fwhm)
{
	double *gaussian=NULL;			// vector to keep Gaussian data
	double sigma=fwhm/(2*sqrt(2*log(2)));	// standard deviation sigma dependence on fwhm
	double normalizationfactor=1, factor=0;				// normalization factor to scale Gaussian to desired peak value
	double sigmafactor=0;						// 2*sigma*sigma division in exponential

	// Check for data consistency
	if(length<=0)
		throw "rmclean::createGaussian length<=0";
	if(peak<=0)
		throw "rmclean::createGaussian peak<=0";
	if(fwhm<=0)
		throw "rmclean::createGaussian fwhm<=0";
		
	//----------------------------------------
	if(!(gaussian=(double*) calloc(length, sizeof(double)))) // allocate memory
	{
		throw "rmclean:createGaussian could not allocate memory for Gaussian";
	}
	
	//------------------------------------------------------------
	// Period Gaussian with specified FWHM and peak
	//
	// Compute normalization and common factor first
	normalizationfactor=peak/(1/(sigma*sqrt(2*M_PI)));
	factor=normalizationfactor*(1/(sigma*sqrt(2*M_PI)));
	sigmafactor=1/(2*sigma*sigma);
	
	for(int i=0; i < floor(length/2); i++)
	{
		gaussian[i]=factor*exp(-sigmafactor*i*i);
	}
	for(int i=ceil(length/2); i < length; i++)
	{
		gaussian[i]=factor*exp(-sigmafactor*(length-i)*(length-i));
	}

	return gaussian;
}


/*
	\brief Create a complex Gaussian for real and imaginary part over length with given peak value and FWHM
	
	\param gaussian - gaussian vector to create
	\param peak - peak value of Gaussian
	\param fwhm - Full Width Half Maximum of Gaussian to create
*/
void rmclean::createGaussian(vector<complex<double> > &complexGaussian, const double peak, const double fwhm)
{
//	vector<complex<double> > complexGaussian(length);
	int length=complexGaussian.size();								// keep length for convenience
//	vector<double> realGaussian(complexGaussian.size());
//	vector<double> imagGaussian(complexGaussian.size());
	double sigma=fwhm/(2*sqrt(2*log(2)));							// standard deviation sigma dependence on fwhm
	double normalizationfactor=0;										// normalization factor to scale Gaussian to desired peak value
	double factor=0;
	double sigmafactor=0;												// 2*sigma*sigma division in exponential
	
	//--------------------------------------------------------
	// Data consistency checks
	//
	if(complexGaussian.size()==0)
		throw "rmclean::createGaussian complex Gaussian vector size is 0";
	if(peak<=0)
		throw "rmclean::createGaussian peak value <= 0";
	if(fwhm<=0)
		throw "rmclean::createGaussian fwhm <= 0";
	
	//------------------------------------------------------------
	// Period Gaussian with specified FWHM and peak
	//
	// Compute normalization and common factor first
	normalizationfactor=peak/(1/(sigma*sqrt(2*M_PI)));
	factor=normalizationfactor*(1/(sigma*sqrt(2*M_PI)));
	sigmafactor=1/(2*sigma*sigma);
	
	for(int i=0; i < floor(length/2); i++)
	{
		complexGaussian[i].real()=factor*exp(-sigmafactor*i*i);
		complexGaussian[i].imag()=complexGaussian[i].real();
	}
	for(int i=ceil(length/2); i < length; i++)
	{
		complexGaussian[i].real()=factor*exp(-sigmafactor*(length-i)*(length-i));
		complexGaussian[i].imag()=complexGaussian[i].real();
	}
}


//************************************************************
//
// Shift RMSF functions
//
//************************************************************


/*!
	\brief Shift the RMSF to a maximum given at maxpos and store it in shiftedRMSF
	
	\param maxpos - position of maximum to shift RMSF to
	\param shiftedRMSF - vector to hold shifted RMSF
*/
void rmclean::shiftRMSF(const unsigned int maxpos, vector<complex<double> > &shiftedRMSF)
{
	const unsigned int length=shiftedRMSF.size();
	int shift=0;								// shift RMSF by this (depending on length and maxpos)
	int sizedifference=0;					// difference in size of RMSF and dirtyMap

	if(RMSF.size()==0)		// if RMSF has zero size, i.e. is not computed yet
		throw "rmclean::shiftRMSF RMSF has size 0";
	else if(RMSF.size() < 2*dirtyMap.size())
		throw "rmclean::shiftRMSF RMSF must be at least twice the size of the dirtyMap for shifting";
	else if(maxpos > dirtyMap.size())
		throw "rmclean::shiftRMSF requested shift exceeds dirtyMap";

	//-------------------------------------------------------------------
	// Calculate size difference of RMSF and dirtyMap to map RMSF correctly
	sizedifference=round(0.5*(RMSF.size()-dirtyMap.size()));
	// Calculate shift from maxpos and length, need to round if length is odd
	shift=round(maxpos-0.5*dirtyMap.size());

	if(shiftedRMSF.size()!=length)								// if shiftedRMSF vector has wrong size...
		shiftedRMSF.resize(length);								// ...resize it to the required length
	for(unsigned int i=0; i < length; i++)
	{
		shiftedRMSF[i] = RMSF[sizedifference+i-shift];		// shiftedRMSF = R(phi - maxpos)
	}
}


/*!
	\brief Compute preshifted RMSFs for all positions given in dirtyMap vector
*/
void rmclean::computePreshiftedRMSF()
{
	const unsigned int length=dirtyMap.size();
		
	if(dirtyMap.size()==0)
		throw "rmclean::computePreshiftedRMSF dirtyMap has size 0";

	// Resize preshiftedRMSF vector to hold length shifted RMSFs
	if(preshiftedRMSF.size()!=length)
		preshiftedRMSF.resize(length);

	for(unsigned int k=0; k < length; k++)				// loop over k positions to shift to
	{
		if(preshiftedRMSF[k].size()!=length)			// if preshiftedRMSF has wrong size...
			preshiftedRMSF[k].resize(length);			// ... then resize k-th preshiftedRMSF
		
		shiftRMSF(k, preshiftedRMSF[k]);				// compute shiftedRMSF at maxpos i
	}
}


/*!
	\brief Compute preshifted RMSFs for positions 0 to length-1
*/
void rmclean::computePreshiftedRMSF(const unsigned int length)
{
	if(length==0)
		throw "rmclean::computePreshiftedRMSF length is 0";
	
	preshiftedRMSF.resize(length);						// resize vector holding k-th shifted RMSF vector
	for(unsigned int k=0; k < length; k++)
	{
		if(preshiftedRMSF[k].size()!=length)			// if preshiftedRMSF has wrong size...
			preshiftedRMSF[k].resize(length);			// ... then resize k-th preshiftedRMSF
		
		shiftRMSF(k, preshiftedRMSF[k]);				// compute shiftedRMSF at maxpos i
	}
}


//************************************************************
//
// FFT helper functions
//
//************************************************************


/*!
	\brief FFT r2r: Real to real Fourier transform
	
	\param in - data vector to be Fourier transformed
	\param out - Fourier transformed real vector
*/
void rmclean::fft_real(vector<double> &in, vector<double> &out)
{
	
}


/*!
	\brief FFT r2r: Real to real inverse Fourier transform
	
	\param in - data vector to be inverse Fourier transformed
	\param out - Fourier transformed real vector
*/
void rmclean::ifft_real(vector<double> &in, vector<double> &out)
{

}


/*!
	\brief FFT r2c: real to complex Fourier Transform
	
	\param data - vector with data to be Fourier Transformed
	\param vector<complex> - complex Fourier Transform of data vector
*/
void rmclean::fft(vector<double> &in, vector<complex<double> > &out)
{

}


/*!
	\brief FFT c2c: complex to complex Fourier transform

	\param in - vector with data to be Fourier Transformed
	\param out - complex Fourier Transform of data vector
*/
void rmclean::fft(vector<complex<double> > &in, vector<complex<double> > &out)
{
	
	
}


/*!
	\brief Inverse FFT c2r: inverse complex vector to real Fourier transform
	
	\param &a - vector to be inverse Fourier transformed
	\param &b - inverse Fourier Transform double vector
*/
void rmclean::ifft(vector<complex<double> > &a, vector<double> &b)
{
	
}


/*!
	\brief Inverse FFT c2c: inverse complex to complex Fourier transform
	
	\param data - vector to be inverse Fourier transformed
	
	\return iFFT - inverse Fourier Transform complex<double> vector
*/
void rmclean::ifft(vector<complex<double> > &a, vector<complex<double> > &b)
{
	
	
}


/*!
	\brief Convolve vector a and b
	
	\param &a - vector<complex<double> > &a
	\param &b - vector<complex<double> > &b
	\param &c - convolution of vectors a and b, i.e. multiplication in Fourier Space
*/
void rmclean::convolution(vector<complex<double> > &a , vector<complex<double> > &b, vector<complex<double> > &c)
{
	
	
	
}


//***********************************************************
//
// Attribute access functions
//
//***********************************************************


/*!
	\brief Get CLEAN components
	
	\return cleanComponents - return the CLEAN components vector
*/
vector<complex<double> > rmclean::getCleanComponents(void)
{
	return this->cleanComponents;
}


/*!
	\brief Get the FWHM
	
	\return fwhm - Full Width Half Maximum
*/
double rmclean::getFWHM()
{
	return fullwidthhalfmaximum;
}


/*!
	\brief Set the FWHM
	
	\param fwhm - Full Width Half Maximum to set to
*/
void rmclean::setFWHM(double fwhm)
{
	if(fwhm <= 0)
		throw "rmclean::setFWHM <= 0";
	else
		this->fullwidthhalfmaximum=fwhm;
}


/*!
	\brief Get the gain
	
	\return gain - gain that is currently set for all CLEAN algorithms
*/
double rmclean::getGain()
{
	return gain;
}


/*!
	\brief Set the gain
	
	\param gain - gain value to set for all CLEAN algorithms
*/
void rmclean::setGain(double gain)
{
	if(gain<=0)
		throw "rmclean::setGain <= 0";
	else
		this->gain=gain;
}


/*!
	\brief Get current number of iterations in CLEAN loop
	
	\return numIterations - current number of iterations
*/
unsigned int rmclean::getNumIterations()
{
	return numIterations;
}


/*!
	\brief Get truth variable if shifted RMSF should be saved

	\return bool - truth variable (true or false), if shifted RMSF that are computed should be stored in preshiftedRMSF
*/
bool rmclean::getKeepShiftedRMSF()
{
	return keepshiftedRMSF;
}


/*!
	\brief Set truth variable if shifted RMSF should be saved
	
	\param bool - truth variable (true or false), if shifted RMSF that are computed are stored in preshiftedRMSF
*/
void rmclean::setKeepShiftedRMSF(bool keepRMSF)
{
	this->keepshiftedRMSF=keepRMSF;
}
