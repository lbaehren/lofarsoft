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
#include <ApplicationSupport/Glish.h>

// Glish includes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>

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

analyseLOPESevent pipeline;

// global Variable for tbbctl data that actually contains all the data
CR::tbbctlIn tbbIn;

// local copies (of the pointers) to access the stuff.
CR::CRinvFFT *pipeline_p;    
DataReader *DataReader_p;

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
	tbbIn.setBlock(i+1);
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
    Int maxsize = (10000000); // 10 000 000 
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
    Vector<Double> data,means(numFiles,0.),stddevs(numFiles,0.),mins(numFiles,0.),maxs(numFiles,0.);
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
      if (((fnum+1)%50)==0) {
	cout << "SimpleDynspecClient:GenInputStatistics: processed " << fnum+1 << " files out of " 
	     << numFiles << "!" << endl;
      };
    };
    output.define("mins",mins);
    output.define("maxs",maxs);
    output.define("means",means);
    output.define("stddevs",stddevs);
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
    Int maxsize = (10000000); // 10 000 000 
    Vector<String> files;
    if (event.val().type() != GlishValue::RECORD) {
      cerr << "SimpleDynspecClient:SimTBBTrigger: Need record with: files (maxsize)!" 
	   << endl;
      glishBus->reply(GlishArray(False));
      return True;
    };
    GlishRecord  inrec = event.val(),outrec;
    Record input, output;
    inrec.toRecord(input);
    if (!(input.isDefined("files")  )) {
      cerr << "SimpleDynspecClient:SimTBBTrigger: Need record with: files (maxsize)!" 
	   << endl;
      glishBus->reply(GlishArray(False));
      return True;
    };
    files = input.asArrayString("files");
    if (input.isDefined("maxsize")){
      maxsize = input.asInt("maxsize");
    };
    Int fnum,numFiles=files.nelements();
    Vector<Double> data,means(numFiles,0.),stddevs(numFiles,0.),mins(numFiles,0.),maxs(numFiles,0.);
    Double dmin,dmax;
    for (fnum=0; fnum<numFiles; fnum++){
      // initialize the Data Reader
      if (! tbbIn.attachFile(files(Slice(fnum,1))) ){
	cerr << "SimpleDynspecClient:SimTBBTrigger: " << "Failed to attach file: " << files(fnum) << endl;
	glishBus->reply(GlishArray(False));
	return True;
      };
      if (tbbIn.blocksize() > (uint)maxsize){
	cerr << "SimpleDynspecClient:SimTBBTrigger: " << "File: " << files(fnum) 
	     << " spans too large a range! (Hole inside?)" << endl;
	continue;
      };
      if (! pipeline_p->InitEvent(&tbbIn)){
	cerr << "SimpleDynspecClient:SimTBBTrigger: "  << "File: " << files(fnum)
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
      if (((fnum+1)%50)==0) {
	cout << "SimpleDynspecClient:SimTBBTrigger: processed " << fnum+1 << " files out of " 
	     << numFiles << "!" << endl;
      };
    };
    output.define("mins",mins);
    output.define("maxs",maxs);
    output.define("means",means);
    output.define("stddevs",stddevs);
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
