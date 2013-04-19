#include <iostream>
#include <string>
#include <fstream>


#include "Reader_Dynspec_Part_Standalone.h"
#include "Stock_Write_Dynspec_Metadata_Part_Standalone.h"
#include "Stock_Write_Dynspec_Data_Part_Standalone.h"

#include <dal/lofar/BF_File.h>

/// \file Reader_Dynspec_Part_Standalone.cpp
///  \brief File C++ (associated to Reader_Dynspec_Part_Standalone.h)for read ICD3 files and put in variables metadatas for the ICD6's Dynspec Group  
///  \details  
/// <br /> Overview:
/// <br /> This function readDynspec is described (programmed) here. It needs as parameter a Stock_Write_Dynspec_Metadata_Part_Standalone object. The readDynspec function will read 
///  dynspec metadatas and stock them in the Stock_Write_Dynspec_Metadata_Part_Standalone object as private attributes thanks to stockDynspecMetadata function.
///  These attributes will be written in the next step by writeDynspecMetadata function which is in the class 
///  Stock_Write_Dynspec_Metadata_Part_Standalone.h  and called at the end of this Reader_Dynspec_Part_Standalone.cpp



using namespace dal;
using namespace std;


  Reader_Dynspec_Part_Standalone::Reader_Dynspec_Part_Standalone(){}	// constructor 
  Reader_Dynspec_Part_Standalone::~Reader_Dynspec_Part_Standalone(){}	// destructor
  
  
   
  void Reader_Dynspec_Part_Standalone::readDynspec(string pathFile,Stock_Write_Dynspec_Metadata_Part_Standalone *dynspecMetadata,Stock_Write_Dynspec_Data_Part_Standalone *dynspecData,int i,int j,int obsNofSAP,int obsNofStockes,vector<string> stokesComponent,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin)
    {        
       
  
/// <br /> Usage:
/// <br />   void Reader_Dynspec_Part_Standalone::readDynspec(string pathFile,Stock_Write_Dynspec_Metadata_Part_Standalone *dynspecMetadata,Stock_Write_Dynspec_Data_Part_Standalone *dynspecData,int i,int j,int q,int obsNofSAP,int obsNofSource,int obsNofFrequencyBand,int obsNofStockes,vector<string> stokesComponent, int SAPindex,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin)
 
/// \param  pathFile  ICD3 file to read 
/// \param  *dynspecMetadata Reader_Dynspec_Part_Standalone Object for loading dynspec metadata in private attributes
/// \param  *dynspecData Stock_Write_Dynspec_Data_Part_Standalone Object for loading new rebinning for data matrix processing 
/// \param  i loop index on SAP
/// \param  j loop index on Beams
/// \param  q loop index on Pxxx if this nomenclature is used
/// \param  obsNofSAP number of SAP
/// \param  obsNofSource number of Sources
/// \param  obsNofFrequencyBand number of Frequency Band
/// \param  obsNofStocke number of Stokes parameter
/// \param  stokesComponent vector with Stokes component 
/// \param  SAPindex Index of the SAP in current processing 
/// \param  timeMinSelect minimum time selection 
/// \param  timeMaxSelect maximum time selection 
/// \param  timeRebin time rebinning
/// \param  frequencyMin minimum frequency selection
/// \param  frequencyMax maximum frequency selection
/// \param  frequencyRebin frequency rebinning    
   
/// \return nothing        
      

      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Programm  Load the hdf5's root part
      
   
      BF_File file(pathFile);				 		// generate an object called file contains Root
      
      
      BF_SubArrayPointing SAP 		= file.subArrayPointing(i);		// generate an object called SAP contains subarray pointings
      BF_BeamGroup BEAM			= SAP.beam(j);      			// generate an object called BEAM contains Beams

      BF_StokesDataset STOCKES 		= BEAM.stokes(0);			// generate an object called STOCKES contains stockes data  
      
      
      CoordinatesGroup COORDS		= BEAM.coordinates();      		// generate an object called COORDS contains Coodinates directory
      Coordinate *TIME_COORDS		= COORDS.coordinate(0);			// generate an object called TIME_COORDS contains time data
      Coordinate *SPECTRAL_COORDS	= COORDS.coordinate(1);			// generate an object called SPECTRAL_COORDS contains spectral data

      NumericalCoordinate *NUM_COORD_TIME = dynamic_cast<NumericalCoordinate*>(TIME_COORDS); // load Numerical coordinates lib 
      DirectionCoordinate *DIR_COORD_TIME = dynamic_cast<DirectionCoordinate*>(TIME_COORDS); // load Direction coordinates lib
      
      NumericalCoordinate *NUM_COORD_SPECTRAL = dynamic_cast<NumericalCoordinate*>(SPECTRAL_COORDS); // load Numerical coordinates lib 
      DirectionCoordinate *DIR_COORD_SPECTRAL = dynamic_cast<DirectionCoordinate*>(SPECTRAL_COORDS); // load Direction coordinates lib
      

      // Redefine ObsnofStokes and vector for special case 
      obsNofStockes = 4;       
      
      
      // Preliminary needs
      int NOF_SAMPLES(STOCKES.nofSamples().get()); 			//Time samples
      
      double SAMPLING_TIME(BEAM.samplingTime().get()*1E6);
      string SAMPLING_TIME_UNIT("μs");

      double TOTAL_INTEGRATION_TIME(NOF_SAMPLES*SAMPLING_TIME/1E6);
      string TOTAL_INTEGRATION_TIME_UNIT("s");           
   
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Modify Time Metadata for taking account time border effects due to the time selection and rebin
      float timeMinSelectTemp =timeMinSelect;
      float timeMaxSelectTemp = timeMaxSelect;
      
      
      float timeIcrementReal((1/(STOCKES.nofSamples().get()/TOTAL_INTEGRATION_TIME)));
      float timeIncrement(timeRebin);
      int timeIndexIncrementRebin(timeIncrement/timeIcrementReal);
      timeRebin = timeIndexIncrementRebin*(1/(STOCKES.nofSamples().get()/TOTAL_INTEGRATION_TIME));

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
      int NOF_SUBBANDS_TEMP=(AXIS_VALUE_WORLD_SPECTRAL_TEMP.size())/(BEAM.channelsPerSubband().get());
      int CHANNELS_PER_SUBANDS_TEMP=(BEAM.channelsPerSubband().get());
      
      
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
        
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////      
      // Variable iniatialization for DYNSPEC group 
      
      string GROUPE_TYPE_DYN("DYNSPEC");
      
      // function file.expTimeStartMJD().get()) return a double value, so it needs to convert float to string using a ostringstream
      std::ostringstream oss;oss << file.observationStartMJD().get()+(timeMinSelect/86400);string DYNSPEC_START_MJD(oss.str());
      oss << file.observationStartMJD().get()+(timeMinSelect/86400)+TOTAL_INTEGRATION_TIME;string DYNSPEC_STOP_MJD(oss.str());
         
      string DYNSPEC_START_UTC("");
      string DYNSPEC_STOP_UTC("");
      
      // TARGET_DYN
      string TRACKING("J2000");  
      vector<string> TARGETS_DYN(BEAM.targets().get());
      string TARGET_DYN(TARGETS_DYN[0]);
      
      string ONOFF;
      if (TRACKING == "J2000"){ONOFF = "ON";}
      if (TRACKING =="LMN"){ONOFF = "OFF_0";}     
      if (TRACKING =="TBD"){ONOFF = "OFF_1";}
      
      double POINT_RA(0);
      double POINT_DEC(0);
      
      double POSITION_OFFSET_RA(0);
      double POSITION_OFFSET_DEC(0);
      
      double BEAM_DIAMETER(0); 
      double BEAM_DIAMETER_RA(0);
      double BEAM_DIAMETER_DEC(0);

 
      double BEAM_NOF_STATIONS(file.observationNofStations().get());
      vector<string> BEAM_STATIONS_LIST(file.observationStationsList().get());
  
      string DEDISPERSION("none");
      double DISPERSION_MEASURE(0);
      string DISPERSION_MEASURE_UNIT("pc/cm^3");
  
      
      bool BARYCENTER(false);
      vector<string> STOCKES_COMPONENT(4);
      bool COMPLEX_VOLTAGE(false);
      string SIGNAL_SUM("COHERENT");
      
      
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Variable iniatialization for COORDINATES group    
      
      string GROUPE_TYPE_COORD("Coordinates");
      vector<double> REF_LOCATION_VALUE(3);REF_LOCATION_VALUE[0]=0;REF_LOCATION_VALUE[1]=0;REF_LOCATION_VALUE[2]=0;
      vector<string> REF_LOCATION_UNIT(3);REF_LOCATION_UNIT[0]="m";REF_LOCATION_UNIT[1]="m";REF_LOCATION_UNIT[2]="m";
      string REF_LOCATION_FRAME("ITRF");
      
      double REF_TIME_VALUE(file.observationStartMJD().get()+(timeMinSelect/86400.0));
      string REF_TIME_UNIT("d");
      string REF_TIME_FRAME("MJD");
      double NOF_COORDINATES(3);
      double NOF_AXIS(3);
      vector<string> COORDINATE_TYPES(3);
      COORDINATE_TYPES[0]="Time";
      COORDINATE_TYPES[1]="Spectral";
      COORDINATE_TYPES[2]="Polarisation";


      
      ///////////////////////////////////////////////////////////
      // Variable iniatialization for TIME coordinate  group        
      
      
      string GROUPE_TYPE_TIME("TimeCoord");
      string COORDINATE_TYPE_TIME("Time");
      vector<string> STORAGE_TYPE_TIME(1); STORAGE_TYPE_TIME[0]="Linear";     
      int NOF_AXES_TIME(TIME_COORDS->nofAxes().get());      
      vector<string> AXIS_NAMES_TIME(TIME_COORDS->axisNames().get());
      vector<string> AXIS_UNIT_TIME(1);AXIS_UNIT_TIME[0]="s"; 
      
      vector<double> REFERENCE_VALUE_TIME(1);REFERENCE_VALUE_TIME[0]=(NUM_COORD_TIME->referenceValue().get());
      vector<double> REFERENCE_PIXEL_TIME(1);REFERENCE_PIXEL_TIME[0]=(NUM_COORD_TIME->referencePixel().get());
          
      vector<double> INCREMENT_TIME(1);INCREMENT_TIME[0]=timeRebin;
            
      vector<double> PC_TIME(2);PC_TIME[0]=1;PC_TIME[1]=0;
            
      int Ntime=((timeMaxSelect-timeMinSelect)/timeRebin);
                

      
     
     
      ///////////////////////////////////////////////////////////
      // Variable iniatialization for SPECTRAL coordinate  group 
      // SEE AT THE BEGINNING OF DYNSPEC PART (line 
      
      string GROUPE_TYPE_SPECTRAL("SpectralCoord");
      string COORDINATE_TYPE_SPECTRAL("Spectral");      
      vector<string> STORAGE_TYPE_SPECTRAL(SPECTRAL_COORDS->storageType().get());    
      int NOF_AXES_SPECTRAL(SPECTRAL_COORDS->nofAxes().get());      
      vector<string> AXIS_NAMES_SPECTRAL(SPECTRAL_COORDS->axisNames().get());
      
      vector<string> AXIS_UNIT_SPECTRAL(1);AXIS_UNIT_SPECTRAL[0]="Mhz"; 
      
      vector<double> REFERENCE_VALUE_SPECTRAL(1);REFERENCE_VALUE_SPECTRAL[0]=0;
      vector<double> REFERENCE_PIXEL_SPECTRAL(1);REFERENCE_PIXEL_SPECTRAL[0]=0;
      vector<double> INCREMENT_SPECTRAL(1);INCREMENT_SPECTRAL[0]=0;          
            
      vector<double> PC_SPECTRAL(2);PC_SPECTRAL[0]=1;PC_SPECTRAL[1]=0;
      
      
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
	    AXIS_VALUE_WORLD_SPECTRAL[i]=(sumSpecRebin/CHANNELS_PER_SUBANDS_TEMP*frequencyRebin);
	}     

      
      ///////////////////////////////////////////////////////////
      // FREQUENCY INFO      

      
      double BEAM_FREQUENCY_MAX((AXIS_VALUE_WORLD_SPECTRAL[(AXIS_VALUE_WORLD_SPECTRAL.size())-1])/1E6);
      double BEAM_FREQUENCY_MIN((AXIS_VALUE_WORLD_SPECTRAL[0])/1E6);   
      double BEAM_FREQUENCY_CENTER((BEAM_FREQUENCY_MAX+BEAM_FREQUENCY_MIN)/2.0);          
      
      double SUBBAND_WIDTH((BEAM.subbandWidth().get())/1E6);  
      string BEAM_FREQUENCY_UNIT("Mhz");
      double DYNSPEC_BANDWIDTH(NOF_SUBBANDS*SUBBAND_WIDTH);
      
         
      ///////////////////////////////////////////////////////////
      // Variable iniatialization for POLARISATION coordinate  group     

      string GROUPE_TYPE_POL("PolarizationCoord");
      string COORDINATE_TYPE_POL("Polarization");      
      vector<string> STORAGE_TYPE_POL(1);STORAGE_TYPE_POL[0]="Tabular";    
      int NOF_AXES_POL(obsNofStockes);      
      vector<string> AXIS_NAMES_POL(stokesComponent);   
      vector<string> AXIS_UNIT_POL(obsNofStockes);for (int o=0;o<obsNofStockes;o++){AXIS_UNIT_POL[o]="none";} 
      
      ///////////////////////////////////////////////////////////
      // META-DATA for Event Group        
      
      string GROUPE_TYPE_EVENT("Event");
      string DATASET_EVENT("Event List");
      int N_AXIS_EVENT(2);
      string N_AXIS_1_EVENT("Fields");
      string N_AXIS_2_EVENT("Event");
      int N_EVENT(1);
      vector<string> FIELD_1(N_EVENT);FIELD_1[0]="none";
      vector<string> FIELD_2(N_EVENT);FIELD_2[0]="none";
      vector<string> FIELD_3(N_EVENT);FIELD_3[0]="none";   
      vector<string> FIELD_4(N_EVENT);FIELD_4[0]="none";
      vector<string> FIELD_5(N_EVENT);FIELD_5[0]="none";
      vector<string> FIELD_6(N_EVENT);FIELD_6[0]="none";
 
      
      ///////////////////////////////////////////////////////////
      // META-DATA for Process HISTORY GROUP
      
      string GROUPE_TYPE_PROCESS("ProcessHistory");    
      bool OBSERVATION_PARSET(0);
      bool OBSERVATION_LOG(0);
      bool DYNSPEC_PARSET(0);
      bool DYNSPEC_LOG(0);
      
      
      std::ostringstream oss_timeMinTemp;oss_timeMinTemp << timeMinSelectTemp;string timeMinTemp(oss_timeMinTemp.str()); 
      std::ostringstream oss_timeMaxTemp;oss_timeMaxTemp << timeMaxSelectTemp;string timeMaxTemp(oss_timeMaxTemp.str());          
      std::ostringstream oss_timeMin;oss_timeMin << timeMinSelect;string timeMin(oss_timeMin.str()); 
      std::ostringstream oss_timeMax;oss_timeMax << timeMaxSelect;string timeMax(oss_timeMax.str());      
      std::ostringstream oss_timeRebinning;oss_timeRebinning << timeRebin;string timeRebinning(oss_timeRebinning.str());      
      std::ostringstream oss_spectralMin;oss_spectralMin << frequencyMin;string spectralMin(oss_spectralMin.str());
      std::ostringstream oss_spectralMax;oss_spectralMax << frequencyMax;string spectralMax(oss_spectralMax.str());      
      std::ostringstream oss_spectralMinExtracted;oss_spectralMinExtracted << start_min/1E6;string spectralMinExtracted(oss_spectralMinExtracted.str());
      std::ostringstream oss_spectralMaxExtracted;oss_spectralMaxExtracted << start_max/1E6;string spectralMaxExtracted(oss_spectralMaxExtracted.str());
      std::ostringstream oss_spectralRebinning;oss_spectralRebinning << frequencyRebin;string spectralRebinning(oss_spectralRebinning.str());
      
      string PREPROCCESSINGINFO1("Pre-processed: Time selection Requested: "+timeMinTemp+"-"+timeMaxTemp +" s");
      string PREPROCCESSINGINFO1b("Pre-processed: Time selection Extracted: "+timeMin+"-"+timeMax +" s");
      string PREPROCCESSINGINFO2("Pre-processed: Time rebinning: "+timeRebinning+" s/pixel");
      string PREPROCCESSINGINFO3("Pre-processed: Frequency selection Requested: "+spectralMin+"-"+ spectralMax+" MHz");
      string PREPROCCESSINGINFO4("Pre-processed: Frequency selection Extracted: "+spectralMinExtracted+"-"+ spectralMaxExtracted+" MHz");
      string PREPROCCESSINGINFO5("Pre-processed: Frequency rebinning: "+spectralRebinning+" Channels/Subbands");

      
      // Time & spectral real dimensions before rebinning
      
      int NtimeReal = (STOCKES.nofSamples().get());
      int NspectralReal = AXIS_VALUE_WORLD_SPECTRAL_TEMP.size();
      
      
      ///////////////////////////////////////////////////////////////////////////////////////       
				  // END READING DYNSPEC META-DATA  //
      //////////////////////////////////////////////////////////////////////////////////////         
      
      //Object generation => STOCK DYNSPEC METADATA

      dynspecMetadata->stockDynspecMetadata(GROUPE_TYPE_DYN,DYNSPEC_START_MJD,DYNSPEC_STOP_MJD,DYNSPEC_START_UTC,DYNSPEC_STOP_UTC,DYNSPEC_BANDWIDTH,BEAM_DIAMETER,TRACKING,TARGET_DYN,  //attr_79
					   ONOFF,POINT_RA,POINT_DEC,POSITION_OFFSET_RA,POSITION_OFFSET_DEC,BEAM_DIAMETER_RA,BEAM_DIAMETER_DEC,BEAM_FREQUENCY_MAX,BEAM_FREQUENCY_MIN,BEAM_FREQUENCY_CENTER,  //attr_89
					   BEAM_FREQUENCY_UNIT,BEAM_NOF_STATIONS,BEAM_STATIONS_LIST,DEDISPERSION,DISPERSION_MEASURE,DISPERSION_MEASURE_UNIT,BARYCENTER,STOCKES_COMPONENT,COMPLEX_VOLTAGE,SIGNAL_SUM,  // attr_99
					   GROUPE_TYPE_COORD,REF_LOCATION_VALUE,REF_LOCATION_UNIT,REF_LOCATION_FRAME,REF_TIME_VALUE,REF_TIME_UNIT,REF_TIME_FRAME,NOF_COORDINATES,NOF_AXIS,COORDINATE_TYPES,  // attr_109
					   GROUPE_TYPE_TIME,COORDINATE_TYPE_TIME,STORAGE_TYPE_TIME, NOF_AXES_TIME,AXIS_NAMES_TIME,AXIS_UNIT_TIME,REFERENCE_VALUE_TIME,REFERENCE_PIXEL_TIME,INCREMENT_TIME,  // attr_118
					   PC_TIME,GROUPE_TYPE_SPECTRAL,COORDINATE_TYPE_SPECTRAL,STORAGE_TYPE_SPECTRAL,NOF_AXES_SPECTRAL,AXIS_NAMES_SPECTRAL,  // attr_127
					   AXIS_UNIT_SPECTRAL,REFERENCE_VALUE_SPECTRAL,REFERENCE_PIXEL_SPECTRAL,INCREMENT_SPECTRAL,PC_SPECTRAL,AXIS_VALUE_WORLD_SPECTRAL,AXIS_VALUE_PIXEL_SPECTRAL,  // attr_135
					   GROUPE_TYPE_POL,COORDINATE_TYPE_POL,STORAGE_TYPE_POL,NOF_AXES_POL,AXIS_NAMES_POL,AXIS_UNIT_POL,Ntime,Nspectral,GROUPE_TYPE_EVENT,DATASET_EVENT,N_AXIS_EVENT,N_AXIS_1_EVENT,  // attr_150
					   N_AXIS_2_EVENT,N_EVENT,FIELD_1,FIELD_2,FIELD_3,FIELD_4,FIELD_5,FIELD_6,GROUPE_TYPE_PROCESS,OBSERVATION_PARSET,OBSERVATION_LOG,DYNSPEC_PARSET,DYNSPEC_LOG,  // attr_163
					   PREPROCCESSINGINFO1,PREPROCCESSINGINFO2,PREPROCCESSINGINFO3,PREPROCCESSINGINFO4,PREPROCCESSINGINFO5); // attr_168
					   
      dynspecData->stockDynspecData(Ntime,Nspectral,NtimeReal,NspectralReal);
      
      
      ///////////////////////////////////////////////      
      // Memory desallocation

      delete NUM_COORD_TIME;
      delete DIR_COORD_TIME;
      delete NUM_COORD_SPECTRAL;
      delete DIR_COORD_SPECTRAL;     

    }  // end of the Reader_Dynspec object 

      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
