/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "qt_fileParams.h"

#include "psrephem.h"
#include <iostream>

using namespace std;

bool qt_fileParams::verbose = false;

qt_fileParams::qt_fileParams ( const QString& startname, QWidget* parent ) 
  : QFileDialog (parent, "EphFileDialog", true )
{
  setSelection (startname);

  QString intro ("TEMPO PSR Parameter Files (*");
  QString close (")");

  QStringList filter;
 
  vector<string> ephext;
  ephext.push_back(".eph");
  ephext.push_back(".par");

  for (vector<string>::iterator str = ephext.begin();
       str != ephext.end(); str ++)
    filter.append ( intro + QString( str->c_str() ) + close );
  filter.append ( "Any File (*.*)" );

  setFilters ( filter );

  connect ( this, SIGNAL ( fileSelected (const QString&) ), 
	    this, SLOT ( chosen (const QString&) ) );
}

int qt_fileParams::open (Legacy::psrephem& eph)
{
  if (verbose)
    cerr << "qt_fileParams::open exec" << endl;

  setMode (QFileDialog::ExistingFile);
  if ( exec () != 1 || fileName.empty() )
    return 0;

  if (verbose)
    cerr << "qt_fileParams::open '" << fileName << "'" << endl;

  eph.load (fileName.c_str());
  cerr << "RETURN FROM psrephem::load (" << fileName << ")" << endl;

  return 1;
}

int qt_fileParams::save (const Legacy::psrephem& eph)
{
  setMode (QFileDialog::AnyFile);
  if ( exec () != 1 || fileName.empty() )
    return 0;

  if (verbose)
    cerr << "qt_fileParams::save to '" << fileName << "'" << endl;

  eph.unload (fileName.c_str());
  return 1;
}

void qt_fileParams::chosen ( const QString& name )
{
  fileName = name.ascii();
  if (verbose)
    cerr << "qt_fileParams::chosen '" << fileName << "'" << endl;
}
