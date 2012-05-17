include(../config.pri)

TEMPLATE = lib
TARGET = maliit-common

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

frameworkheaders.path += $$M_IM_INSTALL_HEADERS/$$MALIIT_FRAMEWORK_HEADER/maliit
frameworkheaders.files += $$FRAMEWORKHEADERSINSTALL

install_pkgconfig.path = $${M_IM_INSTALL_LIBS}/pkgconfig
install_pkgconfig.files = $$OUT_PWD/maliit-framework-$${MALIIT_FRAMEWORK_INTERFACE_VERSION}.pc

install_prf.path = $$M_IM_INSTALL_PRF
install_prf.files = $$OUT_PWD/maliit-framework.prf

INSTALLS += \
    frameworkheaders \
    install_pkgconfig \
    install_prf \

OTHER_FILES += \
    libmaliit-common.pri
