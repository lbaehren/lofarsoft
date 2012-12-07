#ifndef DEF_STOCK_WRITE_DYNSPEC_DATA
#define DEF_STOCK_WRITE_DYNSPEC_DATA

#include<string>
#include<iostream>
#include<vector>

#include <dal/lofar/BF_File.h>

using namespace dal;

class Stock_Write_Dynspec_Data
{  
  // Public Methods
  public:
  
    Stock_Write_Dynspec_Data();
    ~Stock_Write_Dynspec_Data();


    void stockDynspecData(int Ntime,int Nspectral);
    
    void writeDynspecData(Group &dynspec_grp,std::string pathDir,std::string obsName,std::string pathFile,std::string outputFile,File &root_grp,int i,int j,int k,int l,int q,int obsNofStockes,std::vector<std::string> stokesComponent,
			      float memoryRAM, int SAPindex);



  // Private Attributes
  private:

    int m_Ntime;
    int m_Nspectral;
};
 
#endif
