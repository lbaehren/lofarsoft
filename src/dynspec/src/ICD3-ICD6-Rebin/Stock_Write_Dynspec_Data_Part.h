#ifndef DEF_STOCK_WRITE_DYNSPEC_DATA_PART
#define DEF_STOCK_WRITE_DYNSPEC_DATA_PART

#include<string>
#include<iostream>
#include<vector>

#include <dal/lofar/BF_File.h>

using namespace dal;

class Stock_Write_Dynspec_Data_Part
{  
  // Public Methods
  public:
  
    Stock_Write_Dynspec_Data_Part();
    ~Stock_Write_Dynspec_Data_Part();


    void stockDynspecData(int Ntime,int Nspectral, int NtimeReal, int NspectralReal);
    
    void writeDynspecData(Group &dynspec_grp,std::string pathDir,std::string obsName,std::string pathFile,std::string outputFile,File &root_grp,int i,int j,int k,int l,int q,int obsNofStockes,std::vector<std::string> stokesComponent,
			      float memoryRAM, int SAPindex,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin);



  // Private Attributes
  private:

    int m_Ntime;
    int m_Nspectral;
    
    int m_NtimeReal;
    int m_NspectralReal;
};
 
#endif