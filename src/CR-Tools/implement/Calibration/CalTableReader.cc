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

/* $Id$*/

#include <Calibration/CalTableReader.h>

namespace CR {  // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  CalTableReader::CalTableReader () {
    init();
  }
  
  CalTableReader::CalTableReader (const String& tableFilename) {
    init();
    AttachTable(tableFilename);
  }
  
  //CalTableReader::CalTableReader (CalTableReader const& other) {
  //}
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  CalTableReader::~CalTableReader (){
    cleanup();
  }
  
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

  // ----------------------------------------------------------------------- init
  
  Bool CalTableReader::init() {
    masterTable_p = NULL;
    AntIDIndex_p = NULL;
    indexedAnt_p = NULL;
    return True;
  }
  
  // --------------------------------------------------------------------- cleanup

  Bool CalTableReader::cleanup() {
    return True;
  }
  
  // ---------------------------------------------------------------- AttachTable

  Bool CalTableReader::AttachTable(const String& tableFilename){
  try {
    masterTable_p = new Table(tableFilename);
    AntIDIndex_p = new ColumnsIndex(*masterTable_p ,String("AntID"));
    indexedAnt_p = new RecordFieldPtr<Int>(AntIDIndex_p->accessKey(),"AntID");
  } catch (AipsError x) {
    cerr << "CalTableReader::AttachTable: " << x.getMesg() << endl;
    return False;
  }; 
  return True;
  }
  
  // ----------------------------------------------------------------- isAttached

  Bool CalTableReader::isAttached(){
    Bool erg=False;
    try {
      if ((masterTable_p==NULL)||(masterTable_p->isNull())) { 
	erg = False;
      } else { erg = True; };
    } catch (AipsError x) {
      cerr << "CalTableReader::isAttached: " << x.getMesg() << endl;
      return False;
    }; 
    return erg;
  }
  
  
  // ==============================================================================
  // Get the given column table
  // ==============================================================================
  
  Bool  CalTableReader::GetColumnTable(Int const AntID,
				       String const FieldName,
				       Table *subtab,
				       Int *rowNr,
				       String *DataType,
				       Bool *isJunior ){
    try {  
      // get column
      ROScalarColumn<String> col(*masterTable_p,FieldName);  
      // ret the data type
      if (DataType != NULL) {
	*DataType = col.keywordSet().asString("DataType");
      };
      if (isJunior != NULL) {
	*isJunior = col.keywordSet().asString("FriendField").length() != 0;
      };
      // get the row number
      Bool found;
      **indexedAnt_p = AntID;
      *rowNr = AntIDIndex_p->getRowNumber(found);
      if (!found) {
	cerr << "CalTableReader::GetColumnTable: " << "unknown antenna ID:" << AntID <<"!" << endl;
	return False;
      };
      // get the table
      String TableName;
      TableName = col(*rowNr);
      *subtab = col.keywordSet().asTable(TableName);
    } catch (AipsError x) {
      cerr << "CalTableReader::GetColumnTable: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }
  
  // ==============================================================================
  // Get the row number to the corresponding date
  // ==============================================================================
  
  Int CalTableReader::GetDateRow(Int const AntID,
				 Table const colTable,
				 uInt const date,
				 const Bool mindEmpty) {
    Vector<uInt> rowNrs;
    try { 
      rowNrs = colTable(colTable.col("AntID")==AntID && colTable.col("StartDate")<=(Double)date && 
			colTable.col("StopDate")>(Double)date).rowNumbers(colTable);
    } catch (AipsError x) {
      cerr << "CalTableReader::GetDateRow: " << x.getMesg() << endl;
      return -1;
    }; 
    if ( rowNrs.nelements() == 1) {
      return rowNrs(0);
    } else if ( rowNrs.nelements() < 1) {
      if (mindEmpty) {
	cerr << "CalTableReader::GetDateRow: " << "Cannot find date in table: " << colTable.tableName() << endl;
      };
      return -1 ;
    };
    cerr << "CalTableReader::GetDateRow: " << "colTable inconsistent: more than one fitting row!" << endl;
    return -2;
  }
  
  Int CalTableReader::GetJuniorDateRow(Int const AntID,
				       Table const colTable,
				       uInt const date,
				       String MaskField,
				       const Bool mindEmpty) {
    Vector<uInt> rowNrs;
    try { 
      rowNrs = colTable(colTable.col("AntID")==AntID && colTable.col("StartDate")<=(Double)date && 
			colTable.col(MaskField)).sort("StartDate").rowNumbers(colTable);
    } catch (AipsError x) {
      cerr << "CalTableReader::GetJuniorDateRow: " << x.getMesg() << endl;
      return -1;
    }; 
    if ( rowNrs.nelements() >= 1) {
      return rowNrs(rowNrs.nelements()-1);
    } else if ( rowNrs.nelements() < 1) {
      if (mindEmpty) {
	cerr << "CalTableReader::GetJuniorDateRow: " << "Cannot find date in table for field: " << MaskField << endl;
      };
      return -1 ;
    };
    cerr << "CalTableReader::GetJuniorDateRow: " << "colTable inconsistent: more than one fitting row!" << endl;
    return -2;
  }
  
  
  // ==============================================================================
  // Implementation of the GetData methods
  // (Unfortunately every type needs its own implementation)
  // ==============================================================================
  
  Bool CalTableReader::GetData(uInt const date,
			       Int const AntID,
			       String const FieldName,
			       String *result) {
#ifdef DEBUGGING_MESSAGES      
    cout << "CalTableReader::GetData[String] called for " << FieldName << endl;
#endif
    try {
      Int rowNr;
      Table subtab;
      String DataType;
      Bool isJunior;
      if (!GetColumnTable(AntID, FieldName, &subtab, &rowNr, &DataType, &isJunior)) {
	cerr << "CalTableReader::GetData " << "Failed to get the ColumnTable" << endl;
	return False;
      };
      // check the data type
      if (DataType != "String") {
	cerr << "CalTableReader::GetData " << "(String)<-->" << FieldName << ":Incompatible data type" << endl;
	return False;
      };
      if (isJunior) {
	rowNr = GetJuniorDateRow(AntID, subtab, date, (FieldName+"-mask"));
      } else {
	rowNr = GetDateRow(AntID, subtab, date);
      };
      if (rowNr >= 0) {
	*result = ROScalarColumn<String>(subtab,FieldName)(rowNr);
      } else {
	cerr << "CalTableReader::GetData: " << "Date not found!"  << endl;
      };
    } catch (AipsError x) {
      cerr << "CalTableReader::GetData: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }
  
  Bool CalTableReader::GetData(uInt const date,
			       Int const AntID,
			       String const FieldName,
			       Double *result) {
#ifdef DEBUGGING_MESSAGES      
    cout << "CalTableReader::GetData[Double] called for " << FieldName << endl;
#endif
    try {
      Int rowNr;
      Table subtab;
      String DataType;
      Bool isJunior;
      if (!GetColumnTable(AntID, FieldName, &subtab, &rowNr, &DataType, &isJunior)) {
	cerr << "CalTableReader::GetData " << "Failed to get the ColumnTable" << endl;
	return False;
      };
      // check the data type
      if (DataType != "Double") {
	cerr << "CalTableReader::GetData " << "(Double)<-->" << FieldName << ":Incompatible data type" << endl;
	return False;
      };
      if (isJunior) {
	rowNr = GetJuniorDateRow(AntID, subtab, date, (FieldName+"-mask"));
      } else {
	rowNr = GetDateRow(AntID, subtab, date);
      };
      if (rowNr >= 0) {
	*result = ROScalarColumn<Double>(subtab,FieldName)(rowNr);
      } else {
	cerr << "CalTableReader::GetData: " << "Date not found!"  << endl;
      };
    } catch (AipsError x) {
      cerr << "CalTableReader::GetData: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }
  
  Bool CalTableReader::GetData(uInt const date,
			       Int const AntID,
			       String const FieldName,
			       DComplex *result) {
#ifdef DEBUGGING_MESSAGES      
    cout << "CalTableReader::GetData[DComplex] called for " << FieldName << endl;
#endif
    try {
      Int rowNr;
      Table subtab;
      String DataType;
      Bool isJunior;
      if (!GetColumnTable(AntID, FieldName, &subtab, &rowNr, &DataType, &isJunior)) {
	cerr << "CalTableReader::GetData " << "Failed to get the ColumnTable" << endl;
	return False;
      };
      // check the data type
      if (DataType != "DComplex") {
	cerr << "CalTableReader::GetData " << "(DComplex)<-->" << FieldName << ":Incompatible data type" << endl;
	return False;
      };
      if (isJunior) {
	rowNr = GetJuniorDateRow(AntID, subtab, date, (FieldName+"-mask"));
      } else {
	rowNr = GetDateRow(AntID, subtab, date);
      };
      if (rowNr >= 0) {
	*result = ROScalarColumn<DComplex>(subtab,FieldName)(rowNr);
      } else {
	cerr << "CalTableReader::GetData: " << "Date not found!"  << endl;
      };
    } catch (AipsError x) {
      cerr << "CalTableReader::GetData: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }
  
  Bool CalTableReader::GetData(uInt const date,
			       Int const AntID,
			       String const FieldName,
			       Array<Double> *result) {
#ifdef DEBUGGING_MESSAGES      
    cout << "CalTableReader::GetData[Array<Double>] called for " << FieldName << endl;
#endif
    try {
      Int rowNr;
      Table subtab;
      String DataType;
      Bool isJunior;
      if (!GetColumnTable(AntID, FieldName, &subtab, &rowNr, &DataType, &isJunior)) {
	cerr << "CalTableReader::GetData " << "Failed to get the ColumnTable" << endl;
	return False;
      };
      // check the data type
      if (DataType != "Array<Double>") {
	cerr << "CalTableReader::GetData " << "(Array<Double>)<-->" << FieldName << ":Incompatible data type" << endl;
	return False;
      };
      if (isJunior) {
	rowNr = GetJuniorDateRow(AntID, subtab, date, (FieldName+"-mask"));
      } else {
	rowNr = GetDateRow(AntID, subtab, date);
      };
      if (rowNr >= 0) {
	result->reference(ROArrayColumn<Double>(subtab,FieldName)(rowNr));
      } else {
	cerr << "CalTableReader::GetData: " << "Date not found!"  << endl;
      };
    } catch (AipsError x) {
      cerr << "CalTableReader::GetData: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }
  
  Bool CalTableReader::GetData(uInt const date,
			       Int const AntID,
			       String const FieldName,
			       Array<DComplex> *result) {
#ifdef DEBUGGING_MESSAGES      
    cout << "CalTableReader::GetData[Array<DComplex>] called for " << FieldName << endl;
#endif
    try {
      Int rowNr;
      Table subtab;
      String DataType;
      Bool isJunior;
      if (!GetColumnTable(AntID, FieldName, &subtab, &rowNr, &DataType, &isJunior)) {
	cerr << "CalTableReader::GetData " << "Failed to get the ColumnTable" << endl;
	return False;
      };
      // check the data type
      if (DataType != "Array<DComplex>") {
	cerr << "CalTableReader::GetData " << "(Array<DComplex>)<-->" << FieldName << ":Incompatible data type" << endl;
	return False;
      };
      if (isJunior) {
	rowNr = GetJuniorDateRow(AntID, subtab, date, (FieldName+"-mask"));
      } else {
	rowNr = GetDateRow(AntID, subtab, date);
      };
      if (rowNr >= 0) {
	result->reference(ROArrayColumn<DComplex>(subtab,FieldName)(rowNr));
      } else {
	cerr << "CalTableReader::GetData: " << "Date not found!"  << endl;
      };
    } catch (AipsError x) {
      cerr << "CalTableReader::GetData: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };
  
  
  // ==============================================================================
  // Has the value of this field changed between the two given dates?
  // ==============================================================================
  
  Bool CalTableReader::isIdentical(uInt const date1,
				   uInt const date2,
				   Int const AntID, 
				   String const FieldName) {
#ifdef DEBUGGING_MESSAGES      
    cout << "CalTableReader::isIdentical called for " << FieldName << endl;
#endif
    try {  
      Int rowNr;
      Table subtab;
      Bool isJunior;
      //if (date1 == date2) { return True; } // Trivial case. Does not check whether date is valid.
      if (!GetColumnTable(AntID, FieldName, &subtab, &rowNr, NULL, &isJunior)) {
	cerr << "CalTableReader::isIdentical " << "Failed to get the ColumnTable" << endl;
	return False;
      };
      uInt row1,row2;
      if (isJunior) {
	row1 = GetJuniorDateRow(AntID, subtab, date1, (FieldName+"-mask"), False);
	row2 = GetJuniorDateRow(AntID, subtab, date2, (FieldName+"-mask"), False);
      } else {
	row1 = GetDateRow(AntID, subtab, date1, False);
	row2 = GetDateRow(AntID, subtab, date2, False);
      };
      if ( row1==row2 && row1>=0) return True;
    } catch (AipsError x) {
      cerr << "CalTableReader::isIdentical: " << x.getMesg() << endl;
      return False;
    }; 
    return False;
  };
  
  
  // ==============================================================================
  // Get the type of the given field
  // ==============================================================================
  
  String  CalTableReader::GetFieldType(String const FieldName) {
    try {  
      // get column
      ROTableColumn col(*masterTable_p,FieldName);
      return col.keywordSet().asString("DataType");
    } catch (AipsError x) {
      cerr << "CalTableReader::GetColumnTable: " << x.getMesg() << endl;
      return "";
    }; 
    return "";
  }
  
  
  Bool CalTableReader::PrintSummary() {
    cout << *masterTable_p << endl;
    return True;
  }
  
  // ==============================================================================
  // Implementation of the GetKeyword methods
  // (Unfortunately every type needs its own implementation)
  // ==============================================================================
  Bool CalTableReader::GetKeyword(String const KeywordName, String *result){
#ifdef DEBUGGING_MESSAGES      
    cout << "CalTableReader::GetKeyword called for " << KeywordName << endl;
#endif
    try {  
      if (!masterTable_p->keywordSet().isDefined(KeywordName)) {
	cerr << "CalTableReader::GetKeyword: Keyword named \"" << KeywordName << 
	  "\" does not exist" << endl;
	return False;
      };
      *result = masterTable_p->keywordSet().asString(KeywordName);
    } catch (AipsError x) {
      cerr << "CalTableReader::GetKeyword: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }
  Bool CalTableReader::GetKeyword(String const KeywordName, Double *result){
#ifdef DEBUGGING_MESSAGES      
    cout << "CalTableReader::GetKeyword called for " << KeywordName << endl;
#endif
    try {  
      if (!masterTable_p->keywordSet().isDefined(KeywordName)) {
	cerr << "CalTableReader::GetKeyword: Keyword named \"" << KeywordName << 
	  "\" does not exist" << endl;
	return False;
      };
      *result = masterTable_p->keywordSet().asDouble(KeywordName);
    } catch (AipsError x) {
      cerr << "CalTableReader::GetKeyword: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }
  Bool CalTableReader::GetKeyword(String const KeywordName, DComplex *result){
#ifdef DEBUGGING_MESSAGES      
    cout << "CalTableReader::GetKeyword called for " << KeywordName << endl;
#endif
    try {  
      if (!masterTable_p->keywordSet().isDefined(KeywordName)) {
	cerr << "CalTableReader::GetKeyword: Keyword named \"" << KeywordName << 
	  "\" does not exist" << endl;
	return False;
      };
      *result = masterTable_p->keywordSet().asDComplex(KeywordName);
    } catch (AipsError x) {
      cerr << "CalTableReader::GetKeyword: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }
  Bool CalTableReader::GetKeyword(String const KeywordName, Array<Double> *result){
#ifdef DEBUGGING_MESSAGES      
    cout << "CalTableReader::GetKeyword called for " << KeywordName << endl;
#endif
    try {  
      if (!masterTable_p->keywordSet().isDefined(KeywordName)) {
	cerr << "CalTableReader::GetKeyword: Keyword named \"" << KeywordName << 
	  "\" does not exist" << endl;
	return False;
      };
      result->reference(masterTable_p->keywordSet().asArrayDouble(KeywordName));
    } catch (AipsError x) {
      cerr << "CalTableReader::GetKeyword: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }
  Bool CalTableReader::GetKeyword(String const KeywordName, Array<DComplex> *result){
#ifdef DEBUGGING_MESSAGES      
    cout << "CalTableReader::GetKeyword called for " << KeywordName << endl;
#endif
    try {  
      if (!masterTable_p->keywordSet().isDefined(KeywordName)) {
	cerr << "CalTableReader::GetKeyword: Keyword named \"" << KeywordName << 
	  "\" does not exist" << endl;
	return False;
      };
      result->reference(masterTable_p->keywordSet().asArrayDComplex(KeywordName));
    } catch (AipsError x) {
      cerr << "CalTableReader::GetKeyword: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }
  
  
  // ==============================================================================
  // Get the type of the given keyword
  // ==============================================================================
  String CalTableReader::GetKeywordType(String const KeywordName){
    try {  
      if (!masterTable_p->keywordSet().isDefined(KeywordName)) {
	cerr << "CalTableReader::GetKeywordType: Keyword named \"" << KeywordName << 
	  "\" does not exist" << endl;
	return "";
      };
      DataType type = masterTable_p->keywordSet().dataType(KeywordName);
      switch(type) {
      case TpString:
	return "String";
      case TpDouble:
	return "Double";
      case TpDComplex:
	return "DComplex";
      case TpArrayDouble:
	return "Array<Double>";
      case TpArrayDComplex:
	return "Array<DComplex>";
      case TpInt:
	return "Int";
      case TpUInt:
	return "uInt";
      default:
	return "";
      }
    } catch (AipsError x) {
      cerr << "CalTableReader::GetKeywordType: " << x.getMesg() << endl;
      return "";
    }; 
    return "";
  }
  
}  // Namespace CR -- end
