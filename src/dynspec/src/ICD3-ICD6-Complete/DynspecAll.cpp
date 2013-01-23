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
///   	 First, the code wil stock Root group metadata. Root Metadata are red by a function named readRoot and contained in the class Reader_Root.h, the
///   	  Root group is generated and Root's metadata are stocked, then written by a function named writeRootMetadata and contained in the class Stock_Write_Root_Metadata.h
///   	 
///   	 Secondly, the main code will loop on Beam, and generate dynamic spectrum group and its metadata. The dynamic spectrum's metadata are red by a function named
///   	 readDynspec and contained in the class Reader_Dynspec.h, after, metadata are stoked, then written by a function named writeDynspecMetadata contained in
///   	 Stock_Write_Dynspec_Metadata_Part.h
///   	 
///   	 Finaly, data are processed. Because LOFAR data are very voluminous, we have to develop a strategy for:
/// <br />   	 - avoid swaping
/// <br />   	 - have enough memory for loading data
/// <br />   	 etc ...
///   	 We decide to use a frozen quantity of RAM for a processing. This quantity is chosen by the user and corresponds to a number of time bins that we can process
///   	 for this RAM.
///   	 So, the code will loop of the number of Time series we need to process all data. no rebinning is done inside these blocks (see ICD3-ICD6-Rebin module). 
///   	 To conclude, data processing is done time blocks by time blocks, and data generation is done by the function  writeDynspecData which is included in the class 
///   	 Stock_Write_Dynspec_Data.h
///	 All data contained in ICD3 observation are processed


