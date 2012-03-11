/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "rhythm.h"
#include "qt_editParams.h"
#include "find_standard.h"

#include "Pulsar/Integration.h"
#include "Pulsar/Telescope.h"

#include "Pulsar/ProfilePlot.h"
#include "Pulsar/InfoLabel.h"

#include "tempo++.h"
#include "Horizon.h"

#include <qmainwindow.h>
#include <qmessagebox.h> 
#include <qpopupmenu.h>
#include <qpalette.h>
#include <qplatinumstyle.h>
#include <qfont.h>

// #define _DEBUG 1

#include <cpgplot.h>

#include <algorithm>
#include <iostream>

#include <stdlib.h>

using namespace std;

#ifdef _DEBUG
bool Rhythm::verbose = true;
bool Rhythm::vverbose = true;
#else
bool Rhythm::verbose = false;
bool Rhythm::vverbose = false;
#endif

// /////////////////////////////////////////////////////////////////////
// Main procedure
// /////////////////////////////////////////////////////////////////////

int main (int argc, char** argv) try {
 
  if (Rhythm::vverbose)
    cerr << "construct QApplication" << endl;

  // QApplication must be constructed before any other GUI-related object
  QApplication app (argc, argv);

  if (Rhythm::vverbose)
    cerr << "construct Rhythm" << endl;

  Rhythm rhythm (&app, 0, argc, argv);
  
  if (Rhythm::vverbose)
    cerr << "call QApplication::setMainWidget" << endl;
  
  app.setMainWidget (&rhythm);
  
  if (Rhythm::vverbose)
    cerr << "call Rhythm::show" << endl;
  
  rhythm.show();
  
  if (Rhythm::vverbose)
    cerr << "call QApplication::exec" << endl;

  return app.exec();
  
}
catch (Error& error) {
  cerr << "rhythm: exception caught:" << error << endl;
}
catch (...) {
  cerr << "rhythm: fatal exception unhandled" << endl;
}


// /////////////////////////////////////////////////////////////////////
// Class definitions
// /////////////////////////////////////////////////////////////////////


Rhythm::Rhythm (QApplication* master, QWidget* parent, int argc, char** argv) :
  QMainWindow (parent, "Rhythm"),
  opts (0, "Options")
{
  // Initialise variables

  myapp = master;
  
  myapp->setStyle( new QPlatinumStyle() );
  myapp->setPalette( QPalette(Qt::darkBlue, Qt::darkCyan) );

  xq = toaPlot::TOA_MJD;
  yq = toaPlot::ResidualMicro;

  toas.resize(0);
  toas_modified = false;

  mode = 1;

  dataPath = ".";

  autofit = false;
  weights = false;
  track = false;
  ignore_one_eph = false;
  
  // Instantiate a box to hold all the stuff

  QHBox* container = new QHBox(this);
  container -> setFocus();
  setCentralWidget(container);

  // Build the cursor control panel

  leftpanel = new QVBox(container);
  
  // Create a tab widget to hold different displays
  tabs = new QTabWidget(container);
  tabs->setMinimumWidth(400);

  // Instantiate the plotting window

  if (vverbose)
    cerr << "Rhythm:: creating toaPlotter" << endl;

  plot_window = new toaPlot(tabs);

  if (vverbose)
    cerr << "Rhythm:: adding toaPlotter to QTabWidget" << endl;

  tabs->addTab(plot_window, "Plot");

  QObject::connect(plot_window, SIGNAL(ineednewdata()),
		   this, SLOT(request_update()));

  show_list = true;

  if (show_list) {

    if (vverbose)
      cerr << "Rhythm:: constructing QListView" << endl;

    lister = new QListView(tabs);
    tabs->addTab(lister, "List");

    QObject::connect(lister, SIGNAL(selectionChanged()),
		     this, SLOT(scan_selected()));

    lister->setSelectionMode(QListView::Extended);
    lister->addColumn("Arrival Time", 130);
    lister->addColumn("Frequency", 100);
    lister->addColumn("Information", 175);
  }

  if (vverbose)
    cerr << "Rhythm:: constructing header" << endl;

  header = new QLabel(leftpanel);
  header->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  header->setAlignment(Qt::AlignCenter);
  header->setPaletteBackgroundColor(black);
 
  if (vverbose)
    cerr << "Rhythm:: getenv PSRHOME" << endl;

  char* psrhome = getenv("PSRHOME");
  if (psrhome) {
    string banloc = psrhome;
    banloc += "/runtime/rhythm/banner.jpg";
    header->setPixmap( QPixmap(banloc.c_str()) );
  }

  if (vverbose)
    cerr << "Rhythm:: constructing footer" << endl;

  footer = new QLabel("Status Message Box", leftpanel);
  footer->setMinimumHeight(60);
  footer->setMargin(5);
  footer->setAlignment(Qt::AlignCenter);
  footer->setFont (QFont( "Times", 10, QFont::Bold));

  bottompanel = new QHBox(leftpanel);

  controls = new QVBox(bottompanel);
  
  modechanger = new QButtonGroup(3, Qt::Vertical, "Mode", controls);
  modechanger -> setRadioButtonExclusive(true);
  
  zoom = new QRadioButton("Zoom", modechanger);
  sel = new QRadioButton("Select", modechanger);

  zoom->setChecked(true);
  
  modechanger->insert(zoom,1);
  modechanger->insert(sel,2);
  
  QObject::connect(modechanger, SIGNAL(clicked(int)),
		   this, SLOT(change_mode(int)));

  point = new QPushButton("Point", controls);
  QObject::connect(point, SIGNAL(clicked()),
		   this, SLOT(point_slot()));
  xrange = new QPushButton("X-Range", controls);
  QObject::connect(xrange, SIGNAL(clicked()),
		   this, SLOT(xrange_slot()));
  yrange = new QPushButton("Y-Range", controls);  
  QObject::connect(yrange, SIGNAL(clicked()),
		   this, SLOT(yrange_slot()));
  box = new QPushButton("Box", controls);
  QObject::connect(box, SIGNAL(clicked()),
		   this, SLOT(box_slot()));
  autoscl = new QPushButton("Autoscale Axes", controls);
  QObject::connect(autoscl, SIGNAL(clicked()),
		   plot_window, SLOT(autoscale()));
  autobin = new QPushButton("Auto Bin", controls);
  QObject::connect(autobin, SIGNAL(clicked()),
		   this, SLOT(autobin_ask()));
  clearsel = new QPushButton("Clear Selected", controls);
  QObject::connect(clearsel, SIGNAL(clicked()),
		   this, SLOT(clearselection()));
  cut = new QPushButton("Delete Selected", controls);
  QObject::connect(cut, SIGNAL(clicked()),
		   this, SLOT(deleteselection()));
  undel = new QPushButton("Restore Deleted", controls);
  QObject::connect(undel, SIGNAL(clicked()),
		   this, SLOT(undeleteall()));
  show_button = new QPushButton("Show Profile", controls);
  QObject::connect(show_button, SIGNAL(clicked()),
		   this, SLOT(show_me()));
  freqs = new QPushButton("Freq Sort", controls);
  QObject::connect(freqs, SIGNAL(clicked()),
		   this, SLOT(freqsort()));
  colour = new QPushButton("Change Colour", controls);
  QObject::connect(colour, SIGNAL(clicked()),
		   this, SLOT(colour_selector()));
  dotify = new QPushButton("Change Symbol", controls);
  QObject::connect(dotify, SIGNAL(clicked()),
		   this, SLOT(symbol_selector()));


  // Instantiate the Axis selection panels

  chooser = new AxisSelector(bottompanel);

  QObject::connect(chooser, SIGNAL(YChange(toaPlot::AxisQuantity)),
		   this, SLOT(YChange(toaPlot::AxisQuantity)));

  QObject::connect(chooser, SIGNAL(XChange(toaPlot::AxisQuantity)),
		   this, SLOT(XChange(toaPlot::AxisQuantity)));

  QObject::connect(plot_window, SIGNAL(selected(int)),
		   this, SLOT(select(int)));

  QObject::connect(plot_window, SIGNAL(selected(std::vector<int>)),
		   this, SLOT(select(std::vector<int>)));
  
  if (vverbose)
    cerr << "Rhythm:: new qt_editParams" << endl;
  
  fitpopup = new qt_editParams();
  
  connect ( fitpopup, SIGNAL( closed() ),
	    this, SLOT( togledit() ) );
  connect ( fitpopup, SIGNAL( newParams(const psrephem&) ),
	    this, SLOT( set_Params(const psrephem&) ) );
  
  fitpopup->show();
  
  if (vverbose)
    cerr << "Rhythm:: call menubarConstruct" << endl;

  menubarConstruct(); 

  if (vverbose)
    cerr << "Rhythm:: call command_line_parse" << endl;

  command_line_parse (argc, argv);


  // Find standard profiles
  
  char temp[128];
  FILE* fptr = popen("ls -1 *.std", "r");
  if (ferror(fptr)==0) {
    while(fscanf(fptr, "%s\n", temp) == 1) {
      try {
	the_stds.push_back(new Pulsar::Profile(Pulsar::Archive::load(temp)
					       ->total()->get_Profile(0,0,0)));
      }
      catch (Error& error) {
	cerr << "Could not open " << temp << endl;
      }
    }
  }

  pclose(fptr);

  if (fitpopup->hasdata() && toas.size() > 1) {
    fitpopup->fitnone();
    fit();
  }
}

