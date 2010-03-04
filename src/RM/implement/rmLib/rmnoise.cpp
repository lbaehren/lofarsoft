// Implementation of high-level noise generating functions for RM-Synthesis
//
// File:		rmnoise.cpp
// Author:		Sven Duscha (sduscha@mpa-garching.mpg.de)
// Date:		14-10-2009
// Last change:		14-10-2009


#include <vector>
#include <complex>
#include "rmnoise.h"

using namespace std;

/*!
	\brief Create a real noise vector with noise characteristic specified by type
	
	\param noise - real vector to contain noise
	\param sigma - sigma (standard deviation) value for noise distribution
	\param type - specifies noise properties: white, gaussian, power
	\param alpha - exponential factor for powernoise (optional in other cases)
*/
void rmnoise::createNoiseVector(vector<double> &noise, double sigma, const string &type, const double alpha)
{
	unsigned int length=noise.size();
	
	//---------------------------------------------
	// Check input parameters
	if(noise.size()==0)
		throw "noise::createNoiseVector noise vector has size 0";
	if(sigma<=0)
		throw "noise::createNoiseVector sigma <= 0";
	if(type=="")
		throw "noise::createNoiseVector no noise type given";
	if(type!="white" && type!="gaussian" && type!="power")
		throw "noise::createNoiseVector unknown noise type";
		
	//---------------------------------------------
	if(type=="white")
	{
		for(unsigned int i=0; i < length; i++)
		{
			noise[i]=2*sqrt(3)*rand_uni()-2*sqrt(3)*sigma/2;
		}
	}
	if(type=="gaussian")
	{
		for(unsigned int i=0; i < length; i++)
		{
			noise[i]=0.25*2*sqrt(3)*sigma*rand_gauss();	
		}
	}
	/*
	if(type=="power")
	{
		if(alpha==0)
			throw "noise::createNoiseVector exponential factor alpha=0";
		else
		{

		    powernoise(noise, alpha);			// get complete power noise vector
		
		    for(unsigned int i=0; i < length; i++)
		      noise[i]=sigma*noise[i];		// multiply by sigma
		}
	}
	*/
}


/*!
	\brief Create a complex noise vector with noise characteristic specified by type
	
	\param noise - complex vector to contain noise
	\param sigma - sigma (standard deviation) value for noise distribution
	\param type - specifies noise properties: white, gaussian, power
	\param alpha - exponential factor for powernoise (optional in other cases)
*/
void rmnoise::createNoiseVector(vector<complex<double> > &noise, double sigma, const string &type, const double alpha)
{
	unsigned int length=noise.size();
	
	//---------------------------------------------
	// Check input parameters
	if(noise.size()==0)
		throw "noise::createNoiseVector noise vector has size 0";
	if(sigma<=0)
		throw "noise::createNoiseVector sigma <= 0";
	if(type=="")
		throw "noise::createNoiseVector no noise type given";
	if(type!="white" && type!="gaussian" && type!="power")
		throw "noise::createNoiseVector unknown noise type";
			
	//---------------------------------------------
	if(type=="white")
	{
		for(unsigned int i=0; i < length; i++)
		{
			noise[i].real()=sigma*rand_uni()-sigma/2;
			noise[i].imag()=sigma*rand_uni()-sigma/2;
		}
	}
	if(type=="gaussian")
	{
		for(unsigned int i=0; i < length; i++)
		{
			noise[i].real()=sigma*rand_gauss();
			noise[i].imag()=sigma*rand_gauss();	
		}
	}
	/*
	if(type=="power")
	{
		if(alpha==0)
			throw "noise::createNoiseVector exponential factor alpha=0";
		else
		{
			powernoise(noise, alpha);								// get complete complex power noise vector
		
			for(unsigned int i=0; i < length; i++)
			{
				noise[i].real()=sigma*noise[i].real();			// multiply real part by sigma
				noise[i].real()=sigma*noise[i].imag();			// multiply imaginary part by sigma
			}
		}
	}
	*/
}


/*!
	\brief Create a complex noise vector with noise characteristic specified by type
	
	\param noise - itpp cvec vector to contain noise
	\param sigma - sigma (standard deviation) value for noise distribution
	\param type - specifies noise properties: white, gaussian, power
	\param alpha - exponential factor for powernoise (optional in other cases)
*/
void rmnoise::createNoiseVector(itpp::cvec &noise, double sigma, const std::string &type, const double alpha)
{
	unsigned int length=noise.size();
	
	//---------------------------------------------
	// Check input parameters
	if(noise.size()==0)
		throw "noise::createNoiseVector noise vector has size 0";
	if(sigma<=0)
		throw "noise::createNoiseVector sigma <= 0";
	if(type=="")
		throw "noise::createNoiseVector no noise type given";
	if(type!="white" && type!="gaussian" && type!="power")
		throw "noise::createNoiseVector unknown noise type";
			
	//---------------------------------------------
	if(type=="white")
	{
		for(unsigned int i=0; i < length; i++)
		{
			noise[i].real()=sigma*rand_uni()-sigma/2;
			noise[i].imag()=sigma*rand_uni()-sigma/2;
		}
	}
	if(type=="gaussian")
	{
		for(unsigned int i=0; i < length; i++)
		{
			noise[i].real()=sigma*rand_gauss();
			noise[i].imag()=sigma*rand_gauss();	
		}
	}
	/*
	if(type=="power")
	{
		if(alpha==0)
			throw "noise::createNoiseVector exponential factor alpha=0";
		else
		{
			powernoise(noise, alpha);								// get complete complex power noise vector
		
			for(unsigned int i=0; i < length; i++)
			{
				noise[i].real()=sigma*noise[i].real();			// multiply real part by sigma
				noise[i].real()=sigma*noise[i].imag();			// multiply imaginary part by sigma
			}
		}
	}
	*/
}


