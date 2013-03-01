/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/InterfaceDialog.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Plot.h"

#include "ConfigurableScrollBar.h"

#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qvgroupbox.h>
#include <qhbox.h>

#include <iostream>
using namespace std;

InterfaceDialog::InterfaceDialog (QWidget *parent, const char *name)
  : QDialog (parent, name)
{
  forward = new Forward (this);

  QVBoxLayout* base = new QVBoxLayout (this);

  QVGroupBox* layout = new QVGroupBox ("Interface Dialog", this);
  base->addWidget (layout);

  QHBox* hbox = new QHBox (layout);

  QPushButton* add_scroll = new QPushButton ("Add ScrollBar", hbox);
  add_scroll->setDefault (false);
  add_scroll->setAutoDefault (false);

  connect (add_scroll, SIGNAL( clicked() ),
	   this, SLOT( add_scrollbar_CB() ));

  target = new QComboBox (hbox);

  target->insertItem ("Frame");
  target->insertItem ("Plot");
  target->insertItem ("Data");

  connect (target, SIGNAL( highlighted(const QString&) ),
	   this, SLOT( change_target_CB(const QString&) ));

  vbox = new QVBox (layout);

  add_scrollbar_CB ();

}

//! Set the PlotWindow from which interfaces will be retrieved
void InterfaceDialog::set_window (Pulsar::PlotWindow* w)
{
#ifdef _DEBUG
  cerr << "InterfaceDialog::set_window this=" << this << endl;
#endif
  window = w;
}

//! Set the interface to which commands will be sent
void InterfaceDialog::set_interface (TextInterface::Parser* _interface)
{
#ifdef _DEBUG
  cerr << "InterfaceDialog::set_interface " << _interface << endl;
#endif
  interface = _interface;
}

//! Send the specified command to the interface
void InterfaceDialog::send (const std::string& command)
{
#ifdef _DEBUG
  cerr << "InterfaceDialog::send command '" << command << "'" << endl;
#endif

  if (!interface)
  {
    cerr << "InterfaceDialog::send no interface" << endl;
    return;
  }

  try
  {
    interface->process (command);
  }
  catch (Error& error)
  {
    cerr << "InterfaceDialog::send error processing '" << command << "'\n"
	 << error.get_message () << endl;
  }

  if (!window)
  {
    cerr << "InterfaceDialog::send no window" << endl;
    return;
  }

  if (!window->ready())
  {
    cerr << "InterfaceDialog::send window not ready" << endl;
    return;
  }
  
  window->plot_data ();
}

//! Add a configurable scrollbar
void InterfaceDialog::add_scrollbar_CB ()
{
  ConfigurableScrollBar* bar = new ConfigurableScrollBar (vbox);

  bar->output.connect( forward, &InterfaceDialog::Forward::forward );
  bar->show ();
}

void InterfaceDialog::change_target_CB (const QString& selected)
{
#ifdef _DEBUG
  cerr << "InterfaceDialog::change_target_CB selected=" << selected << endl;
#endif

  if (!window)
  {
    cerr << "InterfaceDialog::change_target_CB no window" << endl;
    return;
  }

  if (!window->ready())
  {
    cerr << "InterfaceDialog::change_target_CB window not ready" << endl;
    return;
  }

  if (selected == "Data")
    set_interface( window->get_data()->get_interface() );

  else if (selected == "Frame")
    set_interface( window->get_plot()->get_frame_interface() );

  else if (selected == "Plot")
    set_interface( window->get_plot()->get_interface() );

  else
    cerr << "InterfaceDialog::change_target_CB selected=" 
	 << selected << " not understood" << endl;
}

InterfaceDialog::Forward::Forward (InterfaceDialog* _parent)
{
  parent = _parent;
}

void InterfaceDialog::Forward::forward (const std::string& message)
{
  parent->send (message);
}

