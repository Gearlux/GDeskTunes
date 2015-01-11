QT += qml quick
!no_desktop: QT += widgets

TARGET = GDeskRemote

SOURCES += main.cpp \
    remoteclient.cpp

HEADERS += \
    remoteclient.h

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

INCLUDEPATH += ../../lib/bonjourgear

SOURCES += \
    ../../lib/bonjourgear/client.cpp \
    ../../lib/bonjourgear/bonjourservicebrowser.cpp \
    ../../lib/bonjourgear/bonjourserviceresolver.cpp \
    ../../lib/bonjourgear/protocol.cpp

HEADERS += \
    ../../lib/bonjourgear/client.h \
    ../../lib/bonjourgear/bonjourservicebrowser.h \
    ../../lib/bonjourgear/bonjourserviceresolver.h \
    ../../lib/bonjourgear/protocol.h
