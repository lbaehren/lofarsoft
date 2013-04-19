#include <iostream>
#include <string>


#include "Reader_Dyn2Dyn_Part.h"
#include "Stock_Write_Dyn2Dyn_Metadata_Part.h"
#include "Stock_Write_Dyn2Dyn_Data_Part.h"

#include <dal/lofar/BF_File.h>


/// \file Reader_Dyn2Dyn_Part.cpp
///  \brief File C++ (associated to Reader_Dyn2Dyn_Part.h)for read ICD6 files and put in variables metadatas for the ICD6's Dynspec Group  
///  \details  
/// <br /> Overview:
/// <br /> This function readDynspec is described (programmed) here. It needs as parameter a Stock_Write_Dyn2Dyn_Metadata_Part object. The readDynspec function will read 
///  dynspec metadatas and stock them in the Stock_Write_Dyn2Dyn_Metadata_Part object as private attributes thanks to stockDynspecMetadata function.
///  These attributes will be written in the next step by writeDynspecMetadata function which is in the class 
///  Stock_Write_Dyn2Dyn_Metadata_Part.h  and called at the end of this Reader_Dyn2Dyn_Part.cpp


using namespace dal;
using namespace std;


  Reader_Dyn2Dyn_Part::Reader_Dyn2Dyn_Part(){}	// constructor 
  Reader_Dyn2Dyn_Part::~Reader_Dyn2Dyn_Part(){}	// destructor
  
  
    
  void Reader_Dyn2Dyn_Part::readDynspec(string pathFile,Stock_Write_Dyn2Dyn_Metadata_Part *dynspecMetadata,Stock_Write_Dyn2Dyn_Data_Part *dynspecData,int j,int obsNofStockes,vector<string> stokesComponent, float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin)
    { 
      
/// <br /> Usage:
/// <br />    void Reader_Dyn2Dyn_Part::readDynspec(string pathFile,Stock_Write_Dyn2Dyn_Metadata_Part *dynspecMetadata,Stock_Write_Dyn2Dyn_Data_Part *dynspecData,int j,int obsNofStockes,vector<string> stokesComponent, float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin)

/// \param  pathFile  ICD6 (dynamic spectrum) file to read 
/// \param  *dynspecMetadata Reader_Dyn2Dyn_Part Object for loading dynspec metadata in private attributes
/// \param  *dynspecData Stock_Write_Dyn2Dyn_Data_Part Object for loading new rebinning for data matrix processing 
/// \param  j loop index on Beams
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
      
      
      
      string index_j1;     
      std::ostringstream oss_j;oss_j << j;string index_j(oss_j.str()); 
      if (j<10){index_j1="_00"+index_j;}
      if (j>=10 && j<100){index_j1="_0"+index_j;}
      if (j>=100 && j<1000){index_j1="_"+index_j;}      

      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Programm  Load the hdf5's root part

      File file(pathFile);  					// generate an object called file contains Root
      Group Dynspec(file, "DYNSPEC"+index_j1);			// generate an object called Dynspec contains 1 Dynspec

      Group COORDS(Dynspec,"COORDINATES");			// Generate an Objet called COORDS contains Coords group 
      
      Group COORDS_POLARIZATION(COORDS,"POLARIZATION");	 
      Group COORDS_SPECTRAL(COORDS,"SPECTRAL");
      Group COORDS_TIME(COORDS,"TIME");
      
      Dataset<float> STOCKES(Dynspec, "DATA");			// generate an object called STOCKES contains stockes data      
      
  
      // Redefine ObsnofStokes and vector for special case 
      
      Attribute<int> obj_NOF_AXES_POL(COORDS_POLARIZATION, "NOF_AXES");
      Attribute< vector<string> > obj_AXIS_NAMES_POL(COORDS_POLARIZATION, "AXIS_NAMES");
      
      obsNofStockes = obj_NOF_AXES_POL.value;
      stokesComponent = obj_AXIS_NAMES_POL.value;
      
    
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////      
      // Modify Time Metadata for taking account time border effects due to the time selection and rebin
      float timeMinSelectTemp =timeMinSelect;
      float timeMaxSelectTemp = timeMaxSelect;
      
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
        
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////      
      // Variable iniatialization for DYNSPEC group 
      
      string GROUPE_TYPE_DYN("DYNSPEC");
      
      std::ostringstream oss_timeNewStart;oss_timeNewStart << timeMinSelect;string timeNewStart(oss_timeNewStart.str());
      std::ostringstream oss_timeNewEnd;oss_timeNewEnd << timeMaxSelect;string timeNewEnd(oss_timeNewEnd.str());


      Attribute<string> obj_OBSERVATION_START_UTC(file, "OBSERVATION_START_UTC");
      Attribute<double> obj_OBSERVATION_START_MJD(file, "OBSERVATION_START_MJD");
      Attribute<string> obj_OBSERVATION_END_UTC(file, "OBSERVATION_END_UTC");
      Attribute<double> obj_OBSERVATION_END_MJD(file, "OBSERVATION_END_MJD");    
             
    
      string DYNSPEC_START_UTC(obj_OBSERVATION_START_UTC.value);
      DYNSPEC_START_UTC = DYNSPEC_START_UTC+" + "+timeNewStart+" s";
      double DYNSPEC_START_MJD(obj_OBSERVATION_START_MJD.value+(timeMinSelect/86400));
      string DYNSPEC_STOP_UTC(obj_OBSERVATION_END_UTC.value);
      DYNSPEC_STOP_UTC = DYNSPEC_STOP_UTC+" - "+timeNewEnd+" s";
      double DYNSPEC_STOP_MJD(obj_OBSERVATION_END_MJD.value-(timeMaxSelect/86400));
      
      
      // TARGET_DYN

      Attribute<string> obj_TRACKING(Dynspec, "TRACKING");
      Attribute<string> obj_TARGET(Dynspec, "TARGET");
      
      string TRACKING(obj_TRACKING.value);   
      string TARGET_DYN(obj_TARGET.value);

      string ONOFF;
      if (TRACKING == "J2000"){ONOFF = "ON";}
      if (TRACKING =="LMN"){ONOFF = "OFF_0";}     
      if (TRACKING =="TBD"){ONOFF = "OFF_1";}      
      
      Attribute<double> obj_POINT_RA(Dynspec, "POINT_RA");
      Attribute<double> obj_POINT_DEC(Dynspec, "POINT_DEC");     
      Attribute<double> obj_POSITION_OFFSET_RA(Dynspec, "POSITION_OFFSET_RA");
      Attribute<double> obj_POSITION_OFFSET_DEC(Dynspec, "POSITION_OFFSET_DEC");
      Attribute<double> obj_BEAM_DIAMETER_RA(Dynspec, "BEAM_DIAMETER_RA");
      Attribute<double> obj_BEAM_DIAMETER_DEC (Dynspec, "BEAM_DIAMETER_DEC");

      double POINT_RA(obj_POINT_RA.value);
      double POINT_DEC(obj_POINT_DEC.value);     
      double POSITION_OFFSET_RA(obj_POSITION_OFFSET_RA.value);
      double POSITION_OFFSET_DEC(obj_POSITION_OFFSET_DEC.value);
      double BEAM_DIAMETER_RA(obj_BEAM_DIAMETER_RA.value);
      double BEAM_DIAMETER_DEC(obj_BEAM_DIAMETER_DEC.value);
      
      double BEAM_DIAMETER(0); 
      
      
      Attribute<double> obj_BEAM_NOF_STATIONS(Dynspec, "BEAM_NOF_STATIONS");
      Attribute<vector<string> > obj_BEAM_STATIONS_LIST(Dynspec, "BEAM_STATIONS_LIST");
       
      double BEAM_NOF_STATIONS(obj_BEAM_NOF_STATIONS.value);
      vector<string> BEAM_STATIONS_LIST(obj_BEAM_STATIONS_LIST.value);
      
      
      Attribute<string> obj_DEDISPERSION(Dynspec, "DEDISPERSION");
      Attribute<double> obj_DISPERSION_MEASURE(Dynspec, "DISPERSION_MEASURE");
      Attribute<string> obj_DISPERSION_MEASURE_UNIT(Dynspec, "DISPERSION_MEASURE_UNIT");
      Attribute<bool> obj_BARYCENTER(Dynspec, "BARYCENTER");
      
      //Attribute<vector<string> > obj_STOCKES_COMPONENT(Dynspec, "STOCKES_COMPONENT");
      
      Attribute<bool> obj_COMPLEX_VOLTAGE(Dynspec, "COMPLEX_VOLTAGE");
      Attribute<string> obj_SIGNAL_SUM(Dynspec, "SIGNAL_SUM");
      
      string DEDISPERSION(obj_DEDISPERSION.value);
      double DISPERSION_MEASURE(obj_DISPERSION_MEASURE.value);
      string DISPERSION_MEASURE_UNIT(obj_DISPERSION_MEASURE_UNIT.value);
      bool BARYCENTER(obj_BARYCENTER.value);
      
      //vector<string> STOCKES_COMPONENT(obj_STOCKES_COMPONENT.value);
      vector<string> STOCKES_COMPONENT(stokesComponent);
      
      bool COMPLEX_VOLTAGE(obj_COMPLEX_VOLTAGE.value);
      string SIGNAL_SUM(obj_SIGNAL_SUM.value);
      
   
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Variable iniatialization for COORDINATES group    
      
      Attribute<vector<double> > obj_REF_LOCATION_VALUE(COORDS, "REF_LOCATION_VALUE");
      Attribute<vector<string> > obj_REF_LOCATION_UNIT(COORDS, "REF_LOCATION_UNIT");
      Attribute<string> obj_REF_LOCATION_FRAME(COORDS, "REF_LOCATION_FRAME");
      Attribute<double> obj_REF_TIME_VALUE(COORDS, "REF_TIME_VALUE");
      Attribute<string> obj_REF_TIME_UNIT(COORDS, "REF_TIME_UNIT");
      Attribute<string> obj_REF_TIME_FRAME(COORDS, "REF_TIME_FRAME");
      
       
      vector<double> REF_LOCATION_VALUE(obj_REF_LOCATION_VALUE.value);
      vector<string> REF_LOCATION_UNIT(obj_REF_LOCATION_UNIT.value);
      string REF_LOCATION_FRAME(obj_REF_LOCATION_FRAME.value);     
      double REF_TIME_VALUE(obj_REF_TIME_VALUE.value+(timeMinSelect/86400));
      string REF_TIME_UNIT(obj_REF_TIME_UNIT.value);
      string REF_TIME_FRAME(obj_REF_TIME_FRAME.value);
      
      
      string GROUPE_TYPE_COORD("Coordinates");
      double NOF_COORDINATES(3);
      double NOF_AXIS(3);
      vector<string> COORDINATE_TYPES(3);
      COORDINATE_TYPES[0]="Time";
      COORDINATE_TYPES[1]="Spectral";
      COORDINATE_TYPES[2]="Polarisation";


      
      ///////////////////////////////////////////////////////////
      // Variable iniatialization for TIME coordinate  group        
      
      Attribute<int> obj_NOF_AXES_TIME(COORDS_TIME, "NOF_AXES");
      Attribute<vector<string> > obj_AXIS_NAMES_TIME(COORDS_TIME, "AXIS_NAMES");
      Attribute<vector<double> > obj_REFERENCE_VALUE_TIME(COORDS_TIME, "REFERENCE_VALUE");
      Attribute<vector<double> > obj_REFERENCE_PIXEL_TIME(COORDS_TIME, "REFERENCE_PIXEL");
 
      int NOF_AXES_TIME(obj_NOF_AXES_TIME.value);      
      vector<string> AXIS_NAMES_TIME(obj_AXIS_NAMES_TIME.value);      
      vector<double> REFERENCE_VALUE_TIME(obj_REFERENCE_VALUE_TIME.value);
      vector<double> REFERENCE_PIXEL_TIME(obj_REFERENCE_PIXEL_TIME.value);
        
      
      vector<string> AXIS_UNIT_TIME(1);AXIS_UNIT_TIME[0]="s"; 
      string GROUPE_TYPE_TIME("TimeCoord");
      string COORDINATE_TYPE_TIME("Time");
      vector<string> STORAGE_TYPE_TIME(1); STORAGE_TYPE_TIME[0]="Linear";    
      vector<double> INCREMENT_TIME(1);INCREMENT_TIME[0]=timeRebin;         
      vector<double> PC_TIME(2);PC_TIME[0]=1;PC_TIME[1]=0;           
      int Ntime=((timeMaxSelect-timeMinSelect)/timeRebin);
                

      
     
     
      ///////////////////////////////////////////////////////////
      // Variable iniatialization for SPECTRAL coordinate  group 
      // SEE AT THE BEGINNING OF DYNSPEC PART (line 
      
      Attribute<vector<string> > obj_STORAGE_TYPE_SPECTRAL(COORDS_SPECTRAL, "STORAGE_TYPE");
      Attribute<int> obj_NOF_AXES_SPECTRAL(COORDS_SPECTRAL, "NOF_AXES");
      Attribute<vector<string> > obj_AXIS_NAMES_SPECTRAL(COORDS_SPECTRAL, "AXIS_NAMES");
   
      vector<string> STORAGE_TYPE_SPECTRAL(obj_STORAGE_TYPE_SPECTRAL.value);    
      int NOF_AXES_SPECTRAL(obj_NOF_AXES_SPECTRAL.value);      
      vector<string> AXIS_NAMES_SPECTRAL(obj_AXIS_NAMES_SPECTRAL.value);
      
      
      string GROUPE_TYPE_SPECTRAL("SpectralCoord");
      string COORDINATE_TYPE_SPECTRAL("Spectral");   
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
	    AXIS_VALUE_WORLD_SPECTRAL[i]=sumSpecRebin/CHANNELS_PER_SUBANDS_TEMP*frequencyRebin;
	}     

      
      ///////////////////////////////////////////////////////////
      // FREQUENCY INFO      

      Attribute<int> obj_SUBBAND_WIDTH(file, "SUBBAND_WIDTH");
      double SUBBAND_WIDTH(obj_SUBBAND_WIDTH.value);  
      
      double BEAM_FREQUENCY_MAX((AXIS_VALUE_WORLD_SPECTRAL[(AXIS_VALUE_WORLD_SPECTRAL.size())-1])/1E6);
      double BEAM_FREQUENCY_MIN((AXIS_VALUE_WORLD_SPECTRAL[0])/1E6);   
      double BEAM_FREQUENCY_CENTER((BEAM_FREQUENCY_MAX+BEAM_FREQUENCY_MIN)/2.0);               
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
      
      Attribute<int> obj_NtimeReal(file, "NOF_SAMPLES");
      int NtimeReal = (obj_NtimeReal.value);
      
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
      
      

      
    }  // end of the Reader_Dynspec object      

      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      