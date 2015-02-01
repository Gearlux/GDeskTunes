TEMPLATE = subdirs

SUBDIRS += \
    bonjourgear

win32: {
    SUBDIRS += \
        MMShellHook \
        MMGDeskHook
}
