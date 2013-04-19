#ifndef DEF_READER_DYNSPEC_SUBSTRACTION
#define DEF_READER_DYNSPEC_SUBSTRACTION

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Dynspec_Metadata_Substraction.h"
#include "Stock_Write_Dynspec_Data_Substraction.h"


/// \class Reader_Dynspec_Substraction
///  \brief Class object for read ICD3 files and put in variables metadatas for the ICD6's Dynspec Group  

///  \details  
/// <br /> Usage: 
/// <br /> This class need as parameter a Stock_Write_Dynspec_Metadata_Substraction object. The readDynspec function will read 
///  dynspec metadatas and stock them in the Stock_Write_Dynspec_Metadata_Substraction object as private attributes. 
///  These attributes will be written in the next step by stocDynspectMetadata function which is in the class 
///  Stock_Write_Dynspec_Metadata_Substraction.h  
///  In fact this class is very similar to Reader_Rootclass, the only difference is: this class is for dynspec metadata, 
///  so, the main code (DynspecAll.cpp) loop on dynspec to process and use this class several times (at the opposite of 
///  Reader_Root class which is called only one time)


class Reader_Dynspec_Substraction
{  
  // Public Methods
  public:

  Reader_Dynspec_Substraction();   	// constructor 
  ~Reader_Dynspec_Substraction();	// destructor
  
  void readDynspec(std::string ID, std::string file1, std::string dynspec1, std::string file2, std::string dynspec2, float kfactor,Stock_Write_Dynspec_Metadata_Substraction *dynspecMetadata,Stock_Write_Dynspec_Data_Substraction *dynspecData,int nbSTOkES, std::vector<std::string> stokesComponent);
    
  // Private Attributes
  private:
       
};
 
#endif
