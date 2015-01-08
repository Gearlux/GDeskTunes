#-------------------------------------------------
#
# Project created by QtCreator 2015-01-08T23:00:57
#
#-------------------------------------------------

QT       -= core gui

TARGET = SPMediaKeyTap
TEMPLATE = lib
CONFIG += staticlib

OBJECTIVE_SOURCES += \
    SPMediaKeyTap.m \
    SPInvocationGrabbing/NSObject+SPInvocationGrabbing.m

QMAKE_OBJECTIVE_CFLAGS_WARN_ON = -Wall -Wno-semicolon-before-method-body -Wno-unused-parameter

#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}
