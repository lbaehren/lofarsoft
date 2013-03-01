#include "qapplication.h"
#include "qmainwindow.h"
#include "qradiobutton.h"
#include "qbuttongroup.h"
#include "qerrormessage.h"
#include "qfiledialog.h"
#include "qpopupmenu.h"
#include "qvbox.h"
#include "qmenubar.h"
#include "qtoolbar.h"
#include "qaction.h"
#include "qgroupbox.h"
#include "qcheckbox.h"

#include "Pulsar/Plot.h"
#include "Pulsar/PlotFactory.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Interpreter.h"

#include "plotItem.h"
#include "interfacePanel.h"

#include "TextInterface.h"
#include "Error.h"

#include "cpgplot.h"

#include <iostream>

namespace Pulsar {
  
  class PlotWindow;

  class PulsarGUI : public QMainWindow
    {
      Q_OBJECT

      public:

        // Basic constructor
      PulsarGUI (QApplication* qa);

	void readFile(QString& filename);
      
      protected slots:

	void fileOpen();
        void fileQuit();

	void confFrame();
	void confGraph();
        void confProc();
	void confDialog();

	void plotGraph();

        void preProcess();
        void undoChanges();

      protected:

	Reference::To<Pulsar::Archive> arch;
        Reference::To<TextInterface::Parser> preti;
        Reference::To<Pulsar::Interpreter> ppengine;
	Reference::To<Pulsar::PlotWindow> window;

        QLineEdit* ppLe;
        QCheckBox* autoBase;

	QButtonGroup* psrButtons;

        QPopupMenu* fileMenu;
	QPopupMenu* confMenu;
  
	QToolBar* psrTools;

	QApplication* myApp;

	void report (Error&);

      private:
  
    };
}
