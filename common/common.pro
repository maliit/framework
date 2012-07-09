include(../config.pri)

TOP_DIR = ..

TEMPLATE = lib
TARGET = $$TOP_DIR/lib/maliit-common

CONFIG += staticlib

outputFiles(maliit-framework-$${MALIIT_FRAMEWORK_INTERFACE_VERSION}.pc)
outputFiles(maliit-framework.prf)

OTHER_FILES += \
    maliit-framework-$${MALIIT_FRAMEWORK_INTERFACE_VERSION}.pc.in \
    maliit-framework.prf.in \

FRAMEWORKHEADERSINSTALL = \
    maliit/namespace.h \
    maliit/settingdata.h \

HEADERS += \
    $$FRAMEWORKHEADERSINSTALL \
    maliit/namespaceinternal.h \

SOURCES += \
    maliit/settingdata.cpp \

frameworkheaders.path += $$INCLUDEDIR/$$MALIIT_FRAMEWORK_HEADER/maliit
frameworkheaders.files += $$FRAMEWORKHEADERSINSTALL

install_pkgconfig.path = $${LIBDIR}/pkgconfig
install_pkgconfig.files = $$OUT_PWD/maliit-framework-$${MALIIT_FRAMEWORK_INTERFACE_VERSION}.pc

install_prf.path = $$MALIIT_INSTALL_PRF
install_prf.files = $$OUT_PWD/maliit-framework.prf

INSTALLS += \
    frameworkheaders \
    install_pkgconfig \
    install_prf \

OTHER_FILES += \
    libmaliit-common.pri
