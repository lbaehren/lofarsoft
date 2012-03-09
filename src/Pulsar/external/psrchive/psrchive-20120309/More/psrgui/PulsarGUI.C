
/***************************************************************************
 *
 *   Copyright (C) 2007 Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/psrgui/PulsarGUI.C,v $
   $Revision: 1.12 $
   $Date: 2008/07/16 02:27:57 $
   $Author: straten $ */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PulsarGUI.h"

#include "Pulsar/InterfaceDialog.h"
#include "Pulsar/UsingXSERVE.h"

#ifdef HAVE_QTDRIV
#include "Pulsar/UsingQTDRIV.h"
#endif

using namespace std;

Pulsar::PulsarGUI::PulsarGUI(QApplication* qa)
  : QMainWindow(0, 0, WDestructiveClose)
{
  myApp = qa;

  // First, define all user Actions
  QAction* fileOpenAction = new QAction("&Open File", CTRL+Key_O, this);

  connect(fileOpenAction, SIGNAL(activated()), this, SLOT(fileOpen()));

  QAction* fileQuitAction = new QAction("&Quit", CTRL+Key_Q, this);

  connect(fileQuitAction, SIGNAL(activated()), this, SLOT(fileQuit()));

  QAction* confFrameAction = new QAction("&Frame", CTRL+Key_F, this);

  connect(confFrameAction, SIGNAL(activated()), this, SLOT(confFrame()));

  QAction* confGraphAction = new QAction("&Plot", CTRL+Key_P, this);

  connect(confGraphAction, SIGNAL(activated()), this, SLOT(confGraph()));

  QAction* confProcAction = new QAction("&Data", CTRL+Key_D, this);

  connect(confProcAction, SIGNAL(activated()), this, SLOT(confProc()));

  QAction* confDialogAction = new QAction("&Interactive", CTRL+Key_I, this);

  connect(confDialogAction, SIGNAL(activated()), this, SLOT(confDialog()));

  // Look after the global placement of Widgets
  QHBox* layout = new QHBox(this, "layout");
  setCentralWidget(layout);

  // Look after the global placement of Widgets
  QGroupBox* panel = new QGroupBox(4, Qt::Vertical, "Control Panel", layout);

  // Nest a section for the pre-processor options
  QGroupBox* prebox = new QGroupBox(1, Qt::Horizontal, "Pre-processor Options",
                                    panel);

  // Construct a LineEdit for the pre-processor
  ppLe = new QLineEdit(prebox);

  connect(ppLe, SIGNAL(returnPressed()), this, SLOT(preProcess()));

  // Baseline removal selector
  autoBase = new QCheckBox("Automatic Baseline Removal", prebox);
  autoBase->setChecked(true);

  QPushButton* refreshButton = new QPushButton("Undo All", prebox);
  connect(refreshButton, SIGNAL(clicked()), this, SLOT(undoChanges()));

  ppengine = new Pulsar::Interpreter();

  // Construct a radio button group to hold the plot options
  psrButtons = new QButtonGroup(1, Qt::Horizontal, 
				"Pulsar Plot Methods", panel);
  psrButtons->setRadioButtonExclusive(true);

  // Define some useful main menus
  fileMenu = new QPopupMenu(this);
  menuBar()->insertItem("&File", fileMenu);

  confMenu = new QPopupMenu(this);
  menuBar()->insertItem("&Configure", confMenu);

  // Define a useful shortcuts toolbar
  //psrTools = new QToolBar("Pulsar Tools", this);
  // QWhatsThis::whatsThisButton(psrTools);

  // Connect the Actions to the toolbar and menus
  //fileOpenAction->addTo(psrTools);
  fileOpenAction->addTo(fileMenu);
  fileQuitAction->addTo(fileMenu);

  confFrameAction->addTo(confMenu);
  confGraphAction->addTo(confMenu);
  confProcAction->addTo(confMenu);
  confDialogAction->addTo(confMenu);
  
  // Construct available plot styles
  static PlotFactory factory;

  // Store the pointers in case they come in handy
  vector<plotItem*> pointers;
  
  for (unsigned i = 0; i < factory.get_nplot(); i++)
  {
    plotItem* item = new plotItem (psrButtons,
				   factory.construct(factory.get_name(i)),
				   factory.get_description(i),
				   factory.get_description(i));

    QObject::connect(item, SIGNAL(clicked()), this, SLOT(plotGraph()));

    pointers.push_back(item);
  }

  psrButtons->adjustSize();
  
  // Allow the user to open a pgplot window and draw the active plot
  //QPushButton* launcher = new QPushButton("Draw Plot", panel);
  //QObject::connect(launcher, SIGNAL(clicked()), this, SLOT(plotGraph()));
 
#ifdef HAVE_QTDRIV
  Pulsar::UsingQTDRIV* qtdriv = new Pulsar::UsingQTDRIV (layout, "qtdriv");
  window = qtdriv;
  layout->setStretchFactor (panel, 1);
  layout->setStretchFactor (qtdriv, 4);
  layout->adjustSize();
#else
  window = new Pulsar::UsingXSERVE ("323/xs");
#endif

}

