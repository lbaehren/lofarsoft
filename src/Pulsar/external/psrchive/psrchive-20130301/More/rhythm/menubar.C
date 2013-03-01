/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "rhythm.h"
#include "qt_fileParams.h"
#include "qt_editParams.h"
#include "tempo++.h"
#include "strutil.h"

#include <qapplication.h>
#include <qpopupmenu.h> 
#include <qmenubar.h> 
#include <qmessagebox.h>
#include <qfiledialog.h>

#include <cpgplot.h>

#include <iostream>

using namespace std;

void Rhythm::menubarConstruct ()
{
  menuBar() -> setFixedHeight ( menuBar()->fontMetrics().height() * 2 );

  // ///////////////////////////////////////////////////////////////////////
  // FILE menu options
  //
  QPopupMenu *file = new QPopupMenu( menuBar() );  CHECK_PTR( file );

  file->insertItem( "&Load TOAs",       this, 
		    SLOT( load_toas() ), ALT+Key_L );
  file->insertItem( "&Save TOAs",       this, 
		    SLOT( save_toas() ) );
  file->insertSeparator();
  file->insertItem( "&Save All",       this, 
		    SLOT( save_all() ), ALT+Key_S );
  file->insertSeparator();
  file->insertItem( "Write Postscript", this, 
		    SLOT( hc() ));
  file->insertItem( "Write Colour PS", this, 
		    SLOT( chc() ));
  file->insertSeparator();
  file->insertItem( "&Close", this, SLOT(close_toas()), ALT+Key_C );
  file->insertItem( "E&xit",  this, SLOT(all_done()), ALT+Key_X );

  // ///////////////////////////////////////////////////////////////////////
  // TEMPO menu options
  //
  tempo = new QPopupMenu( menuBar() );  CHECK_PTR (tempo);

  fitID = tempo->insertItem( "&Fit", this, SLOT( fit() ), CTRL+Key_F);
  tempo->setItemEnabled (fitID, false);
  fitSelID = tempo->insertItem( "Fit &Selected", this, 
				SLOT( fit_selected() ), CTRL+Key_S);
  tempo->setItemEnabled (fitSelID, false);
  strideFitID = tempo->insertItem( "Stride Fit", this, SLOT( stride_fit() ));
  tempo->setItemEnabled (strideFitID, false);
  advanceT0ID = tempo->insertItem( "Advance T0", this, SLOT( advanceT0() ));
  advancePEPOCHID = tempo->insertItem( "Advance PEPOCH", this, 
				       SLOT( advancePEPOCH() ));
  dispID = tempo->insertItem( "Display Parameters", this, SLOT( togledit() ));
  tempo->setItemEnabled (dispID, false);

  tempo->insertSeparator();
  tempo->insertItem( "Load Parameters", fitpopup, SLOT( open() ));
  saveParmsID = tempo->insertItem( "Save Parameters", fitpopup, SLOT(save()));
  tempo->setItemEnabled (saveParmsID, false);

  tempo->insertSeparator();
  // Automatic fitting starts out disabled
  autofitID = tempo->insertItem( "Enable &Auto Fit",
				 this, SLOT( toglauto() ));
  // Weights disabled by default, state changes when TOAs are read in
  weightsID = tempo->insertItem( "Enable &Weights",
				this, SLOT( toglweights() ), CTRL+Key_W);
  // Tracking disabled by default
  trackID = tempo->insertItem( "Enable &Tracking",
			       this, SLOT( togltrack() ), CTRL+Key_T);
  tempo->insertSeparator();
  stdID = tempo->insertItem( "Use Standard TEMPO",
			     this, SLOT( set_std_tempo() ));
  ddkID = tempo->insertItem( "Use TEMPO_DDK",
			     this, SLOT( set_ddk() ));
  temsysID = tempo->insertItem( "Customise Tempo Call",
				this, SLOT( temposys() ));

  // ///////////////////////////////////////////////////////////////////////
  // VERBOSITY menu options
  //
  QPopupMenu* verbosity = new QPopupMenu( menuBar() );
  quietID  = verbosity->insertItem( "Quiet" );
  mediumID = verbosity->insertItem( "Verbose" );
  noisyID  = verbosity->insertItem( "Noisy" );

  verbosity->setItemChecked( quietID, true );
  connect ( verbosity, SIGNAL( activated (int) ),
	    this, SLOT( setVerbosity(int) ) );

  // ///////////////////////////////////////////////////////////////////////
  // PLOT menu options
  //
  QPopupMenu* plotter = new QPopupMenu( menuBar() );  
  plotter->insertItem( "Set Data Path", this, SLOT(setDataPath()));

  // ///////////////////////////////////////////////////////////////////////
  // OPTIONS menu options
  //
  options = new QPopupMenu( menuBar() );
  options->insertItem( "Plotter", plotter, ALT+Key_P);
  options->insertSeparator();
  options->insertItem( "&Verbosity", verbosity, ALT+Key_V);

  // ///////////////////////////////////////////////////////////////////////
  // MOVIE menu options
  //
  QPopupMenu* movies = new QPopupMenu ( menuBar() );
  CHECK_PTR (movies);
  movies->insertItem( "Profile Movie", this, SLOT(profileMovie()));

  // ///////////////////////////////////////////////////////////////////////
  // SIMULATION menu options
  //
  QPopupMenu* sims = new QPopupMenu ( menuBar() );
  CHECK_PTR (sims);
  sims->insertItem( "Simulate Model", this, SLOT(simulateModel()));
  
  // ///////////////////////////////////////////////////////////////////////
  // HELP menu options
  //
  QPopupMenu *help = new QPopupMenu( menuBar() );
  CHECK_PTR (help);
  help->insertItem( "&Usage", this, SLOT(about()), CTRL+Key_H );
  if (isTopLevel())
    help->insertItem( "About Qt", this, SLOT(aboutQt()), 0);
  
  menuBar() -> setSeparator ( QMenuBar::InWindowsStyle );
  
  menuBar() -> insertItem   ( "File",        file );
  menuBar() -> insertItem   ( "Tempo",       tempo );
  menuBar() -> insertItem   ( "Options",     options );
  menuBar() -> insertItem   ( "Movies",      movies );
  menuBar() -> insertItem   ( "Simulation",  sims );
  menuBar() -> insertSeparator();
  menuBar() -> insertItem   ( "Help", help );
  
  if (verbose) cerr << "Rhythm::menubarConstruct () returns\n";
}

