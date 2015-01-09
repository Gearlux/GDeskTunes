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
    # Add your path to bonjour here.
    INCLUDEPATH += "C:/Program Files/Bonjour SDK/Include"

    LIBS+= -L"C:/Program Files/Bonjour SDK/Lib/Win32" -ldnssd
}
