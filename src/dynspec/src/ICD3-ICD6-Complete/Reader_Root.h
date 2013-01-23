#ifndef DEF_READER_ROOT
#define DEF_READER_ROOT

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Root_Metadata.h"

/// \class Reader_Root
///  \brief Class object for read ICD3 files and put in variables metadatas for the ICD6's Root Group  

///  \details  
/// <br /> Usage: 
/// <br /> This class need as parameter a Stock_Write_Root_Metadata object. The readRoot function will read 
///  root metadatas and stock them in the Stock_Write_Root_Metadata object as private attributes. 
///  These attributes will be written in the next step by stockRootMetadata function which is in the class 
///  Stock_Write_Root_Metadata.h

class Reader_Root
{  
  // Public Methods
  public:

  Reader_Root();   	// constructor 
  ~Reader_Root();	// destructor
  
  void readRoot(std::string pathFile,std::string obsName,Stock_Write_Root_Metadata *rootMetadata,int obsNofSource,int obsNofBeam,int obsNofFrequencyBand,int obsNofTiledDynspec, std::string SAPname, int SAPindex, int j_indicator);//,int obsNofSource,int obsNofBeam,int obsNofStockes,int obsNofFrequencyBand,int obsNofTiledDynspec,std::vector<double> obsMinFrequency,
	       //std::vector<double> obsMaxFrequency,std::vector<double> obsCentralFrequency);
    
  // Private Attributes
  private:
       
};
 
#endif
