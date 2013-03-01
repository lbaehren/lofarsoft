/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifndef __RHYTHM_PARAMETERS_H
#define __RHYTHM_PARAMETERS_H

#include <qtabdialog.h>

class qt_ModelOptions;

class RhythmOptions : public QTabDialog
{
  Q_OBJECT

 public:
  RhythmOptions ( QWidget *parent, const char *name);

 protected:
  void setupTab2 ();
  void setupTab3 ();
};

#endif
