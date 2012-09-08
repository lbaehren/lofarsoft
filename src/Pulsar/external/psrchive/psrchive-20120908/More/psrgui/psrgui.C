#include "qapplication.h"

#include "PulsarGUI.h"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  Pulsar::PulsarGUI* pg = new Pulsar::PulsarGUI(&app);
  app.setMainWidget(pg);
  pg -> show();

  if (app.argc() > 1)
  {
    QString filename = app.argv()[1];
    pg->readFile(filename);
  }

  return app.exec();

}
