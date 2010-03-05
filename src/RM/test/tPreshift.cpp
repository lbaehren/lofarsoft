#include "rmclean.h"
#include <iostream>

/*!
	\file tPreshift.cpp
	\ingroup RM
	\brief Test program for the RM Clean algorithm
	
	\author Sven Duscha (sduscha@mpa-garching.mpg.de)
	\date 29-07-2009  (Last change: 11-08-2009)
*/

int main(int argc, char **argv)
{
  unsigned int length=100;
  
  // Create new object
  rmclean Cleanobject (length);
  // Vector of size length to contain gaussian
  vector<double> gaussian(length);
  // Faraday depths over which RMSF was computed
  vector<double> RMSFfaradayDepths(length);
  vector<double> faradayDepths(length);
  vector<complex<double> > rmsftest(length);
  vector<complex<double> > shiftedRMSF(length);
  
  // Set up vector with Faraday depth values
  for(int i=0; i < static_cast<int>(length); i++) {
    faradayDepths[i]=i-50;
  }
  
  Cleanobject.createGaussian(gaussian, 10.0, 20.0);
  
  //	Cleanobject.writeRMtoFile(gaussian, "gaussian.dat"); 			// debug
  
  Cleanobject.readRMSFfromFile(RMSFfaradayDepths, rmsftest, "rmsf.dat");			// Load RMSF from file
  
  Cleanobject.RMSF=rmsftest;												// write RMSF into object
  
  // Test preshiftRMSF functions:
  // 	Cleanobject.setKeepshiftedRMSF(true);
  //	Cleanobject.computePreshiftedRMSF(80);			// compute for all positions of the dirtyMap
  
  for(unsigned int i=0; i<80; i++)
    {
      std::stringstream out;
      std::string s;
      // create filename
      out << i;
      s = out.str();
      
      string filename=string("shiftedRMSF") + s + string(".dat");
      
      //cout << "filename = " << filename << endl;
      // Write shifted RMSF with Faraday depths to a text file
//       Cleanobject.writeRMSFtoFile (faradayDepths,
// 				   Cleanobject.preshiftedRMSF[i],
// 				   filename);	// debug output		
    }

  //	Cleanobject.shiftRMSF(57, shiftedRMSF);

  return 0;		// return success
}
