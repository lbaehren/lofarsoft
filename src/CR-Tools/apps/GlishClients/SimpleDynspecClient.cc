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


// CGuipipeline includes
#include <Analysis/analyseLOPESevent.h>
#include <Data/tbbctlIn.h>
#include <Analysis/tbbTools.h>
#include <Calibration/RFIMitigationPlugin.h>
#include <ApplicationSupport/Glish.h>

// Glish includes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>
#include <scimath/Mathematics/FFTServer.h>

// general includes
#include <iostream>
#include <sstream>


/*!
  \file SimpleDynspecClient.cc
  
  \ingroup CR_Applications

  \brief Exceedingly simple dynamic-spectrum application
  
  \author Andreas Horneffer
  
  \date 2007/11/19
  
  <h3>Prerequisite</h3>
  
  analyseLOPESevent 
  tbbctlIn
  
  <h3>Synopsis</h3>
  
  <h3>Example(s)</h3>
  
*/

using CR::analyseLOPESevent;
using CR::LopesEventIn;
using CR::tbbTools;

analyseLOPESevent pipeline;

// global Variable for tbbctl data that actually contains all the data
CR::tbbctlIn tbbIn;

// local copies (of the pointers) to access the stuff.
CR::CRinvFFT *pipeline_p;    
DataReader *DataReader_p;


// Local RFI mitigation object
CR::RFIMitigationPlugin rfiM_p;

//---------------------------------------------------------------------   initPipeline

