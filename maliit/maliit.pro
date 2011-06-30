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
    mpreeditinjectionevent.h \

HEADERS += \
    $$HEADERSINSTALL \
    minputmethodstate_p.h \
    mpreeditinjectionevent_p.h \

SOURCES += \
    minputmethodstate.cpp \
    mpreeditinjectionevent.cpp \

target.path += $$M_IM_INSTALL_LIBS

headers.path += $$M_IM_INSTALL_HEADERS/$$MALIIT_HEADER
headers.files += $$HEADERSINSTALL

outputFiles(maliit-$${MALIIT_INTERFACE_VERSION}.pc)

install_pkgconfig.path = $${M_IM_INSTALL_LIBS}/pkgconfig
install_pkgconfig.files = $$OUT_PWD/maliit-$${MALIIT_INTERFACE_VERSION}.pc

INSTALLS += \
    target \
    headers \
    install_pkgconfig \
