#ifndef DEF_READER_ROOT_PARTIONNED
#define DEF_READER_ROOT_PARTIONNED

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Root_Metadata_Partionned.h"


/// \class Reader_Root_Partionned
///  \brief Class object for read ICD3 files and put in variables metadatas for the ICD6's Root Group  

///  \details  
/// <br /> Usage: 
/// <br /> This class need as parameter a Stock_Write_Root_Metadata_Part object. The readRoot function will read 
///  root metadatas and stock them in the Stock_Write_Root_Metadata_Part object as private attributes. 
///  These attributes will be written in the next step by stockRootMetadata function which is in the class 
///  Stock_Write_Root_Metadata_Part.h


class Reader_Root_Partionned
{    
  // Public Methods
  public:

  Reader_Root_Partionned();   	// constructor 
  ~Reader_Root_Partionned();	// destructor
  
  void readRootPartionned(std::string pathFile,std::string obsName,Stock_Write_Root_Metadata_Partionned *rootMetadata,int obsNofBeam,int obsNofFrequencyBand,int obsNofTiledDynspec, std::string SAPname, int SAPindex,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin, int i, int j,int m,std::vector<std::string> listOfFiles, int obsNofStockes);
    
  // Private Attributes
  private:
    
    
};
 
#endif