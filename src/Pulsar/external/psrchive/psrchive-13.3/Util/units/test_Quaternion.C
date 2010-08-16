/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MatrixTest.h"
#include "Quaternion.h"

using namespace std;

// This must be defined for the MatrixTest
float norm (float x)
{
  return x*x;
}

int main () 
{
#if (RAND_MAX > 9000000)
  unsigned loops = RAND_MAX/1000;
#else
  unsigned loops = RAND_MAX*10;
#endif

  MatrixTest <Quaternion<float,Unitary>,
    Quaternion<double,Unitary>, float> testu;

  try {
    cerr << "Testing " << loops << " Unitary Quaternion variations" << endl;
    testu.runtest (loops);
  }
  catch (string& error) {
    cerr << error << endl;
    return -1;
  }




  MatrixTest <Quaternion<std::complex<float>,Hermitian>, 
    Quaternion<std::complex<double>,Hermitian>, std::complex<float> > testh;

  try {
    cerr 
      << "Testing " << loops << " Hermitian Biquaternion variations" << endl;
    testh.runtest (loops);
  }
  catch (string& error) {
    cerr << error << endl;
    return -1;
  }


  MatrixTest <Quaternion<std::complex<float>, Unitary>, 
    Quaternion<std::complex<double>, Unitary>, std::complex<float> > testub;

  try {
    cerr 
      << "Testing " << loops << " Unitary Biquaternion variations" << endl;
    testub.runtest (loops);
  }
  catch (string& error) {
    cerr << error << endl;
    return -1;
  }

  return 0;
}
