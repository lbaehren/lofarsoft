/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
//#include <qhbox.h>
//#include <qcheckbox.h>

#include "qt_psrParameter.h"
#include "qt_double.h"
#include "qt_MJD.h"
#include "qt_Angle.h"

#include "ephio.h"

using namespace std;

// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
//
// qt_psrParameter
//
// Abstract base class defines the appearance/behaviour of psrParams elements.
// Derived classes are basically an implementation detail that no code need
// worry about after using the psrParameter::factory to obtain a new derived
// qt_psrParameter object.
//
// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////

int  qt_psrParameter::v_precision = 16;
int  qt_psrParameter::e_precision = 3;
bool qt_psrParameter::verbose = false;

// //////////////////////////////////////////////////////////////////////////
// Constructor checks that the eph_index is in ranges, so none of the
// member functions ever need to again (unless someone else messes it up)
qt_psrParameter::qt_psrParameter (int ephind, QWidget* parent,const char* name)
  : QHBox (parent, name),
    psrParameter (ephind),
    checkfit (parmNames[ephind], this),
    label (parmNames[ephind], this),
    body  ((QWidget *)this, "body")
{
  checkfit.setChecked (false);
  if (fitable())
    label.hide();
  else
    checkfit.hide();

  int width = label.fontMetrics().maxWidth();
  int height = label.fontMetrics().height();

  checkfit.setMinimumSize (width * 8, height);
  label.setMinimumSize (width * 8, height);
  body.resize  (width * 20, height);
}

// //////////////////////////////////////////////////////////////////////////
void qt_psrParameter::setFit (bool fit)
{
  psrParameter::setFit (fit);
  checkfit.setChecked (infit);
}

bool qt_psrParameter::getFit () const
{
  return checkfit.isChecked ();
}

// //////////////////////////////////////////////////////////////////////////
// puts text in the box on the left.
// Where this label ends up depends on whether
// the derived qt_psrParameter object is fitable or not.
void qt_psrParameter::setLabel (const char* textval)
{
  if (fitable())
    checkfit.setText (textval);
  else
    label.setText (textval);
}

// //////////////////////////////////////////////////////////////////////////
// puts a graphic in the box on the left.
void qt_psrParameter::setLabel (const QPixmap& ulabel)
{
  if (fitable())
    checkfit.setPixmap (ulabel);
  else
    label.setPixmap (ulabel);
}


// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
//
// qt_psrString
//
// Class defines the appearance/behaviour of psrParams text elements.
//
// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////

class qt_psrString : public qt_psrParameter
{
 public:
  qt_psrString (int eph_index, QWidget* parent=0) :
    qt_psrParameter (eph_index, parent),
    value (&body) { };

  string getString () const { return string (value.text().ascii()); };
  void   setString (const string& str) { value.setText (str.c_str()); };

 protected:
  QLineEdit value;
};

// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
//
// qt_psrDouble
//
// Class defines the appearance/behaviour of psrParams real valued elements.
//
// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
class qt_psrDouble : public qt_psrParameter
{
 private:
  qt_double  value;

 public:
  qt_psrDouble (int eph_index, bool has_error=false, QWidget* parent=0) :
    qt_psrParameter (eph_index, parent),
    value (has_error, &body) { };

  double getDouble () const { return value.getDouble(); };
  void   setDouble (double val) { value.setDouble(val); };

  double getError () const { return value.getError(); };
  void   setError (double val) { value.setError(val); };
};

// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
//
// qt_psrInteger
//
// Class defines the appearance/behaviour of psrParams real valued elements.
//
// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
class qt_psrInteger : public qt_psrParameter
{
 private:
  qt_double  value;

 public:
  qt_psrInteger (int eph_index, bool has_error=false, QWidget* parent=0) :
    qt_psrParameter (eph_index, parent),
    value (has_error, &body) { };

  int   getInteger () const { return (int) value.getDouble(); };
  void  setInteger (int val) { value.setDouble((double)val); };
};

// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
//
// qt_psrMJD
//
// Class defines the appearance/behaviour of psrParams MJD elements.
//
// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
class qt_psrMJD : public qt_psrParameter
{
private:
  qt_MJD value;

public:
  qt_psrMJD (int eph_index, bool has_error=false, QWidget* parent=0) :
    qt_psrParameter (eph_index, parent),
    value (has_error, &body)  { };

  MJD getMJD () const { return value.getMJD (); };
  void setMJD (const MJD& val) { value.setMJD (val); };

  double getError () const { return value.getError(); };
  void   setError (double val) { value.setError(val); };
};

// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
//
// qt_psrAngle
//
// Class defines the appearance/behaviour of psrParams angular elements.
//
// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
class qt_psrAngle : public qt_psrParameter
{
private:
  qt_Angle value;

public:
  qt_psrAngle (int ephind, bool with_error=false, QWidget* parent=0) :
    qt_psrParameter (ephind, parent),
    value (with_error, &body)
  {
    if (parmTypes[get_ephind()] == 2) {
      if (verbose) cerr << "qt_psrAngle::setvalue HMS:" 
			<< parmNames[get_ephind()] << endl;
      value.displayHMS ();
    }
    else {
      if (verbose) cerr << "qt_psrAngle::setvalue DMS:" 
			<< parmNames[get_ephind()] << endl;
      value.displayDMS ();
    }
  };
  // Added this to tell the qt_Angle what type it is  
  void sethms(bool _hms) {value.sethms(_hms);};
  
  Angle getAngle () const { return value.getAngle (); };
  void setAngle (const Angle& val) { value.setAngle (val); };

  double getError () const { return value.getError(); };
  void   setError (double val) { value.setError(val); };
};

// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
//
// qt_psrParameter::factory
//
// returns a pointer to an appropriate instance of one of
// the derived classes of the qt_psrParameter base class
//
// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////

qt_psrParameter* qt_psrParameter::factory (int ephind, QWidget* parent)
{
  if (ephind < 0 || ephind >= EPH_NUM_KEYS) {
    string error ("qt_psrParameter::new_appropriate range error");
    cerr << error << endl;
    throw (error);
  }

  if (verbose)
    cerr << "qt_psrParameter::factory new" << parmNames[ephind] << endl;

  switch (parmTypes[ephind]) {
  case 0:  // other strings (JNAME, TZRSITE, etc.)
    return new qt_psrString (ephind, parent);

  case 1:  // any double
    return new qt_psrDouble (ephind, parmError[ephind]==1, parent);

  case 2:  {
    qt_psrAngle *ang;
    ang = new   qt_psrAngle (ephind, parmError[ephind]==1, parent); 
    ang->sethms(true);
    return ang;
	   } // RAs
  case 3:  // DECs
    return new qt_psrAngle (ephind, parmError[ephind]==1, parent);

  case 4:  // MJDs
    return new qt_psrMJD (ephind, parmError[ephind]==1, parent);

  case 5:  // integers
    return new qt_psrInteger (ephind, parmError[ephind]==1, parent);

  default:
    cerr << "qt_psrParameter::factory no type:" << parmTypes[ephind] << endl;
    return NULL;
  }
}

void qt_psrParameter::setValue (psrParameter* parm)
{
  if (parm->get_ephind() != ephio_index) {
    if (verbose)
      cerr << "qt_psrParameter::setValue invalid ephio index:" 
	   << parm->get_ephind() << " != " << ephio_index << endl;
    return;
  }

  qt_psrParameter::setFit (parm->getFit());
  setError ( parm->getError() );

  switch ( parmTypes[ephio_index] ) {

  case 0:  // other strings (JNAME, TZRSITE, etc.)
    setString ( parm->getString() );
    break;
    
  case 1:  // any double
    setDouble ( parm->getDouble() );
    break;

  case 2:   // RAs
  case 3:  // DECs
    setAngle ( parm->getAngle() );
    break;

  case 4:  // MJDs
    setMJD ( parm->getMJD() );
    break;

  case 5:  // MJDs
    setInteger ( parm->getInteger() );
    break;

  default:
    if (verbose)
      cerr << "qt_psrParameter::setValue unhandled case" << endl;
    break;
  }
}

psrParameter* qt_psrParameter::duplicate ()
{
  if (ephio_index < 0 || ephio_index >= EPH_NUM_KEYS)
    return NULL;

  switch ( parmTypes[ephio_index] ) {

  case 0:  // other strings (JNAME, TZRSITE, etc.)
    return new psrString ( ephio_index, getString(), getFit() );
    break;

  case 1:  // any double
    return new psrDouble ( ephio_index, getDouble(), getError(), getFit() );
    break;

  case 2:  // RAs
    return new psrAngle ( ephio_index, getAngle(), getError(), getFit(), true );
  case 3:  // DECs
    return new psrAngle ( ephio_index, getAngle(), getError(), getFit() );
    break;

  case 4:  // MJDs
    return new psrMJD ( ephio_index, getMJD(), getError(), getFit() );
    break;

  case 5:  // integers
    return new psrInteger ( ephio_index, getInteger(), getError(), getFit() );
    break;

  default:
    return NULL;
  }
}
