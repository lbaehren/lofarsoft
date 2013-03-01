/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* ///////////////////////////////////////////////////////////////////////
   The Qt psrParameter Widgets

      qt_psrParameter   -  abstract base class from which each element of the
                       qt_psrParams widget is derived.

   The following derived classes are defined in qt_psrParameter.C
      qt_ephemString  
      qt_ephemDouble
      qt_ephemMJD
      qt_ephemAngle

   //////////////////////////////////////////////////////////////////////// */

#ifndef __QT_EPHEMVAL_H
#define __QT_EPHEMVAL_H

#include <qhbox.h>
#include <qcheckbox.h>
#include <qlabel.h>

#include "psrParameter.h"

class qt_psrParameter : public QHBox, public psrParameter
{
  friend class qt_psrephem;

 public:
  // constructor used by derived classes only
  qt_psrParameter (int ephio_index, QWidget* parent=0, const char* name=0);

  // virtual destructor
  virtual ~qt_psrParameter () {};

  void setFit (bool infit);
  bool getFit () const;

  void setLabel (const char* textval);
  void setLabel (const QPixmap& ulabel);

  virtual Angle  getAngle  () const { return Angle(); };
  virtual MJD    getMJD    () const { return MJD(); };
  virtual double getDouble () const { return 0.0; };
  virtual std::string getString () const { return std::string(); };
  virtual double getError  () const { return 0.0; };

  virtual void setAngle  (const Angle&) { };
  virtual void setMJD    (const MJD&) { };
  virtual void setDouble (double) { };
  virtual void setString (const std::string&) { };
  virtual void setError  (double) { };

  void setValue (psrParameter* eph);

  // factory returns pointer to one of derived classes.
  static qt_psrParameter* factory (int ephio_index, QWidget* parent=0);

  static int v_precision;    // precision displayed in the value
  static int e_precision;    // precision displayed in the error

  static bool verbose;

 protected:
  QCheckBox checkfit;  // the check-box
  QLabel    label;     // if no check-box, then some text
  QHBox     body;      // the body into which derived classes place data

  // basically a dynamic copy constructor
  psrParameter* duplicate ();
};

#endif
