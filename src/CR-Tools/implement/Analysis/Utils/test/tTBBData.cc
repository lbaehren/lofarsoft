#include <crtools.h>
#include <fstream>
#include <Analysis/Utils/TBBData.h>






int test_TBB(const char *filelist, int tot_size ){
  int nofFailedTests (0);

  try{
    vector<string> names;
    char name[256];
    ifstream ffile(filelist);
    while(!ffile.eof()) {
      ffile>>name;
      if(!ffile.eof()) names.push_back(name);
      cout<<"getting file: "<<name<<endl;
     }
    ffile.close();
  
  
    CR::TBBData tbb;
    bool odd=true; //odd dipole nrs (X)
    bool even=false;//no even dipole nrs (Y)
    tbb.initData(names,even,odd);  //initializes nofantennas and offsets per antenna, 

    int blocksize=1024;
    int readsize=16*1024; //eg. for ppf read data in blocks of 16*1024 shift by 1024
  

    int startsample=0;

  
    int nofAntennas=tbb.getNofAntennas();
    int length=tbb.getLength(); //The maximum nr of samples
    int maxlength = min(length,tot_size);
    startsample = min(startsample,length-blocksize);

    for(;startsample<maxlength;startsample+=blocksize){
      cout<<"reading block:"<<startsample/blocksize<<endl;
      casa::Vector<double> out;
      for(int iant=0;iant<nofAntennas;iant++)
	tbb.getTimeSeries(out,readsize,iant,startsample);
    }
    
   

  }
  catch(std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;

}


int main (int argc,
	  char *argv[])
{

  int nofFailedTests (0);
  int nofParameters (3);
  char * filelist;
  int tot_size;
  if (argc < nofParameters) {
    std::cerr << "[tTBBData] Incomplete list of input parameters!" << std::endl;
    return -1;
  } else {
    filelist  = argv[1];
    tot_size  = atoi(argv[2]);
   // [1] Test for the constructor(s)
    nofFailedTests += test_TBB (filelist,tot_size);
  }
   
  return nofFailedTests;
 
 }
