#include <iostream>
#include <string>
#include <fstream>


#include "Reader_Dynspec_Part.h"
#include "Stock_Write_Dynspec_Metadata_Part.h"
#include "Stock_Write_Dynspec_Data_Part.h"

#include <dal/lofar/BF_File.h>

/// \file Reader_Dynspec_Part.cpp
///  \brief File C++ (associated to Reader_Dynspec_Part.h)for read ICD3 files and put in variables metadatas for the ICD6's Dynspec Group  
///  \details  
/// <br /> Overview:
/// <br /> This function readDynspec is described (programmed) here. It needs as parameter a Stock_Write_Dynspec_Metadata_Part object. The readDynspec function will read 
///  dynspec metadatas and stock them in the Stock_Write_Dynspec_Metadata_Part object as private attributes thanks to stockDynspecMetadata function.
///  These attributes will be written in the next step by writeDynspecMetadata function which is in the class 
///  Stock_Write_Dynspec_Metadata_Part.h  and called at the end of this Reader_Dynspec_Part.cpp



using namespace dal;
using namespace std;


  Reader_Dynspec_Part::Reader_Dynspec_Part(){}	// constructor 
  Reader_Dynspec_Part::~Reader_Dynspec_Part(){}	// destructor
  

   bool is_readableBis( const std::string & file ) 
  { 

/// \param file
/// <br />Check if file is readable, so if file exists !
/// \return boolean value 

    std::ifstream fichier( file.c_str() ); 
    return !fichier.fail(); 
  } 
  
   
  void Reader_Dynspec_Part::readDynspec(string pathFile,Stock_Write_Dynspec_Metadata_Part *dynspecMetadata,Stock_Write_Dynspec_Data_Part *dynspecData,int i,int j,int q,int obsNofSAP,int obsNofSource,int obsNofFrequencyBand,int obsNofStockes,vector<string> stokesComponent, int SAPindex,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin)
    {        
       
  
/// <br /> Usage:
/// <br />   void Reader_Dynspec_Part::readDynspec(string pathFile,Stock_Write_Dynspec_Metadata_Part *dynspecMetadata,Stock_Write_Dynspec_Data_Part *dynspecData,int i,int j,int q,int obsNofSAP,int obsNofSource,int obsNofFrequencyBand,int obsNofStockes,vector<string> stokesComponent, int SAPindex,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin)
 
/// \param  pathFile  ICD3 file to read 
/// \param  *dynspecMetadata Reader_Dynspec_Part Object for loading dynspec metadata in private attributes
/// \param  *dynspecData Stock_Write_Dynspec_Data_Part Object for loading new rebinning for data matrix processing 
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
      
      
      string index_i1,index_j1,index_q1;
      std::ostringstream oss_i;oss_i << i;string index_i(oss_i.str());
      if (i<10){index_i1="_SAP00"+index_i;}
      if (i>=10 && i<100){index_i1="_SAP0"+index_i;}
      if (i>=100 && i<1000){index_i1="_SAP"+index_i;}
      
      std::ostringstream oss_j;oss_j << j;string index_j(oss_j.str()); 
      if (j<10){index_j1="_B00"+index_j;}
      if (j>=10 && j<100){index_j1="_B0"+index_j;}
      if (j>=100 && j<1000){index_j1="_B"+index_j;}      
      
      std::ostringstream oss_q;oss_q << q;string index_q(oss_q.str()); 
      if (q<10){index_q1="_P00"+index_q;}
      if (q>=10 && q<100){index_q1="_P0"+index_q;}
      if (q>=100 && q<1000){index_q1="_P"+index_q;}   
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Programm  Load the hdf5's root part
   
      int noEmptyStokes(0);
      string pathFile0 = pathFile;
      
      pathFile	= pathFile0+index_i1+index_j1+"_S0"+index_q1+"_bf.h5";			

      if (is_readableBis(pathFile)) 
	  {noEmptyStokes =0;}
      else
	  {pathFile= pathFile0+index_i1+index_j1+"_S1"+index_q1+"_bf.h5";
	   if (is_readableBis(pathFile))
		{noEmptyStokes =1;}	   
	   else
		{pathFile= pathFile0+index_i1+index_j1+"_S2"+index_q1+"_bf.h5";
		 if (is_readableBis(pathFile))
		     {noEmptyStokes =2;}
		 else
		     {pathFile= pathFile0+index_i1+index_j1+"_S3"+index_q1+"_bf.h5";
		      if (is_readableBis(pathFile))
			  {noEmptyStokes =3;}
		      else
			  {noEmptyStokes =4;}
		      }
		}
	  }
      
      if (noEmptyStokes < 4)	// Test for checking Empty Dynspec !!
      {      
      BF_File file(pathFile);							// generate an object called file contains Root
      
      BF_SubArrayPointing SAP 		= file.subArrayPointing(i);		// generate an object called SAP contains subarray pointings
      BF_BeamGroup BEAM			= SAP.beam(j);      			// generate an object called BEAM contains Beams

      
      
      CoordinatesGroup COORDS		= BEAM.coordinates();      		// generate an object called COORDS contains Coodinates directory
      Coordinate *TIME_COORDS		= COORDS.coordinate(0);			// generate an object called TIME_COORDS contains time data
      Coordinate *SPECTRAL_COORDS	= COORDS.coordinate(1);			// generate an object called SPECTRAL_COORDS contains spectral data

      NumericalCoordinate *NUM_COORD_TIME = dynamic_cast<NumericalCoordinate*>(TIME_COORDS); // load Numerical coordinates lib 
      DirectionCoordinate *DIR_COORD_TIME = dynamic_cast<DirectionCoordinate*>(TIME_COORDS); // load Direction coordinates lib
      
      NumericalCoordinate *NUM_COORD_SPECTRAL = dynamic_cast<NumericalCoordinate*>(SPECTRAL_COORDS); // load Numerical coordinates lib 
      DirectionCoordinate *DIR_COORD_SPECTRAL = dynamic_cast<DirectionCoordinate*>(SPECTRAL_COORDS); // load Direction coordinates lib
      

      // Redefine ObsnofStokes and vector for special case 
      obsNofStockes = BEAM.observationNofStokes().get();      
      if (obsNofStockes == 1){stokesComponent.resize(1);stokesComponent[0]='I';}
      if (obsNofStockes == 2){stokesComponent.resize(2);stokesComponent[0]='I';stokesComponent[1]='Q';}
      if (obsNofStockes == 3){stokesComponent.resize(3);stokesComponent[0]='I';stokesComponent[1]='Q';stokesComponent[2]='U';}
      if (obsNofStockes == 4){stokesComponent.resize(4);stokesComponent[0]='I';stokesComponent[1]='Q';stokesComponent[2]='U';stokesComponent[3]='V';}
            
      
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////      
      // Modify Time Metadata for taking account time border effects due to the time selection and rebin
      float timeMinSelectTemp =timeMinSelect;
      float timeMaxSelectTemp = timeMaxSelect;
      
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
      // Generate New Rebinned and selected AXIS_VALUE_WORLD_SPECTRAL
      vector<double> AXIS_VALUE_WORLD_SPECTRAL_TEMP(NUM_COORD_SPECTRAL->axisValuesWorld().get());      
      int NOF_SUBBANDS_TEMP= (AXIS_VALUE_WORLD_SPECTRAL_TEMP.size())/(BEAM.channelsPerSubband().get());
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
      oss << file.observationEndMJD().get()-(timeMaxSelect/86400);string DYNSPEC_STOP_MJD(oss.str());
      
      
      std::ostringstream oss_timeNewStart;oss_timeNewStart << timeMinSelect;string timeNewStart(oss_timeNewStart.str());
      std::ostringstream oss_timeNewEnd;oss_timeNewEnd << timeMaxSelect;string timeNewEnd(oss_timeNewEnd.str());
      string DYNSPEC_START_UTC(file.observationStartUTC().get()+" + "+timeNewStart+" s");
      string DYNSPEC_STOP_UTC(file.observationEndUTC().get()+" - "+timeNewEnd+" s");
      
      // TARGET_DYN
      string TRACKING(BEAM.tracking().get());   
      vector<string> TARGETS_DYN(file.targets().get());
      string TARGET_DYN(TARGETS_DYN[SAPindex]);
      
      string ONOFF;
      if (TRACKING == "J2000"){ONOFF = "ON";}
      if (TRACKING =="LMN"){ONOFF = "OFF_0";}     
      if (TRACKING =="TBD"){ONOFF = "OFF_1";}
      
      double POINT_RA(BEAM.pointRA().get());
      double POINT_DEC(BEAM.pointDEC().get());
      
      double POSITION_OFFSET_RA(BEAM.pointOffsetRA().get());
      double POSITION_OFFSET_DEC(BEAM.pointOffsetDEC().get());
      
      double BEAM_DIAMETER(0); 
      double BEAM_DIAMETER_RA(BEAM.beamDiameterRA().get());
      double BEAM_DIAMETER_DEC(BEAM.beamDiameterDEC().get());

 
      double BEAM_NOF_STATIONS(BEAM.nofStations().get());
      vector<string> BEAM_STATIONS_LIST(BEAM.stationsList().get());
  
      string DEDISPERSION(BEAM.dedispersion().get());
      double DISPERSION_MEASURE(BEAM.dispersionMeasure().get());
      string DISPERSION_MEASURE_UNIT(BEAM.dispersionMeasureUnit().get());
  
      
      bool BARYCENTER(BEAM.barycentered().get());
      vector<string> STOCKES_COMPONENT(stokesComponent);
      bool COMPLEX_VOLTAGE(BEAM.complexVoltage().get());
      string SIGNAL_SUM(BEAM.signalSum().get());
      
      
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Variable iniatialization for COORDINATES group    
      
      string GROUPE_TYPE_COORD("Coordinates");
      vector<double> REF_LOCATION_VALUE(COORDS.refLocationValue().get());
      vector<string> REF_LOCATION_UNIT(COORDS.refLocationUnit().get());
      string REF_LOCATION_FRAME(COORDS.refLocationFrame().get());
      
      double REF_TIME_VALUE(file.observationStartMJD().get()+(timeMinSelect/86400));
      string REF_TIME_UNIT(COORDS.refTimeUnit().get());
      string REF_TIME_FRAME(COORDS.refTimeFrame().get());
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
	    AXIS_VALUE_WORLD_SPECTRAL[i]=sumSpecRebin/CHANNELS_PER_SUBANDS_TEMP*frequencyRebin;
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
      
      int NtimeReal = (BEAM.nofSamples().get());
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
    }  // End of empty BEAM

      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
