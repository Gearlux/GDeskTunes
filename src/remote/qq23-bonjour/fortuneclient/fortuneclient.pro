HEADERS       = client.h \
                bonjourservicebrowser.h \
                bonjourserviceresolver.h
SOURCES       = client.cpp \
                main.cpp \
                bonjourservicebrowser.cpp \
                bonjourserviceresolver.cpp
QT           += network widgets

!mac:x11:LIBS+=-ldns_sd
win32:LIBS+=-ldnssd
LIBPATH=C:/Temp/mDNSResponder-107.6/mDNSWindows/DLL/Debug
INCLUDEPATH += c:/Temp/mDNSResponder-107.6/mDNSShared
