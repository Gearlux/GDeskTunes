QMAKE_MAC_SDK = macosx10.9

TEMPLATE = subdirs

OTHER_FILES +=  \
    bootstrap.sh \
    bootstrap.bat \
    liblastfm.pro \
    SPMediaKeyTap.pro \
    qt-solutions.pro \
    QtWaitingSpinner.pro

linux: {
system(thirdparty/bootstrap.sh)
}

win32: {
# Clone some thirdparty software into our path
system(bootstrap.bat)
}

mac: {
system(./bootstrap.sh)
}

SUBDIRS += \
    liblastfm \
    qt-solutions \
    QtWaitingSpinner

mac: {
    SUBDIRS += \
        SPMediaKeyTap
}
