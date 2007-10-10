/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id: CalTableClient.cc,v 1.4 2006/11/08 17:58:44 bahren Exp $*/

#include <GlishClients/CalTableClient.h>

using CR::CalTableWriter;

CalTableWriter table;

Bool isWritable=False;

// -------------------------------------------------------------------- openTable

Bool openTable (GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  GlishArray gtmp;
  try {
    gtmp = event.val();
    String tablename;
    gtmp.get(tablename,0);
    if (!table.AttachTable(tablename)) {
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(True));
    };   
  } catch (AipsError x) {
    cerr << "CalTableClient:openTable " << x.getMesg() << endl;
    return False;
  };   
  return True;
};

// ---------------------------------------------------------------------- getData

Bool getData (GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  GlishArray gtmp;
  try {
    if (event.val().type() != GlishValue::RECORD) {
      cerr << "CalTableClient:getData  Need record with: date, ant, and field"
	   << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    GlishRecord  inrec = event.val();
    if (! (inrec.exists("date")&&inrec.exists("ant")&&inrec.exists("field")) ) {
      cerr << "CalTableClient:getData  Need record with: date, ant, and field"
	   << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    uInt date;
    Int AntID;
    Int tmpint;
    String Field;
    String DataType;
    gtmp = inrec.get("date");
    gtmp.get(tmpint,0);
    date = (uInt)tmpint; //Stupid glish, does not know unsigend integers
    gtmp = inrec.get("ant");
    gtmp.get(AntID,0);
    gtmp = inrec.get("field");
    gtmp.get(Field,0);
    DataType = table.GetFieldType(Field);
    if (DataType == "String") {
      String result;
      if (table.GetData(date,AntID,Field,&result)){
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(result));
	};
      } else {
	ErrorGetData (Field,AntID,date);
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(False));
	};
      };
    } else if (DataType == "Double") {
      Double result;
      if (table.GetData(date,AntID,Field,&result)){
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(result));
	};
      } else {
	ErrorGetData (Field,AntID,date);
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(False));
	};
      };
    } else if (DataType == "DComplex") {
      DComplex result;
      if (table.GetData(date,AntID,Field,&result)){
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(result));
	};
      } else {
	ErrorGetData (Field,AntID,date);
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(False));
	};
      };
    } else if (DataType == "Array<Double>") {
      Array<Double> result;
      if (table.GetData(date,AntID,Field,&result)){
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(result));
	};
      } else {
	ErrorGetData (Field,AntID,date);
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(False));
	};
      };
    } else if (DataType == "Array<DComplex>") {
      Array<DComplex> result;
      if (table.GetData(date,AntID,Field,&result)){
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(result));
	};
      } else {
	ErrorGetData (Field,AntID,date);
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(False));
	};
      };
    } else {
      cerr << "CalTableClient:GetData  Unknown Field '" << Field << "'!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
    };
  } catch (AipsError x) {
    cerr << "CalTableClient:GetData " << x.getMesg() << endl;
    return False;
  };   
  return True;
};


