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

#include <dal/lofar/BF_File.h>


using namespace dal;
using namespace std;


int main(int argc, char *argv[])
{

  // Usage: List-of-Files Output-dir ObsName tmax fmin fmax NofSAP nofBEAM nofStokes data-Percent

  ///////////////////////////////////////////////////////////////////////////////////////
  // Start Codes!
  
  // Time CPU computation  
  clock_t start, end;
  double cpu_time_used;  
  start = clock();
  
  
  ////////////////////////////////////////////////////////////////////////////////////////  
  //Input parameters
  
    
  string pathListFile(argv[1]);
  string outputDir(argv[2]);
  string obsName(argv[3]);
  float timeMaxSelect(atof(argv[4]));  
  float frequencyMin(atof(argv[5]));
  float frequencyMax(atof(argv[6]));
  int obsNofSAP(atof(argv[7]));
  int obsNofBeam(atof(argv[8]));
  int obsNofStockes(atof(argv[9]));  
  float dataTimePercent(atof(argv[10]));
  float dataSpectralPercent(atof(argv[11]));
   
  obsNofStockes = 1;
  
  // Default paramters
  float timeMinSelect(0);
  float timeRebin(1);
  float frequencyRebin(1);
  string rebinAllDynspec("yes");
  
  vector<string> listOfFiles(obsNofSAP*obsNofBeam);
  
  vector<string>  stokesComponent(1);
  if (obsNofStockes == 1){stokesComponent.resize(1);stokesComponent[0]='I';}

  ////////////////////////////////////////////////////////////////////////////////////////    
  // PARAMETERS Determination
  
    int i(0);  // loop index initilization
  
    // Open file in read mode
    ifstream file(pathListFile.c_str());
    if (file) // try open file
    { string line;
      for (i=0;i<obsNofSAP*obsNofBeam*obsNofStockes;i++)
      {getline(file,line);
       listOfFiles[i]= line;
      }
    }
  


      ////////////////////////////////////////////////////////////////////////////////////////
      // SAP  PART  => Each SAP => One ICD6 hdf5 file 
  
      int j(0);   
      int k(0);
      int l(0);
      int m(0);
      int q(0);
      
      for (i=0;i<obsNofSAP;i++)  // Loop on SAP
	{
	  cout << "SAP number " << i << " started" << endl;
	  k = 0;
	  string index_i1;
	  std::ostringstream oss_i;oss_i << i;string index_i(oss_i.str());
	  if (i<10){index_i1="_SAP00"+index_i;}
	  if (i>=10 && i<100){index_i1="_SAP0"+index_i;}
	  if (i>=100 && i<1000){index_i1="_SAP"+index_i;}
	  
	  
	  string pathFile(listOfFiles[m]);
	  string outputFile(outputDir+"QuickLook_"+obsName+index_i1+".h5");
	  
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
  
	  
	  //extract root metadata from the file and the number of stations  
	  rootObject->readRoot(pathFile,obsName,rootMetadata,obsNofBeam,SAPname, SAPindex,timeMinSelect,timeMaxSelect,timeRebin,frequencyMin,frequencyMax,frequencyRebin);
	  

	  
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
		    pathFile = listOfFiles[m];
		    dynspecObject->readDynspec(pathFile,dynspecMetadata,dynspecData,i,j,q,obsNofSAP,obsNofStockes,stokesComponent,SAPindex,timeMinSelect,timeMaxSelect,timeRebin,frequencyMin,frequencyMax,frequencyRebin);
		    delete dynspecObject;

		    // Write METADATA
		    dynspecMetadata->writeDynspecMetadata(dynspec_grp,obsName,pathFile,outputFile,root_grp,i,j,k,l,q,obsNofStockes,stokesComponent);
		    delete dynspecMetadata;

		    //WRITE DATA
		    dynspecData->writeDynspecData(dynspec_grp,obsName,pathFile,outputFile,root_grp,i,j,k,l,q,obsNofStockes,stokesComponent,SAPindex, timeMinSelect, timeMaxSelect, timeRebin, frequencyMin, frequencyMax, frequencyRebin, dataTimePercent,dataSpectralPercent);
		    delete dynspecData;

		    k++;m++;
	    }
	}
  


///////////////////////////////////////////////////////////////////////////////
// End of the quicklook tool


  
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; 

  cout << endl;
  cout << "Duration of processing: " << cpu_time_used << " s" << endl;
  cout << endl;
  
  return 0;
}
