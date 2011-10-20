include(../config.pri)

VERSION = 0.1.0
TEMPLATE = lib
TARGET = $${MALIIT_LIB}

INCLUDEPATH += .. ../common

QT = core gui

OBJECTS_DIR = .obj
MOC_DIR = .moc

HEADERSINSTALL = \
    preeditinjectionevent.h \
    inputmethod.h \
    attributeextension.h \

HEADERS += \
    $$HEADERSINSTALL \
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

outputFiles(maliit-$${MALIIT_INTERFACE_VERSION}.pc)

install_pkgconfig.path = $${M_IM_INSTALL_LIBS}/pkgconfig
install_pkgconfig.files = $$OUT_PWD/maliit-$${MALIIT_INTERFACE_VERSION}.pc

INSTALLS += \
    target \
    headers \
    install_pkgconfig \

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

QMAKE_CLEAN += $$OBJECTS_DIR/*.gcno $$OBJECTS_DIR/*.gcda
