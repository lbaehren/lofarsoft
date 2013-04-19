#include <iostream>
#include <string>
#include <fstream>

#include "Stock_Write_Dynspec_Data_Part_Standalone.h"

#include <dal/lofar/BF_File.h>


/// \file Stock_Write_Dynspec_Data_Part_Standalone.cpp
///  \brief File C++ (associated to Stock_Write_Dynspec_Data_Part_Standalone.h) for stocking data parameters, processing the (select or rebin) and write data Matrix in the ICD6's Dynspec Groups 
///  \details  
/// <br /> Overview:
/// <br /> Functions stockDynspecData and writeDynspecData are described (programmed) here. 
/// The first one (stockDynspecData) will take as parameter processing parameters and stok them as private attributes. 
/// The second function (writeDynspecData) will write them in the hdf5 output file (in the corresponding dynspec).
/// Data processing is coded here !!


using namespace dal;
using namespace std;


  Stock_Write_Dynspec_Data_Part_Standalone::Stock_Write_Dynspec_Data_Part_Standalone(){}
  Stock_Write_Dynspec_Data_Part_Standalone::~Stock_Write_Dynspec_Data_Part_Standalone(){}
  
  void Stock_Write_Dynspec_Data_Part_Standalone::stockDynspecData(int Ntime,int Nspectral,  int NtimeReal, int NspectralReal)
  { 
  /// <br /> Usage:
  /// <br />   void Stock_Write_Dynspec_Data_Part_Standalone::stockDynspecData(int Ntime,int Nspectral,  int NtimeReal, int NspectralReal)
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
  
  
  void Stock_Write_Dynspec_Data_Part_Standalone::writeDynspecData(Group &dynspec_grp,string obsName,string pathFile,string outputFile,File &root_grp,int i,int j,int k,int obsNofStockes,vector<string> stokesComponent,float memoryRAM,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin, vector<string> listOfFiles)
  {
  /// <br /> Usage:
  /// <br />   void Stock_Write_Dynspec_Data_Part_Standalone::writeDynspecData(Group &dynspec_grp,string pathDir,string obsName,string pathFile,string outputFile,File &root_grp,int i,int j,int k,int l,int q,int obsNofStockes,vector<string> stokesComponent,float memoryRAM, int SAPindex,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin)  
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



      ////////////////////////////////////////////////////////////////
      // Redefine ObsStokes for particluar Cases
      
      ////////////////////////////////////////////////////////////////
      // Redefine ObsStokes for particluar Cases
      
      BF_File file(pathFile);  							// generate an object called file contains Root
      BF_SubArrayPointing SAP 		= file.subArrayPointing(i);		// generate an object called SAP contains subarray pointings
      BF_BeamGroup BEAM			= SAP.beam(j);      			// generate an object called BEAM contains Beams 
      
      
      BF_StokesDataset STOCKES 		= BEAM.stokes(0);			// generate an object called STOCKES contains stockes data  
      
      
      CoordinatesGroup COORDS		= BEAM.coordinates();      		// generate an object called COORDS contains Coodinates directory
      Coordinate *SPECTRAL_COORDS	= COORDS.coordinate(1);			// generate an object called SPECTRAL_COORDS contains spectral data


      NumericalCoordinate *NUM_COORD_SPECTRAL = dynamic_cast<NumericalCoordinate*>(SPECTRAL_COORDS); // load Numerical coordinates lib 
      
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Define Time and spectral for rebinning 
      
      // Preliminary needs
      int NOF_SAMPLES(STOCKES.nofSamples().get()); 			//Time samples     
      double SAMPLING_TIME(BEAM.samplingTime().get()*1E6);
      double TOTAL_INTEGRATION_TIME(NOF_SAMPLES*SAMPLING_TIME/1E6);      
      
      // Define Time start and stop indexes
      
      float timeIcrementReal(1/(STOCKES.nofSamples().get()/TOTAL_INTEGRATION_TIME)) ;
      
      
      unsigned long int timeIndexIncrementRebin(timeRebin/timeIcrementReal);
      timeRebin = timeIndexIncrementRebin*(1/(STOCKES.nofSamples().get()/TOTAL_INTEGRATION_TIME));
      
      
      unsigned long int timeIndexStart(timeMinSelect/timeIcrementReal);
      timeIndexStart = (timeIndexStart/timeIndexIncrementRebin)*timeIndexIncrementRebin;
      timeMinSelect  = timeIndexStart*timeIcrementReal;
      
      unsigned long int timeIndexStop(timeMaxSelect/timeIcrementReal);
      timeIndexStop = timeIndexStop/timeIndexIncrementRebin*timeIndexIncrementRebin;
      timeMaxSelect = timeIndexStop*timeIcrementReal;
      
      
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
      int Nspectral=NOF_SUBBANDS*frequencyRebin;   
      
      unsigned long int spectralIndexStart(index_fmin*CHANNELS_PER_SUBANDS_TEMP);
      unsigned long int spectralIndexStop(index_fmax*CHANNELS_PER_SUBANDS_TEMP+CHANNELS_PER_SUBANDS_TEMP-1);
      unsigned long int spectralIndexIncrementRebin((spectralIndexStop-spectralIndexStart+1)/Nspectral);
      
      
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // define the time step for filling the dataset
      int l(0),n(0);
      unsigned long int p(0),r(0);
      
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
	      // Final vector for 3D DataSet
	      vector<float> DATA_3D(sizeTimeLimit*m_Nspectral*obsNofStockes/timeIndexIncrementRebin);
	      
	      // Vector for store 2D matrixes in 3D matrix with Stokes Parameters
	      vector<float> DATA_Pol_Norebin(sizeTimeLimit*(spectralIndexStop-spectralIndexStart+1)*obsNofStockes);
	      // Vector for store 2D matrixes in 3D matrix with Polarization Parameters
	      vector<float> DATA_3D_NoRebin(sizeTimeLimit*(spectralIndexStop-spectralIndexStart+1)*obsNofStockes);
	      
	      
	      if (p==fracTime-1)
	      {
		  if (nofLastElements > 0)
		    {	// Resizing for last time data serie
			DATA_3D.resize(nofLastElements*m_Nspectral*obsNofStockes/timeIndexIncrementRebin);
			
			DATA_Pol_Norebin.resize(nofLastElements*(spectralIndexStop-spectralIndexStart+1)*obsNofStockes);			  
			DATA_3D_NoRebin.resize(nofLastElements*(spectralIndexStop-spectralIndexStart+1)*obsNofStockes);
		    }
	      }	    
 	      

 	      // Start loop on Stokes
	      for (l=0;l<obsNofStockes;l++)
		{
		    double size3D(sizeTimeLimit*(spectralIndexStop-spectralIndexStart+1));

		    // Vector for storing Polarization matrix without rebinning
		    vector<float> DATA_2D(size3D);
		    

		    // Resizing for last time data serie
		    if (p==fracTime-1){
			if (nofLastElements > 0){
				DATA_2D.resize(nofLastElements * m_NspectralReal);
				for (r=0;r<nofLastElements * m_NspectralReal;r++){DATA_2D[r]=0;}}} 
		    
		    
		    pathFile 	= listOfFiles[l+j*l];
		    
 		    int pathFileSize(pathFile.length());
		    string pathDir(pathFile.substr(0,pathFileSize-2));
		    string pathRaw(pathDir+"raw");	  
		    
		    if ( is_readable( pathRaw ) ) 
		    { 			    

			    //Load Stokes data
			    
			    BF_File file(pathFile);  							// generate an object called file contains Root
			    BF_SubArrayPointing SAP 		= file.subArrayPointing(i);		// generate an object called SAP contains subarray pointings
			    BF_BeamGroup BEAM			= SAP.beam(j);      			// generate an object called BEAM contains Beams
			    

			    BF_StokesDataset STOCKES 		= BEAM.stokes(l);			// generate an object called STOCKES contains stockes data  
			  
 
			    //Load 2D Stockes data in a 2D array DATA0 with DAL and stock data in DATA3D for writting (by time step)     
			    if (p!=fracTime-1)
			      {    
				vector<size_t> pos(2);
				pos[0]=p*sizeTimeLimit+timeIndexStart;
				pos[1]=spectralIndexStart;
			  
				vector<size_t> size(2);
				size[0] = sizeTimeLimit;
				size[1] = (spectralIndexStop-spectralIndexStart+1);
			      		
				STOCKES.getMatrix( pos, &DATA_2D[0], size );
				
				for (r=0;r<sizeTimeLimit;r++)
				  {
				    for (n=0;n<m_Nspectral;n++)
				      {		  			  
					if (l==0){DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)]=DATA_2D[(r*m_Nspectral)+n];}
					if (l==1){DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+1]=DATA_2D[(r*m_Nspectral)+n];}
					if (l==2){DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+2]=DATA_2D[(r*m_Nspectral)+n];}
					if (l==3){DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+3]=DATA_2D[(r*m_Nspectral)+n];}					
				      }	  
				  }
				}
			      else
				{
				  if (nofLastElements > 0)
				    {		  
				      vector<size_t> pos(2);
				      pos[0]=p*sizeTimeLimit;
				      pos[1]=0;
			    
				      vector<size_t> size(2);
				      size[0] = nofLastElements;
				      size[1] = m_Nspectral;
				  
				      STOCKES.getMatrix( pos, &DATA_2D[0], size );
					      
				      for (r=0;r<nofLastElements;r++)
					{
					  for (n=0;n<m_Nspectral;n++)
					    {		  			  
					      if (l==0){DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)]=DATA_2D[(r*m_Nspectral)+n];}
					      if (l==1){DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+1]=DATA_2D[(r*m_Nspectral)+n];}
					      if (l==2){DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+2]=DATA_2D[(r*m_Nspectral)+n];}
					      if (l==3){DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+3]=DATA_2D[(r*m_Nspectral)+n];}
								      
					    }	  
					}
				    }
			      } // end loop of else
				

 
			} // end of Existing test 
			else
			{

			Group process_histo_grp(dynspec_grp, "PROCESS_HISTORY");
			Attribute<string> missedData(process_histo_grp, "MISSED DATA");
			missedData.value = stokesComponent[l];
			cout << "RAW DATA are missing for SAP: " << i  << "  BEAM: " << j << " Stokes: " << l << endl;
 			       
			}// end of else existing test			      

		   }  //end loop on l (Stockes)// data are loaded in 3DNoRebin array
		   
		   
		   
		  
	      //Convert to I,U,Q & V
 
	      for (l=0;l<obsNofStockes;l++)
		{ 
		    if (p!=fracTime-1)
		      {    				    
			for (r=0;r<sizeTimeLimit;r++)
			  {
			    for (n=0;n<m_Nspectral;n++)
			      {		  
			      if (l==0){DATA_Pol_Norebin[obsNofStockes*((m_Nspectral*r)+n)]   = ( pow(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)],2.0) + pow(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+1],2.0) + pow(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+2],2.0) + pow(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+3],2.0)       )/2.0;}									 
			      if (l==1){DATA_Pol_Norebin[obsNofStockes*((m_Nspectral*r)+n)+1] = ( pow(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)],2.0) + pow(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+1],2.0) - pow(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+2],2.0) - pow(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+3],2.0)       )/2.0;}
			      if (l==2){DATA_Pol_Norebin[obsNofStockes*((m_Nspectral*r)+n)+2] = ((DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)]*DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+2])+(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+1]*DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+3]));}
			      if (l==3){DATA_Pol_Norebin[obsNofStockes*((m_Nspectral*r)+n)+3] = ((DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)]*DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+3])-(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+1]*DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+2]));}				
			      				
			      }
			  }
		      }
			      
		      else
			{
			  if (nofLastElements > 0)
			    {		  
			      for (r=0;r<nofLastElements;r++)
				  {
				    for (n=0;n<m_Nspectral;n++)
					{		  			  
					    if (l==0){DATA_Pol_Norebin[obsNofStockes*((m_Nspectral*r)+n)]   = ( pow(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)],2.0) + pow(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+1],2.0) + pow(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+2],2.0) + pow(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+3],2.0)       )/2.0;}									 
					    if (l==1){DATA_Pol_Norebin[obsNofStockes*((m_Nspectral*r)+n)+1] = ( pow(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)],2.0) + pow(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+1],2.0) - pow(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+2],2.0) - pow(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+3],2.0)       )/2.0;}
					    if (l==2){DATA_Pol_Norebin[obsNofStockes*((m_Nspectral*r)+n)+2] = ((DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)]*DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+2])+(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+1]*DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+3]));}
					    if (l==3){DATA_Pol_Norebin[obsNofStockes*((m_Nspectral*r)+n)+3] = ((DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)]*DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+3])-(DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+1]*DATA_3D_NoRebin[obsNofStockes*((m_Nspectral*r)+n)+2]));}				
											
					}
				  }
			    }
			} // end loop of else 
 
		} // end loop on l
		
		
 
		// Do NOW (and not before) the rebinning !!
		
		for (l=0;l<obsNofStockes;l++)
		  { 
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
						  if (l==0){rebinPixel = rebinPixel + DATA_Pol_Norebin[(L+K*(spectralIndexStop-spectralIndexStart+1)+J*spectralIndexIncrementRebin+I*timeIndexIncrementRebin*(spectralIndexStop-spectralIndexStart+1))*4];}						
						  if (l==1){rebinPixel = rebinPixel + DATA_Pol_Norebin[((L+K*(spectralIndexStop-spectralIndexStart+1)+J*spectralIndexIncrementRebin+I*timeIndexIncrementRebin*(spectralIndexStop-spectralIndexStart+1))*4)+1];}
						  if (l==2){rebinPixel = rebinPixel + DATA_Pol_Norebin[((L+K*(spectralIndexStop-spectralIndexStart+1)+J*spectralIndexIncrementRebin+I*timeIndexIncrementRebin*(spectralIndexStop-spectralIndexStart+1))*4)+2];}
						  if (l==3){rebinPixel = rebinPixel + DATA_Pol_Norebin[((L+K*(spectralIndexStop-spectralIndexStart+1)+J*spectralIndexIncrementRebin+I*timeIndexIncrementRebin*(spectralIndexStop-spectralIndexStart+1))*4)+3];}
						}
					    }	
					    if (l==0){DATA_3D[(I*m_Nspectral+J)*4]	= rebinPixel/(timeIndexIncrementRebin*spectralIndexIncrementRebin);}
					    if (l==1){DATA_3D[((I*m_Nspectral+J)*4)+1]	= rebinPixel/(timeIndexIncrementRebin*spectralIndexIncrementRebin);}
					    if (l==2){DATA_3D[((I*m_Nspectral+J)*4)+2] = rebinPixel/(timeIndexIncrementRebin*spectralIndexIncrementRebin);}
					    if (l==3){DATA_3D[((I*m_Nspectral+J)*4)+3]	= rebinPixel/(timeIndexIncrementRebin*spectralIndexIncrementRebin);}
					 }
				  }
			}// end if p!=fracTime-1)
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
						  if (l==0){rebinPixel = rebinPixel + DATA_Pol_Norebin[(L+K*(spectralIndexStop-spectralIndexStart+1)+J*spectralIndexIncrementRebin+I*timeIndexIncrementRebin*(spectralIndexStop-spectralIndexStart+1))*4];}						
						  if (l==1){rebinPixel = rebinPixel + DATA_Pol_Norebin[((L+K*(spectralIndexStop-spectralIndexStart+1)+J*spectralIndexIncrementRebin+I*timeIndexIncrementRebin*(spectralIndexStop-spectralIndexStart+1))*4)+1];}
						  if (l==2){rebinPixel = rebinPixel + DATA_Pol_Norebin[((L+K*(spectralIndexStop-spectralIndexStart+1)+J*spectralIndexIncrementRebin+I*timeIndexIncrementRebin*(spectralIndexStop-spectralIndexStart+1))*4)+2];}
						  if (l==3){rebinPixel = rebinPixel + DATA_Pol_Norebin[((L+K*(spectralIndexStop-spectralIndexStart+1)+J*spectralIndexIncrementRebin+I*timeIndexIncrementRebin*(spectralIndexStop-spectralIndexStart+1))*4)+3];}
						}
					    }	
					    if (l==0){DATA_3D[(I*m_Nspectral+J)*4]	= rebinPixel/(timeIndexIncrementRebin*spectralIndexIncrementRebin);}
					    if (l==1){DATA_3D[((I*m_Nspectral+J)*4)+1]	= rebinPixel/(timeIndexIncrementRebin*spectralIndexIncrementRebin);}
					    if (l==2){DATA_3D[((I*m_Nspectral+J)*4)+2] = rebinPixel/(timeIndexIncrementRebin*spectralIndexIncrementRebin);}
					    if (l==3){DATA_3D[((I*m_Nspectral+J)*4)+3]	= rebinPixel/(timeIndexIncrementRebin*spectralIndexIncrementRebin);}
					 }
				  }			
			    }
		      }
				  
	  
				  
			
		  } // end loop on l for rebinning
		  

		  
		
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
	    
	    
	    cout << "SAP N° "<< i <<" and BEAM N° " << j  << endl;
	    cout << "  " << endl;    
	    	      	  
	  
	    
  }
