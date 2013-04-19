#ifndef DEF_READER_ROOT_STANDALONE
#define DEF_READER_ROOT_STANDALONE

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Root_Metadata_Standalone.h"

/// \class Reader_Root_Standalone
///  \brief Class object for read ICD3 files and put in variables metadatas for the ICD6's Root Group  

///  \details  
/// <br /> Usage: 
/// <br /> This class need as parameter a Stock_Write_Root_Metadata_Standalone object. The readRoot function will read 
///  root metadatas and stock them in the Stock_Write_Root_Metadata_Standalone object as private attributes. 
///  These attributes will be written in the next step by stockRootMetadata function which is in the class 
///  Stock_Write_Root_Metadata_Standalone.h

class Reader_Root_Standalone
{  
  // Public Methods
  public:

  Reader_Root_Standalone();   	// constructor 
  ~Reader_Root_Standalone();	// destructor
  
  void readRoot(std::string pathFile,std::string obsName,Stock_Write_Root_Metadata_Standalone *rootMetadata,int obsNofBeam);
  
  
  // Private Attributes
  private:
       
};
 
#endif
