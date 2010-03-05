
/*!
  \file trmClean.cpp
  \ingroup RM
  \brief Test program for the RM Clean algorithm

  \author Sven Duscha (sduscha@mpa-garching.mpg.de)
  \date 29-07-2009 (Last change: 11-08-2009)
*/

#include <iostream>
#include "rmclean.h"

int main(int argc, char **argv)
{
  unsigned int length=100;
  
  rmclean Cleanobject(length);
  
  /* vector of size length to contain gaussian */
  vector<double> gaussian(length);
  /* Faraday depths over which RMSF was computed */
  vector<double> RMSFfaradayDepths(length);
  vector<double> faradayDepths(length);
  vector<complex<double> > rmsftest(length);
  vector<complex<double> > data(length);
  vector<complex<double> > shiftedRMSF(2*length);
  vector<complex<double> > cleanedMap(length);
  
  // Test algorithm functions
  for(int i=0; i < static_cast<int>(length); i++)
    faradayDepths[i]=i-50;
  
  //	Cleanobject.createGaussian(gaussian, 10.0, 20.0);
  
  //	Cleanobject.writeRMtoFile(gaussian, "gaussian.dat"); 			// debug
  
  Cleanobject.readRMSFfromFile(RMSFfaradayDepths, rmsftest, "rmsf.dat");			// Load RMSF from file
  
  Cleanobject.RMSF=rmsftest;												// write RMSF into object
  
  /*
    for(unsigned int i=0; i<80; i++)
    {
    std::stringstream out;
    std::string s;
    // create filename
    out << i;
    s = out.str();
    
    string filename=string("gaussian") + s + string(".dat");
    
    //cout << "filename = " << filename << endl;
    // Write shifted RMSF with Faraday depths to a text file
    // Create a Gaussian
    }
    
    // Test creation of Gaussian
    Cleanobject.createGaussian(gaussian, 50, 10, 30);		
    Cleanobject.writeRMtoFile(gaussian, "gaussian.dat");	// debug output		
  */
  
  // Test rmsfClean
  Cleanobject.rmsfClean(data, cleanedMap, 0.4, 0);
  
  
  return 0;		// return success
}
