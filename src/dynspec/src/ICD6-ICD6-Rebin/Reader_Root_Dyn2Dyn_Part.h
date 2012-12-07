#ifndef DEF_READER_ROOT_DYN2DYN_PART
#define DEF_READER_ROOT_DYN2DYN_PART

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Root_Dyn2Dyn_Metadata_Part.h"

class Reader_Root_Dyn2Dyn_Part
{  
  // Public Methods
  public:

  Reader_Root_Dyn2Dyn_Part();   	// constructor 
  ~Reader_Root_Dyn2Dyn_Part();	// destructor
  
  void readRoot(std::string pathFile,std::string obsName,Stock_Write_Root_Dyn2Dyn_Metadata_Part *rootMetadata,int obsNofBeam,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin);

  
  // Private Attributes
  private:
       
};
 
#endif