#include <iostream>
#include <string>


#include "Reader_Dynspec.h"
#include "Stock_Write_Dynspec_Metadata.h"
#include "Stock_Write_Dynspec_Data.h"

#include <dal/lofar/BF_File.h>


using namespace DAL;


using namespace std;


  Reader_Dynspec::Reader_Dynspec(){}	// constructor 
  Reader_Dynspec::~Reader_Dynspec(){}	// destructor
  
  
    
  void Reader_Dynspec::readDynspec(string pathFile,Stock_Write_Dynspec_Metadata *dynspecMetadata,Stock_Write_Dynspec_Data *dynspecData,int i,int j,int q,int obsNofSAP,int obsNofSource,int obsNofFrequencyBand,int obsNofStockes,vector<string> stokesComponent)
    { 
      string index_i1,index_j1,index_q1;
      std::ostringstream oss_i;oss_i << i;string index_i(oss_i.str());
      if (i<10){index_i1="_SAP00"+index_i;}
      if (i>=10 && i<100){index_i1="_SAP0"+index_i;}
      if (i>=100 && i<1000){index_i1="_SAP"+index_i;}
      
      std::ostringstream oss_j;oss_j << j;string index_j(oss_j.str()); 
      if (j<10){index_j1="_B00"+index_j;}
      if (j>=10 && i<100){index_j1="_B0"+index_j;}
      if (j>=100 && i<1000){index_j1="_B"+index_j;}      
      
      std::ostringstream oss_q;oss_q << q;string index_q(oss_q.str()); 
      if (q<10){index_q1="_P00"+index_q;}
      if (q>=10 && i<100){index_q1="_P0"+index_q;}
      if (q>=100 && i<1000){index_q1="_P"+index_q;}   
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Programm  Load the hdf5's root part
   
      pathFile	= pathFile+index_i1+index_j1+"_S0"+index_q1+"_bf.h5";			

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

      BF_StokesDataset STOCKES 		= BEAM.stokes(0);			// generate an object called STOCKES contains stockes data
      
      
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////      
      // Variable iniatialization for DYNSPEC group 
      
      string GROUPE_TYPE_DYN("DYNSPEC");
      
      // function file.expTimeStartMJD().get()) return a double value, so it needs to convert float to string using a ostringstream
      std::ostringstream oss;oss << file.observationStartMJD().get();string DYNSPEC_START_MJD(oss.str());
      oss << file.observationEndMJD().get();string DYNSPEC_STOP_MJD(oss.str());
      
      string DYNSPEC_START_UTC(file.observationStartUTC().get());
      string DYNSPEC_STOP_UTC(file.observationEndUTC().get());
      string DYNSPEC_START_TAI(file.observationStartTAI().get());
      string DYNSPEC_STOP_TAI(file.observationEndTAI().get());
         
      // TARGET_DYN
      string TRACKING("OFF");
      string TARGET_DYN;
      int k;
      if (obsNofSAP == obsNofSource){TARGET_DYN="SRC"+index_i;}
      else {k = i/obsNofFrequencyBand;std::ostringstream oss_k;oss_k << k;string index_k(oss_k.str());
	    TARGET_DYN="SRC"+index_k;}
      
      string ONOFF("OFF");

      double POINT_RA(SAP.pointRA().get());
      double POINT_DEC(SAP.pointDEC().get());
      
      double POSITION_OFFSET_RA(BEAM.pointOffsetRA().get());
      double POSITION_OFFSET_DEC(BEAM.pointOffsetDEC().get());
      double BEAM_DIAMETER_RA(POINT_RA);
      double BEAM_DIAMETER_DEC(POINT_DEC);

 
      double BEAM_NOF_STATIONS(BEAM.nofStations().get());
      vector<string> BEAM_STATIONS_LIST(BEAM.stationsList().get());

      string DEDISPERSION(BEAM.dedispersion().get());
      double DISPERSION_MEASURE(BEAM.dedispersionMeasure().get());
      string DISPERSION_MEASURE_UNIT(BEAM.dedispersionMeasureUnit().get());
      bool BARYCENTER(false);
      vector<string> STOCKES_COMPONENT(BEAM.stokesComponents().get());
      bool COMPLEX_VOLTAGE(BEAM.complexVoltage().get());
      string SIGNAL_SUM(BEAM.signalSum().get());
      
      
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Variable iniatialization for COORDINATES group    
      
      string GROUPE_TYPE_COORD("Coordinates");
      vector<double> REF_LOCATION_VALUE(COORDS.refLocationValue().get());
      vector<string> REF_LOCATION_UNIT(COORDS.refLocationUnit().get());
      string REF_LOCATION_FRAME(COORDS.refLocationFrame().get());
      
      double REF_TIME_VALUE(COORDS.refTimeValue().get());
      string REF_TIME_UNIT(COORDS.refTimeUnit().get());
      string REF_TIME_FRAME(COORDS.refTimeFrame().get());
      double NOF_COORDINATES(3);
      double NOF_AXIS(3);
      vector<string> COORDINATE_TYPES(3);
      COORDINATE_TYPES[0]="TIME_COORD";
      COORDINATE_TYPES[1]="SPECTRAL_COORD";
      COORDINATE_TYPES[2]="POLARIZATION_COORD";


      
      ///////////////////////////////////////////////////////////
      // Variable iniatialization for TIME coordinate  group        
      
      string GROUPE_TYPE_TIME("TimeCoord");
      string COORDINATE_TYPE_TIME("Time");
      vector<string> STORAGE_TYPE_TIME(1); STORAGE_TYPE_TIME[0]="Tabular";     
      int NOF_AXES_TIME(TIME_COORDS->nofAxes().get());      
      vector<string> AXIS_NAMES_TIME(TIME_COORDS->axisNames().get());
      vector<string> AXIS_UNIT_TIME(1);AXIS_UNIT_TIME[0]="s"; 
      
      vector<double> REFERENCE_VALUE_TIME(1);REFERENCE_VALUE_TIME[0]=(NUM_COORD_TIME->referenceValue().get());
      vector<double> REFERENCE_PIXEL_TIME(1);REFERENCE_PIXEL_TIME[0]=(NUM_COORD_TIME->referencePixel().get());
      vector<double> INCREMENT_TIME(1);INCREMENT_TIME[0]=(NUM_COORD_TIME->increment().get());
            
      vector<double> PC_TIME(2);PC_TIME[0]=1;PC_TIME[1]=0;
      
      
      int Ntime=(STOCKES.nofSamples().get());
      
          
      vector<double> AXIS_VALUE_PIXEL_TIME(Ntime);
      vector<double> AXIS_VALUE_WORLD_TIME(Ntime);
      
      double timeBinStep((file.totalIntegrationTime().get())/(STOCKES.nofSamples().get()));      
      int sizeLimit(100E6);
      if (Ntime> sizeLimit)
	{      
	  int p(0);
	  int frac((Ntime/sizeLimit)+1);
	  int lastElements((Ntime)-((frac-1)*sizeLimit));
	  for (p=0;p<frac;p++)
	    {
	      if (p!=frac-1){for (int i=0; i<sizeLimit;i++){AXIS_VALUE_PIXEL_TIME[p*sizeLimit+i]=p*sizeLimit+i;AXIS_VALUE_WORLD_TIME[p*sizeLimit+i]= timeBinStep*(p*sizeLimit+i);}}
	      else{for (int i=0; i<lastElements;i++){AXIS_VALUE_PIXEL_TIME[p*sizeLimit+i]=p*sizeLimit+i;AXIS_VALUE_WORLD_TIME[p*sizeLimit+i]= timeBinStep*(p*sizeLimit+i);}}
	    }
	}	
      else {for (int i=0; i<Ntime;i++){AXIS_VALUE_PIXEL_TIME[i]=i;AXIS_VALUE_WORLD_TIME[i]= timeBinStep*i;}}
     
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

      vector<double> AXIS_VALUE_WORLD_SPECTRAL(NUM_COORD_SPECTRAL->axisValuesWorld().get());      
      int Nspectral=AXIS_VALUE_WORLD_SPECTRAL.size();
      
      vector<double> AXIS_VALUE_PIXEL_SPECTRAL(Nspectral);

           
      if (Nspectral> sizeLimit)
	{      
	  int p(0);
	  int frac((Nspectral/sizeLimit)+1);
	  int lastElements((Nspectral)-((frac-1)*sizeLimit));
	  for (p=0;p<frac;p++)
	    {
	      if (p!=frac-1){for (int i=0; i<sizeLimit;i++){AXIS_VALUE_PIXEL_SPECTRAL[p*sizeLimit+i]=p*sizeLimit+i;}}//AXIS_VALUE_WORLD_SPECTRAL[p*sizeLimit+i]= AXIS_VALUE_WORLD_SPECTRAL[p*sizeLimit+i]/1E6;}}
	      else{for (int i=0; i<lastElements;i++){AXIS_VALUE_PIXEL_SPECTRAL[p*sizeLimit+i]=p*sizeLimit+i;}}//AXIS_VALUE_WORLD_SPECTRAL[p*sizeLimit+i]= AXIS_VALUE_WORLD_SPECTRAL[p*sizeLimit+i]/1E6;}}
	    }
	}	
      else {for (int i=0; i<Nspectral;i++){AXIS_VALUE_PIXEL_SPECTRAL[i]=i;}}// AXIS_VALUE_WORLD_SPECTRAL[i]=AXIS_VALUE_WORLD_SPECTRAL[i]/1E6;}}     
      
		
      int NOF_SUBBANDS=Nspectral/(file.observationNofBitsPerSample().get());
     

      // FREQUENCY INFO
      
      double BEAM_FREQUENCY_CENTER((BEAM.beamFrequencyCenter().get())/1E6);
            
      double BEAM_FREQUENCY_MAX(AXIS_VALUE_WORLD_SPECTRAL[(AXIS_VALUE_WORLD_SPECTRAL.size())-1]);//BEAM_FREQUENCY_CENTER   +   (((SAP.subbandWidth().get()/1E6)   *NOF_SUBBANDS)/2.0)    );
      double BEAM_FREQUENCY_MIN(AXIS_VALUE_WORLD_SPECTRAL[0]);//BEAM_FREQUENCY_CENTER-(((SAP.subbandWidth().get()/1E6)*NOF_SUBBANDS)/2.0));
      
      string BEAM_FREQUENCY_UNIT("Mhz");
      
      double DYNSPEC_BANDWIDTH(BEAM_FREQUENCY_MAX-BEAM_FREQUENCY_MIN);
      double BEAM_DIAMETER(0);      
      
      
      
      
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
      
      
      ///////////////////////////////////////////////////////////////////////////////////////       
				  // END READING DYNSPEC META-DATA  //
      //////////////////////////////////////////////////////////////////////////////////////         
      
      //Object generation => STOCK DYNSPEC METADATA

      dynspecMetadata->stockDynspecMetadata(GROUPE_TYPE_DYN,DYNSPEC_START_MJD,DYNSPEC_STOP_MJD,DYNSPEC_START_UTC,DYNSPEC_STOP_UTC,DYNSPEC_START_TAI,DYNSPEC_STOP_TAI,DYNSPEC_BANDWIDTH,BEAM_DIAMETER,TRACKING,TARGET_DYN,
					   ONOFF,POINT_RA,POINT_DEC,POSITION_OFFSET_RA,POSITION_OFFSET_DEC,BEAM_DIAMETER_RA,BEAM_DIAMETER_DEC,BEAM_FREQUENCY_MAX,BEAM_FREQUENCY_MIN,BEAM_FREQUENCY_CENTER,
					   BEAM_FREQUENCY_UNIT,BEAM_NOF_STATIONS,BEAM_STATIONS_LIST,DEDISPERSION,DISPERSION_MEASURE,DISPERSION_MEASURE_UNIT,BARYCENTER,STOCKES_COMPONENT,COMPLEX_VOLTAGE,SIGNAL_SUM,
					   GROUPE_TYPE_COORD,REF_LOCATION_VALUE,REF_LOCATION_UNIT,REF_LOCATION_FRAME,REF_TIME_VALUE,REF_TIME_UNIT,REF_TIME_FRAME,NOF_COORDINATES,NOF_AXIS,COORDINATE_TYPES,
					   GROUPE_TYPE_TIME,COORDINATE_TYPE_TIME,STORAGE_TYPE_TIME, NOF_AXES_TIME,AXIS_NAMES_TIME,AXIS_UNIT_TIME,REFERENCE_VALUE_TIME,REFERENCE_PIXEL_TIME,INCREMENT_TIME,
					   PC_TIME,AXIS_VALUE_PIXEL_TIME,AXIS_VALUE_WORLD_TIME,GROUPE_TYPE_SPECTRAL,COORDINATE_TYPE_SPECTRAL,STORAGE_TYPE_SPECTRAL,NOF_AXES_SPECTRAL,AXIS_NAMES_SPECTRAL,
					   AXIS_UNIT_SPECTRAL,REFERENCE_VALUE_SPECTRAL,REFERENCE_PIXEL_SPECTRAL,INCREMENT_SPECTRAL,PC_SPECTRAL,AXIS_VALUE_WORLD_SPECTRAL,AXIS_VALUE_PIXEL_SPECTRAL,
					   GROUPE_TYPE_POL,COORDINATE_TYPE_POL,STORAGE_TYPE_POL,NOF_AXES_POL,AXIS_NAMES_POL,AXIS_UNIT_POL,Ntime,Nspectral,GROUPE_TYPE_EVENT,DATASET_EVENT,N_AXIS_EVENT,N_AXIS_1_EVENT,
					   N_AXIS_2_EVENT,N_EVENT,FIELD_1,FIELD_2,FIELD_3,FIELD_4,FIELD_5,FIELD_6,GROUPE_TYPE_PROCESS,OBSERVATION_PARSET,OBSERVATION_LOG,DYNSPEC_PARSET,DYNSPEC_LOG);
					   
					   
      dynspecData->stockDynspecData(Ntime,Nspectral);
      ///////////////////////////////////////////////      
      // Memory desallocation

      delete NUM_COORD_TIME;
      delete DIR_COORD_TIME;
      delete NUM_COORD_SPECTRAL;
      delete DIR_COORD_SPECTRAL;     
      
    }  // end of the Reader_Dynspec object      

      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      