#ifndef DEF_READER_DYNSPEC
#define DEF_READER_DYNSPEC

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Dynspec_Metadata.h"
#include "Stock_Write_Dynspec_Data.h"

class Reader_Dynspec
{  
  // Public Methods
  public:

  Reader_Dynspec();   	// constructor 
  ~Reader_Dynspec();	// destructor
  
  void readDynspec(std::string pathFile,Stock_Write_Dynspec_Metadata *dynspecMetadata,Stock_Write_Dynspec_Data *dynspecData,int i, int j,int q,int obsNofSAP, int obsNofSource, int obsNofFrequencyBand,int obsNofStockes,std::vector<std::string> stokesComponent, int SAPindex);
    
  // Private Attributes
  private:
       
};
 
#endif
