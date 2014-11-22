#-------------------------------------------------
#
# Project created by QtCreator 2014-09-29T14:09:30
#
#-------------------------------------------------
QT       += core gui webkit widgets network xml

greaterThan(QT_MAJOR_VERSION, 4): CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += webkitwidgets

CONFIG(release, debug|release): DESTDIR = $$OUT_PWD/release
CONFIG(debug, debug|release): DESTDIR = $$OUT_PWD/debug

win32: {
greaterThan(QT_MAJOR_VERSION, 4):
greaterThan(QT_MINOR_VERISON, 0): QT += winextras

HEADERS += \
    thirdparty/liblastfm/src/win/ComSetup.h \
    thirdparty/liblastfm/src/win/IeSettings.h \
    thirdparty/liblastfm/src/win/NdisEvents.h \
    thirdparty/liblastfm/src/win/WmiSink.h \
    thirdparty/liblastfm/src/win/WNetworkConnectionMonitor.h

SOURCES += \
    thirdparty/liblastfm/src/win/NdisEvents.cpp \
    thirdparty/liblastfm/src/win/WmiSink.cpp \
    thirdparty/liblastfm/src/win/WNetworkConnectionMonitor_win.cpp \
    thirdparty/qt-solutions/qtsingleapplication/src/qtlockedfile_win.cpp

LIBS += -lwinhttp -lwbemuuid

RC_FILE = gdesktunes.rc
}

TARGET = GDeskTunes
TEMPLATE = app

mac: {
QMAKE_INFO_PLIST = Info.plist
ICON = g_desk_tunes.icns

OBJECTIVE_SOURCES += thirdparty/SPMediaKeyTap/SPMediaKeyTap.m \
    thirdparty/SPMediaKeyTap/SPInvocationGrabbing/NSObject+SPInvocationGrabbing.m \
    mac/cocoainit.mm \
    mac/macutils.mm \
    mac/mediakeys.mm

HEADERS += mac/cocoainit.h \
    mac/macutils.h \
    mac/mediakeys.h \
    thirdparty/liblastfm/src/mac/MNetworkConnectionMonitor.h \
    thirdparty/liblastfm/src/mac/ProxyDict.h
SOURCES += \
    thirdparty/liblastfm/src/mac/MNetworkConnectionMonitor_mac.cpp \
    thirdparty/qt-solutions/qtsingleapplication/src/qtlockedfile_unix.cpp

LIBS += -framework Cocoa -framework Carbon -framework CoreFoundation -framework SystemConfiguration

greaterThan(QT_MAJOR_VERSION, 4): QMAKE_MAC_SDK = macosx10.9

lessThan(QT_MAJOR_VERSION, 5): {

info.path = $$DESTDIR/$${TARGET}.app/Contents
info.files = $$QMAKE_INFO_PLIST

icns.path = $$DESTDIR/$${TARGET}.app/Contents/Resources
icns.files = $$ICON
export(info)
export(icns)

INSTALLS += icns info
}
}

# Needed to compile liblastfm
DEFINES += LASTFM_LIB_STATIC
INCLUDEPATH += .

HEADERS += *.h
HEADERS += \
    thirdparty/liblastfm/src/AbstractType.h \
    thirdparty/liblastfm/src/Album.h \
    thirdparty/liblastfm/src/Audioscrobbler.h \
    thirdparty/liblastfm/src/Auth.h \
    thirdparty/liblastfm/src/Chart.h \
    thirdparty/liblastfm/src/FingerPrintId.h \
    thirdparty/liblastfm/src/InternetConnectionMonitor.h \
    thirdparty/liblastfm/src/Library.h \
    thirdparty/liblastfm/src/Mbid.h \
    thirdparty/liblastfm/src/NetworkAccessManager.h \
    thirdparty/liblastfm/src/NetworkConnectionMonitor.h \
    thirdparty/liblastfm/src/Playlist.h \
    thirdparty/liblastfm/src/RadioStation.h \
    thirdparty/liblastfm/src/RadioTuner.h \
    thirdparty/liblastfm/src/ScrobbleCache.h \
    thirdparty/liblastfm/src/ScrobblePoint.h \
    thirdparty/liblastfm/src/Tag.h \
    thirdparty/liblastfm/src/Tasteometer.h \
    thirdparty/liblastfm/src/Track.h \
    thirdparty/liblastfm/src/Url.h \
    thirdparty/liblastfm/src/UrlBuilder.h \
    thirdparty/liblastfm/src/User.h \
    thirdparty/liblastfm/src/XmlQuery.h \
    thirdparty/liblastfm/src/Xspf.h \
    thirdparty/liblastfm/src/misc.h \
    thirdparty/liblastfm/src/ws.h \
    \
    thirdparty/qt-solutions/qtsingleapplication/src/qtlocalpeer.h \
    thirdparty/qt-solutions/qtsingleapplication/src/qtlockedfile.h \
    thirdparty/qt-solutions/qtsingleapplication/src/qtsingleapplication.h


SOURCES += *.cpp
SOURCES += \
    thirdparty/liblastfm/src/Album.cpp \
    thirdparty/liblastfm/src/Artist.cpp \
    thirdparty/liblastfm/src/Audioscrobbler.cpp \
    thirdparty/liblastfm/src/Auth.cpp \
    thirdparty/liblastfm/src/Chart.cpp \
    thirdparty/liblastfm/src/FingerprintId.cpp \
    thirdparty/liblastfm/src/InternetConnectionMonitor.cpp \
    thirdparty/liblastfm/src/Library.cpp \
    thirdparty/liblastfm/src/Mbid.cpp \
    thirdparty/liblastfm/src/misc.cpp \
    thirdparty/liblastfm/src/NetworkAccessManager.cpp \
    thirdparty/liblastfm/src/NetworkConnectionMonitor.cpp \
    thirdparty/liblastfm/src/Playlist.cpp \
    thirdparty/liblastfm/src/RadioStation.cpp \
    thirdparty/liblastfm/src/RadioTuner.cpp \
    thirdparty/liblastfm/src/ScrobbleCache.cpp \
    thirdparty/liblastfm/src/ScrobblePoint.cpp \
    thirdparty/liblastfm/src/Tag.cpp \
    thirdparty/liblastfm/src/Tasteometer.cpp \
    thirdparty/liblastfm/src/Track.cpp \
    thirdparty/liblastfm/src/Url.cpp \
    thirdparty/liblastfm/src/UrlBuilder.cpp \
    thirdparty/liblastfm/src/User.cpp \
    thirdparty/liblastfm/src/ws.cpp \
    thirdparty/liblastfm/src/XmlQuery.cpp \
    thirdparty/liblastfm/src/Xspf.cpp \
    \
    thirdparty/qt-solutions/qtsingleapplication/src/qtlocalpeer.cpp \
    thirdparty/qt-solutions/qtsingleapplication/src/qtlockedfile.cpp \
    thirdparty/qt-solutions/qtsingleapplication/src/qtsingleapplication.cpp

RESOURCES += \
    gdesktunes.qrc

OTHER_FILES += \
    icons/gdesktunes.iconset/* \
    js/*.js \
    userstyles/*.css \
    userstyles/mini/*.css

FORMS += \
    mainwindow.ui \
    settings.ui \
    aboutdialog.ui

css.path = $$DESTDIR/userstyles
css.files = userstyles/*
export(css)

minicss.path = $$DESTDIR/userstyles/mini
minicss.files = userstyles/mini/*
export(minicss)

js.path = $$DESTDIR/js
js.files = js/*
export(js)

INSTALLS += css minicss js
