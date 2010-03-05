
#ifndef RMSIM_H
#define RMSIM_H

#include <vector>
#include <complex>	
#include "rmnoise.h"
#include "rmio.h"
#include "rm.h"

namespace RM {
  
  /*!
    \class rmsim 
    \ingroup RM
    
    \brief Simulation of Rotation Measure emission to test algorithms
    
    \author Sven Duscha (sduscha@mpa-garching.mpg.de)
    
    \date 22-08-2009  (Last change: 14-10-2009)
    
    \test tRMSim.cpp
    
    <h3>Prerequisites</h3>
    
    - \ref rm
    - Has to take public routines of rm to perform Forward transform to
    polarized emission
    
    <h3>Synopsis</h3>
    
    This class can create different Faraday emission regions along a line of
    sight vector. These may be simple Faraday screens or block structures, but
    also smoothed emission regions can be created.
    
    Gaussian noise can be added, but it is more useful to add noise to the
    Fourier transformed polarized emission.
    
    <h3>Examples</h3>
  */
  class rmsim : public rmNoise, public rmIO, public rm
  {
    //! Vector for simulated Faraday emission
    std::vector<double> faradayLOS;
    //! Vector for simulated polarized emission (Q and U)
    std::vector<std::complex<double> > polarizedInt;
    //! Alternative way to give position in Faraday depth instead of vector index
    std::vector<double> faradayDepths;
    //! Lambda squareds for simulated observation
    std::vector<double> lambdaSquareds;
    //! Delta lambda squareds for simulated observation
    std::vector<double> deltaLambdaSquareds;
    //! Frequencies for simulated observation 
    std::vector<double> frequencies;
    //! Delta frequencies for simulated observation
    std::vector<double> deltaFrequencies;
    //! Vector containing weighting
    std::vector<double> weights;
    
  public:

    // === Construction =========================================================

    //! Default constructor
    rmsim ();

    //! Argumented constructor
    rmsim (std::vector<double> &faradaydepths,
	   std::vector<double> &frequencies,
	   std::vector<double> &weights);
    
    // === Destruction ==========================================================
    
    /*     //! Default destructor */
    /*     ~rmsim (); */
    
    // === Methods ==============================================================
   
    //! Algorithm function for a complete LOS emission with multiple regions
    void faradayScreens (const std::vector<unsigned int> &positions,
			 const std::vector<double> &heights,
			 const double rmsffwhm);
    //! Algorithm function for a complete LOS emission with multiple regions
    void faradayBlocks (const std::vector<unsigned int> &positions,
			const std::vector<unsigned int> &widths,
			const std::vector<double> &heights,
			const double rmsffwhm);
    //! Algorithm function for a complete LOS emission with multiple regions
    void faradayGaussians (const std::vector<unsigned int> &positions,
			   const std::vector<unsigned int> &fwhms,
			   const std::vector<double> &heights,
			   const double rmsffwhm);
    //! Resize the class internal Faraday LOS vector
    void faradayLOSresize(unsigned int);
    
    // Algorithm functions for a complete LOS emission with multiple regions (providing external vector)
    void faradayScreens(std::vector<double> &los, const std::vector<unsigned int> &positions, const std::vector<double> &heights, double rmsffwhm);
    void faradayBlocks(std::vector<double> &los, const std::vector<unsigned int> &positions, const std::vector<unsigned int> &widths, const std::vector<double> &heights, const double rmsffwhm);
    void faradayGaussians(std::vector<double> &los, const std::vector<unsigned int> &positions, const std::vector<unsigned int> &fwhms, const std::vector<double> &heights, const double rmsffwhm);
    
    /*
     * Algorithm functions to add an additional emission region to an existing
     * LOS (working on class attribute vector)
     */

    //! Add a Faraday screen emission (width=1) to the Faraday LOS vector
    void addFaradayScreen (const unsigned int postion,
			   double height,
			   const double rmsffwhm);
    //! Add a Faraday emission block of height and width  to an existing LOS
    void addFaradayBlock (const unsigned int position,
			  double height,
			  const unsigned int width,
			  const double rmsffwhm);
    void addFaradayGaussian (const unsigned int position,
			     double height,
			     const unsigned int fwhm,
			     const double rmsffwhm);
    //! Add a Faraday slab emission region to the line of sight vector
    void addFaradaySlab (const unsigned int positionleft,
			 const unsigned int positionright,
			 double heightleft, 
			 double heightright,
			 const double rmsffwhm);	
    
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

}  // END -- namespace RM

#endif
