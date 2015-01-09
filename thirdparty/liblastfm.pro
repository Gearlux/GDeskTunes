#-------------------------------------------------
#
# Project created by QtCreator 2015-01-08T22:13:30
#
#-------------------------------------------------

QT       += network xml

QT       -= gui

TARGET = liblastfm
TEMPLATE = lib
CONFIG += staticlib

linux: {
    QT += dbus

    HEADERS += \
        src/linux/LNetworkConnectionMonitor.h

    SOURCES += \
        src/linux/LNetworkConnectionMonitor_linux.cpp
}

win32: {
    # Windows specific lastfm dependencies
    HEADERS += \
        src/win/ComSetup.h \
        src/win/IeSettings.h \
        src/win/NdisEvents.h \
        src/win/WmiSink.h \
        src/win/WNetworkConnectionMonitor.h
    SOURCES += \
        src/win/NdisEvents.cpp \
        src/win/WmiSink.cpp \
        src/win/WNetworkConnectionMonitor_win.cpp
    LIBS += -lwinhttp -lwbemuuid
}

mac: {

    greaterThan(QT_MAJOR_VERSION, 4): QMAKE_MAC_SDK = macosx10.9

    HEADERS += \
        src/mac/MNetworkConnectionMonitor.h \
        src/mac/ProxyDict.h

    SOURCES += \
        src/mac/MNetworkConnectionMonitor_mac.cpp

    QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-mismatched-tags -Wno-deprecated-declarations -Wno-unused-const-variable -Wno-logical-not-parentheses
}

DEFINES += LASTFM_LIB_STATIC
INCLUDEPATH += ..

HEADERS += \
    src/AbstractType.h \
    src/Album.h \
    src/Audioscrobbler.h \
    src/Auth.h \
    src/Chart.h \
    src/FingerprintId.h \
    src/InternetConnectionMonitor.h \
    src/Library.h \
    src/Mbid.h \
    src/NetworkAccessManager.h \
    src/NetworkConnectionMonitor.h \
    src/Playlist.h \
    src/RadioStation.h \
    src/RadioTuner.h \
    src/ScrobbleCache.h \
    src/ScrobblePoint.h \
    src/Tag.h \
    src/Tasteometer.h \
    src/Track.h \
    src/Url.h \
    src/UrlBuilder.h \
    src/User.h \
    src/XmlQuery.h \
    src/Xspf.h \
    src/misc.h \
    src/ws.h

SOURCES += \
    src/Album.cpp \
    src/Artist.cpp \
    src/Audioscrobbler.cpp \
    src/Auth.cpp \
    src/Chart.cpp \
    src/FingerprintId.cpp \
    src/InternetConnectionMonitor.cpp \
    src/Library.cpp \
    src/Mbid.cpp \
    src/misc.cpp \
    src/NetworkAccessManager.cpp \
    src/NetworkConnectionMonitor.cpp \
    src/Playlist.cpp \
    src/RadioStation.cpp \
    src/RadioTuner.cpp \
    src/ScrobbleCache.cpp \
    src/ScrobblePoint.cpp \
    src/Tag.cpp \
    src/Tasteometer.cpp \
    src/Track.cpp \
    src/Url.cpp \
    src/UrlBuilder.cpp \
    src/User.cpp \
    src/ws.cpp \
    src/XmlQuery.cpp \
    src/Xspf.cpp \


#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}

