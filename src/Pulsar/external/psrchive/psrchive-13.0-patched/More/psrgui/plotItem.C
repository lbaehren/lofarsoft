#include "plotItem.h"
#include "qtooltip.h"

Pulsar::plotItem::plotItem(QWidget* parent, Pulsar::Plot* figPtr, 
			   const QString& name, const QString& desc)
  : QRadioButton(name, parent)
{
  // QToolTip::add(this, desc);
  figStyle = figPtr;
}