Rhythm::~Rhythm ()
{
}

void Rhythm::load_toas (const char* fname)
{
  QString str;

  std::vector<Tempo::toa> input;
  
  Tempo::toa::load (fname, &input);
  
  if (input.size() <= 0) {
    str = "No TOAs found in file";
    footer->setText(str);
    return;
  }

  bool first_load = false;

  if (toas.empty())
    first_load = true;

  add_toas(input);

  if (first_load) {
    
    if (toas[0].get_format() == Tempo::toa::Command) {
      char junk[80];
      int  themode = 0;
      sscanf((toas[0].get_auxilliary_text()).c_str(), 
	     "%s %d", junk, &themode);
      if (strcmp(junk, "MODE") == 0)
	if (themode == 1)
	  toglweights();
    }
    
    update_mode();
    
    tempo->setItemEnabled (fitID, true);
    tempo->setItemEnabled (fitSelID, true);
    tempo->setItemEnabled (strideFitID, true);
    
    toa_filename = fname;
  }
  else {
    toas_modified = true;
  }
}

void Rhythm::add_toas (std::vector<Tempo::toa> new_toas)
{
  QString str;

  for (unsigned i = 0; i < new_toas.size(); i++)
    toas.push_back(new_toas[i]);

  str = "Added " + tostring (toas.size()) + " TOA's";
  
  footer->setText(str);
  
  char* useful = new char[4096];
  MJD thetime;
  
  for (unsigned i = 0; i < toas.size(); i++) {
    
    if (toas[i].get_format() == Tempo::toa::Command) {
      toas[i].ci = 4;
      toas[i].di = 4;
      sscanf((toas[i].get_auxilliary_text()).c_str(), "%s", useful);
      if (strcmp(useful, "JUMP") == 0) {
	MJD prev;
	MJD next;
	if (i == 0)
	  prev = next = toas[1].get_arrival();
	else if (i == toas.size()-1)
	  prev = next = toas[toas.size()-2].get_arrival();
	else {
	  prev = toas[i-1].get_arrival();
	  next = toas[i+1].get_arrival();
	}
	toas[i].set_arrival((prev+next)/2.0);
	cerr << "Jump detected at " 
	     << toas[i].get_arrival().printdays(9) << endl;
      }
    }
    
    thetime = toas[i].get_arrival();
    sprintf(useful, "%f", toas[i].get_frequency());
    ltoas.push_back(new QListViewItem(lister,
				      thetime.printdays(9).c_str(), 
				      useful,
				      toas[i].get_auxilliary_text().c_str()));
  }
  
  if (autofit)
    fit ();
}

void Rhythm::save_toas (const char* fname)
{
  if (verbose)
    cerr << "Saving TOAs to '" << fname << "' ...";

  Tempo::toa::unload(fname, toas);

  toa_filename = fname;

  if (verbose)
    cerr << " done." << endl;

  QString str = "TOA list saved";
  
  footer->setText(str);

  toas_modified = false;
}

void Rhythm::hc ()
{
  plot_window->drawPlot("rhythm.ps/ps");
}

void Rhythm::chc ()
{
  plot_window->drawPlot("rhythm.ps/cps");
}

