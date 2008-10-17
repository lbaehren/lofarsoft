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

/* $Id: CGuiPipeClient.cc,v 1.5 2007/07/26 14:37:22 horneff Exp $*/

#include <GlishClients/CGuiPipeClient.h>

using CR::analyseLOPESevent;
using CR::LopesEventIn;

analyseLOPESevent pipeline;

//---------------------------------------------------------------------   initPipeline

Bool initPipeline(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    if (event.val().type() != GlishValue::RECORD) {
      cerr << "CGuiPipeClient:initPipeline: " << "Need the observatories record!" 
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
      cout << "CGuiPipeClient:initPipeline: No entry \"LOPES\" in the record.\n" 
	   << "                             Are your sure this is correct?\n" ;
    }
    if (!pipeline.initPipeline(obsrec)){
      cerr << "CGuiPipeClient:initPipeline: " << "Error while initializing the pipeline." << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
    } else {
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(True));
      };
    }
  } catch (AipsError x) {
    cerr << "CGuiPipeClient:initPipeline: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};

//---------------------------------------------------------------------   ProcessEvent

Bool ProcessEvent(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    String evname, PlotPrefix, Polarization="EW";
    Double Az, El, distance=4000, XC, YC, ExtraDelay=0., UpSamplingRate=0.;
    int doTVcal=-1; 
    Bool RotatePos=True, generatePlots=False, verbose=False, fitPos=False;
    Vector<Int> FlaggedAntIDs; 
    if (event.val().type() != GlishValue::RECORD) {
      cerr << "CGuiPipeClient:initPipeline: Need record with: evname, Az, El, XC, YC!" 
	   << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    GlishRecord  inrec = event.val();
    Record input, output;
    inrec.toRecord(input);
    if (!(input.isDefined("evname") && input.isDefined("Az") && input.isDefined("El") 
	  && input.isDefined("XC") && input.isDefined("YC") )) {
      cerr << "CGuiPipeClient:initPipeline: Need record with: evname, Az, El, XC, YC!" 
	   << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    evname = input.asString("evname");
    Az = input.asDouble("Az");
    El = input.asDouble("El");
    XC = input.asDouble("XC");
    YC = input.asDouble("YC");
    if (input.isDefined("distance")){
      distance = input.asDouble("distance");
    };
    if (input.isDefined("ExtraDelay")){
      ExtraDelay = input.asDouble("ExtraDelay");
    };
    if (input.isDefined("RotatePos")){
      RotatePos = input.asBool("RotatePos");
    };
    if (input.isDefined("verbose")){
      verbose = input.asBool("verbose");
    };
    if (input.isDefined("FitPosition")){
      fitPos = input.asBool("FitPosition");
    };
    if (input.isDefined("generatePlots")){
      generatePlots = input.asBool("generatePlots");
      if (generatePlots) {
	PlotPrefix = "CGuiPipeline";
	if (input.isDefined("PlotPrefix")){
	  PlotPrefix = input.asString("PlotPrefix");
	};
      };
    };
    if (input.isDefined("FlaggedAntIDs")){
      FlaggedAntIDs = input.asArrayInt("FlaggedAntIDs");
    };
    if (input.isDefined("UpSamplingRate")){
      UpSamplingRate = input.asDouble("UpSamplingRate");
    };
    
    output = pipeline.ProcessEvent(evname, Az, El, distance, XC, YC, RotatePos, 
				   PlotPrefix, generatePlots, FlaggedAntIDs, verbose, 
				   fitPos, ExtraDelay, doTVcal, UpSamplingRate, Polarization);
    if (glishBus->replyPending()) {
      GlishRecord outrec;
      outrec.fromRecord(output);
      glishBus->reply(outrec);
    };
    
  } catch (AipsError x) {
    cerr << "CGuiPipeClient:ProcessEvent: " << x.getMesg() << endl;
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
  glishStream.addTarget(ProcessEvent,"ProcessEvent");
  
  glishStream.loop();
  return 0;
}
