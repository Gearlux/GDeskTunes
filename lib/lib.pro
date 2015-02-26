TEMPLATE = subdirs

SUBDIRS += \
    bonjourgear

win32: {
    SUBDIRS += \
        MMShellHook \
        GDeskHook
}

GDeskHook.depends = MMShellHook