void Rhythm::set_Params (const psrephem& eph)
{
  tempo->setItemEnabled (saveParmsID, true);

  if (ignore_one_eph) {
    ignore_one_eph = false;
    return;
  }
  if (autofit)
    fit (eph, false);
}

void Rhythm::close_toas ()
{
  if (toas_modified) {
    if (prompt_save_toas() != 0)
      return;
  }
  
  toas.resize(0);

  tempo->setItemEnabled (fitID, false);
  tempo->setItemEnabled (fitSelID, false);
  tempo->setItemEnabled (strideFitID, false);

  goplot();
}

void Rhythm::update_mode ()
{
  if (toas.size() == 0)
    return;
  
  if (toas[0].get_format() == Tempo::toa::Command)
    if ((toas[0].get_auxilliary_text()).find("MODE",0) != string::npos)
      toas.erase(toas.begin());
  
  if (weights) {
    Tempo::toa mode_card(Tempo::toa::Command);
    mode_card.set_auxilliary_text("MODE 1");
    std::vector<Tempo::toa>::iterator it1 = toas.begin();
    toas.insert(it1, 1, mode_card);
  }
}

void Rhythm::show_me ()
{
  cpgopen("/xs");
  plot_current();
  cpgclos();
}

void Rhythm::plot_current ()
{
  int index = -1;
  
  for (unsigned i = 0; i < toas.size(); i++)
    if (toas[i].get_state() == Tempo::toa::Selected) {
      index = i;
      break;
    }
  
  if (index < 0) {
    footer->setText("There is no selected TOA to plot!");
    return;
  }
  
  if (verbose)
    cerr << "The currently selected TOA is # " << index << endl;

  char useful[80];
  char filename[80];

  int chn = 0;
  int sub = 0;

  toas[index].unload(useful);

  if (sscanf(useful+1, "%s %d %d", filename, &sub, &chn) != 3) {
    throw Error(FailedCall, "Information not available");
  }

  if (verbose)
    cerr << "Attempting to load archive '" << filename << "'" << endl;
  
  string useful2 = dataPath + "/";
  useful2 += filename;

  try {
    Reference::To<Pulsar::Archive> data = Pulsar::Archive::load(useful2);
    data->pscrunch();
    data->centre();
    cpgsvp (0.1,0.9,0.1,0.9);
    
    Pulsar::ProfilePlot plotter;
    plotter.set_subint(sub);
    plotter.set_chan(chn);
    new Pulsar::InfoLabel(&plotter);
    plotter.plot(data);
  }
  catch (Error& error) {
    footer->setText("Error processing archives on disk!");
    if (verbose)
      cerr << "Rhythm::show_me ERROR " << error << endl;
  }  
}

void Rhythm::fit()
{
  if (!fitpopup || !fitpopup -> hasdata())
    return;
  
  if (toas.size() < 1) {
    if (verbose)
      cerr << "Rhythm::fit No Arrival Times loaded" << endl;
    return;
  }
  
  footer->setText("Fitting...");
  
  myapp->processEvents();
  
  update_mode();

  psrephem eph;
  fitpopup -> get_psrephem (eph);
  
  fit (eph, true);

  if (verbose)
    cerr << "Rhythm::fit plotting residuals" << endl;

  goplot();
  plot_window->autoscale();
}

void Rhythm::fit (const psrephem& eph, bool load_new)
{ 
  try {
    
    if (toas.size() < 1) {
      if (verbose)
	cerr << "Rhythm::fit No Arrival Times loaded" << endl;
      return;
    }
    
    if (verbose)
      cerr << "Rhythm::fit Calculating residuals" << endl;
    
    psrephem pf_eph;
    
    Tempo::fit (&eph, toas, &pf_eph, track);
    
    if (load_new && fitpopup) {
      // set_psrephem will result in generation of newEph signal, 
      // which should be ignored since it was set from here.
      ignore_one_eph = true;
      
      if (verbose)
	cerr << "Rhythm::fit Displaying new ephemeris" << endl;
      
      fitpopup -> set_psrephem (pf_eph);
    }
    
    char temp[128];
    
    if (!weights) {
      FILE* fptr = popen("tail -1 tempo.lis", "r");
      fgets(temp, 1024, fptr);
      pclose(fptr);
      
      string temp2 = temp;
      string temp3 = "Residual: " + temp2.substr(23, 54);
      
      footer->setText(temp3.c_str());
    }
    else {
      FILE* fptr = popen("tail -2 tempo.lis", "r");
      fgets(temp, 1024, fptr);
      
      string temp2 = temp;
      fgets(temp, 1024, fptr);
      temp2 += temp;
      
      pclose(fptr);
      
      footer->setText(temp2.c_str());
    }
    
  } 
  catch (Error& error) {
    if (verbose)
      cerr << "Rhythm::fit ERROR " << error << endl;
    QMessageBox::critical (this, "Rhythm TOA Fitting Error",
			   "An unexpected exception occured.",
			   "Acknowledge");
  }
  catch (...) {
    if (verbose)
      cerr << "Rhythm::fit ERROR Unhandled Exception" << endl;
    QMessageBox::critical (this, "Rhythm TOA Fitting Error",
			   "An unexpected exception occured.",
			   "Acknowledge");
  }
}

void Rhythm::fit_selected()
{
  if (!fitpopup || !fitpopup -> hasdata())
    return;

  footer->setText("Fitting...");

  myapp->processEvents();

  psrephem eph;
  fitpopup -> get_psrephem (eph);

  fit_selected (eph, true);

  fitpopup -> get_psrephem (eph);
  eph.nofit();

  fit (eph, false);

  if (verbose)
    cerr << "Rhythm::fit_selected plotting residuals" << endl;

  goplot();
  plot_window->autoscale();
}

