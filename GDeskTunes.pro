TEMPLATE = subdirs

SUBDIRS += \
    thirdparty \
    lib \
    src \
    examples \
    plugins \
    tools

src.depends = lib
examples.depends = lib
src.depends = tools

OTHER_FILES += \
    icons/gdesktunes.iconset/* \
    js/*.js \
    userstyles/*.css \
    userstyles/*.scss \
    userstyles/mini/*.css \
    userstyles/Material/* \
    userstyles/GDeskTunes/* \
    README.md \
    LICENSE \
    deploy/windows.bat \
    deploy/mac.sh \
    deploy/cpld.bash \
    deploy/GDeskTunes.sh \
    deploy/linux.sh
