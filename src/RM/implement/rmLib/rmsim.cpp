// Implementation of RMSim class to simulate Faraday emission regions
//
// File:		rmsim.cpp
// Author:		Sven Duscha (sduscha@mpa-garching.mpg.de)
// Date:		22-08-2009
// Last change:		14-10-2009


#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include "rmsim.h"


using namespace std;

//******************************************************************************
//
// Algorithm functions to add an additional emission region to an existing LOS
//
//*******************************************************************************

/*!
  \brief Add a Faraday screen emission (width=1) to the Faraday LOS vector
  
  \param los - vector containing simulated Faraday emission as total power P
  \param position - position within the vector (index) where the Faraday screen is set to
  \param height - polarized intensity P (Jy/rad m^-2/RMSF) of Faraday screen
  \param rmsf - FWHM of RMSF (optional parameter to normalize to different RMSFs)
*/
void rmsim::addFaradayScreen (vector<double> &los,
			      const unsigned int position,
			      double height,
			      const double rmsffwhm)
{
  /* keep length of vector in own variable */
  //	unsigned int length=los.size();
	
	// Check input parameters
	if(los.size()==0)
		throw "rmsim::addFaradayScreen los vector has size 0";
	if(position < 0 || position > los.size())
		throw "rmsim::addFaradayScreen position is out of range";
	if(height<=0)	
		throw "rmsim::addFaradayScreen height <=0";
	if(rmsffwhm<0)
		throw "rmsim::addFaradayScreen rmsffwhm < 0";
		
	//-------------------------------------------

	if(rmsffwhm!=0)							// if RMSF FWHM to normalize to was given
	{
		height=height/rmsffwhm;				// normalize polarized emission to that RMSF
	}
	
	los[position]+=height;					// put 1-dimensional Faraday screen at position
}


/*!
	\brief Add a Faraday emission block of height and width  to an existing LOS
	
	\param los - vector containing simulated Faraday emission as total power P
	\param position - position within the vector (index) where the Faraday block (centre) is set to
	\param height - polarized intensity P (Jy/rad m^-2/RMSF) of Faraday block
	\param rmsf - FWHM of RMSF (optional parameter to normalize to different RMSFs)	
*/
void rmsim::addFaradayBlock(vector<double> &los, const unsigned int position, double height, const unsigned int width, const double rmsffwhm)
{
	unsigned int length=los.size();
	
	// Check input parameters
	if(length==0)
		throw "rmsim::addFaradayBlock los vector has size 0";
	if(position < 0 || position > length)
		throw "rmsim::addFaradayBlock position is out of range";
	if(height<=0)	
		throw "rmsim::addFaradayBlock height <=0";
	if(rmsffwhm<0)
		throw "rmsim::addFaradayBlock rmsffwhm < 0";


	//-------------------------------------------

	if(rmsffwhm!=0)					// if RMSF FWHM to normalize to was given
	{
		height=height/rmsffwhm;		// normalize polarized emission to that RMSF
	}

	for(unsigned int i=position - (width/2); i < position + (width/2); i++)
	{
		los[i]+=height;
	}
	
}


/*!
	\brief Add a smooth Gaussian Faraday emission region to the line of sight vector
	
	\param los - line of sight vector containing Faraday emission simulation
	\param position - position within the vector (index) where the Faraday block (centre) is set to
	\param height - polarized intensity P (Jy/rad m^-2/RMSF) of Faraday block
	\param rmsffwhm - FWHM of RMSF (optional parameter to normalize to different RMSFs)	
*/
void rmsim::addFaradayGaussian(vector<double> &los, const unsigned int position, double height, const unsigned int fwhm, const double rmsffwhm)
{
	// Check input parameters
	if(los.size()==0)
		throw "rmsim::addFaradayGaussian los vector has size 0";
	if(position < 0 || position > los.size())
		throw "rmsim::addFaradayGaussian position is out of range";
	if(height<=0)	
		throw "rmsim::addFaradayGaussian height <= 0";
	if(fwhm<=0)
		throw "rmsim::addFaradayGaussian fwhm <= 0";
	if(rmsffwhm<0)
		throw "rmsim::addFaradayGaussian rmsffwhm < 0";

	//-----------------------------------------------------------

	if(rmsffwhm!=0)					// if RMSF FWHM to normalize to was given
	{
		height=height/rmsffwhm;		// normalize polarized emission to that RMSF
	}
	
	createGaussian(los, height, fwhm, position);		// create a Gaussian and add it to the line of sight vector los
}


