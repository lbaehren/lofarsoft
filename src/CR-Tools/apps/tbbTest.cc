/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Andreas Horneffer (<mail>)                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// general includes
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

// CGuipipeline includes
#include <crtools.h>
#include <Analysis/analyseLOPESevent.h>
#include <Analysis/tbbTools.h>
#include <Calibration/RFIMitigationPlugin.h>
#include <Data/tbbctlIn.h>
#include <IO/LOFAR_TBB.h>

// Glish includes
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <scimath/Mathematics/FFTServer.h>

/*!
  \file tbbTest.cc
  
  \ingroup CR_Applications

  \brief A collection of routines to test/simulate the TBB behavior
  
  \author Andreas Horneffer
  
  \date 2008/07/09
  
  <h3>Prerequisite</h3>
  
  - analyseLOPESevent 
  - tbbctlIn
  
  <h3>Synopsis</h3>
  
  <h3>Example(s)</h3>
  
*/

using namespace CR;

// global Variable for a pipeline
analyseLOPESevent pipeline;

// global Variable for tbb data that actually contains all the data
tbbctlIn tbbIn;
LOFAR_TBB *hdf5In;

// local copies (of the pointers) to access the stuff.
CRinvFFT *pipeline_p;    
DataReader *DataReader_p;


//-----------------------------------------------------------  configCheckBool
Bool configCheckBool(string value, string keyword){
  if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
    return True;
  } else {
    if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
      return False;
    } else {
      std::cerr << "\nError processing configfile:\n" ;
      std::cerr << "Keyword \"" << keyword << "\" value: \"" << value << "\"\n";
      std::cerr << "Not recognized as boolean! Assuming False!\n";
      return False;
    };
  };
};
//-----------------------------------------------------------  configReadArray
Bool configReadArray(char *line, Vector<double> &outArray){
  try {
    float values[10];
    char tmpstring[4096];
    int nconv=0;
    nconv = sscanf(line,"%s = %f %f %f %f %f %f %f %f %f %f", tmpstring,
		   values+0, values+1, values+2, values+3, values+4,
		   values+5, values+6, values+7, values+8, values+9);
    if (nconv<2) {
      cout << "configReadArray: Error reading in array, no values found!" << endl;
      return False;
    };
    outArray.resize(nconv-1);
    for (int i=0; i<(nconv-1); i++){
      outArray(i) = values[i];
    };
  } catch (AipsError x) {
    cerr << "tbbTest:configReadArray " << x.getMesg() << endl;
    return False;
  };   
  return True;
};


