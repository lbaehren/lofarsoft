/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <iostream>
#include <string>

#include <qbuttongroup.h>
#include <qlayout.h>

#include "qt_toglParams.h"
#include "ephio.h"

qt_toglParams::qt_toglParams (QWidget* parent, const char* name, int columns):
  QWidget (parent, name),
  elements (EPH_NUM_KEYS)
{
  QVBoxLayout* layout = new QVBoxLayout (this);
  layout->setMargin (4);

  QButtonGroup* table = new QButtonGroup (columns, Horizontal, 
					  "TEMPO Pulsar Ephemeris Parameters",
					  this, "buttons");

  layout->addWidget (table);

  int width=-1, height=-1;

  // want to space the elements down the columns (looks nicer)
  // but a QButtonGroup spaces the buttons across the columns (implementation)
  // work around using QButtonGroup::insert() and an extra loop for each row
  int rows = EPH_NUM_KEYS / columns;
  if (EPH_NUM_KEYS % columns)
    rows ++;

  for (int irow = 0; irow < rows; irow++)
    for (int ieph = irow; ieph < EPH_NUM_KEYS; ieph+=rows) {
      elements[ieph] = new QCheckBox (parmNames[ieph], table, parmNames[ieph]);
      int id = table -> insert (elements[ieph], ieph);
      if (id != ieph) {
	std::cerr << "qt_toglParams: error assigning id:" << ieph << std::endl;
	throw std::string ("qt_toglParams::error");
      }
      if (width < 0) {
	width = elements[ieph]->fontMetrics().maxWidth();
	height = elements[ieph]->fontMetrics().height();
      }
      elements[ieph]->setFixedSize (width * 9, height);
    }
  
  layout->activate ();
  //setMinimumSize (columns*width+10, (EPH_NUM_KEYS/columns)*height+10);
  connect (table, SIGNAL( clicked(int) ), this, SLOT ( wasChecked(int) ));
}
