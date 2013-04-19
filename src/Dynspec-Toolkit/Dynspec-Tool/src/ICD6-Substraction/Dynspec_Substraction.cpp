/*
Dynamic spectrum (ICD6) file generator from ICD3 file

by Nicolas Vilchez 25/05/2012
nicolas.vilchez@cnrs-orleans.fr

version 1.1

systeme Ubuntu 12.04 x86 LTS
DAL v2.5 (library for hdf5 file)

*/


/// Projet       LOFAR
/// \file        DynspecAll.cpp 
/// \date        17/12/2012
/// \author      Nicolas VILCHEZ
/// \version     1.0
/// \copyright   LPC2E/CNRS Station de radio-astronomie de Nançay
///  \see DAL Library 2.5 (library for hdf5 file)
///  \brief 	 Main program for converting ICD3 files to ICD6 (dynamic spectrum) files without any time-frequency selection and rebinning (Complete convertion)
///  \details  
/// <br />Algorithm overview:
///   	 This code (pipeline) uses several classes. Each classes has a particular role for processing ICD3 data to dynamic spectrum data (ICD6)
///   	 The main code etablishes principal parameters of the observation (number of SAP, Beam and Stokes parameters. Thanks to robustess 
///   	 nomenclature of ICD3 format, with these parameters, we are able to know which file we have to process for a given SAP, Beam and Stokes parameter)
///   	 After existency tests and principal parameters determination, the main code will loop of the SAPs that it has to process:
///   	 
///   	 First, the code wil stock Root group metadata. Root Metadata are red by a function named readRoot and contained in the class Reader_Root_Substraction.h, the
///   	  Root group is generated and Root's metadata are stocked, then written by a function named writeRootMetadata and contained in the class Stock_Write_Root_Metadata_Substraction.h
///   	 
///   	 Secondly, the main code will loop on Beam, and generate dynamic spectrum group and its metadata. The dynamic spectrum's metadata are red by a function named
///   	 readDynspec and contained in the class Reader_Dynspec_Substraction.h, after, metadata are stoked, then written by a function named writeDynspecMetadata contained in
///   	 Stock_Write_Dynspec_Metadata_Substraction_Part.h
///   	 
///   	 Finaly, data are processed. Because LOFAR data are very voluminous, we have to develop a strategy for:
/// <br />   	 - avoid swaping
/// <br />   	 - have enough memory for loading data
/// <br />   	 etc ...
///   	 We decide to use a frozen quantity of RAM for a processing. This quantity is chosen by the user and corresponds to a number of time bins that we can process
///   	 for this RAM.
///   	 So, the code will loop of the number of Time series we need to process all data. no rebinning is done inside these blocks (see ICD3-ICD6-Rebin module). 
///   	 To conclude, data processing is done time blocks by time blocks, and data generation is done by the function  writeDynspecData which is included in the class 
///   	 Stock_Write_Dynspec_Data_Substraction.h
///	 All data contained in ICD3 observation are processed


#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <unistd.h>

#include "Reader_Root_Substraction.h"
#include "Stock_Write_Root_Metadata_Substraction.h"


#include "Reader_Dynspec_Substraction.h"
#include "Stock_Write_Dynspec_Metadata_Substraction.h"

#include "Stock_Write_Dynspec_Data_Substraction.h"

#include <dal/lofar/BF_File.h>


using namespace dal;
using namespace std;


   bool is_readableTer( const std::string & file ) 
  { 
    
/// \param file
/// <br />Check if file is readable, so if file exists !
/// \return boolean value 

    std::ifstream fichier( file.c_str() ); 
    return !fichier.fail(); 
  } 

  
