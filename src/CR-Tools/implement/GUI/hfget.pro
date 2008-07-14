
#Run: 
#qmake hfget.pro
#make


TEMPLATE = lib
DEPENDPATH += . 
INCLUDEPATH += . /usr/include/python2.4 /usr/local/boost_1_35_0
CONFIG      += warn_off debug console thread dll
LIBS        += /home/falcke/LOFAR/hfplot/boost/bin.v2/libs/python/build/gcc-4.1.2/debug/libboost_python-gcc41-d-1_35.so.1.35.0 \
               /usr/local/boost_1_35_0/bin.v2/libs/thread/build/gcc-4.1.2/release/threading-multi/libboost_thread-gcc41-mt-1_35.so.1.35.0

# Input
HEADERS +=  cell.h \
                finddialog.h \
                gotocelldialog.h \
                mainwindow.h \
                sortdialog.h \
                spreadsheet.h \
                plotter.h\
	   hfget-defs.h \
           hfget.h \
           hfdefs.h \
           hfcast.h \
           hffuncs.h \
           hfget.hpp \
           VectorSelector.h \    
           switch-type.cc \
           hfqt.h
SOURCES += cell.cpp \
                finddialog.cpp \
                gotocelldialog.cpp \
                mainwindow.cpp \
                sortdialog.cpp \
                spreadsheet.cpp\
                plotter.cpp \
           VectorSelector.cc \    
           hffuncs.cc \
           hfget.cc \
           hfqt.cc
FORMS         = gotocelldialog.ui \
                sortdialog.ui
RESOURCES     = spreadsheet.qrc \
                plotter.qrc\

TARGET	      = hfget
