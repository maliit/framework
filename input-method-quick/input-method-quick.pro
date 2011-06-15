include(../config.pri)

VERSION = 0.1.0
TEMPLATE = lib
TARGET = meegoimquick
INCLUDEPATH += ../src

# Input
HEADERSINSTALL = \
        minputmethodquickplugin.h \
        minputmethodquick.h \

HEADERS += $$HEADERSINSTALL \

SOURCES += \
        minputmethodquickplugin.cpp \
        minputmethodquick.cpp \

QT = core gui xml declarative

contains(CONFIG, nomeegotouch) {
} else {
    CONFIG  += meegotouchcore
    DEFINES += HAVE_MEEGOTOUCH
}

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

QMAKE_CLEAN += *.gcno *.gcda

target.path += $$M_IM_INSTALL_LIBS

OBJECTS_DIR = .obj
MOC_DIR = .moc

headers.path += $$M_IM_INSTALL_HEADERS/meegoimquick
headers.files += $$HEADERSINSTALL

outputFiles(meegoimquick.prf)

install_prf.path = $$[QT_INSTALL_DATA]/mkspecs/features
install_prf.files = $$OUT_PWD/meegoimquick.prf

INSTALLS += target \
    headers \
    install_prf \

QMAKE_EXTRA_TARGETS += check
check.target = check
check.depends += lib$${TARGET}.so.$${VERSION}
