//# GlishValue.h: Wrapper for a Glish value (array or record) 
//# Copyright (C) 1994,1995,1996,1998,1999,2000,2001
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
//# $Id: GlishValue.h,v 19.7 2006/12/22 05:33:29 gvandiep Exp $

#ifndef TASKING_GLISHVALUE_H
#define TASKING_GLISHVALUE_H

#include <casa/aips.h>

//# We'd like to forward declare, but Complex and DComplex are typedef's, not
//# the actual class name, and this implementation may change.
#include <casa/BasicSL/Complex.h>
#include <casa/iosfwd.h>

//# The following are "native" Glish classes.
class Value;

namespace casa { //# NAMESPACE CASA - BEGIN
  
  //# Forward Declarations
  class String;
  class GlishRecord;
  class GlishSysEventSource;
  class GlishSysEvent;
  
  /*!
    \class GlishValue
    
    \ingroup ApplicationSupport
    
    \brief holds any type of Glish value
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>It is probably helpful to understand Glish values in a general way.
      The whole point of this class though is to hide the details! This
      Glish User manual has a chapter on the Glish client library where
      the Glish Value class is described.
    </ul>
    
    <h3>Synopsis</h3>
    
    Normally, one of the derived classes (i.e. GlishArray and GlishRecord)
    will be used rather than directly using GlishValue. A GlishValue may
    be freely interconverted between a GlishArray or GlishValue via either
    assignment or the copy constructor.  Since copy-on-write semantics are
    used, these conversions are relatively inexpensive.
    
    <ul>
      <li>Copy-on-write means that when a copy or an assignment is
      made, a physical copy is not made, the representation is shared via
      a pointer. However when one of the copies is changed, a physical copy
      is first made. So, basically this technique avoids the physical copy
      for as long as possible, in the hope that it might never be needed.
    </ul>
    
    The major functionalities provided to end users through the GlishValue
    class are:
    <ol>
      <li> The ability to add or remove attributes (a named GlishValue) to
      this GlishValue.
      <li> The ability to get a formatted string representation of this 
      value (or use a Glish value with an iostream).
    </ol>
    
    Additionally, various inquiry functions are provided, for example to 
    determine whether this value is an array or a record, and to say how
    long it is.
    
    <h3>Example(s)</h3>
    
    See the examples in GlishArray and GlishRecord.
    
    <h3>Motivation</h3>
    
    While it is certainly possible to use the "native" Glish classes, these
    classes make it easier for those working in AIPS++ to interact with
    Glish, and these classes provide some independence. Also, the Glish way
    of doing things is different from how AIPS++ users are used to doing it
    (e.g. the memory management is more explicit).
    
    For the morbidly curious, here's an example of how one would turn
    an AIPS++ Array<Complex> into a native Glish Value object if these
    wrapper classes did not exist.
  */  
  
  // <srcblock>
  // Array<Complex ac = ...;
  // 
  // // Get the values from the AIPS++ array
  // Bool del;
  // Complex *aipsComplexPtr = ac.getStorage(del);
  // 
  // // Create a receptor array note that complex != Complex!!
  // complex *glishComplexPtr = new complex[ac.nelements()];
  // if ((glishComplexPtr) == 0) { ... memory error ...}
  // 
  // // Copy the values
  // for (uInt i=0; i < ac.nelements(); i++)
  //     glishComplexPtr[i] = complex(aipsComplexPtr[i].real(),
  // 				 aipsComplexPtr[i].imag());
  // 
  // // Delete pointer, if necessary.
  // ac.putStorage(aipsComplexPtr, del);
  // 				 
  // // OK; create the Glish Value from the initialized storage
  // Value native(ptr, ac.nelements());
  // 
  // // Now we need to attach the shape to the Glish Value as an attribute
  // IPosition acShape = ac.shape();
  // int shapeArray[ac.ndim()];
  // for (i=0; i < ac.ndim(); i++)
  //     shapeArray[i] = acShape(i);
  // 
  // Value *shapeValuePtr = new Value(shapeArray, ac.ndim(), COPY_ARRAY);
  // if (shapeValuePtr == 0) { ... memory error ...;}
  // native.AssignAttribute("shape", shapeValuePtr);
  // Unref(shapeValuePtr);
  // // Done! native now contains a copy of ac.
  // </srcblock>
  // </motivation>
  //
  // <todo asof="1994/10/17">
  //   <li> Delete attributes
  //   <li> Get all attribute names
  //   <li> Due to the way that Glish initializes itself, GlishValue isn't
  //          entirely reliable unless a GlishSysEvent source object also
  //          exists (e.g. false_value won't be defined). Since it isn't
  //          likely that one would want to use GlishValues without an event
  //          source this isn't too onerous, it should still be fixed (requires
  //          Glish client library changes). Getting the Glish reporters
  //          initialized is the other known problem.
  // </todo>
  
  class GlishValue
  {
  public: 
    friend class GlishRecord;
    friend class GlishSysEventSource;
    friend class GlishSysEvent;
    
    // A scalar is just a length=1 array in Glish.
    enum ValueType {ARRAY, RECORD};

    // The default constructor creates a length=1 array with the value of False.
    GlishValue();

    // This should *only* be used for clients written using the
    // <Glish/Client.h> interface which want to take advantage of
    // the AIPS++ wrappers. This references value.
    GlishValue(Value *value, Bool copy=False);

    // Create a logical copy of other. The physical copy is avoided for as
    // long as possible (copy-on-write semantics).
    GlishValue(const GlishValue &other);

    virtual ~GlishValue();

    // Create a logical copy of other. The physical copy is avoided for as
    // long as possible (copy-on-write semantics).
    GlishValue &operator=(const GlishValue &other);

    // Add an attribute (named GlishValue) to this Value. Note that attributes
    // can in turn have attributes (although this might not be very useful).
    // The only attribute that is systematically used by Glish presently (Oct
    // 94) is a "shape" which is an integer array containing the axis lengths.
    // It is automatically supplied however by the GlishArray class.
    void addAttribute(const String &name, const GlishValue &value);
    // Determine if attribute "name" exists.
    Bool attributeExists(const String &name) const;
    // Return attribute "name." Returns the value False if "name" does not
    // exist.
    GlishValue getAttribute(const String &name) const;

    // Is this object an array or a record?
    ValueType type() const;

    // The number of elements in an array (1 for a scalar), or the number of
    // fields in a record.
    uInt nelements() const;

    // Returns a String representation of this GlishValue object. The iostream
    // output operator is also defined. By default only print the first 10
    // elements of an array.
    String format(uInt maxArrayLength=10) const;

    // Is this GlishValue consistent?
    virtual Bool ok() const;

    // The following might be useful for interfacing to native Glish classes
    // occasionally - however it was mostly put in to allow testing of whether
    // copy-on-write was working.
    const Value *value() const;

    // Is the glish value the unset value?
    Bool isUnset() const;

    // Get the unset value;
    static const GlishRecord& getUnset();

protected:
    Value *value_p;
    // Makes this a copy of value
    void copyset(const Value *value);

private:
    Bool isUnsetPriv() const;
};


inline const Value *GlishValue::value() const
{
    return value_p;
}

inline Bool GlishValue::isUnset() const
{
    return (type() == GlishValue::RECORD  ?  isUnsetPriv() : False);
}


// <summary>output GlishValue to an ostream (in ASCII)</summary>
// <use visibility=export>
// <group name='operator&lt;&lt;'>
ostream &operator<<(ostream &os, const GlishValue &value);
// </group>



} //# NAMESPACE CASA - END

#endif
