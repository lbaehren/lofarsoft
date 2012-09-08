/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <qapplication.h>
#include "qt_double.h"

int main( int argc, char **argv )
{
  QApplication a( argc, argv );

  qt_double display (true);

  a.setMainWidget( &display );
  display.show();

  return a.exec();
}
