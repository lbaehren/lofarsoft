#ifndef DEF_READER_ROOT_SUBSTRACTION
#define DEF_READER_ROOT_SUBSTRACTION

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Root_Metadata_Substraction.h"

/// \class Reader_Root_Substraction
///  \brief Class object for read ICD3 files and put in variables metadatas for the ICD6's Root Group  

///  \details  
/// <br /> Usage: 
/// <br /> This class need as parameter a Stock_Write_Root_Metadata_Substraction object. The readRoot function will read 
///  root metadatas and stock them in the Stock_Write_Root_Metadata_Substraction object as private attributes. 
///  These attributes will be written in the next step by stockRootMetadata function which is in the class 
///  Stock_Write_Root_Metadata_Substraction.h

class Reader_Root_Substraction
{  
  // Public Methods
  public:

  Reader_Root_Substraction();   	// constructor 
  ~Reader_Root_Substraction();	// destructor
  
  void readRoot(Stock_Write_Root_Metadata_Substraction *rootMetadata,std::string ID,std::string file1,std::string dynspec1,std::string file2,std::string dynspec2,float kfactor);
  
  
  // Private Attributes
  private:
       
};
 
#endif
