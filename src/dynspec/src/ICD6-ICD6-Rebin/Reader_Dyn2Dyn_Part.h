#ifndef DEF_READER_DYN2DYN_PART
#define DEF_READER_DYN2DYN_PART

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Dyn2Dyn_Metadata_Part.h"
#include "Stock_Write_Dyn2Dyn_Data_Part.h"

class Reader_Dyn2Dyn_Part
{  
  // Public Methods
  public:

  Reader_Dyn2Dyn_Part();   	// constructor 
  ~Reader_Dyn2Dyn_Part();	// destructor
  
  void readDynspec(std::string pathFile,Stock_Write_Dyn2Dyn_Metadata_Part *dynspecMetadata,Stock_Write_Dyn2Dyn_Data_Part *dynspecData,int j,int obsNofStockes,std::vector<std::string> stokesComponent, float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin);
    
  // Private Attributes
  private:
       
};
 
#endif