Bool addData (GlishSysEvent &event, void *) {
  GlishSysEventSource *glishBus = event.glishSource();
  GlishArray gtmp;
  try {
    if (!isWritable) {
      cerr << "CalTableClient:addData  This table is not (yet) set writable!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    if (event.val().type() != GlishValue::RECORD) {
      cerr << "CalTableClient:addData  Need record with: startdate, ant, field, and data" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    GlishRecord  inrec = event.val();
    if (! (inrec.exists("startdate")&&inrec.exists("ant")&&inrec.exists("field")&&
	   inrec.exists("data")) ) {
      cerr << "CalTableClient:addData  Need record with: startdate, ant, field, and data" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    uInt startdate,stopdate=0;
    Int AntID,tmpint;
    String Field,DataType;
    gtmp = inrec.get("ant");
    gtmp.get(AntID,0);
    gtmp = inrec.get("field");
    gtmp.get(Field,0);
    gtmp = inrec.get("startdate");
    gtmp.get(tmpint,0);
    startdate = (uInt)tmpint; //Stupid glish, doesn't know unsigend integers
    if ( inrec.exists("stopdate") ) {
      gtmp = inrec.get("stopdate");
      gtmp.get(tmpint,0);
      stopdate = (uInt)tmpint; 
    };
    DataType = table.GetFieldType(Field);
    if (DataType == "String") {
      String data;
      gtmp = inrec.get("data");
      gtmp.get(data,0);
      if (table.AddData(data,AntID,Field,startdate,stopdate)){
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(True));
	};
      } else {
	ErrorAddData (Field, AntID, startdate, stopdate);
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(False));
	};
      };
    } else if (DataType == "Double") {
      Double data;
      gtmp = inrec.get("data");
      gtmp.get(data,0);
      if (table.AddData(data,AntID,Field,startdate,stopdate)){
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(True));
	};
      } else {
	ErrorAddData (Field, AntID, startdate, stopdate);
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(False));
	};
      };
    } else if (DataType == "DComplex") {
      DComplex data;
      gtmp = inrec.get("data");
      gtmp.get(data,0);
      if (table.AddData(data,AntID,Field,startdate,stopdate)){
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(True));
	};
      } else {
	ErrorAddData (Field, AntID, startdate, stopdate);
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(False));
	};
      };
    } else if (DataType == "Array<Double>") {
      Array<Double> data;
      gtmp = inrec.get("data");
      gtmp.get(data);
      if (table.AddData(data,AntID,Field,startdate,stopdate)){
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(True));
	};
      } else {
	ErrorAddData (Field, AntID, startdate, stopdate);
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(False));
	};
      };
    } else if (DataType == "Array<DComplex>") {
      Array<DComplex> data;
      gtmp = inrec.get("data");
      gtmp.get(data);
      if (table.AddData(data,AntID,Field,startdate,stopdate)){
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(True));
	};
      } else {
	ErrorAddData (Field, AntID, startdate, stopdate);
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(False));
	};
      };
    } else {
      cerr << "CalTableClient:addData  Unknown Field '" << Field << "'!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
    };
  } catch (AipsError x) {
    cerr << "CalTableClient:addData " << x.getMesg() << endl;
    return False;
  };   
  return True;
};


//------------------------------------------------------------------------------------

Bool GetKeyword(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  GlishArray gtmp;
  String KeywordName;
  try {
    if (event.val().type() != GlishValue::ARRAY) {
      cerr << "CalTableClient:GetKeyword  Need the Keyword-Name" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    gtmp = event.val();
    if (! (gtmp.elementType() == GlishArray::STRING) ) {
      cerr << "CalTableClient:GetKeyword  Need the Keyword-Name" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    gtmp.get(KeywordName,0);
    String type = table.GetKeywordType(KeywordName);
    if (type == "String") {
      String result;
      if (table.GetKeyword(KeywordName,&result)){
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(result));
	};
      } else {
	cerr << "CalTableClient:GetKeyword Could not get '" << KeywordName << "'S!" << endl;
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(False));
	};
      };
    } else if ((type == "Double")||(type == "Float")||(type == "Int")||(type == "uInt")) {
      Double result;
      if (table.GetKeyword(KeywordName,&result)){
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(result));
	};
      } else {
	cerr << "CalTableClient:GetKeyword Could not get '" << KeywordName << "'D!" << endl;
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(False));
	};
      };
    } else if ((type == "DComplex")||(type == "Complex")) {
      DComplex result;
      if (table.GetKeyword(KeywordName,&result)){
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(result));
	};
      } else {
	cerr << "CalTableClient:GetKeyword Could not get '" << KeywordName << "'DC!" << endl;
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(False));
	};
      };
    } else if (type == "Array<Double>") {
      Array<Double> result;
      if (table.GetKeyword(KeywordName,&result)){
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(result));
	};
      } else {
	cerr << "CalTableClient:GetKeyword Could not get '" << KeywordName << "'AD!" << endl;
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(False));
	};
      };
    } else if (type == "Array<DComplex>") {
      Array<DComplex> result;
      if (table.GetKeyword(KeywordName,&result)){
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(result));
	};
      } else {
	cerr << "CalTableClient:GetKeyword Could not get '" << KeywordName << "'ADC!" << endl;
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(False));
	};
      };
    } else {
      cerr << "CalTableClient:GetKeyword  Unknown keyword-type '" << type << "' for keyword '" << KeywordName << "'!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
    };
  } catch (AipsError x) {
    cerr << "CalTableClient:GetKeyword " << x.getMesg() << endl;
    return False;
  };   
  return True;
};

