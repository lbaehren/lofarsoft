#ifndef DEF_READER_DYNSPEC_PART
#define DEF_READER_DYNSPEC_PART

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Dynspec_Metadata_Quick.h"
#include "Stock_Write_Dynspec_Data_Quick.h"


/// \class Reader_Dynspec_Quick
///  \brief Class object for read ICD3 files and put in variables metadatas for the ICD6's Dynspec Group  

///  \details  
/// <br /> Usage: 
/// <br /> This class need as parameter a Stock_Write_Dynspec_Metadata_Quick object. The readDynspec function will read 
///  dynspec metadatas and stock them in the Stock_Write_Dynspec_Metadata_Quick object as private attributes. 
///  These attributes will be written in the next step by stocDynspectMetadata function which is in the class 
///  Stock_Write_Dynspec_Metadata_Quick.h  
///  In fact this class is very similar to Reader_Root_Quick class, the only difference is: this class is for dynspec metadata, 
///  so, the main code (DynspecQuick.cpp) loop on dynspec to process and use this class several times (at the opposite of 
///  Reader_Root_Quick class which is called only once)


class Reader_Dynspec_Quick
{  
  // Public Methods
  public:

  Reader_Dynspec_Quick();   	// constructor 
  ~Reader_Dynspec_Quick();	// destructor
  
  
  void readDynspec(std::string pathFile,Stock_Write_Dynspec_Metadata_Quick *dynspecMetadata,Stock_Write_Dynspec_Data_Quick *dynspecData,int i, int j, int q,int obsNofSAP,int obsNofStockes,std::vector<std::string> stokesComponent, int SAPindex, float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin);
    
  // Private Attributes
  private:
       
};
 
#endif