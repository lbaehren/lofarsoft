#include "Pulsar/Plot.h"

#include "qwidget.h"
#include "qradiobutton.h"
#include "qwhatsthis.h"

namespace Pulsar {
  
  class plotItem : public QRadioButton
    {
      Q_OBJECT
	
      public slots:
	
      public:

	plotItem(QWidget* parent, Pulsar::Plot* figPtr, 
		 const QString& name, const QString& desc);

        Pulsar::Plot* getPlot() {return figStyle;}
	
      protected:

	// Store a pointer to a specific plot style
	Reference::To<Pulsar::Plot> figStyle;

      private:

    };
}