Bool SetKeyword(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  GlishArray gtmp;
  try {
    if (!isWritable) {
      cerr << "CalTableClient:SetKeyword  This table is not (yet) set writable!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    if (event.val().type() != GlishValue::RECORD) {
      cerr << "CalTableClient:SetKeyword  Need record with: KeywordName and data" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    GlishRecord  inrec = event.val();
    if (! (inrec.exists("KeywordName")&&inrec.exists("data")) ) {
      cerr << "CalTableClient:SetKeyword  Need record with: KeywordName and data" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    String KeywordName;
    gtmp = inrec.get("KeywordName");
    gtmp.get(KeywordName,0);
    gtmp = inrec.get("data");
    if (gtmp.elementType() == GlishArray::STRING) {
      String data;
      gtmp.get(data,0);
      if (table.SetKeyword(data,KeywordName)){
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(True));
	};
      } else {
	cerr << "CalTableClient:SetKeyword Could not write Keyword '" << KeywordName << "'!" << endl;
	if (glishBus->replyPending()) {
	  glishBus->reply(GlishArray(False));
	};
      };
    } else if ((gtmp.elementType() == GlishArray::COMPLEX) || 
	       (gtmp.elementType() == GlishArray::DCOMPLEX)) {
      if (gtmp.nelements() == 1 ){
	DComplex data;
	gtmp.get(data,0);
	if (table.SetKeyword(data,KeywordName)){
	  if (glishBus->replyPending()) {
	    glishBus->reply(GlishArray(True));
	  };
	} else {
	  cerr << "CalTableClient:SetKeyword Could not write Keyword '" << KeywordName << "'!" << endl;
	  if (glishBus->replyPending()) {
	    glishBus->reply(GlishArray(False));
	  };
	};
      } else {
	Array<DComplex> data;
	gtmp.get(data);
	if (table.SetKeyword(data,KeywordName)){
	  if (glishBus->replyPending()) {
	    glishBus->reply(GlishArray(True));
	  };
	} else {
	  cerr << "CalTableClient:SetKeyword Could not write Keyword '" << KeywordName << "'!" << endl;
	  if (glishBus->replyPending()) {
	    glishBus->reply(GlishArray(False));
	  };
	};
      };
    } else {
      if (gtmp.nelements() == 1 ){
	Double data;
	gtmp.get(data,0);
	if (table.SetKeyword(data,KeywordName)){
	  if (glishBus->replyPending()) {
	    glishBus->reply(GlishArray(True));
	  };
	} else {
	  cerr << "CalTableClient:SetKeyword Could not write Keyword '" << KeywordName << "'!" << endl;
	  if (glishBus->replyPending()) {
	    glishBus->reply(GlishArray(False));
	  };
	};
      } else {
	Array<Double> data;
	gtmp.get(data);
	if (table.SetKeyword(data,KeywordName)){
	  if (glishBus->replyPending()) {
	    glishBus->reply(GlishArray(True));
	  };
	} else {
	  cerr << "CalTableClient:SetKeyword Could not write Keyword '" << KeywordName << "'!" << endl;
	  if (glishBus->replyPending()) {
	    glishBus->reply(GlishArray(False));
	  };
	};
      };
    };
   } catch (AipsError x) {
    cerr << "CalTableClient:SetKeyword " << x.getMesg() << endl;
    return False;
  };   
  return True;
};


//------------------------------------------------------------------------------------

Bool addField(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  GlishArray gtmp;

  try {
    if (!isWritable) {
      cerr << "CalTableClient:addField  This table is not (yet) set writable!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    if (event.val().type() != GlishValue::RECORD) {
      cerr << "CalTableClient:addField  Need record with: FieldName, FieldDesc, and DataType!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    GlishRecord  inrec = event.val();
    if (! (inrec.exists("FieldName")&&inrec.exists("FieldDesc")&&inrec.exists("DataType")) ) {
      cerr << "CalTableClient:addField  Need record with: FieldName, FieldDesc, and DataType!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    String FieldName,FieldDesc,DataType,FriendField="";
    Bool shapeFixed=False,SingleTables=False;
    IPosition shape(0);
    gtmp = inrec.get("FieldName");
    gtmp.get(FieldName,0);
    gtmp = inrec.get("FieldDesc");
    gtmp.get(FieldDesc,0);
    gtmp = inrec.get("DataType");
    gtmp.get(DataType,0); 
    if ( inrec.exists("FriendField") ) {
      gtmp = inrec.get("FriendField");
      gtmp.get(FriendField,0); 
    };
    if ( inrec.exists("shapeFixed") ) {
      gtmp = inrec.get("shapeFixed");
      gtmp.get(shapeFixed,0); 
    };
    if ( inrec.exists("SingleTables") ) {
      gtmp = inrec.get("SingleTables");
      gtmp.get(SingleTables,0); 
    };
    if ( inrec.exists("exampleData") ) {
      gtmp = inrec.get("exampleData");
      shape=gtmp.shape(); 
    };
    if (table.AddField(FieldName, FieldDesc, DataType, shapeFixed, shape, SingleTables, FriendField)) {
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(True));
      };
    } else {
      cerr << "CalTableClient:addField  Could not add field!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
    };
  } catch (AipsError x) {
    cerr << "CalTableClient:openTable " << x.getMesg() << endl;
    return False;
  };   
  return True;
};