void Rhythm::fit_selected (const psrephem& eph, bool load_new)
{ 
  try {
    
    if (toas.size() < 1) {
      if (verbose)
	cerr << "Rhythm::fit_selected No Arrival Times loaded" << endl;
      return;
    }
    
    update_mode();

    unsigned tally = 0;
    
    for (unsigned i = 0; i < toas.size(); i++) {
      if (toas[i].get_state() == Tempo::toa::Selected)
	tally++;
    }
    
    if (tally < 2) {
      if (verbose)
	cerr << "Rhythm::fit_selected Not Enough Arrival Times selected" << endl;
      return;
    }
    
    if (verbose)
      cerr << "Rhythm::fit_selected Calculating residuals" << endl;
    
    psrephem pf_eph;
    
    Tempo::fit (&eph, toas, &pf_eph, track, Tempo::toa::Selected);
    
    if (load_new && fitpopup) {
      // set_psrephem will result in generation of newEph signal, 
      // which should be ignored since it was set from here.
      ignore_one_eph = true;
      
      if (verbose)
	cerr << "Rhythm::fit_selected Displaying new ephemeris" << endl;
      
      fitpopup -> set_psrephem (pf_eph);
    }
    
    char temp[128];
    
    FILE* fptr = popen("tail -1 tempo.lis", "r");
    fgets(temp, 1024, fptr);
    pclose(fptr);
    
    string temp2 = temp;
    string temp3 = "Residual: " + temp2.substr(23, 54);

    footer->setText(temp3.c_str());

  } 
  catch (Error& error) {
    if (verbose)
      cerr << "Rhythm::fit_selected ERROR " << error << endl;
    QMessageBox::critical (this, "Rhythm TOA Fitting Error",
			   "An unexpected exception occured.  ",
			   "Acknowledge...");
  }
  catch (...) {
    if (verbose)
      cerr << "Rhythm::fit_selected ERROR Unhandled Exception" << endl;
    QMessageBox::critical (this, "Rhythm TOA Fitting Error",
			   "An unexpected exception occured",
			   "Acknowledge");
  }
}

void Rhythm::stride_fit()
{
  int temp = 0;
  
  temp = QInputDialog::getInteger("Rhythm Stride Fit",
				  "Enter the number of blocks to use: ");
  if (temp <= 1) {
    footer->setText("Invalid number of divisions!");
    return;
  }

  clearselection();
  
  std::vector<double> times;
  for (unsigned i = 0; i < toas.size(); i++) {
    if (toas[i].get_state() == Tempo::toa::Deleted)
      continue;
    times.push_back(toas[i].get_arrival().in_days());
  }
  
  double first = times.front();
  double last  = times.front();
  
  for (unsigned i = 0; i < times.size(); i++) {
    if (times[i] < first)
      first = times[i];
    if (times[i] > last)
      last = times[i];
  }

  double span = last - first;
  double strideval = span / double(temp);
  
  bool found = false;
  int  index = 0;
  
  for (unsigned i = 0; i < EPH_NUM_KEYS; i++) {
    if (fitpopup->query_fit_for(i)) {
      if (found) {
	QMessageBox::critical (this, "Rhythm Stride Fit",
			       "Stride fit only supports a single parameter ");
	return;
      }
      index = i;
      found = true;
    }
  }
  
  if (!found) {
    QMessageBox::critical (this, "Rhythm Stride Fit",
			   "You must select something to fit for! ");
    return;
  }
  
  string useful = "Stride Fitting...";
  footer->setText(useful.c_str());
  
  QProgressDialog progress( "Fitting to data... ", "Abort", temp,
			    this, "progress", TRUE );
  std::vector<double> result;

  for (int i = 0; i < temp; i++) {
    clearselection();
    for (unsigned j = 0; j < toas.size(); j++) {
      if ((toas[j].resid.mjd <= first+((i+1)*strideval)) &&
	  (toas[j].resid.mjd >= first+(i*strideval))) {
	select(j);
      }
    }
    if ( progress.wasCancelled() )
      break;
    progress.setProgress(i);
    myapp->processEvents();
    fit_selected();
    psrephem myeph;
    fitpopup -> get_psrephem (myeph);
    result.push_back(myeph.value_double[index]);
    cout.precision(16);
    cout << "Value for block " << i << " = " 
	 << float(myeph.value_double[index]) << endl;
  }
  clearselection();

  // Display the result
  
  if (result.empty())
    return;
  
  float min = result[0];
  float max = result[0];
  
  for (unsigned i = 0; i < result.size(); i++) {
    if (result[i] < min)
      min = result[i];
    if (result[i] > max)
      max = result[i];
  }
  
  if (min == max) {
    footer->setText("No range in result!");
    return;
  }
  
  cpgopen("9295/xs");
  cpgsvp(0.1, 0.9, 0.1, 0.9);
  cpgswin(0, temp, min, max);
  cpgbox("BCNST", 0.0, 0, "BCNST", 0.0, 0);
  cpglab("Block Number", "Value", "Stride Fit Result");
  for (unsigned i = 0; i < result.size(); i++) {
    cpgpt1(i,result[i],0);
  }
  cpgclos();
}

void Rhythm::advanceT0 () {

  int temp = 0;
  
  temp = QInputDialog::getInteger("Rhythm Advance Epoch of Periastron",
				  "Enter the number of orbits to advance: ");
  if (temp <= 0)
    return;

  int step = 1;

  step = QInputDialog::getInteger("Rhythm Advance Epoch of Periastron",
				 "Enter the step size (in orbits): ");

  if (step <= 0)
    return;

  psrephem eph;

  for (int i = 0; i < (temp/step); i++) {
    
    fitpopup->get_psrephem(eph);
    
    MJD current = MJD (eph.value_integer[EPH_T0],
		       eph.value_double [EPH_T0]);
    
    double period  = eph.value_double[EPH_PB]*24.0*60.0*60.0;
    
    current += period*(double)step;
    
    eph.value_integer[EPH_T0] = current.intday();
    eph.value_double[EPH_T0] = current.fracday();
    
    fitpopup->set_psrephem(eph);
    fit();
  }
}

void Rhythm::advancePEPOCH () {

  int temp = 0;
  
  temp = QInputDialog::getInteger("Rhythm Advance Frequency Epoch",
				  "Enter the number of days to advance: ");
  if (temp <= 0)
    return;

  int step = 1;

  step = QInputDialog::getInteger("Rhythm Advance Frequency Epoch",
				 "Enter the step size (in days): ");

  if (step <= 0)
    return;

  psrephem eph;

  for (int i = 0; i < (temp/step); i++) {
    
    fitpopup->get_psrephem(eph);
    
    MJD current = MJD (eph.value_integer[EPH_PEPOCH],
		       eph.value_double [EPH_PEPOCH]);
    
    current += (double)step * 86400.0;
    
    eph.value_integer[EPH_PEPOCH] = current.intday();
    eph.value_double[EPH_PEPOCH] = current.fracday();
    
    fitpopup->set_psrephem(eph);
    fit();
  }
}

