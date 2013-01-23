#include <iostream>
#include <string>

#include <sstream>

#include "Reader_Root.h"
#include "Stock_Write_Root_Metadata.h"

#include <dal/lofar/BF_File.h>


/// \file Reader_Root.cpp
///  \brief File C++ (associated to Reader_Root.h) for read ICD3 files and put in variables metadatas for the ICD6's Root Group  
///  \details  
/// <br /> Overview:
/// <br /> This function readRoot is described (programmed) here. It needs as parameter a Stock_Write_Root_Metadata_Part object. The readRoot function will read 
///  root metadatas and stock them in the Stock_Write_Root_Metadata_Part object as private attributes thanks to stockRootMetadata function.
///  These attributes will be written in the next step by writeRootMetadata function which is in the class 
///  Stock_Write_Root_Metadata_Part.h  and called at the end of this Reader_Root.cpp





using namespace dal;
using namespace std;


  Reader_Root::Reader_Root(){}	// constructor 
  Reader_Root::~Reader_Root(){}	// destructor
  
  
    
  void Reader_Root::readRoot(string pathFile,string obsName,Stock_Write_Root_Metadata *rootMetadata,int obsNofSource,int obsNofBeam,int obsNofFrequencyBand,int obsNofTiledDynspec, string SAPname, int SAPindex, int j_indicator)
    { 

/// <br /> Usage:
/// <br />   void Reader_Root::readRoot(string pathFile,string obsName,Stock_Write_Root_Metadata *rootMetadata,int obsNofSource,int obsNofBeam,int obsNofFrequencyBand,int obsNofTiledDynspec, string SAPname, int SAPindex)

 
/// \param pathFileICD3 file to read
/// \param obsName observation ID
/// \param *rootMetadata Stock_Write_Root_Metadata_Part class object
/// \param  obsNofSource number of sources
/// \param  obsNofBeam number of beam
/// \param  obsNofFrequencyBandnumber of frequency band 
/// \param  obsNofTiledDynspecnumber of tiled dynspec
/// \param  SAPname name of SubArrayPointing
/// \param  SAPindex index of SubArrayPointing
      
   
/// \return nothing         
      
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Programm  Load the hdf5's root part

      BF_File file(pathFile);  							// generate an object called file contains Root
  

      BF_SubArrayPointing SAP 		= file.subArrayPointing(SAPindex);		// generate an object called SAP contains subarray pointings

      BF_BeamGroup BEAM			= SAP.beam(j_indicator);      			// generate an object called BEAM contains Beams

      
      CoordinatesGroup COORDS		= BEAM.coordinates();      		// generate an object called COORDS contains Coodinates directory
      Coordinate *TIME_COORDS		= COORDS.coordinate(0);			// generate an object called TIME_COORDS contains time data
      Coordinate *SPECTRAL_COORDS	= COORDS.coordinate(1);			// generate an object called SPECTRAL_COORDS contains spectral data

      NumericalCoordinate *NUM_COORD_TIME = dynamic_cast<NumericalCoordinate*>(TIME_COORDS); // load Numerical coordinates lib 
      DirectionCoordinate *DIR_COORD_TIME = dynamic_cast<DirectionCoordinate*>(TIME_COORDS); // load Direction coordinates lib
      
      NumericalCoordinate *NUM_COORD_SPECTRAL = dynamic_cast<NumericalCoordinate*>(SPECTRAL_COORDS); // load Numerical coordinates lib 
      DirectionCoordinate *DIR_COORD_SPECTRAL = dynamic_cast<DirectionCoordinate*>(SPECTRAL_COORDS); // load Direction coordinates lib
      

      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // ROOT
      ///////////////////////////////////////////////////////////      
      //Variables Initialization && General Root group variables 

      string GROUPETYPE("Root");
      string FILENAME(obsName+SAPname+".h5");     
      string FILEDATE(file.fileDate().get());
      string FILETYPE("dynspec");
      string TELESCOPE("LOFAR");
      
      string PROJECT_ID(file.projectID().get());
      string PROJECT_TITLE(file.projectTitle().get());
      string PROJECT_PI(file.projectPI().get());
      string PROJECT_CO_I(file.projectCOI().get());   
      string PROJECT_CONTACT(file.projectContact().get());
      
      string OBSERVATION_ID(file.observationID().get());
      
      string OBSERVATION_START_UTC(file.observationStartUTC().get());
      double OBSERVATION_START_MJD(file.observationStartMJD().get());
      string OBSERVATION_END_UTC(file.observationEndUTC().get());
      double OBSERVATION_END_MJD(file.observationEndMJD().get());

      int    OBSERVATION_NOF_STATIONS(file.observationNofStations().get());
    
      vector<string>  OBSERVATION_STATIONS_LIST(file.observationStationsList().get());
      
      double OBSERVATION_FREQUENCY_MAX(file.observationFrequencyMax().get());
      double OBSERVATION_FREQUENCY_MIN(file.observationFrequencyMin().get());
      double OBSERVATION_FREQUENCY_CENTER(file.observationFrequencyCenter().get());	// conversion to Mhz
      
      string OBSERVATION_FREQUENCY_UNIT("Mhz");      
      int    OBSERVATION_NOF_BITS_PER_SAMPLE(file.observationNofBitsPerSample().get());    
      
      
      double CLOCK_FREQUENCY(file.clockFrequency().get());
      string CLOCK_FREQUENCY_UNIT("Mhz");
      string ANTENNA_SET(file.antennaSet().get());
      
      string FILTER_SELECTION(file.filterSelection().get());
      
      vector<string> TARGETS(file.targets().get());
      string TARGET(TARGETS[SAPindex]);
      
      string SYSTEM_VERSION(file.systemVersion().get());
      string PIPELINE_NAME("DYNAMIC SPECTRUM");
      string PIPELINE_VERSION("Dynspec v.2.0");
      
      string DOC_NAME("ICD 6:Dynamic-Spectrum Data");
      string DOC_VERSION("ICD6:v-2.03.10");
      
      string NOTES(file.notes().get());
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      //Variables Initialization && Additionnal Root group variables      
      
      bool DYNSPEC_GROUP(true);
      int NOF_DYNSPEC(obsNofBeam);
      int NOF_TILED_DYNSPEC(obsNofTiledDynspec);
      string CREATE_OFFLINE_ONLINE("Offline");
      
      string BF_FORMAT(file.BFFormat().get());
      string BF_VERSION(file.BFVersion().get());
      
      double PRIMARY_POINTING_DIAMETER(0);
      double POINT_RA(SAP.pointRA().get());
      double POINT_DEC(SAP.pointDEC().get());
      
      vector<string> POINT_ALTITUDE(OBSERVATION_STATIONS_LIST.size(),"none");
      vector<string> POINT_AZIMUTH(OBSERVATION_STATIONS_LIST.size(),"none");
           
      int NOF_SAMPLES(BEAM.nofSamples().get()); 			//Time samples
      
      //double SAMPLING_RATE((SAP.samplingRate().get())/1E6);
      double SAMPLING_RATE(BEAM.nofSamples().get()/file.totalIntegrationTime().get()/1E6);	
      string SAMPLING_RATE_UNIT("Mhz");
      
      double SAMPLING_TIME(1/SAMPLING_RATE);
      string SAMPLING_TIME_UNIT("μs");
      
      double TOTAL_INTEGRATION_TIME(file.totalIntegrationTime().get());
      string TOTAL_INTEGRATION_TIME_UNIT("s");
      
      

      vector<double> AXIS_VALUE_WORLD_SPECTRAL(NUM_COORD_SPECTRAL->axisValuesWorld().get());      
      int Nspectral=AXIS_VALUE_WORLD_SPECTRAL.size();
      int NOF_SUBBANDS=Nspectral/(file.observationNofBitsPerSample().get());
      
      
      int CHANNELS_PER_SUBANDS(BEAM.channelsPerSubband().get());
      
      double SUBBAND_WIDTH((BEAM.subbandWidth().get())/1E6);
      string SUBBAND_WIDTH_UNIT("Mhz");     
      
      double CHANNEL_WIDTH(SUBBAND_WIDTH/CHANNELS_PER_SUBANDS); 
      string CHANNEL_WIDTH_UNIT("Mhz");
      
      double TOTAL_BAND_WIDTH(NOF_SUBBANDS*SUBBAND_WIDTH);
      
      
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
   
      

      // Memory desallocation

      delete NUM_COORD_TIME;
      delete DIR_COORD_TIME;
      delete NUM_COORD_SPECTRAL;
      delete DIR_COORD_SPECTRAL;

      
      
    }  // end of the Reader_Root object
