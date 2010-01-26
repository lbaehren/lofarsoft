/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <qvbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qdatetime.h>

#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlistbox.h>
#include <qfileinfo.h>
#include <qapplication.h>

#include "Options.h"

RhythmOptions::RhythmOptions( QWidget *parent, const char *name )
    : QTabDialog ( parent, name )
{
  setupTab2();
  setupTab3();  
}

static QFileInfo fileinfo ("0437-4715.par");

void RhythmOptions::setupTab2()
{
  QVBox *tab1 = new QVBox( this );
  tab1->setMargin( 5 );
  
  (void)new QLabel( "Filename:", tab1 );
  QLineEdit *fname = new QLineEdit( "", tab1 );
  fname->setFocus();
  
  (void)new QLabel( "Path:", tab1 );
  QLabel *path = new QLabel( fileinfo.dirPath( TRUE ), tab1 );
  path->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  
  (void)new QLabel( "Size:", tab1 );
  QLabel *size = new QLabel( QString( "%1 KB" ).arg( fileinfo.size() ), tab1 );
  size->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  
  (void)new QLabel( "Last Read:", tab1 );
  QLabel *lread = new QLabel( fileinfo.lastRead().toString(), tab1 );
  lread->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  
  (void)new QLabel( "Last Modified:", tab1 );
  QLabel *lmodif = new QLabel( fileinfo.lastModified().toString(), tab1 );
  lmodif->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  
  addTab( tab1, "General" );
}

void RhythmOptions::setupTab3()
{
  QVBox *tab3 = new QVBox( this );
  tab3->setMargin( 5 );
  tab3->setSpacing( 5 );
  
  (void)new QLabel( QString( "Open %1 with:" ).arg( "" ), tab3 );
  
  QListBox *prgs = new QListBox( tab3 );
  for ( unsigned int i = 0; i < 30; i++ ) {
    QString prg = QString( "Application %1" ).arg( i );
    prgs->insertItem( prg );
  }
  prgs->setCurrentItem( 3 );
  
  (void)new QCheckBox( QString( "Open files with the extension '%1' always with this application" ).arg( fileinfo.extension() ), tab3 );
  
  addTab( tab3, "Applications" );
}

