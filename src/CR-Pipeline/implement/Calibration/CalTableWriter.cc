/***************************************************************************
 *   Copyright (C) 2006                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

/* $Id: CalTableWriter.cc,v 1.3 2006/10/31 18:23:24 bahren Exp $*/

#include <Calibration/CalTableWriter.h>

using std::cerr;
using std::cout;
using std::endl;

using casa::AipsError;
using casa::ArrayColumn;
using casa::ArrayColumnDesc;
using casa::ColumnDesc;
using casa::ROScalarColumn;
using casa::ScalarColumn;
using casa::ScalarColumnDesc;
using casa::SetupNewTable;
using casa::Table;
using casa::TableDesc;
using casa::TableRow;
using casa::Vector;

namespace CR {  // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  CalTableWriter::CalTableWriter () 
    : CalTableReader() { 
    if ( masterTable_p != NULL && !masterTable_p->isNull()){
      masterTable_p->reopenRW();
    };
  }
  
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  CalTableWriter::~CalTableWriter ()
  {
    masterTable_p->flush(true);
    delete masterTable_p;
  }
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  bool CalTableWriter::AttachTable(const String& tableFilename){
    try {
      masterTable_p = new Table(tableFilename,Table::Update);
      AntIDIndex_p  = new ColumnsIndex(*masterTable_p ,String("AntID"));
      indexedAnt_p  = new RecordFieldPtr<int>(AntIDIndex_p->accessKey(),"AntID");
    } catch (AipsError x) {
      cerr << "CalTableWriter::AttachTable: " << x.getMesg() << endl;
      return false;
    }; 
    return true;
  }
  
  
  // ==============================================================================
  // Get the preceding and succeding row numbers and reset the stopDate
  // ==============================================================================
  
  bool CalTableWriter::GetPreSucRows(int const AntID,
				     Table const colTab,
				     uInt const startDate, 
				     uInt *stopDate,
				     int *preRow,
				     int *succRow)
  {  
    if ( (*stopDate != 0) && (*stopDate < startDate) ) {
      cerr << "CalTableWriter::GetPreSucRows: "
	   << "stopDate given and stopDate < startDate "
	   << endl;
      return false;
    };
    try {
      Vector<uInt> rowNrs;
      uInt tmp;
      if (colTab.nrow() > 0 ) { // table has entries, find the corresponding rows
	// find preceding row (the one that is currently responsible for the new startDate)
	*preRow = CalTableReader::GetDateRow(AntID, colTab, startDate, false);
	if (*preRow < -1) {
	  cerr << "CalTableWriter::GetPreSucRows: " << "Error in CalTableReader::GetDateRow()" << endl;
	  return false;
	};     
	// find succeding row
	rowNrs = colTab( (colTab.col("StartDate")>(double)startDate) && 
			 (colTab.col("AntID")==AntID) ).sort("StartDate").rowNumbers(colTab);
	if (rowNrs.nelements() == 0 ) {
	  *succRow = -1;
	} else {
	  *succRow = rowNrs(0);
	};
      } else { // table is empty, set RowNrs to indicate that no row was found
	*succRow = -1;
	*preRow = -1;
      };
      // set the new stopDate if necessary 
      if (*succRow == -1) {
	if (*stopDate == 0) {
	  *stopDate = masterTable_p->keywordSet().asuInt("maxDate"); 
	};
      } else {
	tmp = ROScalarColumn<uInt>(colTab,"StartDate")(*succRow);
	if ((*stopDate != 0) && (*stopDate > tmp)) {
	  cerr << "CalTableWriter::GetPreSucRows: " << 
	    "given stopDate is larger than next StartDate" << endl;
	  return false;
	} else if (*stopDate == 0) {
	  *stopDate = tmp;
	};
      };
    } catch (AipsError x) {
      cerr << "CalTableWriter::GetPreSucRows: " << x.getMesg() << endl;
      return false;
    }; 
    return true;
  }
  
  // ============================================================================
  // Implementation of the AddData methods
  // (Unfortunately every type needs its own implementation)
  // ============================================================================

  // -------------------------------------------------------------------- AddData
  