/*!
	\brief Add a Faraday slab emission region to the line of sight vector

	\param los - line of sight vector containing Faraday emission simulation
	\param positionleft - position within the vector (index) where the Faraday slab left edge is set to
	\param positionright - position within the vector (index) where the Faraday slab right edge is set to	
	\param heightleft - polarized intensity P (Jy/rad m^-2/RMSF) on the left side of Faraday slab
	\param heightright - polarized intensity P (Jy/rad m^-2/RMSF) on the right side of Faraday slab
	\param rmsffwhm - FWHM of RMSF (optional parameter to normalize to different RMSFs)
*/
void addFaradaySlab(vector<double> &los, const unsigned int positionleft, const unsigned int positionright, double heightleft, double heightright, const double rmsffwhm)
{
	// Check input parameters
	if(los.size()==0)
		throw "rmsim::addFaradaySlab los vector has size 0";
	if(positionleft < 0 || positionleft > los.size())
		throw "rmsim::addFaradaySlab position is out of range";
	if(positionright < 0 || positionright > los.size())
		throw "rmsim::addFaradaySlab position is out of range";
	if(heightleft<=0)	
		throw "rmsim::addFaradaySlab heightleft <= 0";
	if(heightright<=0)	
		throw "rmsim::addFaradaySlab heightright <= 0";
	if(rmsffwhm<0)
		throw "rmsim::addFaradaySlab rmsffwhm < 0";

	//-----------------------------------------------------------

	if(rmsffwhm!=0)								// if RMSF FWHM to normalize to was given
	{
		heightleft=heightleft/rmsffwhm;		// normalize polarized emission to that RMSF
		heightright=heightright/rmsffwhm;	// normalize polarized emission to that RMSF
	}	
	
	
}	


//******************************************************************************
//
// Algorithm functions to add an additional emission region to the class internal LOS
//
//*******************************************************************************

/*!
	\brief Resize the class internal Faraday LOS vector
	
	\param size - size to resize class internal LOS vector to
*/
void rmsim::faradayLOSresize(unsigned int size)
{
	if(size==0)
		throw "rmsim::faradayLOSresize size is 0";
	else
		faradayLOS.resize(size);
}


/*!
	\brief Add a Faraday screen emission (width=1) to the class internal Faraday LOS vector
	
	\param position - position within the vector (index) where the Faraday screen is set to
	\param height - polarized intensity P (Jy/rad m^-2/RMSF) of Faraday screen
	\param rmsf - FWHM of RMSF (optional parameter to normalize to different RMSFs)
*/
void rmsim::addFaradayScreen(const unsigned int position, double height, const double rmsffwhm=0)
{
//	unsigned int length=faradayLOS.size();			// keep length of vector in own variable
	
	// Check input parameters
	if(faradayLOS.size()==0)
		throw "rmsim::addFaradayScreen faradayLOS vector has size 0";
	if(position < 0 || position > faradayLOS.size())
		throw "rmsim::addFaradayScreen position is out of range";
	if(height<=0)	
		throw "rmsim::addFaradayScreen height <=0";
	if(rmsffwhm<0)
		throw "rmsim::addFaradayScreen rmsffwhm < 0";
		
	//-------------------------------------------

	if(rmsffwhm!=0)							// if RMSF FWHM to normalize to was given
	{
		height=height/rmsffwhm;				// normalize polarized emission to that RMSF
	}
	
	faradayLOS[position]=height;			// put 1-dimensional Faraday screen at position
}


