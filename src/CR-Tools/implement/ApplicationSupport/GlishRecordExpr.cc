//# GlishRecordExpr.cc: Filter GlishRecords using a TaQL expression
//# Copyright (C) 2000,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: GlishRecordExpr.cc,v 19.3 2004/11/30 17:51:10 ddebonis Exp $


#include <ApplicationSupport/GlishRecordExpr.h>
#include <ApplicationSupport/GlishRecord.h>
#include <ApplicationSupport/GlishArray.h>
#include <tables/Tables/ExprNode.h>
#include <tables/Tables/RecordGram.h>
#include <casa/Containers/Record.h>
#include <casa/Containers/RecordDesc.h>
#include <casa/Containers/Block.h>
#include <casa/Arrays/Array.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>


namespace casa { //# NAMESPACE CASA - BEGIN

GlishRecordExpr::GlishRecordExpr (const GlishRecord& proto, const String& expr)
: itsExpr    (0),
  itsRecord  (proto.description(True)),
  itsSubject (0)
{
  itsExpr = new TableExprNode (RecordGram::parse (itsRecord, expr));
}

GlishRecordExpr::GlishRecordExpr (const Record& proto,
				  const TableExprNode& expr)
: itsExpr    (0),
  itsRecord  (proto),
  itsSubject (0)
{
  itsExpr = new TableExprNode (expr);
}

GlishRecordExpr::GlishRecordExpr (const Record& protoDesc, const String& expr)
: itsExpr    (0),
  itsRecord  (protoDesc),
  itsSubject (0)
{
//  cout << "TableExprNode for" << expr << "|" << endl;
  itsExpr = new TableExprNode (RecordGram::parse (itsRecord, expr));
}

GlishRecordExpr::GlishRecordExpr (const GlishRecordExpr& that)
: itsExpr    (0),
  itsRecord  (that.itsRecord),
  itsSubject (that.itsSubject)
{
  if (that.itsExpr != 0) {
    itsExpr = new TableExprNode (*that.itsExpr);
  }
}

GlishRecordExpr::~GlishRecordExpr()
{
  delete itsExpr;
}

GlishRecordExpr& GlishRecordExpr::operator= (const GlishRecordExpr& that)
{
  if (this != &that) {
    delete itsExpr;
    itsExpr = 0;
    if (that.itsExpr != 0) {
      itsExpr = new TableExprNode (*that.itsExpr);
    }
    itsRecord  = that.itsRecord;
    itsSubject = that.itsSubject;
  }
  return *this;
}

Bool GlishRecordExpr::matches (const GlishRecord& record)
{
  // Evaluate the expression for this record.
  itsSubject = &record;
  Bool valb;
  // This class contains the functions to get the values.
  itsExpr->get (*this, valb);
  return valb;
}

GlishArray GlishRecordExpr::getValue (const Block<Int>& fieldNrs) const
{
  DebugAssert (fieldNrs.nelements() > 0, AipsError);
  uInt lastEntry = fieldNrs.nelements() - 1;
  uInt fldnr = fieldNrs[lastEntry];
  DebugAssert (fieldNrs[0] < Int(itsSubject->nelements()), AipsError);
  DebugAssert (itsSubject->name(fieldNrs[0]) == itsRecord.name(fieldNrs[0]),
	       AipsError);
  if (lastEntry == 0) {
    return itsSubject->get (fldnr);
  }
  GlishRecord rec (*itsSubject);
  for (uInt i=0; i<lastEntry; i++) {
    rec = GlishRecord (rec.get (fieldNrs[i]));
  }
  return rec.get (fldnr);
}


Bool GlishRecordExpr::getBool (const Block<Int>& fieldNrs) const
{
  Bool val;
  GlishArray value = getValue (fieldNrs);
  switch (value.elementType()) {
  case GlishArray::BOOL:
    value.get (val);
    break;
  default:
    throw (AipsError("GlishRecordExpr::getBool - mismatching data type"));
  }
  return val;
}

Double GlishRecordExpr::getDouble (const Block<Int>& fieldNrs) const
{
  Double val;
  GlishArray value = getValue (fieldNrs);
  switch (value.elementType()) {
  case GlishArray::BOOL:
  case GlishArray::COMPLEX:
  case GlishArray::DCOMPLEX:
  case GlishArray::STRING:
    throw (AipsError("GlishRecordExpr::getDouble - mismatching data type"));
  default:
    value.get (val);
  }
  return val;
}

DComplex GlishRecordExpr::getDComplex (const Block<Int>& fieldNrs) const
{
  DComplex val;
  GlishArray value = getValue (fieldNrs);
  switch (value.elementType()) {
  case GlishArray::BOOL:
  case GlishArray::STRING:
    throw (AipsError("GlishRecordExpr::getDComplex - mismatching data type"));
  default:
    value.get (val);
  }
  return val;
}

String GlishRecordExpr::getString (const Block<Int>& fieldNrs) const
{
  String val;
  GlishArray value = getValue (fieldNrs);
  switch (value.elementType()) {
  case GlishArray::STRING:
    value.get (val);
    break;
  default:
    throw (AipsError("GlishRecordExpr::getString - mismatching data type"));
  }
  return val;
}


Array<Bool> GlishRecordExpr::getArrayBool (const Block<Int>& fieldNrs) const
{
  Array<Bool> val;
  GlishArray value = getValue (fieldNrs);
  switch (value.elementType()) {
  case GlishArray::BOOL:
    value.get (val);
    break;
  default:
    throw (AipsError("GlishRecordExpr::getBool - mismatching data type"));
  }
  return val;
}

Array<Double> GlishRecordExpr::getArrayDouble
                                          (const Block<Int>& fieldNrs) const
{
  Array<Double> val;
  GlishArray value = getValue (fieldNrs);
  switch (value.elementType()) {
  case GlishArray::BOOL:
  case GlishArray::COMPLEX:
  case GlishArray::DCOMPLEX:
  case GlishArray::STRING:
    throw (AipsError("GlishRecordExpr::getDouble - mismatching data type"));
  default:
    value.get (val);
  }
  return val;
}

Array<DComplex> GlishRecordExpr::getArrayDComplex
                                          (const Block<Int>& fieldNrs) const
{
  Array<DComplex> val;
  GlishArray value = getValue (fieldNrs);
  switch (value.elementType()) {
  case GlishArray::BOOL:
  case GlishArray::STRING:
    throw (AipsError("GlishRecordExpr::getDComplex - mismatching data type"));
  default:
    value.get (val);
  }
  return val;
}

Array<String> GlishRecordExpr::getArrayString
                                          (const Block<Int>& fieldNrs) const
{
  Array<String> val;
  GlishArray value = getValue (fieldNrs);
  switch (value.elementType()) {
  case GlishArray::STRING:
    value.get (val);
    break;
  default:
    throw (AipsError("GlishRecordExpr::getString - mismatching data type"));
  }
  return val;
}


IPosition GlishRecordExpr::shape (const Block<Int>& fieldNrs) const
{
  return getValue(fieldNrs).shape();
}

DataType GlishRecordExpr::dataType (const Block<Int>& fieldNrs) const
{
  // An exception means that the field is undefined or so.
  // Return type TpOther in such a case.
  DataType dtype = TpOther;
  try {
    switch (getValue(fieldNrs).elementType()) {
    case GlishArray::BOOL:
      dtype = TpBool;
      break;
    case GlishArray::BYTE:
      dtype = TpUChar;
      break;
    case GlishArray::SHORT:
      dtype = TpShort;
      break;
    case GlishArray::INT:
      dtype = TpInt;
      break;
    case GlishArray::FLOAT:
      dtype = TpFloat;
      break;
    case GlishArray::DOUBLE:
      dtype = TpDouble;
      break;
    case GlishArray::COMPLEX:
      dtype = TpComplex;
      break;
    case GlishArray::DCOMPLEX:
      dtype = TpDComplex;
      break;
    case GlishArray::STRING:
      dtype = TpString;
      break;
    default:
      dtype = TpOther;
    }
  } catch (AipsError) {
  }
  return dtype;
}

} //# NAMESPACE CASA - END

