TEMPLATE = lib
TARGET = bonjourlibrary

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

QT += network widgets

!mac:x11:LIBS+=-ldns_sd

win32:LIBS+=-ldnssd
LIBPATH=C:/Temp/mDNSResponder-107.6/mDNSWindows/DLL/Debug
INCLUDEPATH += c:/Temp/mDNSResponder-107.6/mDNSShared
