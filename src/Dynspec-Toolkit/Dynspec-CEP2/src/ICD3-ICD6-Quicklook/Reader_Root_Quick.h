#ifndef DEF_READER_ROOT_PART
#define DEF_READER_ROOT_PART

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Root_Metadata_Quick.h"


/// \class Reader_Root_Quick
///  \brief Class object for read ICD3 files and put in variables metadatas for the ICD6's Root Group  

///  \details  
/// <br /> Usage: 
/// <br /> This class need as parameter a Stock_Write_Root_Metadata_Quick object. The readRoot function will read 
///  root metadatas and stock them in the Stock_Write_Root_Metadata_Quick object as private attributes. 
///  These attributes will be written in the next step by stockRootMetadata function which is in the class 
///  Stock_Write_Root_Metadata_Quick.h


class Reader_Root_Quick
{    
  // Public Methods
  public:

  Reader_Root_Quick();   	// constructor 
  ~Reader_Root_Quick();	// destructor
  
  void readRoot(std::string pathFile,std::string obsName,Stock_Write_Root_Metadata_Quick *rootMetadata,int obsNofBeam, std::string SAPname, int SAPindex,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin);
    
  // Private Attributes
  private:
    
    
};
 
#endif