//-----------------------------------------------------------  parseConfigFile
Record parseConfigFile(String configfilename){
  Record conf;
  try {
    Vector<double> tmpvec;
    conf.define("caltablepath","/mnt/lofar/CS1_tbb/LOFAR_CS1_tempCalTable");
    conf.define("outfile","tbbTest.out");
    conf.define("filetype","tbbctl");
    conf.define("doGenDynSpec",False);
    conf.define("doGenInputStatistics",False);
    conf.define("doSimTBBTrigger",False);
    conf.define("level",8);
    conf.define("start",5);
    conf.define("stop",2);
    conf.define("window",4096);
    conf.define("afterwindow",0);
    conf.define("doRFImitigation",False);
    conf.define("doFiltering",False);
    conf.define("addData",False);
    conf.define("maxsize",5000000);
    conf.define("blocksize",5000000);
    conf.define("filterTypes",tmpvec);
    conf.define("centerFreqs",tmpvec);
    conf.define("bandwidths",tmpvec);

    if (configfilename.empty()) {
      return conf;  //return default config.
    };

    ifstream configfile;
    configfile.open (configfilename.c_str(), ifstream::in);
    // check if file could be opened
    if (!(configfile.is_open())) {
      std::cerr << "Failed to open file \"" << configfilename <<"\"." << std::endl;
      return conf;		
    }
    // look for the beginnig of the config data (after a line containing only and at least three '-' or '=' 
    string temp_read;
    bool configs_found = false;
    while ((configs_found == false) && (configfile.good())) {
      configfile >> temp_read;
      if ((temp_read.find("---") != string::npos) || (temp_read.find("===") != string::npos)) {
	configs_found = true;
      };
    };
    
    // print warning if no configs were found and countinue program using default values
    if (configs_found == false) {
      configfile.close();  // close file
      std::cerr << "\nWarning!";
      std::cerr << "\nNo config information was foung in file \"" << configfilename <<"\".\n" ;
      std::cerr << "Use the following file format ('=' seperated by spaces): \n\n";
      std::cerr << "some lines of text\n";
      std::cerr << "===================================\n";
      std::cerr << "doGenDynSpec = False\n";
      std::cerr << "doGenInputStatistics = False\n";
      std::cerr << "doSimTBBTrigger = False\n";
      std::cerr << "... \n";
      std::cerr << "\nProgram will continue using default configuration values." << std::endl;
      return conf;
    };
    char configLine[4096], ckeyword[4096], cvalue[4096];
    while(configfile.good()) { // read configurations if configs_found
      string keyword, value;
      // read in a line from the configfile and look for keyword and value
      configfile.getline(configLine, 4096);
      if (strlen(configLine) < 3) continue;
      if (sscanf(configLine,"%s = %s ",ckeyword,cvalue) < 2){
	std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
	std::cerr << "Could not parse line: \"" << configLine << "\".\n";
	std::cerr << "Program will continue skipping the problem.\n" << std::endl;
	continue;	
      };
      keyword = string(ckeyword);
      value = string(cvalue);
      
      // check if end of file occured:
      // keyword should contain "", if file is terminated be a new line
      if (keyword == "") continue;	// go back to begin of while-loop 
					// configfile.good() should be false now.
      
      // check keywords an set appropriate configurations
      if ( (keyword.compare("doGenDynSpec")==0) ) {
	conf.define(casa::RecordFieldId(keyword),configCheckBool(value,keyword) );
	std::cout << keyword << " set to " << conf.asBool(casa::RecordFieldId(keyword)) << ".\n";
	continue;	
	//	  std::cout << "" << " set to " << conf.asBool("") << ".\n";
      };	
      if ( (keyword.compare("doGenInputStatistics")==0) ) {
	conf.define(casa::RecordFieldId(keyword),configCheckBool(value,keyword) );
	std::cout << keyword << " set to " << conf.asBool(casa::RecordFieldId(keyword)) << ".\n";
	continue;	
      };	
      if ( (keyword.compare("doSimTBBTrigger")==0) ) {
	conf.define(casa::RecordFieldId(keyword),configCheckBool(value,keyword) );
	std::cout << keyword << " set to " << conf.asBool(casa::RecordFieldId(keyword)) << ".\n";
	continue;	
      };	
      if ( (keyword.compare("doRFImitigation")==0) ) {
	conf.define(casa::RecordFieldId(keyword),configCheckBool(value,keyword) );
	std::cout << keyword << " set to " << conf.asBool(casa::RecordFieldId(keyword)) << ".\n";
	continue;	
      };	
      if ( (keyword.compare("doFiltering")==0) ) {
	conf.define(casa::RecordFieldId(keyword),configCheckBool(value,keyword) );
	std::cout << keyword << " set to " << conf.asBool(casa::RecordFieldId(keyword)) << ".\n";
	continue;	
      };	
      if ( (keyword.compare("addData")==0) ) {
	conf.define(casa::RecordFieldId(keyword),configCheckBool(value,keyword) );
	std::cout << keyword << " set to " << conf.asBool(casa::RecordFieldId(keyword)) << ".\n";
	continue;	
      };	
            
      if ( (keyword.compare("caltablepath")==0) ){
	conf.define(keyword,value);
	std::cout << keyword << " set to " << conf.asString(keyword) << endl;
	continue;     
      };
      if ( (keyword.compare("outfile")==0) ){
	conf.define(keyword,value);
	std::cout << keyword << " set to " << conf.asString(keyword) << endl;
	continue;     
      };
      if ( (keyword.compare("filetype")==0) ){
	conf.define(keyword,value);
	std::cout << keyword << " set to " << conf.asString(keyword) << endl;
	continue;     
      };

      if ( (keyword.compare("level")==0) ){
	Int temp = 9999999;
	stringstream(value) >> temp; 
	if (temp != 9999999) {  // will be false, if value is not of typ "int"
	  conf.define(casa::RecordFieldId(keyword),temp);
	  std::cout << keyword << " set to " << conf.asInt(casa::RecordFieldId(keyword)) << ".\n";	    
	} else {
	  std::cerr << "\nError processing configfile:\n";
	  std::cerr << "Keyword \"" << keyword << "\" value: \"" << value << "\"\n";
	  std::cerr << "No valid integer.\n";
	};
	continue;	
      };
      
      if ( (keyword.compare("start")==0) ){
	Int temp = 9999999;
	stringstream(value) >> temp; 
	if (temp != 9999999) {  // will be false, if value is not of typ "int"
	  conf.define(casa::RecordFieldId(keyword),temp);
	  std::cout << keyword << " set to " << conf.asInt(casa::RecordFieldId(keyword)) << ".\n";	    
	} else {
	  std::cerr << "\nError processing configfile:\n" ;
	  std::cerr << "Keyword \"" << keyword <<"\" value: \"" << value << "\"\n";
	  std::cerr << "No valid integer.\n";
	};
	continue;	
      };
      
      if ( (keyword.compare("stop")==0) ){
	Int temp = 9999999;
	stringstream(value) >> temp; 
	if (temp != 9999999) {  // will be false, if value is not of typ "int"
	  conf.define(casa::RecordFieldId(keyword),temp);
	  std::cout << keyword << " set to " << conf.asInt(casa::RecordFieldId(keyword)) << ".\n";	    
	} else {
	  std::cerr << "\nError processing configfile:\n" ;
	  std::cerr << "Keyword \"" << keyword <<"\" value: \"" << value << "\"\n";
	  std::cerr << "No valid integer.\n";
	};
	continue;	
      };
      
      if ( (keyword.compare("window")==0) ){
	Int temp = 9999999;
	stringstream(value) >> temp; 
	if (temp != 9999999) {  // will be false, if value is not of typ "int"
	  conf.define(casa::RecordFieldId(keyword),temp);
	  std::cout << keyword << " set to " << conf.asInt(casa::RecordFieldId(keyword)) << ".\n";	    
	} else {
	  std::cerr << "\nError processing configfile:\n" ;
	  std::cerr << "Keyword \"" << keyword <<"\" value: \"" << value << "\"\n";
	  std::cerr << "No valid integer.\n";
	};
	continue;	
      };
      
      if ( (keyword.compare("afterwindow")==0) ){
	Int temp = 9999999;
	stringstream(value) >> temp; 
	if (temp != 9999999) {  // will be false, if value is not of typ "int"
	  conf.define(casa::RecordFieldId(keyword),temp);
	  std::cout << keyword << " set to " << conf.asInt(casa::RecordFieldId(keyword)) << ".\n";	    
	} else {
	  std::cerr << "\nError processing configfile:\n" ;
	  std::cerr << "Keyword \"" << keyword <<"\" value: \"" << value << "\"\n";
	  std::cerr << "No valid integer.\n";
	};
	continue;	
      };

      if ( (keyword.compare("maxsize")==0) ){
	Int temp = -1;
	stringstream(value) >> temp; 
	if (temp != -1) {  // will be false, if value is not of typ "int"
	  conf.define(casa::RecordFieldId(keyword),temp);
	  std::cout << keyword << " set to " << conf.asInt(casa::RecordFieldId(keyword)) << ".\n";	    
	} else {
	  std::cerr << "\nError processing configfile:\n" ;
	  std::cerr << "Keyword \"" << keyword <<"\" value: \"" << value << "\"\n";
	  std::cerr << "No valid integer.\n";
	};
	continue;	
      };

      if ( (keyword.compare("blocksize")==0) ){
	Int temp = -1;
	stringstream(value) >> temp; 
	if (temp != -1) {  // will be false, if value is not of typ "int"
	  conf.define(casa::RecordFieldId(keyword),temp);
	  std::cout << keyword << " set to " << conf.asInt(casa::RecordFieldId(keyword)) << ".\n";	    
	} else {
	  std::cerr << "\nError processing configfile:\n" ;
	  std::cerr << "Keyword \"" << keyword <<"\" value: \"" << value << "\"\n";
	  std::cerr << "No valid integer.\n";
	};
	continue;	
      };
      
      if ( (keyword.compare("filterTypes")==0) ){
	if (configReadArray(configLine, tmpvec)){
	  conf.define(casa::RecordFieldId(keyword),tmpvec);
	  std::cout << keyword << " set to " << conf.asArrayDouble(casa::RecordFieldId(keyword)) << ".\n";
	} else {
	  std::cerr << "\nError processing configfile:\n" ;
	  std::cerr << "Keyword \"" << keyword <<"\" value: \"" << value << "\"\n";
	  std::cerr << "No valid array of double.\n";
	};
	continue;
      };

      if ( (keyword.compare("centerFreqs")==0) ){
	if (configReadArray(configLine, tmpvec)){
	  conf.define(casa::RecordFieldId(keyword),tmpvec);
	  std::cout << keyword << " set to " << conf.asArrayDouble(casa::RecordFieldId(keyword)) << ".\n";
	} else {
	  std::cerr << "\nError processing configfile:\n" ;
	  std::cerr << "Keyword \"" << keyword <<"\" value: \"" << value << "\"\n";
	  std::cerr << "No valid array of double.\n";
	};
	continue;
      };

      if ( (keyword.compare("bandwidths")==0) ){
	if (configReadArray(configLine, tmpvec)){
	  conf.define(casa::RecordFieldId(keyword),tmpvec);
	  std::cout << keyword << " set to " << conf.asArrayDouble(casa::RecordFieldId(keyword)) << ".\n";
	} else {
	  std::cerr << "\nError processing configfile:\n" ;
	  std::cerr << "Keyword \"" << keyword <<"\" value: \"" << value << "\"\n";
	  std::cerr << "No valid array of double.\n";
	};
	continue;
      };

      //No "if (keyword.compare())" caught, so unknown keyword!
      std::cerr << "Unknown keyword \"" << keyword << "\" ignored!\n";

    } // while(configfile.good())
    // close config file
    configfile.close();

  } catch (AipsError x) {
    cerr << "tbbTest:parseConfigFile " << x.getMesg() << endl;
    return conf;
  };   
  return conf;
};

