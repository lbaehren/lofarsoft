

/// \file Reader_Root_Partionned.cpp
///  \brief File C++ (associated to Reader_Root_Partionned.h) for read ICD3 files and put in variables metadatas for the ICD6's Root Group  
///  \details  
/// <br /> Overview:
/// <br /> This function readRoot is described (programmed) here. It needs as parameter a Stock_Write_Root_Metadata_Part object. The readRoot function will read 
///  root metadatas and stock them in the Stock_Write_Root_Metadata_Part object as private attributes thanks to stockRootMetadata function.
///  These attributes will be written in the next step by writeRootMetadata function which is in the class 
///  Stock_Write_Root_Metadata_Part.h  and called at the end of this Reader_Root_Partionned.cpp




#include <iostream>
#include <string>

#include <sstream>

#include "Reader_Root_Partionned.h"
#include "Stock_Write_Root_Metadata_Partionned.h"

#include <dal/lofar/BF_File.h>




     
     
using namespace dal;
using namespace std;


  Reader_Root_Partionned::Reader_Root_Partionned(){}	// constructor 
  Reader_Root_Partionned::~Reader_Root_Partionned(){}	// destructor
  

    
  void Reader_Root_Partionned::readRootPartionned(string pathFile,string obsName,Stock_Write_Root_Metadata_Partionned *rootMetadata,int obsNofBeam,int obsNofFrequencyBand,int obsNofTiledDynspec, string SAPname, int SAPindex,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin, int i, int j,int m,vector<string> listOfFiles, int obsNofStockes)

  { 
    
/// <br /> Usage:
/// <br /> void Reader_Root_Partionned::Reader_Root_Partionned(string pathFile,string obsName,Stock_Write_Root_Metadata_Part *rootMetadata,int obsNofSource,int obsNofBeam,int obsNofFrequencyBand,int obsNofTiledDynspec, string SAPname, int SAPindex,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin)

 
/// \param pathFileICD3 file to read
/// \param obsName observation ID
/// \param *rootMetadata Stock_Write_Root_Metadata_Part class object
/// \param  obsNofSource number of sources
/// \param  obsNofBeam number of beam
/// \param  obsNofFrequencyBandnumber of frequency band 
/// \param  obsNofTiledDynspecnumber of tiled dynspec
/// \param  SAPname name of SubArrayPointing
/// \param  SAPindex index of SubArrayPointing
/// \param  timeMinSelect minimum time selection 
/// \param  timeMaxSelect maximum time selection 
/// \param  timeRebin time rebinning
/// \param  frequencyMin minimum frequency selection
/// \param  frequencyMax maximum frequency selection
/// \param  frequencyRebin frequency rebinning          
   
/// \return nothing   
   
   
		
   
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Programm  Load the hdf5's root part

      BF_File file(pathFile);  							// generate an object called file contains Root
  
      BF_SubArrayPointing SAP 		= file.subArrayPointing(SAPindex);		// generate an object called SAP contains subarray pointings

      BF_BeamGroup BEAM			= SAP.beam(0);      			// generate an object called BEAM contains Beams
      SAPindex=0;
      
      CoordinatesGroup COORDS		= BEAM.coordinates();      		// generate an object called COORDS contains Coodinates directory
      Coordinate *TIME_COORDS		= COORDS.coordinate(0);			// generate an object called TIME_COORDS contains time data
      Coordinate *SPECTRAL_COORDS	= COORDS.coordinate(1);			// generate an object called SPECTRAL_COORDS contains spectral data

      NumericalCoordinate *NUM_COORD_TIME = dynamic_cast<NumericalCoordinate*>(TIME_COORDS); // load Numerical coordinates lib 
      DirectionCoordinate *DIR_COORD_TIME = dynamic_cast<DirectionCoordinate*>(TIME_COORDS); // load Direction coordinates lib
      
      NumericalCoordinate *NUM_COORD_SPECTRAL = dynamic_cast<NumericalCoordinate*>(SPECTRAL_COORDS); // load Numerical coordinates lib 
      DirectionCoordinate *DIR_COORD_SPECTRAL = dynamic_cast<DirectionCoordinate*>(SPECTRAL_COORDS); // load Direction coordinates lib
      
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Modify Time Metadata for taking account time border effects due to the time selection and rebin
      
      float timeIcrementReal((1/(BEAM.nofSamples().get()/file.totalIntegrationTime().get())));
      float timeIncrement(timeRebin);
      int timeIndexIncrementRebin(timeIncrement/timeIcrementReal);
      timeRebin = timeIndexIncrementRebin*(1/(BEAM.nofSamples().get()/file.totalIntegrationTime().get()));

      int timeIndexStart(timeMinSelect/timeIcrementReal);
      timeIndexStart = (timeIndexStart/timeIndexIncrementRebin)*timeIndexIncrementRebin;
      timeMinSelect  = timeIndexStart*timeIcrementReal;
      
      int timeIndexStop(timeMaxSelect/timeIcrementReal);
      timeIndexStop = timeIndexStop/timeIndexIncrementRebin*timeIndexIncrementRebin;
      timeMaxSelect = timeIndexStop*timeIcrementReal;      
      
      
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////      
      // Modify Spectral Metadata for taking account spectral selection and rebin
      
      // Change frequency 
      // Generate New Rebinned and selected AXIS_VALUE_WORLD_SPECTRAL
      
      vector<double> AXIS_VALUE_WORLD_SPECTRAL_TEMP(NUM_COORD_SPECTRAL->axisValuesWorld().get());      
      int AXIS_VALUE_WORLD_SPECTRAL_TEMP_SIZE(AXIS_VALUE_WORLD_SPECTRAL_TEMP.size()*obsNofFrequencyBand);  
      
      int NOF_SUBBANDS_TEMP= (NUM_COORD_SPECTRAL->axisValuesWorld().get().size()/BEAM.channelsPerSubband().get());
      int CHANNELS_PER_SUBANDS_TEMP=(BEAM.channelsPerSubband().get());
      AXIS_VALUE_WORLD_SPECTRAL_TEMP.resize(AXIS_VALUE_WORLD_SPECTRAL_TEMP_SIZE);
      
      for (int q=0;q<obsNofFrequencyBand;q++)
	    {      
		string pathFile_Partionned(listOfFiles[m*obsNofStockes*obsNofFrequencyBand+q]);

		BF_File file_Partionned(pathFile_Partionned );				 							// generate an object called file contains Root
		BF_SubArrayPointing SAP_Partionned  		= file_Partionned.subArrayPointing(i);						// generate an object called SAP contains subarray pointings
		BF_BeamGroup BEAM_Partionned			= SAP_Partionned.beam(j);      							// generate an object called BEAM contains Beams
		CoordinatesGroup COORDS_Partionned		= BEAM_Partionned.coordinates();      						// generate an object called COORDS contains Coodinates directory
		Coordinate *SPECTRAL_COORDS_Partionned		= COORDS_Partionned.coordinate(1);						// generate an object called SPECTRAL_COORDS contains spectral data
		NumericalCoordinate *NUM_COORD_SPECTRAL_Partionned = dynamic_cast<NumericalCoordinate*>(SPECTRAL_COORDS_Partionned); 	// load Numerical coordinates lib 
		
		vector<double> AXIS_VALUE_WORLD_SPECTRAL_TEMP_Partionned(NUM_COORD_SPECTRAL_Partionned->axisValuesWorld().get());
		int AXIS_VALUE_WORLD_SPECTRAL_TEMP_Partionned_SIZE(AXIS_VALUE_WORLD_SPECTRAL_TEMP_Partionned.size());
		
		
		for (int kk=0;kk<AXIS_VALUE_WORLD_SPECTRAL_TEMP_Partionned_SIZE;kk++)
		{AXIS_VALUE_WORLD_SPECTRAL_TEMP[q*NOF_SUBBANDS_TEMP*CHANNELS_PER_SUBANDS_TEMP+kk] = AXIS_VALUE_WORLD_SPECTRAL_TEMP_Partionned[kk];}
	    }      
      
      
      
      
      // Find the Number of Subband for beginning the processing (min and max subbands indexes)
      int index_fmin(0);
      float start_min(AXIS_VALUE_WORLD_SPECTRAL_TEMP[index_fmin]);    
      while (start_min < frequencyMin*1E6)
      {	  index_fmin 	= index_fmin +1;
	  start_min 	= AXIS_VALUE_WORLD_SPECTRAL_TEMP[index_fmin*CHANNELS_PER_SUBANDS_TEMP];}

	  
      int index_fmax(((AXIS_VALUE_WORLD_SPECTRAL_TEMP.size())/(BEAM.channelsPerSubband().get()))-1); 
      float start_max(AXIS_VALUE_WORLD_SPECTRAL_TEMP[AXIS_VALUE_WORLD_SPECTRAL_TEMP.size()-1]);
      while (start_max > frequencyMax*1E6)
      {	  index_fmax 	= index_fmax -1;
	  start_max 	= AXIS_VALUE_WORLD_SPECTRAL_TEMP[index_fmax*CHANNELS_PER_SUBANDS_TEMP+CHANNELS_PER_SUBANDS_TEMP-1];}	  
      
     
      
      // Avoid border effect about the frequency selection
      for (int k=0; k < NOF_SUBBANDS_TEMP*obsNofFrequencyBand;k++)
	{
	  float miniFreq(AXIS_VALUE_WORLD_SPECTRAL_TEMP[k*CHANNELS_PER_SUBANDS_TEMP]);
	  float maxiFreq(AXIS_VALUE_WORLD_SPECTRAL_TEMP[k*CHANNELS_PER_SUBANDS_TEMP+CHANNELS_PER_SUBANDS_TEMP-1]);
	  if (frequencyMin*1E6 < maxiFreq && frequencyMin*1E6 > miniFreq){index_fmin = index_fmin-1; start_min = AXIS_VALUE_WORLD_SPECTRAL_TEMP[index_fmin*CHANNELS_PER_SUBANDS_TEMP];}
	  if (frequencyMax*1E6 < maxiFreq && frequencyMax*1E6 > miniFreq){index_fmax 	= index_fmax+1;start_max = AXIS_VALUE_WORLD_SPECTRAL_TEMP[index_fmax*CHANNELS_PER_SUBANDS_TEMP+CHANNELS_PER_SUBANDS_TEMP-1];}
	}
      
  
      
      
      
      int NOF_SUBBANDS=index_fmax-index_fmin+1;
      if (NOF_SUBBANDS == 0){NOF_SUBBANDS++;}
      int Nspectral=NOF_SUBBANDS*frequencyRebin;     
  
      
      
      // Determine the new Pixel number
      vector<double> AXIS_VALUE_WORLD_SPECTRAL(Nspectral);
      vector<double> AXIS_VALUE_PIXEL_SPECTRAL(Nspectral);
      // Generate the New selected and rebinned AXIS_VALUE_WORLD_SPECTRAL
      float sumSpecRebin(0);    
      for (int i=0; i<Nspectral;i++)
	{
	    AXIS_VALUE_PIXEL_SPECTRAL[i]=i;
	    sumSpecRebin=0;
	    for (int j=0;j<CHANNELS_PER_SUBANDS_TEMP/frequencyRebin;j++){sumSpecRebin = sumSpecRebin+AXIS_VALUE_WORLD_SPECTRAL_TEMP[i*(CHANNELS_PER_SUBANDS_TEMP/frequencyRebin)+index_fmin*CHANNELS_PER_SUBANDS_TEMP+j ];}
	    AXIS_VALUE_WORLD_SPECTRAL[i]=sumSpecRebin/CHANNELS_PER_SUBANDS_TEMP*frequencyRebin;
	}           
            
      
      double OBSERVATION_FREQUENCY_MAX (file.observationFrequencyMax().get());
      double OBSERVATION_FREQUENCY_MIN(file.observationFrequencyMin().get());
      double OBSERVATION_FREQUENCY_CENTER(file.observationFrequencyCenter().get());      
      
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // ROOT
      ///////////////////////////////////////////////////////////      
      
      // Normal root metadata
      
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
      
      
      std::ostringstream oss_timeNewStart;oss_timeNewStart << timeMinSelect;string timeNewStart(oss_timeNewStart.str());
      std::ostringstream oss_timeNewEnd;oss_timeNewEnd << timeMaxSelect;string timeNewEnd(oss_timeNewEnd.str());
              
      string OBSERVATION_START_UTC(file.observationStartUTC().get()+" + "+timeNewStart+" s");
      double OBSERVATION_START_MJD(file.observationStartMJD().get()+(timeMinSelect/86400));
      string OBSERVATION_END_UTC(file.observationEndUTC().get()+" - "+timeNewEnd+" s");
      double OBSERVATION_END_MJD(file.observationEndMJD().get()-(timeMaxSelect/86400));

      int    OBSERVATION_NOF_STATIONS(file.observationNofStations().get());
    
      vector<string>  OBSERVATION_STATIONS_LIST(file.observationStationsList().get());
      
      
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
           
      int NOF_SAMPLES((timeMaxSelect-timeMinSelect)/timeRebin); 			//Time samples
      
      double SAMPLING_RATE(1/timeRebin/1E6);	
      string SAMPLING_RATE_UNIT("Mhz");
            
      double SAMPLING_TIME(1/SAMPLING_RATE);
      string SAMPLING_TIME_UNIT("μs");
      
      double TOTAL_INTEGRATION_TIME(timeMaxSelect-timeMinSelect);
      string TOTAL_INTEGRATION_TIME_UNIT("s");
     
      int CHANNELS_PER_SUBANDS(frequencyRebin);
      
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
  
      
      rootMetadata->stockRootMetadataPartionned(GROUPETYPE,FILENAME,FILEDATE,FILETYPE,TELESCOPE,PROJECT_ID,PROJECT_TITLE,PROJECT_PI,PROJECT_CO_I,   // attr10
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