/*!
	\brief Add a Faraday emission block of height and width to the internal faradayLOS vector
	
	\param position - position within the vector (index) where the Faraday block (centre) is set to
	\param height - polarized intensity P (Jy/rad m^-2/RMSF) of Faraday block
	\param rmsf - FWHM of RMSF (optional parameter to normalize to different RMSFs)	
*/
void rmsim::addFaradayBlock(const unsigned int position, double height, const unsigned int width, const double rmsffwhm=0)
{
	unsigned int length=faradayLOS.size();
	
	// Check input parameters
	if(length==0)
		throw "rmsim::addFaradayBlock faradayLOS vector has size 0";
	if(position < 0 || position > length)
		throw "rmsim::addFaradayBlock position is out of range";
	if(height<=0)	
		throw "rmsim::addFaradayBlock height <=0";
	if(rmsffwhm<0)
		throw "rmsim::addFaradayBlock rmsffwhm < 0";


	//-------------------------------------------

	if(rmsffwhm!=0)					// if RMSF FWHM to normalize to was given
	{
		height=height/rmsffwhm;		// normalize polarized emission to that RMSF
	}

	for(unsigned int i=position - (width/2); i < position + (width/2); i++)
	{
		faradayLOS[i]=height;
	}
	
}


/*!
	\brief Add a smooth Gaussian Faraday emission region to the internal faradayLOS vector

	\param position - position within the vector (index) where the Faraday block (centre) is set to
	\param height - polarized intensity P (Jy/rad m^-2/RMSF) of Faraday block
	\param rmsffwhm - FWHM of RMSF (optional parameter to normalize to different RMSFs)	
*/
void rmsim::addFaradayGaussian(const unsigned int position, double height, const unsigned int fwhm, const double rmsffwhm=0)
{
	// Check input parameters
	if(faradayLOS.size()==0)
		throw "rmsim::addFaradayGaussian faradayLOS vector has size 0";
	if(position < 0 || position > faradayLOS.size())
		throw "rmsim::addFaradayGaussian position is out of range";
	if(height<=0)	
		throw "rmsim::addFaradayGaussian height <= 0";
	if(fwhm<=0)
		throw "rmsim::addFaradayGaussian fwhm <= 0";
	if(rmsffwhm<0)
		throw "rmsim::addFaradayGaussian rmsffwhm < 0";

	//-----------------------------------------------------------

	if(rmsffwhm!=0)					// if RMSF FWHM to normalize to was given
	{
		height=height/rmsffwhm;		// normalize polarized emission to that RMSF
	}
	
	createGaussian(faradayLOS, height, fwhm, position);		// create a Gaussian and add it to the line of sight vector los
}


/*!
	\brief Add a Faraday slab emission region to the internal faradayLOS vector

	\param positionleft - position within the vector (index) where the Faraday slab left edge is set to
	\param positionright - position within the vector (index) where the Faraday slab right edge is set to	
	\param heightleft - polarized intensity P (Jy/rad m^-2/RMSF) on the left side of Faraday slab
	\param heightright - polarized intensity P (Jy/rad m^-2/RMSF) on the right side of Faraday slab
	\param rmsffwhm - FWHM of RMSF (optional parameter to normalize to different RMSFs)
*/
void rmsim::addFaradaySlab(const unsigned int positionleft, const unsigned int positionright, double heightleft, double heightright, const double rmsffwhm)
{
	// Check input parameters
	if(faradayLOS.size()==0)
		throw "rmsim::addFaradaySlab los vector has size 0";
	if(positionleft < 0 || positionleft > faradayLOS.size())
		throw "rmsim::addFaradaySlab position is out of range";
	if(positionright < 0 || positionright > faradayLOS.size())
		throw "rmsim::addFaradaySlab position is out of range";
	if(heightleft<=0)	
		throw "rmsim::addFaradaySlab heightleft <= 0";
	if(heightright<=0)	
		throw "rmsim::addFaradaySlab heightright <= 0";
	if(rmsffwhm<0)
		throw "rmsim::addFaradaySlab rmsffwhm < 0";

	//-----------------------------------------------------------

	if(rmsffwhm!=0)								// if RMSF FWHM to normalize to was given
	{
		heightleft=heightleft/rmsffwhm;		// normalize polarized emission to that RMSF
		heightright=heightright/rmsffwhm;	// normalize polarized emission to that RMSF
	}	
	
	
}	



