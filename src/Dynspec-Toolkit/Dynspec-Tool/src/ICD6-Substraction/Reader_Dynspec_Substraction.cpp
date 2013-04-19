#include <iostream>
#include <string>
#include <fstream>

#include "Reader_Dynspec_Substraction.h"
#include "Stock_Write_Dynspec_Metadata_Substraction.h"
#include "Stock_Write_Dynspec_Data_Substraction.h"

#include <dal/lofar/BF_File.h>

/// \file Reader_Dynspec_Substraction.cpp
///  \brief File C++ (associated to Reader_Dynspec_Substraction.h)for read ICD3 files and put in variables metadatas for the ICD6's Dynspec Group  
///  \details  
/// <br /> Overview:
/// <br /> This function readDynspec is described (programmed) here. It needs as parameter a Stock_Write_Dynspec_Metadata_Substraction object. The readDynspec function will read 
///  dynspec metadatas and stock them in the Stock_Write_Dynspec_Metadata_Substraction object as private attributes thanks to stockDynspecMetadata function.
///  These attributes will be written in the next step by writeDynspecMetadata function which is in the class 
///  Stock_Write_Dynspec_Metadata_Substraction.h  and called at the end of this Reader_Dynspec_Substraction.cpp


using namespace dal;
using namespace std;


  Reader_Dynspec_Substraction::Reader_Dynspec_Substraction(){}	// constructor 
  Reader_Dynspec_Substraction::~Reader_Dynspec_Substraction(){}	// destructor
  
  
  
   bool is_readableBis( const std::string & file ) 
  { 

/// \param file
/// <br />Check if file is readable, so if file exists !
/// \return boolean value 

    std::ifstream fichier( file.c_str() ); 
    return !fichier.fail(); 
  } 
    
    
  void Reader_Dynspec_Substraction::readDynspec(string ID, string file1, string dynspec1, string file2, string dynspec2, float kfactor,Stock_Write_Dynspec_Metadata_Substraction *dynspecMetadata,Stock_Write_Dynspec_Data_Substraction *dynspecData,int nbSTOkES,vector<string> stokesComponent)
    {
      
/// <br /> Usage:
/// <br />     void Reader_Dynspec_Substraction::readDynspec(string pathFile,Stock_Write_Dynspec_Metadata_Substraction *dynspecMetadata,Stock_Write_Dynspec_Data_Substraction *dynspecData,int i,int j,int q,int obsNofSAP,int obsNofSource,int obsNofFrequencyBand,int obsNofStockes,vector<string> stokesComponent, int SAPindex)

 
/// \param  pathFile  ICD3 file to read 
/// \param  *dynspecMetadata Reader_Dynspec_Substraction_Part Object for loading dynspec metadata in private attributes
/// \param  *dynspecData Stock_Write_Dynspec_Data_Substraction_Part Object for loading new rebinning for data matrix processing 
/// \param  i loop index on SAP
/// \param  j loop index on Beams
/// \param  q loop index on Pxxx if this nomenclature is used
/// \param  obsNofSAP number of SAP
/// \param  obsNofSource number of Sources
/// \param  obsNofFrequencyBand number of Frequency Band
/// \param  obsNofStocke number of Stokes parameter
/// \param  stokesComponent vector with Stokes component 
/// \param  SAPindex Index of the SAP in current processing 

/// \return nothing        
            		

  // Start the substraction => load datas
  
  // First Dynspec 
  File  File1(file1);  					// generate an object called file contains Root
  Group Dynspec1(File1,dynspec1);				// generate an object called Dynspec contains 1 Dynspec
  Group COORDS1(Dynspec1,"COORDINATES");			// Generate an Objet called COORDS contains Coords group      
  Group COORDS_POLARIZATION1(COORDS1,"POLARIZATION");	 
  Group COORDS_SPECTRAL1(COORDS1,"SPECTRAL");
  Group COORDS_TIME1(COORDS1,"TIME");     
  Dataset<float> STOCKES1(Dynspec1, "DATA");		// generate an object called STOCKES contains stockes data  
  
  
  // Second Dynspec 
  File  File2(file2);  					// generate an object called file contains Root
  Group Dynspec2(File2,dynspec2);				// generate an object called Dynspec contains 1 Dynspec
  Group COORDS2(Dynspec2,"COORDINATES");			// Generate an Objet called COORDS contains Coords group      
  Group COORDS_POLARIZATION2(COORDS2,"POLARIZATION");	 
  Group COORDS_SPECTRAL2(COORDS2,"SPECTRAL");
  Group COORDS_TIME2(COORDS2,"TIME");     
  Dataset<float> STOCKES2(Dynspec2, "DATA");		// generate an object called STOCKES contains stockes data  
            		
            		

	  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Programm  Load the hdf5's root part

  
  // Redefine ObsnofStokes and vector for special case 
  
  Attribute<int> obj_NOF_AXES_POL(COORDS_POLARIZATION1, "NOF_AXES");
  Attribute< vector<string> > obj_AXIS_NAMES_POL(COORDS_POLARIZATION1, "AXIS_NAMES");
  
  int obsNofStockes(obj_NOF_AXES_POL.value);
  stokesComponent = obj_AXIS_NAMES_POL.value;      

    
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////      
  // Variable iniatialization for DYNSPEC group   
  
  string GROUPE_TYPE_DYN("DYNSPEC");
  

  Attribute<string> obj_OBSERVATION_START_UTC(File1, "OBSERVATION_START_UTC");
  Attribute<double> obj_OBSERVATION_START_MJD(File1, "OBSERVATION_START_MJD");
  Attribute<string> obj_OBSERVATION_END_UTC(File1, "OBSERVATION_END_UTC");
  Attribute<double> obj_OBSERVATION_END_MJD(File1, "OBSERVATION_END_MJD");    
  
  
  string DYNSPEC_START_UTC(obj_OBSERVATION_START_UTC.value);
  double DYNSPEC_START_MJD(obj_OBSERVATION_START_MJD.value);
  string DYNSPEC_STOP_UTC(obj_OBSERVATION_END_UTC.value);
  double DYNSPEC_STOP_MJD(obj_OBSERVATION_END_MJD.value);      
      
  // TARGET_DYN

  Attribute<string> obj_TRACKING(Dynspec1, "TRACKING");
  string TRACKING(obj_TRACKING.value);
  
  string ONOFF;
  if (TRACKING == "J2000"){ONOFF = "ON";}
  if (TRACKING =="LMN"){ONOFF = "OFF_0";}     
  if (TRACKING =="TBD"){ONOFF = "OFF_1";}        
  
  
  Attribute<string> obj_TARGET1(Dynspec1, "TARGET");
  Attribute<string> obj_TARGET2(Dynspec2, "TARGET");      
  
  std::ostringstream oss_kfactor;oss_kfactor << kfactor;string strkfactor(oss_kfactor.str());  
  string TARGET_DYN_TEMP1(obj_TARGET1.value);
  string TARGET_DYN_TEMP2(obj_TARGET2.value);
  
  string TARGET_DYN_TEMP;
  string TARGET_DYN(TARGET_DYN_TEMP1+"-"+strkfactor+"x"+TARGET_DYN_TEMP2);
  
  
  Attribute<double> obj_POINT_RA(Dynspec1, "POINT_RA");
  Attribute<double> obj_POINT_DEC(Dynspec1, "POINT_DEC");
  
  Attribute<double> obj_POSITION_OFFSET_RA(Dynspec2, "POINT_RA");
  Attribute<double> obj_POSITION_OFFSET_DEC(Dynspec2, "POINT_DEC");
  
  Attribute<double> obj_BEAM_DIAMETER_RA(Dynspec1, "BEAM_DIAMETER_RA");
  Attribute<double> obj_BEAM_DIAMETER_DEC (Dynspec1, "BEAM_DIAMETER_DEC");
  
  
  double POINT_RA(obj_POINT_RA.value);
  double POINT_DEC(obj_POINT_DEC.value);     
  double POSITION_OFFSET_RA(obj_POSITION_OFFSET_RA.value);
  double POSITION_OFFSET_DEC(obj_POSITION_OFFSET_DEC.value);
  double BEAM_DIAMETER_RA(obj_BEAM_DIAMETER_RA.value);
  double BEAM_DIAMETER_DEC(obj_BEAM_DIAMETER_DEC.value);
  
  double BEAM_DIAMETER(0); 
  

  // Station list => special cases    
  
  Attribute<double> obj_BEAM_NOF_STATIONS1(Dynspec1, "BEAM_NOF_STATIONS");
  Attribute<vector<string> > obj_BEAM_STATIONS_LIST1(Dynspec1, "BEAM_STATIONS_LIST");
  
  Attribute<double> obj_BEAM_NOF_STATIONS2(Dynspec2, "BEAM_NOF_STATIONS");
  Attribute<vector<string> > obj_BEAM_STATIONS_LIST2(Dynspec2, "BEAM_STATIONS_LIST");
  
  
  double BEAM_NOF_STATIONS1(obj_BEAM_NOF_STATIONS1.value);
  vector<string> BEAM_STATIONS_LIST1(obj_BEAM_STATIONS_LIST1.value);
  
  double BEAM_NOF_STATIONS2(obj_BEAM_NOF_STATIONS2.value);
  vector<string> BEAM_STATIONS_LIST2(obj_BEAM_STATIONS_LIST2.value);      
  
  int k(0);
  double BEAM_NOF_STATIONS(BEAM_NOF_STATIONS1+BEAM_NOF_STATIONS2);
  vector<string> BEAM_STATIONS_LIST(1);
  
  BEAM_STATIONS_LIST[0]="{";
  for (k=0;k< BEAM_NOF_STATIONS1;k++){	
    BEAM_STATIONS_LIST[0]=BEAM_STATIONS_LIST[0]+BEAM_STATIONS_LIST1[k]+"-";	  
  }      
  BEAM_STATIONS_LIST[0] = BEAM_STATIONS_LIST[0]+"}-"+strkfactor+"x{";     
  for (k=0;k< BEAM_NOF_STATIONS2;k++){	
    BEAM_STATIONS_LIST[0]=BEAM_STATIONS_LIST[0]+BEAM_STATIONS_LIST2[k]+"-";
  }    
  BEAM_STATIONS_LIST[0] = BEAM_STATIONS_LIST[0]+"}";
  
  
  Attribute<string> obj_DEDISPERSION(Dynspec1, "DEDISPERSION");
  Attribute<double> obj_DISPERSION_MEASURE(Dynspec1, "DISPERSION_MEASURE");
  Attribute<string> obj_DISPERSION_MEASURE_UNIT(Dynspec1, "DISPERSION_MEASURE_UNIT");
  Attribute<bool> obj_BARYCENTER(Dynspec1, "BARYCENTER");          
  Attribute<bool> obj_COMPLEX_VOLTAGE(Dynspec1, "COMPLEX_VOLTAGE");
  Attribute<string> obj_SIGNAL_SUM(Dynspec1, "SIGNAL_SUM");
  
  string DEDISPERSION(obj_DEDISPERSION.value);
  double DISPERSION_MEASURE(obj_DISPERSION_MEASURE.value);
  string DISPERSION_MEASURE_UNIT(obj_DISPERSION_MEASURE_UNIT.value);
  bool BARYCENTER(obj_BARYCENTER.value);          
  bool COMPLEX_VOLTAGE(obj_COMPLEX_VOLTAGE.value);
  string SIGNAL_SUM(obj_SIGNAL_SUM.value);
  
  
  vector<string> STOCKES_COMPONENT(stokesComponent);
      
      
      
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Variable iniatialization for COORDINATES group    
  
  Attribute<vector<double> > obj_REF_LOCATION_VALUE(COORDS1, "REF_LOCATION_VALUE");
  Attribute<vector<string> > obj_REF_LOCATION_UNIT(COORDS1, "REF_LOCATION_UNIT");
  Attribute<string> obj_REF_LOCATION_FRAME(COORDS1, "REF_LOCATION_FRAME");
  Attribute<double> obj_REF_TIME_VALUE(COORDS1, "REF_TIME_VALUE");
  Attribute<string> obj_REF_TIME_UNIT(COORDS1, "REF_TIME_UNIT");
  Attribute<string> obj_REF_TIME_FRAME(COORDS1, "REF_TIME_FRAME");
  
  
  vector<double> REF_LOCATION_VALUE(obj_REF_LOCATION_VALUE.value);
  vector<string> REF_LOCATION_UNIT(obj_REF_LOCATION_UNIT.value);
  string REF_LOCATION_FRAME(obj_REF_LOCATION_FRAME.value);     
  double REF_TIME_VALUE(obj_REF_TIME_VALUE.value);
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
  
  Attribute<int> obj_NOF_AXES_TIME(COORDS_TIME1, "NOF_AXES");
  Attribute<vector<string> > obj_AXIS_NAMES_TIME(COORDS_TIME1, "AXIS_NAMES");
  Attribute<vector<double> > obj_REFERENCE_VALUE_TIME(COORDS_TIME1, "REFERENCE_VALUE");
  Attribute<vector<double> > obj_REFERENCE_PIXEL_TIME(COORDS_TIME1, "REFERENCE_PIXEL");
  
  int NOF_AXES_TIME(obj_NOF_AXES_TIME.value);      
  vector<string> AXIS_NAMES_TIME(obj_AXIS_NAMES_TIME.value);      
  vector<double> REFERENCE_VALUE_TIME(obj_REFERENCE_VALUE_TIME.value);
  vector<double> REFERENCE_PIXEL_TIME(obj_REFERENCE_PIXEL_TIME.value);
  
  Attribute<vector<double> > obj_INCREMENT_TIME(COORDS_TIME1, "INCREMENT");
  vector<double> INCREMENT_TIME(obj_INCREMENT_TIME.value);
  
  vector<string> AXIS_UNIT_TIME(1);AXIS_UNIT_TIME[0]="s"; 
  string GROUPE_TYPE_TIME("TimeCoord");
  string COORDINATE_TYPE_TIME("Time");
  vector<string> STORAGE_TYPE_TIME(1); STORAGE_TYPE_TIME[0]="Linear";    
  vector<double> PC_TIME(2);PC_TIME[0]=1;PC_TIME[1]=0;           
  
  Attribute<int> obj_NOF_SAMPLES (File1, "NOF_SAMPLES");
  int Ntime=(obj_NOF_SAMPLES.value);      
      
      
  
  ///////////////////////////////////////////////////////////
  // Variable iniatialization for SPECTRAL coordinate  group 
  // SEE AT THE BEGINNING OF DYNSPEC PART (line 
  
  Attribute<vector<string> > obj_STORAGE_TYPE_SPECTRAL(COORDS_SPECTRAL1, "STORAGE_TYPE");
  Attribute<int> obj_NOF_AXES_SPECTRAL(COORDS_SPECTRAL1, "NOF_AXES");
  Attribute<vector<string> > obj_AXIS_NAMES_SPECTRAL(COORDS_SPECTRAL1, "AXIS_NAMES");
  
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
  Attribute<vector<double> > obj_AXIS_VALUE_WORLD_SPECTRAL(COORDS_SPECTRAL1, "AXIS_VALUE_WORLD");
  Attribute<vector<double> > obj_AXIS_VALUE_PIXEL_SPECTRAL(COORDS_SPECTRAL1, "AXIS_VALUE_PIXEL");
  
  
  vector<double> AXIS_VALUE_WORLD_SPECTRAL(obj_AXIS_VALUE_WORLD_SPECTRAL.value);
  vector<double> AXIS_VALUE_PIXEL_SPECTRAL(obj_AXIS_VALUE_PIXEL_SPECTRAL.value);
  int Nspectral(AXIS_VALUE_WORLD_SPECTRAL.size());
  

  ///////////////////////////////////////////////////////////
  // FREQUENCY INFO      
  
  Attribute<int> obj_CHANNELS_PER_SUBANDS (File1, "CHANNELS_PER_SUBANDS");
  int CHANNELS_PER_SUBANDS(obj_CHANNELS_PER_SUBANDS.value);
  
  int NOF_SUBBANDS(Nspectral/CHANNELS_PER_SUBANDS);
  
  Attribute<int> obj_SUBBAND_WIDTH(File1, "SUBBAND_WIDTH");
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
  
  
  
  string PREPROCCESSINGINFO1("Substraction of : {file1} - kfactor x {file2}");
  string PREPROCCESSINGINFO2("file1 = "+file1);
  string PREPROCCESSINGINFO3("file2 = "+file2);
  string PREPROCCESSINGINFO4("kfactor = "+strkfactor);
  string PREPROCCESSINGINFO5("No rebinning changes from original files");
  
  
      
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
					   
			   
      dynspecData->stockDynspecData(Ntime,Nspectral);
      

    }  // end of the Reader_Dynspec_Substraction object 

      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
