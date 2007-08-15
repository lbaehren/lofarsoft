//# GlishRecord.cc: Wrapper for a Glish record 
//# Copyright (C) 1994,1995,1996,1997,1998,2000,2001,2002,2003
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
//# $Id: GlishRecord.cc,v 19.4 2004/11/30 17:51:10 ddebonis Exp $

#include <Glish/Value.h>

//# AIPS++ and Glish both have a List class.
#if defined(List)
#undef List
#endif

#include <ApplicationSupport/GlishRecord.h>
#include <ApplicationSupport/GlishArray.h>
#include <casa/BasicSL/String.h>
#include <casa/Exceptions/Error.h>
#include <casa/Utilities/Assert.h>

#include <casa/Containers/RecordDesc.h>
#include <casa/Containers/RecordField.h>
#include <casa/Containers/Record.h>
#include <tables/Tables/TableRecord.h>
#include <tables/Tables/TableKeyword.h>

namespace casa { //# NAMESPACE CASA - BEGIN

GlishRecord::GlishRecord() : GlishValue()
{
    // It default's to false in GlishValue ctor. Over-ride with an empty
    // Record.
    Unref(value_p);
    value_p = create_record();
    DebugAssert(ok(), AipsError);
}

GlishRecord::GlishRecord(const GlishRecord &other) : GlishValue(other)
{
    // Nothing
    DebugAssert(ok(), AipsError);
}

GlishRecord::GlishRecord(const GlishValue &other) : GlishValue(other)
{
    if (type() != RECORD) {
	throw(AipsError("GlishRecord::GlishRecord(const GlishValue &other) - "
			"non-record type"));
    }
    DebugAssert(ok(), AipsError);
}

GlishRecord::~GlishRecord()
{
    // Nothing
}

void GlishRecord::fromRecord(const RecordInterface &record)
{
    // First wipe out the existing stuff
    GlishRecord tmp;
    *this = tmp;

    // Then copy over the record field by field
    RecordDesc desc = record.description();
    uInt nf = desc.nfields();
    // We could eliminate code by using template functions.
    for (uInt i=0; i<nf; i++) {
      switch(desc.type(i)) {
      case TpRecord:
	{
	  GlishRecord tmp;
	  // Recursively descend into sub-records.
	  // Use the correct type.
	  const TableRecord* trecp = dynamic_cast<const TableRecord*>(&record);
	  if (trecp != 0) {
	    RORecordFieldPtr<TableRecord> field (*trecp, i);
	    tmp.fromRecord(*field);
	  } else {
	    const Record* recp = dynamic_cast<const Record*>(&record);
	    if (recp != 0) {
	      RORecordFieldPtr<Record> field (*recp, i);
	      tmp.fromRecord(*field);
	    } else {
	      Record rec(record);
	      RORecordFieldPtr<Record> field (rec, i);
	      tmp.fromRecord(*field);
	    }
	  }
	  this->add(desc.name(i), tmp);
	}
	break;
      case TpBool:
	{
	  RORecordFieldPtr<Bool> field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpUChar:
	{
	  RORecordFieldPtr<uChar> field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpShort:
	{
	  RORecordFieldPtr<Short> field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpInt:
	{
	  RORecordFieldPtr<Int> field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpUInt:
	{
	  RORecordFieldPtr<uInt> field(record, i);
	  this->add(desc.name(i), Int(*field));
	}
	break;
      case TpFloat:
	{
	  RORecordFieldPtr<Float> field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpDouble:
	{
	  RORecordFieldPtr<Double> field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpComplex:
	{
	  RORecordFieldPtr<Complex> field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpDComplex:
	{
	  RORecordFieldPtr<DComplex> field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpString:
	{
	  RORecordFieldPtr<String> field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpArrayBool:
	{
	  RORecordFieldPtr<Array<Bool> > field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpArrayUChar:
	{
	  RORecordFieldPtr<Array<uChar> > field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpArrayShort:
	{
	  RORecordFieldPtr<Array<Short> > field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpArrayInt:
	{
	  RORecordFieldPtr<Array<Int> > field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpArrayFloat:
	{
	  RORecordFieldPtr<Array<Float> > field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpArrayDouble:
	{
	  RORecordFieldPtr<Array<Double> > field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpArrayComplex:
	{
	  RORecordFieldPtr<Array<Complex> > field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpArrayDComplex:
	{
	  RORecordFieldPtr<Array<DComplex> > field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpArrayString:
	{
	  RORecordFieldPtr<Array<String> > field(record, i);
	  this->add(desc.name(i), *field);
	}
	break;
      case TpTable:
	// If it's a table, return the table's name as is done for
	// keywords. NOTE: It's assumed that record is actually a TableRecord.
	{ const TableKeyword *kw = 
			(const TableKeyword*) record.get_pointer(i, TpTable);
	  String tname = kw->tableName();
	  String name = "Table: " + tname;
	  this->add(desc.name(i), name);
	}
	break;
      default:
	  throw(AipsError("GlishRecord::fromRecord - unrecognized type"));
      }
    }
}

RecordDesc GlishRecord::description (Bool recursive) const
{
  RecordDesc desc;
  for (uInt i=0; i < this->nelements(); i++) {
      GlishValue value = this->get(i);
      Bool isScalar = (value.nelements() == 1 &&
			     !value.attributeExists("shape"));
      if (value.type() == GlishValue::RECORD) {
	// Recursive or not!
	if (recursive) {
	  GlishRecord tmp = this->get(i);
	  desc.addField(this->name(i), tmp.description(recursive));
	} else {
	  desc.addField(this->name(i), RecordDesc());
	}
      } else {
	GlishArray val = value;
	switch (val.elementType()) {
	case GlishArray::BOOL: 
	  if (isScalar) {
	    desc.addField(this->name(i), TpBool);
	  } else {
	    desc.addField(this->name(i), TpArrayBool);
	  }
	  break;
	case GlishArray::BYTE:
	  if (isScalar) {
	    desc.addField(this->name(i), TpUChar);
	  } else {
	    desc.addField(this->name(i), TpArrayUChar);
	  }
	  break;
	case GlishArray::SHORT:
	  if (isScalar) {
	    desc.addField(this->name(i), TpShort);
	  } else {
	    desc.addField(this->name(i), TpArrayShort);
	  }
	  break;
	case GlishArray::INT:
	  if (isScalar) {
	    desc.addField(this->name(i), TpInt);
	  } else {
	    desc.addField(this->name(i), TpArrayInt);
	  }
	  break;
	case GlishArray::FLOAT:
	  if (isScalar) {
	    desc.addField(this->name(i), TpFloat);
	  } else {
	    desc.addField(this->name(i), TpArrayFloat);
	  }
	  break;
	case GlishArray::DOUBLE:
	  if (isScalar) {
	    desc.addField(this->name(i), TpDouble);
	  } else {
	    desc.addField(this->name(i), TpArrayDouble);
	  }
	  break;
	case GlishArray::COMPLEX:
	  if (isScalar) {
	    desc.addField(this->name(i), TpComplex);
	  } else {
	    desc.addField(this->name(i), TpArrayComplex);
	  }
	  break;
	case GlishArray::DCOMPLEX:
	  if (isScalar) {
	    desc.addField(this->name(i), TpDComplex);
	  } else {
	    desc.addField(this->name(i), TpArrayDComplex);
	  }
	  break;
	case GlishArray::STRING:
	  if (isScalar) {
	    desc.addField(this->name(i), TpString);
	  } else {
	    desc.addField(this->name(i), TpArrayString);
	  }
	  break;
	default:
	  throw(AipsError("GlishRecord::description - unrecognized type"));
	};
      }
  }
  return desc;
}

void GlishRecord::toRecord(RecordInterface &record) const
{
  // Use a temporary Record, because the passed in record could be
  // a TableRecord object which would give problems in toRecordCopy.
  Record tmp;
  toRecord (tmp);
  record.assign (tmp);
}

void GlishRecord::toRecord(Record &record) const
{
  record.restructure (description(), False);
  toRecordCopy (record);
}

void GlishRecord::toRecordCopy(Record &record) const
{
  for (uInt i=0; i < this->nelements(); i++) {
    GlishValue value = this->get(i);
    Bool isScalar = (value.nelements() == 1 &&
			   !value.attributeExists("shape"));
    if (value.type() == RECORD) {
      RecordFieldPtr<Record> field(record,i);
      GlishRecord glishrec = value;
      glishrec.toRecord(*field);
    } else {
      GlishArray val = value;
      switch (val.elementType()) {
      case GlishArray::BOOL: 
	if (isScalar) {
	  RecordFieldPtr<Bool> field(record, i);
	  val.get(*field);
	} else {
	  RecordFieldPtr<Array<Bool> > field(record, i);
	  (*field).resize(val.shape());
	  val.get(*field);
	}
	break;
      case GlishArray::BYTE: 
	if (isScalar) {
	  RecordFieldPtr<uChar> field(record, i);
	  val.get(*field);
	} else {
	  RecordFieldPtr<Array<uChar> > field(record, i);
	  (*field).resize(val.shape());
	  val.get(*field);
	}
	break;
      case GlishArray::SHORT: 
	if (isScalar) {
	  RecordFieldPtr<Short> field(record, i);
	  val.get(*field);
	} else {
	  RecordFieldPtr<Array<Short> > field(record, i);
	  (*field).resize(val.shape());
	  val.get(*field);
	}
	break;
      case GlishArray::INT: 
	if (isScalar) {
	  RecordFieldPtr<Int> field(record, i);
	  val.get(*field);
	} else {
	  RecordFieldPtr<Array<Int> > field(record, i);
	  (*field).resize(val.shape());
	  val.get(*field);
	}
	break;
      case GlishArray::FLOAT: 
	if (isScalar) {
	  RecordFieldPtr<Float> field(record, i);
	  val.get(*field);
	} else {
	  RecordFieldPtr<Array<Float> > field(record, i);
	  (*field).resize(val.shape());
	  val.get(*field);
	}
	break;
      case GlishArray::DOUBLE: 
	if (isScalar) {
	  RecordFieldPtr<Double> field(record, i);
	  val.get(*field);
	} else {
	  RecordFieldPtr<Array<Double> > field(record, i);
	  (*field).resize(val.shape());
	  val.get(*field);
	}
	break;
      case GlishArray::COMPLEX: 
	if (isScalar) {
	  RecordFieldPtr<Complex> field(record, i);
	  val.get(*field);
	} else {
	  RecordFieldPtr<Array<Complex> > field(record, i);
	  (*field).resize(val.shape());
	  val.get(*field);
	}
	break;
      case GlishArray::DCOMPLEX: 
	if (isScalar) {
	  RecordFieldPtr<DComplex> field(record, i);
	  val.get(*field);
	} else {
	  RecordFieldPtr<Array<DComplex> > field(record, i);
	  (*field).resize(val.shape());
	  val.get(*field);
	}
	break;
      case GlishArray::STRING: 
	if (isScalar) {
	  RecordFieldPtr<String> field(record, i);
	  val.get(*field);
	} else {
	  RecordFieldPtr<Array<String> > field(record, i);
	  (*field).resize(val.shape());
	  val.get(*field);
	}
	break;
      default:
	  throw(AipsError("GlishRecord::toRecord - unrecognized type"));
      };
    }
  }
}

GlishRecord &GlishRecord::operator=(const GlishRecord &other)
{
    DebugAssert(ok(), AipsError);
    if (this == &other)
	return *this;

    // Base-class operator= suffices.
    GlishValue &This = *this;
    This = other;
    return *this;
}

GlishRecord &GlishRecord::operator=(const GlishValue &other)
{
    DebugAssert(ok(), AipsError);
    if (this == &other)
	return *this;

    if (other.type() != RECORD) {
	throw(AipsError("GlishRecord::operator=(const GlishValue &other) - "
			"non-record type in other"));
    }

    GlishValue &This = *this;
    This = other;
    return *this;
}

GlishRecord& GlishRecord::add(const String &name, const GlishValue &value)
{
    DebugAssert(ok(), AipsError);
    // Copy-on-write semantics.
    value_p = value_p->CopyUnref();
    value_p->SetField(name.chars(), value.value_p);
    return *this;
}

GlishRecord& GlishRecord::add(const String &name, const GlishArray &value)
{
    DebugAssert(ok(), AipsError);
    return add(name, (const GlishValue &)value);
}

GlishRecord& GlishRecord::add(const char *name, const char *value)
{
    return add(String(name), String(value));
}

Bool GlishRecord::exists(const String &name) const
{
    DebugAssert(ok(), AipsError);
    // AIPS++ and Glish both have a Field, at least until old Tables go away.

    return ((value_p->Field(name.chars())) != 0);

}

GlishValue GlishRecord::get(const String &name) const
{
    DebugAssert(ok(), AipsError);
    if (! exists(name)) {
	return GlishValue();
    }

    return GlishValue(

    // AIPS++ and Glish both have a Field, at least until old Tables go away.
	value_p->Field(name.chars()), 

	True);
}

GlishValue GlishRecord::get(uInt fieldNumber) const
{
    DebugAssert(ok(), AipsError);
    if (fieldNumber >= nelements()) {
	throw(AipsError("GlishRecord::get(uInt fieldNumber) const - "
			"fieldNumber too large"));
    }

    return GlishValue(value_p->NthField(fieldNumber+1));
}

String GlishRecord::name(uInt fieldNumber) const
{
    DebugAssert(ok(), AipsError);
    return String(value_p->NthFieldName(fieldNumber+1));
}

Bool GlishRecord::ok() const
{
    return (GlishValue::ok() && type() == RECORD);
}

} //# NAMESPACE CASA - END

