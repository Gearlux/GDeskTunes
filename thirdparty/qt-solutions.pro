#-------------------------------------------------
#
# Project created by QtCreator 2015-01-08T23:19:41
#
#-------------------------------------------------

QT       += widgets network

TARGET = qt-solutions
TEMPLATE = lib
CONFIG += staticlib

win32: {
    SOURCES += \
        qtsingleapplication/src/qtlockedfile_win.cpp
}

unix: {
    SOURCES += \
        qtsingleapplication/src/qtlockedfile_unix.cpp
}

HEADERS += \
    qtsingleapplication/src/qtlocalpeer.h \
    qtsingleapplication/src/qtlockedfile.h \
    qtsingleapplication/src/qtsingleapplication.h

SOURCES += \
    qtsingleapplication/src/qtlocalpeer.cpp \
    qtsingleapplication/src/qtlockedfile.cpp \
    qtsingleapplication/src/qtsingleapplication.cpp

#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}
