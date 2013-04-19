#ifndef DEF_READER_ROOT_LINEAR_POLAR
#define DEF_READER_ROOT_LINEAR_POLAR

#include<string>
#include<iostream>

#include <dal/lofar/BF_File.h>
#include "Stock_Write_Root_Metadata_Linear_Polar.h"

/// \class Reader_Root_Linear_Polar
///  \brief Class object for read ICD3 files and put in variables metadatas for the ICD6's Root Group  

///  \details  
/// <br /> Usage: 
/// <br /> This class need as parameter a Stock_Write_Root_Metadata_Linear_Polar object. The readRoot function will read 
///  root metadatas and stock them in the Stock_Write_Root_Metadata_Linear_Polar object as private attributes. 
///  These attributes will be written in the next step by stockRootMetadata function which is in the class 
///  Stock_Write_Root_Metadata_Linear_Polar.h

class Reader_Root_Linear_Polar
{  
  // Public Methods
  public:

  Reader_Root_Linear_Polar();   	// constructor 
  ~Reader_Root_Linear_Polar();	// destructor
  
  void readRoot(Stock_Write_Root_Metadata_Linear_Polar *rootMetadata,std::string ID,std::string filename, int nbBEAM);
  
  
  // Private Attributes
  private:
       
};
 
#endif
