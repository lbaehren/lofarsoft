
#include <iostream>
#include <vector>
#include "rmsim.h"

using namespace std;
using RM::rmsim;

/*!
	\file rRMSim.cpp
	\ingroup RM
	\brief Test program for RMSim class to simulate Faraday emission regions
	
	\author Sven Duscha (sduscha@mpa-garching.mpg.de)
	\date 26-08-2009  (Last change: 13-11-2009)
*/

int main()
{
  // nof. of Faraday depths to test
  unsigned int nfaradaydepths=300;
  // number of lambda squared channels for simulated observation
  //	unsigned int nchannels=600;
  // vector to contain simulated Faraday emission
  vector<double> faradayLOS(nfaradaydepths);

  /* Create a rmsim object */
  rmsim FaradaySimulation;
  
  try {
    
    // Test shape creation functions	
    /*
      FaradaySimulation.addFaradayScreen(faradayLOS, 50, 20.0, 0);	
      FaradaySimulation.addFaradayScreen(faradayLOS, 10, 50.0, 0);
      
      FaradaySimulation.addFaradayGaussian(faradayLOS, 30, 30, 10, 0);		
      FaradaySimulation.addFaradayGaussian(faradayLOS, 60, 50, 10, 0);
    */	
    FaradaySimulation.addFaradayBlock(faradayLOS, 189, 2, 20, 0);
    FaradaySimulation.addFaradayBlock(faradayLOS, 244, 3, 10, 0);	
    
    vector<unsigned int> positions(1);
    vector<double> heights(1);
    vector<unsigned int> widths(1);
    
    // 	positions[0]=100; positions[1]=225;
    // 	heights[0]=30; heights[1]=35;
    // 	widths[0]=10; widths[1]=10;
    
    // 	FaradaySimulation.faradayGaussians(faradayLOS, static_cast<const vector<unsigned int> >(positions), static_cast<const vector<unsigned int> >(widths), static_cast<const vector<double> >(heights), 0);
    FaradaySimulation.addFaradayScreen(faradayLOS, 130, 10, 0);
    //	FaradaySimulation.faradayScreens(faradayLOS, static_cast<const vector<unsigned int> >(positions), static_cast<const vector<double> >(heights), 0);
    
    // Test the noise generating functions;
    FaradaySimulation.writeRMtoFile(faradayLOS, "brentjens.dat");
    
    
    // forward snythesize simulation into polarized emission in Q and U
    /*	FaradaySimulation.polarizedInt=forwardFourier(	FaradaySimulation.lambda_sqs,											FaradaySimulation.rmpolint,
	FaradaySimulation.faradays,
	const vector<double> &weights,
	const vector<double> &delta_faradays,
	const double lambdaZero);	      */
  }
  
  catch(const char* s) {
    std::cerr << s << std::endl;
  }
}
