/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <qapplication.h>
#include "ConfigurableScrollBar.h"

using namespace std;

void print (const std::string& value)
{
  cerr << "scrollbar says " << value << endl;
}

int main( int argc, char **argv )
{
  QApplication a( argc, argv );

  ConfigurableScrollBar display;

  display.output.connect( &print );

  a.setMainWidget( &display );
  display.show();

  return a.exec();
}
