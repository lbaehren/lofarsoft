#include "interfacePanel.h"

Pulsar::interfacePanel::interfacePanel(QWidget* parent, 
				       TextInterface::Parser* ui)
  : QDialog(parent)
{
  ti = ui;

  QVBoxLayout* base = new QVBoxLayout(this);

  QGroupBox* layout = new QGroupBox(5, Qt::Horizontal, "Interface Panel", this);
  base->addWidget(layout);

  QButtonGroup* buttons = new QButtonGroup(2, Qt::Horizontal, "Actions", this);
  base->addWidget(buttons);

  QPushButton* updateMe = new QPushButton("&Update", buttons);
  QObject::connect(updateMe, SIGNAL(clicked()), this, SLOT(processRequest()));
  updateMe->setDefault(true);

  QString useful1, useful2;
  for (unsigned i = 0; i < ui->get_nvalue(); i++) {
    useful1 = ti->get_name(i);
    useful2 = ti->get_value(useful1.ascii());
    
    parameters.push_back(new QLineEdit(useful1 + " = " + useful2, layout));
  }
  
  QPushButton* closeMe = new QPushButton("Close", buttons);
  QObject::connect(closeMe, SIGNAL(clicked()), this, SLOT(accept()));
  closeMe->setDefault(false);

  layout->adjustSize();

  setSizeGripEnabled(true);
  adjustSize();
}

void Pulsar::interfacePanel::processRequest()
{
  for (unsigned i = 0; i < parameters.size(); i++) {
    if (parameters[i]->isModified()) {
      try {
	ti->process((parameters[i]->text().remove(' ')).ascii());
      }
      catch (Error& error) {
	QErrorMessage* em = new QErrorMessage(this);
	QString useful = "Invalid Request: ";
	useful += (error.get_message()).c_str();
	em->message(useful);
      }
    }
  }
}
