#ifndef DEF_STOCK_WRITE_DYNSPEC_DATA_PART
#define DEF_STOCK_WRITE_DYNSPEC_DATA_PART

#include<string>
#include<iostream>
#include<vector>

#include <dal/lofar/BF_File.h>


/// \class Stock_Write_Dynspec_Data_Quick
///  \brief Class object for stocking data parameters, processing the (select or rebin) and write data Matrix in the ICD6's Dynspec Groups 
///  \details  
/// <br /> Usage: 
/// <br /> This class need as parameter all metadata of the dynamic spectrum Goup for process the data. 
/// Data have 3 dimensions (at the opposite to ICD3 data which have 2). The additionnal dimension is Stokes. 
/// The function stockDynspecData stocks  paramerter (for rebinning the data) in private attributes, and the function writeDynspecData
/// process the data themselves. 


using namespace dal;

class Stock_Write_Dynspec_Data_Quick
{  
  // Public Methods
  public:
  
    Stock_Write_Dynspec_Data_Quick();
    ~Stock_Write_Dynspec_Data_Quick();


    void stockDynspecData(int Ntime,int Nspectral, int NtimeReal, int NspectralReal);
    
    void writeDynspecData(Group &dynspec_grp,std::string obsName,std::string pathFile,std::string outputFile,File &root_grp,int i,int j,int k,int l,int q,int obsNofStockes,std::vector<std::string> stokesComponent,
			   int SAPindex,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin, float dataTimePercent, float dataSpectralPercent);



  // Private Attributes
  private:

    int m_Ntime;
    int m_Nspectral;
    
    int m_NtimeReal;
    int m_NspectralReal;
};
 
#endif