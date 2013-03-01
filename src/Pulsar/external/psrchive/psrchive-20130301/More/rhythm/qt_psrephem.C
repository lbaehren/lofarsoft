/***************************************************************************
 *
 *   Copyright (C) 2001 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "qt_psrephem.h"
#include "qt_psrParameter.h"

#include "psrephem.h"
#include "ephio.h"
#include "psrParams.h"

#include "qtools.h"

#include <qvbox.h>
#include <iostream>

using namespace std;

// //////////////////////////////////////////////////////////////////////////
//
// qt_psrephem
//
// organizes qt_psrParameter objects in a VBox
//
// //////////////////////////////////////////////////////////////////////////
bool qt_psrephem::verbose = false;

qt_psrephem::qt_psrephem (const Legacy::psrephem& eph,
			  QWidget* parent, const char* name) :
  QHBox (parent, name)
{
  commonConstruct ();
  set_psrephem (eph);
}

qt_psrephem::qt_psrephem (QWidget* parent, const char* name) :
  QHBox (parent, name)
{
  commonConstruct ();
}

int qt_psrephem::max_chars = 36;

void qt_psrephem::commonConstruct ()
{
  if (verbose)
    cerr << "qt_psrephem::commonConstruct" << endl;

  QVBox* most = new QVBox (this);
  psrname = new QLabel ("no ephemeris loaded", most);
  psrname->setAlignment (AlignHCenter | AlignTop);

  glitch = new QVBox (this);
  QLabel* glabel = new QLabel ("Glitch Parameters", glitch);
  glabel->setAlignment (AlignHCenter | AlignTop);

  shown_glitch_parameters = 0;
  glitch->hide();

  for (int iparm=0; iparm < EPH_NUM_KEYS; iparm++) {

    if (verbose) cerr << "qt_psrephem::commonConstruct " << iparm << "/"
		      << EPH_NUM_KEYS << "[" << parmNames[iparm] << "]" <<endl;

    qt_psrParameter* ephitem = 0;

    if (strncmp(parmNames[iparm], "GL", 2) == 0)
      ephitem = qt_psrParameter::factory (iparm, glitch);
    else
      ephitem = qt_psrParameter::factory (iparm, most);

    if (ephitem == NULL)
      throw ("bad_alloc");

    params.push_back (ephitem);
    ephitem->hide();

  }

  if (verbose)
    cerr << "qt_psrephem::commonConstruct prepare fonts" << endl;

  max_width = psrname->fontMetrics().maxWidth() * max_chars;
  psrname->setFixedHeight (psrname->fontMetrics().height());
  psrname->setFixedWidth (max_width);
}

qt_psrephem::~qt_psrephem ()
{
  for (unsigned iparm=0; iparm < params.size(); iparm++)
    delete params[iparm];
}

bool qt_psrephem::item_shown (int ephind)
{
  if (params[ephind]->isVisible())
    return true;
  
  return false;
}

bool qt_psrephem::item_fit (int ephind)
{
  return params[ephind]->getFit();
}

void qt_psrephem::show_item (psrParameter* parm)
{
  int ephind = psrParameter::check (parm -> get_ephind());

  params[ephind] -> setValue (parm);
  show_item (ephind);
}

// returns a pointer to the relevant widget
void qt_psrephem::show_item (int ephind)
{
  if (params[ephind]->isVisible()) {
    if (qt_psrephem::verbose)
      cerr << "qt_psrephem::show_item already visible:" << ephind << endl;
    return;
  }

  if (strncmp(parmNames[ephind], "GL", 2) == 0)
    shown_glitch_parameters ++;

  if (shown_glitch_parameters == 1)
    glitch->show();

  params[ephind]->show();
  emit item_shown (ephind, true);

  updateGeometry();

  if (qt_psrephem::verbose)
    cerr << "qt_psrephem::show_item return" << endl;
}

void qt_psrephem::hide_item (int ephind)
{
  if (!params[ephind]->isVisible()) {
    if (qt_psrephem::verbose)
      cerr << "qt_psrephem::hide_item already hidden:" << ephind << endl;
    return;
  }

  if (strncmp(parmNames[ephind], "GL", 2) == 0)
    shown_glitch_parameters --;

  if (shown_glitch_parameters == 0)
    glitch->hide();

  params[ephind]->hide();
  emit item_shown (ephind, false);

  updateGeometry();

  if (qt_psrephem::verbose)
    cerr << "qt_psrephem::hide_item return" << endl;
}

// //////////////////////////////////////////////////////////////////////////
// slot allows psrParameters to be toggled in and out of the picture
void qt_psrephem::show_item (int ephind, bool show)
{
  if (show)
    show_item (ephind);
  else
    hide_item (ephind);
}

QSize qt_psrephem::sizeHint () const
{
  int height = psrname->fontMetrics().height();
  int mwidth = max_width;

  for (unsigned iparm=0; iparm < params.size(); iparm++)
    if (params[iparm]->isVisible()) {
      height += params[iparm]->sizeHint().height();
      if (params[iparm]->sizeHint().width() > mwidth)
	mwidth = params[iparm]->sizeHint().width();
    }

  if (shown_glitch_parameters)
    mwidth *= 2;

  if (verbose)
    cerr << "qt_psrephem::sizeHint = " << QSize (mwidth, height) << endl;

  return QSize (mwidth, height);
}

void qt_psrephem::setFitAll (bool fit) {
  for (unsigned ipar=0; ipar<params.size(); ipar++)
    if (params[ipar]->isVisible())
      params[ipar]->setFit (fit);
}

int qt_psrephem::set_psrephem (const Legacy::psrephem& eph)
{
  unsigned iparm=0;
  for (iparm=0; iparm < params.size(); iparm++)
    hide_item (iparm);

  psrname->setText (eph.psrname().c_str());

  psrParams meph;
  meph.set_psrephem (eph);
  for (iparm=0; iparm < meph.params.size(); iparm++)
    show_item (meph.params[iparm]);

  return 0;
}

int qt_psrephem::get_psrephem (Legacy::psrephem& eph)
{
  psrParams meph;
  // duplicate those elements that are not currently hidden
  for (unsigned ipar=0; ipar<params.size(); ipar++) {
    if (params[ipar]->isVisible()) {    
      psrParameter* parm = params[ipar]->qt_psrParameter::duplicate();
      if (parm)
	meph.params.push_back(parm);
    }
  }

  if (verbose) cerr << "qt_psrephem::get_psrephem " << meph.params.size()
		    << " parameters" << endl;

  meph.get_psrephem (eph);

  if (verbose) cerr << "qt_psrephem::get_psrephem returns\n";
  return 0;
}

void qt_psrephem::unload (FILE* outstream)
{
  Legacy::psrephem data;
  if (verbose) cerr << "qt_psrephem::unload calling get_psrephem\n";
  get_psrephem (data);
  if (verbose) cerr << "qt_psrephem::unload calling psrephem::unload\n";
  data.unload (outstream);
}
