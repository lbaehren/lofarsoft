#ifndef DEF_READER_DYNSPEC_PART
#define DEF_READER_DYNSPEC_PART

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Dynspec_Metadata_Part.h"
#include "Stock_Write_Dynspec_Data_Part.h"

class Reader_Dynspec_Part
{  
  // Public Methods
  public:

  Reader_Dynspec_Part();   	// constructor 
  ~Reader_Dynspec_Part();	// destructor
  
  
  void readDynspec(std::string pathFile,Stock_Write_Dynspec_Metadata_Part *dynspecMetadata,Stock_Write_Dynspec_Data_Part *dynspecData,int i, int j,int q,int obsNofSAP, int obsNofSource, int obsNofFrequencyBand,int obsNofStockes,std::vector<std::string> stokesComponent, int SAPindex, float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin);
    
  // Private Attributes
  private:
       
};
 
#endif