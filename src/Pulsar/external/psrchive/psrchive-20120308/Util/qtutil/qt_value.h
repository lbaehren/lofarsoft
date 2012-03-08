/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifndef __QT_VALUE_H
#define __QT_VALUE_H

#include <qhbox.h>
#include <qlineedit.h>
#include <qlabel.h>

class qt_value : public QHBox
{
  Q_OBJECT
 public:
  qt_value (bool error=false, QWidget *parent=NULL, const char *name=NULL);

  void   setError (double error=0.0);
  double getError () const { return errset; };

  static bool verbose;
  static int val_max_len;
  static int err_max_len;
  static int default_val_precision;
  static int default_err_precision;

  static char str_data [80];

  int val_precision;
  int err_precision;

 protected slots:
  void error_Entered_CB ();

 protected:
  // Widgets
  QLineEdit value;
  QLabel    plusminus;
  QLineEdit error;

  bool has_error;
  double errset;
};

#endif
