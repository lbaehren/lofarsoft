/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include <iostream>
#include "qt_double.h"

qt_double::qt_double (bool with_error, QWidget *parent, const char *name) :
  qt_value (with_error, parent, name)
{
  setDouble();
  connect (&value, SIGNAL(returnPressed()), this, SLOT(value_Entered_CB()));
}

void qt_double::setDouble (double d_val)
{
  sprintf (str_data, "%.*le", val_precision, d_val);
  value.setText (str_data);
  valset = d_val;
}

void qt_double::value_Entered_CB ()
{
  double newval;

  if (sscanf (value.text().ascii(), "%lf", &newval) != 1) {
    if (verbose)
      std::cerr << "qt_double:: invalid value '"
                << value.text() << "'" << std::endl;
    newval = valset;
  }
  
  setDouble (newval);
}
