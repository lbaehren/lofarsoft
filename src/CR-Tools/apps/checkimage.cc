
#include <crtools.h>
#include <images/Images/HDF5Image.h>


using namespace casa;

int main()
{

  // Open the image (as readonly for the moment).
  HDF5Image<Float> image ("/home/swelles/usg/data/lopes/example.event.hdf5.img");

  Array<Float> myarray;
  myarray = image.get();
  
  //cout<<sizeof(myarray)<<endl;
  
  int count =0;
  Array<Float>::iterator iterend(myarray.end());
         for (Array<Float>::iterator iter=myarray.begin(); iter!=iterend; ++iter) {
           *iter += 1;
           count += 1;
         }
  cout<<"number of pixels:\t"<<count<<endl;
 // int count = 0;
  //for(count = 0; count < myarray.end()
}


