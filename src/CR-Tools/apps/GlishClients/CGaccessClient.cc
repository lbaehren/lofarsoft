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

/* $Id: CGaccessClient.cc,v 1.1 2007/07/26 14:37:22 horneff Exp $*/

#include <GlishClients/CGaccessClient.h>

using CR::DataReader;
using CR::LopesEventIn;

// global Variable for LOPES events that actually contains all the data
CR::analyseLOPESevent pipeline;

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
      cerr << "CGaccessClient:initPipeline: " << "Need the observatories record!" 
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
      cout << "CGaccessClient:initPipeline: No entry \"LOPES\" in the record.\n" 
	   << "                             Are your sure this is correct?\n" ;
    }
    if (!pipeline.initPipeline(obsrec)){
      cerr << "CGaccessClient:initPipeline: " << "Error while initializing the pipeline." << endl;
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
    cerr << "CGaccessClient:initPipeline: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};

//----------------------------------------------------------------   LoadLOPESEvent
Bool LoadLOPESEvent(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    String evname;
    if (event.val().type() != GlishValue::ARRAY) {
      cerr << "CGaccessClient:LoadLOPESEvent: " << "Need the path to the file!" 
	   << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    GlishArray gtmp = event.val();
    gtmp.get(evname);
    LopesEventIn *lev_p=pipeline.GetDataReader();
    // initialize the Data Reader
    if (! lev_p->attachFile(evname) ){
      cerr << "CGaccessClient:LoadLOPESEvent: " << "Failed to attach file: " << evname << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    if (! pipeline_p->InitEvent(lev_p)){
      cerr << "CGaccessClient:LoadLOPESEvent: " << "Failed to initialize the DataReader!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    DataReader_p = lev_p;
    Record outrec;
    outrec.merge(lev_p->header(),RecordInterface::OverwriteDuplicates);
    pipeline_p->setPhaseCenter(0.,0.);
    pipeline_p->GetTimeSeries(lev_p);
    outrec.define("AntPos",pipeline_p->GetAntPositions());

    if (glishBus->replyPending()) {
      GlishRecord grec;
      grec.fromRecord(outrec);
      glishBus->reply(grec);
    };
  } catch (AipsError x) {
    cerr << "CGaccessClient:LoadLOPESEvent: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};


//----------------------------------------------------------------   LoadTBBCTLEvent
Bool LoadTBBCTLEvent(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    Vector<String> evnames;
    if (event.val().type() != GlishValue::ARRAY) {
      cerr << "CGaccessClient:LoadTBBCTLEvent: " << "Need the path to the files!" 
	   << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    GlishArray gtmp = event.val();
    gtmp.get(evnames);

    // initialize the Data Reader
    if (! tbbIn.attachFile(evnames) ){
      cerr << "CGaccessClient:LoadTBBCTLEvent: " << "Failed to attach files: " << evnames << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    if (! pipeline_p->InitEvent(&tbbIn)){
      cerr << "CGaccessClient:LoadTBBCTLEvent: " << "Failed to initialize the DataReader!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    DataReader_p = &tbbIn;
    Record outrec;
    outrec.merge(tbbIn.header(),RecordInterface::OverwriteDuplicates);
    pipeline_p->setPhaseCenter(0.,0.);
    pipeline_p->GetTimeSeries(DataReader_p);
    outrec.define("AntPos",pipeline_p->GetAntPositions());

    if (glishBus->replyPending()) {
      GlishRecord grec;
      grec.fromRecord(outrec);
      glishBus->reply(grec);
    };
  } catch (AipsError x) {
    cerr << "CGaccessClient:LoadTBBCTLEvent: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};





//---------------------------------------------------------------------  GetTime 
Bool GetTime(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    Vector<Double> Time;
    Time = DataReader_p->timeValues();
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(Time));
    };    
  } catch (AipsError x) {
    cerr << "CGaccessClient:GetTime: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};
//---------------------------------------------------------------------   GetFrequency
Bool GetFrequency(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    Vector<Double> Frequency;
    Frequency = DataReader_p->frequencyValues();
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(Frequency));
    };    
  } catch (AipsError x) {
    cerr << "CGaccessClient:GetFrequency: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};

//---------------------------------------------------------------------   GetFX
Bool GetFX(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    Matrix<Double> FX;
    FX = DataReader_p->fx();
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(FX));
    };    
  } catch (AipsError x) {
    cerr << "CGaccessClient:GetFX: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};
//---------------------------------------------------------------------   GetVoltage
Bool GetVoltage(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    Matrix<Double> Voltage;
    Voltage = DataReader_p->voltage();
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(Voltage));
    };    
  } catch (AipsError x) {
    cerr << "CGaccessClient:GetVoltage: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};
//--------------------------------------------------------------------- GetFFT  
Bool GetFFT(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    Matrix<DComplex> FFT;
    FFT = DataReader_p->fft();
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(FFT));
    };    
  } catch (AipsError x) {
    cerr << "CGaccessClient:GetFFT: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};
//---------------------------------------------------------------------   GetCalFFT
Bool GetCalFFT(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    Matrix<DComplex> CalFFT;
    CalFFT = DataReader_p->calfft();
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(CalFFT));
    };    
  } catch (AipsError x) {
    cerr << "CGaccessClient:GetCalFFT: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};

//---------------------------------------------------------------------  GetFilteredFFT 
Bool GetFilteredFFT(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    Matrix<DComplex> FilteredFFT;
    FilteredFFT = pipeline_p->GetData(DataReader_p);
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(FilteredFFT));
    };    
  } catch (AipsError x) {
    cerr << "CGaccessClient:GetFilteredFFT: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};

//---------------------------------------------------------------------   GetTCXP
Bool GetTCXP(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    Double Az, El, distance=1e37, XC, YC, ExtraDelay=0.; 
    Bool RotatePos=False;
    Vector<Bool> FlaggedAnts; 
    if (event.val().type() != GlishValue::RECORD) {
      cerr << "CGaccessClient:GetTCXP: Need record with: Az, El, XC, YC!" 
	   << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    GlishRecord  inrec = event.val();
    Record input, output;
    inrec.toRecord(input);
    if (!(input.isDefined("Az") && input.isDefined("El") 
	  && input.isDefined("XC") && input.isDefined("YC") )) {
      cerr << "CGaccessClient:GetTCXP: Need record with: Az, El, XC, YC!" 
	   << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
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
    if (input.isDefined("FlaggedAnts")){
      FlaggedAnts = input.asArrayBool("FlaggedAnts");
    };    
    Matrix<Double> TimeSeries;
    Vector<Double> ccBeamData, xBeamData, pBeamData;
    pipeline_p->setPhaseCenter(XC, YC, RotatePos);
    pipeline_p->setDirection(Az, El, distance);
    pipeline_p->setExtraDelay(ExtraDelay);
    pipeline_p->GetTCXP(DataReader_p, TimeSeries, ccBeamData, xBeamData, 
			pBeamData, FlaggedAnts);
    if (glishBus->replyPending()) {
      output.define("FieldStrength",pipeline_p->GetTimeSeries(DataReader_p));
      output.define("CCBeam",ccBeamData);
      output.define("XBeam",xBeamData);
      output.define("PBeam",pBeamData);
      GlishRecord outrec;
      outrec.fromRecord(output);
      glishBus->reply(outrec);
    };
  } catch (AipsError x) {
    cerr << "CGaccessClient:GetTCXP: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};




//---------------------------------------------------------------------   GetFieldStrength
Bool GetFieldStrength(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    Matrix<Double> FieldStrength;
    FieldStrength = pipeline_p->GetTimeSeries(DataReader_p);
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(FieldStrength));
    };    
  } catch (AipsError x) {
    cerr << "CGaccessClient:GetFieldStrength: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};

//---------------------------------------------------------------------   GetCCBeam
Bool GetCCBeam(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    Vector<Double> CCBeam;
    CCBeam = pipeline_p->GetCCBeam(DataReader_p);
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(CCBeam));
    };    
  } catch (AipsError x) {
    cerr << "CGaccessClient:GetCCBeam: " << x.getMesg() << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return False;
  };   
  return True;
};
//---------------------------------------------------------------------   GetXBeam
Bool GetXBeam(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    Vector<Double> XBeam;
    XBeam = pipeline_p->GetXBeam(DataReader_p);
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(XBeam));
    };    
  } catch (AipsError x) {
    cerr << "CGaccessClient:GetXBeam: " << x.getMesg() << endl;
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

  pipeline_p = NULL;
  DataReader_p = NULL;

  glishStream.addTarget(initPipeline,"initPipeline");
  glishStream.addTarget(LoadLOPESEvent,"LoadLOPESEvent");
  glishStream.addTarget(LoadTBBCTLEvent,"LoadTBBCTLEvent");

  glishStream.addTarget(GetTime,"GetTime");
  glishStream.addTarget(GetFrequency,"GetFrequency");
  glishStream.addTarget(GetFX,"GetFX");
  glishStream.addTarget(GetVoltage,"GetVoltage");
  glishStream.addTarget(GetFFT,"GetFFT");
  glishStream.addTarget(GetCalFFT,"GetCalFFT");
  glishStream.addTarget(GetFilteredFFT,"GetFilteredFFT");
  glishStream.addTarget(GetTCXP,"GetTCXP");

  glishStream.addTarget(GetFieldStrength,"GetFieldStrength");
  glishStream.addTarget(GetCCBeam,"GetCCBeam");
  glishStream.addTarget(GetXBeam,"GetXBeam");
  
  glishStream.loop();
  return 0;
}
