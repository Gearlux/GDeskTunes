HEADERS       = \
                bonjourservicebrowser.h \
                bonjourserviceresolver.h \
    ../fortuneserver/protocol.h \
    bonjourrecord.h \
    gui.h \
    client.h
SOURCES       = \
                main.cpp \
                bonjourservicebrowser.cpp \
                bonjourserviceresolver.cpp \
    ../fortuneserver/protocol.cpp \
    gui.cpp \
    client.cpp
QT           += network widgets

!mac:x11:LIBS+=-ldns_sd
win32:LIBS+=-ldnssd
LIBPATH=C:/Temp/mDNSResponder-107.6/mDNSWindows/DLL/Debug
INCLUDEPATH += c:/Temp/mDNSResponder-107.6/mDNSShared
