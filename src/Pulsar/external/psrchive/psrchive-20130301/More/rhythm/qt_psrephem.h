/***************************************************************************
 *
 *   Copyright (C) 2001 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* ///////////////////////////////////////////////////////////////////////
   The Qt psrephem Widget
      Display TEMPO PSR ephemerides with this gui component:

      To manipulate a qt_psrephem object, see qt_psrephem_editor.h.

   Author: Willem van Straten 
   /////////////////////////////////////////////////////////////////////// */

#ifndef __QT_PSREPHEM_H
#define __QT_PSREPHEM_H

#include <vector>
#include <stdio.h>

#include <qhbox.h>
#include <qlabel.h>

#include "psrephem.h"

class qt_psrParameter;  // implementation detail defined in qt_psrParameter.h
class psrParameter;     // base class implementing item of psrephem
class QVBox;

class qt_psrephem : public QHBox
{
  Q_OBJECT
    
  friend class qt_toglParamsem;

 public:
  static bool verbose;    // add debugging things to the widget

  qt_psrephem (const Legacy::psrephem& eph, QWidget* parent=0, const char* name=0);
  qt_psrephem (QWidget* parent=0, const char* name=0);

  ~qt_psrephem ();

  // display the ephemeris given / get the epehemeris displayed
  int set_psrephem (const Legacy::psrephem& eph);
  int get_psrephem (Legacy::psrephem& eph);

  void setFitAll (bool fit);

  void unload (FILE* outstream);

  bool item_shown ( int ephind );
  bool item_fit   ( int ephind );

  static int max_chars;

  // Functions that assist the geometry manager
  QSize sizeHint () const;
  QSize minimumSizeHint () const { return sizeHint(); };
  QSizePolicy sizePolicy () const 
    { return QSizePolicy (QSizePolicy::Minimum, QSizePolicy::Fixed); };

 signals:
  void item_shown ( int ephind, bool );

 protected slots:
  void show_item ( int ephind, bool );

 protected:
  std::vector<qt_psrParameter*> params;   // parameters displayed
  int max_width;                     // maximum width required by psrParameter

  void show_item (psrParameter* parm);

  void show_item (int ephind);
  void hide_item (int ephind);

 private:

  void commonConstruct ();

  QLabel* psrname;

  QVBox* glitch;

  unsigned shown_glitch_parameters;

};

#endif