//******************************************************************************
//
// Algorithm functions to create multiple emission regions in a LOS vector
//
//******************************************************************************


/*!
	\brief Creaet (multiple) Faraday screens emission (width=1) to the Faraday LOS vector
	
	\param los - vector containing simulated Faraday emission as total power P
	\param positions - positions within the vector (index) where the Faraday screen is set to
	\param height - polarized intensities P (Jy/rad m^-2/RMSF) of Faraday screen
	\param rmsf - FWHM of RMSF (optional parameter to normalize to different RMSFs)
*/
void rmsim::faradayScreens(vector<double> &los, const vector<unsigned int> &positions, const vector<double> &heights, double rmsffwhm=0)
{
	unsigned int length=los.size();
	unsigned int num=positions.size();
	vector<double> normheights=heights;		// vector containing normalized heights
	
	// Check input parameters
	if(los.size()==0)
		throw "rmsim::addFaradayScreens los vector has size 0";
	if(rmsffwhm<0)
		throw "rmsim::addFaradayScreens rmsffwhm < 0";

	//-------------------------------------------

	if(rmsffwhm!=0)										// if RMSF FWHM to normalize to was given
	{
		for(unsigned int i=0; i < num; i++)
		{
			if(heights[i] <= 0)	
				throw "rmsim::addFaradayScreen height <= 0";
			
			normheights[i]=heights[i]/rmsffwhm;		// normalize polarized emission to that RMSF
		}

	}
	
	for(unsigned int i=0; i < num; i++)
	{
		if(positions[i] < 0 || positions[i] > length)
			throw "rmsim::addFaradayScreen position is out of range";

		los[positions[i]]=heights[i];			// put FaradayScreen to position			
	}

}


/*!
	\brief Create a set of num Faraday emission blocks, with given parameters in arrays
	
	\param los - vector containing simulated Faraday emission as total power P
	\param positions - vector containing positions within the vector (index) where the Faraday block (centre) is set to
	\param heights - vector with polarized intensities P (Jy/rad m^-2/RMSF) of Faraday block
	\param rmsf - FWHM of RMSF (optional parameter to normalize to different RMSFs)	
*/
void rmsim::faradayBlocks(vector<double> &los, const vector<unsigned int> &positions, const vector<unsigned int> &widths, const vector<double> &heights, const double rmsffwhm=0)
{
	unsigned int length=los.size();							// length of line of sight vector
	unsigned int num=positions.size();						// number of Faraday Blocks to create = number of positions
	vector<double> normheights=heights;						// normalized heights (make copy of heights input parameter)
	
	// Check input parameters
	if(los.size()==0)
		throw "rmsim::faradayBlocks los vector has size 0";
	if(positions.size()==0)
		throw "rmsim::faradayBlocks positions vector has size 0";
	if(widths.size()==0)
		throw "rmsim::faradayBlocks positions vector has size 0";
	if(widths.size()!=positions.size())
		throw "rmsim::faradayBlocks widths size differs from positions size";
	if(heights.size()!=positions.size())
		throw "rmsim::faradayBlocks heights size differs from positions size";

	//-------------------------------------------

	if(rmsffwhm!=0)												// if RMSF FWHM to normalize to was given
	{
		for(unsigned int i=0; i < num; i++)					// loop over heights array
			normheights[i]=normheights[i]/rmsffwhm; 		// normalize polarized emission to that RMSF
	}

	for(unsigned int b=0; b < num; b++)						// loop over blocks
	{
//		cout << "b = " << b << "   width[" << b << "] = " << widths[b] << endl;
		for(unsigned int i=positions[b]-(widths[b]/2); i < positions[b]+(widths[b]/2); i++)
		{
			//cout << "heights[" << b << "] = " << heights[b] << endl;
			if(!(i < 0 || i>length))	// set only if we are within los vector range
				los[i]=heights[b];
		}
	}
	
}


