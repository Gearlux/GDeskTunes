#-------------------------------------------------
#
# Project created by QtCreator 2015-01-10T16:43:45
#
#-------------------------------------------------

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

    contains(QMAKE_HOST.arch, x86_64): {
        LIBS+= -L"C:/Program Files/Bonjour SDK/Lib/x64" -ldnssd
    } else {
        LIBS+= -L"C:/Program Files/Bonjour SDK/Lib/Win32" -ldnssd
    }
}

#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}
