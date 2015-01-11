#-------------------------------------------------
#
# Project created by QtCreator 2015-01-10T17:10:26
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gearserver
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    fortuneserver.cpp

HEADERS  += dialog.h \
    fortuneserver.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/bonjourgear/release/ -lbonjourgear
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/bonjourgear/debug/ -lbonjourgear
else:unix: LIBS += -L$$OUT_PWD/../../lib/bonjourgear/ -lbonjourgear

INCLUDEPATH += $$PWD/../../lib/bonjourgear
DEPENDPATH += $$PWD/../../lib/bonjourgear

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/bonjourgear/release/libbonjourgear.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/bonjourgear/debug/libbonjourgear.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/bonjourgear/release/bonjourgear.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/bonjourgear/debug/bonjourgear.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../lib/bonjourgear/libbonjourgear.a
