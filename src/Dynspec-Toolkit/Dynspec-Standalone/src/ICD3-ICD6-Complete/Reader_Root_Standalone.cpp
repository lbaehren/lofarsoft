#include <iostream>
#include <string>

#include <sstream>

#include "Reader_Root_Standalone.h"
#include "Stock_Write_Root_Metadata_Standalone.h"

#include <dal/lofar/BF_File.h>


/// \file Reader_Root_Standalone.cpp
///  \brief File C++ (associated to Reader_Root_Standalone.h) for read ICD3 files and put in variables metadatas for the ICD6's Root Group  
///  \details  
/// <br /> Overview:
/// <br /> This function readRoot is described (programmed) here. It needs as parameter a Stock_Write_Root_Metadata_Standalone_Part object. The readRoot function will read 
///  root metadatas and stock them in the Stock_Write_Root_Metadata_Standalone_Part object as private attributes thanks to stockRootMetadata function.
///  These attributes will be written in the next step by writeRootMetadata function which is in the class 
///  Stock_Write_Root_Metadata_Standalone_Part.h  and called at the end of this Reader_Root_Standalone.cpp





using namespace dal;
using namespace std;


  Reader_Root_Standalone::Reader_Root_Standalone(){}	// constructor 
  Reader_Root_Standalone::~Reader_Root_Standalone(){}	// destructor
  
  
    
  void Reader_Root_Standalone::readRoot(string pathFile,string obsName,Stock_Write_Root_Metadata_Standalone *rootMetadata,int obsNofBeam)
    { 

/// <br /> Usage:
/// <br />   void Reader_Root_Standalone::readRoot(string pathFile,string obsName,Stock_Write_Root_Metadata_Standalone *rootMetadata,int obsNofSource,int obsNofBeam,int obsNofFrequencyBand,int obsNofTiledDynspec, string SAPname, int SAPindex)

 
/// \param pathFileICD3 file to read
/// \param obsName observation ID
/// \param *rootMetadata Stock_Write_Root_Metadata_Standalone_Part class object
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
  

      BF_SubArrayPointing SAP 		= file.subArrayPointing(0);		// generate an object called SAP contains subarray pointings

      BF_BeamGroup BEAM			= SAP.beam(0);      			// generate an object called BEAM contains Beams

      BF_StokesDataset STOCKES 		= BEAM.stokes(0);			// generate an object called STOCKES contains stockes data  

      
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
      string FILENAME("Dynspec_Standalone_"+obsName+".h5");     
      string FILEDATE("none");
      string FILETYPE("dynspec");
      string TELESCOPE("LOFAR");
      
      string PROJECT_ID("L_"+obsName);
      string PROJECT_TITLE("none");
      string PROJECT_PI("M.Serylak");
      string PROJECT_CO_I("JM.Griessmeier");   
      string PROJECT_CONTACT("N.Vilchez");
      
      string OBSERVATION_ID("L_"+obsName);
      
      
      int NOF_SAMPLES(STOCKES.nofSamples().get()); 			//Time samples
      
      double SAMPLING_TIME(BEAM.samplingTime().get()*1E6);
      string SAMPLING_TIME_UNIT("μs");
      
      double TOTAL_INTEGRATION_TIME(NOF_SAMPLES*SAMPLING_TIME/1E6);
      string TOTAL_INTEGRATION_TIME_UNIT("s");     
      
      double OBSERVATION_START_MJD(file.observationStartMJD().get());      
      double OBSERVATION_END_MJD(OBSERVATION_START_MJD+(TOTAL_INTEGRATION_TIME/86400));
         
      string OBSERVATION_START_UTC("");
      string OBSERVATION_END_UTC("");


      int    OBSERVATION_NOF_STATIONS(file.observationNofStations().get());    
      vector<string>  OBSERVATION_STATIONS_LIST(file.observationStationsList().get());
      
      double OBSERVATION_FREQUENCY_CENTER(BEAM.beamFrequencyCenter().get());
      double TOTAL_BAND_WIDTH(file.bandwidth().get());
      
      double OBSERVATION_FREQUENCY_MAX(OBSERVATION_FREQUENCY_CENTER+(TOTAL_BAND_WIDTH/2));
      double OBSERVATION_FREQUENCY_MIN(OBSERVATION_FREQUENCY_CENTER-(TOTAL_BAND_WIDTH/2));
      
      string OBSERVATION_FREQUENCY_UNIT("Mhz");      
      int    OBSERVATION_NOF_BITS_PER_SAMPLE(file.observationNofBitsPerSample().get());    
      
      
      double CLOCK_FREQUENCY(file.clockFrequency().get());
      string CLOCK_FREQUENCY_UNIT("Mhz");
      
      string ANTENNA_SET;
      string FILTER_SELECTION;
      if (OBSERVATION_FREQUENCY_CENTER < 100)
      {
	ANTENNA_SET="LBA_OUTER";FILTER_SELECTION="LBA_10_90";
      }
      else
      {
	ANTENNA_SET="HBA_OUTER";FILTER_SELECTION="HBA_110_270";	
      }
	  
      vector<string> TARGETS(BEAM.targets().get());
      string TARGET(TARGETS[0]);
      
      string SYSTEM_VERSION("unknown");
      string PIPELINE_NAME("DYNAMIC SPECTRUM");
      string PIPELINE_VERSION("Dynspec v.2.0");
      
      string DOC_NAME("ICD6: Dynamic-Spectrum Data");
      string DOC_VERSION("ICD6:v-2.03.10");
      
      string NOTES(file.notes().get());
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      //Variables Initialization && Additionnal Root group variables      
      
      bool DYNSPEC_GROUP(true);
      int NOF_DYNSPEC(obsNofBeam);
      int NOF_TILED_DYNSPEC(0);
      string CREATE_OFFLINE_ONLINE("Offline");
      
      string BF_FORMAT(file.BFFormat().get());
      string BF_VERSION(file.BFVersion().get());
      
      double PRIMARY_POINTING_DIAMETER(0);
      double POINT_RA(0);
      double POINT_DEC(0);
      
      vector<string> POINT_ALTITUDE(OBSERVATION_STATIONS_LIST.size(),"none");
      vector<string> POINT_AZIMUTH(OBSERVATION_STATIONS_LIST.size(),"none");
           

//       double SAMPLING_RATE((SAP.samplingRate().get())/1E6);
      double SAMPLING_RATE(NOF_SAMPLES/TOTAL_INTEGRATION_TIME/1E6);	
      string SAMPLING_RATE_UNIT("Mhz");
            

      vector<double> AXIS_VALUE_WORLD_SPECTRAL(NUM_COORD_SPECTRAL->axisValuesWorld().get());      
//       int Nspectral=AXIS_VALUE_WORLD_SPECTRAL.size();
      int CHANNELS_PER_SUBANDS(BEAM.channelsPerSubband().get());
//       int NOF_SUBBANDS=Nspectral/CHANNELS_PER_SUBANDS;
      
      double SUBBAND_WIDTH((BEAM.subbandWidth().get())/1E6);
      string SUBBAND_WIDTH_UNIT("Mhz");     
      
      double CHANNEL_WIDTH(SUBBAND_WIDTH/CHANNELS_PER_SUBANDS); 
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
   
      

      //Memory desallocation

      delete NUM_COORD_TIME;
      delete DIR_COORD_TIME;
      delete NUM_COORD_SPECTRAL;
      delete DIR_COORD_SPECTRAL;

      
      
    }  // end of the Reader_Root_Standalone object
