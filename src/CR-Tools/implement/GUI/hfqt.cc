//#define DBG_MODE 0
//#define DBG_MODE 1

#include <QApplication>
#include <QtGui>

using namespace std;
#include "mainwindow.h"
#include "hfdefs.h"
#include "hfcast.h"
#include "hfget.h"
#include "hfqt.h"
#include <mgl/mgl_qt.h>


void qrun2(QLabel **label){
  int argc;
  char *argv[1];

  cout << "Thread: Label=" << (*label);

  argv[0]="hfplot";
  argc=1;
  HString s="Hallo Heino";
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
//This is an example function from mgl used temporarily ..

int sample_1(mglGraph *gr, void *)
{
	setlocale(LC_ALL, "C");
	mglData  a(50,15),d(50),d1(50),d2(50);
	d.Modify("0.7*sin(2*pi*x) + 0.5*cos(3*pi*x) + 0.2*sin(pi*x)");
	d1.Modify("cos(2*pi*x)");
	d2.Modify("sin(2*pi*x)");
	a.Modify("pow(x,4*y)");

	gr->NewFrame();
	gr->Box();	gr->Axis("xy");	gr->Label('x',"x");	gr->Label('y',"y");
	gr->Text(mglPoint(0,1.2,1),"Simple plot of one curve");
	gr->Plot(d);
	gr->EndFrame();

	gr->NewFrame();
	gr->Box();	gr->Axis("xy");	gr->Label('x',"x");	gr->Label('y',"y");
	gr->Text(mglPoint(0,1.2,1),"Three curves with manual styles");
	gr->Plot(d,"b");
	gr->Plot(d1,"ri");
	gr->Plot(d2,"m|^");
	gr->Plot(d,"l o");
	gr->EndFrame();

	gr->NewFrame();
	gr->Box();	gr->Axis("xy");	gr->Label('x',"x");	gr->Label('y',"y");
	gr->Text(mglPoint(0,1.2,1),"Three curves with automatic styles");
	gr->Plot(d);
	gr->Plot(d1);
	gr->Plot(d2);
	gr->EndFrame();

	gr->NewFrame();
	gr->Box();	gr->Axis("xy");	gr->Label('x',"x");	gr->Label('y',"y");
	gr->Text(mglPoint(0,1.2,1),"Curves from matrix");
	gr->Plot(a);
	gr->EndFrame();

	gr->NewFrame();
	gr->Box();	gr->Axis("xy");	gr->Label('x',"x");	gr->Label('y',"y");
	gr->Text(mglPoint(0,1.2,1),"Parametrical curves in 2D");
	gr->Plot(d1,d2,"b");
	gr->Plot(d1,d,"ri");
	gr->EndFrame();

	gr->NewFrame();
	gr->Text(mglPoint(0,1.2,1),"Parametrical curves in 3D");
	gr->Rotate(60,40);
	gr->Box();	gr->Axis();	gr->Label('x',"x");	gr->Label('y',"y");	gr->Label('z',"z");
	gr->Plot(d1,d2,d,"b");
	gr->EndFrame();

	gr->NewFrame();
	gr->SubPlot(2,2,0);
	gr->Box();	gr->Axis("xy");	gr->Label('x',"x");	gr->Label('y',"y");
	gr->Text(mglPoint(0,1.2,1),"Area plot");
	gr->Area(d);
	gr->SubPlot(2,2,1);
	gr->Box();	gr->Axis("xy");	gr->Label('x',"x");	gr->Label('y',"y");
	gr->Text(mglPoint(0,1.2,1),"Step plot");
	gr->Step(d);
	gr->SubPlot(2,2,2);
	gr->Box();	gr->Axis("xy");	gr->Label('x',"x");	gr->Label('y',"y");
	gr->Text(mglPoint(0,1.2,1),"Stem plot");
	gr->Stem(d);
	gr->SubPlot(2,2,3);
	gr->Box();	gr->Axis("xy");	gr->Label('x',"x");	gr->Label('y',"y");
	gr->Text(mglPoint(0,1.2,1),"Bars plot");
	gr->Bars(d);
	gr->EndFrame();

	gr->NewFrame();
	gr->SubPlot(2,2,0);
	gr->Text(mglPoint(0,1.2,1),"Area plot in 3D");
	gr->Rotate(60,40);
	gr->Box();	gr->Axis();	gr->Label('x',"x");	gr->Label('y',"y");
	gr->Area(d1,d2,d);
	gr->SubPlot(2,2,1);
	gr->Text(mglPoint(0,1.2,1),"Step plot in 3D");
	gr->Rotate(60,40);
	gr->Box();	gr->Axis();	gr->Label('x',"x");	gr->Label('y',"y");
	gr->Step(d1,d2,d);
	gr->SubPlot(2,2,2);
	gr->Text(mglPoint(0,1.2,1),"Stem plot in 3D");
	gr->Rotate(60,40);
	gr->Box();	gr->Axis();	gr->Label('x',"x");	gr->Label('y',"y");
	gr->Stem(d1,d2,d);
	gr->SubPlot(2,2,3);
	gr->Text(mglPoint(0,1.2,1),"Bars plot in 3D");
	gr->Rotate(60,40);
	gr->Box();	gr->Axis();	gr->Label('x',"x");	gr->Label('y',"y");
	gr->Bars(d1,d2,d);
	gr->EndFrame();
	return gr->GetNumFrame();
}

void mglrun(mglGraphQT **mglwin){
  //To make it clear: mglwin is a pointer to a location in
  //memory. This location is accessible from both threads. That
  //location stores as content again a pointer, which points to the
  //MathGL window. So, (*mglwin) gives a pointer to the window.
  //**mglwin is the window class. *mglwin->Method willc all methods of
  //the MathGL window.

  int argc;
  char *argv[1];
  argv[0]="hfplot-mgl-window";
  argc=1;
 
  DBG("Create new mgl Window.");
  *mglwin=new mglGraphQT;

  (*mglwin)->Window(argc,argv,sample_1,"1D plots",NULL);	
  DBG("Run mglQT Application.");
  mglQtRun();
  DBG("Done mglQTApplication.");
}

HQLabel::HQLabel(QWidget *parent) 
  : QLabel(parent)
{
  label_text="Nothing printed yet.";
  ncalled=0;
}

void HQLabel::print(const QString &text){
//void HQLabel::print(){
//  HString text="Clicked..";
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
