TOP_DIR = ..

include(../config.pri)
include(../connection/libmaliit-connection.pri)
include(../maliit/libmaliit.pri)
include(../common/libmaliit-common.pri)

VERSION = $$MALIIT_ABI_VERSION
TEMPLATE = lib
TARGET = $$TOP_DIR/lib/$${MALIIT_SETTINGS_LIB}

DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"$${MALIIT_INPUTCONTEXT_NAME}\\\"

QT = core gui

OBJECTS_DIR = .obj
MOC_DIR = .moc

HEADERSINSTALL = \
    settingsentry.h \
    pluginsettings.h \
    settingsmanager.h \

HEADERS += \
    $$HEADERSINSTALL \

SOURCES += \
    settingsentry.cpp \
    pluginsettings.cpp \
    settingsmanager.cpp \

target.path += $$LIBDIR

headers.path += $$INCLUDEDIR/$$MALIIT_HEADER/maliit
headers.files += $$HEADERSINSTALL

outputFiles(maliit-settings.pc)

OTHER_FILES += \
    maliit-settings.pc.in

install_pkgconfig.path = $${LIBDIR}/pkgconfig
install_pkgconfig.files = $$OUT_PWD/maliit-settings.pc

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
    libmaliit-settings.pri