  bool CalTableWriter::AddData (String data,
				int const AntID,
				String const FieldName, 
				uInt const startDate,
				uInt const stopDate) {
    try {
      int newRowNr,preRowNr,succRowNr;
      uInt stopDa;
      Table subtab;
      String DataType;
      bool isJunior;
      if (!CalTableReader::GetColumnTable(AntID, FieldName, &subtab, &succRowNr, &DataType, &isJunior)) {
	cerr << "CalTableWriter::AddData: " << "Failed to get the ColumnTable" << endl;
	return false;
      };
      // check the data type
      if (DataType != "String") {
	cerr << "CalTableWriter::AddData: " << " (String)<-->" << FieldName << ":Incompatible data type" << endl;
	return false;
      };
      if (isJunior) { // this is a junior friend, so use an existing row.
	Vector<uInt> rowNrs;
	rowNrs = subtab( (subtab.col("StartDate") == (double)startDate) && 
			 (subtab.col("AntID")==AntID) ).rowNumbers(subtab);
	if (rowNrs.nelements() != 1) {
	  cerr << "CalTableWriter::AddData: " << "Failed to find a matching row while writing a junior friend." << endl;
	  return false;
	};
	ScalarColumn<String>(subtab,FieldName).put(rowNrs(0),data);
	ScalarColumn<bool>(subtab,(FieldName+"-mask")).put(rowNrs(0),true);
      } else {
	stopDa = stopDate;
	if (! GetPreSucRows(AntID, subtab, startDate, &stopDa, &preRowNr, &succRowNr)) {
	  cerr << "CalTableWriter::AddData: " << "error while retrieving preceding and succeding row" << endl;
	  return false;
	};
	if ((preRowNr !=-1) && ROScalarColumn<uInt>(subtab,"StartDate")(preRowNr) == startDate) {
	  // do not make a new row, but replace the old value
	  newRowNr = preRowNr;
	  preRowNr = -1;
	} else {
	  // add a new row 
	  subtab.addRow(1,true);
	  newRowNr = subtab.nrow()-1;
	  ScalarColumn<int>(subtab,"AntID").put(newRowNr,AntID);
	};
	// fill the data
	ScalarColumn<uInt>(subtab,"StartDate").put(newRowNr,startDate);
	ScalarColumn<uInt>(subtab,"StopDate").put(newRowNr,stopDa);
	ScalarColumn<String>(subtab,FieldName).put(newRowNr,data);
	ScalarColumn<bool>(subtab,(FieldName+"-mask")).put(newRowNr,true);
	// set the StopDate of the preceding row if necessary
	if (preRowNr != -1) { ScalarColumn<uInt>(subtab,"StopDate").put(preRowNr,startDate);};
      };
      subtab.flush();
    } catch (AipsError x) {
      cerr << "CalTableWriter::AddData:(catched) " << x.getMesg() << endl;
      return false;
    }; 
    return true;
  }

  // -------------------------------------------------------------------- AddData
  
  bool CalTableWriter::AddData (double data,
				int const AntID,
				String const FieldName, 
				uInt const startDate,
				uInt const stopDate) {
    try {
      int newRowNr,preRowNr,succRowNr;
      uInt stopDa;
      Table subtab;
      String DataType;
      bool isJunior;
      if (!CalTableReader::GetColumnTable(AntID, FieldName, &subtab, &succRowNr, &DataType, &isJunior)) {
	cerr << "CalTableWriter::AddData: " << "Failed to get the ColumnTable" << endl;
	return false;
      };
      // check the data type
      if (DataType != "double") {
	cerr << "CalTableWriter::AddData: " << " (double)<-->" << FieldName << ":Incompatible data type" << endl;
	return false;
      };
      if (isJunior) { // this is a junior friend, so use an existing row.
	Vector<uInt> rowNrs;
	rowNrs = subtab( (subtab.col("StartDate") == (double)startDate)&& 
			 (subtab.col("AntID")==AntID) ).rowNumbers(subtab);
	if (rowNrs.nelements() != 1) {
	  cerr << "CalTableWriter::AddData: " << "Failed to find a matching row while writing a junior friend." << endl;
	  return false;
	};
	ScalarColumn<double>(subtab,FieldName).put(rowNrs(0),data);
	ScalarColumn<bool>(subtab,(FieldName+"-mask")).put(rowNrs(0),true);
      } else {
	stopDa = stopDate;
	if (! GetPreSucRows(AntID, subtab, startDate, &stopDa, &preRowNr, &succRowNr)) {
	  cerr << "CalTableWriter::AddData: " << "error while retrieving preceding and succeding row" << endl;
	  return false;
	};
	if ((preRowNr !=-1) && ROScalarColumn<uInt>(subtab,"StartDate")(preRowNr) == startDate) {
	  // do not make a new row, but replace the old value
	  newRowNr = preRowNr;
	  preRowNr = -1;
	} else {
	  // add a new row 
	  subtab.addRow(1,true);
	  newRowNr = subtab.nrow()-1;
	  ScalarColumn<int>(subtab,"AntID").put(newRowNr,AntID);
	};
	// fill the data
	ScalarColumn<uInt>(subtab,"StartDate").put(newRowNr,startDate);
	ScalarColumn<uInt>(subtab,"StopDate").put(newRowNr,stopDa);
	ScalarColumn<double>(subtab,FieldName).put(newRowNr,data);
	ScalarColumn<bool>(subtab,(FieldName+"-mask")).put(newRowNr,true);
	// set the StopDate of the preceding row if necessary
	if (preRowNr != -1) { ScalarColumn<uInt>(subtab,"StopDate").put(preRowNr,startDate);};
      };
      subtab.flush();
    } catch (AipsError x) {
      cerr << "CalTableWriter::AddData:(catched) " << x.getMesg() << endl;
      return false;
    }; 
    return true;
  }

