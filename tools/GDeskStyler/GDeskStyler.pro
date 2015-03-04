#-------------------------------------------------
#
# Project created by QtCreator 2015-03-04T07:57:15
#
#-------------------------------------------------

include(../../thirdparty/qt-solutions/qtpropertybrowser/src/qtpropertybrowser.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GDeskStyler
TEMPLATE = app


SOURCES += main.cpp\
        gdeskstyler.cpp

HEADERS  += gdeskstyler.h

FORMS    += gdeskstyler.ui
