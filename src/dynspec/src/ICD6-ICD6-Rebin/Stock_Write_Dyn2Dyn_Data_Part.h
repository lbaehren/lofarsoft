#ifndef DEF_STOCK_WRITE_DYN2DYN_DATA_PART
#define DEF_STOCK_WRITE_DYN2DYN_DATA_PART

#include<string>
#include<iostream>
#include<vector>

#include <dal/lofar/BF_File.h>

using namespace dal;

class Stock_Write_Dyn2Dyn_Data_Part
{  
  // Public Methods
  public:
  
    Stock_Write_Dyn2Dyn_Data_Part();
    ~Stock_Write_Dyn2Dyn_Data_Part();


    void stockDynspecData(int Ntime,int Nspectral, int NtimeReal, int NspectralReal);
    
    void writeDynspecData(Group &dynspec_grp,std::string obsName,std::string pathFile,std::string outputFile,File &root_grp,int j,int k,int obsNofStockes,std::vector<std::string> stokesComponent,
			      float memoryRAM,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin);



  // Private Attributes
  private:

    int m_Ntime;
    int m_Nspectral;
    
    int m_NtimeReal;
    int m_NspectralReal;
};
 
#endif