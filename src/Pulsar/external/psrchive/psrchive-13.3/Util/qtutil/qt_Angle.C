/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "qt_Angle.h"
#include <iostream>
using namespace std;

qt_Angle::qt_Angle (bool with_error, QWidget *parent, const char *name) :
  qt_value (with_error, parent, name)
{
  setAngle ();
  connect (&value, SIGNAL(returnPressed()), this, SLOT(value_Entered_CB()));
}

void qt_Angle::setAngle (const Angle& angle)
{
  valset = angle;
  
  if (hms) {
    valset.setWrapPoint(2*M_PI);
    if (angle.getRadians() < 0){
	valset.setRadians(angle.getRadians() + 2*M_PI);
    }
    value.setText (valset.getHMS(val_precision).c_str());
  }
  else
    value.setText (valset.getDMS(val_precision).c_str());	
}

void qt_Angle::value_Entered_CB ()
{
  int retval = 0;
  Angle newval;

  if (hms) {
    retval = newval.setHMS (value.text().ascii());
    newval.setWrapPoint(2*M_PI);
  }
  else
    retval = newval.setDMS (value.text().ascii());

  if (retval < 0) {
    if (Angle::verbose)
      cerr << "gtk_Angle:: invalid angle:" << value.text() << endl;
    newval = valset;
  }
  setAngle (newval);
}

void qt_Angle::displayHMS ()
{
  hms = true;
  valset.setWrapPoint (2*M_PI);

  if (valset.getRadians() < 0)
	valset.setRadians(valset.getRadians() + 2*M_PI);

  setAngle (valset);
}

void qt_Angle::displayDMS ()
{
  hms = false;
  valset.setWrapPoint (M_PI);
  setAngle (valset);
}

