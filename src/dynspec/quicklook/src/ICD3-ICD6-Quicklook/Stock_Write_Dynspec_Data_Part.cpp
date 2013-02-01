#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>

#include "Stock_Write_Dynspec_Data_Part.h"

#include <dal/lofar/BF_File.h>


/// \file Stock_Write_Dynspec_Data_Part.cpp
///  \brief File C++ (associated to Stock_Write_Dynspec_Data_Part.h) for stocking data parameters, processing the (select or rebin) and write data Matrix in the ICD6's Dynspec Groups 
///  \details  
/// <br /> Overview:
/// <br /> Functions stockDynspecData and writeDynspecData are described (programmed) here. 
/// The first one (stockDynspecData) will take as parameter processing parameters and stok them as private attributes. 
/// The second function (writeDynspecData) will write them in the hdf5 output file (in the corresponding dynspec).
/// Data processing is coded here !!


using namespace dal;
using namespace std;


  Stock_Write_Dynspec_Data_Part::Stock_Write_Dynspec_Data_Part(){}
  Stock_Write_Dynspec_Data_Part::~Stock_Write_Dynspec_Data_Part(){}
  
  void Stock_Write_Dynspec_Data_Part::stockDynspecData(int Ntime,int Nspectral,  int NtimeReal, int NspectralReal)
  { 
  /// <br /> Usage:
  /// <br />   void Stock_Write_Dynspec_Data_Part::stockDynspecData(int Ntime,int Nspectral,  int NtimeReal, int NspectralReal)
  /// \param   Ntime  new time binning
  /// \param   Nspectral new spectral (frequency)  binning (number of channels per subbands)
  /// \param   NtimeReal current time binning before rebinning 
  /// \param   NspectralReal current spectral (frequency)  binning (number of channels per subbands) before rebinning 
  
    m_Ntime = Ntime;
    m_Nspectral = Nspectral;
    
    m_NtimeReal = NtimeReal;
    m_NspectralReal =NspectralReal;   
  }

  
   bool is_readable( const std::string & file ) 
  { 
    /// \param file
    /// <br />Check if file is readable, so if file exists !
    /// \return boolean value 

    std::ifstream fichier( file.c_str() ); 
    return !fichier.fail(); 
  } 
  
  
  void Stock_Write_Dynspec_Data_Part::writeDynspecData(Group &dynspec_grp,string obsName,string pathFile,string outputFile,File &root_grp,int i,int j,int k,int l,int q,int obsNofStockes,vector<string> stokesComponent, int SAPindex,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin, float dataTimePercent, float dataSpectralPercent)  
  {
  /// <br /> Usage:
  /// <br />   void Stock_Write_Dynspec_Data_Part::writeDynspecData(Group &dynspec_grp,string pathDir,string obsName,string pathFile,string outputFile,File &root_grp,int i,int j,int k,int l,int q,int obsNofStockes,vector<string> stokesComponent,float memoryRAM, int SAPindex,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin)  
  /// \param  &dynspec_grp Group Object(Dynamic spectrum object)
  /// \param  pathDir ICD3 path directory
  /// \param  obsName Observation ID
  /// \param  pathFile ICD3 path for loading file
  /// \param  outputFile output file 
  /// \param  &root_grp File Object (Root Group object)
  /// \param  i Subarray pointing loop index
  /// \param  j Beams loop index
  /// \param  k loop index
  /// \param  l loop index
  /// \param  q Pxx nomenclature loop index
  /// \param  obsNofStockes number of Stokes components
  /// \param  stokesComponent vector which contains all Stokes components
  /// \param  memoryRAM RAM memory consuption by processing
  /// \param  SAPindex Subarray pointings to process
  /// \param  timeMinSelect time minimum selected
  /// \param  timeMaxSelect time maximum selected
  /// \param  timeRebin time binning
  /// \param  frequencyMin frequency minimum selected
  /// \param  frequencyMax frequency maximum selected
  /// \param  frequencyRebin frequency binning


	
	    int pathFileSize(pathFile.length());			// to be sure  
	    string pathFileTempraw(pathFile.substr(0,pathFileSize-2));
	    string pathFileraw(pathFileTempraw+"raw");
	    
	    if ( is_readable(pathFileraw))
	    {
      
	    ////////////////////////////////////////////////////////////////
	    // Redefine ObsStokes for particluar Cases
	    
	    BF_File file(pathFile);  							// generate an object called file contains Root
	    BF_SubArrayPointing SAP 		= file.subArrayPointing(i);		// generate an object called SAP contains subarray pointings
	    BF_BeamGroup BEAM			= SAP.beam(j);      			// generate an object called BEAM contains Beams 
		

	    BF_StokesDataset STOCKES 		= BEAM.stokes(0);			// generate an object called STOCKES contains stockes data  
	  

	    CoordinatesGroup COORDS		= BEAM.coordinates();      		// generate an object called COORDS contains Coodinates directory
	    Coordinate *SPECTRAL_COORDS		= COORDS.coordinate(1);			// generate an object called SPECTRAL_COORDS contains spectral data      
				
	    NumericalCoordinate *NUM_COORD_SPECTRAL = dynamic_cast<NumericalCoordinate*>(SPECTRAL_COORDS); // load Numerical coordinates lib 	  
		     
	    BF_File fileTEMP(pathFile);  								// generate an object called file contains Root
	    BF_SubArrayPointing SAPTEMP 		= fileTEMP.subArrayPointing(i);		// generate an object called SAP contains subarray pointings
	    BF_BeamGroup BEAMTEMP			= SAPTEMP.beam(j);
	    

	    obsNofStockes = 1;      
	    if (obsNofStockes == 1){stokesComponent.resize(1);stokesComponent[0]='I';}  
	    

	    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    // Define Time and spectral for rebinning 

	    // Define Time start and stop indexes

	    float timeIcrementReal(1/(BEAM.nofSamples().get()/file.totalIntegrationTime().get())) ;

		
	    unsigned long int timeIndexIncrementRebin(timeRebin/timeIcrementReal);
	    timeRebin = timeIndexIncrementRebin*(1/(BEAM.nofSamples().get()/file.totalIntegrationTime().get()));

	    
	    unsigned long int timeIndexStart(timeMinSelect/timeIcrementReal);
	    timeIndexStart = (timeIndexStart/timeIndexIncrementRebin)*timeIndexIncrementRebin;
	    timeMinSelect  = timeIndexStart*timeIcrementReal;
	    
	    unsigned long int timeIndexStop(timeMaxSelect/timeIcrementReal);
	    timeIndexStop = timeIndexStop/timeIndexIncrementRebin*timeIndexIncrementRebin;
	    timeMaxSelect = timeIndexStop*timeIcrementReal;
	    
	    
	    
	    // Time parameters for the quicklook
	    int nofTimeToSumEverySecond	(BEAM.nofSamples().get()/file.totalIntegrationTime().get()*dataTimePercent);
	    if (nofTimeToSumEverySecond==0){nofTimeToSumEverySecond++;}
	    int stepForOneSecond(BEAM.nofSamples().get()/file.totalIntegrationTime().get());

	    
	    
	    // Define Spectral start and stop indexes
	    vector<double> AXIS_VALUE_WORLD_SPECTRAL_TEMP(NUM_COORD_SPECTRAL->axisValuesWorld().get());      
      	    unsigned long int NOF_SUBBANDS_TEMP= (AXIS_VALUE_WORLD_SPECTRAL_TEMP.size())/(BEAM.channelsPerSubband().get());
	    unsigned long int CHANNELS_PER_SUBANDS_TEMP=(BEAM.channelsPerSubband().get());
	    
	    // Find the Number of Subband for beginning the processing (min and max subbands indexes)
	    unsigned long int index_fmin(0);
	    float start_min(AXIS_VALUE_WORLD_SPECTRAL_TEMP[index_fmin]);    
	    while (start_min < frequencyMin*1E6)
	    {	  index_fmin 	= index_fmin +1;
		start_min 	= AXIS_VALUE_WORLD_SPECTRAL_TEMP[index_fmin*CHANNELS_PER_SUBANDS_TEMP];}
	
	    unsigned long int index_fmax((AXIS_VALUE_WORLD_SPECTRAL_TEMP.size()/BEAM.channelsPerSubband().get())-1); 
	    float start_max(AXIS_VALUE_WORLD_SPECTRAL_TEMP[AXIS_VALUE_WORLD_SPECTRAL_TEMP.size()-1]);
	    while (start_max > frequencyMax*1E6)
	    {	  index_fmax 	= index_fmax -1;
		start_max 	= AXIS_VALUE_WORLD_SPECTRAL_TEMP[index_fmax*CHANNELS_PER_SUBANDS_TEMP+CHANNELS_PER_SUBANDS_TEMP-1];}	  
		
	    // Avoid border effect about the frequency selection
	    for (unsigned long int k=0; k < NOF_SUBBANDS_TEMP;k++)
	      {
		float miniFreq(AXIS_VALUE_WORLD_SPECTRAL_TEMP[k*CHANNELS_PER_SUBANDS_TEMP]);
		float maxiFreq(AXIS_VALUE_WORLD_SPECTRAL_TEMP[k*CHANNELS_PER_SUBANDS_TEMP+CHANNELS_PER_SUBANDS_TEMP-1]);
		if (frequencyMin*1E6 < maxiFreq && frequencyMin*1E6 > miniFreq){index_fmin = index_fmin-1; start_min = AXIS_VALUE_WORLD_SPECTRAL_TEMP[index_fmin*CHANNELS_PER_SUBANDS_TEMP];}
		if (frequencyMax*1E6 < maxiFreq && frequencyMax*1E6 > miniFreq){index_fmax 	= index_fmax+1;start_max = AXIS_VALUE_WORLD_SPECTRAL_TEMP[index_fmax*CHANNELS_PER_SUBANDS_TEMP+CHANNELS_PER_SUBANDS_TEMP-1];}
	      }
	    
	    unsigned long int NOF_SUBBANDS=index_fmax-index_fmin+1;
	    if (NOF_SUBBANDS == 0){NOF_SUBBANDS++;}
	    
	    

	    // Spectral parameters for the quicklook
	    int nofSpectralToSumEverySubband(CHANNELS_PER_SUBANDS_TEMP*dataSpectralPercent);
	    if  (nofSpectralToSumEverySubband == 0){nofSpectralToSumEverySubband++;}
	    int middlesubband(CHANNELS_PER_SUBANDS_TEMP/2);if (middlesubband ==0){middlesubband++;}

	    //cout << nofTimeToSumEverySecond  << "  " << stepForOneSecond  << endl;
	    //cout <<  nofSpectralToSumEverySubband << "  "  << middlesubband  << endl;
	    
	    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    // define the time step for filling the dataset
	    
	    int p(0);


      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      //WRITE THE DATA Matrix
            
      // generating the dataset
      
      Dataset<float> data_grp(dynspec_grp, "DATA");
      vector<ssize_t> dimensions(3);
      dimensions[0] = m_Ntime;
      dimensions[1] = m_Nspectral;
      dimensions[2] = obsNofStockes;      
      data_grp.create( dimensions );
      
     
      
      // generating the quick look

      vector<float> DATA_3D(m_Ntime*m_Nspectral*obsNofStockes);
      vector<float> DATA_2D(nofTimeToSumEverySecond*CHANNELS_PER_SUBANDS_TEMP*NOF_SUBBANDS_TEMP);
      
      
      for (p=0;p<m_Ntime;p++)
	     {	      
	      
	      BF_File file(pathFile);  							// generate an object called file contains Root
	      BF_SubArrayPointing SAP 		= file.subArrayPointing(i);		// generate an object called SAP contains subarray pointings
	      BF_BeamGroup BEAM			= SAP.beam(j);      	       
		
 	      BF_StokesDataset STOCKES 	= BEAM.stokes(0);	// generate an object called STOCKES contains stockes data  
// 
// 	       
	      vector<size_t> pos(2);
	      pos[0]=p*stepForOneSecond;
	      pos[1]=0;	       
	       
	       
	      vector<size_t> size(2);
	      size[0] = nofTimeToSumEverySecond;
	      size[1] = CHANNELS_PER_SUBANDS_TEMP*NOF_SUBBANDS_TEMP;
	
		
	      STOCKES.getMatrix( pos, &DATA_2D[0], size );
	      
	      float rebinPixel(0);
	      for (int J=0;J<m_Nspectral;J++)
		  {
		    rebinPixel=0;
		    
		    for (int K=0;K<nofTimeToSumEverySecond;K++)
		      {
			  for (int L=0;L<nofSpectralToSumEverySubband;L++)
			      {
				rebinPixel = rebinPixel+DATA_2D[(middlesubband-nofSpectralToSumEverySubband/2)-1+L+K*CHANNELS_PER_SUBANDS_TEMP*NOF_SUBBANDS_TEMP+J*CHANNELS_PER_SUBANDS_TEMP];
 		      		//cout << (middlesubband-nofSpectralToSumEverySubband/2)-1+L+K*CHANNELS_PER_SUBANDS_TEMP*NOF_SUBBANDS_TEMP+J*CHANNELS_PER_SUBANDS_TEMP << endl;
			      }
		      }
		      DATA_3D[p*m_Nspectral+J]=rebinPixel/(nofTimeToSumEverySecond*nofSpectralToSumEverySubband);	      
		  }
	     }
	     
	     vector<size_t> data_grp_size(3);
	     vector<size_t> data_grp_pos(3);

	     data_grp_pos[0] = 0;
	     data_grp_pos[1] = 0;
	     data_grp_pos[2] = 0;   	  
		
	     data_grp_size[0] = m_Ntime;
	     data_grp_size[1] = m_Nspectral;
	     data_grp_size[2] = 1;
		  

	     data_grp.setMatrix( data_grp_pos, &DATA_3D[0], data_grp_size );	     
	     

	    //META-DATA in DATA  writter
	      
	    string GROUPE_TYPE_DATA("Data");
	    string WCSINFO("/Coordinates");
	    unsigned long int DATASET_NOF_AXIS(3);
	    vector<unsigned long int> DATASET_SHAPE(1);DATASET_SHAPE[0];
		      
	    Attribute<string> attr_143(data_grp, "GROUPE_TYPE");
	    Attribute<string> attr_144 (data_grp, "WCSINFO");
	    Attribute<unsigned long int> attr_145(data_grp, "DATASET_NOF_AXIS");
	    Attribute< vector<unsigned long int> > attr_146(data_grp, "DATASET_SHAPE");
		
	    attr_143.value = GROUPE_TYPE_DATA;
	    attr_144.value = WCSINFO;
	    attr_145.value = DATASET_NOF_AXIS;
	    attr_146.create().set(DATASET_SHAPE);      
	    
	      
	    }
	    else  // if .raw don't exists
	    {  
	        Dataset<float> data_grp(dynspec_grp, "DATA");
		vector<ssize_t> dimensions(3);
		dimensions[0] = m_Ntime;
		dimensions[1] = m_Nspectral;
		dimensions[2] = obsNofStockes;      
		data_grp.create( dimensions );
	    }
	    
	   	    
	    
  }