  // -------------------------------------------------------------------- AddData

  bool CalTableWriter::AddData (DComplex data,
				int const AntID,
				String const FieldName, 
				uInt const startDate,
				uInt const stopDate) {
    try {
      int newRowNr,preRowNr,succRowNr;
      uInt stopDa;
      Table subtab;
      String DataType;
      bool isJunior;
      if (!CalTableReader::GetColumnTable(AntID, FieldName, &subtab, &succRowNr, &DataType, &isJunior)) {
	cerr << "CalTableWriter::AddData: " << "Failed to get the ColumnTable" << endl;
	return false;
      };
      // check the data type
      if (DataType != "DComplex") {
	cerr << "CalTableWriter::AddData: " << " (DComplex)<-->" << FieldName << ":Incompatible data type" << endl;
	return false;
      };
      if (isJunior) { // this is a junior friend, so use an existing row.
	Vector<uInt> rowNrs;
	rowNrs = subtab( (subtab.col("StartDate") == (double)startDate) && 
			 (subtab.col("AntID")==AntID) ).rowNumbers(subtab);
	if (rowNrs.nelements() != 1) {
	  cerr << "CalTableWriter::AddData: " << "Failed to find a matching row while writing a junior friend." << endl;
	  return false;
	};
	ScalarColumn<DComplex>(subtab,FieldName).put(rowNrs(0),data);
	ScalarColumn<bool>(subtab,(FieldName+"-mask")).put(rowNrs(0),true);
      } else {
	stopDa = stopDate;
	if (! GetPreSucRows(AntID, subtab, startDate, &stopDa, &preRowNr, &succRowNr)) {
	  cerr << "CalTableWriter::AddData: " << "error while retrieving preceding and succeding row" << endl;
	  return false;
	};
	if ((preRowNr !=-1) && ROScalarColumn<uInt>(subtab,"StartDate")(preRowNr) == startDate) {
	  // do not make a new row, but replace the old value
	  newRowNr = preRowNr;
	  preRowNr = -1;
	} else {
	  // add a new row 
	  subtab.addRow(1,true);
	  newRowNr = subtab.nrow()-1;
	  ScalarColumn<int>(subtab,"AntID").put(newRowNr,AntID);
	};
	// fill the data
	ScalarColumn<uInt>(subtab,"StartDate").put(newRowNr,startDate);
	ScalarColumn<uInt>(subtab,"StopDate").put(newRowNr,stopDa);
	ScalarColumn<DComplex>(subtab,FieldName).put(newRowNr,data);
	ScalarColumn<bool>(subtab,(FieldName+"-mask")).put(newRowNr,true);
	// set the StopDate of the preceding row if necessary
	if (preRowNr != -1) { ScalarColumn<uInt>(subtab,"StopDate").put(preRowNr,startDate);};
      };
      subtab.flush();
    } catch (AipsError x) {
      cerr << "CalTableWriter::AddData:(catched) " << x.getMesg() << endl;
      return false;
    }; 
    return true;
  }
  bool CalTableWriter::AddData(Array<double> data,  int const AntID, String const FieldName, 
			       uInt const startDate, uInt const stopDate){
    try {
      int newRowNr,preRowNr,succRowNr;
      uInt stopDa;
      Table subtab;
      String DataType;
      bool isJunior;
      if (!CalTableReader::GetColumnTable(AntID, FieldName, &subtab, &succRowNr, &DataType, &isJunior)) {
	cerr << "CalTableWriter::AddData: " << "Failed to get the ColumnTable" << endl;
	return false;
      };
      // check the data type
      if (DataType != "Array<double>") {
	cerr << "CalTableWriter::AddData: " << " (Array<double>)<-->" << FieldName << ":Incompatible data type" << endl;
	return false;
      };
      if (isJunior) { // this is a junior friend, so use an existing row.
	Vector<uInt> rowNrs;
	rowNrs = subtab( (subtab.col("StartDate") == (double)startDate) && 
			 (subtab.col("AntID")==AntID) ).rowNumbers(subtab);
	if (rowNrs.nelements() != 1) {
	  cerr << "CalTableWriter::AddData: " << "Failed to find a matching row while writing a junior friend." << endl;
	  return false;
	};
	ArrayColumn<double>(subtab,FieldName).put(rowNrs(0),data);
	ScalarColumn<bool>(subtab,(FieldName+"-mask")).put(rowNrs(0),true);
	subtab.flush();
      } else {
	stopDa = stopDate;
	if (! GetPreSucRows(AntID, subtab, startDate, &stopDa, &preRowNr, &succRowNr)) {
	  cerr << "CalTableWriter::AddData: " << "error while retrieving preceding and succeding row" << endl;
	  return false;
	};
	if ((preRowNr !=-1) && ROScalarColumn<uInt>(subtab,"StartDate")(preRowNr) == startDate) {
	  // do not make a new row, but replace the old value
	  newRowNr = preRowNr;
	  preRowNr = -1;
	} else {
	  // add a new row 
	  subtab.addRow(1,true);
	  newRowNr = subtab.nrow()-1;
	  ScalarColumn<int>(subtab,"AntID").put(newRowNr,AntID);
	};
	// fill the data
	ScalarColumn<uInt>(subtab,"StartDate").put(newRowNr,startDate);
	ScalarColumn<uInt>(subtab,"StopDate").put(newRowNr,stopDa);
	ArrayColumn<double>(subtab,FieldName).put(newRowNr,data);
	ScalarColumn<bool>(subtab,(FieldName+"-mask")).put(newRowNr,true);
	// set the StopDate of the preceding row if necessary
	if (preRowNr != -1) { ScalarColumn<uInt>(subtab,"StopDate").put(preRowNr,startDate);};
      };
      subtab.flush();
    } catch (AipsError x) {
      cerr << "CalTableWriter::AddData:(catched) " << x.getMesg() << endl;
      return false;
    }; 
    return true;
  }
  bool CalTableWriter::AddData(Array<DComplex> data,  int const AntID, String const FieldName, 
			       uInt const startDate, uInt const stopDate){
  try {
    int newRowNr,preRowNr,succRowNr;
    uInt stopDa;
    Table subtab;
    String DataType;
    bool isJunior;
    if (!CalTableReader::GetColumnTable(AntID, FieldName, &subtab, &succRowNr, &DataType, &isJunior)) {
      cerr << "CalTableWriter::AddData: " << "Failed to get the ColumnTable" << endl;
      return false;
    };
    // check the data type
    if (DataType != "Array<DComplex>") {
      cerr << "CalTableWriter::AddData: " << " (Array<DComplex>)<-->" << FieldName << ":Incompatible data type" << endl;
      return false;
    };
    if (isJunior) { // this is a junior friend, so use an existing row.
      Vector<uInt> rowNrs;
      rowNrs = subtab( (subtab.col("StartDate") == (double)startDate) && 
		       (subtab.col("AntID")==AntID) ).rowNumbers(subtab);
      if (rowNrs.nelements() != 1) {
	cerr << "CalTableWriter::AddData: " << "Failed to find a matching row while writing a junior friend." << endl;
	return false;
      };
      ArrayColumn<DComplex>(subtab,FieldName).put(rowNrs(0),data);
      ScalarColumn<bool>(subtab,(FieldName+"-mask")).put(rowNrs(0),true);
    } else {
      stopDa = stopDate;
      if (! GetPreSucRows(AntID, subtab, startDate, &stopDa, &preRowNr, &succRowNr)) {
	cerr << "CalTableWriter::AddData: " << "error while retrieving preceding and succeding row" << endl;
	return false;
      };
      if ((preRowNr !=-1) && ROScalarColumn<uInt>(subtab,"StartDate")(preRowNr) == startDate) {
	// do not make a new row, but replace the old value
	newRowNr = preRowNr;
	preRowNr = -1;
      } else {
	// add a new row 
	subtab.addRow(1,true);
	newRowNr = subtab.nrow()-1;
	ScalarColumn<int>(subtab,"AntID").put(newRowNr,AntID);
      };
      // fill the data
      ScalarColumn<uInt>(subtab,"StartDate").put(newRowNr,startDate);
      ScalarColumn<uInt>(subtab,"StopDate").put(newRowNr,stopDa);
      ArrayColumn<DComplex>(subtab,FieldName).put(newRowNr,data);
      ScalarColumn<bool>(subtab,(FieldName+"-mask")).put(newRowNr,true);
      // set the StopDate of the preceding row if necessary
      if (preRowNr != -1) { ScalarColumn<uInt>(subtab,"StopDate").put(preRowNr,startDate);};
    };
    subtab.flush();
  } catch (AipsError x) {
    cerr << "CalTableWriter::AddData:(catched) " << x.getMesg() << endl;
    return false;
  }; 
  return true;
}



// ==============================================================================
// Implementation of the AddField method
// ==============================================================================
bool CalTableWriter::AddField(const String FieldName, const String FieldDesc, const String DataType, 
			       const bool shapeFixed, const IPosition shape, const bool SingleTables,
			       const String FriendField){
  try {
    if (masterTable_p->actualTableDesc().isColumn(FieldName)){
      cerr<<"CalTableReader::AddField "<<"A field with the name "<< FieldName<<" already exists!"<<endl;
      return false;
    };
    if ((FriendField.length() != 0 )&&(!masterTable_p->actualTableDesc().isColumn(FriendField))){
      cerr<<"CalTableReader::AddField "<<"Unknown friend field "<< FriendField <<"!"<<endl;
      return false;
    };
    if (masterTable_p->nrow()<1) {
      cerr<<"CalTableReader::AddField "<<"Cannot add a field to a table without antennas." << endl;
      cerr<<"CalTableReader::AddField "<<"I'm sorry, but that's the way it is." << endl;
      return false;
    };
    // make the description of the new data column
    ColumnDesc newCol;
    if (DataType=="String") {
      newCol = ScalarColumnDesc<String>(FieldName);
    } else if (DataType=="double") {
      newCol = ScalarColumnDesc<double>(FieldName);
    } else if (DataType=="DComplex") {
      newCol = ScalarColumnDesc<DComplex>(FieldName);
    } else if (DataType=="Array<double>") {
	if (shapeFixed){
	  if (shape.nelements() == 0) {
	    cerr<<"CalTableReader::AddField: "<<"Array of fixed shape of length 0."<<endl;
	    return false;
	  };
	  newCol = ArrayColumnDesc<double>(FieldName,shape,ColumnDesc::Direct);
	} else {
	  newCol = ArrayColumnDesc<double>(FieldName);
	};
    } else if (DataType=="Array<DComplex>") {
      if (shapeFixed){
	if (shape.nelements() == 0) {
	  cerr<<"CalTableReader::AddField: "<<"Array of fixed shape of length 0."<<endl;
	  return false; 
	};
	newCol = ArrayColumnDesc<DComplex>(FieldName,shape,ColumnDesc::Direct);
      } else {
	newCol = ArrayColumnDesc<DComplex>(FieldName);
      };
    } else {
      cerr<<"CalTableReader::AddField: "<<"unknown data type "<< DataType <<"!"<<endl;
      return false;     
    };
    ScalarColumnDesc<bool> newMaskCol((FieldName+"-mask"),("Bitmask for field "+FieldName));
    newMaskCol.setDefault(false);
    Table subTable;
    // make the table description for the new subtable
    TableDesc entryDesc("master",TableDesc::Scratch);
    entryDesc.addColumn(ScalarColumnDesc<uInt>("StartDate", "First date when this entry is valid"));
    entryDesc.addColumn(ScalarColumnDesc<uInt>("StopDate", "Last date when this entry is valid"));
    entryDesc.addColumn(ScalarColumnDesc<int>("AntID", "ID of the antenna"));
    entryDesc.addColumn(newCol);
    entryDesc.addColumn(newMaskCol);
    
    // make the column description for the column in the master table
    ScalarColumnDesc<String>  newColDesc(FieldName,FieldDesc); 
    // add the column
    masterTable_p->addColumn(newColDesc);
    ScalarColumn<String> col(*masterTable_p,FieldName);
    col.rwKeywordSet().define("DataType",DataType);
    col.rwKeywordSet().define("SingleTables",SingleTables);
    col.rwKeywordSet().define("FriendField",FriendField);
    String tableName,tablePath;
    uInt i;
    if (FriendField.length()==0){ // has no FriendField
      if (SingleTables){
	// fill the column with empty subtables
	ROScalarColumn<int> antcol(*masterTable_p,"AntID");
	for (i=0; i<masterTable_p->nrow(); i++) { //one table for every antenna
	  tableName = FieldName + antcol(i);
	  tablePath = masterTable_p->tableName() + "/" + tableName;
	  SetupNewTable subTableMaker(tablePath,entryDesc,Table::NewNoReplace);
	  subTable = Table(subTableMaker);
	  col.rwKeywordSet().defineTable(tableName,subTable);
	  col.put(i,tableName);
      };
      } else {
	// one table for all antennas
	tableName = FieldName;
	tablePath = masterTable_p->tableName() + "/" + tableName;
	SetupNewTable subTableMaker(tablePath,entryDesc,Table::NewNoReplace);
	subTable = Table(subTableMaker);
	col.rwKeywordSet().defineTable(tableName,subTable);
	col.rwKeywordSet().define("TableName",tableName);
	for (i=0; i<masterTable_p->nrow(); i++) {
	  col.put(i,tableName);
	};
      };
    } else { // has a FriendField
      ROScalarColumn<String> friendCol(*masterTable_p,FriendField);
      if (friendCol.keywordSet().asBool("SingleTables")){ //FriendField has single tables
	col.rwKeywordSet().define("SingleTables",true);
	for (i=0; i<masterTable_p->nrow(); i++) {
	  tableName = friendCol(i);
	  subTable = friendCol.keywordSet().asTable(tableName);
	  subTable.addColumn(newCol);
	  subTable.addColumn(newMaskCol);
	  col.rwKeywordSet().defineTable(tableName,subTable);
	  col.put(i,tableName);
	};
      } else { //FriendField has a common table
	col.rwKeywordSet().define("SingleTables",false);
	tableName = friendCol.keywordSet().asString("TableName");
	subTable = friendCol.keywordSet().asTable(tableName);
	subTable.addColumn(newCol);
	subTable.addColumn(newMaskCol);
	col.rwKeywordSet().defineTable(tableName,subTable);
	col.rwKeywordSet().define("TableName",tableName);
	for (i=0; i<masterTable_p->nrow(); i++) {
	  col.put(i,tableName);
	};
      };
    };
  } catch (AipsError x) {
    cerr << "CalTableWriter::AddField: " << x.getMesg() << endl;
    return false;
  }; 
  return true; 
}


// ==============================================================================
// Implementation of the AddAntenna method
// ==============================================================================
  
