#ifndef DEF_STOCK_WRITE_DYNSPEC_DATA_LINEAR_POLAR
#define DEF_STOCK_WRITE_DYNSPEC_DATA_LINEAR_POLAR

#include<string>
#include<iostream>
#include<vector>

#include <dal/lofar/BF_File.h>


/// \class Stock_Write_Dynspec_Data_Linear_Polar
///  \brief Class object for stocking data parameters and write datas for the ICD6's Dynspec Group
///  \details  
/// <br /> Usage: 
/// <br /> This class need as parameter all metadata of the dynamic spectrum Goup for process the data. 
/// Data have 3 dimensions (at the opposite to ICD3 data which have 2). The additionnal dimension is Stokes. 
/// The function stockDynspecData stocks  paramerter in private attributes, and the function writeDynspecData
/// process the data themselves. 


using namespace dal;

class Stock_Write_Dynspec_Data_Linear_Polar
{  
  // Public Methods
  public:
  
    Stock_Write_Dynspec_Data_Linear_Polar();
    ~Stock_Write_Dynspec_Data_Linear_Polar();


    void stockDynspecData(int Ntime,int Nspectral);
    
    void writeDynspecData(Group &dynspec_grp,std::string ID,std::string filename,std::string dynspec, std::string outputFile,File &root_grp,int nbSTOkES,std::vector<std::string> stokesComponent,float memoryRAM);

    

  // Private Attributes
  private:

    int m_Ntime;
    int m_Nspectral;
};
 
#endif
