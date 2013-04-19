#include <iostream>
#include <string>

#include <sstream>

#include "Reader_Root_Substraction.h"
#include "Stock_Write_Root_Metadata_Substraction.h"

#include <dal/lofar/BF_File.h>


/// \file Reader_Root_Substraction.cpp
///  \brief File C++ (associated to Reader_Root_Substraction.h) for read ICD3 files and put in variables metadatas for the ICD6's Root Group  
///  \details  
/// <br /> Overview:
/// <br /> This function readRoot is described (programmed) here. It needs as parameter a Stock_Write_Root_Metadata_Substraction_Part object. The readRoot function will read 
///  root metadatas and stock them in the Stock_Write_Root_Metadata_Substraction_Part object as private attributes thanks to stockRootMetadata function.
///  These attributes will be written in the next step by writeRootMetadata function which is in the class 
///  Stock_Write_Root_Metadata_Substraction_Part.h  and called at the end of this Reader_Root_Substraction.cpp





using namespace dal;
using namespace std;


  Reader_Root_Substraction::Reader_Root_Substraction(){}	// constructor 
  Reader_Root_Substraction::~Reader_Root_Substraction(){}	// destructor
  
  
    
  void Reader_Root_Substraction::readRoot(Stock_Write_Root_Metadata_Substraction *rootMetadata,string ID,string file1,string dynspec1,string file2,string dynspec2,float kfactor)
    { 

/// <br /> Usage:
/// <br />   void Reader_Root_Substraction::readRoot(string pathFile,string obsName,Stock_Write_Root_Metadata_Substraction *rootMetadata,int obsNofSource,int obsNofFrequencyBand,int obsNofTiledDynspec, string SAPname, int SAPindex)

 
/// \param pathFileICD3 file to read
/// \param obsName observation ID
/// \param *rootMetadata Stock_Write_Root_Metadata_Substraction_Part class object
/// \param  obsNofSource number of sources
/// \param  obsNofFrequencyBandnumber of frequency band 
/// \param  obsNofTiledDynspecnumber of tiled dynspec
/// \param  SAPname name of SubArrayPointing
/// \param  SAPindex index of SubArrayPointing
      
   
/// \return nothing     



  // Start the substraction => load datas => use file 1 for the root group 
  
  // First Dynspec 
  File  File1(file1);  					// generate an object called file contains Root
  Group Dynspec1(File1,dynspec1);				// generate an object called Dynspec contains 1 Dynspec
  Group COORDS1(Dynspec1,"COORDINATES");			// Generate an Objet called COORDS contains Coords group      
  Group COORDS_POLARIZATION1(COORDS1,"POLARIZATION");	 
  Group COORDS_SPECTRAL1(COORDS1,"SPECTRAL");
  Group COORDS_TIME1(COORDS1,"TIME");     
  Dataset<float> STOCKES1(Dynspec1, "DATA");		// generate an object called STOCKES contains stockes data  
  

      
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // ROOT
  ///////////////////////////////////////////////////////////      
  
  // Normal root metadata
  
  //Variables Initialization && General Root group variables 
  
  Attribute<string> obj_GROUPETYPE(File1, "GROUPETYPE");
//   Attribute<string> obj_FILENAME(File1, "FILENAME");
  Attribute<string> obj_FILEDATE(File1, "FILEDATE");
  Attribute<string> obj_FILETYPE(File1, "FILETYPE");
  Attribute<string> obj_TELESCOPE(File1, "TELESCOPE");   
//   Attribute<string> obj_PROJECT_ID(File1,  "PROJECT_ID");
//   Attribute<string> obj_PROJECT_TITLE(File1,"PROJECT_TITLE");
  Attribute<string> obj_PROJECT_PI(File1, "PROJECT_PI");
  Attribute<string> obj_PROJECT_CO_I(File1,"PROJECT_CO_I");
  Attribute<string> obj_PROJECT_CONTACT(File1,"PROJECT_CONTACT");      
  Attribute<string> obj_OBSERVATION_ID(File1, "OBSERVATION_ID");
  
  string GROUPETYPE(obj_GROUPETYPE.value);
  string FILENAME("Dynspec_Subsracted_"+ID+".h5");
  string FILEDATE(obj_FILEDATE.value);
  string FILETYPE(obj_FILETYPE.value);
  string TELESCOPE(obj_TELESCOPE.value);      
  string PROJECT_ID(ID);
  string PROJECT_TITLE("Dynspec Substraction");
  string PROJECT_PI(obj_PROJECT_PI.value);
  string PROJECT_CO_I(obj_PROJECT_CO_I.value);
  string PROJECT_CONTACT(obj_PROJECT_CONTACT.value);     
  string OBSERVATION_ID(obj_OBSERVATION_ID.value);  
  

  Attribute<string> obj_OBSERVATION_START_UTC(File1, "OBSERVATION_START_UTC");
  Attribute<double> obj_OBSERVATION_START_MJD(File1, "OBSERVATION_START_MJD");
  Attribute<string> obj_OBSERVATION_END_UTC(File1, "OBSERVATION_END_UTC");
  Attribute<double> obj_OBSERVATION_END_MJD(File1, "OBSERVATION_END_MJD");     
  
  string OBSERVATION_START_UTC(obj_OBSERVATION_START_UTC.value);
  double OBSERVATION_START_MJD(obj_OBSERVATION_START_MJD.value);
  string OBSERVATION_END_UTC(obj_OBSERVATION_END_UTC.value);
  double OBSERVATION_END_MJD(obj_OBSERVATION_END_MJD.value);
  
  
  Attribute<int> obj_OBSERVATION_NOF_STATIONS(File1, "OBSERVATION_NOF_STATIONS");
  Attribute< vector<string> > obj_OBSERVATION_STATIONS_LIST(File1, "OBSERVATION_STATIONS_LIST");
  Attribute<int> obj_OBSERVATION_NOF_BITS_PER_SAMPLE(File1, "OBSERVATION_NOF_BITS_PER_SAMPLE");
  
  int    OBSERVATION_NOF_STATIONS(obj_OBSERVATION_NOF_STATIONS.value);
  vector<string>  OBSERVATION_STATIONS_LIST(obj_OBSERVATION_STATIONS_LIST.value);
  int    OBSERVATION_NOF_BITS_PER_SAMPLE(obj_OBSERVATION_NOF_BITS_PER_SAMPLE.value);    
  string OBSERVATION_FREQUENCY_UNIT("Mhz"); 
  
  
  Attribute<double> obj_CLOCK_FREQUENCY(File1, "CLOCK_FREQUENCY");
  Attribute<string> obj_ANTENNA_SET(File1, "ANTENNA_SET");
  Attribute<string> obj_FILTER_SELECTION(File1, "FILTER_SELECTION");
  Attribute<string> obj_TARGET(File1, "TARGET");
  Attribute<string> obj_SYSTEM_VERSION(File1, "SYSTEM_VERSION");
  Attribute<string> obj_NOTES(File1, "NOTES");
  
  double CLOCK_FREQUENCY(obj_CLOCK_FREQUENCY.value);     
  string ANTENNA_SET(obj_ANTENNA_SET.value);      
  string FILTER_SELECTION(obj_FILTER_SELECTION.value);     
  string TARGET(obj_TARGET.value);         
  string SYSTEM_VERSION(obj_SYSTEM_VERSION.value);
  string NOTES(obj_NOTES.value);
  
  
  string CLOCK_FREQUENCY_UNIT("Mhz");
  string PIPELINE_NAME("SUBSTRACTION OF DYNAMIC SPECTRUM");
  string PIPELINE_VERSION("Dynspec v.2.0");     
  string DOC_NAME("ICD6:Dynamic-Spectrum Data");
  string DOC_VERSION("ICD6:v-2.03.10");  
  
  
  
  Attribute<double> obj_OBSERVATION_FREQUENCY_CENTER(File1, "OBSERVATION_FREQUENCY_CENTER");
  Attribute<double> obj_OBSERVATION_FREQUENCY_MAX(File1, "OBSERVATION_FREQUENCY_MAX");
  Attribute<double> obj_OBSERVATION_FREQUENCY_MIN( File1, "OBSERVATION_FREQUENCY_MIN");
  
  double OBSERVATION_FREQUENCY_CENTER(obj_OBSERVATION_FREQUENCY_CENTER.value);
  double OBSERVATION_FREQUENCY_MAX(obj_OBSERVATION_FREQUENCY_MAX.value);
  double OBSERVATION_FREQUENCY_MIN(obj_OBSERVATION_FREQUENCY_MIN.value);
  
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Variables Initialization && Additionnal Root group variables      
  
  Attribute<string> obj_BF_FORMAT(File1, "BF_FORMAT");
  Attribute<string> obj_BF_VERSION(File1, "BF_VERSION");
  Attribute<double> obj_POINT_RA(File1, "POINT_RA");
  Attribute<double> obj_POINT_DEC(File1, "POINT_DEC");
  Attribute<double> obj_SUBBAND_WIDTH(File1, "SUBBAND_WIDTH");
  
  string BF_FORMAT(obj_BF_FORMAT.value);
  string BF_VERSION(obj_BF_VERSION.value);
  double POINT_RA(obj_POINT_RA.value);
  double POINT_DEC(obj_POINT_DEC.value);
  double SUBBAND_WIDTH(obj_SUBBAND_WIDTH.value);      
  
  
  bool DYNSPEC_GROUP(true);
  int NOF_DYNSPEC(1);     
  int NOF_TILED_DYNSPEC(0);
  string CREATE_OFFLINE_ONLINE("Offline");
  double PRIMARY_POINTING_DIAMETER(0);
  vector<string> POINT_ALTITUDE(OBSERVATION_STATIONS_LIST.size(),"none");
  vector<string> POINT_AZIMUTH(OBSERVATION_STATIONS_LIST.size(),"none"); 
  
  
  Attribute<int> obj_NOF_SAMPLES(File1, "NOF_SAMPLES");
  Attribute<double> obj_SAMPLING_RATE(File1, "SAMPLING_RATE");		
  Attribute<double> obj_SAMPLING_TIME(File1, "SAMPLING_TIME");
  Attribute<double> obj_TOTAL_INTEGRATION_TIME(File1, "TOTAL_INTEGRATION_TIME"); 
  Attribute<int> obj_CHANNELS_PER_SUBANDS(File1, "CHANNELS_PER_SUBANDS");
  Attribute<double> obj_CHANNEL_WIDTH(File1, "CHANNEL_WIDTH");
  Attribute<double> obj_TOTAL_BAND_WIDTH(File1, "TOTAL_BAND_WIDTH");   
  
  int NOF_SAMPLES(obj_NOF_SAMPLES.value);     
  double SAMPLING_RATE(obj_SAMPLING_RATE.value);	
  double SAMPLING_TIME(obj_SAMPLING_TIME.value);
  double TOTAL_INTEGRATION_TIME(obj_TOTAL_INTEGRATION_TIME.value);
  int CHANNELS_PER_SUBANDS(obj_CHANNELS_PER_SUBANDS.value);      
  double CHANNEL_WIDTH(obj_CHANNEL_WIDTH.value); 
  double TOTAL_BAND_WIDTH(obj_TOTAL_BAND_WIDTH.value);      
  
  string SAMPLING_RATE_UNIT("Mhz"); 
  string SAMPLING_TIME_UNIT("μs");      
  string TOTAL_INTEGRATION_TIME_UNIT("s");
  string SUBBAND_WIDTH_UNIT("Mhz"); 
  string CHANNEL_WIDTH_UNIT("Mhz");
  
  
  vector<string>  WHEATER_STATIONS_LIST(OBSERVATION_STATIONS_LIST.size(),"none");
  vector<double>  WHEATER_TEMPERATURE(OBSERVATION_STATIONS_LIST.size(),0);
  vector<double>  WHEATER_HUMIDITY(OBSERVATION_STATIONS_LIST.size(),0);
  vector<double>  SYSTEM_TEMPERATURE(OBSERVATION_STATIONS_LIST.size(),0);

      
  ///////////////////////////////////////////////////////////////////////////////////////       
  // END ROOT DATA  //
  //////////////////////////////////////////////////////////////////////////////////////       
  
  //Object generation => STOCK ROOT METADATA
  
  
  rootMetadata->stockRootMetadata(GROUPETYPE,FILENAME,FILEDATE,FILETYPE,TELESCOPE,PROJECT_ID,PROJECT_TITLE,PROJECT_PI,PROJECT_CO_I,   // attr10
			       PROJECT_CONTACT,OBSERVATION_ID,OBSERVATION_START_UTC,OBSERVATION_START_MJD,OBSERVATION_END_UTC,OBSERVATION_END_MJD,OBSERVATION_NOF_STATIONS,  //attr19
			       OBSERVATION_STATIONS_LIST,OBSERVATION_FREQUENCY_MAX,OBSERVATION_FREQUENCY_MIN,OBSERVATION_FREQUENCY_CENTER,OBSERVATION_FREQUENCY_UNIT,OBSERVATION_NOF_BITS_PER_SAMPLE,CLOCK_FREQUENCY,CLOCK_FREQUENCY_UNIT,   //attr 27
			       ANTENNA_SET,FILTER_SELECTION,TARGET,SYSTEM_VERSION,PIPELINE_NAME,PIPELINE_VERSION,DOC_NAME,DOC_VERSION,NOTES,  //attr36
			       DYNSPEC_GROUP,NOF_DYNSPEC,NOF_TILED_DYNSPEC,CREATE_OFFLINE_ONLINE,BF_FORMAT,BF_VERSION,PRIMARY_POINTING_DIAMETER,POINT_RA,POINT_DEC,  //attr47
			       POINT_ALTITUDE,POINT_AZIMUTH,NOF_SAMPLES,SAMPLING_RATE,SAMPLING_RATE_UNIT,SAMPLING_TIME,SAMPLING_TIME_UNIT,TOTAL_INTEGRATION_TIME,TOTAL_INTEGRATION_TIME_UNIT,  //attr58
			       CHANNELS_PER_SUBANDS,SUBBAND_WIDTH,SUBBAND_WIDTH_UNIT,CHANNEL_WIDTH,CHANNEL_WIDTH_UNIT,TOTAL_BAND_WIDTH,WHEATER_STATIONS_LIST,WHEATER_TEMPERATURE,  //attr66
			       WHEATER_HUMIDITY,SYSTEM_TEMPERATURE);  //attr68  
  
  
 
      
    }  // end of the Reader_Root_Substraction object
