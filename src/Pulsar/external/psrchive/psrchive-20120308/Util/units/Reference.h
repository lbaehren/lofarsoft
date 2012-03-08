//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Reference.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:55 $
   $Author: straten $ */

//! Contains two basic classes that simplify dynamic memory management
/*! 

  The Reference::Able mixin base class and Reference::To template
  class jointly implement a hybrid set of dynamic memory management
  features.  Proper use of these classes will reduce the chances of
  memory leaks and dangling references.

  A Reference::To object behaves very much like a normal pointer,
  except that it can only be used to refer to objects that inherit
  Reference::Able.  For instance, if:

  class klass: public Reference::Able { [...] };

  then

  klass* kp = new klass;

  may be replaced with:

  Reference::To<klass> kp = new klass;

  Multiple Reference::To objects may point to a single Reference::Able
  instance.  However, no single Reference::To instance manages that
  Reference::Able object.  Instead, the number of Reference::To
  objects that point to the Reference::Able object are counted.  Note
  that reference counting is used only to eliminate ownership
  tracking, and no high-level resource sharing is implemented.

  The reference count is decremented whenever a Reference::To object
  is re-assigned or detroyed.  When the reference count reaches zero,
  the Reference::Able object is automatically deleted.  By noting
  which Reference::Able objects have been dynamcially allocated on the
  heap, static instances will not be automatically deleted.
  Therefore, the Reference::To class may also be safely used as
  follows:

  klass instance;
  Reference::To<klass> kp = &instance;

  Alternatively, if the Reference::Able object is deleted before its
  reference count reaches zero, all Reference::To instances that
  remain pointing to this object will be automatically invalidated.

  If an attempt is made to dereference an invalid Reference::To<klass>
  instance, a Reference::To<klass>::invalid exception will be thrown.

*/

#ifndef __Reference_h
#define __Reference_h

namespace Reference {

  //! Base class of reference managed objects
  class Able;

  //! Template class manages Reference::Able objects
  template<class Type, bool active> class To;

}

#include "Error.h"

#include "ReferenceTo.h"
#include "ReferenceAble.h"

#endif
