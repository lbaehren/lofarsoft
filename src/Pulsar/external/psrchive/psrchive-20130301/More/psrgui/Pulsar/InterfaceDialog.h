//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __QT_InterfaceDialog_H
#define __QT_InterfaceDialog_H

#include "Pulsar/PlotWindow.h"
#include "TextInterface.h"

#include <qdialog.h>

class QVBox;
class QComboBox;
class ConfigurableScrollBar;

class InterfaceDialog : public QDialog
{
  Q_OBJECT

 public:

  InterfaceDialog (QWidget *parent=NULL, const char *name=NULL);

  //! Set the PlotWindow from which interfaces will be retrieved
  void set_window (Pulsar::PlotWindow*);

  //! Set the interface to which commands will be sent
  void set_interface (TextInterface::Parser*);

  //! Send the specified command to the interface
  void send (const std::string& command);

 protected slots:

  //! Add a configurable scrollbar
  void add_scrollbar_CB ();

  //! Change the target of scrollbar commands
  void change_target_CB (const QString&);

 protected:

  Reference::To<Pulsar::PlotWindow> window;
  Reference::To<TextInterface::Parser> interface;

  QComboBox* target;
  QVBox* vbox;

  class Forward;
  Reference::To<Forward> forward;

};

//! Forward inherits Reference::Able and forwards Callbacks to InterfaceDialog
/*! Avoids multiple inheritance of QDialog and Reference::Able */
class InterfaceDialog::Forward : public Reference::Able
{
public:
  Forward (InterfaceDialog* parent);
  void forward (const std::string&);
private:
  InterfaceDialog* parent;
};

#endif