/*!
	\brief Add noise to real signal (single polarization)
	
	\param signal - signal to add noise to
	\param sigma - sigma value of noise
	\param type - noise type ("gaussian", "white", "pink"): Gaussian, White of Pink noise
	\param alpha - powerlaw exponent in case of pink noise (optional)	
*/
void rmnoise::addNoise(vector<double> &signal, double sigma, string &type, double alpha)
{
	vector<double> noise(signal.size());	// noise vector of length of signal vector

	// Check input paramaters
	if(signal.size()==0)
		throw "rmsim::addNoise signal has length 0";
	if(type=="")
		throw "rmsim:addNoise no noise type given";
	if(type!="gaussian" && type!="white")
		throw "rmsim::addNoise wrong noise type given";

	//-------------------------------------------------------	
	rmnoise rng(24);			// create a Planck random number generator object

	if(type=="gaussian")
	{
	  // Generate Gaussian noise with sigma over length of signal
	  for(unsigned int i=0; i<signal.size(); i++)
	  {
	    noise[i]=2*sqrt(3)*sigma*rng.rand_gauss();	  // create Gaussian noise of sigma amplitude
	    signal[i]=signal[i]+noise[i]; 	 	  // Add noise vector to signal vector
	  }
	}
	else if(type=="white")
	{
	  // Generate Uniform noise with sigma over length of signal
	  for(unsigned int i=0; i<signal.size(); i++)
	  {
	    noise[i]=sigma*rng.rand_uni()-sigma/2;	  // create Uniform noise of sigma amplitude
	    signal[i]=signal[i]+noise[i];  	  // Add noise vector to signal vector
	  }
	}
	else if(type=="powernoise")
	{
	    // TODO
	}
}


/*!
	\brief Add noise to complex signal (Q and U)
	
	\param signal - signal to add noise to
	\param sigma - sigma value of noise
	\param type - noise type ("gaussian", "white", "pink"): Gaussian, White or Pink noise
	\param alpha - powerlaw exponent in case of pink noise (optional)
*/
void rmnoise::addNoise(vector<complex<double> > &signal, double sigma, string &type, double alpha)
{
	vector<complex<double> > noise(signal.size());

	// Check input paramaters
	if(signal.size()==0)
		throw "rmsim::addNoise signal has length 0";
	if(type=="")
		throw "rmsim:addNoise no noise type given";
	if(type!="gaussian" && type!="white")
		throw "rmsim::addNoise wrong noise type given";
	if(type=="pink" && alpha==0)
		throw "";
		
	//-------------------------------------------------------
	rmnoise rng(24);		// initialize Planck random number generator

	if(type=="gaussian")
	{
	  // Generate Gaussian noise with sigma over length of signal
	  for(unsigned int i=0; i<signal.size(); i++)
	  {
	    noise[i]=0.25*sigma*rng.rand_gauss();		  // create Gaussian noise of sigma amplitude
	    signal[i]=signal[i]+noise[i];  	  // Add noise vector to signal vector
	  }
	}
	else if(type=="white")
	{
	  // Generate Uniform noise with sigma over length of signal
	  for(unsigned int i=0; i<signal.size(); i++)
	  {
	    noise[i]=sigma*rng.rand_uni()-sigma/2;	  // create Uniform noise of sigma amplitude
	    signal[i]=signal[i]+noise[i];  	  // Add noise vector to signal vector
	  }
	}
	else if(type=="powernoise")
	{
	    // TODO
	}
}


/*!
	\brief Add noise to a ITTP complex signal vector cvec
	
	\param signal - cvec signal vector to add noise to
	\param sigma - sigma value of noise
	\param type - noise type ("gaussian", "white", "pink"): Gaussian, White or Pink noise
	\param alpha - powerlaw exponent in case of pink noise (optional)
*/
void rmnoise::addNoise(itpp::cvec &signal, double sigma, std::string &type, double alpha)
{
	itpp::cvec noise(signal.size());

	// Check input paramaters
	if(signal.size()==0)
		throw "rmsim::addNoise signal has length 0";
	if(type=="")
		throw "rmsim:addNoise no noise type given";
	if(type!="gaussian" && type!="white")
		throw "rmsim::addNoise wrong noise type given";
	if(type=="pink" && alpha==0)
		throw "";
		
	//-------------------------------------------------------
	rmnoise rng(24);		// initialize Planck random number generator

	if(type=="gaussian")
	{
	  // Generate Gaussian noise with sigma over length of signal
	  for(int i=0; i < signal.size(); i++)
	  {
	    noise[i]=0.25*sigma*rng.rand_gauss();		  // create Gaussian noise of sigma amplitude
	    signal[i]=signal[i]+noise[i];  	  // Add noise vector to signal vector
	  }
	}
	else if(type=="white")
	{
	  // Generate Uniform noise with sigma over length of signal
	  for(int i=0; i < signal.size(); i++)
	  {
	    noise[i]=sigma*rng.rand_uni()-sigma/2;	  // create Uniform noise of sigma amplitude
	    signal[i]=signal[i]+noise[i];  	  // Add noise vector to signal vector
	  }
	}
	else if(type=="powernoise")
	{
	    // TODO
	}
}
