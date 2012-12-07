/*
Dynamic spectrum (ICD6) file generator from ICD3 file

by Nicolas Vilchez 24/09/2012
nicolas.vilchez@cnrs-orleans.fr

version 2.0

systeme Ubuntu 12.04 x86 LTS
DAL v2.5 (library for hdf5 file)

*/
#include <iostream>
#include <string>
#include <time.h>
#include <unistd.h>

#include "Reader_Root_Dyn2Dyn_Part.h"
#include "Stock_Write_Root_Dyn2Dyn_Metadata_Part.h"

#include "Reader_Dyn2Dyn_Part.h"
#include "Stock_Write_Dyn2Dyn_Metadata_Part.h"

#include "Stock_Write_Dyn2Dyn_Data_Part.h"

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
  
  
  if (argc< 11 ){cout << "   " << endl; cout << "missing parameter: USAGE: Dyn2Dyn  Dynspec_File  Observation-Number(ex:Lxxxxx) Output-hdf5-File TimeMin TimeMax TimeRebin(s/pixel) FrequencyMin FrequencyMax FrequencyRebin(MHz/pixel) RebinAll(yes or no) [Facultative: Dataset-RAM-allocation (in Go; default value: 1Go) NumberOfSAP(if No, number of SAP to rebin)] " << endl;}
  if (argc> 13 ){cout << "   " << endl; cout << "too much parameters: USAGE: Dyn2Dyn  Dynspec_File  Observation-Number(ex:Lxxxxx) Output-hdf5-File TimeMin TimeMax TimeRebin(s/pixel) FrequencyMin FrequencyMax FrequencyRebin(MHz/pixel) RebinAll(yes or no) [Facultative: Dataset-RAM-allocation (in Go; default value: 1Go) Number-Of-SAP(if RebinAll=No, number of SAP to rebin is needed)] " << endl;}
    
  if (argc <= 13 && argc >= 11)
  {
  string rebinAllDynspec(argv[10]);  
  if (rebinAllDynspec ==  "no" && argc < 13){cout << "   " << endl; cout << "For process only ONE SAP, using RebinAll=no, you MUST specified the number of the SAP to rebin and also the RAM parameter" << endl;}
  else
  {  
    
  string pathFile(argv[1]);
  string obsName(argv[2]);
  string outputFile(argv[3]);
  
  float memoryRAM(1);
  if (argc== 12 or argc== 13){memoryRAM=atof(argv[11]);};

  float timeMinSelect(atof(argv[4]));
  float timeMaxSelect(atof(argv[5]));
  float timeRebin(atof(argv[6]));
  
  float frequencyMin(atof(argv[7]));
  float frequencyMax(atof(argv[8]));
  float frequencyRebin(atof(argv[9]));
  
  string rebinAllDynspec(argv[10]);

  
  ////////////////////////////////////////////////////////////////////////////////////////    
  // PARAMETERS Determination

  
  ////////////////////////////////////  
  // Stockes component determination

  File file(pathFile, File::READ);				// generate an object called file contains Root
  Group Dynspec(file, "DYNSPEC_000");				// generate an object called Dynspec contains 1 Dynspec
  
  
  Attribute< vector<string> > Obj_stokesComponent(Dynspec, "STOCKES_COMPONENT");
  vector<string> stokesComponent = Obj_stokesComponent.value;
  int obsNofStockes(stokesComponent.size());

  
  Attribute<int> Obj_obsNofBeam(file, "NOF_DYNSPEC");
  int obsNofBeam = Obj_obsNofBeam.value;  
  
  
//   cout <<  obsNofSAP << endl;
//   cout <<  obsNofBeam << endl;
//   cout <<  obsNofStockes << endl;
  

  ////////////////////////////////////////////////////////////////////////////////////////
  // DYNSPEC  PART

  int k(0);  //Dynspec iterator
  int j(0);  //BEAM iterator
	  
  k = 0;

  //HDF5FileBase root_grp(outputFile, HDF5FileBase::CREATE);
  File root_grp( outputFile, File::CREATE );
  // generation of an object which will contains and write ROOT METADATA
  Stock_Write_Root_Dyn2Dyn_Metadata_Part *rootMetadata;
  rootMetadata = new Stock_Write_Root_Dyn2Dyn_Metadata_Part();
  // generation of an object which will read ROOT METADATA
  Reader_Root_Dyn2Dyn_Part *rootObject;
  rootObject = new Reader_Root_Dyn2Dyn_Part();
   
  
  //extract root metadata from the file and the number of stations  
  rootObject->readRoot(pathFile,obsName,rootMetadata,obsNofBeam,timeMinSelect,timeMaxSelect,timeRebin,frequencyMin,frequencyMax,frequencyRebin);
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
	Reader_Dyn2Dyn_Part *dynspecObject;
	dynspecObject= new Reader_Dyn2Dyn_Part();
		    
	//generation of an object which will contains and write DYNSPEC METADATA  
	Stock_Write_Dyn2Dyn_Metadata_Part *dynspecMetadata;
	dynspecMetadata = new Stock_Write_Dyn2Dyn_Metadata_Part();

	// generation of an object which will contains and write DYNSPEC DATA  
	Stock_Write_Dyn2Dyn_Data_Part *dynspecData;
	dynspecData = new Stock_Write_Dyn2Dyn_Data_Part();
		    
	
	
	//extract dynspec metadata from the file	      
	dynspecObject->readDynspec(pathFile,dynspecMetadata,dynspecData,j,obsNofStockes,stokesComponent,timeMinSelect,timeMaxSelect,timeRebin,frequencyMin,frequencyMax,frequencyRebin);
	delete dynspecObject;

	//Write METADATA
	dynspecMetadata->writeDynspecMetadata(dynspec_grp,obsName,pathFile,outputFile,root_grp,j,k,obsNofStockes,stokesComponent,memoryRAM);
	delete dynspecMetadata;
		    		    
	//WRITE DATA
	dynspecData->writeDynspecData(dynspec_grp,obsName,pathFile,outputFile,root_grp,j,k,obsNofStockes,stokesComponent,memoryRAM, timeMinSelect, timeMaxSelect, timeRebin, frequencyMin, frequencyMax, frequencyRebin);
	delete dynspecData;
		    
	k++;
  }

  
   


  } // end else parameter is right
  } // End if parameters is right
  
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; 

  cout << endl;
  cout << "Duration of processing: " << cpu_time_used << " s" << endl;
  cout << endl;
  
  return 0;
}