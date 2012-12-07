#include <iostream>
#include <string>

#include "Stock_Write_Dynspec_Data.h"

#include <dal/lofar/BF_File.h>

using namespace dal;
using namespace std;


  Stock_Write_Dynspec_Data::Stock_Write_Dynspec_Data(){}
  Stock_Write_Dynspec_Data::~Stock_Write_Dynspec_Data(){}
  
  void Stock_Write_Dynspec_Data::stockDynspecData(int Ntime,int Nspectral)
  {
    m_Ntime = Ntime;
    m_Nspectral = Nspectral;
  }


  void Stock_Write_Dynspec_Data::writeDynspecData(Group &dynspec_grp,string pathDir,string obsName,string pathFile,string outputFile,File &root_grp,int i,int j,int k,int l,int q,int obsNofStockes,vector<string> stokesComponent,float memoryRAM, int SAPindex)  
  {
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
      
      pathFile	= pathFile+index_i1+index_j1+"_S0"+index_q1+"_bf.h5"; 
    
      
      ////////////////////////////////////////////////////////////////
      // Redefine ObsStokes for particluar Cases
      
      
      BF_File fileTEMP(pathFile);  								// generate an object called file contains Root
      BF_SubArrayPointing SAPTEMP 		= fileTEMP.subArrayPointing(i);		// generate an object called SAP contains subarray pointings
      BF_BeamGroup BEAMTEMP			= SAPTEMP.beam(j);
      
      obsNofStockes = BEAMTEMP.observationNofStokes().get();      
      if (obsNofStockes == 1){stokesComponent.resize(1);stokesComponent[0]='I';}
      if (obsNofStockes == 2){stokesComponent.resize(2);stokesComponent[0]='I';stokesComponent[1]='Q';}
      if (obsNofStockes == 3){stokesComponent.resize(3);stokesComponent[0]='I';stokesComponent[1]='Q';stokesComponent[2]='U';}
      if (obsNofStockes == 4){stokesComponent.resize(4);stokesComponent[0]='I';stokesComponent[1]='Q';stokesComponent[2]='U';stokesComponent[3]='V';}
      
      
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
      
      
      // define the time step for filling the dataset
      int p(0),m(0),n(0);
      int sizeTimeLimit((2.2E8*memoryRAM)/(m_Nspectral*obsNofStockes));	// time step ~ 1Go RAM memory maximum !
      int fracTime((m_Ntime/sizeTimeLimit)+1);
      int nofLastElements((m_Ntime)-((fracTime-1)*sizeTimeLimit));
      
      
      
      
      
      for (p=0;p<fracTime;p++)
	{
	  
	  
	vector<float> DATA_3D(sizeTimeLimit*m_Nspectral*obsNofStockes);    
	if (p==fracTime-1){if (nofLastElements > 0){DATA_3D.resize(nofLastElements*m_Nspectral*obsNofStockes);}}	    

	for (l=0;l<obsNofStockes;l++)
	  {
	      vector<float> DATA_2D(sizeTimeLimit * m_Nspectral);
	      if (p==fracTime-1){if (nofLastElements > 0){DATA_2D.resize(nofLastElements * m_Nspectral);}} 
	      
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
	      pathFile = pathFile+index_i1+index_j1+index_l1;
	      BF_File file(pathFile);  						// generate an object called file contains Root
	      BF_SubArrayPointing SAP 		= file.subArrayPointing(i);		// generate an object called SAP contains subarray pointings
	      BF_BeamGroup BEAM			= SAP.beam(j);      			// generate an object called BEAM contains Beams
	      BF_StokesDataset STOCKES 	= BEAM.stokes(l);			// generate an object called STOCKES contains stockes data  

	      
	      //Load 2D Stockes data in a 2D array DATA0 with DAL and stock data in DATA3D for writting (by time step)     
	      if (p!=fracTime-1)
		{    
		  vector<size_t> pos(2);
		  pos[0]=p*sizeTimeLimit;
		  pos[1]=0;
	    
		  vector<size_t> size(2);
		  size[0] = sizeTimeLimit;
		  size[1] = m_Nspectral;
      		
		  STOCKES.getMatrix( pos, &DATA_2D[0], size );
	     
		  for (m=0;m<sizeTimeLimit;m++)
		    {
		      for (n=0;n<m_Nspectral;n++)
			{		  			  
			  if (l==0){DATA_3D[obsNofStockes*((m_Nspectral*m)+n)]=DATA_2D[(m*m_Nspectral)+n];}
			  if (l==1){DATA_3D[obsNofStockes*((m_Nspectral*m)+n)+1]=DATA_2D[(m*m_Nspectral)+n];}
			  if (l==2){DATA_3D[obsNofStockes*((m_Nspectral*m)+n)+2]=DATA_2D[(m*m_Nspectral)+n];}
			  if (l==3){DATA_3D[obsNofStockes*((m_Nspectral*m)+n)+3]=DATA_2D[(m*m_Nspectral)+n];}
			  			  
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
		      	      
		      for (m=0;m<nofLastElements;m++)
			{
			  for (n=0;n<m_Nspectral;n++)
			    {  
			      if (l==0){DATA_3D[obsNofStockes*((m_Nspectral*m)+n)]=DATA_2D[(m*m_Nspectral)+n];}
			      if (l==1){DATA_3D[obsNofStockes*((m_Nspectral*m)+n)+1]=DATA_2D[(m*m_Nspectral)+n];}
			      if (l==2){DATA_3D[obsNofStockes*((m_Nspectral*m)+n)+2]=DATA_2D[(m*m_Nspectral)+n];}
			      if (l==3){DATA_3D[obsNofStockes*((m_Nspectral*m)+n)+3]=DATA_2D[(m*m_Nspectral)+n];}		
			    }
			}
		    }
		}
	  }  //end loop on l (Stockes)// data are loaded in 3D array
	  
	  
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
	      data_grp_size[2] = obsNofStockes;
	    
	      data_grp.setMatrix( data_grp_pos, &DATA_3D[0], data_grp_size );
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
      
      
      cout << "SubArrayPointings N° "<< SAPindex <<" and Dynspec N° " << index_k << " is done" << endl; 
      cout << endl;
  }
