QMAKE_MAC_SDK = macosx10.9

TEMPLATE = subdirs

OTHER_FILES +=  $$PWD/README \
                $$PWD/bootstrap.sh

mac{
    system(./bootstrap.sh)
}