void Rhythm::setClassVerbose (bool verbose)
{
  qt_editParams::verbose = verbose;
  Tempo::verbose = verbose;
  Tempo::toa::verbose = verbose;
}

std::vector<double> Rhythm::give_me_data (toaPlot::AxisQuantity q)
{
  std::vector<double> retval;
  
  char useful[80];
  char filename[80];
  int chn = 0;
  int sub = 0;
  
  Pulsar::StandardSNR snrobj;

  QProgressDialog progress( "Calculating...", "Abort", toas.size(),
			    this, "progress", TRUE );
  
  switch (q) {

  case toaPlot::TOA_MJD:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      //progress.setProgress( i );
      //myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      retval.push_back((toas[i].resid.mjd)-50000.0);
    }
    //progress.setProgress( toas.size() );
    
    return retval;
    break;

  case toaPlot::BinaryPhase:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      //progress.setProgress( i );
      //myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      retval.push_back(toas[i].resid.binaryphase);
    }
    //progress.setProgress( toas.size() );
    
    return retval;
    break;

  case toaPlot::ObsFreq:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      //progress.setProgress( i );
      //myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      retval.push_back(toas[i].resid.obsfreq);
    }
    //progress.setProgress( toas.size() );
    
    return retval;
    break;
    
  case toaPlot::DayOfYear:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      //progress.setProgress( i );
      //myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      char* tempstr = new char[8];
      int tempint = 0;
      toas[i].get_arrival().datestr(tempstr, 8, "%j");
      if (sscanf(tempstr, "%d", &tempint) != 1)
	tempint = 999;
      retval.push_back(double(tempint));
      delete[] tempstr;
    }
    //progress.setProgress( toas.size() );
    
    return retval;
    break;

  case toaPlot::ResidualMicro:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      //progress.setProgress( i );
      //myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      retval.push_back(toas[i].resid.time);
    }
    //progress.setProgress( toas.size() );
    
    return retval;
    break;
    
  case toaPlot::ParallacticAngle:
    if (fitpopup->hasdata()) {

      for ( unsigned i = 0; i < toas.size(); i++ ) {

	progress.setProgress( i );
	myapp->processEvents();
	
	if ( progress.wasCancelled() )
	  break;
	
	if (toas[i].get_format() == Tempo::toa::Command) {
	  retval.push_back(0.0);
	  continue;
	}
	
	if (toas[i].get_pa() != Tempo::toa::UNSET) {
	  retval.push_back(toas[i].get_pa());
	  continue;
	}
	
	// Extract the time
	MJD mjd(toas[i].resid.mjd);
	
	toas[i].unload(useful);
	
	if (sscanf(useful+1, "%s ", filename) != 1)
	  throw Error(FailedCall, "No archive-derived info available");

	if (verbose)
	  cerr << "Attempting to load archive '" << filename << "'" << endl;
	  
	string useful2 = dataPath + "/";
	useful2 += filename;

	double answer = Tempo::toa::UNSET;

	try {

	  Reference::To<Pulsar::Archive> data = Pulsar::Archive::load(useful2);
	  Pulsar::Telescope* telescope = data->get<Pulsar::Telescope>();
	  if (!telescope)
	    cerr << "" << endl;

	  Horizon horizon;

	  horizon.set_epoch( mjd );
	  horizon.set_source_coordinates( data->get_coordinates() );
	  horizon.set_observatory_latitude
	    ( telescope->get_latitude().getRadians() );
	  horizon.set_observatory_longitude
	    ( telescope->get_longitude().getRadians() );

	  answer = horizon.get_parallactic_angle() * 180/M_PI;

	}
	catch (Error& error) {
	  if (verbose)
	    cerr << error << endl;
	}

	retval.push_back(answer);
	toas[i].set_pa(answer);

      }
      progress.setProgress( toas.size() );
    }
    return retval;
    break;
    
  case toaPlot::ResidualMilliTurns:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      //progress.setProgress( i );
      //myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      retval.push_back((toas[i].resid.turns)*1000.0);
    }
    //progress.setProgress( toas.size() );
    
    return retval;
    break;

  case toaPlot::ErrorMicro:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      //progress.setProgress( i );
      //myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      retval.push_back(toas[i].resid.error);
    }
    //progress.setProgress( toas.size() );
    
    return retval;
    break;
    
  case toaPlot::PointNumber:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      //progress.setProgress( i );
      //myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      retval.push_back(i);
    }
    //progress.setProgress( toas.size() );
    
    return retval;
    break;

  case toaPlot::SignalToNoise:

    for ( unsigned i = 0; i < toas.size(); i++ ) {
      progress.setProgress( i );
      myapp->processEvents();
      
      if ( progress.wasCancelled() )
	break;
      
      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      
      if (toas[i].get_StoN() != Tempo::toa::UNSET) {
	retval.push_back(toas[i].get_StoN());
	continue;
      }
      
      toas[i].unload(useful);
      
      if (sscanf(useful+1, "%s %d %d", filename, &sub, &chn) != 3) {
	throw Error(FailedCall, "Information not available");
      }
      else {
	if (verbose)
	  cerr << "Attempting to load archive '" << filename << "'" << endl;
	
	string useful2 = dataPath + "/";
	useful2 += filename;
	
	try {
	  Reference::To<Pulsar::Archive> data = Pulsar::Archive::load(useful2);
	  data->pscrunch();
	  Pulsar::Profile* stdprof = 0;
	  try { stdprof = Pulsar::find_standard(data, the_stds);
	  } catch (Error& error) {}
	  if (stdprof) {
	    snrobj.set_standard(stdprof);
	    toas[i].set_StoN(snrobj.get_snr(data->get_Profile(sub,0,chn)));
	  }
	  else {
	    toas[i].set_StoN(data->get_Profile(sub,0,chn)->snr());
	  }
	  retval.push_back(toas[i].get_StoN());
	}
	catch (Error& error) {
	  footer->setText("Error processing archives on disk!");
	  if (verbose)
	    cerr << error << endl;
	  retval.resize(0);
	  break;
	}
      }
    }
    progress.setProgress( toas.size() );
    
    return retval;
    break;
    
  case toaPlot::Bandwidth:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      progress.setProgress( i );
      myapp->processEvents();
      
      if ( progress.wasCancelled() )
	break;
      
      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      
      if (toas[i].get_bw() != Tempo::toa::UNSET) {
	retval.push_back(toas[i].get_bw());
	continue;
      }
      
      toas[i].unload(useful);
      
      if (sscanf(useful+1, "%s ", filename) != 1) {
	throw Error(FailedCall, "No archive-derived info available");
      }
      else {
	if (verbose)
	  cerr << "Attempting to load archive '" << filename << "'" << endl;
	
	string useful2 = dataPath + "/";
	useful2 += filename;
	
	try {
	  Reference::To<Pulsar::Archive> data = Pulsar::Archive::load(useful2);
	  toas[i].set_bw(data->get_bandwidth());
	  retval.push_back(toas[i].get_bw());     
	}
	catch (Error& error) {
	  footer->setText("Error processing archives on disk!");
	  if (verbose)
	    cerr << error << endl;
	  retval.resize(0);
	  break;
	}
      }
    }
    progress.setProgress( toas.size() );
    
    return retval;
    break;
    
  case toaPlot::DispersionMeasure:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      progress.setProgress( i );
      myapp->processEvents();

      if ( progress.wasCancelled() )
	break;

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      
      if (toas[i].get_dm() != Tempo::toa::UNSET) {
	retval.push_back(toas[i].get_dm());
	continue;
      }
      
      toas[i].unload(useful);
      
      if (sscanf(useful+1, "%s ", filename) != 1) {
	throw Error(FailedCall, "No archive-derived info available");
      }
      else {
	if (verbose)
	  cerr << "Attempting to load archive '" << filename << "'" << endl;
	
	string useful2 = dataPath + "/";
	useful2 += filename;
	
	try {
	  Reference::To<Pulsar::Archive> data = Pulsar::Archive::load(useful2);
	  toas[i].set_dm(data->get_dispersion_measure());
	  retval.push_back(toas[i].get_dm());
	}
	catch (Error& error) {
	  footer->setText("Error processing archives on disk!");
	  if (verbose)
	    cerr << error << endl;
	  retval.resize(0);
	  break;
	}
      }
    }
    progress.setProgress( toas.size() );
    
    return retval;
    break;

  case toaPlot::Duration:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      progress.setProgress( i );
      myapp->processEvents();

      if ( progress.wasCancelled() )
	break;
      
      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      
      if (toas[i].get_dur() != Tempo::toa::UNSET) {
	retval.push_back(toas[i].get_dur());
	continue;
      }
      
      toas[i].unload(useful);

      if (sscanf(useful+1, "%s %d %d", filename, &sub, &chn) != 3) {
	throw Error(FailedCall, "Information not available");
      }
      else {
      	if (verbose)
	  cerr << "Attempting to load archive '" << filename << "'" << endl;
	
	string useful2 = dataPath + "/";
	useful2 += filename;
	
	try {
	  Reference::To<Pulsar::Archive> data = Pulsar::Archive::load(useful2);
	  toas[i].set_dur(data->get_Integration(sub)->get_duration());
	  retval.push_back(toas[i].get_dur());     
	}
	catch (Error& error) {
	  footer->setText("Error processing archives on disk!");
	  if (verbose)
	    cerr << error << endl;
	  retval.resize(0);
	  break;
	}
      }
    }
    progress.setProgress( toas.size() );
    
    return retval;
    break;

  default:
    return retval;
    break;
  }
}
 
