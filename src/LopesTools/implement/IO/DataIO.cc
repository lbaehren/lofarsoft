
#include <IO/DataIO.h>

namespace lopestools {
  
  // --------------------------------------------------------------------- DataIO
  
  template <typename T>
  DataIO<T>::DataIO ()
  {}
  
  template <typename T>
  DataIO<T>::DataIO (const string& infile)
    : infile_p (infile)
  {}
  
  template <typename T>
  DataIO<T>::~DataIO ()
  {}
  
  // ------------------------------------------------------------------ fromASCII
  
  template <typename T>
  vector<T> DataIO<T>::fromASCII ()
  {
    vector<T> data;
    ifstream infile;
    
    infile.open (infile_p.c_str(), ios::in);
    
    // read in the shape of the stored data array ...
    shape_p.resize (2,0);
    infile >> shape_p[0] >> shape_p[1];
    
    // give some feedback
    cout << " - File name         : " << infile_p << endl;
    cout << " - # of rows         : " << shape_p[0] << endl;
    cout << " - # of columns      : " << shape_p[1] << endl;
    cout << " - # of elements     : " << shape_p[0]*shape_p[1] << endl;
    cout << " - # of image plains : " << shape_p[1]-2 << endl;
    
    // resize the arrays storing the data
    data.resize (shape_p[0]*shape_p[1],0);
    
    // read in the data from disk
    int n (0);
    int m(0);
    
    cout << " - Reading data from disk ... " << flush;
    for (n=0; n<shape_p[0]; n++) {
      for (m=0; m<shape_p[1]; m++) {
	infile >> data[shape_p[1]*n+m];
      }
    }
    cout << "done" << endl;
    
    infile.close();
    
    return data;
  }
  
  template <typename T>
  vector<T> DataIO<T>::fromASCII (const string& infile)
  {
    // store the name on the input file
    infile_p = infile;
    // access the data on disk
    return DataIO::fromASCII ();
  }
  
// ----------------------------------------------------------------- toRootScript
  
  template <typename T>
  void DataIO<T>::toRootScript (const string& filename)
  {
    ofstream outfile;
    outfile.open(filename.c_str(),ios::out);
    
    outfile << "{" << endl;
    outfile << "}" << endl;
    
    outfile.close();
  }
  
  //
  // Template handling
  //
  
  template class DataIO<int>;
  template class DataIO<float>;
  template class DataIO<double>;
  
}
