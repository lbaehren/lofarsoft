/*
Dynamic spectrum (ICD6) file generator from ICD3 file

by Nicolas Vilchez 24/09/2012
nicolas.vilchez@cnrs-orleans.fr

version 2.0

systeme Ubuntu 12.04 x86 LTS
DAL v2.5 (library for hdf5 file)

*/



/// Projet       LOFAR
/// \file        DynspecPart.cpp 
/// \date        17/12/2012
/// \author      Nicolas VILCHEZ
/// \version     1.0
/// \copyright   LPC2E/CNRS Station de radio-astronomie de Nançay
///  \see DAL Library 2.5 (library for hdf5 file)
///  \brief 	 Main program for converting ICD3 files to ICD6 (dynamic spectrum) files with (or not) a time-frequency selection and/or rebinning
///  \details  
/// <br />Algorithm overview:
/// <br />This code (pipeline) uses several classes. Each classes has a particular role for processing ICD3 data to dynamic spectrum data (ICD6)
///   	 Main code etablishes the principal parameter of the observation (number of SAP, Beam and Stokes parameters. Thanks to robustess 
///   	 nomenclature of ICD3 format, with these parameter we are able to know which file we have to process for a given SAP, Beam and Stokes parameter)
///   	 After existency tests and principal parameters determination, the main code will loop of the SAPs that it has to process:
///   	 
///   	 First the code wil stock Root group metadata. Root Metadata are red by a function named readRoot and contained in the class Reader_Root_Part.h, the
///   	  Root group is generated and Root's matadata are stocked, then written by a function named writeRootMetadata and contained in the class Stock_Write_Root_Metadata_Part.h
///   	 
///   	 Secondly, the main code will loop on Beam, and generate dynamic spectrum group and its metadata. The dynamic spectrum's metadata are red by a function named
///   	 readDynspec and contained in the class Reader_Dynspec_Part.h, after, metadata are stocked, then written by a function named writeDynspecMetadata contained in
///   	 Stock_Write_Dynspec_Metadata_Part.h
///   	 
///   	 Finaly, data are processed. Because LOFAR data are very voluminous, we have to develop a strategy for:
/// <br /> - To avoid swaping
/// <br /> - To have enough memory for loading data
/// <br /> - etc ...
///   	 We decide to use a frozen quantity of RAM for a processing. This quantity is chosen by the user and corresponds to a number of time bins that we can process
///   	 for this RAM.
///   	 So, the code will loop of the number of Time series we need to process all the selected data. Rebinning is done inside these blocks. 
///   	 To conclude, data processing is done time blocks by time blocks, and data generation is done by the function  writeDynspecData which is included in the class 
///   	 Stock_Write_Dynspec_Data_Part.h
/// 	 Only selected data (time-frequency selection) are processed and/or rebinned !!





#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <unistd.h>

#include "Reader_Root_Part.h"
#include "Stock_Write_Root_Metadata_Part.h"
#include "Reader_Dynspec_Part.h"
#include "Stock_Write_Dynspec_Metadata_Part.h"
#include "Stock_Write_Dynspec_Data_Part.h"

#include "Reader_Root_Partionned.h"
#include "Stock_Write_Root_Metadata_Partionned.h"
#include "Reader_Dynspec_Partionned.h"
#include "Stock_Write_Dynspec_Metadata_Partionned.h"
#include "Stock_Write_Dynspec_Data_Partionned.h"

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

/// <br />Usage: DynspecPart Observation-Path-DIRObservation-Number(ex:Lxxxxx) Output-hdf5-DIR TimeMin TimeMax TimeRebin(s/pixel) FrequencyMin FrequencyMax FrequencyRebin(MHz/pixel) RebinAll(yes or no) [Facultative: Dataset-RAM-allocation (in Go; default value: 1Go) NumberOfSAP(if No, number of SAP to rebin/ if yes put 0 as default Value))] 

/// \param argc Number of arguments
/// \param argv Table of arguments(see above c.f Usage)