/*!
	\brief Create a set of num Faraday emission Gaussians, with given parameters in arrays

	\param los - vector containing simulated Faraday emission as total power P
	\param positions - vector containing positions within the vector (index) where the Faraday block (centre) is set to
	\param heights - vector with polarized intensities P (Jy/rad m^-2/RMSF) of Faraday block
	\param fwhms - vector containing the FWHMs of the Gaussians to create
	\param rmsf - FWHM of RMSF (optional parameter to normalize to different RMSFs)	
*/
void rmsim::faradayGaussians(vector<double> &los, const vector<unsigned int> &positions, const vector<unsigned int> &fwhms, const vector<double> &heights, const double rmsffwhm=0)
{
//	unsigned int length=los.size();					// length of line of sight vector
	unsigned int num=positions.size();				// number of positions = number of Gaussians to put
	vector<double> normheights=heights;
	
	// Check input parameters
	if(los.size()==0)
		throw "rmsim::faradayGaussians los vector has size 0";
	if(positions.size()==0)
		throw "rmsim::faradayGaussians positions vector has size 0";
	if(fwhms.size()==0)
		throw "rmsim::faradayGaussians fwhms vector has size 0";
	if(fwhms.size()!=positions.size())
		throw "rmsim::faradayGaussians fwhms vector size differs from positions vector size";
	if(heights.size()!=positions.size())
		throw "rmsim::faradayGaussians heights vector size differs from positions vector size";
	

	//-------------------------------------------

	if(rmsffwhm!=0)										// if RMSF FWHM to normalize to was given
	{
		for(unsigned int i=0; i < num; i++)			// loop over heights array
			normheights[i]=normheights[i]/rmsffwhm; 			// normalize polarized emission to that RMSF
	}

	for(unsigned int g=0; g < num; g++)				// for number num of requested Gaussians
	{
		// use functions createGaussian imported from RMClean
		createGaussian(los, heights[g], fwhms[g], positions[g]);
	}
	
}


//**********************************************
//
// Member access functions
//
//**********************************************


/*!
	\brief Get the vector containing the simulated Faraday emission along the line of sight
	
	\param faradayLOS - vector to hold simulated Faraday emission
*/
void rmsim::getFaradayLOS(vector<double> &faradayLOS)
{
	// Copy class attribute vector into function parameter vector
	faradayLOS=this->faradayLOS;
}


/*!
	\brief Get the vector containing the simulated complex polarized (Q and U) emission along the line of sight
	
	\param polarizedInt - vector to hold simulated complex polarized (Q and U) emission
*/
void rmsim::getPolarizedInt(vector<complex<double> > &polarizedInt)
{
	// Copy class attribute vector into function parameter vector	
	polarizedInt=this->polarizedInt;
}


/*!
	\brief Get the vector containing the Q polarized emission along the line of sight
	
	\param polarizedLOSQ - vector to hold simulated Q polarized emission
*/
void rmsim::getPolarizedQ(vector<double> &polarizedQ)
{
	unsigned int length=polarizedInt.size();

	//-------------------------------------------------
	if(polarizedQ.size() !=  length)
	  polarizedQ.resize(length);
	
	// Copy class attribute vector into function parameter vector	
	for(unsigned int i=0; i < length; i++)
	  polarizedInt[i]=this->polarizedInt[i].real();
}


