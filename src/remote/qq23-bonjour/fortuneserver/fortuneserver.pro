HEADERS       = server.h \
                bonjourserviceregister.h \
    protocol.h
SOURCES       = server.cpp \
                main.cpp \
                bonjourserviceregister.cpp \
    protocol.cpp
QT           += network widgets

!mac:x11:LIBS+=-ldns_sd

win32 {
    LIBS+=-ldnssd
    # Add your path to bonjour here.
    LIBPATH=C:/Temp/mDNSResponder-107.6/mDNSWindows/DLL/Debug
    INCLUDEPATH += c:/Temp/mDNSResponder-107.6/mDNSShared
}
