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


   bool is_readableTer( const std::string & file ) 
  { 
    std::ifstream fichier( file.c_str() ); 
    return !fichier.fail(); 
  } 



int main(int argc, char *argv[])
{
  // Time CPU computation  
  clock_t start, end;
  double cpu_time_used;  
  start = clock();
  

  ////////////////////////////////////////////////////////////////////////////////////////  
  //INPUT PARAMETERS
  
  
  if (argc< 11 ){cout << "   " << endl; cout << "missing parameter: USAGE: Dynspec  Observation-Path-DIR  Observation-Number(ex:Lxxxxx) Output-hdf5-DIR TimeMin TimeMax TimeRebin(s/pixel) FrequencyMin FrequencyMax FrequencyRebin(MHz/pixel) RebinAll(yes or no) [Facultative: Dataset-RAM-allocation (in Go; default value: 1Go) NumberOfSAP(if No, number of SAP to rebin)] " << endl;}
  if (argc> 13 ){cout << "   " << endl; cout << "too much parameters: USAGE: Dynspec  Observation-Path-DIR  Observation-Number(ex:Lxxxxx) Output-hdf5-DIR TimeMin TimeMax TimeRebin(s/pixel) FrequencyMin FrequencyMax FrequencyRebin(MHz/pixel) RebinAll(yes or no) [Facultative: Dataset-RAM-allocation (in Go; default value: 1Go) Number-Of-SAP(if RebinAll=No, number of SAP to rebin is needed)] " << endl;}
    
  if (argc <= 13 && argc >= 11)
  {
  string rebinAllDynspec(argv[10]);  
  if (rebinAllDynspec ==  "no" && argc < 13){cout << "   " << endl; cout << "For process only ONE SAP, using RebinAll=no, you MUST specified the number of the SAP to rebin and also the RAM parameter" << endl;}
  else
  {  
    
  string pathDir(argv[1]);
  string obsName(argv[2]);
  
  float memoryRAM(1);
  if (argc== 12 or argc== 13){memoryRAM=atof(argv[11]);};
  
  float timeMinSelect(atof(argv[4]));
  float timeMaxSelect(atof(argv[5]));
  float timeRebin(atof(argv[6]));
  
  float frequencyMin(atof(argv[7]));
  float frequencyMax(atof(argv[8]));
  float frequencyRebin(atof(argv[9]));
  
  string rebinAllDynspec(argv[10]);
  int SAPNumber(atof(argv[12]));
  
  

  ////////////////////////////////////////////////////////////////////////////////////////    
  // PARAMETERS Determination


  int i(0);  // loop index initilization
  
  ////////////////////////////////////  
  // Stockes component determination

  
  // Test of existing files (if files are missing ....
  
  string pathFile0(pathDir+obsName);
  string pathFile(pathFile0+"_SAP000_B000_S0_P000_bf.h5");
  
  int testIndicator(0),i_indicator(0),j_indicator(0);

  string index_itest,index_jtest,index_ktest;
  
  //if (testIndicator != 0){break;}
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
			      {testIndicator++; i_indicator = itest, j_indicator = jtest;cout << pathFile  << "  test indic:" << testIndicator  << endl; }
			}
		  }
	      }
	  }
    
	  	  
  // First right file found
  
  if (testIndicator != 0)
  {
  cout << "   " << endl;
  cout << pathFile  << endl;
  BF_File file(pathFile);  							// generate an object called file contains Root
  BF_SubArrayPointing SAP = file.subArrayPointing(i_indicator);				// generate an object called SAP contains 1 subarray pointing
  BF_BeamGroup BEAM	= SAP.beam(j_indicator); 						// generate an object called BEAM contains 1 beam					

  int obsNofStockes(BEAM.nofStokes().get());
  
  vector<string> stokesComponent(BEAM.stokesComponents().get()); 


  ////////////////////////////////////  
  // Others Parameters determination
    
  // SAP determination
  
  int obsNofSAP(file.observationNofSubArrayPointings().get());
  int obsNofBeam(SAP.observationNofBeams().get());  
  
  
  vector<float> raPoint(obsNofSAP),decPoint(obsNofSAP),sapFrequencyCenter(obsNofSAP);
  
  string pathFile2(pathFile);
  for (i=0;i<obsNofSAP;i++)
    { 
      std::ostringstream oss;oss << i;string index(oss.str());      
      if (i<10){index="_SAP00"+index+"_B000_S0_P000_bf.h5";}
      if (i>=10 && i<100){index="_SAP0"+index+"_B000_S0_P000_bf.h5";}
      if (i>=100 && i<1000){index="_SAP"+index+"_B000_S0_P000_bf.h5";}
       
      if (i == 0)
	{
	  BF_File file(pathFile2);
	  BF_SubArrayPointing SAP = file.subArrayPointing(i);
	  BF_BeamGroup BEAM	= SAP.beam(0); 	
	  raPoint[i]=SAP.pointRA().get();
	  decPoint[i]=SAP.pointDEC().get(); 
	  sapFrequencyCenter[i]=BEAM.beamFrequencyCenter().get()/1E6; 
	  
	}
	else  
	{
	  int testIndicator2(0);
	  while (testIndicator2 == 0)
	  {
	    for (int jtest=0;jtest<244;jtest++)
	      {
		for (int ktest=0;ktest<4;ktest++)
		  {			
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
			      sapFrequencyCenter[i]=BEAM.beamFrequencyCenter().get()/1E6;testIndicator2++;
			  }		      			  
		  }
	      }
	  break;
	  }  // end while
	}// end else
    } // end for

    
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
  
      int pathFile0Size(pathFile.length());			// to be sure  
      string pathFileTemp(pathFile.substr(0,pathFile0Size-10));

      string pathFile3(pathFileTemp + "P000_bf.h5");  
      const char *filename = pathFile3.c_str();
      int res(access(filename,F_OK));
      i++;
  
      //cout << filename << endl;
      
      while (res ==0)
	{
	  std::ostringstream oss;oss << i;string index(oss.str());
	  if (i<10){index="P00"+index;}
	  if (i>=10 && i<100){index="P0"+index;}
	  if (i>=100 && i<1000){index="P"+index;}
	  pathFile3 = pathFileTemp+index+"_bf.h5";
	  filename = pathFile3.c_str();
	  res = access(filename,F_OK);
	  if (res == 0){obsNofFrequencyBand++;i++;}	}

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
    */ 


  ////////////////////////////////////////////////////////////////////////////////////////
  // SAP  PART  => Each SAP => One ICD6 hdf5 file 
  
  
  if (rebinAllDynspec == "no"){obsNofSAP = 1;}

  
  int k(0),l(0),q(0);  // Dynspec iterator

  
  // Dynspec using SAP method

  if (methodFlag == 0)
    {

      int j(0);
      // SAP method
      for (i=i_indicator;i<obsNofSAP;i++)  // Loop on SAP
	{
	  if (rebinAllDynspec == "no"){i=SAPNumber;}
	  
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
	  Stock_Write_Root_Metadata_Part *rootMetadata;
	  rootMetadata = new Stock_Write_Root_Metadata_Part();
	  // generation of an object which will read ROOT METADATA
	  Reader_Root_Part *rootObject;
	  rootObject = new Reader_Root_Part();
   
	  
	  string SAPname(index_i1);
	  int SAPindex(i);
	  
	  // extract root metadata from the file and the number of stations  
	  rootObject->readRoot(pathFile,obsName,rootMetadata,obsNofSource,obsNofBeam,obsNofFrequencyBand,obsNofTiledDynspec,SAPname, SAPindex,timeMinSelect,timeMaxSelect,timeRebin,frequencyMin,frequencyMax,frequencyRebin);//,obsNofSource,obsNofBeam,obsNofStockes,obsNofFrequencyBand,obsNofTiledDynspec,obsMinFrequency,obsMaxFrequency,obsCentralFrequency);
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
		    pathFile = pathDir+obsName;
		    dynspecObject->readDynspec(pathFile,dynspecMetadata,dynspecData,i,j,q,obsNofSAP,obsNofSource,obsNofFrequencyBand,obsNofStockes,stokesComponent,SAPindex,timeMinSelect,timeMaxSelect,timeRebin,frequencyMin,frequencyMax,frequencyRebin);
		    delete dynspecObject;

		    // Write METADATA
		    dynspecMetadata->writeDynspecMetadata(dynspec_grp,pathDir,obsName,pathFile,outputFile,root_grp,i,j,k,l,q,obsNofStockes,stokesComponent,memoryRAM);
		    delete dynspecMetadata;

		    //WRITE DATA
		    dynspecData->writeDynspecData(dynspec_grp,pathDir,obsName,pathFile,outputFile,root_grp,i,j,k,l,q,obsNofStockes,stokesComponent,memoryRAM,SAPindex, timeMinSelect, timeMaxSelect, timeRebin, frequencyMin, frequencyMax, frequencyRebin);
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
      for (i=i_indicator;i<obsNofSAP;i++)  // Loop on SAP
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
	  Stock_Write_Root_Metadata_Part *rootMetadata;
	  rootMetadata = new Stock_Write_Root_Metadata_Part();
	  // generation of an object which will read ROOT METADATA
	  Reader_Root_Part *rootObject;
	  rootObject = new Reader_Root_Part();
   
	  
	  string SAPname(index_i1);
	  int SAPindex(i);
	  
	  // extract root metadata from the file and the number of stations  
	  rootObject->readRoot(pathFile,obsName,rootMetadata,obsNofSource,obsNofBeam,obsNofFrequencyBand,obsNofTiledDynspec,SAPname, SAPindex,timeMinSelect,timeMaxSelect,timeRebin,frequencyMin,frequencyMax,frequencyRebin);
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
		    Reader_Dynspec_Part *dynspecObject;
		    dynspecObject= new Reader_Dynspec_Part();
		    
		    // generation of an object which will contains and write DYNSPEC METADATA  
		    Stock_Write_Dynspec_Metadata_Part *dynspecMetadata;
		    dynspecMetadata = new Stock_Write_Dynspec_Metadata_Part();

		    // generation of an object which will contains and write DYNSPEC DATA  
		    Stock_Write_Dynspec_Data_Part *dynspecData;
		    dynspecData = new Stock_Write_Dynspec_Data_Part();
		    
		    
		    // extract dynspec metadata from the file	      
		    pathFile = pathDir+obsName;
		    dynspecObject->readDynspec(pathFile,dynspecMetadata,dynspecData,i,j,q,obsNofSAP,obsNofSource,obsNofFrequencyBand,obsNofStockes,stokesComponent,SAPindex,timeMinSelect,timeMaxSelect,timeRebin,frequencyMin,frequencyMax,frequencyRebin);
		    delete dynspecObject;
		    
		    // Write METADATA
		    dynspecMetadata->writeDynspecMetadata(dynspec_grp,pathDir,obsName,pathFile,outputFile,root_grp,i,j,k,l,q,obsNofStockes,stokesComponent,memoryRAM);
		    delete dynspecMetadata;
		    
		    //WRITE DATA
		    dynspecData->writeDynspecData(dynspec_grp,pathDir,obsName,pathFile,outputFile,root_grp,i,j,k,l,q,obsNofStockes,stokesComponent,memoryRAM,SAPindex, timeMinSelect, timeMaxSelect, timeRebin, frequencyMin, frequencyMax, frequencyRebin);
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
    
    
  } // end else parameter is right
  } // End if parameters is right
  
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; 

  cout << endl;
  cout << "Duration of processing: " << cpu_time_used << " s" << endl;
  cout << endl;
  
  return 0;
}