void Rhythm::togledit()
{
  if (!fitpopup)
    return;

  if (fitpopup->isVisible()) {
    fitpopup->hide();
    tempo->setItemEnabled (dispID, TRUE);
  }
  else {
    fitpopup->show();
    tempo->setItemEnabled (dispID, FALSE);
  }
}

void Rhythm::temposys()
{
  QString current = "The current tempo system call is:\n\n";
  current += (Tempo::get_system()).c_str();
  current += "\n\nPlease enter the new system call to use:";
  
  QString temp = QInputDialog::getText("Tempo System Call", current);
  
  if (temp.isEmpty())
    return;
  else
    Tempo::set_system(temp.ascii());
}

void Rhythm::set_std_tempo()
{
  Tempo::set_system("tempo");
}

void Rhythm::set_ddk()
{
  Tempo::set_system("tempo_ddk");
}

void Rhythm::setDataPath()
{
  QString temp = QInputDialog::getText("Set Data Path",
                                       "Please enter the archive path:");
  if (!temp.isEmpty())
    dataPath = temp.ascii();
}

void Rhythm::profileMovie()
{
  QProgressDialog progress( "Displaying Profiles...", "Abort", 
			    toas.size(), this, "progress", TRUE );
  cpgopen("9090/xs");
  
  for (unsigned i = 0; i < toas.size(); i++) {
    if (toas[i].get_format() == Tempo::toa::Command)
      continue;
    cpgbbuf();
    cpgeras();
    progress.setProgress( i );
    myapp->processEvents();
    plot_current();
    cpgebuf();
    if ( progress.wasCancelled() )
      break;
  }
  progress.setProgress( toas.size() );
  cpgclos();
}

