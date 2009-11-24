#include <crtools.h>
#include <fstream>
#include <Analysis/Utils/PolyPhaseFilter.h>
#include <Analysis/Utils/InvPolyPhaseFilter.h>







int test_PPF(const char *weightsfile,vector<complex double> &freqdat)
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_PolyPhaseFilter]\n" << std::endl;
  try {
    CR::PolyPhaseFilter ppf;
    
    vector<double> weights(1024*16);
    int fpos=0;
  
    ifstream coeffs(weightsfile);
    while(!coeffs.eof() && !(fpos>=1024*16)) {
      coeffs>>weights[fpos];
      //if(fpos%1024==0) cout<<"weights "<<weights[fpos]<<endl;
      fpos++;
    }
    coeffs.close();
    

    ppf.setWeights(weights);
    
    //creates some timeseries
    vector<double> timeseries(16*1024);
    
    for(int i=0;i<16*1024;i++){
      timeseries[i]=sin(5.*2*3.14*i/(200.));
    }  
    
    ppf.Convert(timeseries);
    
    freqdat.clear();
    for(int i=0;i<513;i++){
      freqdat.push_back(ppf.GetComplex(i));
    }
    
  }
  catch(std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}


int test_PPFinv(const char *weightsfile,const vector<complex double> &freqdat,vector<double> &timeseries)
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_INVPolyPhaseFilter]\n" << std::endl;
  try {
    CR::InvPolyPhaseFilter invppf;
    
    vector<double> weights(1024*16);
    int fpos=0;
  
    ifstream coeffs(weightsfile);
    while(!coeffs.eof() && !(fpos>=1024*16)) {
      coeffs>>weights[fpos];
      //if(fpos%1024==0) cout<<"weights "<<weights[fpos]<<endl;
      fpos++;
    }
    coeffs.close();
    
    //set the weights
    invppf.setWeights(weights);
    
    
    invppf.Convert(freqdat);  //in fact result of invppf only makes sense after first 16 blocks
    //
    timeseries.resize(1024);
    for(int i=0;i<1024;i++){
      timeseries[i]=invppf.GetAmp(i);
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
  char * ppfweights;
  char * ppfinvweights;

  if (argc < nofParameters) {
    std::cerr << "[tPolyPhaseFilter] Incomplete list of input parameters!" << std::endl;
    return -1;
  } else {
    ppfweights  = argv[1];
    ppfinvweights = argv[2];
    vector<complex double> freqdat;
    vector<double> timeseries;
   // [1] Test for the constructor(s)
    nofFailedTests += test_PPF (ppfweights,freqdat);
    nofFailedTests += test_PPFinv (ppfinvweights,freqdat,timeseries);
  }
  
  return nofFailedTests;
}
