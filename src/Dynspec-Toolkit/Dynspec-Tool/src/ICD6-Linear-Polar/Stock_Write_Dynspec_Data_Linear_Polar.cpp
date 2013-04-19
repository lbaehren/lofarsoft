#include <iostream>
#include <string>
#include <fstream>
#include <math.h>

#include "Stock_Write_Dynspec_Data_Linear_Polar.h"

#include <dal/lofar/BF_File.h>

/// \file Stock_Write_Dynspec_Data_Linear_Polar.cpp
///  \brief File C++ (associated to Stock_Write_Dynspec_Data_Linear_Polar_Part.h) for stock process parameters (for rebinning the data)  and write data Matrix for the ICD6's Dynspec Groups 
///  \details  
/// <br /> Overview:
/// <br /> Functions stockDynspecData and writeDynspecData are described (programmed) here. 
/// The first one (stockDynspecData) will take as parameter processing parameters and stok them as private attributes. 
/// The second function (writeDynspecData) will write them in the hdf5 output file (in the corresponding dynspec).
/// Data processing is coded here !!

using namespace dal;
using namespace std;


  Stock_Write_Dynspec_Data_Linear_Polar::Stock_Write_Dynspec_Data_Linear_Polar(){}
  Stock_Write_Dynspec_Data_Linear_Polar::~Stock_Write_Dynspec_Data_Linear_Polar(){}
  
  void Stock_Write_Dynspec_Data_Linear_Polar::stockDynspecData(int Ntime,int Nspectral)
  {
    
  /// <br /> Usage:
  /// <br />     void Stock_Write_Dynspec_Data_Linear_Polar::stockDynspecData(int Ntime,int Nspectral)
  /// \param   Ntime   time binning
  /// \param   Nspectral  spectral (frequency)  binning (number of channels per subbands)
    
    m_Ntime = Ntime;
    m_Nspectral = Nspectral;
  }


  
   bool is_readable( const std::string & file ) 
  { 
    /// \param file
    /// <br />Check if file is readable, so if file exists !
    /// \return boolean value 

    std::ifstream fichier( file.c_str() ); 
    return !fichier.fail(); 
  } 
    
  
  
  void Stock_Write_Dynspec_Data_Linear_Polar::writeDynspecData(Group &dynspec_grp,string ID,string filename,string dynspec, string outputFile,File &root_grp,int nbSTOkES,vector<string> stokesComponent,float memoryRAM)  
  {
      
  /// <br /> Usage:
  /// <br />   void Stock_Write_Dynspec_Data_Linear_Polar::writeDynspecData(Group &dynspec_grp,string pathDir,string obsName,string pathFile,string outputFile,File &root_grp,int i,int j,int k,int l,int q,int nbSTOkES,vector<string> stokesComponent,float memoryRAM, int SAPindex)  

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
  /// \param  nbSTOkES number of Stokes components
  /// \param  stokesComponent vector which contains all Stokes components
  /// \param  memoryRAM RAM memory consuption by processing
  /// \param  SAPindex Subarray pointings to process

   
   
   
  // Start the substraction => load datas
  
  // First Dynspec 
  File  File1(filename);  					// generate an object called file contains Root
  Group Dynspec1(File1,dynspec);				// generate an object called Dynspec contains 1 Dynspec
  Group COORDS1(Dynspec1,"COORDINATES");			// Generate an Objet called COORDS contains Coords group      
  Group COORDS_POLARIZATION1(COORDS1,"POLARIZATION");	 
  Group COORDS_SPECTRAL1(COORDS1,"SPECTRAL");
  Group COORDS_TIME1(COORDS1,"TIME");     
  Dataset<float> STOCKES1(Dynspec1, "DATA");		// generate an object called STOCKES contains stockes data  
  
  

  
  
  
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //WRITE THE DATA Matrix
  
  //generating the dataset
  
  Dataset<float> data_grp(dynspec_grp, "DATA");
  vector<ssize_t> dimensions(3);
  dimensions[0] = m_Ntime;
  dimensions[1] = m_Nspectral;
  dimensions[2] = nbSTOkES;      
  data_grp.create( dimensions );
      
      

  // define the time step for filling the dataset
  int p(0),r(0),n(0);
  
  
  int sizeTimeLimit((1.08E6*memoryRAM)/(m_Nspectral*nbSTOkES));	// time step ~ 1Go RAM memory maximum !
  int fracTime((m_Ntime/sizeTimeLimit)+1);
  int nofLastElements((m_Ntime)-((fracTime-1)*sizeTimeLimit));
      
            
      for (p=0;p<fracTime;p++)
	{
	 
	  vector<float> DATA_3D_File1(sizeTimeLimit*m_Nspectral*nbSTOkES);    
	  vector<float> DATA_3D_Linear(sizeTimeLimit*m_Nspectral*nbSTOkES);

	  if (p==fracTime-1){if (nofLastElements > 0){DATA_3D_File1.resize(nofLastElements*m_Nspectral*nbSTOkES);}}	  
	  if (p==fracTime-1){if (nofLastElements > 0){DATA_3D_Linear.resize(nofLastElements*m_Nspectral*nbSTOkES);}}	  
	  

	  
	  // Get matrixes NOW!!
	  
	      if (p!=fracTime-1)
		{ 	  
		  vector<size_t> pos(3);
		  pos[0]=p*sizeTimeLimit;
		  pos[1]=0;
		  pos[2]=0;
	    
		  vector<size_t> size(3);
		  size[0] = sizeTimeLimit;
		  size[1] = m_Nspectral;
		  size[2] = nbSTOkES;
     
		  STOCKES1.get3D( pos, &DATA_3D_File1[0], size[0],size[1],size[2] );	  
		    
		}
	      else
		{
		  if (nofLastElements > 0)
		    {	
		      
			vector<size_t> pos(3);
			pos[0]=p*sizeTimeLimit;
			pos[1]=0;
			pos[2]=0;
		  
			vector<size_t> size(3);
			size[0] = nofLastElements;
			size[1] = m_Nspectral;
			size[2] = nbSTOkES;
	  
			STOCKES1.get3D( pos, &DATA_3D_File1[0], size[0],size[1],size[2] );	
		    }
		}
		 		 
	  // Convert to I,Lin, PA, TOTAL
	  
	  
	  if (p!=fracTime-1)
	    {    	  
	      for (r=0;r<sizeTimeLimit;r++)
		  {
		    for (n=0;n<m_Nspectral;n++)
		      {		  			  			
			
			DATA_3D_Linear[nbSTOkES*((m_Nspectral*r)+n)] 	= DATA_3D_File1[nbSTOkES*((m_Nspectral*r)+n)];			
			DATA_3D_Linear[nbSTOkES*((m_Nspectral*r)+n)+1]	= sqrt(pow(DATA_3D_File1[nbSTOkES*((m_Nspectral*r)+n)+1],2)+pow(DATA_3D_File1[nbSTOkES*((m_Nspectral*r)+n)+2],2));			
			DATA_3D_Linear[nbSTOkES*((m_Nspectral*r)+n)+2]	= 0.5*atan((DATA_3D_File1[nbSTOkES*((m_Nspectral*r)+n)+2])/(DATA_3D_File1[nbSTOkES*((m_Nspectral*r)+n)+1]));			
			DATA_3D_Linear[nbSTOkES*((m_Nspectral*r)+n)+3]	= sqrt(pow(DATA_3D_File1[nbSTOkES*((m_Nspectral*r)+n)+1],2)+pow(DATA_3D_File1[nbSTOkES*((m_Nspectral*r)+n)+2],2)+pow(DATA_3D_File1[nbSTOkES*((m_Nspectral*r)+n)+3],2));					
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
			      DATA_3D_Linear[nbSTOkES*((m_Nspectral*r)+n)] 	= DATA_3D_File1[nbSTOkES*((m_Nspectral*r)+n)];			      
			      DATA_3D_Linear[nbSTOkES*((m_Nspectral*r)+n)+1]	= sqrt(pow(DATA_3D_File1[nbSTOkES*((m_Nspectral*r)+n)+1],2)+pow(DATA_3D_File1[nbSTOkES*((m_Nspectral*r)+n)+2],2));			      
			      DATA_3D_Linear[nbSTOkES*((m_Nspectral*r)+n)+2]	= 0.5*atan((DATA_3D_File1[nbSTOkES*((m_Nspectral*r)+n)+2])/(DATA_3D_File1[nbSTOkES*((m_Nspectral*r)+n)+1]));			      
			      DATA_3D_Linear[nbSTOkES*((m_Nspectral*r)+n)+3]	= sqrt(pow(DATA_3D_File1[nbSTOkES*((m_Nspectral*r)+n)+1],2)+pow(DATA_3D_File1[nbSTOkES*((m_Nspectral*r)+n)+2],2)+pow(DATA_3D_File1[nbSTOkES*((m_Nspectral*r)+n)+3],2));		      
			    }
			}
		    }
	     }		 
		 

	  // Write data by time steps	
	  if (p!=fracTime-1)
	    {	  
	      vector<size_t> data_grp_size(3);
	      vector<size_t> data_grp_pos(3);

	      data_grp_pos[0] = p*sizeTimeLimit;
	      data_grp_pos[1] = 0;
	      data_grp_pos[2] = 0;   	  
	  
	      data_grp_size[0] = sizeTimeLimit;
	      data_grp_size[1] = m_Nspectral;
	      data_grp_size[2] = nbSTOkES;
	    
	      data_grp.setMatrix( data_grp_pos, &DATA_3D_Linear[0], data_grp_size );
	    }
	  else
	    {		  
	      if (nofLastElements > 0)
		{
		  vector<size_t> data_grp_size(3);
		  vector<size_t> data_grp_pos(3);

		  data_grp_pos[0] = p*sizeTimeLimit;
		  data_grp_pos[1] = 0;
		  data_grp_pos[2] = 0;   	  
	  
		  data_grp_size[0] = nofLastElements;
		  data_grp_size[1] = m_Nspectral;
		  data_grp_size[2] = nbSTOkES;
	    
		  data_grp.setMatrix( data_grp_pos, &DATA_3D_Linear[0], data_grp_size );		    
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
 	    
	    	  
	    
  }
