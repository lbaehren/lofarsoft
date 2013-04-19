#include <iostream>
#include <string>

#include "Stock_Write_Root_Metadata_Partionned.h"

#include <dal/lofar/BF_File.h>


/// \file Stock_Write_Root_Metadata_Partionned.cpp
///  \brief File C++ (associated to Stock_Write_Root_Metadata_Partionned.h) for stock and write metadatas for the ICD6's Root Group 
///  \details  
/// <br /> Overview:
/// <br /> Functions stockRootMetadata and writeRootMetadata are described (programmed) here. 
/// The first one (stockRootMetadata) will take as parameter all Root Group's metadata and stock them in the 
/// private attributes of the class object. The second function (writeRootMetadata) will write them in the hdf5 output
/// file. 






using namespace dal;
using namespace std;


  Stock_Write_Root_Metadata_Partionned::Stock_Write_Root_Metadata_Partionned(){}
  Stock_Write_Root_Metadata_Partionned::~Stock_Write_Root_Metadata_Partionned(){}


  void Stock_Write_Root_Metadata_Partionned::stockRootMetadataPartionned(string attr_1,string attr_2,string attr_3,string attr_4,string attr_5,string attr_7,string attr_8,string attr_9,string attr_10,
				       string attr_11,string attr_12,string attr_13,double attr_14,string attr_16,double attr_17,int attr_19,
				       vector<string> attr_20,double attr_21,double attr_22,double attr_23,string attr_24,int attr_25,double attr_26,string attr_27,
				       string attr_28,string attr_29,string attr_30,string attr_31,string attr_32,string attr_33,string attr_34,string attr_35,string attr_36,
				       bool attr_37,int attr_38,int attr_39,string attr_40,string attr_41,string attr_42,double attr_45,double attr_46,double attr_47,
				       vector<string> attr_48,vector<string> attr_49,int attr_52,double attr_53,string attr_54,double attr_55,string attr_56,double attr_57,string attr_58,
				       int attr_59,double attr_60,string attr_61,double attr_62,string attr_63,double attr_64,vector<string> attr_65,vector<double> attr_66,
				       vector<double> attr_67,vector<double> attr_68) 
 {
    // Stock metadata in an object
    // ROOT group meta-data

/// <br /> Usage:
/// <br />   void Stock_Write_Root_Metadata_Partionned::stockRootMetadataPartionned(string attr_1,string attr_2,string attr_3,string attr_4,string attr_5,string attr_7,string attr_8,string attr_9,string attr_10,
/// string attr_11,string attr_12,string attr_13,double attr_14,string attr_16,double attr_17,int attr_19,
/// vector<string> attr_20,double attr_21,double attr_22,double attr_23,string attr_24,int attr_25,double attr_26,string attr_27,
/// string attr_28,string attr_29,string attr_30,string attr_31,string attr_32,string attr_33,string attr_34,string attr_35,string attr_36,
/// bool attr_37,int attr_38,int attr_39,string attr_40,string attr_41,string attr_42,double attr_45,double attr_46,double attr_47,
/// vector<string> attr_48,vector<string> attr_49,int attr_52,double attr_53,string attr_54,double attr_55,string attr_56,double attr_57,string attr_58,
/// int attr_59,double attr_60,string attr_61,double attr_62,string attr_63,double attr_64,vector<string> attr_65,vector<double> attr_66,
/// vector<double> attr_67,vector<double> attr_68) 
	
/// \param  attr_1 GROUPETYPE
/// \param  attr_2 FILENAME
/// \param  attr_3 FILEDATE
/// \param  attr_4 FILETYPE
/// \param  attr_5 TELESCOPE
/// \param  attr_7 PROJECT_ID
/// \param  attr_8 PROJECT_TITLE
/// \param  attr_9 PROJECT_PI
/// \param  attr_10 PROJECT_CO_I
/// \param  attr_11 PROJECT_CONTACT
/// \param  attr_12 OBSERVATION_ID
/// \param  attr_13 OBSERVATION_START_UTC
/// \param  attr_14 OBSERVATION_START_MJD
/// \param  attr_16 OBSERVATION_END_UTC
/// \param  attr_17 OBSERVATION_END_MJD
/// \param  attr_19 OBSERVATION_NOF_STATIONS
/// \param  attr_20 OBSERVATION_STATIONS_LIST
/// \param  attr_21 OBSERVATION_FREQUENCY_MAX
/// \param  attr_22 OBSERVATION_FREQUENCY_MIN
/// \param  attr_23 OBSERVATION_FREQUENCY_CENTER
/// \param  attr_24 OBSERVATION_FREQUENCY_UNIT
/// \param  attr_25 OBSERVATION_NOF_BITS_PER_SAMPLE
/// \param  attr_26 CLOCK_FREQUENCY
/// \param  attr_27 CLOCK_FREQUENCY_UNIT
/// \param  attr_28 ANTENNA_SET
/// \param  attr_29 FILTER_SELECTION
/// \param  attr_30 TARGET
/// \param  attr_31 SYSTEM_VERSION
/// \param  attr_32 PIPELINE_NAME
/// \param  attr_33 PIPELINE_VERSION
/// \param  attr_34 DOC_NAME
/// \param  attr_35 DOC_VERSION
/// \param  attr_36 NOTES
/// \param  attr_37 DYNSPEC_GROUP
/// \param  attr_38 NOF_DYNSPEC
/// \param  attr_39 NOF_TILED_DYNSPEC
/// \param  attr_40 CREATE_OFFLINE_ONLINE
/// \param  attr_41 BF_FORMAT
/// \param  attr_42 BF_VERSION
/// \param  attr_45 PRIMARY_POINTING_DIAMETER
/// \param  attr_46 POINT_RA
/// \param  attr_47 POINT_DEC
/// \param  attr_48 POINT_ALTITUDE
/// \param  attr_49 POINT_AZIMUTH
/// \param  attr_52 NOF_SAMPLES
/// \param  attr_53 SAMPLING_RATE
/// \param  attr_54 SAMPLING_RATE_UNIT
/// \param  attr_55 SAMPLING_TIME
/// \param  attr_56 SAMPLING_TIME_UNIT
/// \param  attr_57 TOTAL_INTEGRATION_TIME
/// \param  attr_58 TOTAL_INTEGRATION_TIME_UNIT
/// \param  attr_59 CHANNELS_PER_SUBANDS
/// \param  attr_60 SUBBAND_WIDTH
/// \param  attr_61 SUBBAND_WIDTH_UNIT
/// \param  attr_62 CHANNEL_WIDTH
/// \param  attr_63 CHANNEL_WIDTH_UNIT
/// \param  attr_64 TOTAL_BAND_WIDTH
/// \param  attr_65 WHEATER_STATIONS_LIST
/// \param  attr_66 WHEATER_TEMPERATURE
/// \param  attr_67 WHEATER_HUMIDITY
/// \param  attr_68 SYSTEM_TEMPERATURE
 
/// \return nothing       
    
    
    
    m_attr_1	= attr_1;
    m_attr_2	= attr_2;
    m_attr_3	= attr_3;
    m_attr_4	= attr_4;
    m_attr_5	= attr_5;
    m_attr_7	= attr_7;
    m_attr_8	= attr_8;
    m_attr_9	= attr_9;
    m_attr_10	= attr_10;
    m_attr_11	= attr_11;
    m_attr_12	= attr_12;
    m_attr_13	= attr_13;
    m_attr_14	= attr_14;
    m_attr_16	= attr_16;
    m_attr_17	= attr_17;
    m_attr_19	= attr_19;
    
    m_size_attr_20= attr_20.size();
    m_attr_20.resize(m_size_attr_20);
    for (size_t i=0;i< attr_20.size();i++)
    {m_attr_20[i] = attr_20[i];}

    m_attr_21 	= attr_21;
    m_attr_22 	= attr_22;
    m_attr_23 	= attr_23;       
    m_attr_24	= attr_24;
    m_attr_25	= attr_25;
    m_attr_26	= attr_26;
    m_attr_27	= attr_27;
    m_attr_28	= attr_28;
    m_attr_29	= attr_29;
    m_attr_30	= attr_30;          
    m_attr_31	= attr_31;
    m_attr_32	= attr_32;
    m_attr_33	= attr_33;
    m_attr_34	= attr_34;
    m_attr_35	= attr_35;
    m_attr_36	= attr_36;
    m_attr_37	= attr_37;
    m_attr_38	= attr_38;
    m_attr_39	= attr_39;
    m_attr_40	= attr_40;
    m_attr_41	= attr_41;
    m_attr_42	= attr_42;
     
    m_attr_45	= attr_45;
    m_attr_46	= attr_46;
    m_attr_47	= attr_47;
    
    m_size_attr_48= attr_48.size();
    m_attr_48.resize(m_size_attr_48);
    for (size_t i=0;i< attr_48.size();i++)
    {m_attr_48[i] = attr_48[i];}  
    
    m_size_attr_49= attr_49.size();
    m_attr_49.resize(m_size_attr_49);
    for (size_t i=0;i< attr_49.size();i++)
    {m_attr_49[i] = attr_49[i];}   
    
    
    m_attr_52	= attr_52;
    m_attr_53	= attr_53;
    m_attr_54	= attr_54;
    m_attr_55	= attr_55;
    m_attr_56	= attr_56;
    m_attr_57	= attr_57;
    m_attr_58	= attr_58;
    m_attr_59	= attr_59;
    m_attr_60	= attr_60;
    m_attr_61	= attr_61;
    m_attr_62	= attr_62;
    m_attr_63	= attr_63;
    m_attr_64	= attr_64;
				        
    m_size_attr_65= attr_65.size();
    m_attr_65.resize(m_size_attr_65);
    for (size_t i=0;i< attr_65.size();i++)
    {m_attr_65[i] = attr_65[i];}  
    
    m_size_attr_66= attr_66.size();
    m_attr_66.resize(m_size_attr_66);
    for (size_t i=0;i< attr_66.size();i++)
    {m_attr_66[i] = attr_66[i];}  
    
    m_size_attr_67= attr_67.size();
    m_attr_67.resize(m_size_attr_67);
    for (size_t i=0;i< attr_67.size();i++)
    {m_attr_67[i] = attr_67[i];}  
    
    m_size_attr_68= attr_68.size();
    m_attr_68.resize(m_size_attr_68);
    for (size_t i=0;i< attr_68.size();i++)
    {m_attr_68[i] = attr_68[i];}  

  }



  void Stock_Write_Root_Metadata_Partionned::writeRootMetadataPartionned(string outputFile,File &root_grp)
  {
    
/// <br /> Usage:
/// <br /> void Stock_Write_Root_Metadata_Partionned::writeRootMetadataPartionned(string outputFile,File &root_grp)

/// \param outputFile this is the output file (dynamic spectrum file)
/// \param &root_grp Stock_Write_Root_Metadata_Partionned Object for catching stocked private attributes
    
      // Write the ROOT meta-data in the output file
    
      Attribute<string> attr_01(root_grp, "GROUPETYPE");  
      Attribute<string> attr_02(root_grp, "FILENAME");
      Attribute<string> attr_03(root_grp, "FILEDATE");
      Attribute<string> attr_04(root_grp, "FILETYPE");      
      Attribute<string> attr_05(root_grp, "TELESCOPE");
      Attribute<string> attr_07(root_grp, "PROJECT_ID");
      Attribute<string> attr_08(root_grp, "PROJECT_TITLE");
      Attribute<string> attr_09(root_grp, "PROJECT_PI");
      Attribute<string> attr_10(root_grp, "PROJECT_CO_I");
      Attribute<string> attr_11(root_grp, "PROJECT_CONTACT");  
      Attribute<string> attr_12(root_grp, "OBSERVATION_ID");
      Attribute<string> attr_13(root_grp, "OBSERVATION_START_UTC");
      Attribute<double> attr_14(root_grp, "OBSERVATION_START_MJD");
      Attribute<string> attr_16(root_grp, "OBSERVATION_END_UTC");
      Attribute<double> attr_17(root_grp, "OBSERVATION_END_MJD");
      Attribute<int> attr_19(root_grp, "OBSERVATION_NOF_STATIONS");
      Attribute< vector<string> > attr_20(root_grp, "OBSERVATION_STATIONS_LIST");
      Attribute<double> attr_21(root_grp, "OBSERVATION_FREQUENCY_MAX");
      Attribute<double> attr_22(root_grp, "OBSERVATION_FREQUENCY_MIN");
      Attribute<double> attr_23(root_grp, "OBSERVATION_FREQUENCY_CENTER");
      Attribute<string> attr_24(root_grp, "OBSERVATION_FREQUENCY_UNIT");
      Attribute<int> attr_25(root_grp, "OBSERVATION_NOF_BITS_PER_SAMPLE");     
      Attribute<double> attr_26(root_grp, "CLOCK_FREQUENCY");
      Attribute<string> attr_27(root_grp, "CLOCK_FREQUENCY_UNIT");
      Attribute<string> attr_28(root_grp, "ANTENNA_SET");      
      Attribute<string> attr_29(root_grp, "FILTER_SELECTION");
      Attribute<string> attr_30(root_grp, "TARGET");      
      Attribute<string> attr_31(root_grp, "SYSTEM_VERSION");
      Attribute<string> attr_32(root_grp, "PIPELINE_NAME");
      Attribute<string> attr_33(root_grp, "PIPELINE_VERSION");
      Attribute<string> attr_34(root_grp, "DOC_NAME");
      Attribute<string> attr_35(root_grp, "DOC_VERSION");
      Attribute<string> attr_36(root_grp, "NOTES");
      Attribute<bool> attr_37(root_grp, "DYNSPEC_GROUP");
      Attribute<int> attr_38(root_grp, "NOF_DYNSPEC");
      Attribute<int> attr_39(root_grp, "NOF_TILED_DYNSPEC");
      Attribute<string> attr_40(root_grp, "CREATE_OFFLINE_ONLINE");      
      Attribute<string> attr_41(root_grp, "BF_FORMAT");
      Attribute<string> attr_42(root_grp, "BF_VERSION");
      Attribute<double> attr_45(root_grp, "PRIMARY_POINTING_DIAMETER");
      Attribute<double> attr_46(root_grp, "POINT_RA");
      Attribute<double> attr_47(root_grp, "POINT_DEC");      
      Attribute< vector<string> > attr_48(root_grp, "POINT_ALTITUDE");
      Attribute< vector<string> > attr_49(root_grp, "POINT_AZIMUTH");          
      Attribute<int> attr_52(root_grp, "NOF_SAMPLES");      
      Attribute<double> attr_53(root_grp, "SAMPLING_RATE");
      Attribute<string> attr_54(root_grp, "SAMPLING_RATE_UNIT");      
      Attribute<double> attr_55(root_grp, "SAMPLING_TIME");
      Attribute<string> attr_56(root_grp, "SAMPLING_TIME_UNIT");     
      Attribute<double> attr_57(root_grp, "TOTAL_INTEGRATION_TIME");
      Attribute<string> attr_58(root_grp, "TOTAL_INTEGRATION_TIME_UNIT");  
      Attribute<int> attr_59(root_grp, "CHANNELS_PER_SUBANDS");      
      Attribute<double> attr_60(root_grp, "SUBBAND_WIDTH");
      Attribute<string> attr_61(root_grp, "SUBBAND_WIDTH_UNIT");      
      Attribute<double> attr_62(root_grp, "CHANNEL_WIDTH");
      Attribute<string> attr_63(root_grp, "CHANNEL_WIDTH_UNIT");
      Attribute<double> attr_64(root_grp, "TOTAL_BAND_WIDTH");      
      Attribute< vector<string> > attr_65(root_grp, "WHEATER_STATIONS_LIST");
      Attribute< vector<double> > attr_66(root_grp, "WHEATER_TEMPERATURE");
      Attribute< vector<double> > attr_67(root_grp, "WHEATER_HUMIDITY");
      Attribute< vector<double> > attr_68(root_grp, "SYSTEM_TEMPERATURE");
      
      attr_01.value = m_attr_1;      
      attr_02.value = m_attr_2;
      attr_03.value = m_attr_3;
      attr_04.value = m_attr_4;
      attr_05.value = m_attr_5;
      attr_07.value = m_attr_7;
      attr_08.value = m_attr_8;
      attr_09.value = m_attr_9;
      attr_10.value = m_attr_10;    
      attr_11.value = m_attr_11;
      attr_12.value = m_attr_12;
      attr_13.value = m_attr_13;
      attr_14.value = m_attr_14;
      attr_16.value = m_attr_16;
      attr_17.value = m_attr_17;
      attr_19.value = m_attr_19;      
      attr_20.create().set(m_attr_20);
      attr_21.value = m_attr_21;
      attr_22.value = m_attr_22;
      attr_23.value = m_attr_23;
      attr_24.value = m_attr_24;
      attr_25.value = m_attr_25;      
      attr_26.value = m_attr_26;
      attr_27.value = m_attr_27;
      attr_28.value = m_attr_28;     
      attr_29.value = m_attr_29;      
      attr_30.create().set(m_attr_30);            
      attr_31.value = m_attr_31;
      attr_32.value = m_attr_32;
      attr_33.value = m_attr_33;
      attr_34.value = m_attr_34;
      attr_35.value = m_attr_35;
      attr_36.value = m_attr_36;            
      attr_37.value = m_attr_37;
      attr_38.value = m_attr_38;
      attr_39.value = m_attr_39;
      attr_40.value = m_attr_40;      
      attr_41.value = m_attr_41;
      attr_42.value = m_attr_42;   
      attr_45.value = m_attr_45;
      attr_46.value = m_attr_46;
      attr_47.value = m_attr_47;   
      attr_48.create().set(m_attr_48);
      attr_49.create().set(m_attr_49);        
      attr_52.value = m_attr_52;    
      attr_53.value = m_attr_53;
      attr_54.value = m_attr_54;      
      attr_55.value = m_attr_55;
      attr_56.value = m_attr_56;      
      attr_57.value = m_attr_57;
      attr_58.value = m_attr_58;      
      attr_59.value = m_attr_59;      
      attr_60.value = m_attr_60;
      attr_61.value = m_attr_61;      
      attr_62.value = m_attr_62;
      attr_63.value = m_attr_63;
      attr_64.value = m_attr_64;      
      attr_65.create().set(m_attr_65);
      attr_66.create().set(m_attr_66);
      attr_67.create().set(m_attr_67);
      attr_68.create().set(m_attr_68);   
      
      Group sys_log_grp(root_grp, "SYS_LOG");
      sys_log_grp.create();
      
      
  }