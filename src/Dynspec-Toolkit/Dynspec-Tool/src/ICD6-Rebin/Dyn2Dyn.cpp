/*
Dynamic spectrum (ICD6) file generator from ICD3 file

by Nicolas Vilchez 24/09/2012
nicolas.vilchez@cnrs-orleans.fr

version 2.0

systeme Ubuntu 12.04 x86 LTS
DAL v2.5 (library for hdf5 file)

*/


/// Projet       LOFAR
/// \file        Dyn2Dyn.cpp 
/// \date        10/01/2013
/// \author      Nicolas VILCHEZ
/// \version     1.0
/// \copyright   LPC2E/CNRS Station de radio-astronomie de Nançay
///  \see DAL Library 2.5 (library for hdf5 file)
///  \brief 	 Main program for converting ICD3 files to ICD6 (dynamic spectrum) files with (or not) a time-frequency selection and/or rebinning
///  \details  
/// <br />Algorithm overview:
/// <br />This code (pipeline) uses several classes. Each classes has a particular role for processing dynamic spectrum data (ICD6). 
///	 The role of this code is to convert a dynamic spectrum file in another lighter dynamic spectrum file. In fact, after a first processing, we obtain 
///	 a first dynamic spectrum, but this dynamic spectrum is to massive to be visualized or the user needs only a smaller part of this spectrum. It is 
/// 	 useless and unoptimized to reprocess all the ICD3 data ! Because ICD3 data must be very massive (more than 100 To) and time reprocessing could be very long (more than several day) !
/// 	 So we introduce this tool to avoid reprocessing ! 
///
///  <br />Main code etablishes principal parameters of the observation (number of SAP, Beam and Stokes parameters. Thanks to robustess 
///   	 nomenclature of ICD3 format, with these parameter we are able to know which file we have to process for a given SAP, Beam and Stokes parameter)
///   	 After existency tests and principal parameters determination, the main code will loop of the SAPs that it has to process:
///   	 
///   	 First the code wil stock Root group metadata. Root Metadata are red by a function named readRoot and contained in the class Reader_Root_Dyn2Dyn_Part.h, the
///   	  Root group is generated and Root's matadata are stocked, then written by a function named writeRootMetadata and contained in the class Stock_Write_Root_Dyn2Dyn_Metadata_Part.h
///   	 
///   	 Secondly, the main code will loop on Beam, and generate dynamic spectrum group and its metadata. The dynamic spectrum's metadata are red by a function named
///   	 readDynspec and contained in the class Reader_Dyn2Dyn_Part.h, after, metadata are stocked, then written by a function named writeDynspecMetadata contained in
///   	 Stock_Write_Dyn2Dyn_Metadata_Part.h
///   	 
///   	 Finaly, data are processed. Because LOFAR data are very voluminous, we have to develop a strategy for:
/// <br /> - To avoid swaping
/// <br /> - To have enough memory for loading data
/// <br /> - etc ...
///   	 We decide to use a frozen quantity of RAM for a processing. This quantity is chosen by the user and corresponds to a number of time bins that we can process
///   	 for this RAM.
///   	 So, the code will loop of the number of Time series we need to process all the selected data. Rebinning is done inside these blocks. 
///   	 To conclude, data processing is done time blocks by time blocks, and data generation is done by the function  writeDynspecData which is included in the class 
///   	 Stock_Write_Dyn2Dyn_Data_Part.h
/// 	 Only selected data (time-frequency selection) are processed and/or rebinned !!




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
  
  
/// <br /> USAGE: Dyn2Dyn  Dynspec_File  Observation-Number(ex:Lxxxxx) Output-hdf5-File TimeMin TimeMax TimeRebin(s/pixel) FrequencyMin FrequencyMax FrequencyRebin(MHz/pixel) RebinAll(yes or no) [Facultative: Dataset-RAM-allocation (in Go; default value: 1Go) NumberOfSAP(if No, number of SAP to rebin/ if yes put 0 as default Value)]

/// \param argc Number of arguments
/// \param argv Table of arguments(see above c.f Usage)

/// <br />\see Variables:
/// <br />Dynspec_File: Observation File (ICD6 file) 
/// <br />obsName: Observation ID
/// <br />outputFile: Output file for One SAP contains all dynamic spectrum associated to this SAP
/// <br />timeMinSelect: Minimum Time selection
/// <br />timeMaxSelect: Maximum Time selection
/// <br />timeRebin: Time rebinning
/// <br />frequencyMin: Minimum Frequency selection
/// <br />frequencyMax: Maximum Frequency selection
/// <br />frequencyRebin: Frequency rebinning in a subbands => equal to 1 or a multiply of 2^n
/// <br />rebinAllDynspec: yes or no => convert one or all SAP (Sub array pointing i.e line of sight for an observation i.e observed source)
/// <br />SAPNumber Number: of SAP to process 
/// <br />memoryRAM: RAM Memory allocation for processing
/// <br />
/// <br />Test Existing files allow to determinine main observation parameter:
/// <br />obsNofSAP: Number of SAP contained in the observation directory
/// <br />obsNofBeam: Number of BEAM (i.e signals from a station or a correlated sum) in the observation directory
/// <br />obsNofStockes: Number of Stokes paramters contained in the observation directory
/// <br />stokesComponent: Vector of Stokes parameter
/// <br />
/// <br />We have 2 kinds of observation: using or not Pxxx nomenclature 
/// <br />rebinAllDynspec: Flag which allow to process only one selected SAP or All
/// <br />methodFlag: Flag which allow to know if Pxxxnomenclatureis used

/// <br />\see 2 modes (depends with nomenclature):
/// <br />
/// <br /> FIRST MODE:
/// <br />Loop on SAPs
/// <br />Root group generation => generation of the output file (*.h5) and its metadata	
/// <br />Loop on BEAMs
/// <br />generation of a dynamic spectrum in the Root group and its metadata
/// <br />Data processing for this dynamic spectrum
/// <br />
/// <br /> SECOND MODE:
/// <br />(if Pxxx nomenclature is used:) Loop on SAPs
/// <br />(if Pxxx nomenclature is used:) Loop on BEAMs	
/// <br />(if Pxxx nomenclature is used) Loop on differents Parts of the dynamic spectrum
/// <br />(if Pxxx nomenclature is used) generation of a dynamic spectrum in the Root group and its metadata, One dynamic spectrum for each Part
/// <br />(if Pxxx nomenclature is used) Data processing for this dynamic spectrum	  
  
/// <br />\return ICD6files  
  
  
  
  
  // Time CPU computation  
  clock_t start, end;
  double cpu_time_used;  
  start = clock();
  

  ////////////////////////////////////////////////////////////////////////////////////////  
  //INPUT PARAMETERS
  
    
  string pathFile(argv[1]);
  string obsName(argv[2]);
  string outputFile(argv[3]);

  float timeMinSelect(atof(argv[4]));
  float timeMaxSelect(atof(argv[5]));
  float timeRebin(atof(argv[6]));
  
  float frequencyMin(atof(argv[7]));
  float frequencyMax(atof(argv[8]));
  float frequencyRebin(atof(argv[9]));
  
  float memoryRAM=atof(argv[10]);

  
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