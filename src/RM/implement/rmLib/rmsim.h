// Create simulated RM data, used to test RM-synthesis functions
//
// File:		rmsim.h
// Author:		Sven Duscha (sduscha@mpa-garching.mpg.de)
// Date:		22-08-2009
// Last change:		14-10-2009


#ifndef RMSIM_H
#define RMSIM_


/*!
  \class RMSim 
  
  \ingroup RM
  
  \brief Simulation of Rotation Measure emission to test algorithms
  
  \author Sven Duscha (sduscha@mpa-garching.mpg.de)
  
  \test tRMSim.cpp
  
  <h3>Prerequisites</h3>
  
  - \ref rm
  
  <h3>Synopsis</h3>
  
  This class can create different Faraday emission regions along a line of
  sight vector. These may be simple Faraday screens or block structures, but
  also smoothed emission regions can be created.
  
  Gaussian noise can be added, but it is more useful to add noise to the
  Fourier transformed polarized emission.
  
  <h3>Examples</h3>
*/

#include <vector>
#include <complex>	
#include "rmnoise.h"
#include "rmio.h"
#include "rm.h"

class rmsim : public rmnoise, public rmio, public rm		// has to take public routines of rm to perform Forward transform to polarized emission
{
 private:
	std::vector<double> faradayLOS;					//! vector for simulated Faraday emission
	std::vector<std::complex<double> > polarizedInt;	//! vector for simulated polarized emission (Q and U)
	std::vector<double> faradayDepths;				//! alternative way to give position in Faraday depth instead of vector index
	std::vector<double> lambdaSquareds;				//! lambda squareds for simulated observation
	std::vector<double> deltaLambdaSquareds;		//! delta lambda squareds for simulated observation
	std::vector<double> frequencies;				//! frequencies for simulated observation 
	std::vector<double> deltaFrequencies;			//! delta frequencies for simulated observation
	std::vector<double> weights;					//! vector containing weighting

public:
	rmsim();											//! constructor
//	rmsim(std::vector<double> &, std::vector<double> &, std::vector<double> &);
//	~rmsim();											//! destructor
	
	// Algorithm functions for a complete LOS emission with multiple regions (working on class attribute vector)
	void faradayScreens(const std::vector<unsigned int> &positions, const std::vector<double> &heights, const double rmsffwhm);
	void faradayBlocks(const std::vector<unsigned int> &positions, const std::vector<unsigned int> &widths, const std::vector<double> &heights, const double rmsffwhm);
	void faradayGaussians(const std::vector<unsigned int> &positions, const std::vector<unsigned int> &fwhms, const std::vector<double> &heights, const double rmsffwhm);

	void faradayLOSresize(unsigned int);

	// Algorithm functions for a complete LOS emission with multiple regions (providing external vector)
	void faradayScreens(std::vector<double> &los, const std::vector<unsigned int> &positions, const std::vector<double> &heights, double rmsffwhm);
	void faradayBlocks(std::vector<double> &los, const std::vector<unsigned int> &positions, const std::vector<unsigned int> &widths, const std::vector<double> &heights, const double rmsffwhm);
	void faradayGaussians(std::vector<double> &los, const std::vector<unsigned int> &positions, const std::vector<unsigned int> &fwhms, const std::vector<double> &heights, const double rmsffwhm);

	// Algorithm functions to add an additional emission region to an existing LOS (working on class attribute vector)
	void addFaradayScreen(const unsigned int postion, double height, const double rmsffwhm);
	void addFaradayBlock(const unsigned int position, double height, const unsigned int width, const double rmsffwhm);
	void addFaradayGaussian(const unsigned int position, double height, const unsigned int fwhm, const double rmsffwhm);
	void addFaradaySlab(const unsigned int positionleft, const unsigned int positionright, double heightleft, double heightright, const double rmsffwhm);	

	// Algorithm functions to add an additional emission region to an existing LOS (providing external vector)
	void addFaradayScreen(std::vector<double> &los, const unsigned int postion, double height, const double rmsffwhm);
	void addFaradayBlock(std::vector<double> &los, const unsigned int position, double height, const unsigned int width, const double rmsffwhm);
	void addFaradayGaussian(std::vector<double> &los, const unsigned int position, double height, const unsigned int fwhm, const double rmsffwhm);
	void addFaradaySlab(std::vector<double> &los, const unsigned int positionleft, const unsigned int positionright, double heightleft, double heightright, const double rmsffwhm);	

	//*******************************************************************
	//
	// Member access functions
	//
	//*******************************************************************
	void getFaradayLOS(std::vector<double> &faradayLOS);
	void getPolarizedInt(std::vector<std::complex<double> > &polarizedInt);
	void getPolarizedQ(std::vector<double> &polarizedQ);
	void getPolarizedU(std::vector<double> &polarizedU);
	void getFaradayDepths(std::vector<double> &faradayDepths);
	void setFaradayDepths(const double min, const double max, const double step);
	void setFaradayDepths(const std::vector<double> &faradayDepths);

	//*******************************************************************
	//
	// Helper functions
	//
	//*******************************************************************	
	void createGaussian(std::vector<double> &, const double peak, const double fwhm, const unsigned int peakpos);					// create a Gaussian with equivalent FWHM and peak height shifted to peakpos

	//*******************************************************************
	//
	// High-level simulation functions
	//
	//*******************************************************************	
	void computePolarizedEmission();	// uses internally forwaredFourier() function of RM-Synthesis
};


#endif