//-------------------------------------------------------  readEventfilelistFile
Vector<String> readEventfilelistFile(String eventsFileName){
  Vector<String> eventlist(0);
  try {
    if (eventsFileName.empty()) {
      return eventlist;  //return empty string
    };

    ifstream eventsfile;
    eventsfile.open(eventsFileName.c_str(), ifstream::in);
    // check if file could be opened
    if (!(eventsfile.is_open())) {
      std::cerr << "Failed to open file \"" << eventsFileName <<"\"." << std::endl;
      return eventlist;		
    }
    // look for the beginnig of the event data (after a line containing only and at least three '-' or '=' 
    string temp_read;
    bool events_found = false;
    while ((events_found == false) && (eventsfile.good())) {
      eventsfile >> temp_read;
      if ((temp_read.find("---") != string::npos) || (temp_read.find("===") != string::npos)) {
	events_found = true;
      };
    };
    int vecsize=100,nfiles=0;
    eventlist.resize(vecsize);
    while(eventsfile.good()) { 
      eventsfile >> temp_read;
      if (temp_read.find(".") != string::npos) { //dot in the string, so assume its valid
	eventlist(nfiles) = temp_read;
	nfiles++;
      } else {
	cerr << "tbbTest:readEventfilelistFile \"" << temp_read 
	     << "\" doesn't look like a valid file!" << endl;
      };
      if (nfiles>=vecsize) { // need to increase the size of the vector
	vecsize = vecsize+100;
	eventlist.resize(vecsize,True);
      };
    };
    eventlist.resize(nfiles,True); //set the sice of the vector to the number of files.
  } catch (AipsError x) {
    cerr << "tbbTest:readEventfilelistFile " << x.getMesg() << endl;
    return Vector<String>();
  };   
  return eventlist;
};


