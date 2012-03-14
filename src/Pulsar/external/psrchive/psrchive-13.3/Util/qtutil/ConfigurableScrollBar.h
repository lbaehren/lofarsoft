//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __QT_ConfigurableScrollBar_H
#define __QT_ConfigurableScrollBar_H

#include "Callback.h"

#include <qvbox.h>

class QLineEdit;
class QComboBox;
class QScrollBar;

class RangePolicy;

class ConfigurableScrollBar : public QVBox
{
  Q_OBJECT

 public:

  ConfigurableScrollBar (QWidget *parent=NULL, const char *name=NULL);

  Callback<std::string> output;

 protected slots:

  void parameter_name_CB ();
  void minimum_value_CB ();
  void maximum_value_CB ();
  void text_changed_CB (const QString&);
  void conversion_type_CB (const QString&);
  void scroll_bar_CB (int value);

 protected:

  QLineEdit* parameter_name;
  QLineEdit* minimum_value;
  QLineEdit* maximum_value;
  QComboBox* conversion_type;
  QScrollBar* scroll_bar;

  QPalette default_palette;
  QPalette highlight_palette;

  std::string current_parameter_name;

  RangePolicy* range_policy;
  void set_range_policy (RangePolicy*);

};


#endif
