#ifndef DEF_READER_ROOT_PART
#define DEF_READER_ROOT_PART

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Root_Metadata_Part.h"

class Reader_Root_Part
{  
  // Public Methods
  public:

  Reader_Root_Part();   	// constructor 
  ~Reader_Root_Part();	// destructor
  
  void readRoot(std::string pathFile,std::string obsName,Stock_Write_Root_Metadata_Part *rootMetadata,int obsNofSource,int obsNofBeam,int obsNofFrequencyBand,int obsNofTiledDynspec, std::string SAPname, int SAPindex,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin);//,int obsNofSource,int obsNofBeam,int obsNofStockes,int obsNofFrequencyBand,int obsNofTiledDynspec,std::vector<double> obsMinFrequency,
	       //std::vector<double> obsMaxFrequency,std::vector<double> obsCentralFrequency);
    
  // Private Attributes
  private:
       
};
 
#endif