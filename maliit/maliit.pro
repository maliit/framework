include(../config.pri)

VERSION = 0.1.0
TEMPLATE = lib
TARGET = $${MALIIT_LIB}

INCLUDEPATH += . .. ../src

QT = core gui

OBJECTS_DIR = .obj
MOC_DIR = .moc

HEADERSINSTALL = \
    minputmethodstate.h \
    preeditinjectionevent.h \
    inputmethod.h \
    attributeextension.h \

FRAMEWORKHEADERSINSTALL = \
    namespace.h

HEADERS += \
    $$HEADERSINSTALL \
    $$FRAMEWORKHEADERSINSTALL \
    preeditinjectionevent_p.h \
    inputmethod_p.h \
    attributeextension_p.h \
    attributeextensionregistry_p.h \
    attributeextensionregistry.h \

SOURCES += \
    preeditinjectionevent.cpp \
    inputmethod.cpp \
    attributeextension.cpp \
    attributeextensionregistry.cpp \

target.path += $$M_IM_INSTALL_LIBS

headers.path += $$M_IM_INSTALL_HEADERS/$$MALIIT_HEADER/maliit
headers.files += $$HEADERSINSTALL

frameworkheaders.path += $$M_IM_INSTALL_HEADERS/$$MALIIT_FRAMEWORK_HEADER/maliit
frameworkheaders.files += $$FRAMEWORKHEADERSINSTALL

outputFiles(maliit-$${MALIIT_INTERFACE_VERSION}.pc, maliit-framework-$${MALIIT_FRAMEWORK_INTERFACE_VERSION}.pc)

install_pkgconfig.path = $${M_IM_INSTALL_LIBS}/pkgconfig
install_pkgconfig.files = $$OUT_PWD/maliit-$${MALIIT_INTERFACE_VERSION}.pc $$OUT_PWD/maliit-framework-$${MALIIT_FRAMEWORK_INTERFACE_VERSION}.pc

INSTALLS += \
    target \
    headers \
    frameworkheaders \
    install_pkgconfig \
