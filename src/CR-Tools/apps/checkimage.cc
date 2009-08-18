
#include <crtools.h>
#include <images/Images/HDF5Image.h>


using namespace casa;

int main(int argc, char *argv[])
{
  string infile;
  if (argc < 2) {
    cerr << "Usage: checkimage <input.image>" << endl;
    return 1;
  } else {
    infile = argv[1];
  }

  // Open the image (as readonly for the moment).
  HDF5Image<Float> image (infile);

  Array<Float> myarray;
  myarray = image.get();
  
  //cout<<sizeof(myarray)<<endl;
  
  int count =0;
  Array<Float>::iterator iterend(myarray.end());
         for (Array<Float>::iterator iter=myarray.begin(); iter!=iterend; ++iter) {
           *iter += 1;
           count += 1;
         }
  cout<<"number of pixels in the file:\t"<<count<<endl;
 // int count = 0;
  //for(count = 0; count < myarray.end()
}


