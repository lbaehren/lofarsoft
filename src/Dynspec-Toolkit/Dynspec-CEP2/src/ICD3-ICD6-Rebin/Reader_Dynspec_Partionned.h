#ifndef DEF_READER_DYNSPEC_PARTIONNED
#define DEF_READER_DYNSPEC_PARTIONNED

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Dynspec_Metadata_Partionned.h"
#include "Stock_Write_Dynspec_Data_Partionned.h"


/// \class Reader_Dynspec_Part
///  \brief Class object for read ICD3 files and put in variables metadatas for the ICD6's Dynspec Group  

///  \details  
/// <br /> Usage: 
/// <br /> This class need as parameter a Stock_Write_Dynspec_Metadata_Partionned object. The readDynspec function will read 
///  dynspec metadatas and stock them in the Stock_Write_Dynspec_Metadata_Partionned object as private attributes. 
///  These attributes will be written in the next step by stocDynspectMetadata function which is in the class 
///  Stock_Write_Dynspec_Metadata_Partionned.h  
///  In fact this class is very similar to Reader_Root_Part class, the only difference is: this class is for dynspec metadata, 
///  so, the main code (DynspecPart.cpp) loop on dynspec to process and use this class several times (at the opposite of 
///  Reader_Root_Part class which is called only once)


class Reader_Dynspec_Partionned
{  
  // Public Methods
  public:

  Reader_Dynspec_Partionned();   	// constructor 
  ~Reader_Dynspec_Partionned();	// destructor
  
  
  void readDynspecPartionned(std::string pathFile,Stock_Write_Dynspec_Metadata_Partionned *dynspecMetadata,Stock_Write_Dynspec_Data_Partionned *dynspecData,int i, int j,int q,int obsNofSAP, int obsNofFrequencyBand,int obsNofStockes,std::vector<std::string> stokesComponent, int SAPindex, float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin, int m, std::vector<std::string> listOfFiles);
    
  // Private Attributes
  private:
       
};
 
#endif