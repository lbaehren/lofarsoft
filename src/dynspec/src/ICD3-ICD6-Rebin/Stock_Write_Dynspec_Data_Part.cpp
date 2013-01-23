#include <iostream>
#include <string>
#include <fstream>

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
  
  
  void Stock_Write_Dynspec_Data_Part::writeDynspecData(Group &dynspec_grp,string pathDir,string obsName,string pathFile,string outputFile,File &root_grp,int i,int j,int k,int l,int q,int obsNofStockes,vector<string> stokesComponent,float memoryRAM, int SAPindex,float timeMinSelect,float timeMaxSelect,float timeRebin,float frequencyMin,float frequencyMax,float frequencyRebin)  
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

  
      cout << "begin BEAM N° :" <<  j << endl;

      // Go for Writting, but before test if files exists !!	    
    
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
      
  
      string pathFile0(pathFile);  
      pathFile		= pathFile0+index_i1+index_j1+"_S0"+index_q1+"_bf.h5"; 
      string pathRaw 	= pathFile0+index_i1+index_j1+"_S0"+index_q1+"_bf.raw"; 
      
	

    
      // Condition for generating the Dynamic Spectrum Stockes I must
      
      unsigned long int noEmptyStokes(0);
      
      if ( is_readable(pathRaw) and is_readable(pathFile)) 
	  {noEmptyStokes =0;}
      else
	  {pathRaw = pathFile0+index_i1+index_j1+"_S1"+index_q1+"_bf.raw";
	   pathFile= pathFile0+index_i1+index_j1+"_S1"+index_q1+"_bf.h5";
	   if ( is_readable(pathRaw) and is_readable(pathFile))
		{noEmptyStokes =1;}	   
	   else
		{pathRaw = pathFile0+index_i1+index_j1+"_S2"+index_q1+"_bf.raw";
		 pathFile= pathFile0+index_i1+index_j1+"_S2"+index_q1+"_bf.h5";
		 if ( is_readable(pathRaw) and is_readable(pathFile))
		     {noEmptyStokes =2;}
		 else
		     {pathRaw = pathFile0+index_i1+index_j1+"_S3"+index_q1+"_bf.raw";
		      pathFile= pathFile0+index_i1+index_j1+"_S3"+index_q1+"_bf.h5";
		      if ( is_readable(pathRaw)  and is_readable(pathFile))
			  {noEmptyStokes =3;}
		      else
			  {noEmptyStokes =4;}
		      }
		}
	  }
	

      if (noEmptyStokes < 4)	// Test for checking Empty Dynspec !!
	{	
	

	    ////////////////////////////////////////////////////////////////
	    // Redefine ObsStokes for particluar Cases
	    
	    BF_File file(pathFile);  							// generate an object called file contains Root
	    BF_SubArrayPointing SAP 		= file.subArrayPointing(i);		// generate an object called SAP contains subarray pointings
	    BF_BeamGroup BEAM			= SAP.beam(j);      			// generate an object called BEAM contains Beams 
		

	    BF_StokesDataset STOCKES 		= BEAM.stokes(noEmptyStokes);			// generate an object called STOCKES contains stockes data  
	  

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
	    unsigned long int p(0),n(0),m(0);

	    unsigned long int sizeTimeLimit((1.08E9*memoryRAM)/((spectralIndexStop-spectralIndexStart+1)*obsNofStockes));	// time step ~ 1Go RAM memory maximum !        
	    
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

	       cout << "index p N°: " << p  <<  " sur " << fracTime <<endl;
 	      

	      for (l=0;l<obsNofStockes;l++)
		{
		    double size3D(sizeTimeLimit*(spectralIndexStop-spectralIndexStart+1));
		    float size2D(sizeTimeLimit/timeIndexIncrementRebin*m_Nspectral);

		    vector<float> DATA_2D(size3D);
		    vector<float> DATA2DRebin(size2D);
		    

		    // Initialization of DATA2D Matrixes to 0 
		    for (m=0;m<sizeTimeLimit*(spectralIndexStop-spectralIndexStart+1);m++){DATA_2D[m]=0;}
		    for (m=0;m<sizeTimeLimit/timeIndexIncrementRebin*m_Nspectral;m++){DATA2DRebin[m]=0;}
		    
		    if (p==fracTime-1){
			if (nofLastElements > 0){
				DATA_2D.resize(nofLastElements * m_NspectralReal);
				for (m=0;m<nofLastElements * m_NspectralReal;m++){DATA_2D[m]=0;}}} 
		    

		    pathFile = pathDir+obsName;
		  
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
		

		    // Load Stokes data		    
		    pathFile = pathFile0+index_i1+index_j1+index_l1;
		    pathRaw =  pathFile0+index_i1+index_j1+"_S"+index_l+index_q1+"_bf.raw";	    
		    
		    if ( is_readable( pathRaw ) ) 
		    { 			    

			    // Load Stokes data
			    
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
				
				
				for (m=0;m<sizeTimeLimit/timeIndexIncrementRebin;m++)
				  {
				    for (n=0; n<m_Nspectral_longversion;n++)
				      {		  			  
					DATA_3D[obsNofStockes*((m_Nspectral*m)+n)+l]=DATA2DRebin[(m*m_Nspectral)+n];
				      }	  
				  }	    
			      }
			      
			    else
			      {
				if (nofLastElements > 0)
				  {		  
				    vector<size_t> pos(2);
				    pos[0]=p*sizeTimeLimit+timeIndexStart;
				    pos[1]=spectralIndexStart;
			  
				    vector<size_t> size(2);
				    size[0] = nofLastElements;
				    size[1] = (spectralIndexStop-spectralIndexStart+1);
				
				    STOCKES.getMatrix( pos, &DATA_2D[0], size );
										    
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
				for (m=0;m<nofLastElements/timeIndexIncrementRebin;m++)
				  {
				    for (n=0; n<m_Nspectral_longversion;n++)
				      {		  			  
					DATA_3D[obsNofStockes*((m_Nspectral*m)+n+l)]=DATA2DRebin[(m*m_Nspectral)+n];
				      }	  
				  }
				  }
			      } // end loop of else

			} // end of Existing test 
			else
			{
			    if (p!=fracTime-1)
			      { 
				for (m=0;m<sizeTimeLimit/timeIndexIncrementRebin;m++)
				  {
				    for (n=0; n<m_Nspectral_longversion;n++)
				      {		  			  
					DATA_3D[obsNofStockes*((m_Nspectral*m)+n+l)]=DATA2DRebin[(m*m_Nspectral)+n];
				      }	  
				  }	
			       }
			    else
			      {
				if (nofLastElements > 0)
				  {
					for (m=0;m<nofLastElements/timeIndexIncrementRebin;m++)
				  	{
				    		for (n=0; n<m_Nspectral_longversion;n++)
				      		{		  			  
						  DATA_3D[obsNofStockes*((m_Nspectral*m)+n)+l]=DATA2DRebin[(m*m_Nspectral)+n];
				      		}	  
				  	}
				  }
			       } 

			Group process_histo_grp(dynspec_grp, "PROCESS_HISTORY");
			Attribute<string> missedData(process_histo_grp, "MISSED DATA");
			missedData.value = stokesComponent[l];
 			       
			}// end of else existing test			      

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
			if (data_grp_size[0] ==0){data_grp_size[0]++;}

			data_grp_size[1] = m_Nspectral;
			if (data_grp_size[1] ==0){data_grp_size[1]++;}

			data_grp_size[2] = obsNofStockes;
		  	
			data_grp.setMatrix( data_grp_pos, &DATA_3D[0], data_grp_size );
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
	    
	    
	    cout << "SubArrayPointings N° "<< SAPindex <<" and Dynspec N° " << index_k << " is done" << endl;	      
	      
	      
	      
	      

	} // end of empty stokes
	else
	  {
	    
	    Group process_histo_grp(dynspec_grp, "PROCESS_HISTORY");
	    Attribute<string> missedData(process_histo_grp, "MISSED DATA");
	    missedData.value = "All Stokes Parameter Data are missing in ICD3 Observation";	    
	    
	    
	    
	    Dataset<float> data_grp(dynspec_grp, "DATA");
	    vector<ssize_t> dimensions(3);
	    dimensions[0] = m_Ntime;
	    dimensions[1] = m_Nspectral;
	    dimensions[2] = 1;      
	    data_grp.create( dimensions );


	    string GROUPE_TYPE_DATA("Data");
	    string WCSINFO("/Coordinates");
	    unsigned long int DATASET_NOF_AXIS(3);
	    vector<unsigned long int> DATASET_SHAPE(1);DATASET_SHAPE[0];
		      
	    Attribute<string> attr_143(data_grp, "GROUPE_TYPE");
	    Attribute<string> attr_144 (data_grp, "WCSINFO: DATA SET IS EMPTY (BEFORE PROCESSING)");
	    Attribute<unsigned long int> attr_145(data_grp, "DATASET_NOF_AXIS");
	    Attribute< vector<unsigned long int> > attr_146(data_grp, "DATASET_SHAPE");
		
	    attr_143.value = GROUPE_TYPE_DATA;
	    attr_144.value = WCSINFO;
	    attr_145.value = DATASET_NOF_AXIS;
	    attr_146.create().set(DATASET_SHAPE);  
	    
	    cout << "SubArrayPointings N° "<< SAPindex <<" and Dynspec N° " << index_k << " is done and empty => data missed" << endl;	  
	  }
	  
	    
  }
