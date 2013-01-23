#include <iostream>
#include <string>

#include "Stock_Write_Dyn2Dyn_Data_Part.h"

#include <dal/lofar/BF_File.h>


/// \file Stock_Write_Dyn2Dyn_Data_Part.cpp
///  \brief File C++ (associated to Stock_Write_Dyn2Dyn_Data_Part.h) for stocking data parameters, processing the (select or rebin) and write data Matrix in the ICD6's Dynspec Groups 
///  \details  
/// <br /> Overview:
/// <br /> Functions stockDynspecData and writeDynspecData are described (programmed) here. 
/// The first one (stockDynspecData) will take as parameter processing parameters and stok them as private attributes. 
/// The second function (writeDynspecData) will write them in the hdf5 output file (in the corresponding dynspec).
/// Data processing is coded here !!

using namespace dal;
using namespace std;


  Stock_Write_Dyn2Dyn_Data_Part::Stock_Write_Dyn2Dyn_Data_Part(){}
  Stock_Write_Dyn2Dyn_Data_Part::~Stock_Write_Dyn2Dyn_Data_Part(){}
  
  void Stock_Write_Dyn2Dyn_Data_Part::stockDynspecData(int Ntime,int Nspectral,  int NtimeReal, int NspectralReal)
  {
    
  /// <br /> Usage:
  /// <br />   void Stock_Write_Dyn2Dyn_Data_Part::stockDynspecData(int Ntime,int Nspectral,  int NtimeReal, int NspectralReal)
  /// \param   Ntime  new time binning
  /// \param   Nspectral new spectral (frequency)  binning (number of channels per subbands)
  /// \param   NtimeReal current time binning before rebinning 
  /// \param   NspectralReal current spectral (frequency)  binning (number of channels per subbands) before rebinning     
    
    m_Ntime = Ntime;
    m_Nspectral = Nspectral;
    
    m_NtimeReal = NtimeReal;
    m_NspectralReal =NspectralReal;   
  }


  void Stock_Write_Dyn2Dyn_Data_Part::writeDynspecData(Group &dynspec_grp,string obsName,string pathFile,string outputFile,File &root_grp,int j,int k,int obsNofStockes,vector<string> stokesComponent,float memoryRAM,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin)  
  {
      
    
  /// <br /> Usage:
  /// <br />   void Stock_Write_Dyn2Dyn_Data_Part::writeDynspecData(Group &dynspec_grp,string obsName,string pathFile,string outputFile,File &root_grp,int j,int k,int obsNofStockes,vector<string> stokesComponent,float memoryRAM,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin)  
  /// \param  &dynspec_grp Group Object(Dynamic spectrum object)
  /// \param  obsName Observation ID
  /// \param  pathFile ICD3 path for loading file
  /// \param  outputFile output file 
  /// \param  &root_grp File Object (Root Group object)
  /// \param  j Beams loop index
  /// \param  k loop index
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
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      //WRITE THE DATA Matrix
            
      //generating the dataset
      
      Dataset<float> data_grp(dynspec_grp, "DATA");
      vector<ssize_t> dimensions(3);
      dimensions[0] = m_Ntime;
      dimensions[1] = m_Nspectral;
      dimensions[2] = obsNofStockes;      
      data_grp.create( dimensions );

      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Define Time and spectral for rebinning 

      // Define Time start and stop indexes
      
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
      
      
      // Define Spectral start and stop indexes
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

      
      int spectralIndexStart(index_fmin*CHANNELS_PER_SUBANDS_TEMP);
      int spectralIndexStop(index_fmax*CHANNELS_PER_SUBANDS_TEMP+CHANNELS_PER_SUBANDS_TEMP-1);
      int spectralIndexIncrementRebin((spectralIndexStop-spectralIndexStart+1)/Nspectral);
      
               
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // define the time step for filling the dataset
      int p(0),m(0),n(0);
      int sizeTimeLimit((2.2E8*memoryRAM)/((spectralIndexStop-spectralIndexStart+1)*obsNofStockes));	// time step ~ 1Go RAM memory maximum !        
      
      // SizeTimeLimit must be a multiple of Time Rebin for well cycling and avoid to lost data
      int timeFactor(sizeTimeLimit/timeIndexIncrementRebin);
      if (timeFactor == 0){timeFactor=1;}

      sizeTimeLimit = timeFactor*timeIndexIncrementRebin;
      
      // Determine with frac time the number of loop needed for splited data needed to be rebin etc... by blocks
      int fracTime((timeIndexStop-timeIndexStart)/sizeTimeLimit+1);         
      int nofLastElements((timeIndexStop-timeIndexStart)-((fracTime-1)*sizeTimeLimit));
      
       // nofLastElements must be a multiple of Time Rebin for well cycling and avoid to lost data     
      int nofLastElementsFactor(nofLastElements/timeIndexIncrementRebin);
      nofLastElements = nofLastElementsFactor*timeIndexIncrementRebin;
      

      
      // Loop on p (fraction of time for RAM optimisation consumming)
      
      for (p=0;p<fracTime;p++)
	{
	  
	  
 	vector<float> DATA_3D(sizeTimeLimit*m_Nspectral*obsNofStockes);
	if (p==fracTime-1){if (nofLastElements > 0){DATA_3D.resize(nofLastElements*m_Nspectral*obsNofStockes);}}	    

	for (int l=0;l<obsNofStockes;l++)
	  {
	      vector<float> DATA_2D(sizeTimeLimit*(spectralIndexStop-spectralIndexStart+1));
	      vector<float> DATA2DRebin(sizeTimeLimit/timeIndexIncrementRebin*m_Nspectral);
	      
	      if (p==fracTime-1){if (nofLastElements > 0){DATA_2D.resize(nofLastElements * m_NspectralReal);}} 

	      string index_j1;     
	      std::ostringstream oss_j;oss_j << j;string index_j(oss_j.str()); 
	      if (j<10){index_j1="_00"+index_j;}
	      if (j>=10 && j<100){index_j1="_0"+index_j;}
	      if (j>=100 && j<1000){index_j1="_"+index_j;}  
		      	      	      	      
  
	      if (p!=fracTime-1)
		{    
		  vector<size_t> pos(3);
		  pos[0]=p*sizeTimeLimit+timeIndexStart;
		  pos[1]=spectralIndexStart;
		  pos[2]=l;
	    
		  vector<size_t> size(3);
		  size[0] = sizeTimeLimit;
		  size[1] = (spectralIndexStop-spectralIndexStart+1);
		  size[2] = 1;
     
		  STOCKES.get3D( pos, &DATA_2D[0], size[0],size[1],size[2] );
		  		  
		  
		  for (int I=0;I<sizeTimeLimit/timeIndexIncrementRebin;I++)
		    {
		      for (int J=0;J<m_Nspectral;J++)
			{ 
			  float rebinPixel = 0;
			  for (int K=0;K<timeIndexIncrementRebin;K++)
			    {
			      for (int L=0;L<spectralIndexIncrementRebin;L++)
				{				  
				  rebinPixel = rebinPixel+DATA_2D[L+K*(spectralIndexStop-spectralIndexStart+1)+J*spectralIndexIncrementRebin+I*timeIndexIncrementRebin*(spectralIndexStop-spectralIndexStart+1)];
				}
			    }			    
			  DATA2DRebin[I*m_Nspectral+J] = rebinPixel/(timeIndexIncrementRebin*spectralIndexIncrementRebin);
			}
		     }
		  
		  
		  for (m=0;m<sizeTimeLimit/timeIndexIncrementRebin;m++)
		    {
		      for (n=0; n<m_Nspectral;n++)
			{		  			  
			  if (l==0){DATA_3D[obsNofStockes*((m_Nspectral*m)+n)]=DATA2DRebin[(m*m_Nspectral)+n];}
			  if (l==1){DATA_3D[obsNofStockes*((m_Nspectral*m)+n)+1]=DATA2DRebin[(m*m_Nspectral)+n];}
			  if (l==2){DATA_3D[obsNofStockes*((m_Nspectral*m)+n)+2]=DATA2DRebin[(m*m_Nspectral)+n];}
			  if (l==3){DATA_3D[obsNofStockes*((m_Nspectral*m)+n)+3]=DATA2DRebin[(m*m_Nspectral)+n];}
			}	  
		    }	    
	      }
 		
	      else
		{
		  if (nofLastElements > 0)
		    {		  
		      vector<size_t> pos(3);
		      pos[0]=p*sizeTimeLimit+timeIndexStart;
		      pos[1]=spectralIndexStart;
		      pos[2]=l;
		      
		      vector<size_t> size(3);
		      size[0] = nofLastElements;
		      size[1] = (spectralIndexStop-spectralIndexStart+1);
		      size[2] = 1;
		      
		      STOCKES.getMatrix( pos, &DATA_2D[0], size );
		      		      		      		      
		      for (int I=0;I<nofLastElements/timeIndexIncrementRebin;I++)
			{
			  for (int J=0;J<m_Nspectral;J++)
			    { 
			      float rebinPixel = 0;
			      for (int K=0;K<timeIndexIncrementRebin;K++)
				{
				  for (int L=0;L<spectralIndexIncrementRebin;L++)
				    {				  
				      rebinPixel = rebinPixel+DATA_2D[L+K*(spectralIndexStop-spectralIndexStart+1)+J*spectralIndexIncrementRebin+I*timeIndexIncrementRebin*(spectralIndexStop-spectralIndexStart+1)];
				    }
				}			    
			      DATA2DRebin[I*m_Nspectral+J] = rebinPixel/(timeIndexIncrementRebin*spectralIndexIncrementRebin);
			    }
			}		      
		  for (m=0;m<nofLastElements/timeIndexIncrementRebin;m++)
		    {
		      for (n=0; n<m_Nspectral;n++)
			{		  			  
			  if (l==0){DATA_3D[obsNofStockes*((m_Nspectral*m)+n)]=DATA2DRebin[(m*m_Nspectral)+n];}
			  if (l==1){DATA_3D[obsNofStockes*((m_Nspectral*m)+n)+1]=DATA2DRebin[(m*m_Nspectral)+n];}
			  if (l==2){DATA_3D[obsNofStockes*((m_Nspectral*m)+n)+2]=DATA2DRebin[(m*m_Nspectral)+n];}
			  if (l==3){DATA_3D[obsNofStockes*((m_Nspectral*m)+n)+3]=DATA2DRebin[(m*m_Nspectral)+n];}
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
	  
		  data_grp_size[0] = (nofLastElements/timeIndexIncrementRebin)-1;
		  data_grp_size[1] = m_Nspectral;
		  data_grp_size[2] = obsNofStockes;
	    
		  data_grp.setMatrix( data_grp_pos, &DATA_3D[0], data_grp_size );
		}
	    }

	} // end loop on p (time step)

 
      //META-DATA in DATA  writter
	
      string GROUPE_TYPE_DATA("Data");
      string WCSINFO("/Coordinates");
      int DATASET_NOF_AXIS(3);
      vector<int> DATASET_SHAPE(1);DATASET_SHAPE[0];
		
      Attribute<string> attr_143(data_grp, "GROUPE_TYPE");
      Attribute<string> attr_144 (data_grp, "WCSINFO");
      Attribute<int> attr_145(data_grp, "DATASET_NOF_AXIS");
      Attribute< vector<int> > attr_146(data_grp, "DATASET_SHAPE");
	  
      attr_143.value = GROUPE_TYPE_DATA;
      attr_144.value = WCSINFO;
      attr_145.value = DATASET_NOF_AXIS;
      attr_146.create().set(DATASET_SHAPE);     
      
      
      cout << "Dynspec N° "<< index_j << " is re-processed" << endl;    
  }