//-----------------------------------------------------------  initPipeline
Bool initPipeline(String caltablepath){
  try {
    Record obsrec;
    obsrec.define("LOFAR",caltablepath);
    if (!pipeline.initPipeline(obsrec)){
      cerr << "tbbTest:initPipeline: " << "Error while initializing the pipeline." << endl;
      return False;
    } else {
      pipeline_p = pipeline.GetPipeline();
      DataReader_p = pipeline.GetDataReader();
      pipeline_p->setVerbosity(False);
    };
  } catch (AipsError x) {
    cerr << "tbbTest:initPipeline: " << x.getMesg() << endl;
    return False;
  };   
  return True;
};

//-----------------------------------------------------------  SimTBBTrigger
Bool SimTBBTrigger(Record confRec, Vector<String> files){
  try {
    tbbTools triggerTool;
    tbbTools *filterTools=NULL;
    Int i;
    Vector<Double> Fc,BW,FilterTypes,data2add(0);
    // Local RFI mitigation object
    CR::RFIMitigationPlugin rfiM_p;
    //Vector<Int> FilterTypes;
    Double resolution=1024.;

    Int fnum,numFiles=files.nelements(),block,numblocks,newpeaks,nfilepeaks,samplenumber,secadd;
    Vector<Int> index,sum,width,peak,meanval,afterval;
    Vector<Int> antIDs;
    uint date,startSample,filesize; 
    Vector<Double> data;

    FILE *outfile;
    outfile = fopen(confRec.asString("outfile").c_str(),"w");
    for (fnum=0; fnum<numFiles; fnum++){ //start of loop over files
      // initialize the Data Reader
      if (confRec.asString("filetype") == "tbbctl") {
	if (! tbbIn.attachFile(files(Slice(fnum,1))) ){
	  cerr << "tbbTest:SimTBBTrigger: " << "Failed to attach file: " << files(fnum) << endl;
	  continue;
	};
	DataReader_p = &tbbIn;
	filesize = DataReader_p->blocksize();
      } else if (confRec.asString("filetype") == "LOFAR") {
	if ( hdf5In != NULL) { delete hdf5In; };
	//hdf5In = new LOFAR_TBB(files(fnum),8388608);
	hdf5In = new LOFAR_TBB(files(fnum),524288);
	if ( hdf5In == NULL) { 
	  cerr << "tbbTest:SimTBBTrigger: " << "Failed to create new  LOFAR_TBB object!"<< endl;  
	};
	//hdf5In->summary(True,True);
	//cout << hdf5In->times() << endl << endl;
	if ( hdf5In->blocksize() == 0 ){
	  cerr << "tbbTest:SimTBBTrigger: " << "Failed to attach file: " << files(fnum) << endl;
	  continue;
	};
	//cout << endl << hdf5In->headerRecord() <<endl;
	hdf5In->headerRecord().get("Filesize",filesize);
	cout << "tbbTest:SimTBBTrigger: HDF5-Filesize: " << filesize << "!!!" << endl;
	{//patch to make it work with the old (dummy-)CalTable
	  hdf5In->headerRecord().get("AntennaIDs",antIDs);
	  antIDs = 65536;//+antIDs(0)%100;
	  Record tmprec;
	  tmprec.define("AntennaIDs",antIDs);
	  //patches for buggy/incomplete LOFAR_TBB class
	  //tmprec.define("StartSample",0);
	  tmprec.define("Observatory","LOFAR");
	  hdf5In->setHeaderRecord(tmprec);
	}
	DataReader_p = hdf5In;
	//cout << endl << hdf5In->attributes2record(true) <<endl;
      } else {
	cerr << "tbbTest:SimTBBTrigger: Unknown filetype:" << confRec.asString("filetype") 
	     << " aborting!" << endl;
	fclose(outfile);
	return False;
      };
      if (DataReader_p->blocksize() > (uInt)confRec.asInt("maxsize")){
	cerr << "tbbTest:SimTBBTrigger: " << "File: " << files(fnum) 
	     << " spans too large a range! (>"<< confRec.asInt("maxsize") 
	     << ",Hole inside?)" << endl;
	continue;
      };
      //handling of blocks
      if (filesize <= (uInt)confRec.asInt("blocksize")){
	numblocks = 1;
      } else {
	numblocks = filesize / confRec.asInt("blocksize");
	DataReader_p->setBlocksize(confRec.asInt("blocksize"));
      };
      if (numblocks>1) {
	cout << "split file " << files(fnum)<< " into " << numblocks << " blocks." <<endl;
      };
      //pipeline initialization
      if (! pipeline_p->InitEvent(DataReader_p)){
	cerr << "tbbTest:SimTBBTrigger: "  << "File: " << files(fnum)
	     << " failed to initialize the DataReader!" << endl;
	continue;
      };
      //Get header data
      DataReader_p->headerRecord().get("AntennaIDs",antIDs);
      //antIDs(0) = hdf5In->channelIDs()(0);
      DataReader_p->headerRecord().get("Date",date);
      DataReader_p->headerRecord().get("StartSample",startSample);
      nfilepeaks =0;

      for (block =0; block<numblocks; block++) { //start of loop over blocks  
	if (confRec.asBool("doRFImitigation")){
	  //	DataReader_p->setHanningFilter(0.);
	  Matrix<DComplex> fdata;
	  Vector<DComplex> tfdata;
	  Int blocksize=DataReader_p->blocksize(),fftlen=blocksize/2+1;
	  FFTServer<Double,DComplex> server(IPosition(1,blocksize),
					    FFTEnums::REALTOCOMPLEX);
	  data = DataReader_p->fx().column(0);
	  if (confRec.asBool("addData")){
	    data(Slice(0,data2add.nelements())) = data(Slice(0,data2add.nelements()))+data2add;
	  };
	  tfdata.resize(fftlen);
	  server.fft(tfdata,data);
	  fdata.resize(fftlen,1);
	  fdata.column(0) = tfdata;
	  rfiM_p.parameters().define("dataBlockSize",blocksize);
	  // Do the RFI mitigation
	  rfiM_p.apply(fdata,True);
	  data.resize(0);
	  server.fft(data,fdata.column(0));
	} else if (confRec.asBool("doFiltering")){
	  confRec.get("centerFreqs",Fc);
	  confRec.get("bandwidths",BW);
	  confRec.get("filterTypes",FilterTypes);
 	  int numFilters;
 	  data = DataReader_p->fx().column(0);
// 	  if (addData){
// 	    data(Slice(0,data2add.nelements())) = data(Slice(0,data2add.nelements()))+data2add;
// 	  };
 	  numFilters = Fc.nelements();
 	  if ((numFilters<1)||(numFilters != (int)BW.nelements())||(numFilters != (int)FilterTypes.nelements())) {
 	    cerr << "tbbTest:SimTBBTrigger: "  << "\"doFiltering\" with inconsistend parameters!" 
 		 << endl;
 	    if ((numFilters<1))
 	      cerr << "                                   "  << "empty \"centerFreqs\" paramter-vector." << endl;
 	    if ((numFilters != (int)BW.nelements())||(numFilters != (int)FilterTypes.nelements()))
 	      cerr << "                                   "  << "different length of parameters" << endl;
 	  } else {
	    if (block==0) {
	      if (filterTools != NULL){
		delete [] filterTools;
	      };
	      filterTools = new tbbTools[numFilters];
	    };
 	    for (i=0; i<numFilters; i++){
 	      if ((FilterTypes.nelements()<uInt(i+1))||(floor(FilterTypes(i)+0.5) == 1)){
		//Hardcode sample-rate of 200MHz (Fc and BW in MHZ!)
 		data = filterTools[i].FPGAfilterNotch(data, Fc(i), BW(i), 200., resolution, block==0); 
 	      } else if (floor(FilterTypes(i)+0.5) == 2) {
 		data = filterTools[i].FPGAfilterLPF(data, Fc(i), BW(i), 200., resolution, block==0);
 	      } else if (floor(FilterTypes(i)+0.5) == 3){
 		data = filterTools[i].FPGAfilterHPF(data, Fc(i), BW(i), 200., resolution, block==0);
 	      } else {
 		cout << "tbbTest:SimTBBTrigger: " << 
 		  "Unknown FilterTypes value:" << FilterTypes(i) << endl;
 		cout << "                                   " << 
 		  " 1: Notch ; 2: LPF ; 3: HPF; " << endl;
 	      };
 	    };
//  	    if (returnFiltered) {
//  	      output.define("FilteredData",data);
//  	    };
 	  };	
	} else {
	  data = DataReader_p->fx().column(0);
	  if (confRec.asBool("addData")){
	    data(Slice(0,data2add.nelements())) = data(Slice(0,data2add.nelements()))+data2add;
	  };
	};
//  	if (block == 0) {
//  	  cout << data(Slice(0,10)) << endl;
//  	}
// 	Bool reset= (block==0);
// 	cerr << "datalen:" << data.nelements() 
// 	     << " mean:" << mean(data) 
// 	     << " stddev:" << stddev(data) 
// 	     << " reset:" << reset << endl;	
	if (! triggerTool.meanFPGAtrigger(data,
				      confRec.asInt("level"), confRec.asInt("start"), 
				      confRec.asInt("stop"), confRec.asInt("window"), 
				      confRec.asInt("afterwindow"),
				      index, sum, width, peak, meanval, afterval,
				      (block==0))) { //block==0 -> reset if we work on the first block
	  cerr << "tbbTest:SimTBBTrigger: "  << "File: " << files(fnum)
	       << " failed during meanFPGAtrigger!" << endl;
	  continue;
	};
	
      //      cout << "file: " << files(fnum) << " Date: " << DataReader_p->headerRecord().asuInt("Date") <<" dDate: " << DataReader_p->headerRecord().asDouble("dDate") << endl;
	newpeaks = index.nelements();
	nfilepeaks += newpeaks;
	//cout << "Block: " << (block+1) << " npeaks: " << newpeaks << "  ";
	//if (((block+1)%5) == 0) { cout << endl; };
	for (i=0; i<newpeaks; i++){
	  samplenumber = (startSample+(block*confRec.asInt("blocksize"))+index(i));
	  secadd = samplenumber/200000000;
	  samplenumber = samplenumber - (secadd*200000000);
	  fprintf(outfile,"%9i %8i %10u %10i %8i %8i %8i %8i %8i \n",
		  antIDs(0),// RCU-number
		  1, // Sequence-number
		  (date+secadd), // Time
		  samplenumber, 
		  sum(i), 
		  width(i),
		  peak(i),
		  meanval(i),
		  afterval(i));
	};
	DataReader_p->nextBlock();
      }; //loop over blocks 
      //cout << endl;
      cout << "Total peaks found: " << nfilepeaks << endl;
      if (((fnum+1)%50)==0) {
	cout << "tbbTest:SimTBBTrigger: processed " << fnum+1 << " files out of " 
	     << numFiles << "!" << endl;
      };
    }; //loop over files
    fclose(outfile);
  } catch (AipsError x) {
    cerr << "tbbTest:SimTBBTrigger: " << x.getMesg() << endl;
    return False;
  };   
  return True;
};






