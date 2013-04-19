#ifndef DEF_READER_DYNSPEC_LINEAR_POLAR
#define DEF_READER_DYNSPEC_LINEAR_POLAR

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Dynspec_Metadata_Linear_Polar.h"
#include "Stock_Write_Dynspec_Data_Linear_Polar.h"


/// \class Reader_Dynspec_Linear_Polar
///  \brief Class object for read ICD3 files and put in variables metadatas for the ICD6's Dynspec Group  

///  \details  
/// <br /> Usage: 
/// <br /> This class need as parameter a Stock_Write_Dynspec_Metadata_Linear_Polar object. The readDynspec function will read 
///  dynspec metadatas and stock them in the Stock_Write_Dynspec_Metadata_Linear_Polar object as private attributes. 
///  These attributes will be written in the next step by stocDynspectMetadata function which is in the class 
///  Stock_Write_Dynspec_Metadata_Linear_Polar.h  
///  In fact this class is very similar to Reader_Rootclass, the only difference is: this class is for dynspec metadata, 
///  so, the main code (DynspecAll.cpp) loop on dynspec to process and use this class several times (at the opposite of 
///  Reader_Root class which is called only one time)


class Reader_Dynspec_Linear_Polar
{  
  // Public Methods
  public:

  Reader_Dynspec_Linear_Polar();   	// constructor 
  ~Reader_Dynspec_Linear_Polar();	// destructor
  
  void readDynspec(std::string ID, std::string filename, std::string dynspec, Stock_Write_Dynspec_Metadata_Linear_Polar *dynspecMetadata,Stock_Write_Dynspec_Data_Linear_Polar *dynspecData,int nbSTOkES, std::vector<std::string> stokesComponent);
    
  // Private Attributes
  private:
       
};
 
#endif