void Rhythm::toglweights()
{
  QString text;
  if (weights)
    text = "Enable";
  else
    text = "Disable";
  text += " &Weights";

  tempo->changeItem ( weightsID, text );
  weights = !weights;

  if (verbose) {
    cerr << "Rhythm: Weighted Fitting ";
    if (weights)
      cerr << "enabled.\n";
    else
      cerr << "disabled.\n";
    cerr << endl;
  }

  update_mode();
}

void Rhythm::togltrack()
{
  QString text;
  if (track)
    text = "Enable";
  else
    text = "Disable";
  text += " &Tracking";
  
  tempo->changeItem ( trackID, text );
  track = !track;
  
  if (verbose) {
    cerr << "Rhythm: Phase tracking ";
    if (track)
      cerr << "enabled.\n";
    else
      cerr << "disabled.\n";
    cerr << endl;
  }
}

void Rhythm::toglauto()
{
  QString text;
  if (autofit)
    text = "Enable";
  else
    text = "Disable";
  text += " &Auto Fit";

  tempo->changeItem ( autofitID, text );
  autofit = !autofit;

  if (verbose) {
    cerr << "Rhythm: Automatic Fitting ";
    if (autofit)
      cerr << "enabled.\n";
    else
      cerr << "disabled.\n";
    cerr << endl;
  }
}

void Rhythm::about()
{
  QMessageBox::about (NULL, "Rhythm Pulsar Timing",
		      "A graphical user interface to TEMPO.\n\n"
		      "Written by: W. van Straten & A. W. Hotan\n\n"
		      "http://www.astronomy.swin.edu.au/pulsar/software/manuals/rhythm\n");
}

void Rhythm::aboutQt()
{
  QMessageBox::aboutQt (NULL, "Qt Information");
}

void Rhythm::load_toas ()
{  
  QString startName = QString::null;
  if ( !toa_filename.empty() )
    startName = toa_filename.c_str();

  QString fileName (QFileDialog::getOpenFileName ( startName, "*.tim", this ));

  if ( fileName.isNull() )
    return;

  load_toas ( fileName.ascii() );
}

int Rhythm::prompt_save_toas ()
{
  switch( QMessageBox::information( this, "Rhythm",
				    "You have edited the TOA list...\n"
				    "Do you want to save your changes?",
				    "&Save", "&Discard", "Cancel",
				    0,      // Enter == button 0
				    2 ) ) { // Escape == button 2
  case 0: // Save clicked or Alt+S pressed or Enter pressed.
    save_toas();
    return 0;
    break;
  case 1: // Discard clicked or Alt+D pressed
    return 0;
    break;
  case 2: // Cancel clicked or Alt+C pressed or Escape pressed
    return -1;
    break;
  }
  return -1;
}

void Rhythm::save_toas ()
{
  QString startName = QString::null;
  if ( !toa_filename.empty() )
    startName = toa_filename.c_str();

  QString fileName (QFileDialog::getSaveFileName ( startName, "*.tim", this ));

  if ( fileName.isNull() )
    return;

  save_toas ( fileName.ascii() );
}

void Rhythm::save_all ()
{
  string toaName;
  if ( !toa_filename.empty() )
    toaName = toa_filename;
  
  if ( toaName.empty() )
    return;
  
  save_toas ( toaName.c_str() );
  
  string parName = replace_extension (toaName, ".par");
  
  if (fitpopup)
    fitpopup->save(parName);

  QString str = "TOAs and Ephemeris saved";
  
  footer->setText(str);
}

void Rhythm::setVerbosity ( int verbosityID )
{
  options->setItemChecked( quietID,  quietID  == verbosityID);
  options->setItemChecked( mediumID, mediumID == verbosityID);
  options->setItemChecked( noisyID,  noisyID  == verbosityID);

  verbose = (mediumID == verbosityID || noisyID  == verbosityID);
  vverbose = (noisyID == verbosityID);

  setClassVerbose (vverbose);

  if (vverbose)
    cerr << "rhythm: very verbose on" << endl;
  else if (verbose)
    cerr << "rhythm: verbose on" << endl;
}

void Rhythm::all_done ()
{
  if (toas_modified) {
    if (prompt_save_toas() != 0)
      return;
  }

  qApp->quit();
}
