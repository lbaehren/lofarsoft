/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "psrParameter.h"
#include "strutil.h"
#include "ephio.h"

using namespace std;

// //////////////////////////////////////////////////////////////////////////
// qt_psrParameter static variables

psrParameter psrParameter::null;
bool psrParameter::verbose = false;

// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
//
// psrParameter
//
// Abstract base class defines the common methods for each element.
// Derived classes are basically an implementation detail that no code need
// worry about after using the psrParameter::factory to obtain a new instance
// of a derived type.
//
// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////

psrParameter::psrParameter (int ephind, bool in_fit, double err)
{
  ephio_index = check (ephind);
  infit = in_fit && fitable();
  error = 0.0;
  if (has_error())
    error = err;
}

// //////////////////////////////////////////////////////////////////////////
// checks that the index given is within the proper limits.
//    returns the value if ok
//    throws a string exception if not
int psrParameter::check (int ephind)
{
  if (ephind < 0 || ephind >= EPH_NUM_KEYS) {
    string err ("psrParameter::range error");
    cerr << err << endl;
    throw ( err );
  }
  return ephind;
}

bool psrParameter::fitable() { return parmTypes[ephio_index] != 0; }
bool psrParameter::has_error() { return parmError[ephio_index] == 1; }

// return the value
string psrParameter::getString ()
{
  string retval;
  psrString* me = dynamic_cast<psrString*>(this);
  if (me)
    retval = me -> getString();

  return retval;
}

double psrParameter::getDouble ()
{
  double retval = 0.0;
  psrDouble* me = dynamic_cast<psrDouble*>(this);
  if (me)
    retval = me -> getDouble();

  return retval;
}

int psrParameter::getInteger ()
{
  int retval = 0;
  psrInteger* me = dynamic_cast<psrInteger*>(this);
  if (me)
    retval = me -> getInteger();

  return retval;
}

MJD psrParameter::getMJD ()
{
  MJD  retval;
  psrMJD* me = dynamic_cast<psrMJD*>(this);
  if (me)
    retval = me -> getMJD ();

  return retval;
}

Angle psrParameter::getAngle ()
{
  Angle retval;
  psrAngle* me = dynamic_cast<psrAngle*>(this);
  if (me)
    retval = me -> getAngle();

  return retval;
}


// set value
void psrParameter::setString (const string& val)
{
  psrString* me = dynamic_cast<psrString*>(this);
  if (me)
    me -> setString(val);
}

void psrParameter::setInteger (int val)
{
  psrInteger* me = dynamic_cast<psrInteger*>(this);
  if (me)
    me -> setInteger(val);
}

void psrParameter::setDouble (double val)
{
  psrDouble* me = dynamic_cast<psrDouble*>(this);
  if (me)
    me -> setDouble(val);
}

void psrParameter::setMJD (const MJD& val)
{
  psrMJD* me = dynamic_cast<psrMJD*>(this);
  if (me)
    me -> setMJD(val);
}

void psrParameter::setAngle (const Angle& val)
{
  psrAngle* me = dynamic_cast<psrAngle*>(this);
  if (me)
    me -> setAngle(val);
}

psrParameter* psrParameter::duplicate ()
{
  if (ephio_index < 0 || ephio_index >= EPH_NUM_KEYS)
    return NULL;

  switch ( parmTypes[ephio_index] ) {

  case 0:  // other strings (JNAME, TZRSITE, etc.)
    return new psrString ( *dynamic_cast<psrString*>(this) );

  case 1:  // any double
    return new psrDouble ( *dynamic_cast<psrDouble*>(this) );

  case 2:  {
	     psrAngle *pangle = new psrAngle (*dynamic_cast<psrAngle*>(this));
	     pangle->is_RA();
	     return pangle;
	   }// RAs
  case 3:  // DECs
    return new psrAngle ( *dynamic_cast<psrAngle*>(this) );

  case 4:  // MJDs
    return new psrMJD ( *dynamic_cast<psrMJD*>(this) );

  case 5:  // MJDs
    return new psrInteger ( *dynamic_cast<psrInteger*>(this) );

  default:
    return NULL;
  }
}

