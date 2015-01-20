QT += qml quick
!no_desktop: QT += widgets

QTPLUGIN += qsvg

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

INCLUDEPATH += \
    ../../lib/bonjourgear \
    ../../../../OneDrive/workspace/mdnsresponder/mDNSShared \
    ../../../../OneDrive/workspace/mdnsresponder/mDNSCore

SOURCES += \
    ../../lib/bonjourgear/client.cpp \
    ../../lib/bonjourgear/bonjourservicebrowser.cpp \
    ../../lib/bonjourgear/bonjourserviceresolver.cpp \
    ../../lib/bonjourgear/protocol.cpp \

android: {
    # QMAKE_CFLAGS += -D__ANDROID__ -D_GNU_SOURCE -DHAVE_IPV6 -DNOT_HAVE_SA_LEN -DUSES_NETLINK -DTARGET_OS_LINUX -fno-strict-aliasing -DHAVE_LINUX -DMDNS_UDS_SERVERPATH=\"/dev/socket/mdnsd\" -DMDNS_DEBUGMSGS=0
    QMAKE_CFLAGS += -D__ANDROID__ -D_GNU_SOURCE -DHAVE_IPV6 -DNOT_HAVE_SA_LEN -DUSES_NETLINK -DTARGET_OS_LINUX -fno-strict-aliasing -DHAVE_LINUX -DMDNS_DEBUGMSGS=0

    SOURCES += \
    ../../../../OneDrive/workspace/mdnsresponder/mDNSShared/dnssd_clientstub.c \
    ../../../../OneDrive/workspace/mdnsresponder/mDNSShared/dnssd_ipc.c
    #../../../../OneDrive/workspace/mdnsresponder/mDNSPosix/mDNSPosix.c \
    #../../../../OneDrive/workspace/mdnsresponder/mDNSShared/mDNSDebug.c \
}

HEADERS += \
    ../../lib/bonjourgear/client.h \
    ../../lib/bonjourgear/bonjourservicebrowser.h \
    ../../lib/bonjourgear/bonjourserviceresolver.h \
    ../../lib/bonjourgear/protocol.h
