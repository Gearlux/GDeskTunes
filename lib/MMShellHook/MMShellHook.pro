#-------------------------------------------------
#
# Project created by QtCreator 2015-01-31T13:54:05
#
#-------------------------------------------------

QT       -= core gui

win32: {
    contains(QMAKE_HOST.arch, x86_64){
        TARGET = MMShellHook64
        LIBS += -luser32 /def:"$$PWD/MMShellHook64.def"
    } else {
        TARGET = MMSHellHook
        LIBS += -luser32 /def:"$$PWD/MMShellHook.def"
    }
}
TEMPLATE = lib

DEFINES += MMSHELLHOOK_LIBRARY

SOURCES += mmshellhook.cpp

HEADERS += mmshellhook.h\
        mmshellhook_global.h

OTHER_FILES += \
    MMShellHook.def \
    MMShellHook64.def

CONFIG(release, debug|release): BINDIR = $$OUT_PWD/release
CONFIG(debug, debug|release): BINDIR = $$OUT_PWD/debug
PROJDIR = $$PWD
BINDIR ~= s,/,\\,g
PROJDIR ~= s,/,\\,g
contains(QMAKE_HOST.arch, x86_64){
QMAKE_POST_LINK += $$QMAKE_COPY $$BINDIR\\MMShellHook64.dll $$PROJDIR\\..\\..\\bin
} else {
QMAKE_POST_LINK += $$QMAKE_COPY $$BINDIR\\MMShellHook.dll $$PROJDIR\\..\\..\\bin
}