int main(int argc, char *argv[])
{
  
  
/// <br />Usage: Dynspec  Observation-Path-DIR  Observation-Number(ex:Lxxxxx) Output-hdf5-DIR [Facultative:Dataset-RAM-allocation (in Go; default value: 1Go)]

/// \param argc Number of arguments
/// \param argv Table of arguments(see above c.f Usage)

/// <br />\see Variables:
/// <br />pathDir Observation (ICD3) directory
/// <br />obsName Observation ID
/// <br />SAPNumber Number of SAP to process 
/// <br />memoryRAM RAM Memory allocation for processing 
/// <br />Test Existing files allow to determinine main observation parameter:
/// <br />obsNofSAP Number of SAP contained in the observation directory
/// <br />nbBEAM Number of BEAM (i.e signals from a station or a correlated sum) in the observation directory
/// <br />nbSTOkES Number of Stokes paramters contained in the observation directory
/// <br />stokesComponent Vector of Stokes parameter
/// <br />We have 2 kinds of observation: using or not Pxxx nomenclature 
/// <br />rebinAllDynspec Flag which allow to process only one selected SAP or All
/// <br />methodFlag Flag which allow to know if Pxxxnomenclatureis used
/// <br />outputFile Output file for One SAP contains all dynamic spectrum

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
  

  string outDir(argv[1]);
  string ID(argv[2]);
  string file1(argv[3]);
  string dynspec1(argv[4]);
  string file2(argv[5]);
  string dynspec2(argv[6]);
  float kfactor(atof(argv[7]));
  float memoryRAM(atof(argv[8]));
  int nbSTOkES(atof(argv[9]));
  

  vector<string> stokesComponent(nbSTOkES);
  if (stokesComponent.size() ==1){stokesComponent[0]='I';}
  else{stokesComponent[0]='I';stokesComponent[1]='Q';stokesComponent[2]='U';stokesComponent[3]='V';}
  
      

  ////////////////////////////////////////////////////////////////////////////////////////
  // DYNSPEC SUBSTRACTION  PART  
  

  // fixed the outfile
  string outputFile(outDir+"Dynspec_Subsracted_"+ID+".h5");
    
  //HDF5FileBase root_grp(outputFile, HDF5FileBase::CREATE);
  File root_grp( outputFile, File::CREATE );
  // generation of an object which will contains and write ROOT METADATA
  Stock_Write_Root_Metadata_Substraction *rootMetadata;
  rootMetadata = new Stock_Write_Root_Metadata_Substraction();
  // generation of an object which will read ROOT METADATA
  Reader_Root_Substraction *rootObject;
  rootObject = new Reader_Root_Substraction();
  
  
  // extract root metadata from the file and the number of stations  
  rootObject->readRoot(rootMetadata,ID,file1,dynspec1,file2,dynspec2,kfactor);
  delete rootObject;  
  // write the Root metadata in the output file
  rootMetadata->writeRootMetadata(outputFile,root_grp);
  delete rootMetadata;



  // Generate Dynspec Group    	      
  Group dynspec_grp(root_grp, "DYNSPEC_000");      
  dynspec_grp.create();
  
  
  // generation of an object which will read ROOT DYNSPEC METADATA
  Reader_Dynspec_Substraction *dynspecObject;
  dynspecObject= new Reader_Dynspec_Substraction();
  
  // generation of an object which will contains and write DYNSPEC METADATA  
  Stock_Write_Dynspec_Metadata_Substraction *dynspecMetadata;
  dynspecMetadata = new Stock_Write_Dynspec_Metadata_Substraction();
  
  // generation of an object which will contains and write DYNSPEC DATA  
  Stock_Write_Dynspec_Data_Substraction *dynspecData;
  dynspecData = new Stock_Write_Dynspec_Data_Substraction();
  
  
  // extract dynspec metadata from the file	      		    
  dynspecObject->readDynspec(ID, file1, dynspec1, file2, dynspec2, kfactor, dynspecMetadata, dynspecData, nbSTOkES, stokesComponent);
  delete dynspecObject;
		    
  // Write METADATA
  dynspecMetadata->writeDynspecMetadata(dynspec_grp,outputFile,root_grp);
  delete dynspecMetadata;
		    
		    
  //WRITE DATA
  dynspecData->writeDynspecData(dynspec_grp,ID, file1, dynspec1, file2, dynspec2, kfactor,outputFile,root_grp,nbSTOkES,stokesComponent,memoryRAM);
  delete dynspecData;

	    
  cout << "Substraction Process Finished" << endl;
  cout << "  " << endl;


  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; 

  cout << endl;
  cout << "Duration of processing: " << cpu_time_used << " s" << endl;
  cout << endl;
  
  return 0;
}
