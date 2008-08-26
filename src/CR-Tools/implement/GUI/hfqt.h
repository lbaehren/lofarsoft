#ifndef HFQT_H
#define HFQT_H

#include "hfdefs.h"

#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include <QtGui/QDialog>
#include <QtGui/QtGui>

using namespace std;

/* Forward declarations */
class QCheckBox;
class QLineEdit;
class QPushButton;

// ==============================================================================
//
//  Class : HQLabel
//
// ==============================================================================

/*!
  \class HQLabel

  \ingroup CR_GUI

  \author Heino Falcke
*/
class HQLabel : public QLabel {

  Q_OBJECT
    
    public:
  HQLabel(QWidget *parent=NULL);
  
  // signals:
  
  private slots:
  void print(const QString &);
  //  void print();
  
 private:
  
  QString label_text;
  Integer ncalled;
  
};

// ==============================================================================
//
//  Class : HQLabel
//
// ==============================================================================

/*!
  \class FindDialog1

  \ingroup CR_GUI

  \author Heino Falcke
*/
class FindDialog1 : public QDialog
{
  Q_OBJECT
    
    public:
  FindDialog1(QWidget *parent = 0);
  
 signals:
  void findNext(const QString &str, Qt::CaseSensitivity cs);
  void findPrevious(const QString &str, Qt::CaseSensitivity cs);
  
  private slots:
  void findClicked();
  void enableFindButton(const QString &text);
  
 private:
  /*! Widget label */
  QLabel *label;
  QLineEdit *lineEdit;
  /*! GUI element: check-box */
  QCheckBox *caseCheckBox;
  QCheckBox *backwardCheckBox;
  /*! GUI elelement: find button */
  QPushButton *findButton;
  /*! GUI element: close button */
  QPushButton *closeButton;
};

#endif
