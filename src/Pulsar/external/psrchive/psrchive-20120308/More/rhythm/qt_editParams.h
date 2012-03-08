/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* ///////////////////////////////////////////////////////////////////////
   The Qt editeph Widget
      Manipulate Qt psrephem widget with this gui component

   Author: Willem van Straten 
   /////////////////////////////////////////////////////////////////////// */

#ifndef __QT_EDITEPH_H
#define __QT_EDITEPH_H

#include <vector>

#include <qpopupmenu.h>

#include "qt_psrephem.h"
#include "psrephem.h"
#include "psrParams.h"

using Legacy::psrephem;

class qt_fileParams;

class qt_editParams : public QWidget
{
  Q_OBJECT

 public:
  static bool verbose;
  static int  border_width;  // size of border around widget frame

  qt_editParams (QWidget* parent=0, const char* name=0);

  // load/unload ephemeris from/to filename
  void load   (const char* filename);
  void unload (const char* filename);

  // display the ephemeris given / get the epehemeris displayed
  void set_psrephem (const psrephem& eph);
  void get_psrephem (psrephem& eph);

  // see fitall() and fitnone() slots

  // returns true if the editor currently contains a set of TEMPO 11 parameters
  bool hasdata() { return current != -1; };

 signals:
  // emitted when the user selects 'File/Close'
  void closed ();
  // emitted when new data have been loaded/set
  void newParams ( const psrephem& );

 public slots:
  // File Menu Callbacks
  void open();
  void save();
  void print();
  void closeWin();

  void save (std::string filename);

  // Edit Menu Callbacks
  void forward();          // go forward to newer parameter set
  void backward();         // go back to last parameter set
  void select_parms();     // hides/shows the selection grid
  void fitall();
  void fitnone();

  // Interface functions
  int      get_hist_length();
  psrephem get_historic(unsigned which);
  bool     query_fit_for(int ephind);

  // Help Menu Callbacks
  void about();
  void aboutQt();

 protected:

  // the psr parameters display widget
  qt_psrephem display;

  // the parameters kept in the editor display
  psrephem data;

  // history of parameter sets as the user loads and modifies
  std::vector<psrParams> data_history;
  int current;

  void menubarConstruct();   // constructs the menubar
  QWidget* menu;              // the menubar
  
  void parmboxConstruct();   // constructs a dialog box with qt_toglParams
  QWidget* eph_select;        // the dialog created in parmboxConstruct ();

  // the open/save popup dialog
  qt_fileParams* io_dialog;

  // performs various book-keeping tasks whenever
  // a new TEMPO Parameter set is loaded/set
  void new_data(bool add_to_history = true);

  // ///////////////////////////////////////////////////////////////////
  // the popup menus
  QPopupMenu* file;
  int saveID;

  QPopupMenu* edit;   // Pointer to the edit popup menu
  int selectID;       // ID of parameter select menu option
  int backwardID;     // ID of backward menu option
  int forwardID;      // ID of forward menu option

 protected:
  // Functions that assist the geometry manager
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const 
    { return QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding); };
  QSize minimumSizeHint() const 
    { return sizeHint(); };
};

#endif