/// <br />\see Variables:
/// <br />pathDir: Observation (ICD3) directory
/// <br />obsName: Observation ID
/// <br />outputFile Output file for One SAP contains all dynamic spectrum
/// <br />timeMinSelect: Minimum Time selection
/// <br />timeMaxSelect: Maximum Time selection
/// <br />timeRebin: Time rebinning
/// <br />frequencyMin: Minimum Frequency selection
/// <br />frequencyMax: Maximum Frequency selection
/// <br />frequencyRebin: Frequency rebinning in a subbands => equal to 1 or a multiply of 2^n
/// <br />rebinAllDynspec: yes or no => convert one or all SAP (Sub array pointing i.e line of sight for an observation i.e observed source)
/// <br />SAPNumber: Number of SAP to process 
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
  
    
  string pathListFile(argv[1]);
  string outputDir(argv[2]);
  string obsName(argv[3]);
  
  float timeMinSelect(atof(argv[4]));
  float timeMaxSelect(atof(argv[5]));
  float timeRebin(atof(argv[6]));
  
  float frequencyMin(atof(argv[7]));
  float frequencyMax(atof(argv[8]));
  float frequencyRebin(atof(argv[9]));
  
  float memoryRAM(atof(argv[10]));
  string rebinAllDynspec(argv[11]);
  int SAPNumber(atof(argv[12]));
  
  int obsNofSAP(atof(argv[13]));
  int obsNofBeam(atof(argv[14]));    
  int obsNofStockes(atof(argv[15]));
  int obsNofFrequencyBand(atof(argv[16]));
  
 

	int i(0);
  	string pathFile("");
 	int obsNofTiledDynspec(0),methodFlag(0);    
        if (obsNofFrequencyBand >1){obsNofTiledDynspec++;methodFlag=1;}
     
      ///////////////////////////////////  
      // Others Parameters determination  

      // Final listOfFiles after obsNofFrequencyBand determine
      vector<string> listOfFiles(obsNofSAP*obsNofBeam*obsNofFrequencyBand*obsNofStockes);  

      // Open file in read mode
      ifstream file1(pathListFile.c_str());
      if (file1) // try open file
      { string line;
	for (i=0;i<obsNofSAP*obsNofBeam*obsNofFrequencyBand*obsNofStockes;i++)
	{getline(file1,line);
	listOfFiles[i]= line;
	}
      }
      

  
    pathFile	= listOfFiles[0];
    BF_File file(pathFile);  							// generate an object called file contains Root
    BF_SubArrayPointing SAP = file.subArrayPointing(0);		// generate an object called SAP contains 1 subarray pointing
    BF_BeamGroup BEAM	= SAP.beam(0); 				// generate an object called BEAM contains 1 beam					

    vector<string> stokesComponent(BEAM.stokesComponents().get()); 


 
//    cout <<  obsNofSAP << endl;
//    cout <<  obsNofBeam << endl;   
//    cout <<  obsNofStockes << endl;   
//    cout <<  obsNofFrequencyBand << endl;
//    cout <<  obsNofTiledDynspec << endl;
//    cout << methodFlag  << endl;


