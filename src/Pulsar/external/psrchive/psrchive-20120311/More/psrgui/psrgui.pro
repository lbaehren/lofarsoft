
TEMPLATE = app
QT += qt3support

INCLUDEPATH += . Pulsar ../../local_include 

include (config.pro)

HEADERS += interfacePanel.h \
           ConfigurableScrollBar.h \
           plotItem.h \
           RangePolicy.h \
           PulsarGUI.h \
           Pulsar/InterfaceDialog.h \
           Pulsar/PlotWindow.h \
#           Pulsar/UsingQTDRIV.h \
           Pulsar/UsingXSERVE.h

SOURCES += InterfaceDialog.C \
           interfacePanel.C \
           ConfigurableScrollBar.C \
           plotItem.C \
           RangePolicy.C \
           PlotWindow.C \
           psrgui.C \
           PulsarGUI.C \
#           UsingQTDRIV.C \
           UsingXSERVE.C
