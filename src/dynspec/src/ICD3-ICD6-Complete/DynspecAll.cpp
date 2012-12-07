/*
Dynamic spectrum (ICD6) file generator from ICD3 file

by Nicolas Vilchez 25/05/2012
nicolas.vilchez@cnrs-orleans.fr

version 1.1

systeme Ubuntu 10.04 x86 LTS
DAL v2.0 (library for hdf5 file)

*/
#include <iostream>
#include <string>
#include <time.h>
#include <unistd.h>

#include "Reader_Root.h"
#include "Stock_Write_Root_Metadata.h"


#include "Reader_Dynspec.h"
#include "Stock_Write_Dynspec_Metadata.h"

#include "Stock_Write_Dynspec_Data.h"

#include <dal/lofar/BF_File.h>


using namespace dal;
using namespace std;



int main(int argc, char *argv[])
{
  // Time CPU computation  
  clock_t start, end;
  double cpu_time_used;  
  start = clock();
  

  
  ////////////////////////////////////////////////////////////////////////////////////////  
  //INPUT PARAMETERS
  
  if (argc< 4 ){cout << "missing parameter: Dynspec  Observation-Path-DIR  Observation-Number(ex:Lxxxxx) Output-hdf5-DIR [Facultative:Dataset-RAM-allocation (in Go; default value: 1Go)] " << endl;}
  if (argc> 5 ){cout << "too much parameters: Dynspec  Observation-Path-DIR  Observation-Number(ex:Lxxxxx) Output-hdf5-DIR [Facultative:Dataset-RAM-allocation (in Go; default value: 1Go)] " << endl;}
  

  
  if (argc < 6 && argc > 3 )
  {  
    
  string pathDir(argv[1]);
  string obsName(argv[2]);
  
  float memoryRAM(1);
  if (argc== 5 ){memoryRAM=atof(argv[4]);};

  

  ////////////////////////////////////////////////////////////////////////////////////////    
  // PARAMETERS Determination


  int i(0);  // loop index initilization
  
  ////////////////////////////////////  
  // Stockes component determination

  string pathFile0(pathDir+obsName+"_SAP000_B000_S0_P000_bf.h5");
  BF_File file(pathFile0);  							// generate an object called file contains Root
  BF_SubArrayPointing SAP = file.subArrayPointing(0);				// generate an object called SAP contains 1 subarray pointing
  BF_BeamGroup BEAM	= SAP.beam(0); 						// generate an object called BEAM contains 1 beam					

  int obsNofStockes(BEAM.observationNofStokes().get());
  
  vector<string> stokesComponent(BEAM.stokesComponents().get()); 


  ////////////////////////////////////  
  // Others Parameters determination
    
  // SAP determination
  
  int obsNofSAP(file.observationNofSubArrayPointings().get());
  int obsNofBeam(SAP.observationNofBeams().get());  
  
  
  vector<float> raPoint(obsNofSAP),decPoint(obsNofSAP),sapFrequencyCenter(obsNofSAP);
  
  for (i=0;i<obsNofSAP;i++)
    { 
      std::ostringstream oss;oss << i;string index(oss.str());      
      if (i<10){index="_SAP00"+index+"_B000_S0_P000_bf.h5";}
      if (i>=10 && i<100){index="_SAP0"+index+"_B000_S0_P000_bf.h5";}
      if (i>=100 && i<1000){index="_SAP"+index+"_B000_S0_P000_bf.h5";}
       
      pathFile0 = pathDir+obsName+index;
      BF_File file(pathFile0);
      BF_SubArrayPointing SAP = file.subArrayPointing(i);
      BF_BeamGroup BEAM	= SAP.beam(0); 	
      raPoint[i]=SAP.pointRA().get();
      decPoint[i]=SAP.pointDEC().get(); 
      sapFrequencyCenter[i]=BEAM.beamFrequencyCenter().get()/1E6;   
    }

  std::sort(sapFrequencyCenter.begin(), sapFrequencyCenter.end());

  int obsNofSource(1),obsNofFrequencyBand(1);
  for (i=0;i<obsNofSAP-1;i++)
    {
      if (raPoint[i] != raPoint[i+1] || decPoint[i] != decPoint[i+1]){obsNofSource++;}
      if (sapFrequencyCenter[i] != sapFrequencyCenter[i+1]){obsNofFrequencyBand++;}
    }
    
  int obsNofTiledDynspec(0),methodFlag(0);  
  if (obsNofFrequencyBand>1){obsNofTiledDynspec++;methodFlag=0;} 
      
  
  ////////////////////////////////////  
  // Determine the number of frequency bands using another nomenclature !!   	// methodFlag=0 => tiled dynspec using SAP
  // Using Pxxx at the end of the file name 					// methodFlag=1 => tiled dynspec using Pxxx
  
  if (obsNofTiledDynspec == 0)
    {
      i = 0;
  
      int pathFile0Size(pathFile0.length());			// to be sure  
      string pathFileTemp(pathFile0.substr(0,pathFile0Size-10));

      pathFile0 = pathFileTemp + "P000_bf.h5";  
      const char *filename = pathFile0.c_str();
      int res(access(filename,F_OK));
      i++;
  
      //cout << filename << endl;
      
      while (res ==0)
	{
	  std::ostringstream oss;oss << i;string index(oss.str());
	  if (i<10){index="P00"+index;}
	  if (i>=10 && i<100){index="P0"+index;}
	  if (i>=100 && i<1000){index="P"+index;}
	  pathFile0 = pathFileTemp+index+"_bf.h5";
	  filename = pathFile0.c_str();
	  res = access(filename,F_OK);
	  if (res == 0){obsNofFrequencyBand++;i++;}
	}

      if (obsNofFrequencyBand>1){obsNofTiledDynspec=1;}
      else{ obsNofTiledDynspec=0;}
 
      if (obsNofTiledDynspec==1){methodFlag=1;} 
	  
    } 
/*  
  cout <<  obsNofSAP << endl;
  cout <<  obsNofBeam << endl;
  cout <<  obsNofStockes << endl;
  cout <<  obsNofSource << endl;
  cout <<  obsNofFrequencyBand << endl;
  cout <<  obsNofTiledDynspec << endl;
  cout << methodFlag  << endl;
  cout <<  methodFlagObsNofFrequencyBand << endl;
  
*/  
    /////////	END OF INTERNAL PARAMETERS DETERMINATION	//////////////////////// 
/*    
    List of useful parameter before ICD6 processing ! 
      obsNofSAP
      obsNofBeam
      obsNofStockes
      stokesComponent
    
      obsNofSource
      obsNofFrequencyBand
      obsNofTiledDynspec
    
      methodFlag
      methodFlagObsNofFrequencyBand  // Number of Pxxx frequency bands
    
    */ 


  ////////////////////////////////////////////////////////////////////////////////////////
  // SAP  PART  => Each SAP => One ICD6 hdf5 file 

  int k(0),l(0),q(0);  // Dynspec iterator

  // Dynspec using SAP method

  if (methodFlag == 0)
    {
      
      int j(0);
      // SAP method
      for (i=0;i<obsNofSAP;i++)  // Loop on SAP
	{
	  k = 0;
	  string index_i1;
	  std::ostringstream oss_i;oss_i << i;string index_i(oss_i.str());
	  if (i<10){index_i1="_SAP00"+index_i;}
	  if (i>=10 && i<100){index_i1="_SAP0"+index_i;}
	  if (i>=100 && i<1000){index_i1="_SAP"+index_i;}
	  
	  string outputFile(argv[3]);
	  string pathFile(pathDir+obsName+index_i1+"_B000_S0_P000_bf.h5");
	  outputFile = outputFile+obsName+index_i1+".h5";
	  
	  
	  //HDF5FileBase root_grp(outputFile, HDF5FileBase::CREATE);
	  File root_grp( outputFile, File::CREATE );
	  // generation of an object which will contains and write ROOT METADATA
	  Stock_Write_Root_Metadata *rootMetadata;
	  rootMetadata = new Stock_Write_Root_Metadata();
	  // generation of an object which will read ROOT METADATA
	  Reader_Root *rootObject;
	  rootObject = new Reader_Root();
   
	  
	  string SAPname(index_i1);
	  int SAPindex(i);
	  
	  // extract root metadata from the file and the number of stations  
	  rootObject->readRoot(pathFile,obsName,rootMetadata,obsNofSource,obsNofBeam,obsNofFrequencyBand,obsNofTiledDynspec,SAPname, SAPindex);//,obsNofSource,obsNofBeam,obsNofStockes,obsNofFrequencyBand,obsNofTiledDynspec,obsMinFrequency,obsMaxFrequency,obsCentralFrequency);
	  delete rootObject;  
	  // write the Root metadata in the output file
	  rootMetadata->writeRootMetadata(outputFile,root_grp);
	  delete rootMetadata;


	  for (j=0;j<obsNofBeam;j++)
	    {
	     
		    // Generate Dynspec Group
		    string index_k1;  
		    std::ostringstream oss_k;oss_k << k;string index_k(oss_k.str());
		    if (k<10){index_k1="00"+index_k;}
		    if (k>=10 && k<100){index_k1="0"+index_k;}
		    if (k>=100 && k<1000){index_k1=index_k;}     	      
		    Group dynspec_grp(root_grp, "DYNSPEC_"+index_k1);      
		    dynspec_grp.create();
	      
		    
		    // generation of an object which will read ROOT DYNSPEC METADATA
		    Reader_Dynspec *dynspecObject;
		    dynspecObject= new Reader_Dynspec();
		    
		    // generation of an object which will contains and write DYNSPEC METADATA  
		    Stock_Write_Dynspec_Metadata *dynspecMetadata;
		    dynspecMetadata = new Stock_Write_Dynspec_Metadata();

		    // generation of an object which will contains and write DYNSPEC DATA  
		    Stock_Write_Dynspec_Data *dynspecData;
		    dynspecData = new Stock_Write_Dynspec_Data();
		    
		    
		    // extract dynspec metadata from the file	      
		    pathFile = pathDir+obsName;
		    dynspecObject->readDynspec(pathFile,dynspecMetadata,dynspecData,i,j,q,obsNofSAP,obsNofSource,obsNofFrequencyBand,obsNofStockes,stokesComponent,SAPindex);
		    delete dynspecObject;
		    
		    // Write METADATA
		    dynspecMetadata->writeDynspecMetadata(dynspec_grp,pathDir,obsName,pathFile,outputFile,root_grp,i,j,k,l,q,obsNofStockes,stokesComponent,memoryRAM);
		    delete dynspecMetadata;
		    
		    //WRITE DATA
		    dynspecData->writeDynspecData(dynspec_grp,pathDir,obsName,pathFile,outputFile,root_grp,i,j,k,l,q,obsNofStockes,stokesComponent,memoryRAM,SAPindex);
		    delete dynspecData;
		    
		    k++;
	    }
	}
  
    }


    
    
  ///////////////////////////////////////////////////////////////////////////////////////////////  
  ///////////////////////////////////////////////////////////////////////////////////////////////  
    
  // Dynspec using Pxxx method
  
  if (methodFlag == 1)
  // Dynspec using SAP method
    {
      int j(0);
      
      // SAP method
      for (i=0;i<obsNofSAP;i++)  // Loop on SAP
	{
	  k = 0;
	  string index_i1;
	  std::ostringstream oss_i;oss_i << i;string index_i(oss_i.str());
	  if (i<10){index_i1="_SAP00"+index_i;}
	  if (i>=10 && i<100){index_i1="_SAP0"+index_i;}
	  if (i>=100 && i<1000){index_i1="_SAP"+index_i;}
	  
	  string outputFile(argv[3]);
	  string pathFile(pathDir+obsName+index_i1+"_B000_S0_P000_bf.h5");
	  outputFile = outputFile+obsName+index_i1+".h5";
	  
	  
	  //HDF5FileBase root_grp(outputFile, HDF5FileBase::CREATE);
	  File root_grp( outputFile, File::CREATE );
	  // generation of an object which will contains and write ROOT METADATA
	  Stock_Write_Root_Metadata *rootMetadata;
	  rootMetadata = new Stock_Write_Root_Metadata();
	  // generation of an object which will read ROOT METADATA
	  Reader_Root *rootObject;
	  rootObject = new Reader_Root();
   
	  
	  string SAPname(index_i1);
	  int SAPindex(i);
	  
	  // extract root metadata from the file and the number of stations  
	  rootObject->readRoot(pathFile,obsName,rootMetadata,obsNofSource,obsNofBeam,obsNofFrequencyBand,obsNofTiledDynspec,SAPname, SAPindex);//,obsNofSource,obsNofBeam,obsNofStockes,obsNofFrequencyBand,obsNofTiledDynspec,obsMinFrequency,obsMaxFrequency,obsCentralFrequency);
	  delete rootObject;  
	  // write the Root metadata in the output file
	  rootMetadata->writeRootMetadata(outputFile,root_grp);
	  delete rootMetadata;

	  for (j=0;j<obsNofBeam;j++)
	    {
	      
	      for (q=0;q<obsNofFrequencyBand;q++)
		{
		    // Generate Dynspec Group
		    string index_k1;  
		    std::ostringstream oss_k;oss_k << k;string index_k(oss_k.str());
		    if (k<10){index_k1="00"+index_k;}
		    if (k>=10 && k<100){index_k1="0"+index_k;}
		    if (k>=100 && k<1000){index_k1=index_k;}     	      
		    Group dynspec_grp(root_grp, "DYNSPEC_"+index_k1);      
		    dynspec_grp.create();
	      
		    
		    // generation of an object which will read ROOT DYNSPEC METADATA
		    Reader_Dynspec *dynspecObject;
		    dynspecObject= new Reader_Dynspec();
		    
		    // generation of an object which will contains and write DYNSPEC METADATA  
		    Stock_Write_Dynspec_Metadata *dynspecMetadata;
		    dynspecMetadata = new Stock_Write_Dynspec_Metadata();

		    // generation of an object which will contains and write DYNSPEC DATA  
		    Stock_Write_Dynspec_Data *dynspecData;
		    dynspecData = new Stock_Write_Dynspec_Data();
		    
		    
		    // extract dynspec metadata from the file	      
		    pathFile = pathDir+obsName;
		    dynspecObject->readDynspec(pathFile,dynspecMetadata,dynspecData,i,j,q,obsNofSAP,obsNofSource,obsNofFrequencyBand,obsNofStockes,stokesComponent, SAPindex);
		    delete dynspecObject;
		    
		    // Write METADATA
		    dynspecMetadata->writeDynspecMetadata(dynspec_grp,pathDir,obsName,pathFile,outputFile,root_grp,i,j,k,l,q,obsNofStockes,stokesComponent,memoryRAM);
		    delete dynspecMetadata;
		    
		    //WRITE DATA
		    dynspecData->writeDynspecData(dynspec_grp,pathDir,obsName,pathFile,outputFile,root_grp,i,j,k,l,q,obsNofStockes,stokesComponent,memoryRAM,SAPindex);
		    delete dynspecData;
	
		    
		    k++;
		}
	    }
	}
  
    }// end if loop

  } // End if parameters are right
  
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; 

  cout << endl;
  cout << "Duration of processing: " << cpu_time_used << " s" << endl;
  cout << endl;
  
  return 0;
}
