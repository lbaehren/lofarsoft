#define DBG_MODE 0

using namespace std;
#include "mainwindow.h"
#include "hfdefs.h"
#include "hfcast.h"
#include "hfget.h"
#include "hfqt.h"

void qrun2(QLabel **label){
  int argc;
  char *argv[1];

  cout << "Thread: Label=" << (*label);

  argv[0]="hfplot";
  argc=1;
  String s="Hallo Heino";
  cout << s <<endl;
  QApplication qapp(argc,argv);
  
  MSG("New Label:");
  //  *label = new QLabel("Hier ist der app Thread");
  //cout << "Thread: Label=" << *label;
  //  (*label)->show();

  /*  MSG("Define label2");
  MainWindow **label2;  
  MSG("Define label2");
  (*label2) = new MainWindow;
  MSG("show()");
  (*label2)->show();
  */

    MainWindow mainWin;
    mainWin.show();


  MSG("Run QApplication.");
  qapp.exec();
  MSG("Done.");
}

void qrun(MainWindow **mwin_pp){
  int argc=1;
  char *argv[1];

  argv[0]="hfplot";
  QApplication qapp(argc,argv);

  *mwin_pp = new MainWindow;
  (*mwin_pp)->show();

  DBG("Run QApplication.");
  qapp.exec();
  DBG("Done.");
}

HQLabel::HQLabel(QWidget *parent) 
  : QLabel(parent)
{
  label_text="Nothing printed yet.";
  ncalled=0;
}

void HQLabel::print(const QString &text){
//void HQLabel::print(){
//  String text="Clicked..";
  QString s;
  s.setNum(ncalled);
  ncalled++;
  //  MSG("QLabel: #" << ncalled);
  MSG("QLabel: #" << ncalled << " Text=" << text.toStdString());
  //  s=s+QString(text.c_str());
  s=s+">"+text;
  //  QMetaObject::invokeMethod(this,SLOT(setText(const QString &)),Q_ARG(QString, s));
  //  QMetaObject::invokeMethod(this,SLOT(setText(const QString &)),Q_ARG(QString, s));

  if (ncalled%2==0) {QMetaObject::invokeMethod(this, "clear",
					       Qt::QueuedConnection);}
  else {setText(s);};
}


FindDialog1::FindDialog1(QWidget *parent)
    : QDialog(parent)
{
  //    label = new HQLabel(tr("Find &what:"));
    label = new HQLabel();
    lineEdit = new QLineEdit;
    label->setBuddy(lineEdit);

    caseCheckBox = new QCheckBox(tr("Match &case"));
    backwardCheckBox = new QCheckBox(tr("Search &backward"));

    findButton = new QPushButton(tr("&Print"));
    findButton->setDefault(true);
    findButton->setEnabled(false);

    closeButton = new QPushButton(tr("Close"));

    connect(lineEdit, SIGNAL(textChanged(const QString &)),
            label, SLOT(print(const QString &)));
    /*  
    connect(lineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(enableFindButton(const QString &)));
    connect(findButton, SIGNAL(clicked()),
            this, SLOT(findClicked()));
    */
    /*    connect(findButton, SIGNAL(clicked()),
            label, SLOT(print()));
    */
    connect(closeButton, SIGNAL(clicked()),
            this, SLOT(close()));

    QHBoxLayout *topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(label);
    topLeftLayout->addWidget(lineEdit);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(topLeftLayout);
    leftLayout->addWidget(caseCheckBox);
    leftLayout->addWidget(backwardCheckBox);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(findButton);
    rightLayout->addWidget(closeButton);
    rightLayout->addStretch();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Find"));
    setFixedHeight(sizeHint().height());
}

void FindDialog1::findClicked()
{
    QString text = lineEdit->text();
    Qt::CaseSensitivity cs =
            caseCheckBox->isChecked() ? Qt::CaseSensitive
                                      : Qt::CaseInsensitive;
    if (backwardCheckBox->isChecked()) {
        emit findPrevious(text, cs);
    } else {
        emit findNext(text, cs);
    }
}

void FindDialog1::enableFindButton(const QString &text)
{
    findButton->setEnabled(!text.isEmpty());
}
