/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include <iostream>
#include "qt_value.h"

int qt_value::val_max_len = 20;
int qt_value::err_max_len = 12;
int qt_value::default_val_precision = 10;
int qt_value::default_err_precision = 3;

char qt_value::str_data [80];
bool qt_value::verbose = false;

qt_value::qt_value (bool with_error, QWidget *parent, const char *name) :
  QHBox     (parent, name),
  value     (this),
  plusminus ("+/-", this),
  error     (this)
{
  has_error = with_error;
  val_precision = default_val_precision;
  err_precision = default_err_precision;

  QSize onechar = value.minimumSizeHint ();

  value.setMaxLength (val_max_len);
  value.resize (onechar.width()*val_max_len, onechar.height());
  value.setMinimumSize (onechar.width()*val_precision, onechar.height());

  plusminus.setMaximumSize (onechar.width()*3, onechar.height());

  error.setMaxLength (err_max_len);
  error.setMaximumSize (onechar.width()*err_max_len, onechar.height());
  error.resize (onechar.width()*err_precision, onechar.height());
  error.setMinimumSize (onechar.width()*(err_precision+1), onechar.height());

  if (!has_error) {
    error.hide();
    plusminus.hide();
  }
  else
    connect (&error, SIGNAL(returnPressed()), this, SLOT(error_Entered_CB()));

  setError ();
}

void qt_value::setError (double err)
{
  if (!has_error)
    return;

  sprintf (str_data, "%.*le", err_precision, err);
  error.setText (str_data);

  errset = err;
}

void qt_value::error_Entered_CB ()
{
  double newval;

  if (sscanf (error.text().ascii(), "%lf", &newval) != 1) {
    if (verbose)
      std::cerr << "qt_value:: invalid value '"
                << error.text() << "'" << std::endl;
    // restore the old value
    newval = errset;
  }
  setError (newval);
}
