//# GlishArray.h: Convenient AIPS++ wrapper class for Glish array values.
//# Copyright (C) 1994,1995,1996,1997,1998
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
//# $Id$

#ifndef TASKING_GLISHARRAY_H
#define TASKING_GLISHARRAY_H

#include <casa/aips.h>

#include <ApplicationSupport/GlishValue.h>

namespace casa { //# NAMESPACE CASA - BEGIN
  
  class String;
  class IPosition;
  template<class T> class Array;
  
  /*!
    \class GlishArray
    
    \ingroup ApplicationSupport
    
    \brief holds any Glish array

    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>GlishValue
    </ul>
    
    <h3>Synopsis</h3>
    
    A GlishArray stores a (multidimensional) array of Glish values which are all
    of the same (scalar) type. It is also used for storing single values. (A scalar
    in Glish is just a 1-element Array).
    
    GlishArray values are polymorphic. For example, an array which was created
    as Float, may be retrieved as Complex. The only interconversion which is
    not allowed is String to and from any other type. Complex values are 
    converted to real types by dropping the imaginary part.
    
    <h3>Example(s)</h3>
    
  */
  
//
// <example>
// The following code fragment illustrates GlishArrays being interconverted
// between scalars and AIPS++ arrays.
// <srcblock>
//     GlishArray ga(3.0);                         // 1
//     Array<Complex> ac = getArrayComplex();      // 2
//     ga = ac;                                    // 3
//     DComplex val;                               // 4
//     if (! ga.get(val,2)) { ... }                // 5
//     if (! ga.get(ac)) { ... }                   // 6
//     
// </srcblock>
// <ol>
//    <li> Declares a GlishArray object, ga, containing the single value
//           3.0. This illustrates the creation of a GlishArray from a scalar.
//    <li> Get an Array<Complex> from some function.
//    <li> ga now contains the values in ac, and the shapes will also
//           be identical.
//    <li> Define a DComplex  variable, val.
//    <li> Read the third value from the GlishArray into variable val. Note
//           that it it will be converted to a DComplex even though the
//           underlying values are Complex. If the conversion couldn't
//           be done, the if statement (...) would be taken.
//   <li>  Read from the GlishArray into the Array<Complex> ac. If the
//           underlying type cannot be converted the get would fail and the
//           if would be taken.
// </ol>
// </example>
//
// <todo asof="1994/10/17">
//   <li> It would be even more efficient if a GlishArray could reference
//          the storage in an Array rather than copying it.
//   <li> Perhaps a member function to determine which types can
//          be polymorphed into which others?
//   <li> Should the "get" conformance rules be changed? Only use will
//          tell.
//   <li> Should handle the case where Glish types differ from native (e.g.
//          Glish Complex vs AIPS++ complex) more thoroughly. There are if's
//          in the code that should cause exceptions to be thrown if there is
//          ever an actual problem.
//   <li> GlishArray could be derived from Lattice.
// </todo>
//
  class GlishArray : public GlishValue
  {
  public: 
    /*!
      \brief Type of the array elements
    */
    enum ElementType {
      BOOL,
      BYTE,
      SHORT,
      INT,
      FLOAT,
      DOUBLE,
      COMPLEX,
      DCOMPLEX,
      STRING};
    
    /*!
      \brief Default constructor
    
      Make a False, length=1, array.
    */
    GlishArray();
    
    /*!
      \brief Make a logical copy of other
    
      Make a logical copy of other. A physical copy is only made when necessary
      (copy-on-write), so this is efficient.
    */
    GlishArray(const GlishArray &other);
    
    /*!
      Create a GlishArray from a GlishValue. If type() isn't ARRAY, then
      an exception (<src>AipsError</src>) is thrown.
    */
    GlishArray(const GlishValue &other);

    // Create a GlishArray by copying from "values". After creation, the
    // shape of the GlishArray will match "values.shape()".
    // <group>
    GlishArray(const Array<Bool> &values);
    GlishArray(const Array<uChar> &values);
    GlishArray(const Array<Short> &values);
    GlishArray(const Array<Int> &values);
    GlishArray(const Array<Float> &values);
    GlishArray(const Array<Double> &values);
    GlishArray(const Array<Complex> &values);
    GlishArray(const Array<DComplex> &values);
    GlishArray(const Array<String> &values);
    // </group>

    // Create a length one GlishArray containing "value".
    // <group>
    GlishArray(Bool value);
    GlishArray(uChar value);
    GlishArray(Short value);
    GlishArray(Int value);
    GlishArray(Float value);
    GlishArray(Double value);
    GlishArray(Complex value);
    GlishArray(DComplex value);
    GlishArray(const String &value);
    GlishArray(const Char *value);
    // </group>

    ~GlishArray();

    // Make a logical copy of other. A physical copy is only made when necessary
    // (copy-on-write), so this is efficient.
    // <group>
    GlishArray &operator=(const GlishArray &other);
    GlishArray &operator=(const GlishValue &other);
    // </group>

    /*!
      Since a GlishArray might be very large, "reset" has been provided as
      a way to reclaim its storage. After reset, the GlishArray consists
      of a one element Boolean containing False, i.e.  like the result of
      the default constructor.
    */
    void reset();

    // shape() returns the length of each axis, i.e.
    // <src>nelements() == shape.product();</src>
    IPosition shape() const;

    // Returns the element type of the GlishArray.
    ElementType elementType() const;

    // Get all the values as an AIPS++ Array. If "values.nelements()"  differs
    // from this.nelements(), an attempt is made to resize the array to
    // this.shape() if attemptResize is True.
    // <group>
    Bool get(Array<Bool> &values, Bool attemptResize=True) const;
    Bool get(Array<uChar> &values, Bool attemptResize=True) const;
    Bool get(Array<Short> &values, Bool attemptResize=True) const;
    Bool get(Array<Int> &values, Bool attemptResize=True) const;
    Bool get(Array<Float> &values, Bool attemptResize=True) const;
    Bool get(Array<Double> &values, Bool attemptResize=True) const;
    Bool get(Array<Complex> &values, Bool attemptResize=True) const;
    Bool get(Array<DComplex> &values, Bool attemptResize=True) const;
    Bool get(Array<String> &values, Bool attemptResize=True) const;
    // </group>

    // Get the <em>index</em>th element of the GlishArray. This should be 
    // particularly useful for GlishArray objects with only one element,
    // i.e. a <em>scalar</em>.
    // Note that unlike the native Glish classes, indexing is zero-relative.
    // <group>
    Bool get(Bool &val, uInt index = 0) const;
    Bool get(uChar &val, uInt index = 0) const;
    Bool get(Short &val, uInt index = 0) const;
    Bool get(Int &val, uInt index = 0) const;
    Bool get(Float &val, uInt index = 0) const;
    Bool get(Double &val, uInt index = 0) const;
    Bool get(Complex &val, uInt index = 0) const;
    Bool get(DComplex &val, uInt index = 0) const;
    Bool get(String &val, uInt index = 0) const;
    // </group>

    // Is this GlishArray object consistent?
    Bool ok() const;
};


} //# NAMESPACE CASA - END

#endif
