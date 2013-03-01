/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <qpixmap.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>

#include "rhythm.h"
#include "pulsar.xpm"

void Rhythm::toolbarConstruct ()
{
  QPixmap pulsar (pulsar_xpm);

  QToolBar* toolbar = new QToolBar ( "Timing Tools", this );
  toolbar -> addSeparator ();

  QToolButton* rhythm = 
    new QToolButton ( pulsar, "Rhythm", 
		      "Display a brief message about this application", 
		      this, SLOT( about() ), toolbar);
  rhythm -> setMinimumSize ( rhythm->sizeHint() );
}

