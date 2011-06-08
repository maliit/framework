include(../mconfig.pri)

VERSION = 0.1.0
TEMPLATE = lib
TARGET = maliit

INCLUDEPATH += . ../src

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

headers.path += $$M_IM_INSTALL_HEADERS/meegoimframework
headers.files += $$HEADERSINSTALL

INSTALLS += \
    target \
    headers \