Bool addAntenna(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  GlishArray gtmp;
  try {
    if (!isWritable) {
      cerr << "CalTableClient:addAntenna  This table is not (yet) set to writable!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    if (event.val().type() != GlishValue::RECORD) {
      cerr << "CalTableClient:addAntenna  Need record with: AntID and AntName!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    GlishRecord  inrec = event.val();
    if (! (inrec.exists("AntID")&&inrec.exists("AntName")) ) {
      cerr << "CalTableClient:addAntenna  Need record with: AntID and AntName!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    String AntName;
    Int AntID;
    gtmp = inrec.get("AntName");
    gtmp.get(AntName,0);
    gtmp = inrec.get("AntID");
    gtmp.get(AntID,0);
    if (table.AddAntenna(AntID, AntName)) {
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(True));
      };
    } else {
      cerr << "CalTableClient:adAntenna  Could not add antenna!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
    };
  } catch (AipsError x) {
    cerr << "CalTableClient:openTable " << x.getMesg() << endl;
    return False;
  };   
  return True;
};


Bool PrintSummary(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    table.PrintSummary();
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(True));
    };   
  } catch (AipsError x) {
    cerr << "CalTableClient:PrintSummary " << x.getMesg() << endl;
    return False;
  };   
  return True;
};

Bool openWrite(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    isWritable = True;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(True));
    };   
  } catch (AipsError x) {
    cerr << "CalTableClient:openWrite " << x.getMesg() << endl;
    return False;
  };   
  return True;
};

Bool CreateEmptyTable(GlishSysEvent &event, void *){
  GlishSysEventSource *glishBus = event.glishSource();
  try {
    if (event.val().type() != GlishValue::RECORD) {
      cerr << "CalTableClient:CreateEmptyTable  Need record with: TableName and Magic!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    GlishRecord  inrec = event.val();
    GlishArray gtmp;
    if (! (inrec.exists("TableName")&&inrec.exists("Magic")) ) {
      cerr << "CalTableClient:CreateEmptyTable  Need record with: TableName and Magic!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    String TableName,Magic;
    gtmp = inrec.get("TableName");
    gtmp.get(TableName,0);
    gtmp = inrec.get("Magic");
    gtmp.get(Magic,0);
    if (Magic != "HokusPokus") {
      cerr << "CalTableClient:CreateEmptyTable  Wrong Magic!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    CalTableCreator maker;   
    if (!maker.newTable(TableName)) {
      cerr << "CalTableClient:CreateEmptyTable  Could no create table " << 
	TableName <<"!" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      };
      return True;
    };
    cout << "CalTableClient:CreateEmptyTable  Created new, empty table " << 
      TableName << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(True));
    };   
  } catch (AipsError x) {
    cerr << "CalTableClient:CreateEmptyTable " << x.getMesg() << endl;
    return False;
  };   
  return True;
};


int main(int argc, char *argv[])
{
  GlishSysEventSource glishStream(argc, argv);

  glishStream.addTarget(openTable,"openTable");
  glishStream.addTarget(getData,"getData");
  glishStream.addTarget(addData,"addData");
  glishStream.addTarget(GetKeyword,"GetKeyword");
  glishStream.addTarget(SetKeyword,"SetKeyword");
  glishStream.addTarget(addField,"addField");
  glishStream.addTarget(addAntenna,"addAntenna");
  glishStream.addTarget(PrintSummary,"PrintSummary");
  glishStream.addTarget(openWrite,"openWrite");
  glishStream.addTarget(CreateEmptyTable,"CreateEmptyTable");
  
  glishStream.loop();
  return 0;
}
