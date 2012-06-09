#include "TextInterface.h"
#include "Error.h"

#include "qerrormessage.h"
#include "qpushbutton.h"
#include "qdialog.h"
#include "qlayout.h"
#include "qgroupbox.h"
#include "qbuttongroup.h"
#include "qlineedit.h"
#include "qstring.h"

#include <vector>

namespace Pulsar {

  class interfacePanel : public QDialog
    {
      Q_OBJECT
	
      public:

        interfacePanel(QWidget* parent, TextInterface::Parser* ui);

      protected:

	std::vector<QLineEdit*> parameters;
	TextInterface::Parser* ti;

      protected slots:

	void processRequest();

      private:
    };
}
