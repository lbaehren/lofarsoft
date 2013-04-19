#include <iostream>
#include <string>
#include <fstream>

#include "Stock_Write_Dynspec_Data_Partionned.h"

#include <dal/lofar/BF_File.h>


/// \file Stock_Write_Dynspec_Data_Partionned.cpp
///  \brief File C++ (associated to Stock_Write_Dynspec_Data_Partionned.h) for stocking data parameters, processing the (select or rebin) and write data Matrix in the ICD6's Dynspec Groups 
///  \details  
/// <br /> Overview:
/// <br /> Functions stockDynspecData and writeDynspecData are described (programmed) here. 
/// The first one (stockDynspecData) will take as parameter processing parameters and stok them as private attributes. 
/// The second function (writeDynspecData) will write them in the hdf5 output file (in the corresponding dynspec).
/// Data processing is coded here !!


using namespace dal;
using namespace std;


  Stock_Write_Dynspec_Data_Partionned::Stock_Write_Dynspec_Data_Partionned(){}
  Stock_Write_Dynspec_Data_Partionned::~Stock_Write_Dynspec_Data_Partionned(){}
  
  void Stock_Write_Dynspec_Data_Partionned::stockDynspecDataPartionned(int Ntime,int Nspectral,  int NtimeReal, int NspectralReal)
  { 
  /// <br /> Usage:
  /// <br />   void Stock_Write_Dynspec_Data_Partionned::stockDynspecData(int Ntime,int Nspectral,  int NtimeReal, int NspectralReal)
  /// \param   Ntime  new time binning
  /// \param   Nspectral new spectral (frequency)  binning (number of channels per subbands)
  /// \param   NtimeReal current time binning before rebinning 
  /// \param   NspectralReal current spectral (frequency)  binning (number of channels per subbands) before rebinning 
  
    m_Ntime = Ntime;
    m_Nspectral = Nspectral;
    
    m_NtimeReal = NtimeReal;
    m_NspectralReal =NspectralReal;   
  }

  
   bool is_readablePartionned( const std::string & file ) 
  { 
    /// \param file
    /// <br />Check if file is readable, so if file exists !
    /// \return boolean value 

    std::ifstream fichier( file.c_str() ); 
    return !fichier.fail(); 
  } 
  
  
  void Stock_Write_Dynspec_Data_Partionned::writeDynspecDataPartionned(Group &dynspec_grp,string obsName,string pathFile,string outputFile,File &root_grp,int i,int j,int k,int l,int q,int obsNofStockes,vector<string> stokesComponent,float memoryRAM, int SAPindex,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin, vector<string> listOfFiles, int m, int obsNofFrequencyBand)
  {
  /// <br /> Usage:
  /// <br />   void Stock_Write_Dynspec_Data_Partionned::writeDynspecData(Group &dynspec_grp,string pathDir,string obsName,string pathFile,string outputFile,File &root_grp,int i,int j,int k,int l,int q,int obsNofStockes,vector<string> stokesComponent,float memoryRAM, int SAPindex,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin)  
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

      int mindex(m);

      //Go for Writting, but before test if files exists !!	    

      int pathFileSize(pathFile.length());			// to be sure  
      string pathFile0(pathFile.substr(0,pathFileSize-26)); 
      
      string index_i1,index_j1,index_k1,index_q1;
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
      
      
      std::ostringstream oss_k;oss_k << k;string index_k(oss_k.str());
      if (k<10){index_k1="00"+index_k;}
      if (k>=10 && k<100){index_k1="0"+index_k;}
      if (k>=100 && k<1000){index_k1=index_k;}
      
  

      pathFile		= pathFile0+index_i1+index_j1+"_S0"+index_q1+"_bf.h5"; 
      string pathRaw 	= pathFile0+index_i1+index_j1+"_S0"+index_q1+"_bf.raw"; 

	
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
	    

	    obsNofStockes = BEAMTEMP.observationNofStokes().get();      
	    if (obsNofStockes == 1){stokesComponent.resize(1);stokesComponent[0]='I';}
	    if (obsNofStockes == 2){stokesComponent.resize(2);stokesComponent[0]='I';stokesComponent[1]='Q';}
	    if (obsNofStockes == 3){stokesComponent.resize(3);stokesComponent[0]='I';stokesComponent[1]='Q';stokesComponent[2]='U';}
	    if (obsNofStockes == 4){stokesComponent.resize(4);stokesComponent[0]='I';stokesComponent[1]='Q';stokesComponent[2]='U';stokesComponent[3]='V';}      
	    

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
	    
  
	    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    // Generate New Rebinned and selected AXIS_VALUE_WORLD_SPECTRAL
	    
	    
	    vector<double> AXIS_VALUE_WORLD_SPECTRAL_TEMP(NUM_COORD_SPECTRAL->axisValuesWorld().get());      
	    int AXIS_VALUE_WORLD_SPECTRAL_TEMP_SIZE(AXIS_VALUE_WORLD_SPECTRAL_TEMP.size()*obsNofFrequencyBand);  
	    
	    int NOF_SUBBANDS_TEMP= (NUM_COORD_SPECTRAL->axisValuesWorld().get().size()/BEAM.channelsPerSubband().get());
	    int CHANNELS_PER_SUBANDS_TEMP=(BEAM.channelsPerSubband().get());
	    AXIS_VALUE_WORLD_SPECTRAL_TEMP.resize(AXIS_VALUE_WORLD_SPECTRAL_TEMP_SIZE);
	    
	    for (q=0;q<obsNofFrequencyBand;q++)
		  {      
		      string pathFile_Partionned(listOfFiles[m*obsNofStockes*obsNofFrequencyBand+q]);

		      BF_File file_Partionned(pathFile_Partionned );				 						// generate an object called file contains Root
		      BF_SubArrayPointing SAP_Partionned  		= file_Partionned.subArrayPointing(i);					// generate an object called SAP contains subarray pointings
		      BF_BeamGroup BEAM_Partionned			= SAP_Partionned.beam(j);      						// generate an object called BEAM contains Beams
		      CoordinatesGroup COORDS_Partionned		= BEAM_Partionned.coordinates();      					// generate an object called COORDS contains Coodinates directory
		      Coordinate *SPECTRAL_COORDS_Partionned		= COORDS_Partionned.coordinate(1);					// generate an object called SPECTRAL_COORDS contains spectral data
		      NumericalCoordinate *NUM_COORD_SPECTRAL_Partionned = dynamic_cast<NumericalCoordinate*>(SPECTRAL_COORDS_Partionned); 	// load Numerical coordinates lib 
		      //DirectionCoordinate *DIR_COORD_SPECTRAL_Partionned = dynamic_cast<DirectionCoordinate*>(SPECTRAL_COORDS_Partionned); 	// load Direction coordinates lib
		      
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
	    
	    unsigned long int spectralIndexStart(index_fmin*CHANNELS_PER_SUBANDS_TEMP);
	    unsigned long int spectralIndexStop(index_fmax*CHANNELS_PER_SUBANDS_TEMP+CHANNELS_PER_SUBANDS_TEMP-1);
	    unsigned long int spectralIndexIncrementRebin((spectralIndexStop-spectralIndexStart+1)/Nspectral);
	    
		
	    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    // define the time step for filling the dataset
	    unsigned long int p(0),n(0),r(0);

	    unsigned long int sizeTimeLimit((1.08E6*memoryRAM)/((spectralIndexStop-spectralIndexStart+1)*obsNofStockes));	// time step ~ 1Go RAM memory maximum !        
	    
	    // SizeTimeLimit must be a multiple of Time Rebin for well cycling and avoid to lost data
	    unsigned long int timeFactor(sizeTimeLimit/timeIndexIncrementRebin);
	    if (timeFactor == 0){timeFactor=1;}
	    
	    sizeTimeLimit = timeFactor*timeIndexIncrementRebin;
	    
	    // Determine with frac time the number of loop needed for splited data needed to be rebin etc... by blocks
	    unsigned long int fracTime((timeIndexStop-timeIndexStart)/sizeTimeLimit+1);         
	    unsigned long int nofLastElements((timeIndexStop-timeIndexStart)-((fracTime-1)*sizeTimeLimit));
	    
	    // nofLastElements must be a multiple of Time Rebin for well cycling and avoid to lost data     
	    unsigned long int nofLastElementsFactor(nofLastElements/timeIndexIncrementRebin);
	    nofLastElements = nofLastElementsFactor*timeIndexIncrementRebin;
	    	
      
	    

      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      //WRITE THE DATA Matrix
            
      // generating the dataset

      unsigned long int m_Nspectral_longversion(m_Nspectral);
      
      Dataset<float> data_grp(dynspec_grp, "DATA");
      vector<ssize_t> dimensions(3);
      dimensions[0] = m_Ntime;
      dimensions[1] = m_Nspectral;
      dimensions[2] = obsNofStockes;      
      data_grp.create( dimensions );
	
      
	    for (p=0;p<fracTime;p++)
	      {


	      vector<float> DATA_3D(sizeTimeLimit*m_Nspectral*obsNofStockes/timeIndexIncrementRebin);
	      if (p==fracTime-1){if (nofLastElements > 0){DATA_3D.resize(nofLastElements*m_Nspectral*obsNofStockes/timeIndexIncrementRebin);}}	    
 	      

	      for (l=0;l<obsNofStockes;l++)
		{
		    
		  int qiterator(0);
		  int squareAtStart(0);
		    
		  vector<float> DATA_2D(sizeTimeLimit*(spectralIndexStop-spectralIndexStart+1));
		    vector<float> DATA2DRebin(sizeTimeLimit/timeIndexIncrementRebin*m_Nspectral);
		    
		    
		    // Initialization of DATA2D Matrixes to 0 
		    for (r=0;r<sizeTimeLimit*(spectralIndexStop-spectralIndexStart+1);r++){DATA_2D[r]=0;}
		    for (r=0;r<sizeTimeLimit/timeIndexIncrementRebin*m_Nspectral;r++){DATA2DRebin[r]=0;}
		    
		    if (p==fracTime-1){if (nofLastElements > 0){DATA_2D.resize(nofLastElements * m_NspectralReal);}}
		    
				
		    // reading the different Part Pxxx and store them in the matrix !!
		    for (q=0;q<obsNofFrequencyBand;q++)
			  {    
			      int qbreak(0);
			    
			      pathFile 	= listOfFiles[mindex*obsNofStockes*obsNofFrequencyBand+q+l*obsNofFrequencyBand];			      
			      				      
			      pathFileSize=pathFile.length();
			      int obsNameSize(obsName.length());
			      string pathDir(pathFile.substr(0,pathFileSize-26-obsNameSize));
			      
			    
			      string index_i1,index_j1,index_l1;	    
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
			    
			      std::ostringstream oss_l;oss_l << l;string index_l(oss_l.str());
			      index_l1 = "_S"+index_l+index_q1+"_bf.h5";
			  

			      //Load Stokes data		    
			      pathFile = pathDir+obsName+index_i1+index_j1+index_l1;
			      pathRaw =  pathDir+obsName+index_i1+index_j1+"_S"+index_l+index_q1+"_bf.raw";	
			      
			      
			      int tempmin(q*NOF_SUBBANDS_TEMP);
			      int tempmax(q*NOF_SUBBANDS_TEMP+NOF_SUBBANDS_TEMP-1);
			      
			      
			      // Definition of data Matrix 2d Part
			      vector<float> DATA2DPart(sizeTimeLimit*NOF_SUBBANDS_TEMP*CHANNELS_PER_SUBANDS_TEMP);
			      if (p==fracTime-1){if (nofLastElements > 0){DATA2DPart.resize(nofLastElements*NOF_SUBBANDS_TEMP*CHANNELS_PER_SUBANDS_TEMP);}}
			    
			      
			      
			      if ((tempmax < index_fmin) && (tempmin < index_fmin))
				{
				 qbreak++;
				}
				 
			      
			      if ((tempmin > index_fmax) && (tempmax > index_fmax))
				{
				 qbreak++;
				}
				 				
			      if (qbreak ==0)
			      {
				    
				    if (is_readablePartionned(pathRaw)) 
				      { 	
										
					BF_File file(pathFile);  							// generate an object called file contains Root
					BF_SubArrayPointing SAP 		= file.subArrayPointing(i);		// generate an object called SAP contains subarray pointings
					BF_BeamGroup BEAM			= SAP.beam(j);      			// generate an object called BEAM contains Beams				
					BF_StokesDataset STOCKES 		= BEAM.stokes(l);					
				      
					
					
					////////////////////////////////////////////////////					
					// if we take all data from the Partxxx
					if ((tempmin >= index_fmin) && (tempmax <= index_fmax))
					  {
					    
					    //Load 2D Stockes data in a 2D array DATA0 with DAL and stock data in DATA3D for writting (by time step)     					       
					    vector<size_t> pos(2);
					    pos[0]=p*sizeTimeLimit;
					    pos[1]=0;
					    
					    vector<size_t> size(2);
					    size[0] = sizeTimeLimit;	
					    if (p==fracTime-1){if (nofLastElements > 0){size[0] = nofLastElements;}}				    					    
					    size[1] = (NOF_SUBBANDS_TEMP*CHANNELS_PER_SUBANDS_TEMP);					
					    
					    STOCKES.getMatrix( pos, &DATA2DPart[0], size );
					    
					    
					    if (p==fracTime-1)
					      {
						if (nofLastElements > 0)
						  {				    
						    for (unsigned long int y=0; y<nofLastElements;y++)							      
						      {
							for (int z=0; z<NOF_SUBBANDS_TEMP*CHANNELS_PER_SUBANDS_TEMP;z++)
							  {
							    DATA_2D[(qiterator*NOF_SUBBANDS_TEMP*CHANNELS_PER_SUBANDS_TEMP)+squareAtStart+z+(spectralIndexStop-spectralIndexStart+1)*y] = DATA2DPart[y*(NOF_SUBBANDS_TEMP*CHANNELS_PER_SUBANDS_TEMP-1)+z];	    
							    
							  }
						      }
						  }
					      }
					      else
					      {
						
						    for (unsigned long int y=0; y<sizeTimeLimit;y++)					      						      
						      {
							for (int z=0; z<NOF_SUBBANDS_TEMP*CHANNELS_PER_SUBANDS_TEMP;z++)
							  {

							    
							    DATA_2D[(qiterator*NOF_SUBBANDS_TEMP*CHANNELS_PER_SUBANDS_TEMP)+squareAtStart+z+(spectralIndexStop-spectralIndexStart+1)*y] = DATA2DPart[y*(NOF_SUBBANDS_TEMP*CHANNELS_PER_SUBANDS_TEMP-1)+z];	    
							    
							  }
						      }
					      }					    
					   
					      qiterator++;
					      
					  } //end if tempmin >index_fmin && tempmax<index_fmax
					  
					  
					  
					  
					//////////////////////////////////////////////////// 
					// If take only the last portion of the Partxxx ( frequency minimum is in the middle of the Partxxx)
					if ((tempmin < index_fmin) && (tempmax > index_fmin))
					  {
					    					    
					    squareAtStart = ((tempmax-index_fmin+1)*CHANNELS_PER_SUBANDS_TEMP);
					    
					    
					    DATA2DPart.resize(sizeTimeLimit*(tempmax-index_fmin+1)*CHANNELS_PER_SUBANDS_TEMP);
					    if (p==fracTime-1){if (nofLastElements > 0){DATA2DPart.resize(nofLastElements*(tempmax-index_fmin+1)*CHANNELS_PER_SUBANDS_TEMP);}}
					    
					    
					    vector<size_t> pos(2);
					    pos[0]=p*sizeTimeLimit;
					    pos[1]=(index_fmin-tempmin)*CHANNELS_PER_SUBANDS_TEMP;	
					    
					    vector<size_t> size(2);
					    size[0] = sizeTimeLimit;
					    if (p==fracTime-1){if (nofLastElements > 0){size[0] = nofLastElements;}}
					    size[1] = (tempmax-index_fmin+1)*CHANNELS_PER_SUBANDS_TEMP;					
					    
					    STOCKES.getMatrix( pos, &DATA2DPart[0], size );
					    
					    
					    if (p==fracTime-1)
					      {
						if (nofLastElements > 0)
						  {				    
						    for (unsigned long int y=0; y<nofLastElements;y++)							      
						      {
							for (int z=0; z<(tempmax-index_fmin+1)*CHANNELS_PER_SUBANDS_TEMP;z++)
							  {
							    DATA_2D[z+(spectralIndexStop-spectralIndexStart+1)*y] = DATA2DPart[y*((tempmax-index_fmin)*CHANNELS_PER_SUBANDS_TEMP-1)+z];	    
							  }
						      }
						  }
					      }
					      else
					      {
						    for (unsigned long int y=0; y<sizeTimeLimit-1;y++)					      						      
						      {
							for (int z=0; z<(tempmax-index_fmin+1)*CHANNELS_PER_SUBANDS_TEMP-1;z++)
							  {

							    DATA_2D[z+(spectralIndexStop-spectralIndexStart+1)*y] = DATA2DPart[y*((tempmax-index_fmin)*CHANNELS_PER_SUBANDS_TEMP-1)+z];
							  }
						      }
					      }
						    
						    
						    
					  } //end if tempmin >index_fmin && tempmax<index_fmax					    
					    
					 
					 
					 
					////////////////////////////////////////////////////					 
					// If take only the first on of the Partxxx ( frequency maximum is in the middle of the Partxxx)
					if ((tempmin > index_fmin) && (tempmax > index_fmax))
					  {					  
					    DATA2DPart.resize(sizeTimeLimit*(index_fmax-tempmin+1)*CHANNELS_PER_SUBANDS_TEMP);
					    if (p==fracTime-1){if (nofLastElements > 0){DATA2DPart.resize(nofLastElements*(index_fmax-tempmin+1)*CHANNELS_PER_SUBANDS_TEMP);}}
					    					    
					    vector<size_t> pos(2);
					    pos[0]=p*sizeTimeLimit;
					    pos[1]=0;	
					    
					    vector<size_t> size(2);
					    size[0] = sizeTimeLimit;
					    if (p==fracTime-1){if (nofLastElements > 0){size[0] = nofLastElements;}}
					    size[1] = (index_fmax-tempmin+1)*CHANNELS_PER_SUBANDS_TEMP;					
					    
					    STOCKES.getMatrix( pos, &DATA2DPart[0], size );
					    
					    
					    
					    if (p==fracTime-1)
					      {
						if (nofLastElements > 0)
						  {				    
						    for (unsigned long int y=0; y<nofLastElements;y++)							      
						      {
							for (int z=0; z<(index_fmax-tempmin+1)*CHANNELS_PER_SUBANDS_TEMP;z++)
							  {
							    
							    DATA_2D[(qiterator*NOF_SUBBANDS_TEMP*CHANNELS_PER_SUBANDS_TEMP)+squareAtStart+z+(spectralIndexStop-spectralIndexStart+1)*y] = DATA2DPart[y*(index_fmax-tempmin)*CHANNELS_PER_SUBANDS_TEMP-1+z];	    
							    
							  }
						      }
						  }
					      }
					      else
					      {
						    for (unsigned long int y=0; y<sizeTimeLimit;y++)					      						      
						      {
							for (int z=0; z<(index_fmax-tempmin+1)*CHANNELS_PER_SUBANDS_TEMP;z++)
							  {
							    DATA_2D[(qiterator*NOF_SUBBANDS_TEMP*CHANNELS_PER_SUBANDS_TEMP)+squareAtStart+z+(spectralIndexStop-spectralIndexStart+1)*y] = DATA2DPart[y*(index_fmax-tempmin)*CHANNELS_PER_SUBANDS_TEMP-1+z];	    
							    							    
							  }
						      }
					      }						    

					      
					      
					  } //end if tempmin >index_fmin && tempmax<index_fmax					    
					   					 
					   
					
					
					
				    } // end of if existing test
				    else
				    {
				    Group process_histo_grp(dynspec_grp, "PROCESS_HISTORY");
				    Attribute<string> missedData(process_histo_grp, "MISSED DATA");
				    missedData.value = stokesComponent[l];
				    cout << "RAW DATA are missing for SAP: " << i  << "  BEAM: " << j << " Stokes: " << l << " and Part: " << q << endl;
				    cout << "  " << endl;       
				    }// end of else existing test				      
			      
			      
			      } // end of if qbreak =0

			      
			  } //end of loop on q 
		

			    if (p!=fracTime-1)
			      {  

				for (unsigned long int I=0;I<sizeTimeLimit/timeIndexIncrementRebin;I++)
				  {
				    for (unsigned long int J=0;J<m_Nspectral_longversion;J++)
				      { 
					float rebinPixel = 0;
					for (unsigned long int K=0;K<timeIndexIncrementRebin;K++)
					  {
					    for (unsigned long int L=0;L<spectralIndexIncrementRebin;L++)
					      {			
						  
						rebinPixel = rebinPixel+DATA_2D[L+K*(spectralIndexStop-spectralIndexStart+1)+J*spectralIndexIncrementRebin+I*timeIndexIncrementRebin*(spectralIndexStop-spectralIndexStart+1)];
					      }
					  }			    
					DATA2DRebin[I*m_Nspectral+J] = rebinPixel/(timeIndexIncrementRebin*spectralIndexIncrementRebin);
				      }
				  }
				

				for (r=0;r<sizeTimeLimit/timeIndexIncrementRebin;r++)
				  {
				    for (n=0; n<m_Nspectral_longversion;n++)
				      {		  	
				       	if (l==0){DATA_3D[obsNofStockes*((m_Nspectral*r)+n)]=DATA2DRebin[(r*m_Nspectral)+n];}
					if (l==1){DATA_3D[obsNofStockes*((m_Nspectral*r)+n)+1]=DATA2DRebin[(r*m_Nspectral)+n];}
					if (l==2){DATA_3D[obsNofStockes*((m_Nspectral*r)+n)+2]=DATA2DRebin[(r*m_Nspectral)+n];}
					if (l==3){DATA_3D[obsNofStockes*((m_Nspectral*r)+n)+3]=DATA2DRebin[(r*m_Nspectral)+n];}
					       
				      }	  
				  }

			      }
 			      
			    else
			      {
				if (nofLastElements > 0)
				  {		  
				    				    
				    for (unsigned long int I=0;I<nofLastElements/timeIndexIncrementRebin;I++)
				      {
					for (unsigned long int J=0;J<m_Nspectral_longversion;J++)
					  { 
					    float rebinPixel = 0;
					    for (unsigned long int K=0;K<timeIndexIncrementRebin;K++)
					      {
						for (unsigned long int L=0;L<spectralIndexIncrementRebin;L++)
						  {				  
						    rebinPixel = rebinPixel+DATA_2D[L+K*(spectralIndexStop-spectralIndexStart+1)+J*spectralIndexIncrementRebin+I*timeIndexIncrementRebin*(spectralIndexStop-spectralIndexStart+1)];
						  }
					      }			    
					    DATA2DRebin[I*m_Nspectral+J] = rebinPixel/(timeIndexIncrementRebin*spectralIndexIncrementRebin);
					  }
				      }		      
    				     for (r=0;r<nofLastElements/timeIndexIncrementRebin;r++)
    				  	{
    				    	   for (n=0; n<m_Nspectral_longversion;n++)
    				      	     {		  			  
						  if (l==0){DATA_3D[obsNofStockes*((m_Nspectral*r)+n)]=DATA2DRebin[(r*m_Nspectral)+n];}
						  if (l==1){DATA_3D[obsNofStockes*((m_Nspectral*r)+n)+1]=DATA2DRebin[(r*m_Nspectral)+n];}
						  if (l==2){DATA_3D[obsNofStockes*((m_Nspectral*r)+n)+2]=DATA2DRebin[(r*m_Nspectral)+n];}
						  if (l==3){DATA_3D[obsNofStockes*((m_Nspectral*r)+n)+3]=DATA2DRebin[(r*m_Nspectral)+n];}
				      	     }	  
    				  	}
				  }
				  
				  
			      } // end loop of else
 

			  		
		}  //end loop on l (Stockes)// data are loaded in 3D array
				
			  

		//Write data by time steps	
		if (p!=fracTime-1)
		  {	  
		    
		    vector<size_t> data_grp_size(3);
		    vector<size_t> data_grp_pos(3);

		    data_grp_pos[0] = p*sizeTimeLimit/timeIndexIncrementRebin;
		    data_grp_pos[1] = 0;
		    data_grp_pos[2] = 0;   	  
		
		    data_grp_size[0] = sizeTimeLimit/timeIndexIncrementRebin;
		    data_grp_size[1] = m_Nspectral;
		    data_grp_size[2] = obsNofStockes;
		  

		    data_grp.setMatrix( data_grp_pos, &DATA_3D[0], data_grp_size );

		  }
		else
		  {	
		    if (nofLastElements > 0)
		      {
			vector<size_t> data_grp_size(3);
			vector<size_t> data_grp_pos(3);

			data_grp_pos[0] = p*sizeTimeLimit/timeIndexIncrementRebin;
			
			data_grp_pos[1] = 0;
			data_grp_pos[2] = 0;   	  
		

			if (  (m_Ntime-(p*sizeTimeLimit/timeIndexIncrementRebin)) == (nofLastElements/timeIndexIncrementRebin) )
			{
			
			    data_grp_size[0] = (nofLastElements/timeIndexIncrementRebin);
			    data_grp_size[1] = m_Nspectral;
			    data_grp_size[2] = obsNofStockes;
			    			    
			    data_grp.setMatrix( data_grp_pos, &DATA_3D[0], data_grp_size );
			}
			else
			{
			    data_grp_size[0] = m_Ntime-(p*sizeTimeLimit/timeIndexIncrementRebin);
			    data_grp_size[1] = m_Nspectral;
			    data_grp_size[2] = obsNofStockes;
			    			    
			    data_grp.setMatrix( data_grp_pos, &DATA_3D[0], data_grp_size );
			}
			
		      }
		  }    
	      
	      } // end loop on p (time step)
	      
	      	      
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
	    
	    
	    cout << "SAP N° "<< SAPindex <<" and BEAM N° " << j << " Stokes: " << l << " and Part: " << q <<  " is done" << endl;
	    cout << "  " << endl;    
	    	      	  
	  
	    
  }		      
		      
		       
		      

