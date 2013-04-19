#ifndef DEF_STOCK_WRITE_DYNSPEC_DATA_STANDALONE
#define DEF_STOCK_WRITE_DYNSPEC_DATA_STANDALONE

#include<string>
#include<iostream>
#include<vector>

#include <dal/lofar/BF_File.h>


/// \class Stock_Write_Dynspec_Data_Standalone
///  \brief Class object for stocking data parameters and write datas for the ICD6's Dynspec Group
///  \details  
/// <br /> Usage: 
/// <br /> This class need as parameter all metadata of the dynamic spectrum Goup for process the data. 
/// Data have 3 dimensions (at the opposite to ICD3 data which have 2). The additionnal dimension is Stokes. 
/// The function stockDynspecData stocks  paramerter in private attributes, and the function writeDynspecData
/// process the data themselves. 


using namespace dal;

class Stock_Write_Dynspec_Data_Standalone
{  
  // Public Methods
  public:
  
    Stock_Write_Dynspec_Data_Standalone();
    ~Stock_Write_Dynspec_Data_Standalone();


    void stockDynspecData(int Ntime,int Nspectral);
    
    void writeDynspecData(Group &dynspec_grp,std::string obsName,std::string pathFile,std::string outputFile,File &root_grp,int i,int j,int k,int l,int obsNofStockes,std::vector<std::string> stokesComponent,
			      float memoryRAM, std::vector<std::string> listOfFiles);



  // Private Attributes
  private:

    int m_Ntime;
    int m_Nspectral;
};
 
#endif
