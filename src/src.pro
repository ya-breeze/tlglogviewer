#-------------------------------------------------
#
# Project created by QtCreator 2012-05-25T17:27:17
#
#-------------------------------------------------

DEFINES += 'VERSION=\'\"1.0.0\"\''
QMAKE_CXXFLAGS += -std=c++0x -Wc++0x-compat

CONFIG   += debug qxt
QT       += core gui
QXT      += core gui

TARGET = tlglogviewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    transaction.cpp \
    callflow.cpp \
    parser.cpp \
    itemarrow.cpp \
    itemcomp.cpp \
    exportdialog.cpp \
    timestampimage.cpp \
    filter.cpp \
    sortfiltermodel.cpp \
    expressionparser.cpp \
    expressionparserimpl.cpp

HEADERS  += mainwindow.h \
    transaction.h \
    callflow.h \
    parser.h \
    itemarrow.h \
    itemcomp.h \
    ICallFlow.h \
    exportdialog.h \
    timestampimage.h \
    filter.h \
    sortfiltermodel.h \
    expressionparser.h \
    expressionparserimpl.h

FORMS    += mainwindow.ui \
    exportdialog.ui

unix { 
    #VARIABLES
    isEmpty(PREFIX) {
	PREFIX = /usr
    }
    BINDIR = $$PREFIX/bin
    DATADIR =$$PREFIX/share
    DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"
    
    #MAKE INSTALL
    target.path = /usr/bin
    data.path = /usr/share/$$TARGET/data
    data.files = data/*
    INSTALLS += target data
}
