#ifndef DEF_READER_ROOT_PART_STANDALONE
#define DEF_READER_ROOT_PART_STANDALONE

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Root_Metadata_Quick_Standalone.h"


/// \class Reader_Root_Quick_Standalone
///  \brief Class object for read ICD3 files and put in variables metadatas for the ICD6's Root Group  

///  \details  
/// <br /> Usage: 
/// <br /> This class need as parameter a Stock_Write_Root_Metadata_Quick_Standalone object. The readRoot function will read 
///  root metadatas and stock them in the Stock_Write_Root_Metadata_Quick_Standalone object as private attributes. 
///  These attributes will be written in the next step by stockRootMetadata function which is in the class 
///  Stock_Write_Root_Metadata_Quick_Standalone.h


class Reader_Root_Quick_Standalone
{    
  // Public Methods
  public:

  Reader_Root_Quick_Standalone();   	// constructor 
  ~Reader_Root_Quick_Standalone();	// destructor
  
  void readRoot(std::string pathFile,std::string obsName,Stock_Write_Root_Metadata_Quick_Standalone *rootMetadata,int obsNofBeam,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin);
    
  // Private Attributes
  private:
    
    
};
 
#endif