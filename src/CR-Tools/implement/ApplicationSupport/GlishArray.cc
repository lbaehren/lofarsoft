//# GlishArray.cc: Convenient AIPS++ wrapper class for Glish array values. 
//# Copyright (C) 1995,1994,1995,1996,1997,1998,1999,2000,2001
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
//# $Id: GlishArray.cc,v 19.4 2004/11/30 17:51:10 ddebonis Exp $

#include <Glish/Value.h>

//# The following is to shupt up multiple #definitions of List
#if defined(List)
#undef List
#endif

#include <casa/BasicSL/String.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Exceptions/Error.h>
#include <casa/Utilities/Assert.h>
#include <casa/Arrays/ArrayPosIter.h>
#include <casa/stdlib.h>

#include <ApplicationSupport/GlishArray.h>

namespace casa { //# NAMESPACE CASA - BEGIN

GlishArray::GlishArray() : GlishValue()
{
    // Nothing
    DebugAssert(ok(), AipsError);
}

GlishArray::GlishArray(const GlishArray &other) : GlishValue(other)
{
    // Nothing
    DebugAssert(ok(), AipsError);
}

GlishArray::GlishArray(const GlishValue &other) : GlishValue(other)
{
    if (type() == GlishValue::RECORD) {
	throw(AipsError("GlishArray::GlishArray(const GlishValue &other) - "
			"construction from a RECORD value"));
    }
    DebugAssert(ok(), AipsError);
}

//# Hidden helper function
static void throw_alloc_error(uInt size)
{
    throw(AllocError("GlishArray:: - a new has failed", size));
}

static void *allocate(size_t size)
{
    // Force ourselves to always do an allocation even if the length
    // is zero.
    if (size < 1) {
        size = 1;
    }
    void *ptr = malloc(size);
    if (ptr == 0) {
        throw_alloc_error(size);
    }
    return ptr;
}

//# Hidden helper function
//# No initialization takes place
static Value *make_glish_value(glish_type type, Int length)
{
    if (length < 0) {
	throw(AipsError("make_bool_value(glish_type type, Int length) - "
			"length <= 0 - internal error"));
    }
    Value *value = 0;
    switch (type) {
    case TYPE_BOOL:
	{
	    glish_bool *ptr = (glish_bool*)allocate(sizeof(glish_bool)*length);
	    value = new Value(ptr, length);
	}
	break;
    case TYPE_BYTE:
	{
	    byte *ptr = (byte*) allocate(sizeof(byte)*length);
	    value = new Value(ptr, length);
	}
	break;
    case TYPE_SHORT:
	{
	    short *ptr = (short*)allocate(sizeof(short)*length);
	    value = new Value(ptr, length);
	}
	break;
    case TYPE_INT:
	{
	    int *ptr = (int*) allocate(sizeof(int)*length);
	    value = new Value(ptr, length);
	}
	break;
    case TYPE_FLOAT:
	{
	    float *ptr = (float*)allocate(sizeof(float)*length);
	    value = new Value(ptr, length);
	}
	break;
    case TYPE_DOUBLE:
	{
	    double *ptr = (double*) allocate(sizeof(double)*length);
	    value = new Value(ptr, length);
	}
	break;
    case TYPE_STRING:
	{
	    charptr *ptr = (charptr*) allocate(sizeof(charptr)*length);
	    for (Int i=0; i < length; i++) {
		if ((ptr[i] = strdup("")) == 0) {
		    throw_alloc_error(sizeof(""));
		}
	    }
	    value = new Value(ptr, length);
	}
	break;
    case TYPE_COMPLEX:
	{
	    glish_complex *ptr = (glish_complex*) allocate(sizeof(glish_complex)*length);
	    value = new Value(ptr, length);
	}
	break;
    case TYPE_DCOMPLEX:
	{
	    glish_dcomplex *ptr = (glish_dcomplex*)allocate(sizeof(glish_dcomplex)*length);
	    value = new Value(ptr, length);
	}
	break;
    default:
	throw(AipsError("make_glish_value(glish_type type, Int length) - "
			"unknown scalar type"));
    }
    if (value == 0) {
	throw_alloc_error(length*sizeof(Value));
    }
	
    return value;
}

// Hidden helper function
static void add_shape(Value *value, const IPosition &shape)
{
    Value *shape_attribute = make_glish_value(TYPE_BOOL, shape.nelements());
    int *shape_ptr = shape_attribute->IntPtr();
    for (uInt i=0; i < shape.nelements(); i++) {
	shape_ptr[i] = shape(i);
    }
    value->AssignAttribute("shape", shape_attribute);
    Unref(shape_attribute);
}

GlishArray::GlishArray(const Array<Bool> &values) : GlishValue()
{
    Unref(value_p);
    uInt length = values.nelements();
    value_p = make_glish_value(TYPE_BOOL, length);
    glish_bool *out_ptr = value_p->BoolPtr();
    Bool delete_it;
    const Bool *in_ptr = values.getStorage(delete_it);
    for (uInt i=0; i<length; i++) {
	out_ptr[i] = in_ptr[i] ? glish_true : glish_false;
    }
    values.freeStorage(in_ptr, delete_it);
    add_shape(value_p, values.shape());
    DebugAssert(ok(), AipsError);
}

GlishArray::GlishArray(const Array<uChar> &values) : GlishValue()
{
    Unref(value_p);
    uInt length = values.nelements();
    value_p = make_glish_value(TYPE_BYTE, length);
    byte *out_ptr = value_p->BytePtr();
    Bool delete_it;
    const uChar *in_ptr = values.getStorage(delete_it);
    for (uInt i=0; i<length; i++) {
	out_ptr[i] = in_ptr[i];
    }
    values.freeStorage(in_ptr, delete_it);
    add_shape(value_p, values.shape());
    DebugAssert(ok(), AipsError);
}

GlishArray::GlishArray(const Array<Short> &values) : GlishValue()
{
    Unref(value_p);
    uInt length = values.nelements();
    value_p = make_glish_value(TYPE_SHORT, length);
    short *out_ptr = value_p->ShortPtr();
    Bool delete_it;
    const Short *in_ptr = values.getStorage(delete_it);
    for (uInt i=0; i<length; i++) {
	out_ptr[i] = in_ptr[i];
    }
    values.freeStorage(in_ptr, delete_it);
    add_shape(value_p, values.shape());
    DebugAssert(ok(), AipsError);
}

GlishArray::GlishArray(const Array<Int> &values) : GlishValue()
{
    Unref(value_p);
    uInt length = values.nelements();
    value_p = make_glish_value(TYPE_INT, length);
    int *out_ptr = value_p->IntPtr();
    Bool delete_it;
    const Int *in_ptr = values.getStorage(delete_it);
    for (uInt i=0; i<length; i++) {
	out_ptr[i] = in_ptr[i];
    }
    values.freeStorage(in_ptr, delete_it);
    add_shape(value_p, values.shape());
    DebugAssert(ok(), AipsError);
}

GlishArray::GlishArray(const Array<Float> &values) : GlishValue()
{
    Unref(value_p);
    uInt length = values.nelements();
    value_p = make_glish_value(TYPE_FLOAT, length);
    float *out_ptr = value_p->FloatPtr();
    Bool delete_it;
    const Float *in_ptr = values.getStorage(delete_it);
    for (uInt i=0; i<length; i++) {
	out_ptr[i] = in_ptr[i];
    }
    values.freeStorage(in_ptr, delete_it);
    add_shape(value_p, values.shape());
    DebugAssert(ok(), AipsError);
}

GlishArray::GlishArray(const Array<Double> &values) : GlishValue()
{
    Unref(value_p);
    uInt length = values.nelements();
    value_p = make_glish_value(TYPE_DOUBLE, length);
    double *out_ptr = value_p->DoublePtr();
    Bool delete_it;
    const Double *in_ptr = values.getStorage(delete_it);
    for (uInt i=0; i<length; i++) {
	out_ptr[i] = in_ptr[i];
    }
    values.freeStorage(in_ptr, delete_it);
    add_shape(value_p, values.shape());
    DebugAssert(ok(), AipsError);
}

GlishArray::GlishArray(const Array<Complex> &values) : GlishValue()
{
    Unref(value_p);
    uInt length = values.nelements();
    value_p = make_glish_value(TYPE_COMPLEX, length);
    glish_complex *out_ptr = value_p->ComplexPtr();
    Bool delete_it;
    const Complex *in_ptr = values.getStorage(delete_it);
    for (uInt i=0; i<length; i++) {
	out_ptr[i] = glish_complex(in_ptr[i].real(), in_ptr[i].imag());
    }
    values.freeStorage(in_ptr, delete_it);
    add_shape(value_p, values.shape());
    DebugAssert(ok(), AipsError);
}

GlishArray::GlishArray(const Array<DComplex> &values) : GlishValue()
{
    Unref(value_p);
    uInt length = values.nelements();
    value_p = make_glish_value(TYPE_DCOMPLEX, length);
    glish_dcomplex *out_ptr = value_p->DcomplexPtr();
    Bool delete_it;
    const DComplex *in_ptr = values.getStorage(delete_it);
    for (uInt i=0; i<length; i++) {
	out_ptr[i] = glish_dcomplex(in_ptr[i].real(), in_ptr[i].imag());
    }
    values.freeStorage(in_ptr, delete_it);
    add_shape(value_p, values.shape());
    DebugAssert(ok(), AipsError);
}

GlishArray::GlishArray(const Array<String> &values) : GlishValue()
{
    Unref(value_p);
    uInt length = values.nelements();
    value_p = make_glish_value(TYPE_STRING, length);
    charptr *out_ptr = value_p->StringPtr();
    Bool delete_it;
    const String *in_ptr = values.getStorage(delete_it);
    for (uInt i=0; i<length; i++) {
	char *old = (char*) out_ptr[i];
	if ((out_ptr[i] = strdup(in_ptr[i].chars())) == 0) {
	    throw_alloc_error(strlen(in_ptr[i].chars()) + 1);
	}
	if ( old ) free_memory( old );
    }
    values.freeStorage(in_ptr, delete_it);
    add_shape(value_p, values.shape());
    DebugAssert(ok(), AipsError);
}

GlishArray::GlishArray(Bool value) : GlishValue()
{
    if (value) {
	Unref(value_p);
	glish_bool bvalue = (value  ?  glish_true : glish_false);
	value_p = new Value(bvalue);
    }
    if (! value_p) throw_alloc_error(sizeof(Value));
    DebugAssert(ok(), AipsError);
}

GlishArray::GlishArray(uChar value) : GlishValue()
{
    Unref(value_p);
    value_p = new Value(byte(value));
    if (! value_p) throw_alloc_error(sizeof(Value));
    DebugAssert(ok(), AipsError);
}

GlishArray::GlishArray(Short value) : GlishValue()
{
    Unref(value_p);
    value_p = new Value(short(value));
    if (! value_p) throw_alloc_error(sizeof(Value));
    DebugAssert(ok(), AipsError);
}


GlishArray::GlishArray(Int value) : GlishValue()
{
    Unref(value_p);
    value_p = new Value(int(value));
    if (! value_p) throw_alloc_error(sizeof(Value));
    DebugAssert(ok(), AipsError);
}

GlishArray::GlishArray(Float value) : GlishValue()
{
    Unref(value_p);
    value_p = new Value(float(value));
    if (! value_p) throw_alloc_error(sizeof(Value));
    DebugAssert(ok(), AipsError);
}

GlishArray::GlishArray(Double value) : GlishValue()
{
    Unref(value_p);
    value_p = new Value(double(value));
    if (! value_p) throw_alloc_error(sizeof(Value));
    DebugAssert(ok(), AipsError);
}


GlishArray::GlishArray(Complex value) : GlishValue()
{
    Unref(value_p);
    value_p = new Value(glish_complex(value.real(), value.imag()));
    if (! value_p) throw_alloc_error(sizeof(Value));
    DebugAssert(ok(), AipsError);
}


GlishArray::GlishArray(DComplex value) : GlishValue()
{
    Unref(value_p);
    value_p = new Value(glish_dcomplex(value.real(), value.imag()));
    if (! value_p) throw_alloc_error(sizeof(Value));
    DebugAssert(ok(), AipsError);
}


GlishArray::GlishArray(const String &value) : GlishValue()
{
    Unref(value_p);
    value_p = new Value(value.chars());
    if (! value_p) throw_alloc_error(sizeof(Value));
    DebugAssert(ok(), AipsError);
}

GlishArray::GlishArray(const Char *value) : GlishValue()
{
    Unref(value_p);
    value_p = new Value(value);
    if (! value_p) throw_alloc_error(sizeof(Value));
    DebugAssert(ok(), AipsError);
}

GlishArray::~GlishArray()
{
    // Nothing
}

GlishArray &GlishArray::operator=(const GlishValue &other)
{
    DebugAssert(ok(), AipsError);
    if (this == &other) {
	return *this;
    }

    if (other.type() == GlishValue::RECORD) {
	throw(AipsError("GlishArray::operator=(const GlishValue &other) - "
			"assignment to a RECORD value"));
    }
    (GlishValue &)(*this) = other;
    return *this;
}

GlishArray &GlishArray::operator=(const GlishArray &other)
{
    DebugAssert(ok(), AipsError);
    if (this == &other) {
	return *this;
    }
    // Use base class assignment
    (GlishValue &)(*this) = (GlishValue &)other;
    return *this;
}

void GlishArray::reset()
{
    DebugAssert(ok(), AipsError);
    GlishArray empty;
    *this = empty;
}

IPosition GlishArray::shape() const
{
    // No ok() to avoid recursion
    const Value *shape_attribute = value_p->HasAttribute("shape");
    IPosition retval;

    int length_from_shape = -1;


    if (shape_attribute && shape_attribute->IsNumeric()) {
        // Make it an int array if necessary. More efficient than 
        // CoerceToIntArray if we are goint to get the shape more than once
        if (shape_attribute->Type() != TYPE_INT) {
	    // Logically const, not physically
	    Value *nc_shape_attribute = (Value *)shape_attribute;
	    nc_shape_attribute->Polymorph(TYPE_INT);
	}

	int ndim = shape_attribute->Length();

	retval.resize(ndim);
	const int *values = shape_attribute->IntPtr();
	if (ndim >= 1) {
	    length_from_shape = values[0];
	    retval(0) = values[0];
	}
	while (--ndim > 0) {
	    length_from_shape *= values[ndim];
	    // Ensure that length_from_shape is negative if any element is
	    // negative since we exit on the first negative value
	    if (values[ndim] < 0) {
		break;  
	    }
	    retval(ndim) = values[ndim];
	}
    }

    if (shape_attribute == 0 || shape_attribute == false_value || 
	length_from_shape < 0) {
	// If we don't have a shape, or we don't have a valid shape,
	// say we're a Vector.
	retval.resize(1);
	retval(0) = nelements();
    }

    DebugAssert(retval.product() == Int(nelements()), AipsError);

    return retval;
}

GlishArray::ElementType GlishArray::elementType() const
{
    DebugAssert(ok(), AipsError);

    switch (value_p->Type()) {

    case TYPE_BOOL:        return BOOL;
    case TYPE_BYTE:        return BYTE;
    case TYPE_SHORT:        return SHORT;
    case TYPE_INT:         return INT;
    case TYPE_FLOAT:       return FLOAT;
    case TYPE_DOUBLE:      return DOUBLE;
    case TYPE_COMPLEX:     return COMPLEX;
    case TYPE_DCOMPLEX:    return DCOMPLEX;
    case TYPE_STRING:      return STRING;
    default:
        throw(AipsError("ElementType GlishArray::elementType() const - "
			"unknown type"));
    }
    return BOOL; // NOTREACHED
}

Bool GlishArray::get(Array<Bool> &values, Bool attemptResize) const
{
    DebugAssert(ok(), AipsError);
    if (elementType() == STRING) {
	return False;
    }
    uInt nr = nelements();
    if (values.nelements() != nr) {
	if (attemptResize) {
	    values.resize(shape());
	} else {
	    throw(AipsError("Bool GlishArray::get(Array<Bool> &values) - "
			    "values not large enough"));
	}
    }
    if (nr == 0) {
        return True;
    }

    Bool delete_to;
    int delete_from;
    Bool *to = values.getStorage(delete_to);
    // sizeof glish_bool and Bool can be different, so we need a temporary.
    glish_bool *tmp = new glish_bool [nr];
    value_p->CoerceToBoolArray(delete_from, nr, tmp);
    for (uInt i=0; i<nr; i++) {
	to[i] = (tmp[i]  ?  True : False);
    }
    delete [] tmp;
    values.putStorage(to, delete_to);
    return True;
}

Bool GlishArray::get(Array<Int> &values, Bool attemptResize) const
{
    DebugAssert(ok(), AipsError);
    if (elementType() == STRING) {
	return False;
    }
    if (values.nelements() != nelements()) {
	if (attemptResize) {
	    values.resize(shape());
	} else {
	    throw(AipsError("Bool GlishArray::get(Array<Int> &values) - "
			    "values not large enough"));
	}
    }
    if (nelements() == 0) {
        return True;
    }

    Bool delete_to;
    int delete_from;
    Int *to = values.getStorage(delete_to);
    // If the following ever fails we need more development
    if (sizeof(Int) != sizeof(int)) {
	throw(AipsError("Glish and native types do not agree"));
    }
    value_p->CoerceToIntArray(delete_from, nelements(), (int *)to);
    values.putStorage(to, delete_to);
    return True;
}

Bool GlishArray::get(Array<uChar> &values, Bool attemptResize) const
{
    DebugAssert(ok(), AipsError);
    if (elementType() == STRING) {
	return False;
    }
    if (values.nelements() != nelements()) {
	if (attemptResize) {
	    values.resize(shape());
	} else {
	    throw(AipsError("Bool GlishArray::get(Array<uChar> &values) - "
			    "values not large enough"));
	}
    }
    if (nelements() == 0) {
        return True;
    }

    Bool delete_to;
    int delete_from;
    uChar *to = values.getStorage(delete_to);
    // If the following ever fails we need more development
    if (sizeof(uChar) != sizeof(byte)) {
	throw(AipsError("Glish and native types do not agree"));
    }
    value_p->CoerceToByteArray(delete_from, nelements(), (byte *)to);
    values.putStorage(to, delete_to);
    return True;
}

Bool GlishArray::get(Array<Short> &values, Bool attemptResize) const
{
    DebugAssert(ok(), AipsError);
    if (elementType() == STRING) {
	return False;
    }
    if (values.nelements() != nelements()) {
	if (attemptResize) {
	    values.resize(shape());
	} else {
	    throw(AipsError("Bool GlishArray::get(Array<Short> &values) - "
			    "values not large enough"));
	}
    }
    if (nelements() == 0) {
        return True;
    }

    Bool delete_to;
    int delete_from;
    Short *to = values.getStorage(delete_to);
    // If the following ever fails we need more development
    if (sizeof(Short) != sizeof(short)) {
	throw(AipsError("Glish and native types do not agree"));
    }
    value_p->CoerceToShortArray(delete_from, nelements(), (short *)to);
    values.putStorage(to, delete_to);
    return True;
}

Bool GlishArray::get(Array<Float> &values, Bool attemptResize) const
{
    DebugAssert(ok(), AipsError);
    if (elementType() == STRING) {
	return False;
    }
    if (values.nelements() != nelements()) {
	if (attemptResize) {
	    values.resize(shape());
	} else {
	    throw(AipsError("Bool GlishArray::get(Array<Float> &values) - "
			    "values not large enough"));
	}
    }
    if (nelements() == 0) {
        return True;
    }

    Bool delete_to;
    int delete_from;
    Float *to = values.getStorage(delete_to);
    // If the following ever fails we need more development
    if (sizeof(Float) != sizeof(float)) {
	throw(AipsError("Glish and native types do not agree"));
    }
    value_p->CoerceToFloatArray(delete_from, nelements(), (float *)to);
    values.putStorage(to, delete_to);
    return True;
}

Bool GlishArray::get(Array<Double> &values, Bool attemptResize) const
{
    DebugAssert(ok(), AipsError);
    if (elementType() == STRING) {
	return False;
    }
    if (values.nelements() != nelements()) {
	if (attemptResize) {
	    values.resize(shape());
	} else {
	    throw(AipsError("Bool GlishArray::get(Array<Double> &values) - "
			    "values not large enough"));
	}
    }
    if (nelements() == 0) {
        return True;
    }

    Bool delete_to;
    int delete_from;
    Double *to = values.getStorage(delete_to);
    // If the following ever fails we need more development
    if (sizeof(Double) != sizeof(double)) {
	throw(AipsError("Glish and native types do not agree"));
    }
    value_p->CoerceToDoubleArray(delete_from, nelements(), (double *)to);
    values.putStorage(to, delete_to);
    return True;
}

Bool GlishArray::get(Array<Complex> &values, Bool attemptResize) const
{
    DebugAssert(ok(), AipsError);
    if (elementType() == STRING) {
	return False;
    }
    if (values.nelements() != nelements()) {
	if (attemptResize) {
	    values.resize(shape());
	} else {
	    throw(AipsError("Bool GlishArray::get(Array<Complex> &values) - "
			    "values not large enough"));
	}
    }
    if (nelements() == 0) {
        return True;
    }

    Bool delete_to;
    int delete_from;
    Complex *to = values.getStorage(delete_to);
    // If the following ever fails we need more development
    if (sizeof(Complex) != sizeof(glish_complex)) {
	throw(AipsError("Glish and native types do not agree"));
    }
    value_p->CoerceToComplexArray(delete_from, nelements(), (glish_complex *)to);
    values.putStorage(to, delete_to);
    return True;
}

Bool GlishArray::get(Array<DComplex> &values, Bool attemptResize) const
{
    DebugAssert(ok(), AipsError);
    if (elementType() == STRING) {
	return False;
    }
    if (values.nelements() != nelements()) {
	if (attemptResize) {
	    values.resize(shape());
	} else {
	    throw(AipsError("Bool GlishArray::get(Array<DComplex> &values) - "
			    "values not large enough"));
	}
    }
    if (nelements() == 0) {
        return True;
    }

    Bool delete_to;
    int delete_from;
    DComplex *to = values.getStorage(delete_to);
    // If the following ever fails we need more development
    if (sizeof(DComplex) != sizeof(glish_dcomplex)) {
	throw(AipsError("Glish and native types do not agree"));
    }
    value_p->CoerceToDcomplexArray(delete_from, nelements(), (glish_dcomplex *)to);
    values.putStorage(to, delete_to);
    return True;
}

Bool GlishArray::get(Array<String> &values, Bool attemptResize) const
{
    DebugAssert(ok(), AipsError);
    if (elementType() != STRING) {
	return False;
    }
    if (values.nelements() != nelements()) {
	if (attemptResize) {
	    values.resize(shape());
	} else {
	    throw(AipsError("Bool GlishArray::get(Array<Complex> &values) - "
			    "values not large enough"));
	}
    }
    if (nelements() == 0) {
        return True;
    }

    int is_copy;
    charptr * strings = value_p->CoerceToStringArray(is_copy, nelements());
    // String copies are expensive anyway...
    ArrayPositionIterator api(values.shape(), 0);
    uInt i=0;
    while (! api.pastEnd()) {
	values(api.pos()) = strings[i];
	api.next(); i++;
    }
    return True;
}

Bool GlishArray::get(Bool &val, uInt index) const
{
    DebugAssert(index <= nelements(), AipsError);
    DebugAssert(ok(), AipsError);

    if (value_p->Type() == TYPE_STRING) {

	return False;
    }

    val =  (int(value_p->BoolVal(index+1)));
    return True;
}

Bool GlishArray::get(uChar &val, uInt index) const
{
    DebugAssert(index <= nelements(), AipsError);
    DebugAssert(ok(), AipsError);

    if (value_p->Type() == TYPE_STRING) {

	return False;
    }

    val = value_p->ByteVal(index+1);
    return True;
}

Bool GlishArray::get(Short &val, uInt index) const
{
    DebugAssert(index <= nelements(), AipsError);
    DebugAssert(ok(), AipsError);

    if (value_p->Type() == TYPE_STRING) {

	return False;
    }

    val = value_p->ShortVal(index+1);
    return True;
}

Bool GlishArray::get(Int &val, uInt index) const
{
    DebugAssert(index <= nelements(), AipsError);
    DebugAssert(ok(), AipsError);

    if (value_p->Type() == TYPE_STRING) {

	return False;
    }

    val = value_p->IntVal(index+1);
    return True;
}

Bool GlishArray::get(Float &val, uInt index) const
{
    DebugAssert(index <= nelements(), AipsError);
    DebugAssert(ok(), AipsError);

    if (value_p->Type() == TYPE_STRING) {

	return False;
    }

    val = value_p->FloatVal(index+1);
    return True;
}

Bool GlishArray::get(Double &val, uInt index) const
{
    DebugAssert(index <= nelements(), AipsError);
    DebugAssert(ok(), AipsError);

    if (value_p->Type() == TYPE_STRING) {

	return False;
    }

    val = value_p->DoubleVal(index+1);
    return True;
}

Bool GlishArray::get(Complex &val, uInt index) const
{
    DebugAssert(index <= nelements(), AipsError);
    DebugAssert(ok(), AipsError);

    if (value_p->Type() == TYPE_STRING) {

	return False;
    }

    glish_complex value = value_p->ComplexVal(index+1);
    ///    val.real() = value.r;
    ///    val.imag() = value.i;
    val = Complex(value.r, value.i);
    return True;
}

Bool GlishArray::get(DComplex &val, uInt index) const
{
    DebugAssert(index <= nelements(), AipsError);
    DebugAssert(ok(), AipsError);

    if (value_p->Type() == TYPE_STRING) {

	return False;
    }

    glish_dcomplex value = value_p->DcomplexVal(index+1);
    ///    val.real() = value.r;
    ///    val.imag() = value.i;
    val = DComplex(value.r, value.i);
    return True;
}

Bool GlishArray::get(String &val, uInt index) const
{
    DebugAssert(index <= nelements(), AipsError);
    DebugAssert(ok(), AipsError);

    if (value_p->Type() != TYPE_STRING) {

	return False;
    }
    val = index < nelements() ? value_p->StringPtr(0)[index] : "";
    return True;
}

Bool GlishArray::ok() const
{
    Bool isok = (Int(nelements()) == shape().product());
    return (isok && GlishValue::ok() && type() == ARRAY);
}

} //# NAMESPACE CASA - END

