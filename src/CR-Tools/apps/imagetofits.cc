
#include <crtools.h>
#include <images/Images/ImageFITSConverter.h>
#include <images/Images/HDF5Image.h>
#include <images/Images/PagedImage.h>
#include <images/Images/RebinImage.h>
//#include <casa/Arrays/Slicer.h>
using namespace casa;
using namespace std;

int main(int argc, char *argv[])
{
  string infile;
  string datatype;
  
  /*
    Check if filename of the dataset and the format are provided on the command line.
  */
  if (argc < 2) {
    cerr << "Usage: imagetofits <input.event> <datatype>\n"
	 << "Supported datatypes are 'hdf5' and 'paged'."
	 << endl;
    return 1;
  } else {
    infile = argv[1];
    if (argc > 2) {
      datatype = argv[2];
    };
  }



  
  String fitsName = "new.fits";
  String error;
  Bool ok;
  
  								// Open the image and convert it to fits:
  if (datatype == "hdf5") {
    HDF5Image<Float> imageIn (infile);
	    
										// Regridding of input file:
  	IPosition factors (imageIn.shape());
  	factors    = 1;
  	factors(3) = 513;
   	RebinImage<Float> rb(imageIn, factors);
   	IPosition shapeOut = rb.shape();
   	TiledShape tShapeOut(shapeOut);
	
/* 	const LELCoordinates* pLatCoord = &(rb.lelCoordinates());
    const LELImageCoord* pImCoord =
    dynamic_cast<const LELImageCoord*>(pLatCoord);
    CoordinateSystem coords = pImCoord->coordinates();
 */
 /*
	CoordinateSystem coords = rb.coordinates();
   	TempImage<Float> imageOut(tShapeOut, coords); //srb.coordinates());
   	LatticeUtilities::copyDataAndMask(os, imageOut, rb);
   	ImageUtilities::copyMiscellaneous (imageOut, imageIn);
  */
  cout<<"Hier gaat het nog goed...1"<<endl;		
  	ok = casa::ImageFITSConverter::ImageToFITS(error, rb, fitsName);

  } else if (datatype == "paged") {
    PagedImage<Float> imageIn (infile);
	ok = casa::ImageFITSConverter::ImageToFITS(error, imageIn, fitsName);

  } else {
    cerr << "Usage: imagetofits <input.event> <datatype>\n"
	 << "Supported datatypes are 'hdf5' and 'paged'."
	 << endl;
  }
  

  
  return 0;
}