std::vector<double> Rhythm::give_me_errs (toaPlot::AxisQuantity q)
{
  std::vector<double> retval;
  
  // This may require adjustment! Find out what the error
  // is stored as in Willem's residual class...

  switch (q) {

  case toaPlot::TOA_MJD:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;

  case toaPlot::ResidualMicro:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(toas[i].resid.error);
    return retval;
    break;

  case toaPlot::PointNumber:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;
    
  case toaPlot::ResidualMilliTurns:
    // Need the pulsar period:
    if (fitpopup->hasdata()) {
      psrephem myeph;
      fitpopup -> get_psrephem (myeph);
      double period = myeph.p();
      period *= 1000.0;
      for (unsigned i = 0; i < toas.size(); i++)
	retval.push_back(toas[i].resid.error / period);
      return retval;
    }
    else {
      for (unsigned i = 0; i < toas.size(); i++)
	retval.push_back(0.0);
      return retval;
    }
    break;

  case toaPlot::BinaryPhase:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;

  case toaPlot::ObsFreq:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;
    
  case toaPlot::ParallacticAngle:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;

  case toaPlot::DayOfYear:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;

  case toaPlot::ErrorMicro:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;

  case toaPlot::SignalToNoise:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;

  case toaPlot::Bandwidth:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;

  case toaPlot::DispersionMeasure:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;

  case toaPlot::Duration:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;

  default:
    return retval;
    break;
  }
}

void Rhythm::XChange (toaPlot::AxisQuantity q)
{
  xq = q;
  goplot ();
  plot_window->autoscale();
}

void Rhythm::YChange (toaPlot::AxisQuantity q)
{
  yq = q;
  goplot ();
  plot_window->autoscale();
}

void Rhythm::goplot ()
{
  std::vector<double> tempx = give_me_data(xq);
  std::vector<double> tempy = give_me_data(yq);
  std::vector<double> xerrs = give_me_errs(xq);
  std::vector<double> yerrs = give_me_errs(yq);
  
  if ( tempx.size() != toas.size() || tempy.size() != toas.size() ||
       xerrs.size() != toas.size() || yerrs.size() != toas.size() ) {
    footer->setText("Computation incomplete!");
    return;
  }
  
  std::vector<wrapper> useme;
  
  for (unsigned i = 0; i < toas.size(); i++) {
    
    if (toas[i].get_state() == Tempo::toa::Deleted)
      continue;
    
    if (toas[i].get_format() == Tempo::toa::Command)
      continue;
    
    wrapper tempw;
    
    tempw.x = tempx[i];
    tempw.y = tempy[i];
    tempw.ex = xerrs[i];
    tempw.ey = yerrs[i];
    tempw.id = i;
    
    tempw.dot = toas[i].di;
    
    if (toas[i].get_state() == Tempo::toa::Selected)
      tempw.ci = 2;
    else
      tempw.ci = toas[i].ci;
    
    useme.push_back(tempw);
  }
  plot_window->setPoints(xq, yq, useme, 
			 chooser->isLogX(), chooser->isLogY());
}