void Pulsar::PulsarGUI::fileOpen()
{
  QString filename = QFileDialog::getOpenFileName("./", "*.*", this);
  if (!filename.isEmpty())
    readFile(filename);
}

void Pulsar::PulsarGUI::readFile(QString& filename) try
{
  arch = Pulsar::Archive::load(filename);
  if (autoBase->isChecked())
    arch->remove_baseline();
  preti = arch->get_interface();
  ppengine->set(arch);
  preProcess();
}
catch (Error& error)
{
  report (error);
}

void Pulsar::PulsarGUI::fileQuit()
{
  myApp->exit();
}

void Pulsar::PulsarGUI::confFrame()
{
  plotItem* pi = 0;
  TextInterface::Parser* ui = 0;

  pi = dynamic_cast<plotItem*> (psrButtons->selected());

  if (pi) {
    ui = pi->getPlot()->get_frame_interface();
  }
  
  if (ui) {
    interfacePanel* panl = new interfacePanel(this, ui);
    panl->show();
  }
}

void Pulsar::PulsarGUI::confGraph()
{
  plotItem* pi = 0;
  TextInterface::Parser* ui = 0;

  pi = dynamic_cast<plotItem*> (psrButtons->selected());

  if (pi)
    ui = pi->getPlot()->get_interface();

  if (!ui)
    return;

  interfacePanel* panl = new interfacePanel(this, ui);
  panl->show();
}


void Pulsar::PulsarGUI::confProc()
{
  if (!preti)
    return;

  interfacePanel* panl = new interfacePanel(this, preti);
  panl->show();
}

void Pulsar::PulsarGUI::confDialog()
{
  if (!window)
    return;

  InterfaceDialog* panl = new InterfaceDialog (this);
  panl->set_window (window);
  panl->show();
}

void Pulsar::PulsarGUI::preProcess()
{
  QString jobs = ppLe->text();

  if (!jobs.isEmpty()) try
  {
    vector<string> joblist;
    separate(jobs.ascii(), joblist, ",");
    ppengine->script(joblist);
  }
  catch(Error& error)
  {
    report (error);
  }

  plotGraph ();
}

void Pulsar::PulsarGUI::undoChanges()
{
  arch->refresh();
  if (autoBase->isChecked())
    arch->remove_baseline();
  plotGraph ();
}

void Pulsar::PulsarGUI::plotGraph() try
{
  if (!arch)
    throw Error (InvalidState, "Pulsar::PulsarGUI::plotGraph",
		 "no Archive loaded");

  plotItem* pi = dynamic_cast<plotItem*> (psrButtons->selected());
    
  if (pi)
    window->set_plot( pi->getPlot() );

    window->set_data( arch );

  window->plot_data ();
}
catch (Error& error)
  {
    report (error);
  }

void Pulsar::PulsarGUI::report (Error& error)
{
  QErrorMessage* em = new QErrorMessage(this);
  QString useful = error.get_message().c_str();
  em->message(useful);
}
