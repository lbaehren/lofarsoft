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

/* $Id: CalTableReader.cc,v 1.5 2007/01/15 14:41:09 horneff Exp $*/

#include <Calibration/CalTableReader.h>

using std::cerr;
using std::cout;
using std::endl;

using casa::ROArrayColumn;
using casa::ROScalarColumn;
using casa::ROTableColumn;
using casa::Vector;

namespace CR {  // Namespace CR -- begin
  
  // ==============================================================================
  //
  //  Construction
  //
  // ==============================================================================
  
  CalTableReader::CalTableReader () {
    init();
  }
  
  CalTableReader::CalTableReader (const String& tableFilename) {
    init();
    AttachTable(tableFilename);
  }
  
  //CalTableReader::CalTableReader (CalTableReader const& other) {
  //}
  
  // ==============================================================================
  //
  //  Destruction
  //
  // ==============================================================================
  
  CalTableReader::~CalTableReader (){
    cleanup();
  }
  
  // ==============================================================================
  //
  //  Parameters
  //
  // ==============================================================================
  
  
  
  // ==============================================================================
  //
  //  Methods
  //
  // ==============================================================================
  
  bool CalTableReader::init() {
    masterTable_p = NULL;
    AntIDIndex_p = NULL;
    indexedAnt_p = NULL;
    return true;
  }
  
  bool CalTableReader::cleanup() {
    return true;
  }
  
  bool CalTableReader::AttachTable(const String& tableFilename){
    try {
      masterTable_p = new Table(tableFilename);
      AntIDIndex_p = new ColumnsIndex(*masterTable_p ,String("AntID"));
      indexedAnt_p = new RecordFieldPtr<int>(AntIDIndex_p->accessKey(),"AntID");
    } catch (AipsError x) {
      cerr << "CalTableReader::AttachTable: " << x.getMesg() << endl;
      return false;
    }; 
    return true;
  }
  
  bool CalTableReader::isAttached(){
    bool erg=false;
    try {
      if ((masterTable_p==NULL)||(masterTable_p->isNull())) { 
	erg = false;
      } else { erg = true; };
    } catch (AipsError x) {
      cerr << "CalTableReader::isAttached: " << x.getMesg() << endl;
      return false;
    }; 
    return erg;
  }
  
  
  // ==============================================================================
  // Get the given column table
  // ==============================================================================
  
  bool  CalTableReader::GetColumnTable(int const AntID,
				       String const FieldName,
				       Table *subtab,
				       int *rowNr,
				       String *DataType,
				       bool *isJunior ){
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
      bool found;
      **indexedAnt_p = AntID;
      *rowNr = AntIDIndex_p->getRowNumber(found);
      if (!found) {
	cerr << "CalTableReader::GetColumnTable: " << "unknown antenna ID!" << endl;
	return false;
      };
      // get the table
      String TableName;
      TableName = col(*rowNr);
      *subtab = col.keywordSet().asTable(TableName);
    } catch (AipsError x) {
      cerr << "CalTableReader::GetColumnTable: " << x.getMesg() << endl;
      return false;
    }; 
    return true;
  }

// ==============================================================================
// Get the row number to the corresponding date
// ==============================================================================

int CalTableReader::GetDateRow(int const AntID,
			       Table const colTable,
			       uInt const date,
			       const bool mindEmpty) {
  Vector<uInt> rowNrs;
  try { 
    rowNrs = colTable(colTable.col("AntID")==AntID && colTable.col("StartDate")<=(double)date && 
		      colTable.col("StopDate")>(double)date).rowNumbers(colTable);
  } catch (AipsError x) {
    cerr << "CalTableReader::GetDateRow: " << x.getMesg() << endl;
    return -1;
  }; 
  if ( rowNrs.nelements() == 1) {
    return rowNrs(0);
  } else if ( rowNrs.nelements() < 1) {
    if (mindEmpty) {
            cerr << "CalTableReader::GetDateRow: " << "Cannot find date in table" << endl;
    };
    return -1 ;
  };
  cerr << "CalTableReader::GetDateRow: " << "colTable inconsistent: more than one fitting row!" << endl;
  return -2;
}