void Rhythm::deselect (int pt)
{
  if (pt >= int(toas.size()))
    return;
  if (pt < 0)
    return;
  
  if (toas[pt].get_state() == Tempo::toa::Deleted)
    return;
  
  toas[pt].set_state(Tempo::toa::Normal);
  lister->setSelected(ltoas[pt], false);
}

void Rhythm::deselect (std::vector<int> pts)
{
  QProgressDialog progress( "De-Selecting Points...", "Abort", pts.size(),
			    this, "progress", TRUE );
  for ( unsigned i = 0; i < pts.size(); i++ ) {
    progress.setProgress( i );
    myapp->processEvents();
    
    if ( progress.wasCancelled() )
      break;
    
    deselect(pts[i]);
  }
  progress.setProgress( pts.size() );
}

void Rhythm::request_update ()
{
  goplot ();
}

void Rhythm::select (int pt)
{
  int size = toas.size();
  
  if (pt >= size)
    return;
  if (pt < 0)
    return;
  
  if (toas[pt].get_state() == Tempo::toa::Deleted)
    return;
  
  toas[pt].set_state(Tempo::toa::Selected);
  lister->setSelected(ltoas[pt], true);
}

void Rhythm::select (std::vector<int> pts)
{
  QProgressDialog progress( "Selecting Points...", "Abort", pts.size(),
			    this, "progress", TRUE );
  for ( unsigned i = 0; i < pts.size(); i++ ) {
    progress.setProgress( i );
    myapp->processEvents();
    
    if ( progress.wasCancelled() )
      break;
    
    select(pts[i]);
  }
  progress.setProgress( pts.size() );
  
}

void Rhythm::autobin_ask ()
{
  int temp = -1;
  
  temp = QInputDialog::getInteger("Rhythm",
				  "Please enter the number of bins:"); 
  if (temp >= 0)
    plot_window->autobin(temp);
}

void Rhythm::freqsort ()
{
  for (unsigned i = 0; i < toas.size(); i++) {
    if (toas[i].resid.obsfreq != 0.0) {
      if (toas[i].resid.obsfreq < 500.0)
	toas[i].ci = 3;
      else if (toas[i].resid.obsfreq < 800.0)
	toas[i].ci = 4;
      else if (toas[i].resid.obsfreq < 1400.0)
	toas[i].ci = 5;
      else if (toas[i].resid.obsfreq < 2400.0)
	toas[i].ci = 6;
      else if (toas[i].resid.obsfreq < 5500.0)
	toas[i].ci = 8;
      else if (toas[i].resid.obsfreq < 9000.0)
	toas[i].ci = 9;
      else
	toas[i].ci = 10;
    }
    else if (toas[i].get_frequency() != 0.0) {
      if (toas[i].get_frequency() < 500.0)
	toas[i].ci = 3;
      else if (toas[i].get_frequency() < 800.0)
	toas[i].ci = 4;
      else if (toas[i].get_frequency() < 1400.0)
	toas[i].ci = 5;
      else if (toas[i].get_frequency() < 2400.0)
	toas[i].ci = 6;
      else if (toas[i].get_frequency() < 5500.0)
	toas[i].ci = 8;
      else if (toas[i].get_frequency() < 9000.0)
	toas[i].ci = 9;
      else
	toas[i].ci = 10;
    }
  }
}

void Rhythm::change_mode (int m)
{
  mode = m;
  plot_window->ptselector();
}

void Rhythm::xrange_slot ()
{
  switch (mode) {
  case 1: // Zoom
    plot_window->xzoomer();
    break;
  case 2: // Select
    plot_window->xselector();
    break;
  }
}

void Rhythm::yrange_slot ()
{
  switch (mode) {
  case 1: // Zoom
    plot_window->yzoomer();
    break;
  case 2: // Select
    plot_window->yselector();
    break;
  }
}

void Rhythm::point_slot ()
{
  switch (mode) {
  case 1: // Zoom
    // Do nothing here...
    break;
  case 2: // Select
    plot_window->ptselector();
    break;
  case 3: // Cut
    break;
  }
}

void Rhythm::box_slot ()
{
  switch (mode) {
  case 1: // Zoom
    plot_window->boxzoomer();
    break;
  case 2: // Select
    plot_window->boxselector();
    break;
  }
}

void Rhythm::deleteselection ()
{
  QProgressDialog progress( "Deleting Points...", "Abort", toas.size(),
			    this, "progress", TRUE );
  for ( unsigned i = 0; i < toas.size(); i++ ) {
    progress.setProgress( i );
    myapp->processEvents();
    
    if ( progress.wasCancelled() )
      break;
    
    if (toas[i].get_state() == Tempo::toa::Selected)
      toas[i].set_state(Tempo::toa::Deleted);
  }
  progress.setProgress( toas.size() );
  
  toas_modified = true;
  
  goplot ();
  plot_window->autoscale();
}

void Rhythm::scan_selected()
{
  for (unsigned i = 0; i < toas.size(); i++) {
    if (toas[i].get_state() != Tempo::toa::Deleted)
      if (ltoas[i]->isSelected())
	toas[i].set_state(Tempo::toa::Selected);
      else
	toas[i].set_state(Tempo::toa::Normal);
  }
}

void Rhythm::undeleteall ()
{
  QProgressDialog progress( "Restoring Deleting Points...", 
			    "Abort", toas.size(),
			    this, "progress", TRUE );
  for ( unsigned i = 0; i < toas.size(); i++ ) {
    progress.setProgress( i );
    myapp->processEvents();
    
    if ( progress.wasCancelled() )
      break;
    
    if (toas[i].get_state() == Tempo::toa::Deleted) {
      if (toas[i].get_format() != Tempo::toa::Comment)
	toas[i].set_state(Tempo::toa::Normal);
    }
  }
  progress.setProgress( toas.size() );
  
  goplot ();
  plot_window->autoscale();
}


void Rhythm::clearselection ()
{
  QProgressDialog progress( "Clearing Selection List...", "Abort", toas.size(),
			    this, "progress", TRUE );
  for ( unsigned i = 0; i < toas.size(); i++ ) {
    progress.setProgress( i );
    myapp->processEvents();
    
    if ( progress.wasCancelled() )
      break;
    
    if (toas[i].get_state() == Tempo::toa::Deleted)
      continue;
    
    toas[i].set_state(Tempo::toa::Normal);
    lister->setSelected(ltoas[i], false);
  }
  progress.setProgress( toas.size() );
  
  goplot ();
}

