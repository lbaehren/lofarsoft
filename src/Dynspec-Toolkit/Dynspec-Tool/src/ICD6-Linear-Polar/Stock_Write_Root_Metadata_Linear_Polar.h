#ifndef DEF_STOCK_WRITE_ROOT_METADATA_LINEAR_POLAR
#define DEF_STOCK_WRITE_ROOT_METADATA_LINEAR_POLAR

#include<string>
#include<iostream>
#include<vector>

#include <dal/lofar/BF_File.h>


/// \class Stock_Write_Root_Metadata_Linear_Polar
///  \brief Class object for stock and write metadatas for the ICD6's Root Group
///  \details  
/// <br /> Usage: 
/// <br /> This class need as parameter all metadata for the ICD6( or dynamic spectrum)'s Root Group. 
/// The function stockRootMetadata stocks Root metadata in private attributes, and the function writeRootMetadata
/// write them in the Root Group of the hdf5 file in output


using namespace dal;

class Stock_Write_Root_Metadata_Linear_Polar
{  
  // Public Methods
  public:
  
    Stock_Write_Root_Metadata_Linear_Polar();
    ~Stock_Write_Root_Metadata_Linear_Polar();


    void stockRootMetadata(std::string attr_1,std::string attr_2,std::string attr_3,std::string attr_4,std::string attr_5,std::string attr_7,std::string attr_8,std::string attr_9,std::string attr_10,
			 std::string attr_11,std::string attr_12,std::string attr_13,double attr_14,std::string attr_16,double attr_17,int attr_19,
			 std::vector<std::string> attr_20,double attr_21,double attr_22,double attr_23,std::string attr_24,int attr_25,double attr_26,std::string attr_27,
			 std::string attr_28,std::string attr_29,std::string attr_30,std::string attr_31,std::string attr_32,std::string attr_33,std::string attr_34,std::string attr_35,std::string attr_36,
			 bool attr_37,int attr_38,int attr_39,std::string attr_40,std::string attr_41,std::string attr_42,double attr_45,double attr_46,double attr_47,
			 std::vector<std::string> attr_48,std::vector<std::string> attr_49,int attr_52,double attr_53,std::string attr_54,double attr_55,std::string attr_56,double attr_57,std::string attr_58,
			 int attr_59,double attr_60,std::string attr_61,double attr_62,std::string attr_63,double attr_64,std::vector<std::string> attr_65,std::vector<double> attr_66,
			 std::vector<double> attr_67,std::vector<double> attr_68);
		 
			 
    void writeRootMetadata(std::string outputFile,File &root_grp);

  // Private Attributes
  private:
    
    std::string m_attr_1;
    std::string m_attr_2;
    std::string m_attr_3;
    std::string m_attr_4;
    std::string m_attr_5;
    std::string m_attr_7;
    std::string m_attr_8;
    std::string m_attr_9;
    std::string m_attr_10;
    std::string m_attr_11; 
    std::string m_attr_12;
    std::string m_attr_13;
    double m_attr_14;
    std::string m_attr_16;
    double m_attr_17;
    int m_attr_19;  
    
    int m_size_attr_20;
    std::vector<std::string> m_attr_20;
        
    double m_attr_21;   
    double m_attr_22;
    double m_attr_23;
    
    std::string m_attr_24;
    int m_attr_25;
    double m_attr_26;
    std::string m_attr_27;
    std::string m_attr_28;
    std::string m_attr_29;
    std::string m_attr_30;    
    std::string m_attr_31;
    std::string m_attr_32;
    std::string m_attr_33;
    std::string m_attr_34;
    std::string m_attr_35;
    std::string m_attr_36;
    bool m_attr_37;
    int m_attr_38;
    int m_attr_39;
    std::string m_attr_40;
    std::string m_attr_41;
    std::string m_attr_42;

    double m_attr_45;
    double m_attr_46;
    double m_attr_47;
    
    int m_size_attr_48;
    std::vector<std::string> m_attr_48;
    
    int m_size_attr_49;
    std::vector<std::string> m_attr_49;

    int m_attr_52;
    double m_attr_53;
    std::string m_attr_54;
    double m_attr_55;
    std::string m_attr_56;
    double m_attr_57;
    std::string m_attr_58;
    int m_attr_59;
    double m_attr_60;
    std::string m_attr_61;
    double m_attr_62;
    std::string m_attr_63;
    double m_attr_64;
    
    int m_size_attr_65;
    std::vector<std::string> m_attr_65;
    
    int m_size_attr_66;
    std::vector<double> m_attr_66;
    
    int m_size_attr_67;
    std::vector<double> m_attr_67;
    
    int m_size_attr_68;
    std::vector<double> m_attr_68;

};
 
#endif
