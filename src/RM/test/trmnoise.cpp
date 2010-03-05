
#include <iostream>
#include "rmnoise.h"

using namespace std;

/*!
  \file trmnoise.cpp
  \ingroup RM
  \brief Program for testing of high-level noise generating functions
  
  \author Sven Duscha (sduscha@mpa-garching.mpg.de)
  \date 14-10-2009 (Last change: 14-10-2009)
*/
int main(int argc, char **argv)
{
  RM::rmNoise RMnoise;		// create rmnoise object
  vector<double> noise(100);	// noise vector
  
  RMnoise.createNoiseVector(noise, 100, "gaussian", 0);
  
  for(unsigned int i=0; i<noise.size(); i++) {
    cout << noise[i] << endl;
  }
  
  return 0;
}