//     /////////	END OF INTERNAL PARAMETERS DETERMINATION	//////////////////////// 
// /*    
//     List of useful parameter before ICD6 processing ! 
//       obsNofSAP
//       obsNofBeam
//       obsNofStockes
//       stokesComponent
//       obsNofFrequencyBand
//       obsNofTiledDynspec
//     
//       methodFlag    
//     */ 




    ////////////////////////////////////////////////////////////////////////////////////////
    // SAP  PART  => Each SAP => One ICD6 hdf5 file 
    
    
    if (rebinAllDynspec == "no"){obsNofSAP = 1;}
    int j(0);   
    int k(0);
    int l(0);
    int m(0);
    int q(0);
    

  ///////////////////////////
  // Dynspec using SAP method

    if (methodFlag == 0)
      {

	// SAP method
	for (i=0;i<obsNofSAP;i++)  // Loop on SAP
	  {
	    
	  
	  if (rebinAllDynspec == "no"){i=SAPNumber;}
	  cout << "SAP number " << i << " started" << endl;
	  
	  if (m ==0)
	    {
	      if (i!=0)
	      {m=i*obsNofBeam;}
	    }
	
	
	  k = 0;
	  string index_i1;
	  std::ostringstream oss_i;oss_i << i;string index_i(oss_i.str());
	  if (i<10){index_i1="_SAP00"+index_i;}
	  if (i>=10 && i<100){index_i1="_SAP0"+index_i;}
	  if (i>=100 && i<1000){index_i1="_SAP"+index_i;}

	  string pathFile(listOfFiles[m*obsNofStockes]);
	  string outputFile(outputDir+"Dynspec_rebinned_"+obsName+index_i1+".h5");


	  //HDF5FileBase root_grp(outputFile, HDF5FileBase::CREATE);
	  File root_grp( outputFile, File::CREATE );
	  // generation of an object which will contains and write ROOT METADATA
	  Stock_Write_Root_Metadata_Part *rootMetadata;
	  rootMetadata = new Stock_Write_Root_Metadata_Part();
	  // generation of an object which will read ROOT METADATA
	  Reader_Root_Part *rootObject;
	  rootObject = new Reader_Root_Part();
   
	  
	  string SAPname(index_i1);
	  int SAPindex(i);
	  
	  // extract root metadata from the file and the number of stations  
	  rootObject->readRoot(pathFile,obsName,rootMetadata,obsNofBeam,obsNofFrequencyBand,obsNofTiledDynspec,SAPname, SAPindex,timeMinSelect,timeMaxSelect,timeRebin,frequencyMin,frequencyMax,frequencyRebin);//,obsNofSource,obsNofBeam,obsNofStockes,obsNofFrequencyBand,obsNofTiledDynspec,obsMinFrequency,obsMaxFrequency,obsCentralFrequency);
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
		    Reader_Dynspec_Part *dynspecObject;
		    dynspecObject= new Reader_Dynspec_Part();
		    
		    //generation of an object which will contains and write DYNSPEC METADATA  
		    Stock_Write_Dynspec_Metadata_Part *dynspecMetadata;
		    dynspecMetadata = new Stock_Write_Dynspec_Metadata_Part();

		    // generation of an object which will contains and write DYNSPEC DATA  
		    Stock_Write_Dynspec_Data_Part *dynspecData;
		    dynspecData = new Stock_Write_Dynspec_Data_Part();
		    
		    
		    //extract dynspec metadata from the file	      
		    pathFile = listOfFiles[m*obsNofStockes];
		    dynspecObject->readDynspec(pathFile,dynspecMetadata,dynspecData,i,j,q,obsNofSAP,obsNofFrequencyBand,obsNofStockes,stokesComponent,SAPindex,timeMinSelect,timeMaxSelect,timeRebin,frequencyMin,frequencyMax,frequencyRebin);
		    delete dynspecObject;

		    //Write METADATA
		    dynspecMetadata->writeDynspecMetadata(dynspec_grp,obsName,pathFile,outputFile,root_grp,i,j,k,l,q,obsNofStockes,stokesComponent,memoryRAM);
		    delete dynspecMetadata;

		    //WRITE DATA
		    dynspecData->writeDynspecData(dynspec_grp,obsName,pathFile,outputFile,root_grp,i,j,k,l,q,obsNofStockes,stokesComponent,memoryRAM,SAPindex, timeMinSelect, timeMaxSelect, timeRebin, frequencyMin, frequencyMax, frequencyRebin, listOfFiles, m, obsNofFrequencyBand);
		    delete dynspecData;

		    k++;m++;
	    }
	    
	    cout << "SAP number " << i << " done " << endl;
	    cout << "  " << endl;
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
	    
	  
	  if (rebinAllDynspec == "no"){i=SAPNumber;}
	  cout << "SAP number " << i << " started" << endl;
	  
	  if (m ==0)
	    {
	      if (i!=0)
	      {m=i*obsNofBeam;}
	    }
	
	
	  k = 0;
	  string index_i1;
	  std::ostringstream oss_i;oss_i << i;string index_i(oss_i.str());
	  if (i<10){index_i1="_SAP00"+index_i;}
	  if (i>=10 && i<100){index_i1="_SAP0"+index_i;}
	  if (i>=100 && i<1000){index_i1="_SAP"+index_i;}
	  
	  string pathFile(listOfFiles[m*obsNofStockes*obsNofFrequencyBand]);
	  string outputFile(outputDir+"Dynspec_rebinned_"+obsName+index_i1+".h5");
	    

	  //HDF5FileBase root_grp(outputFile, HDF5FileBase::CREATE);
	  File root_grp( outputFile, File::CREATE );
	  // generation of an object which will contains and write ROOT METADATA
	  Stock_Write_Root_Metadata_Partionned *rootMetadata;
	  rootMetadata = new Stock_Write_Root_Metadata_Partionned();
	  // generation of an object which will read ROOT METADATA
	  Reader_Root_Partionned *rootObject;
	  rootObject = new Reader_Root_Partionned();
   
	  
	  string SAPname(index_i1);
	  int SAPindex(i);

	  // extract root metadata from the file and the number of stations  
	  rootObject->readRootPartionned(pathFile,obsName,rootMetadata,obsNofBeam,obsNofFrequencyBand,obsNofTiledDynspec,SAPname, SAPindex,timeMinSelect,timeMaxSelect,timeRebin,frequencyMin,frequencyMax,frequencyRebin,i,j,m,listOfFiles,obsNofStockes);
	  delete rootObject;  
	  // write the Root metadata in the output file
	  rootMetadata->writeRootMetadataPartionned(outputFile,root_grp);
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
		    Reader_Dynspec_Partionned *dynspecObject;
		    dynspecObject= new Reader_Dynspec_Partionned();
		    
		    // generation of an object which will contains and write DYNSPEC METADATA  
		    Stock_Write_Dynspec_Metadata_Partionned *dynspecMetadata;
		    dynspecMetadata = new Stock_Write_Dynspec_Metadata_Partionned();

		    // generation of an object which will contains and write DYNSPEC DATA  
		    Stock_Write_Dynspec_Data_Partionned *dynspecData;
		    dynspecData = new Stock_Write_Dynspec_Data_Partionned();
		    
		    
		    // extract dynspec metadata from the file	      
		    pathFile = listOfFiles[m*obsNofStockes*obsNofFrequencyBand+q];		    		    
		    
		    dynspecObject->readDynspecPartionned(pathFile,dynspecMetadata,dynspecData,i,j,q,obsNofSAP,obsNofFrequencyBand,obsNofStockes,stokesComponent,SAPindex,timeMinSelect,timeMaxSelect,timeRebin,frequencyMin,frequencyMax,frequencyRebin,m,listOfFiles);
		    delete dynspecObject;
		    
		    // Write METADATA
		    dynspecMetadata->writeDynspecMetadataPartionned(dynspec_grp,obsName,pathFile,outputFile,root_grp,i,j,k,l,q,obsNofStockes,stokesComponent,memoryRAM);
		    delete dynspecMetadata;
		    
		    
 		    //WRITE DATA
		    dynspecData->writeDynspecDataPartionned(dynspec_grp,obsName,pathFile,outputFile,root_grp,i,j,k,l,q,obsNofStockes,stokesComponent,memoryRAM,SAPindex, timeMinSelect, timeMaxSelect, timeRebin, frequencyMin, frequencyMax, frequencyRebin,  listOfFiles, m, obsNofFrequencyBand);
		    delete dynspecData;
			    
		
		k++;m++;
		
	    }
	}
  
    }// end if loop


    

  
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; 

  cout << endl;
  cout << "Duration of processing: " << cpu_time_used << " s" << endl;
  cout << endl;
  
  return 0;
}
