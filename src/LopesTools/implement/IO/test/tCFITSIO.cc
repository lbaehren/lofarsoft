
#include <iostream>

#include <fitsio.h>

int main () {

  std::string filename ("testfile.fits");
  int status (0);
  long naxis (2);
  long naxes[2] = {100,100};

  std::cout << " - Creating new FITS file ... " << std::flush;
  fitsfile *fptr;
  fits_create_file (&fptr, filename.c_str(), &status);
  std::cout << "done" << std::endl;

  std::cout << " - Creating primary image array ... " << std::flush;
  fits_create_img (fptr, FLOAT_IMG, naxis, naxes, &status);
  std::cout << "done" << std::endl;
  
  return 0;
}
