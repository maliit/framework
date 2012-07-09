include(../config.pri)

TOP_DIR = ..

VERSION = $$MALIIT_ABI_VERSION
TEMPLATE = lib
TARGET = $$TOP_DIR/lib/$${MALIIT_LIB}

include($$TOP_DIR/connection/libmaliit-connection.pri)
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

target.path += $$LIBDIR

headers.path += $$INCLUDEDIR/$$MALIIT_HEADER/maliit
headers.files += $$HEADERSINSTALL

outputFiles(maliit-$${MALIIT_INTERFACE_VERSION}.pc)

OTHER_FILES += \
    maliit-$${MALIIT_INTERFACE_VERSION}.pc.in

install_pkgconfig.path = $${LIBDIR}/pkgconfig
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

OTHER_FILES += \
    libmaliit.pri
