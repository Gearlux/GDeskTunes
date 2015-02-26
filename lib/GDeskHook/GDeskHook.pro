TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

include(deployment.pri)
qtcAddDeployment()

win32: {

    LIBS += -luser32
}

CONFIG(release, debug|release): BINDIR = $$OUT_PWD/release
CONFIG(debug, debug|release): BINDIR = $$OUT_PWD/debug
PROJDIR = $$PWD
BINDIR ~= s,/,\\,g
PROJDIR ~= s,/,\\,g
contains(QMAKE_HOST.arch, x86_64){
TARGET=GDeskHook64
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../MMShellHook/release/ -lMMShellHook64
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../MMShellHook/debug/ -lMMShellHook64
QMAKE_POST_LINK += $$QMAKE_COPY $$BINDIR\\GDeskHook64.exe $$PROJDIR\\..\\..\\bin
} else {
TARGET=GDeskHook
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../MMShellHook/release/ -lMMShellHook
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../MMShellHook/debug/ -lMMShellHook
QMAKE_POST_LINK += $$QMAKE_COPY $$BINDIR\\GDeskHook.exe $$PROJDIR\\..\\..\\bin
}

INCLUDEPATH += $$PWD/../MMShellHook
DEPENDPATH += $$PWD/../MMShellHook
