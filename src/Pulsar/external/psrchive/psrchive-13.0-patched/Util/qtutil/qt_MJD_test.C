/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <qapplication.h>
#include "qt_MJD.h"

int main( int argc, char **argv )
{
  QApplication a( argc, argv );

  MJD today (time (NULL));

  qt_MJD display;
  display.setMJD (today);

  a.setMainWidget( &display );
  display.show();

  return a.exec();
}