#include <iostream>
#include <string>
#include <fstream>
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
/// <br />obsNofBeam Number of BEAM (i.e signals from a station or a correlated sum) in the observation directory
/// <br />obsNofStockes Number of Stokes paramters contained in the observation directory
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
  

  // Test of existing files (if files are missing ....  => Find the first readable h5 file for getting Root metadata
  
  string pathFile0(pathDir+obsName);
  string pathFile(pathFile0+"_SAP000_B000_S0_P000_bf.h5");
  
  int testIndicator(0),i_indicator(0),j_indicator(0);
  string index_itest,index_jtest,index_ktest;
  
      for (int itest=0;itest<244;itest++)
	  {if (testIndicator != 0){break;}
	    for (int jtest=0;jtest<244;jtest++)
	      {if (testIndicator != 0){break;}
		for (int ktest=0;ktest<4;ktest++)
		  {
			if (testIndicator != 0){break;}
			else
			{
			std::ostringstream oss_i;oss_i << itest;string index_itest(oss_i.str());
			if (itest<10){index_itest="_SAP00"+index_itest;}
			if (itest>=10 && itest<100){index_itest="_SAP0"+index_itest;}
			if (itest>=100 && itest<1000){index_itest="_SAP"+index_itest;}
			
			std::ostringstream oss_j;oss_j << jtest;string index_jtest(oss_j.str()); 
			if (jtest<10){index_jtest="_B00"+index_jtest;}
			if (jtest>=10 && jtest<100){index_jtest="_B0"+index_jtest;}
			if (jtest>=100 && jtest<1000){index_jtest="_B"+index_jtest;}      
			
			std::ostringstream oss_ktest;oss_ktest << ktest;string index_ktest(oss_ktest.str()); 
			index_ktest="_S"+index_ktest;
 		    
			pathFile = pathFile0+index_itest+index_jtest+index_ktest+"_P000_bf.h5";						
			if (is_readableTer(pathFile)) 
			      {testIndicator++; i_indicator = itest, j_indicator = jtest;}
			}
		  }
	      }
	  }
	  
  // => First right file has been found
  // end of first right file determination
  

  ////////////////////////////////////////////////////////////////////////////////////////    
  // Number of Beam, Stokes and Stokes Component Determination 
  
  if (testIndicator != 0)
  {
 
  // Re-Initialization of index_itest, index_jtest & index_ktest indicator !! (values are lost in the previous loop)

  std::ostringstream oss_i;oss_i << i_indicator;string index_itest(oss_i.str());
  if (i_indicator<10){index_itest="_SAP00"+index_itest;}
  if (i_indicator>=10 && i_indicator<100){index_itest="_SAP0"+index_itest;}
  if (i_indicator>=100 && i_indicator<1000){index_itest="_SAP"+index_itest;} 
  
  std::ostringstream oss_j;oss_j << j_indicator;string index_jtest(oss_j.str()); 
  if (j_indicator<10){index_jtest="_B00"+index_jtest;}
  if (j_indicator>=10 && j_indicator<100){index_jtest="_B0"+index_jtest;}
  if (j_indicator>=100 && j_indicator<1000){index_jtest="_B"+index_jtest;}      
    
   
  // Load  pathFile
  BF_File file(pathFile);  							// generate an object called file contains Root
  BF_SubArrayPointing SAP = file.subArrayPointing(i_indicator);		// generate an object called SAP contains 1 subarray pointing
  BF_BeamGroup BEAM	= SAP.beam(j_indicator); 				// generate an object called BEAM contains 1 beam					

  // Number of Stokes & Stokes Component determination
  int obsNofStockes(BEAM.nofStokes().get());
  vector<string> stokesComponent(BEAM.stokesComponents().get()); 
  
  // SAP determination
  
  int obsNofSAP(file.observationNofSubArrayPointings().get());
  int obsNofBeam(SAP.observationNofBeams().get());  
  vector<float> raPoint(obsNofSAP),decPoint(obsNofSAP),sapFrequencyCenter(obsNofSAP);
  
  // Another indicator for existency
  int testIndicator2(0);
  
  string pathFile2(pathFile);
  for (i=i_indicator;i<obsNofSAP;i++)
    { 
      std::ostringstream oss;oss << i;string index(oss.str());
      if (i<10){index="_SAP00"+index+"_B000_S0_P000_bf.h5";}
      if (i>=10 && i<100){index="_SAP0"+index+"_B000_S0_P000_bf.h5";}
      if (i>=100 && i<1000){index="_SAP"+index+"_B000_S0_P000_bf.h5";}
       
      if (i == 0)
	{
	  BF_File file(pathFile2);
	  BF_SubArrayPointing SAP = file.subArrayPointing(i);
	  BF_BeamGroup BEAM	= SAP.beam(j_indicator); 	
	  raPoint[i]=SAP.pointRA().get();
	  decPoint[i]=SAP.pointDEC().get(); 
	  sapFrequencyCenter[i]=BEAM.beamFrequencyCenter().get();
	}
	else  
	{   
	    for (int jtest=0;jtest<244;jtest++)
	      {
		for (int ktest=0;ktest<4;ktest++)
		  {	

			if (testIndicator2 != 0){break;}
			else
			{  
			
			
			std::ostringstream oss;oss << i;string index(oss.str());
			if (i<10){index="_SAP00"+index;}
			if (i>=10 && i<100){index="_SAP0"+index;}
			if (i>=100 && i<1000){index="_SAP"+index;}			
			
			
			std::ostringstream oss_j;oss_j << jtest;string index_jtest(oss_j.str()); 
			if (jtest<10){index_jtest="_B00"+index_jtest;}
			if (jtest>=10 && jtest<100){index_jtest="_B0"+index_jtest;}
			if (jtest>=100 && jtest<1000){index_jtest="_B"+index_jtest;}      
			
			std::ostringstream oss_k;oss_k << ktest;string index_ktest(oss_k.str()); 
			index_ktest="_S"+index_ktest;
 		    
			
			pathFile2 = pathFile0+index+index_jtest+index_ktest+"_P000_bf.h5";
			
			if (is_readableTer(pathFile2)) 
			  {
			      BF_File file(pathFile2);
			      BF_SubArrayPointing SAP = file.subArrayPointing(i);
			      BF_BeamGroup BEAM	= SAP.beam(jtest); 	
			      raPoint[i]=SAP.pointRA().get();
			      decPoint[i]=SAP.pointDEC().get(); 
			      sapFrequencyCenter[i]=BEAM.beamFrequencyCenter().get();testIndicator2++;
			  }		      			  
		  }
	      }
	  }  // end else
	}// end else
    } // end for

     
  std::sort(sapFrequencyCenter.begin(), sapFrequencyCenter.end());

  int obsNofSource(1),obsNofFrequencyBand(1);
  for (i=0;i<obsNofSAP-1;i++)
    {
      if (raPoint[i] != raPoint[i+1] || decPoint[i] != decPoint[i+1]){obsNofSource++;}
      if (sapFrequencyCenter[i] != sapFrequencyCenter[i+1]){obsNofFrequencyBand++; cout << sapFrequencyCenter[i] << "  " << sapFrequencyCenter[i+1] << endl;}
    }
    
  int obsNofTiledDynspec(0),methodFlag(0);  
  if (obsNofFrequencyBand>1){obsNofTiledDynspec++;methodFlag=0;} 
             
      
  ////////////////////////////////////  
  // Determine the number of frequency bands using another nomenclature !!   	// methodFlag=0 => tiled dynspec using SAP
  // Using Pxxx at the end of the file name 					// methodFlag=1 => tiled dynspec using Pxxx
  
  if (obsNofTiledDynspec == 0)
    {
      i = 0;
  
      int pathFile0Size(pathFile.length());			// to be sure  
      string pathFileTemp(pathFile.substr(0,pathFile0Size-10));

      pathFile2 = pathFileTemp + "P000_bf.h5";  
      const char *filename = pathFile.c_str();
      int res(access(filename,F_OK));
      i++;
      
      while (res ==0)
	{
	  std::ostringstream oss;oss << i;string index(oss.str());
	  if (i<10){index="P00"+index;}
	  if (i>=10 && i<100){index="P0"+index;}
	  if (i>=100 && i<1000){index="P"+index;}
	  pathFile2 = pathFileTemp+index+"_bf.h5";
	  filename = pathFile2.c_str();
	  	  
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
      for (i=i_indicator;i<obsNofSAP;i++)  // Loop on SAP
	{
	  
	  // Test of existency for following SAP !!
	  if (i!= i_indicator)
	  {
	    std::ostringstream oss_i;oss_i << i;string index_itest(oss_i.str());
	    if (i_indicator<10){index_itest="_SAP00"+index_itest;}
	    if (i_indicator>=10 && i_indicator<100){index_itest="_SAP0"+index_itest;}
	    if (i_indicator>=100 && i_indicator<1000){index_itest="_SAP"+index_itest;} 
	    
	    pathFile0=pathDir+obsName;
	    pathFile=pathFile0+index_itest+"_B000_S0_P000_bf.h5";
	    
	    int testIndicator3(0);
	    j_indicator=0;
		      
	    for (int jtest=0;jtest<244;jtest++)
		{if (testIndicator3 != 0){break;}
		for (int ktest=0;ktest<4;ktest++)
			{
			  if (testIndicator3 != 0){break;}
			  else
			  {
					      
			  std::ostringstream oss_j;oss_j << jtest;string index_jtest(oss_j.str()); 
			  if (jtest<10){index_jtest="_B00"+index_jtest;}
			  if (jtest>=10 && jtest<100){index_jtest="_B0"+index_jtest;}
			  if (jtest>=100 && jtest<1000){index_jtest="_B"+index_jtest;}      
			  
			  std::ostringstream oss_ktest;oss_ktest << ktest;string index_ktest(oss_ktest.str()); 
			  index_ktest="_S"+index_ktest;
					  
			  pathFile = pathFile0+index_itest+index_jtest+index_ktest+"_P000_bf.h5";
			  
			  if (is_readableTer(pathFile)) 
			  {testIndicator3++; j_indicator = jtest;}
			  }
			}
		}
			          
	  }
	  // end of existency test	  
	  
	  k = 0;
	  string index_i1;
	  std::ostringstream oss_i;oss_i << i;string index_i(oss_i.str());
	  if (i<10){index_i1="_SAP00"+index_i;}
	  if (i>=10 && i<100){index_i1="_SAP0"+index_i;}
	  if (i>=100 && i<1000){index_i1="_SAP"+index_i;}
	  
	  string outputFile(argv[3]);
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
	  rootObject->readRoot(pathFile,obsName,rootMetadata,obsNofSource,obsNofBeam,obsNofFrequencyBand,obsNofTiledDynspec,SAPname, SAPindex, j_indicator);//,obsNofSource,obsNofBeam,obsNofStockes,obsNofFrequencyBand,obsNofTiledDynspec,obsMinFrequency,obsMaxFrequency,obsCentralFrequency);
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
	  
	  // Test of existency for following SAP !!
	  if (i!= i_indicator)
	  {
	    std::ostringstream oss_i;oss_i << i;string index_itest(oss_i.str());
	    if (i_indicator<10){index_itest="_SAP00"+index_itest;}
	    if (i_indicator>=10 && i_indicator<100){index_itest="_SAP0"+index_itest;}
	    if (i_indicator>=100 && i_indicator<1000){index_itest="_SAP"+index_itest;} 
	    
	    pathFile0=pathDir+obsName;
	    pathFile=pathFile0+index_itest+"_B000_S0_P000_bf.h5";
	    
	    int testIndicator3(0);
	    j_indicator=0;
		      
	    for (int jtest=0;jtest<244;jtest++)
		{if (testIndicator3 != 0){break;}
		for (int ktest=0;ktest<4;ktest++)
			{
			  if (testIndicator3 != 0){break;}
			  else
			  {
					      
			  std::ostringstream oss_j;oss_j << jtest;string index_jtest(oss_j.str()); 
			  if (jtest<10){index_jtest="_B00"+index_jtest;}
			  if (jtest>=10 && jtest<100){index_jtest="_B0"+index_jtest;}
			  if (jtest>=100 && jtest<1000){index_jtest="_B"+index_jtest;}      
			  
			  std::ostringstream oss_ktest;oss_ktest << ktest;string index_ktest(oss_ktest.str()); 
			  index_ktest="_S"+index_ktest;
					  
			  pathFile = pathFile0+index_itest+index_jtest+index_ktest+"_P000_bf.h5";
			  
			  if (is_readableTer(pathFile)) 
			  {testIndicator3++; j_indicator = jtest;}
			  }
			}
		}
			          
	  }	  
	  
	  
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
	  rootObject->readRoot(pathFile,obsName,rootMetadata,obsNofSource,obsNofBeam,obsNofFrequencyBand,obsNofTiledDynspec,SAPname, SAPindex, j_indicator);//,obsNofSource,obsNofBeam,obsNofStockes,obsNofFrequencyBand,obsNofTiledDynspec,obsMinFrequency,obsMaxFrequency,obsCentralFrequency);
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

  }  // end if there are no valid files !!
  else
    {
     cout << "WARNING !!:  THIS OBSERVATION IS COMPLETELY EMPTY, NO VALID *.h5 FILES !!" << endl;
     cout << "  " << endl;
    }
    
  } // end if parameter is right
    
    
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; 

  cout << endl;
  cout << "Duration of processing: " << cpu_time_used << " s" << endl;
  cout << endl;
  
  return 0;
}
