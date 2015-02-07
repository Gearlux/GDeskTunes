TEMPLATE = subdirs

SUBDIRS += \
    thirdparty \
    lib \
    src \
    examples \
    plugins

src.depends = lib
examples.depends = lib

OTHER_FILES += \
    icons/gdesktunes.iconset/* \
    js/*.js \
    userstyles/*.css \
    userstyles/mini/*.css \
    README.md \
    LICENSE \
    deploy/windows.bat \
    deploy/mac.sh \
    deploy/cpld.bash \
    deploy/GDeskTunes.sh \
    deploy/linux.sh