int main (int argc, char *argv[]) {
  string eventfilelistname, configfilename;	// Files to be read in
  
  try {
    Record confRec;
    Vector<String> files;

    std::cout << "tbbTest execution started!" << endl;

    // Check correct number of arguments (1 or 2 + program name = 2 or 3)
    if ((argc < 2) || (argc > 3)) {
      std::cerr << "Wrong number of arguments in call of \"tbbTest\". The correct format is:\n";
      std::cerr << "tbbTest eventfilelist [configfile]\n" << std::endl;
      return 1;				// Exit the program
    };
    
    // First argument should be the name of the file containing the list of event files
    eventfilelistname.assign(argv[1]);
    std::cout << "File containing list of event files: " << eventfilelistname << std::endl;
    
    // If a second argument is available it sould contain the name of the config file
    if (argc == 3) {
      configfilename.assign(argv[2]);
      std::cout << "Reading config data from file: " << configfilename << std::endl;
      confRec = parseConfigFile(configfilename);
    } else {
      std::cout << "Using default configuration. " << std::endl;
      confRec = parseConfigFile(String());
    };

    //read in the list of events
    files = readEventfilelistFile(eventfilelistname);
    cout << "List of files:" << endl;
    cout << files <<endl;


    //initialize the pipeline
    if (!initPipeline(confRec.asString("caltablepath"))){
      cerr << "tbbTest: initPipeline() failed! Aborting! " << endl;
      return 1;
    };

    //Check what should be done
    if (confRec.asBool("doGenDynSpec")) {
      cerr << "tbbTest: Generation of dynamic spectra not implemented yet!" << endl;
    };

    if (confRec.asBool("doGenInputStatistics")) {
      cerr << "tbbTest: Generation of input statistics not implemented yet!" << endl;
    };

    if (confRec.asBool("doSimTBBTrigger")) {
      cerr << "tbbTest: Generating simulated TBB trigger output!" << endl;
      SimTBBTrigger(confRec, files);
    };


  } catch (std::string message) {
    std::cerr << message << std::endl;
  }
  
  return 0;
}
