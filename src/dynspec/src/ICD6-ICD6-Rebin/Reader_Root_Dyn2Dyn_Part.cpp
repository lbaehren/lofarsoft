#include <iostream>
#include <string>

#include <sstream>

#include "Reader_Root_Dyn2Dyn_Part.h"
#include "Stock_Write_Root_Dyn2Dyn_Metadata_Part.h"

#include <dal/lofar/BF_File.h>

/// \file Reader_Root_Dyn2Dyn_Part.cpp
///  \brief File C++ (associated to Reader_Root_Dyn2Dyn_Part.h) for read ICD3 files and put in variables metadatas for the ICD6's Root Group  
///  \details  
/// <br /> Overview:
/// <br /> This function readRoot is described (programmed) here. It needs as parameter a Stock_Write_Root_Dyn2Dyn_Metadata_Part object. The readRoot function will read 
///  root metadatas and stock them in the Stock_Write_Root_Dyn2Dyn_Metadata_Part object as private attributes thanks to stockRootMetadata function.
///  These attributes will be written in the next step by writeRootMetadata function which is in the class 
///  Stock_Write_Root_Dyn2Dyn_Metadata_Part.h  and called at the end of this Reader_Root_Part.cpp


using namespace dal;
using namespace std;


  Reader_Root_Dyn2Dyn_Part::Reader_Root_Dyn2Dyn_Part(){}	// constructor 
  Reader_Root_Dyn2Dyn_Part::~Reader_Root_Dyn2Dyn_Part(){}	// destructor
  
  
    
  void Reader_Root_Dyn2Dyn_Part::readRoot(string pathFile,string obsName,Stock_Write_Root_Dyn2Dyn_Metadata_Part *rootMetadata,int obsNofBeam,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin)

  { 
   
/// <br /> Usage:
/// <br />   void Reader_Root_Dyn2Dyn_Part::readRoot(string pathFile,string obsName,Stock_Write_Root_Dyn2Dyn_Metadata_Part *rootMetadata,int obsNofBeam,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin)
 
/// \param pathFile file (dynamic spectrum) to read
/// \param obsName observation ID
/// \param *rootMetadata Stock_Write_Root_Dyn2Dyn_Metadata_Part class object
/// \param  obsNofBeam number of beam
/// \param  timeMinSelect minimum time selection 
/// \param  timeMaxSelect maximum time selection 
/// \param  timeRebin time rebinning
/// \param  frequencyMin minimum frequency selection
/// \param  frequencyMax maximum frequency selection
/// \param  frequencyRebin frequency rebinning          
   
/// \return nothing       
    
    
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Programm  Load the hdf5's root part

      File file(pathFile);  					// generate an object called file contains Root
      Group Dynspec(file, "DYNSPEC_000");			// generate an object called Dynspec contains 1 Dynspec

      Group COORDS(Dynspec,"COORDINATES");			// Generate an Objet called COORDS contains Coords group 
      
      Group COORDS_POLARIZATION(COORDS,"POLARIZATION");	 
      Group COORDS_SPECTRAL(COORDS,"SPECTRAL");
      Group COORDS_TIME(COORDS,"TIME");
      
      Dataset<float> STOCKES(Dynspec, "DATA");			// generate an object called STOCKES contains stockes data
      
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Modify Time Metadata for taking account time border effects due to the time selection and rebin

      Attribute<float> Obj_nofSample(file, "NOF_SAMPLES");
      float nofSample(Obj_nofSample.value);
      
      Attribute<float> Obj_totalIntegrationTime(file, "TOTAL_INTEGRATION_TIME");
      float totalIntegrationTime(Obj_totalIntegrationTime.value);
      
      float timeIcrementReal(1/(nofSample/totalIntegrationTime));
      float timeIncrement(timeRebin);
      int timeIndexIncrementRebin(timeIncrement/timeIcrementReal);
      timeRebin = timeIndexIncrementRebin*(1/(nofSample/totalIntegrationTime));

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
      
      Attribute< vector<double> > Obj_AXIS_VALUE_WORLD_SPECTRAL_TEMP(COORDS_SPECTRAL, "AXIS_VALUE_WORLD");
      Attribute<int> Obj_CHANNELS_PER_SUBANDS_TEMP(file, "CHANNELS_PER_SUBANDS");
      
      vector<double> AXIS_VALUE_WORLD_SPECTRAL_TEMP(Obj_AXIS_VALUE_WORLD_SPECTRAL_TEMP.value);
      int CHANNELS_PER_SUBANDS_TEMP(Obj_CHANNELS_PER_SUBANDS_TEMP.value);
      int NOF_SUBBANDS_TEMP=AXIS_VALUE_WORLD_SPECTRAL_TEMP.size()/CHANNELS_PER_SUBANDS_TEMP;
      
      
      // Find the Number of Subband for beginning the processing (min and max subbands indexes)
      int index_fmin(0);
      float start_min(AXIS_VALUE_WORLD_SPECTRAL_TEMP[index_fmin]);    
      while (start_min < frequencyMin*1E6)
      {	  index_fmin 	= index_fmin +1;
	  start_min 	= AXIS_VALUE_WORLD_SPECTRAL_TEMP[index_fmin*CHANNELS_PER_SUBANDS_TEMP];}

	  
      int index_fmax(NOF_SUBBANDS_TEMP-1); 
      float start_max(AXIS_VALUE_WORLD_SPECTRAL_TEMP[AXIS_VALUE_WORLD_SPECTRAL_TEMP.size()-1]);
      while (start_max > frequencyMax*1E6)
      {	  index_fmax 	= index_fmax -1;
	  start_max 	= AXIS_VALUE_WORLD_SPECTRAL_TEMP[index_fmax*CHANNELS_PER_SUBANDS_TEMP+CHANNELS_PER_SUBANDS_TEMP-1];}	  
      
      
      // Avoid border effect about the frequency selection
      for (int k=0; k < NOF_SUBBANDS_TEMP;k++)
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
      
      	  
      double OBSERVATION_FREQUENCY_MAX((AXIS_VALUE_WORLD_SPECTRAL[(AXIS_VALUE_WORLD_SPECTRAL.size())-1])/1E6);
      double OBSERVATION_FREQUENCY_MIN((AXIS_VALUE_WORLD_SPECTRAL[0])/1E6);
      double OBSERVATION_FREQUENCY_CENTER((OBSERVATION_FREQUENCY_MAX+OBSERVATION_FREQUENCY_MIN)/2.0);	// conversion to Mhz
        
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // ROOT
      ///////////////////////////////////////////////////////////      
      
      // Normal root metadata
      
      //Variables Initialization && General Root group variables 

      Attribute<string> obj_GROUPETYPE(file, "GROUPETYPE");
      Attribute<string> obj_FILENAME(file, "FILENAME");
      Attribute<string> obj_FILEDATE(file, "FILEDATE");
      Attribute<string> obj_FILETYPE(file, "FILETYPE");
      Attribute<string> obj_TELESCOPE(file, "TELESCOPE");   
      Attribute<string> obj_PROJECT_ID(file,  "PROJECT_ID");
      Attribute<string> obj_PROJECT_TITLE(file,"PROJECT_TITLE");
      Attribute<string> obj_PROJECT_PI(file, "PROJECT_PI");
      Attribute<string> obj_PROJECT_CO_I(file,"PROJECT_CO_I");
      Attribute<string> obj_PROJECT_CONTACT(file,"PROJECT_CONTACT");      
      Attribute<string> obj_OBSERVATION_ID(file, "OBSERVATION_ID");
      
      string GROUPETYPE(obj_GROUPETYPE.value);
      string FILENAME(obj_FILENAME.value);
      string FILEDATE(obj_FILEDATE.value);
      string FILETYPE(obj_FILETYPE.value);
      string TELESCOPE(obj_TELESCOPE.value);      
      string PROJECT_ID(obj_PROJECT_ID.value);
      string PROJECT_TITLE(obj_PROJECT_TITLE.value);
      string PROJECT_PI(obj_PROJECT_PI.value);
      string PROJECT_CO_I(obj_PROJECT_CO_I.value);
      string PROJECT_CONTACT(obj_PROJECT_CONTACT.value);     
      string OBSERVATION_ID(obj_OBSERVATION_ID.value);
      

      
      std::ostringstream oss_timeNewStart;oss_timeNewStart << timeMinSelect;string timeNewStart(oss_timeNewStart.str());
      std::ostringstream oss_timeNewEnd;oss_timeNewEnd << timeMaxSelect;string timeNewEnd(oss_timeNewEnd.str());


      Attribute<string> obj_OBSERVATION_START_UTC(file, "OBSERVATION_START_UTC");
      Attribute<double> obj_OBSERVATION_START_MJD(file, "OBSERVATION_START_MJD");
      Attribute<string> obj_OBSERVATION_END_UTC(file, "OBSERVATION_END_UTC");
      Attribute<double> obj_OBSERVATION_END_MJD(file, "OBSERVATION_END_MJD");     
              
      string OBSERVATION_START_UTC(obj_OBSERVATION_START_UTC.value);
      OBSERVATION_START_UTC = OBSERVATION_START_UTC+" + "+timeNewStart+" s";
      double OBSERVATION_START_MJD(obj_OBSERVATION_START_MJD.value+(timeMinSelect/86400));
      string OBSERVATION_END_UTC(obj_OBSERVATION_END_UTC.value);
      OBSERVATION_END_UTC = OBSERVATION_END_UTC+" - "+timeNewEnd+" s";
      double OBSERVATION_END_MJD(obj_OBSERVATION_END_MJD.value-(timeMaxSelect/86400));
      

      Attribute<int> obj_OBSERVATION_NOF_STATIONS(file, "OBSERVATION_NOF_STATIONS");
      Attribute< vector<string> > obj_OBSERVATION_STATIONS_LIST(file, "OBSERVATION_STATIONS_LIST");
      Attribute<int> obj_OBSERVATION_NOF_BITS_PER_SAMPLE(file, "OBSERVATION_NOF_BITS_PER_SAMPLE");

      int    OBSERVATION_NOF_STATIONS(obj_OBSERVATION_NOF_STATIONS.value);
      vector<string>  OBSERVATION_STATIONS_LIST(obj_OBSERVATION_STATIONS_LIST.value);
      int    OBSERVATION_NOF_BITS_PER_SAMPLE(obj_OBSERVATION_NOF_BITS_PER_SAMPLE.value);    
      string OBSERVATION_FREQUENCY_UNIT("Mhz"); 
      
      
      Attribute<double> obj_CLOCK_FREQUENCY(file, "CLOCK_FREQUENCY");
      Attribute<string> obj_ANTENNA_SET(file, "ANTENNA_SET");
      Attribute<string> obj_FILTER_SELECTION(file, "FILTER_SELECTION");
      Attribute<string> obj_TARGET(file, "TARGET");
      Attribute<string> obj_SYSTEM_VERSION(file, "SYSTEM_VERSION");
      Attribute<string> obj_NOTES(file, "NOTES");

      double CLOCK_FREQUENCY(obj_CLOCK_FREQUENCY.value);     
      string ANTENNA_SET(obj_ANTENNA_SET.value);      
      string FILTER_SELECTION(obj_FILTER_SELECTION.value);     
      string TARGET(obj_TARGET.value);         
      string SYSTEM_VERSION(obj_SYSTEM_VERSION.value);
      string NOTES(obj_NOTES.value);
      
      
      string CLOCK_FREQUENCY_UNIT("Mhz");
      string PIPELINE_NAME("DYNAMIC SPECTRUM");
      string PIPELINE_VERSION("Dynspec v.2.0");     
      string DOC_NAME("ICD 6:Dynamic-Spectrum Data");
      string DOC_VERSION("ICD6:v-2.03.10");
      
      
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      //Variables Initialization && Additionnal Root group variables      

      Attribute<string> obj_BF_FORMAT(file, "BF_FORMAT");
      Attribute<string> obj_BF_VERSION(file, "BF_VERSION");
      Attribute<double> obj_POINT_RA(file, "POINT_RA");
      Attribute<double> obj_POINT_DEC(file, "POINT_DEC");
      Attribute<double> obj_SUBBAND_WIDTH(file, "SUBBAND_WIDTH");

      string BF_FORMAT(obj_BF_FORMAT.value);
      string BF_VERSION(obj_BF_VERSION.value);
      double POINT_RA(obj_POINT_RA.value);
      double POINT_DEC(obj_POINT_DEC.value);
      double SUBBAND_WIDTH(obj_SUBBAND_WIDTH.value);      
           
      
      bool DYNSPEC_GROUP(true);
      int NOF_DYNSPEC(obsNofBeam);     
      int NOF_TILED_DYNSPEC(0);
      string CREATE_OFFLINE_ONLINE("Offline");
      double PRIMARY_POINTING_DIAMETER(0);
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
   
   
    }  // end of the Reader_Root object