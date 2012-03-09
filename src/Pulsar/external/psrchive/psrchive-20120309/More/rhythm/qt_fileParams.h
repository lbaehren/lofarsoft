/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifndef __QT_FILEEPH_H
#define __QT_FILEEPH_H

#include "psrephem.h"

#include <string>
#include <qfiledialog.h>

class qt_fileParams : public QFileDialog
{
  Q_OBJECT

 public:
  static bool verbose;
  qt_fileParams (const QString& startname = QString::null, QWidget* parent=0);

  // open() and save() return zero if the user cancels, 1 otherwise
  int open ( Legacy::psrephem& eph );
  int save ( const Legacy::psrephem& eph );

  // string filename () { return string (fileName); };
 protected slots:
  // called when the user has chosen a filename
  void chosen ( const QString & );

 protected:
  std::string fileName;
};

#endif
