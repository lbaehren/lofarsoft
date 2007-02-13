
/* $Id: tDataConversion.cc,v 1.1 2005/07/15 07:16:47 bahren Exp $ */

#include <Nodes/DataConversion.h>

/*!
  \file tDataConversion.cc
  
  \brief A collection of tests for DataConversion
  
  \author Lars B&aumll;hren
  
  \data 2005/05
*/

using casa::Int;
using CR::DataConversion;

// =============================================================================

/*!
  \brief Test construction of DataConversion object
  
  \param nofAntennas -- Number of antennas.
  \param blocksize   -- Blocksize by which data are read; this is the input 
                        size of the FFT.
  \param fftlength   -- Output length of the FFT.

  \return ok -- Status of the test routine.
*/
bool test_DataConversion (const Int nofAntennas,
			  const Int blocksize,
			  const Int fftlength)
{
  bool ok (true);

  std::cout << "[1]" << std::endl;
  try {
    DataConversion conv (nofAntennas,blocksize);
  } catch (AipsError x) {
    std::cerr << "[tDataConversion] " << x.getMesg() << std::endl;
    ok = false;
  }

  std::cout << "[2]" << std::endl;
  try {
    DataConversion conv (nofAntennas,blocksize,fftlength);
  } catch (AipsError x) {
    std::cerr << "[tDataConversion] " << x.getMesg() << std::endl;
    ok = false;
  }

  return ok;
}

// =============================================================================


int main () {

  bool ok (true);
  Int nofAntennas (60);
  Int blocksize (65536);
  Int fftlength (32769);

  {
    ok = test_DataConversion (nofAntennas,blocksize,fftlength);
  }

  return 0;
}
