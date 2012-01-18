include(../config.pri)

TEMPLATE = lib
TARGET = maliit-common

outputFiles(maliit-framework-$${MALIIT_FRAMEWORK_INTERFACE_VERSION}.pc)

HEADERSINSTALL = \
    minputmethodnamespace.h \

FRAMEWORKHEADERSINSTALL = \
    maliit/namespace.h \

HEADERS += \
    $$HEADERSINSTALL \
    $$FRAMEWORKHEADERSINSTALL \
    maliit/namespaceinternal.h \

headers.path += $$M_IM_INSTALL_HEADERS/$$MALIIT_PLUGINS_HEADER
headers.files += $$HEADERSINSTALL

frameworkheaders.path += $$M_IM_INSTALL_HEADERS/$$MALIIT_FRAMEWORK_HEADER/maliit
frameworkheaders.files += $$FRAMEWORKHEADERSINSTALL

install_pkgconfig.path = $${M_IM_INSTALL_LIBS}/pkgconfig
install_pkgconfig.files = $$OUT_PWD/maliit-framework-$${MALIIT_FRAMEWORK_INTERFACE_VERSION}.pc

INSTALLS += \
    headers \
    frameworkheaders \
    install_pkgconfig \
