#-------------------------------------------------
#
# Project created by QtCreator 2014-09-10T19:49:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mapviewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    parse.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    icon.rc

RESOURCES += \
    resources.qrc
