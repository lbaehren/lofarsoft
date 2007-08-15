//# GlishRecord.h: Glish record interface
//# Copyright (C) 1994,1995,1996,1997,1998,2003
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
//# $Id: GlishRecord.h,v 19.4 2004/11/30 17:51:10 ddebonis Exp $

#ifndef TASKING_GLISHRECORD_H
#define TASKING_GLISHRECORD_H

#include <casa/aips.h>
#include <ApplicationSupport/GlishValue.h>

namespace casa { //# NAMESPACE CASA - BEGIN
  
  class RecordDesc;
  class RecordInterface;
  class Record;
  class GlishArray;
  class String;
  
  /*!
    \class GlishRecord
    
    \ingroup ApplicationSupport
    
    \brief holds a Glish record

    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>GlishValue
      <li>GlishArray
    </ul>
    
    <h3>Synopsis</h3>
    
    A GlishRecord consists of a heterogeneous set of named GlishValues.
    The named GlishValue may in turn be a GlishRecord, so in this way 
    the data structure is hierarchical.
    
    A GlishRecord may be converted to and from an AIPS++ Record object.

    <h3>Example(s)</h3>
    
    The following example creates a GlishRecord with two fields, and
    then retrieves a field.
    \code
    Vector<Complex> vis;  ...                                     // 1
    GlishRecord record;                                           // 2
    record.add("hello", 3.0)     ;                                // 3
    record.add("world", vis);                                     // 4
    ...
    GlishArray tmp;                                               // 5
    if (!record.exists("world")) {...}                            // 6
    tmp = record.get("world");                                    // 7
    Vector<DComplex> vis2(tmp.nelements();                        // 8
    if (!tmp.get(vis2)) { ... }                                   // 9
    ... use vis2 ...
    \endcode
  */  
  
  // <example>
// <ol>
//    <li> Declare a Vector<Complex> containing some values.
//    <li> Declare a (record); initially empty.
//    <li> Add a field named "hello" containing the value 3.0.
//    <li> Add a field named "world" containing the Vector vis.
//    <li> Declare a GlishArray temporary value.
//    <li> See if a field named "world" exists in record (if not, an error).
//    <li> Get the field.
//    <li> Declare a Vector to hold the result, note that we are getting it
//           out as DComplex rather than Complex (for some reason).
//    <li> Get the values out into a Vector. If this fails for some reason
//           do some error handling.
// </ol>
// </example>
//
// <todo asof="1996/01/24">
//    <item> Put GlishRecord under a common base class so all record-like
//           classes have a similar interface.
// </todo>

class GlishRecord : public GlishValue
{
public: 
    // Null Glish record (no fields).
    GlishRecord();

    // Logically make this object a copy of other. Physically avoids the copy
    // for as long as possible (copy on write). 
    // <group>
    GlishRecord(const GlishRecord &other);
    //If other isn't a record, an exception (<src>AipsError</src>) is thrown.
    GlishRecord(const GlishValue &other);
    // </group>

    ~GlishRecord();

    // Describe the layout of this object.
    // By default it is not done recursively (thus not for subrecords).
    RecordDesc description (Bool recursive=False) const;

    // Interconvert a GlishRecord and a Record. The converted object is
    // zero'd first, i.e. any existing fields are wiped out before copying
    // the fields.
    // 
    // These functions could in principle be global functions instead of member
    // functions, but it seemed likely that users would look for this 
    // functionality here. But they can be moved if this seems best to others.
    // The present implementation isn't particularly fast.
    // <group>
    void fromRecord(const RecordInterface &record);
    void toRecord(RecordInterface &record) const;
    void toRecord(Record &record) const;
    // </group>

    // Logically make this object a copy of other. Physically avoids the copy
    // for as long as possible (copy on write).
    // <group>
    GlishRecord &operator=(const GlishRecord &other);
    //If other isn't a record, an exception (<src>AipsError</src>) is thrown.
    GlishRecord &operator=(const GlishValue &other);
    // </group>

    // Add field "name" with the supplied value to this GlishRecord. If one
    // already exists, it is overwritten. These functions return a reference
    // to the glish record itself so that you can write things like:
    // <src>rec.add("a", 1).add("b", 2);</src>
    // <group>
    //# Common case not found with language rules (two conversions needed)
    GlishRecord& add(const char *name, const char *value);
    GlishRecord& add(const String &name, const GlishValue &value);
    // This (overloaded) version was written so that automatic conversions
    // of scalars and AIPS++ arrays will take place, e.g.
    // <src>record.add("pi", 3.14159)</src>.
    GlishRecord& add(const String &name, const GlishArray &value);
    // </group>

    // Does a field with the given name exist?
    Bool exists(const String &name) const;

    // Get the field with the given name from this record. Returns False
    // is no field with that name exists.
    GlishValue get(const String &name) const;

    // Get the fieldNumber'th field from this record. fieldNumber ranges between
    // 0 and nelements() - 1.
    GlishValue get(uInt fieldNumber) const;

    // Get the name corresponding to the fieldNumber'th field in this record.
    String name(uInt fieldNumber) const;

    // Is this record consistent?
    Bool ok() const;

private:
    void toRecordCopy(Record &record) const;
};


} //# NAMESPACE CASA - END

#endif
