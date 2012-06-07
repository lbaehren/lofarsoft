/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* ///////////////////////////////////////////////////////////////////////
   The PsrParameters

      PsrParameter   -  base class from which each element of the
                        PsrParams class is derived.

   The following derived classes are defined:
      psrString  
      psrDouble
      psrMJD
      psrAngle

   Philosophy: It might have been neater to make a number of virtual member
               functions such as getMJD, getAngle, etc. in the base class
               and allow derived classes to over-ride.  However, the whole
               point to this exercise was to make a light-weight, typed
	       placeholder of information, not a huge vtbl.  So, static
	       member functions are provided that do dynamic casting to
	       get information in-out in a type-safe manner.

   Author: Willem van Straten

   //////////////////////////////////////////////////////////////////////// */

#ifndef __PSRPARAMETER_H
#define __PSRPARAMETER_H

#include <string>

#include "MJD.h"
#include "Angle.h"

class psrParameter
{
  friend class psrParams;

 public:
  static bool verbose;
  static psrParameter null;  // null value returned in some instances

  // constructor used by derived classes only
  psrParameter (int ephio_index, bool infit = false, double error = 0.0);
  
  virtual ~psrParameter () {};

  // verifies the index given, throws an exception if invalid, otherwise
  // returns the index given.
  static int check (int ephio_index);

  // return true if this psrParameter may have an associated error
  bool has_error();
  // return the error in this parameter
  double getError () { return error; };

  // return true if this psrParameter is fitable
  bool fitable ();
  // return true if the flag is set to fit for this element
  bool getFit () const { return infit; };

  // return true if this psrParameter is valid
  bool valid() const { return ephio_index != -1; }

  // return the index as in ephio.h
  int  get_ephind () const { return ephio_index; };

  // set the fit flag to 'infit'
  void setFit (bool fit) { infit = fit && fitable(); };

  // return the value
  std::string getString ();
  double getDouble ();
  MJD    getMJD    ();
  Angle  getAngle  ();
  int    getInteger();

  // set value
  void setString (const std::string&);
  void setDouble (double );
  void setMJD    (const MJD&);
  void setAngle  (const Angle&);
  void setInteger(int);

 protected:
  int    ephio_index; // where in the ephio.h of things this element fits in
  double error;       // the error in the value
  bool   infit;       // the 'fit' flag is set for this element

  // basically a dynamic copy constructor
  psrParameter* duplicate ();

  // null constructor is protected
  psrParameter () { ephio_index = -1; error = 0.0; infit = false; };
};

// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
//
// psrString
//
// Class defines the appearance/behaviour of psrParams text elements.
//
// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////

class psrString : public psrParameter
{
 public:
  psrString (int ephind, const std::string& val, bool in_fit=false) :
    psrParameter (ephind, false, in_fit),
    value (val)  { };

  std::string getString () { return value; };
  void   setString (const std::string& str) { value = str; };

 protected:
  std::string value;
};

// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
//
// psrDouble
//
// Class defines the appearance/behaviour of psrParams real valued elements.
//
// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
class psrDouble : public psrParameter
{
 public:
  psrDouble (int ephind, double val,
	     double err = 0.0, bool in_fit = false) :
    psrParameter (ephind, in_fit, err),
    value (val) { };
    
  double getDouble () { return value; };
  void   setDouble (double val, double err)
    { value = val; error = err; };
  void   setDouble (double val)
    { value = val; };

 protected:
  double value;
};

// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
//
// psrInteger
//
// Class defines the appearance/behaviour of psrParams real valued elements.
//
// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
class psrInteger : public psrParameter
{
 public:
  psrInteger (int ephind, int val, int err = 0, bool in_fit = false) :
    psrParameter (ephind, in_fit, err),
    value (val) { };
    
  int getInteger () { return value; };
  void setInteger (int val, int err)
    { value = val; error = err; };
  void setInteger (int val)
    { value = val; };

 protected:
  int value;
};

// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
//
// psrMJD
//
// Class defines the appearance/behaviour of psrParams MJD elements.
//
// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
class psrMJD : public psrParameter
{
 public:
  psrMJD (int ephind, const MJD& mjd, double err=0, bool in_fit=false) :
    psrParameter (ephind, in_fit, err),
    value (mjd) { };

  psrMJD (int ephind, int days, double fracdays, 
	  double err=0, bool in_fit=false) :
    psrParameter (ephind, in_fit, err),
    value (days, fracdays) { };
  
  MJD    getMJD () { return value; };
  void   setMJD (const MJD& val, double err)
    { value = val; error = err; };
  void   setMJD (const MJD& val)
    { value = val; };

 protected:
  MJD value;
};

// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
//
// psrAngle
//
// Class defines the appearance/behaviour of psrParams angular elements.
//
// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
class psrAngle : public psrParameter
{
 public:
  psrAngle (int ephind, const Angle& angle, double err=0, bool in_fit=false, bool RA=false) :
    psrParameter (ephind, in_fit, err),
    value (angle) { };

  psrAngle (int ephind, double turns, double err=0, bool in_fit=false) :
    psrParameter (ephind, in_fit, err),
    value (turns * 2.0 * M_PI) { };

  Angle  getAngle () { return value; };
  void   setAngle (const Angle& val, double err)
    { value = val; error = err; };
  void   setAngle (const Angle& val)
    { value = val; };
 
  void is_RA () { 
                value.setWrapPoint(2*M_PI);
   		if (value.getRadians() < 0) {
		   value.setRadians(value.getRadians()+2*M_PI);
		};
  };	
 protected:
  Angle value;
};

#endif