void Rhythm::colour_selector ()
{
  int temp = -1;
  
  temp = QInputDialog::getInteger("Rhythm",
				  "Please enter the colour index: (0-15)");
  if (temp >= 0 && temp < 16)
    setselcol (temp);
}

void Rhythm::setselcol (int index)
{
  for (unsigned i = 0; i < toas.size(); i++) {
    
    if (toas[i].get_state() == Tempo::toa::Selected)
      toas[i].ci = index;
    
  }
  goplot ();
}

void Rhythm::symbol_selector ()
{
  int temp = -1;
  
  temp = QInputDialog::getInteger("Rhythm",
				  "Please enter the symbol index: (0-127)");
  
  if (temp >= 0 && temp < 128)
    setseldot (temp);
}

void Rhythm::setseldot (int index)
{
  for (unsigned i = 0; i < toas.size(); i++) {
    
    if (toas[i].get_state() == Tempo::toa::Selected)
      toas[i].di = index;
    
  }
  goplot ();
}


void Rhythm::simulateModel()
{
  if (toas.empty()) {
    footer->setText("Cannot simulate: no TOAS loaded");
    return;
  }
  
  if (!fitpopup || !fitpopup -> hasdata()) {
    footer->setText("Cannot simulate: no model loaded");
    return;
  }
  
  fit();

  std::vector<Tempo::toa> fake;
  fake.resize(toas.size());

  for (unsigned i = 0; i < toas.size(); i++) {
    fake[i] = toas[i];
    fake[i].set_arrival(toas[i].get_arrival()-(toas[i].resid.time/1000000.0));
  }
  
  QString fileName (QFileDialog::getSaveFileName ( "perfect.tim", "*.tim", this ));
  
  if ( fileName.isNull() )
    return;
  
  Tempo::toa::unload(fileName.ascii(), fake);
  
}

AxisSelector::AxisSelector (QWidget* parent)
  : QHBox(parent)
{
  Xgrp = new QButtonGroup(14, Qt::Vertical, "X Axis", this);
  Xgrp -> setRadioButtonExclusive(true);
  
  Ygrp = new QButtonGroup(14, Qt::Vertical, "Y Axis", this);
  Ygrp -> setRadioButtonExclusive(true);

  X1 = new QRadioButton("Residual (us)", Xgrp);
  X2 = new QRadioButton("Residual (mt)", Xgrp);
  X3 = new QRadioButton("TOA (MJD)", Xgrp);
  X4 = new QRadioButton("Binary Phase", Xgrp);
  X5 = new QRadioButton("Obs Freq", Xgrp);
  X6 = new QRadioButton("Day of Year", Xgrp);
  X7 = new QRadioButton("Timing Error", Xgrp);
  X8 = new QRadioButton("Signal / Noise", Xgrp);
  X9 = new QRadioButton("Bandwidth", Xgrp);
  X10 = new QRadioButton("DM", Xgrp);
  X11 = new QRadioButton("Length", Xgrp);
  X12 = new QRadioButton("P.A.", Xgrp);
  X13 = new QRadioButton("TOA Index", Xgrp);
  Xlog = new QCheckBox("Log Scale", Xgrp);
  
  X3->setChecked(true);

  Y1 = new QRadioButton("Residual (us)", Ygrp);
  Y2 = new QRadioButton("Residual (mt)", Ygrp);
  Y3 = new QRadioButton("TOA (MJD)", Ygrp);
  Y4 = new QRadioButton("Binary Phase", Ygrp);
  Y5 = new QRadioButton("Obs Freq", Ygrp);
  Y6 = new QRadioButton("Day of Year", Ygrp);
  Y7 = new QRadioButton("Timing Error", Ygrp);
  Y8 = new QRadioButton("Signal / Noise", Ygrp);
  Y9 = new QRadioButton("Bandwidth", Ygrp);
  Y10 = new QRadioButton("DM", Ygrp);
  Y11 = new QRadioButton("Length", Ygrp);
  Y12 = new QRadioButton("P.A.", Ygrp);
  Y13 = new QRadioButton("TOA Index", Ygrp);
  Ylog = new QCheckBox("Log Scale", Ygrp);

  Y1->setChecked(true);
  
  Xgrp->insert(X1,1);
  Xgrp->insert(X2,2);
  Xgrp->insert(X3,3);
  Xgrp->insert(X4,4);
  Xgrp->insert(X5,5);
  Xgrp->insert(X6,6);
  Xgrp->insert(X7,7);
  Xgrp->insert(X8,8);
  Xgrp->insert(X9,9);
  Xgrp->insert(X10,10);
  Xgrp->insert(X11,11);
  Xgrp->insert(X12,12);
  Xgrp->insert(X13,13);
  Xgrp->insert(Xlog,14);

  Ygrp->insert(Y1,1);
  Ygrp->insert(Y2,2);
  Ygrp->insert(Y3,3);
  Ygrp->insert(Y4,4);
  Ygrp->insert(Y5,5);
  Ygrp->insert(Y6,6);
  Ygrp->insert(Y7,7);
  Ygrp->insert(Y8,8);
  Ygrp->insert(Y9,9);
  Ygrp->insert(Y10,10);
  Ygrp->insert(Y11,11);
  Ygrp->insert(Y12,12);
  Ygrp->insert(Y13,13);
  Ygrp->insert(Ylog,14);

  QObject::connect(Xgrp, SIGNAL(clicked(int)),
		   this, SLOT(Xuseful(int)));
  
  QObject::connect(Ygrp, SIGNAL(clicked(int)),
		   this, SLOT(Yuseful(int)));
}

void AxisSelector::Xuseful(int placeholder)
{
  if (placeholder == 14)
    return;

  emit XChange(toaPlot::AxisQuantity(placeholder));
}

void AxisSelector::Yuseful(int placeholder)
{
  if (placeholder == 14)
    return;

  emit YChange(toaPlot::AxisQuantity(placeholder));
}

bool AxisSelector::isLogX()
{
  return (Xlog->isChecked());
}

bool AxisSelector::isLogY()
{
  return (Ylog->isChecked());
}
