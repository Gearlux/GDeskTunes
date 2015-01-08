HEADERS       = \
                bonjourserviceregister.h \
    protocol.h \
    fortuneserver.h \
    gui.h \
    server.h \
    bonjourrecord.h
SOURCES       = \
                main.cpp \
                bonjourserviceregister.cpp \
    protocol.cpp \
    fortuneserver.cpp \
    gui.cpp \
    server.cpp
QT           += network widgets

!mac:x11:LIBS+=-ldns_sd

win32 {
    LIBS+=-ldnssd
    # Add your path to bonjour here.
    LIBPATH=C:/Temp/mDNSResponder-107.6/mDNSWindows/DLL/Debug
    INCLUDEPATH += c:/Temp/mDNSResponder-107.6/mDNSShared
}