int CalTableReader::GetJuniorDateRow(int const AntID,
				     Table const colTable,
				     uInt const date,
				     String MaskField,
				     const bool mindEmpty) {
   Vector<uInt> rowNrs;
   try { 
    rowNrs = colTable(colTable.col("AntID")==AntID && colTable.col("StartDate")<=(double)date && 
		      colTable.col(MaskField)).sort("StartDate").rowNumbers(colTable);
  } catch (AipsError x) {
    cerr << "CalTableReader::GetJuniorDateRow: " << x.getMesg() << endl;
    return -1;
  }; 
  if ( rowNrs.nelements() >= 1) {
    return rowNrs(rowNrs.nelements()-1);
  } else if ( rowNrs.nelements() < 1) {
    if (mindEmpty) {
      cerr << "CalTableReader::GetJuniorDateRow: " << "Cannot find date in table" << endl;
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

bool CalTableReader::GetData(uInt const date,
			     int const AntID,
			     String const FieldName,
			     String *result) {
  try {
    int rowNr;
    Table subtab;
    String DataType;
    bool isJunior;
    if (!GetColumnTable(AntID, FieldName, &subtab, &rowNr, &DataType, &isJunior)) {
      cerr << "CalTableReader::GetData " << "Failed to get the ColumnTable" << endl;
      return false;
    };
    // check the data type
    if (DataType != "String") {
      cerr << "CalTableReader::GetData " << "(String)<-->" << FieldName << ":Incompatible data type" << endl;
      return false;
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
    return false;
  }; 
  return true;
}

bool CalTableReader::GetData(uInt const date,
			     int const AntID,
			     String const FieldName,
			     double *result) {
  try {
    int rowNr;
    Table subtab;
    String DataType;
    bool isJunior;
    if (!GetColumnTable(AntID, FieldName, &subtab, &rowNr, &DataType, &isJunior)) {
      cerr << "CalTableReader::GetData " << "Failed to get the ColumnTable" << endl;
      return false;
    };
    // check the data type
    if (DataType != "double") {
      cerr << "CalTableReader::GetData " << "(double)<-->" << FieldName << ":Incompatible data type" << endl;
      return false;
    };
    if (isJunior) {
      rowNr = GetJuniorDateRow(AntID, subtab, date, (FieldName+"-mask"));
    } else {
    rowNr = GetDateRow(AntID, subtab, date);
    };
    if (rowNr >= 0) {
      *result = ROScalarColumn<double>(subtab,FieldName)(rowNr);
    } else {
      cerr << "CalTableReader::GetData: " << "Date not found!"  << endl;
    };
  } catch (AipsError x) {
    cerr << "CalTableReader::GetData: " << x.getMesg() << endl;
    return false;
  }; 
  return true;
}

bool CalTableReader::GetData(uInt const date,
			     int const AntID,
			     String const FieldName,
			     DComplex *result) {
  try {
    int rowNr;
    Table subtab;
    String DataType;
    bool isJunior;
    if (!GetColumnTable(AntID, FieldName, &subtab, &rowNr, &DataType, &isJunior)) {
      cerr << "CalTableReader::GetData " << "Failed to get the ColumnTable" << endl;
      return false;
    };
    // check the data type
    if (DataType != "DComplex") {
      cerr << "CalTableReader::GetData " << "(DComplex)<-->" << FieldName << ":Incompatible data type" << endl;
      return false;
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
    return false;
  }; 
  return true;
}

bool CalTableReader::GetData(uInt const date,
			     int const AntID,
			     String const FieldName,
			     Array<double> *result) {
  try {
    int rowNr;
    Table subtab;
    String DataType;
    bool isJunior;
    if (!GetColumnTable(AntID, FieldName, &subtab, &rowNr, &DataType, &isJunior)) {
      cerr << "CalTableReader::GetData " << "Failed to get the ColumnTable" << endl;
      return false;
    };
    // check the data type
    if (DataType != "Array<double>") {
      cerr << "CalTableReader::GetData " << "(Array<double>)<-->" << FieldName << ":Incompatible data type" << endl;
      return false;
    };
    if (isJunior) {
      rowNr = GetJuniorDateRow(AntID, subtab, date, (FieldName+"-mask"));
    } else {
      rowNr = GetDateRow(AntID, subtab, date);
    };
    if (rowNr >= 0) {
      result->reference(ROArrayColumn<double>(subtab,FieldName)(rowNr));
    } else {
      cerr << "CalTableReader::GetData: " << "Date not found!"  << endl;
    };
  } catch (AipsError x) {
    cerr << "CalTableReader::GetData: " << x.getMesg() << endl;
    return false;
  }; 
  return true;
}

bool CalTableReader::GetData(uInt const date,
			     int const AntID,
			     String const FieldName,
			     Array<DComplex> *result) {
  try {
    int rowNr;
    Table subtab;
    String DataType;
    bool isJunior;
    if (!GetColumnTable(AntID, FieldName, &subtab, &rowNr, &DataType, &isJunior)) {
      cerr << "CalTableReader::GetData " << "Failed to get the ColumnTable" << endl;
      return false;
    };
    // check the data type
    if (DataType != "Array<DComplex>") {
      cerr << "CalTableReader::GetData " << "(Array<DComplex>)<-->" << FieldName << ":Incompatible data type" << endl;
      return false;
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
    return false;
  }; 
  return true;
};


// ==============================================================================
// Has the value of this field changed between the two given dates?
// ==============================================================================

bool CalTableReader::isIdentical(uInt const date1,
				 uInt const date2,
				 int const AntID, 
				 String const FieldName) {
  try {  
    int rowNr;
    Table subtab;
    bool isJunior;
    //if (date1 == date2) { return true; } // Trivial case. Does not check whether date is valid.
    if (!GetColumnTable(AntID, FieldName, &subtab, &rowNr, NULL, &isJunior)) {
      cerr << "CalTableReader::isIdentical " << "Failed to get the ColumnTable" << endl;
      return false;
    };
    uInt row1,row2;
    if (isJunior) {
      row1 = GetJuniorDateRow(AntID, subtab, date1, (FieldName+"-mask"), false);
      row2 = GetJuniorDateRow(AntID, subtab, date2, (FieldName+"-mask"), false);
    } else {
      row1 = GetDateRow(AntID, subtab, date1, false);
      row2 = GetDateRow(AntID, subtab, date2, false);
    };
    if ( row1==row2 && row1>=0) return true;
  } catch (AipsError x) {
    cerr << "CalTableReader::isIdentical: " << x.getMesg() << endl;
    return false;
  }; 
  return false;
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


bool CalTableReader::PrintSummary() {
  cout << *masterTable_p << endl;
  return true;
}

// ==============================================================================
// Implementation of the GetKeyword methods
// (Unfortunately every type needs its own implementation)
// ==============================================================================
bool CalTableReader::GetKeyword(String const KeywordName, String *result){
  try {  
    if (!masterTable_p->keywordSet().isDefined(KeywordName)) {
      cerr << "CalTableReader::GetKeyword: Keyword named \"" << KeywordName << 
	"\" does not exist" << endl;
      return false;
    };
    *result = masterTable_p->keywordSet().asString(KeywordName);
  } catch (AipsError x) {
    cerr << "CalTableReader::GetKeyword: " << x.getMesg() << endl;
    return false;
  }; 
  return true;
}
bool CalTableReader::GetKeyword(String const KeywordName, double *result){
  try {  
    if (!masterTable_p->keywordSet().isDefined(KeywordName)) {
      cerr << "CalTableReader::GetKeyword: Keyword named \"" << KeywordName << 
	"\" does not exist" << endl;
      return false;
    };
    *result = masterTable_p->keywordSet().asdouble(KeywordName);
  } catch (AipsError x) {
    cerr << "CalTableReader::GetKeyword: " << x.getMesg() << endl;
    return false;
  }; 
  return true;
}
bool CalTableReader::GetKeyword(String const KeywordName, DComplex *result){
  try {  
    if (!masterTable_p->keywordSet().isDefined(KeywordName)) {
      cerr << "CalTableReader::GetKeyword: Keyword named \"" << KeywordName << 
	"\" does not exist" << endl;
      return false;
    };
    *result = masterTable_p->keywordSet().asDComplex(KeywordName);
  } catch (AipsError x) {
    cerr << "CalTableReader::GetKeyword: " << x.getMesg() << endl;
    return false;
  }; 
  return true;
}
bool CalTableReader::GetKeyword(String const KeywordName, Array<double> *result){
  try {  
    if (!masterTable_p->keywordSet().isDefined(KeywordName)) {
      cerr << "CalTableReader::GetKeyword: Keyword named \"" << KeywordName << 
	"\" does not exist" << endl;
      return false;
    };
    result->reference(masterTable_p->keywordSet().asArraydouble(KeywordName));
  } catch (AipsError x) {
    cerr << "CalTableReader::GetKeyword: " << x.getMesg() << endl;
    return false;
  }; 
  return true;
}
bool CalTableReader::GetKeyword(String const KeywordName, Array<DComplex> *result){
  try {  
    if (!masterTable_p->keywordSet().isDefined(KeywordName)) {
      cerr << "CalTableReader::GetKeyword: Keyword named \"" << KeywordName << 
	"\" does not exist" << endl;
      return false;
    };
    result->reference(masterTable_p->keywordSet().asArrayDComplex(KeywordName));
  } catch (AipsError x) {
    cerr << "CalTableReader::GetKeyword: " << x.getMesg() << endl;
    return false;
  }; 
  return true;
}


// ==============================================================================
// Get the type of the given keyword
// ==============================================================================

String CalTableReader::GetKeywordType(String const KeywordName){
  try {  
    if (!masterTable_p->keywordSet().isDefined(KeywordName)) {
      cerr << "CalTableReader::GetKeywordType: Keyword named \""
	   << KeywordName
	   << "\" does not exist" << endl;
      return "";
    };
    casa::DataType type = masterTable_p->keywordSet().dataType(KeywordName);
    switch(type) {
    case casa::TpString:
      return "String";
    case casa::TpDouble:
      return "Double";
    case casa::TpDComplex:
      return "DComplex";
    case casa::TpArrayDouble:
      return "Array<Double>";
    case casa::TpArrayDComplex:
      return "Array<DComplex>";
    case casa::TpInt:
      return "Int";
    case casa::TpUInt:
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
