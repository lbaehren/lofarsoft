#ifndef DEF_READER_DYNSPEC
#define DEF_READER_DYNSPEC

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Dynspec_Metadata.h"
#include "Stock_Write_Dynspec_Data.h"


/// \class Reader_Dynspec
///  \brief Class object for read ICD3 files and put in variables metadatas for the ICD6's Dynspec Group  

///  \details  
/// <br /> Usage: 
/// <br /> This class need as parameter a Stock_Write_Dynspec_Metadata object. The readDynspec function will read 
///  dynspec metadatas and stock them in the Stock_Write_Dynspec_Metadata object as private attributes. 
///  These attributes will be written in the next step by stocDynspectMetadata function which is in the class 
///  Stock_Write_Dynspec_Metadata.h  
///  In fact this class is very similar to Reader_Rootclass, the only difference is: this class is for dynspec metadata, 
///  so, the main code (DynspecAll.cpp) loop on dynspec to process and use this class several times (at the opposite of 
///  Reader_Root class which is called only one time)


class Reader_Dynspec
{  
  // Public Methods
  public:

  Reader_Dynspec();   	// constructor 
  ~Reader_Dynspec();	// destructor
  
  void readDynspec(std::string pathFile,Stock_Write_Dynspec_Metadata *dynspecMetadata,Stock_Write_Dynspec_Data *dynspecData,int i, int j,int q,int obsNofSAP, int obsNofFrequencyBand,int obsNofStockes,std::vector<std::string> stokesComponent, int SAPindex);
    
  // Private Attributes
  private:
       
};
 
#endif
