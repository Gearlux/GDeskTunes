#-------------------------------------------------
#
# Project created by QtCreator 2015-03-04T07:57:15
#
#-------------------------------------------------

include(../../thirdparty/qt-solutions/qtpropertybrowser/src/qtpropertybrowser.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GDeskStyler
TEMPLATE = lib
CONFIG += staticlib


SOURCES += main.cpp\
        gdeskstyler.cpp \
    gstyle.cpp

HEADERS  += gdeskstyler.h \
    gstyle.h \
    gserializer.h

FORMS    += gdeskstyler.ui