  bool CalTableWriter::AddAntenna(int AntID,
				  String AntName) {
    try {
      bool found;
      int rowNr;
      uInt i;
      // check whether this ID already exists
      **indexedAnt_p = AntID;
      rowNr = AntIDIndex_p->getRowNumber(found);
      if (found) {
	cerr << "CalTableWriter::AddAntenna: " << "Antenna ID already exists!" << endl;
	return false;
      };
    // Add the row
    masterTable_p->addRow();
    rowNr = masterTable_p->nrow()-1;
    TableRow row(*masterTable_p); //generates the internal record 
    Vector<String> colNames = row.columnNames();
    if ((colNames(0) != "AntID") || (colNames(1) != "AntName")){
      cerr << "CalTableWriter::AddAntenna: " << "The first two columns are not AntID and AntName!" << endl;
      cerr << "The access to rows with AIPS++ tables sucks!" << endl;
      return false;
    };
    row.record().define("AntID",AntID);
    row.record().define("AntName",AntName);
    String tableName;
    Table subTable;
    for (i=2; i<colNames.nelements(); i++) {
      ScalarColumn<String> col(*masterTable_p,colNames(i));
      if (col.keywordSet().asString("FriendField").length() == 0){ // this column has no friends
	if (col.keywordSet().asBool("SingleTables")){ //this column has single tables
	  TableDesc subTabDesc(col.keywordSet().asTable(col(0)).actualTableDesc());
	  tableName = colNames(i) + AntID;
	  String tablePath = masterTable_p->tableName() + "/" + tableName;
	  SetupNewTable subTableMaker(tablePath,subTabDesc,Table::NewNoReplace);
	  subTable = Table(subTableMaker);
	  col.rwKeywordSet().defineTable(tableName,subTable);
	  row.record().define(colNames(i),tableName);
	} else {
	  tableName = col(0);
	  row.record().define(colNames(i),tableName);
	};
      } else { // this column has a FriendField, 
	//this means also for single tables the subtable is already created.
	String FriendField=col.keywordSet().asString("FriendField");
	ScalarColumn<String> friendCol(*masterTable_p,FriendField);
	tableName = row.record().asString(FriendField);
	subTable = friendCol.keywordSet().asTable(tableName);
	if (!col.keywordSet().isDefined(tableName)) //only true for single tables
	    col.rwKeywordSet().defineTable(tableName,subTable);
	row.record().define(colNames(i),tableName);
      };
    };
    row.put(rowNr); //put the data back into the table
  } catch (AipsError x) {
    cerr << "CalTableWriter::AddAntenna: " << x.getMesg() << endl;
    return false;
  }; 
  return true; 
}


// ==============================================================================
// Implementation of the SetKeyword method
// (Unfortunately every type needs its own implementation)
// ==============================================================================
bool CalTableWriter::SetKeyword(String data, String const KeywordName) {
  try {
    if (masterTable_p->keywordSet().isDefined(KeywordName)) {
      masterTable_p->rwKeywordSet().removeField(KeywordName); //not nice but should work...
    };
    masterTable_p->rwKeywordSet().define(KeywordName,data);
  } catch (AipsError x) {
    cerr << "CalTableWriter::SetKeyword: " << x.getMesg() << endl;
    return false;
  }; 
  return true; 
}
bool CalTableWriter::SetKeyword(double data, String const KeywordName) {
  try {
    if (masterTable_p->keywordSet().isDefined(KeywordName)) {
      masterTable_p->rwKeywordSet().removeField(KeywordName); //not nice but should work...
    };
    masterTable_p->rwKeywordSet().define(KeywordName,data);
  } catch (AipsError x) {
    cerr << "CalTableWriter::SetKeyword: " << x.getMesg() << endl;
    return false;
  }; 
  return true; 
}
bool CalTableWriter::SetKeyword(DComplex data, String const KeywordName) {
  try {
    if (masterTable_p->keywordSet().isDefined(KeywordName)) {
      masterTable_p->rwKeywordSet().removeField(KeywordName); //not nice but should work...
    };
    masterTable_p->rwKeywordSet().define(KeywordName,data);
  } catch (AipsError x) {
    cerr << "CalTableWriter::SetKeyword: " << x.getMesg() << endl;
    return false;
  }; 
  return true; 
}
bool CalTableWriter::SetKeyword(Array<double> data, String const KeywordName) {
  try {
    if (masterTable_p->keywordSet().isDefined(KeywordName)) {
      masterTable_p->rwKeywordSet().removeField(KeywordName); //not nice but should work...
    };
    masterTable_p->rwKeywordSet().define(KeywordName,data);
  } catch (AipsError x) {
    cerr << "CalTableWriter::SetKeyword: " << x.getMesg() << endl;
    return false;
  }; 
  return true; 
}
bool CalTableWriter::SetKeyword(Array<DComplex> data, String const KeywordName) {
  try {
    if (masterTable_p->keywordSet().isDefined(KeywordName)) {
      masterTable_p->rwKeywordSet().removeField(KeywordName); //not nice but should work...
    };
    masterTable_p->rwKeywordSet().define(KeywordName,data);
  } catch (AipsError x) {
    cerr << "CalTableWriter::SetKeyword: " << x.getMesg() << endl;
    return false;
  }; 
  return true; 
}

}  // Namespace CR -- end
