#-------------------------------------------------
#
# Project created by QtCreator 2015-01-10T16:43:45
#
#-------------------------------------------------

QT      -= gui
QT      += network

TARGET = bonjourgear
TEMPLATE = lib
CONFIG += staticlib

HEADERS       = \
    bonjourrecord.h \
    bonjourservicebrowser.h \
    bonjourserviceregister.h \
    bonjourserviceresolver.h \
    client.h \
    protocol.h \
    server.h

SOURCES       = \
    bonjourservicebrowser.cpp \
    bonjourserviceregister.cpp \
    bonjourserviceresolver.cpp \
    client.cpp \
    protocol.cpp \
    server.cpp

!mac:x11:LIBS+=-ldns_sd

win32 {
    # Add your path to bonjour here.
    INCLUDEPATH += "C:/Program Files/Bonjour SDK/Include"

    LIBS+= -L"C:/Program Files/Bonjour SDK/Lib/Win32" -ldnssd
}

#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}
