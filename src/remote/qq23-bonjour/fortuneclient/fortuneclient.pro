HEADERS       = \
                bonjourservicebrowser.h \
                bonjourserviceresolver.h \
    ../fortuneserver/protocol.h \
    bonjourrecord.h \
    client.h \
    fortuneclient.h
SOURCES       = \
                main.cpp \
                bonjourservicebrowser.cpp \
                bonjourserviceresolver.cpp \
    ../fortuneserver/protocol.cpp \
    client.cpp \
    fortuneclient.cpp
QT           += network widgets

!mac:x11:LIBS+=-ldns_sd

win32 {
    # Add your path to bonjour here.
    INCLUDEPATH += "C:/Program Files/Bonjour SDK/Include"

    LIBS+= -L"C:/Program Files/Bonjour SDK/Lib/Win32" -ldnssd
}
