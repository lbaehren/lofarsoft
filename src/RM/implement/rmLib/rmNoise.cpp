
#include "rmNoise.h"

using namespace std;

namespace RM {

  //_____________________________________________________________________________
  //                                                            createNoiseVector
  
  /*!
    \param noise - real vector to contain noise
    \param sigma - sigma (standard deviation) value for noise distribution
    \param type - specifies noise properties: white, gaussian, power
    \param alpha - exponential factor for powernoise (optional in other cases)
  */
  void rmNoise::createNoiseVector (vector<double> &noise,
				   double sigma,
				   const string &type,
				   const double alpha)
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
  
  //_____________________________________________________________________________
  //                                                            createNoiseVector
  
  /*!
    \param noise - complex vector to contain noise
    \param sigma - sigma (standard deviation) value for noise distribution
    \param type - specifies noise properties: white, gaussian, power
    \param alpha - exponential factor for powernoise (optional in other cases)
  */
  void rmNoise::createNoiseVector (vector<complex<double> > &noise,
				   double sigma,
				   const string &type,
				   const double alpha)
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
  
  //_____________________________________________________________________________
  //                                                            createNoiseVector
  
  /*!
    \param noise - itpp cvec vector to contain noise
    \param sigma - sigma (standard deviation) value for noise distribution
    \param type - specifies noise properties: white, gaussian, power
    \param alpha - exponential factor for powernoise (optional in other cases)
  */
  void rmNoise::createNoiseVector (itpp::cvec &noise,
				   double sigma, 
				   const std::string &type,
				   const double alpha)
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
  
  //_____________________________________________________________________________
  //                                                                     addNoise
  
  /*!
    \param signal - signal to add noise to
    \param sigma - sigma value of noise
    \param type - noise type ("gaussian", "white", "pink"): Gaussian, White or
           Pink noise
    \param alpha - powerlaw exponent in case of pink noise (optional)	
  */
  void rmNoise::addNoise (vector<double> &signal,
			  double sigma,
			  string &type,
			  double alpha)
  {
    // Noise vector of length of signal vector
    vector<double> noise (signal.size());
    
    // Check input paramaters
    if(signal.size()==0)
      throw "rmsim::addNoise signal has length 0";
    if(type=="")
      throw "rmsim:addNoise no noise type given";
    if(type!="gaussian" && type!="white")
      throw "rmsim::addNoise wrong noise type given";
    
    //-------------------------------------------------------	
    rmNoise rng(24);			// create a Planck random number generator object
    
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
  
  //_____________________________________________________________________________
  //                                                                     addNoise
  
  /*!
    \param signal - signal to add noise to
    \param sigma - sigma value of noise
    \param type - noise type ("gaussian", "white", "pink"): Gaussian, White or
           Pink noise
    \param alpha - powerlaw exponent in case of pink noise (optional)
  */
  void rmNoise::addNoise (vector<complex<double> > &signal,
			  double sigma,
			  string &type,
			  double alpha)
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
    rmNoise rng(24);		// initialize Planck random number generator
    
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
  
  //_____________________________________________________________________________
  //                                                                     addNoise
  
  /*!
    \brief Add noise to a ITTP complex signal vector cvec
    
    \param signal - cvec signal vector to add noise to
    \param sigma - sigma value of noise
    \param type - noise type ("gaussian", "white", "pink"): Gaussian, White or
           Pink noise
    \param alpha - powerlaw exponent in case of pink noise (optional)
  */
  void rmNoise::addNoise (itpp::cvec &signal,
			  double sigma,
			  std::string &type,
			  double alpha)
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
    rmNoise rng(24);		// initialize Planck random number generator
    
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
  
}  // end namespace RM
