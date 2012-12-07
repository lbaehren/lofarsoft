#ifndef DEF_READER_ROOT
#define DEF_READER_ROOT

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Root_Metadata.h"

class Reader_Root
{  
  // Public Methods
  public:

  Reader_Root();   	// constructor 
  ~Reader_Root();	// destructor
  
  void readRoot(std::string pathFile,std::string obsName,Stock_Write_Root_Metadata *rootMetadata,int obsNofSource,int obsNofBeam,int obsNofFrequencyBand,int obsNofTiledDynspec, std::string SAPname, int SAPindex);//,int obsNofSource,int obsNofBeam,int obsNofStockes,int obsNofFrequencyBand,int obsNofTiledDynspec,std::vector<double> obsMinFrequency,
	       //std::vector<double> obsMaxFrequency,std::vector<double> obsCentralFrequency);
    
  // Private Attributes
  private:
       
};
 
#endif
