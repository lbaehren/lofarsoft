
#include <crtools.h>
#include <images/Images/ImageFITSConverter.h>
#include <images/Images/HDF5Image.h>
#include <images/Images/PagedImage.h>

using namespace casa;
using namespace std;

int main(int argc, char *argv[])
{
  string infile, datatype;
  /*
    Check if filename of the dataset is provided on the command line; if only
    a fraction of the possible tests can be carried out.
  */
  if (argc < 2) {
    cerr << "Usage: imagetofits <input.event> <datatype>\n Supported datatypes are 'hdf5' and 'paged'." << endl;
    return 1;
  } else {
    infile = argv[1];
     if (argc > 2) {
       datatype = argv[2];
     };
  }

  String fitsName = "new.fits";
  String error;
  
  // Open the image and convert it to fits:
  if (datatype == "hdf5") {
    HDF5Image<Float> image (infile);
    Bool ok = casa::ImageFITSConverter::ImageToFITS(error, image, fitsName);
  } else if (datatype == "paged") {
    PagedImage<Float> image (infile);
    Bool ok = casa::ImageFITSConverter::ImageToFITS(error, image, fitsName);
  } else {
    cerr<<"Usage: imagetofits <input.event> <datatype>\n Supported datatypes are 'hdf5' and 'paged'." << endl;
  }

}
