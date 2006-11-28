
/* $Id: tDataConversion.cc,v 1.1 2005/07/15 07:16:47 bahren Exp $ */

#include <lopes/LopesBase/DataConversion.h>

/*!
  \file tDataConversion.cc

  \brief A collection of tests for DataConversion

  \author Lars B&aumll;hren

  \data 2005/05
*/

// =============================================================================

/*!
  \brief Test construction of DataConversion object

  \param nofAntennas -- Number of antennas.
  \param blocksize   -- Blocksize by which data are read; this is the input 
                        size of the FFT.
  \param fftlength   -- Output length of the FFT.

  \return ok -- Status of the test routine.
*/
Bool test_DataConversion (const Int nofAntennas,
			  const Int blocksize,
			  const Int fftlength)
{
  Bool ok (True);

  cout << "[1]" << endl;
  try {
    DataConversion conv (nofAntennas,blocksize);
  } catch (AipsError x) {
    cerr << "[tDataConversion] " << x.getMesg() << endl;
    ok = False;
  }

  cout << "[2]" << endl;
  try {
    DataConversion conv (nofAntennas,blocksize,fftlength);
  } catch (AipsError x) {
    cerr << "[tDataConversion] " << x.getMesg() << endl;
    ok = False;
  }

  return ok;
}

// =============================================================================


int main () {

  Bool ok (True);
  Int nofAntennas (60);
  Int blocksize (65536);
  Int fftlength (32769);

  {
    ok = test_DataConversion (nofAntennas,blocksize,fftlength);
  }

  return 0;
}