Bool initPipeline(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    if (event.val().type() != GlishValue::RECORD) {
      cerr << "SimpleDynspecClient:initPipeline: " << "Need the observatories record!" 
	   << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    GlishRecord  inrec = event.val();
    Record obsrec;
    inrec.toRecord(obsrec);
    if (!obsrec.isDefined("LOPES")) {
      cout << "SimpleDynspecClient:initPipeline: No entry \"LOPES\" in the record.\n" 
	   << "                             Are your sure this is correct?\n" ;
    }
    if (!pipeline.initPipeline(obsrec)){
      cerr << "SimpleDynspecClient:initPipeline: " << "Error while initializing the pipeline." << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
    } else {
      pipeline_p = pipeline.GetPipeline();
      DataReader_p = pipeline.GetDataReader();
      pipeline_p->setVerbosity(False);
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(True));
      };
    }
  } catch (AipsError x) {
    cerr << "SimpleDynspecClient:initPipeline: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};


//----------------------------------------------------------------   GenDynSpec
Bool GenDynSpec(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  if (!glishBus->replyPending()) {
    cerr << "SimpleDynspecClient:GenDynSpec: Not much use to call GenDynSpec without waiting for a reply!" 
	 << endl;
    return True;
  };
  try {
    Int blocksize=2048,maxsize = (10000000); // 10 000 000 
    Vector<String> files;
    if (event.val().type() != GlishValue::RECORD) {
      cerr << "SimpleDynspecClient:GenDynSpec: Need record with: files (blocksize, maxsize)!" 
	   << endl;
      glishBus->reply(GlishArray(False));
      return True;
    };
    GlishRecord  inrec = event.val();
    Record input;
    inrec.toRecord(input);
    if (!(input.isDefined("files")  )) {
      cerr << "SimpleDynspecClient:GenDynSpec: Need record with: files (blocksize, maxsize)!" 
	   << endl;
      glishBus->reply(GlishArray(False));
      return True;
    };
    files = input.asArrayString("files");
    if (input.isDefined("blocksize")){
      blocksize = input.asInt("blocksize");
    };
    if (input.isDefined("maxsize")){
      maxsize = input.asInt("maxsize");
    };
    Int fnum,numFiles=files.nelements(),fftsize=(blocksize/2+1);
    Int i,numblocks;
    Matrix<Double> data(fftsize,numFiles,0.);
    for (fnum=0; fnum<numFiles; fnum++){
      // initialize the Data Reader
      if (! tbbIn.attachFile(files(Slice(fnum,1))) ){
	cerr << "SimpleDynspecClient:GenDynSpec: " << "Failed to attach file: " << files(fnum) << endl;
	glishBus->reply(GlishArray(False));
	return True;
      };
      if (tbbIn.blocksize() > (uint)maxsize){
	cerr << "SimpleDynspecClient:GenDynSpec: " << "File: " << files(fnum) 
	     << " spans too large a range! (Hole inside?)" << endl;
	continue;
      };
      numblocks = tbbIn.blocksize()/blocksize;
      tbbIn.setBlocksize(blocksize);
      if (! pipeline_p->InitEvent(&tbbIn)){
	cerr << "SimpleDynspecClient:GenDynSpec: "  << "File: " << files(fnum)
	     << " failed to initialize the DataReader!" << endl;
	continue;
      };
      Vector<Double> tmpvec(fftsize,0.);
      for (i=0; i<numblocks; i++){
	tbbIn.setBlock(i);
	tmpvec += amplitude(tbbIn.calfft().column(0));
      };
      data.column(fnum) = tmpvec/(Double)numblocks;    
      if (((fnum+1)%50)==0) {
	cout << "SimpleDynspecClient:GenDynSpec: processed " << fnum+1 << " files out of " 
	     << numFiles << "!" << endl;
      };
    };
    glishBus->reply(GlishArray(data));
  } catch (AipsError x) {
    cerr << "SimpleDynspecClient:GenDynSpec: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};

//-----------------------------------------------------------  GenInputStatistics
Bool GenInputStatistics(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  if (!glishBus->replyPending()) {
    cerr << "SimpleDynspecClient:GenInputStatistics: Not much use to call GenInputStatistics without waiting for a reply!" 
	 << endl;
    return True;
  };
  try {
    Int maxsize = (5000000); // 5 000 000 
    Vector<String> files;
    if (event.val().type() != GlishValue::RECORD) {
      cerr << "SimpleDynspecClient:GenInputStatistics: Need record with: files (maxsize)!" 
	   << endl;
      glishBus->reply(GlishArray(False));
      return True;
    };
    GlishRecord  inrec = event.val(),outrec;
    Record input, output;
    inrec.toRecord(input);
    if (!(input.isDefined("files")  )) {
      cerr << "SimpleDynspecClient:GenInputStatistics: Need record with: files (maxsize)!" 
	   << endl;
      glishBus->reply(GlishArray(False));
      return True;
    };
    files = input.asArrayString("files");
    if (input.isDefined("maxsize")){
      maxsize = input.asInt("maxsize");
    };
    Int fnum,numFiles=files.nelements();
    Vector<Double> data,means(numFiles,0.),stddevs(numFiles,0.);
    Vector<Double> mins(numFiles,0.),maxs(numFiles,0.),ddates(numFiles,0.);
    Double dmin,dmax;
    for (fnum=0; fnum<numFiles; fnum++){
      // initialize the Data Reader
      if (! tbbIn.attachFile(files(Slice(fnum,1))) ){
	cerr << "SimpleDynspecClient:GenInputStatistics: " << "Failed to attach file: " << files(fnum) << endl;
	glishBus->reply(GlishArray(False));
	return True;
      };
      if (tbbIn.blocksize() > (uint)maxsize){
	cerr << "SimpleDynspecClient:GenInputStatistics: " << "File: " << files(fnum) 
	     << " spans too large a range! (Hole inside?)" << endl;
	continue;
      };
      if (! pipeline_p->InitEvent(&tbbIn)){
	cerr << "SimpleDynspecClient:GenInputStatistics: "  << "File: " << files(fnum)
	     << " failed to initialize the DataReader!" << endl;
	continue;
      };
      //dmin = mean(tbbIn.fx().column(0));
      //data = abs(tbbIn.fx().column(0)-dmin);
      data = abs(tbbIn.fx().column(0));
      minMax(dmin, dmax, data);
      mins(fnum)    = dmin;
      maxs(fnum)    = dmax;
      means(fnum)   = mean(data);
      stddevs(fnum) = stddev(data);
      ddates(fnum) = tbbIn.header().asDouble("dDate");;
      if (((fnum+1)%50)==0) {
	cout << "SimpleDynspecClient:GenInputStatistics: processed " << fnum+1 << " files out of " 
	     << numFiles << "!" << endl;
      };
    };
    output.define("mins",mins);
    output.define("maxs",maxs);
    output.define("means",means);
    output.define("stddevs",stddevs);
    output.define("ddate",ddates);
    outrec.fromRecord(output);
    glishBus->reply(outrec);
  } catch (AipsError x) {
    cerr << "SimpleDynspecClient:GenInputStatistics: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};

//-----------------------------------------------------------  SimTBBTrigger
Bool SimTBBTrigger(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  if (!glishBus->replyPending()) {
    cerr << "SimpleDynspecClient:SimTBBTrigger: Not much use to call SimTBBTrigger without waiting for a reply!" 
	 << endl;
    return True;
  };
  try {
    tbbTools tbbtool;
    Int i,maxsize = (5000000); // 5 000 000 
    Vector<String> files;
    Int level=8, start=5, stop=2, window=4096, afterwindow=0;
    Bool doRFImitigation=False, doFiltering=False,returnFiltered=False,addData=False;
    Vector<Double> Fc,BW,data2add;
    Vector<Int> FilterTypes;
    Double resolution=1024.;
    if (event.val().type() != GlishValue::RECORD) {
      cerr << "SimpleDynspecClient:SimTBBTrigger: Need record with: files (level, start, stop, window, maxsize, RFImitigation, afterwindow)!" 
	   << endl;
      glishBus->reply(GlishArray(False));
      return True;
    };
    GlishRecord  inrec = event.val(),outrec;
    Record input, output;
    inrec.toRecord(input);
    if (!(input.isDefined("files")  )) {
      cerr << "SimpleDynspecClient:SimTBBTrigger: Need record with: files (level, start, stop, window, maxsize, RFImitigation, afterwindow)!" 
	   << endl;
      glishBus->reply(GlishArray(False));
      return True;
    };
    files = input.asArrayString("files");
    if (input.isDefined("level")){
      level = input.asInt("level");
    };
    if (input.isDefined("start")){
      start = input.asInt("start");
    };
    if (input.isDefined("stop")){
      stop = input.asInt("stop");
    };
    if (input.isDefined("window")){
      window = input.asInt("window");
    };
    if (input.isDefined("afterwindow")){
      afterwindow = input.asInt("afterwindow");
    };
    if (input.isDefined("maxsize")){
      maxsize = input.asInt("maxsize");
    };
    if (input.isDefined("RFImitigation")){
      doRFImitigation = input.asBool("RFImitigation");
    };
    if (input.isDefined("doFiltering")){
      doFiltering = input.asBool("doFiltering");
      if (input.isDefined("FC")){
	Fc = input.toArrayDouble("FC");
      };
      if (input.isDefined("BW")){
	BW = input.toArrayDouble("BW");
      };
      if (input.isDefined("resolution")){
	resolution = input.asDouble("resolution");
      };
      if (input.isDefined("FilterTypes")){
	FilterTypes = input.toArrayInt("FilterTypes");
      };
      if (input.isDefined("returnFiltered")){
	returnFiltered = input.asBool("returnFiltered");
      };
    };
    if (input.isDefined("data2add")){
      data2add = input.asArrayDouble("data2add");
      addData=True;
    };
    
    Int fnum,numFiles=files.nelements();
    Int numpulses=0, vecsize=100, newpeaks;
    Vector<Int> gesindex(100),gessum(100),geswidth(100),gespeak(100),gesmeanval(100),gesafterval(100);
    Vector<String> filenames(100);
    Vector<Double> dates(100);
    Vector<Int> index,sum,width,peak,meanval,afterval;
    Vector<Double> data;
    
    for (fnum=0; fnum<numFiles; fnum++){
      // initialize the Data Reader
      if (! tbbIn.attachFile(files(Slice(fnum,1))) ){
	cerr << "SimpleDynspecClient:SimTBBTrigger: " << "Failed to attach file: " << files(fnum) << endl;
	glishBus->reply(GlishArray(False));
	return True;
      };
      if (tbbIn.blocksize() > (uint)maxsize){
	cerr << "SimpleDynspecClient:SimTBBTrigger: " << "File: " << files(fnum) 
	     << " spans too large a range! (>"<< maxsize <<",Hole inside?)" << endl;
	continue;
      };
      if (! pipeline_p->InitEvent(&tbbIn)){
	cerr << "SimpleDynspecClient:SimTBBTrigger: "  << "File: " << files(fnum)
	     << " failed to initialize the DataReader!" << endl;
	continue;
      };
      if (doRFImitigation){
	//	tbbIn.setHanningFilter(0.);
	Matrix<DComplex> fdata;
	Vector<DComplex> tfdata;
	Int blocksize=tbbIn.blocksize(),fftlen=blocksize/2+1;
	FFTServer<Double,DComplex> server(IPosition(1,blocksize),
					  FFTEnums::REALTOCOMPLEX);
	data = tbbIn.fx().column(0);
	if (addData){
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
      } else if (doFiltering){
	int numFilters;
	data = tbbIn.fx().column(0);
	if (addData){
	  data(Slice(0,data2add.nelements())) = data(Slice(0,data2add.nelements()))+data2add;
	};
	numFilters = Fc.nelements();
	if ((numFilters<1)||(numFilters != (int)BW.nelements())) {
	  cerr << "SimpleDynspecClient:SimTBBTrigger: "  << "\"doFiltering\" with inconsistend parameters!" 
	       << endl;
	  if ((numFilters<1))
	    cerr << "                                   "  << "empty \"Fc\" paramter-vector." << endl;
	  if (numFilters != (int)BW.nelements())
	    cerr << "                                   "  << "different length of parameters" << endl;
	} else {
	  for (i=0; i<numFilters; i++){
	    if ((FilterTypes.nelements()<uInt(i+1))||(FilterTypes(i) == 1)){
	      data = tbbtool.FPGAfilterNotch(data, Fc(i), BW(i), 200., resolution); //Hardcode sample-rate of 200MHz (Fc and BW in MHZ!)
	    } else if (FilterTypes(i) == 2) {
	      data = tbbtool.FPGAfilterLPF(data, Fc(i), BW(i), 200., resolution);
	    } else if (FilterTypes(i) == 3){
	      data = tbbtool.FPGAfilterHPF(data, Fc(i), BW(i), 200., resolution);
	    } else {
	      cout << "SimpleDynspecClient:SimTBBTrigger: " << 
		"Unknown FilterTypes value:" << FilterTypes(i) << endl;
	      cout << "                                   " << 
		" 1: Notch ; 2: LPF ; 3: HPF; " << endl;
	    };
	  };
	  if (returnFiltered) {
	    output.define("FilteredData",data);
	  };
	};	
      } else {
	data = tbbIn.fx().column(0);
	if (addData){
	  data(Slice(0,data2add.nelements())) = data(Slice(0,data2add.nelements()))+data2add;
	};
      };
      if (! tbbtool.meanFPGAtrigger(data,
				    level, start, stop, window, afterwindow,
				    index, sum, width, peak, meanval, afterval)) {
	cerr << "SimpleDynspecClient:SimTBBTrigger: "  << "File: " << files(fnum)
	     << " failed during meanFPGAtrigger!" << endl;
	continue;
      };

      //      cout << "file: " << files(fnum) << " Date: " << tbbIn.header().asuInt("Date") <<" dDate: " << tbbIn.header().asDouble("dDate") << endl;
      newpeaks = index.nelements();
      if (numpulses+newpeaks > vecsize) {
	vecsize = numpulses+newpeaks+100;
	filenames.resize(vecsize,True);
	dates.resize(vecsize,True);
	gesindex.resize(vecsize,True);
	gessum.resize(vecsize,True);
	geswidth.resize(vecsize,True);
	gespeak.resize(vecsize,True);
	gesmeanval.resize(vecsize,True);
	gesafterval.resize(vecsize,True);
      };
      filenames(Slice(numpulses,newpeaks))   = files(fnum);
      dates(Slice(numpulses,newpeaks))       = tbbIn.header().asDouble("dDate");
      gesindex(Slice(numpulses,newpeaks))    = index;
      gessum(Slice(numpulses,newpeaks))      = sum;
      geswidth(Slice(numpulses,newpeaks))    = width;
      gespeak(Slice(numpulses,newpeaks))     = peak;
      gesmeanval(Slice(numpulses,newpeaks))  = meanval;
      gesafterval(Slice(numpulses,newpeaks)) = afterval;
      numpulses += newpeaks;
      if (((fnum+1)%50)==0) {
	cout << "SimpleDynspecClient:SimTBBTrigger: processed " << fnum+1 << " files out of " 
	     << numFiles << "!" << endl;
      };
    };
    filenames.resize(numpulses,True);
    dates.resize(numpulses,True);
    gesindex.resize(numpulses,True);
    gessum.resize(numpulses,True);
    geswidth.resize(numpulses,True);
    gespeak.resize(numpulses,True);
    gesmeanval.resize(numpulses,True);
    gesafterval.resize(numpulses,True);
    
    output.define("filename",filenames);
    output.define("date",dates);
    output.define("index",gesindex);
    output.define("sum",gessum);
    output.define("width",geswidth);
    output.define("peak",gespeak);
    output.define("meanval",gesmeanval);
    output.define("afterval",gesafterval);
    outrec.fromRecord(output);
    glishBus->reply(outrec);
  } catch (AipsError x) {
    cerr << "SimpleDynspecClient:SimTBBTrigger: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};


//------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  GlishSysEventSource glishStream(argc, argv);

  glishStream.addTarget(initPipeline,"initPipeline");
  glishStream.addTarget(GenDynSpec,"GenDynSpec");
  glishStream.addTarget(GenInputStatistics,"GenInputStatistics");
  glishStream.addTarget(SimTBBTrigger,"SimTBBTrigger");

  glishStream.loop();
  return 0;
}
