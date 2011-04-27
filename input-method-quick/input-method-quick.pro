include(../mconfig.pri)

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

install_prf.path = $$[QT_INSTALL_DATA]/mkspecs/features
install_prf.files = meegoimquick.prf
install_prf.CONFIG += no_check_exist

INSTALLS += target \
    headers \
    install_prf \

QMAKE_EXTRA_TARGETS += check
check.target = check
check.depends += lib$${TARGET}.so.$${VERSION}

IN_FILES = meegoimquick.prf.in

infilegenerator.output = ${QMAKE_FILE_BASE}
infilegenerator.input = IN_FILES
infilegenerator.commands += sed -e \"s:@M_IM_QUICK_FEATURE@:$$M_IM_QUICK_FEATURE:g\" \
                                -e \"s:@M_IM_PREFIX@:$$M_IM_PREFIX:g\" \
                                -e \"s:@M_IM_INSTALL_BIN@:$$M_IM_INSTALL_BIN:g\" \
                                -e \"s:@M_IM_INSTALL_HEADERS@:$$M_IM_INSTALL_HEADERS:g\" \
                                -e \"s:@M_IM_INSTALL_LIBS@:$$M_IM_INSTALL_LIBS:g\" \
                                -e \"s:@M_IM_VERSION@:$$M_IM_VERSION:g\" \
                                 ${QMAKE_FILE_NAME} > ${QMAKE_FILE_OUT}
infilegenerator.CONFIG = target_predeps no_link
infilegenerator.depends += Makefile
QMAKE_EXTRA_COMPILERS += infilegenerator