/*!
	\brief Get the vector containing the Q polarized emission along the line of sight
	
	\param polarizedLOSQ - vector to hold simulated Q polarized emission
*/
void rmsim::getPolarizedU(vector<double> &polarizedU)
{
	unsigned int length=polarizedInt.size();
	
	//-------------------------------------------------
	if(polarizedU.size() !=  length)
	  polarizedU.resize(length);
	
	// Copy class attribute vector into function parameter vector	
	for(unsigned int i=0; i < length; i++)
	  polarizedU[i]=this->polarizedInt[i].imag();
}

/*!
	\brief Get the vector containing the Faraday depths in simulation along the line of sight
	
	\param faradayDepths - vector to hold Faraday depths which are associated with the simulation
*/
void rmsim::getFaradayDepths(std::vector<double> &faradayDepths)
{
	unsigned int length=polarizedInt.size();

	//-------------------------------------------------
	if(faradayDepths.size() !=  length)
	  faradayDepths.resize(length);

        faradayDepths=this->faradayDepths;
}


/*!
  \brief Set Faraday depths for simulation with lower, upper limit and equidistant step size 
  
  \param min - lower limit for Faraday depths
  \param max - upper limit for Faraday depths
  \param stepsize - step size between Faraday depths
*/
void rmsim::setFaradayDepths(const double min, const double max, const double stepsize)
{
  if(min > max)
    throw "rmsim::setFaradayDepths min>max";
  if(min==max)
    throw "rmsim::setFaradayDepths min==max";
  if(stepsize==0)
    throw "rmsim::setFaradayDepths stepsize is 0";

  if((fmod((max-min),stepsize))!=0)
    throw "rmsim::setFaradayDepths (warning) no integral stepsize number for faradayDepths";

  faradayDepths.resize((max-min)/stepsize); // resize faradayDepths
  for(unsigned int i=0; i<faradayDepths.size(); i++)
  {
    faradayDepths[i]=min+i*stepsize;
  }
}


/*!
  \brief Set Faraday depths for simulation to values fiven by vector
  
  \param faradayDepths - double vector containing Faraday depths for polarized emission simulation
*/
void rmsim::setFaradayDepths(const std::vector<double> &faradayDepths)
{
  if(faradayDepths.size()==0)
    throw "rmsim::setFaradayDepths faradayDepths has size 0";
  else
    this->faradayDepths=faradayDepths;
}


//**********************************************
//
// Helper functions
//
//**********************************************



/*!
	\brief Create a Gaussian over length with given peak value and FWHM at peakpos position
	
	\param gaussian - vector to hold Gaussian created
	\param peak - peak value of Gaussian
	\param fwhm - Full Width Half Maximum of Gaussian to create
	\param peakpos - position of Gaussian peak
	
	\return gaussian - gaussian with equivalent FWHM and peak value
*/
void rmsim::createGaussian(vector<double> &gaussian, const double peak, const double fwhm, 
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
		gaussian[i]+=factor*exp(-sigmafactor*(i-mean)*(i-mean));
	}
}


/*!
    \brief Compute polarized emission from simulated Faraday emission
*/
void rmsim::computePolarizedEmission()
{
  // check class attributes first, if they are set correctly
  if(lambdaSquareds.size()==0 && frequencies.size()==0)
    throw "rmsim::computePolarizedEmission lambda squareds and frequencies vector is 0";
  if(deltaLambdaSquareds.size()==0 && deltaFrequencies.size()==0)
    throw "rmsim::computePolarizedEmission delta lambda squareds and delta frequencies vector is 0";
  if(faradayDepths.size()==0)
    throw "rmsim::computePolarizedEmission faradayDepths vector is 0";

  // If we only have frequencies, convert those to lambda squareds
  if(lambdaSquareds.size()==0)
  {
  
  }
  if(deltaLambdaSquareds.size()==0)
  {
  
  }

  // Forward Fourier Transform to compute polarized intensities from RM for a selection of lambdas
/*  vector<complex<double> > forwardFourier(const vector<double> &lambda_sqs,
														 const vector<complex<double> > &rmpolint,
														 const vector<double> &faradays,
														 const vector<double> &weights,
														 const vector<double> &delta_faradays,
														 const double lambdaZero);	
*/
}
