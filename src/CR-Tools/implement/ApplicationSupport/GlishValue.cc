//# GlishValue.cc: Wrapper for a Glish value (array or record)
//# Copyright (C) 1995,1994,1995,1996,1997,1998,1999,2000,2001,2003
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
//# $Id: GlishValue.cc,v 19.3 2004/11/30 17:51:11 ddebonis Exp $

#include <Glish/Value.h>
#include <ApplicationSupport/GlishArray.h>
#include <casa/Arrays/IPosition.h>
#include <ApplicationSupport/GlishRecord.h>

#include <casa/sstream.h>
#include <casa/iostream.h>
#include <casa/stdlib.h>

//# The following is to shut up multiple #definitions of List (AIPS++ and
//# Glish both have a List class, neither of which is needed).
#if defined(List)
#undef List
#endif

//# Native Glish member functions and functions tend to start in upper case.
//# copy_value being a notable exception.

#include <ApplicationSupport/GlishValue.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <throw>
//    <item> AllocError
// </thrown>
GlishValue::GlishValue()
{
    value_p = new Value(glish_false);
    if (value_p == 0) {
	throw(AllocError("GlishValue::GlishValue() - alloc of 1 element"
			 " Value fails", sizeof(Value)));
    }
    DebugAssert(ok(), AipsError);
}

GlishValue::GlishValue(const GlishValue &other) : value_p(other.value_p)
{
    Ref(value_p);
    DebugAssert(ok(), AipsError);
}

GlishValue::~GlishValue()
{
    Unref(value_p);
    DebugAssert(ok(), AipsError);
}

GlishValue &GlishValue::operator=(const GlishValue &other)
{
    DebugAssert(ok(), AipsError);
    if (this == &other) {
	return *this;
    }

    Unref(value_p);
    value_p = other.value_p;
    Ref(value_p);
    return *this;
}

void GlishValue::addAttribute(const String &name, const GlishValue &value)
{
    DebugAssert(ok(), AipsError);
    // We're changing this value, so implement copy-on-write
    value_p = value_p->CopyUnref();
    value_p->AssignAttribute(name.chars(), value.value_p);
}

Bool GlishValue::attributeExists(const String &name) const
{
    DebugAssert(ok(), AipsError);
    // false_value is a pointer defined in the Glish Value.h header.
    return (value_p->HasAttribute(name.chars()) == 0 ? 0 : 1);
}

GlishValue GlishValue::getAttribute(const String &name) const
{
    DebugAssert(ok(), AipsError);
    GlishValue val;
    if (attributeExists(name)) {
	val.copyset(value_p->ExistingAttribute(name.chars()));
    }
    return val;
}

// <throw>
//    <item> AipsError
// </thrown>
GlishValue::ValueType GlishValue::type() const
{
    // Don't check ok() to avoid recursion!

    switch(value_p->Type()) {

    case TYPE_BOOL:
    case TYPE_BYTE:
    case TYPE_SHORT:
    case TYPE_INT:
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_COMPLEX:
    case TYPE_DCOMPLEX:
    case TYPE_STRING:       // Fall through
	return GlishValue::ARRAY;
    case TYPE_RECORD:
	return GlishValue::RECORD;
    default:
        throw(AipsError("::glish_value_type(const Value *value)"
                        " - unknown Glish type"));
    }
    return GlishValue::ARRAY;           // to make compiler happy
}

uInt GlishValue::nelements() const
{
    // Avoid recursion - no ok()
    // AIPS++ has a Length class; avoid namespace collision.
    
    return value_p->Length();
    
}

String GlishValue::format(uInt maxArrayLength) const
{
    DebugAssert(ok(), AipsError);

    String returnval;

    uInt n = nelements();
    if (type() == RECORD) {
        GlishRecord rec(*this);
        returnval += '[';
	GlishValue tmp;
        for (uInt i=0; i < n; i++) {
	    returnval += rec.name(i);
	    returnval += "=";
	    tmp = rec.get(i);
	    returnval += tmp.format(maxArrayLength); // recurse
	    if (i != n-1) {
	        returnval += " ";
	    }
	}
	returnval += "]";
    } else {
	if (n <= maxArrayLength) {
	    charptr string_rep = value_p->StringVal();
	    returnval += string_rep;
	    free( (void*) string_rep );
	} else {
	    ostringstream buffer;
	    GlishArray tmp;
	    buffer << "[" << n << " element array, shaped ";
	    buffer << tmp.shape() << "]";
	    returnval += buffer.str();
	}
    }

    return returnval;
}

// <throw>
//    <item> AllocError
// </thrown>
void GlishValue::copyset(const Value *value)
{
    Unref(value_p);
    value_p = copy_value(value);
    if (value_p == 0) {
	throw(AllocError("void GlishValue::set(const Value *value) - "
			 " new of Value fails", sizeof(Value)));

    }
}

// <throw>
//    <item> AipsError
// </thrown>
GlishValue::GlishValue(Value *value, Bool copy) : value_p(0)
{
    if (copy) {
	value_p = copy_value(value);
    } else {
	value_p = value;
	if ( value_p ) Ref(value_p);
    }
    if (! value_p) {
	throw(AipsError("GlishValue::GlishValue(Value *value, Bool copy) - "
			"null pointer sent in, or allocation failed"));
    }
    DebugAssert(ok(), AipsError);
}

Bool GlishValue::ok() const
{
    return ((value_p != 0));
}

ostream &operator<<(ostream &os, const GlishValue &value)
{
    os << value.format();
    return os;
}


// Handle unset values the same as in unset.g.
Bool GlishValue::isUnsetPriv() const
{
    if (nelements() == 1) {
        GlishRecord rec(*this);
	if (rec.exists ("i_am_unset")) {
	    return True;
	}
    }
    return False;
}

const GlishRecord& GlishValue::getUnset()
{
  static GlishRecord* theUnset = 0;
  if (theUnset == 0) {
    theUnset = new GlishRecord();
    theUnset->add ("i_am_unset", "i_am_unset");
  }
  return *theUnset;
}

} //# NAMESPACE CASA - END

