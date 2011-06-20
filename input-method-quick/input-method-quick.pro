include(../config.pri)

VERSION = 0.1.0
TEMPLATE = lib
TARGET = $$MALIIT_PLUGINS_QUICK_LIB
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

headers.path += $$M_IM_INSTALL_HEADERS/$$MALIIT_PLUGINS_QUICK_HEADER
headers.files += $$HEADERSINSTALL

nomeegotouch {
    outputFiles(maliit-plugins-quick-$${MALIIT_PLUGINS_QUICK_INTERFACE_VERSION}.pc)
} else {
    outputFiles(meegoimquick.prf)
}

install_prf.path = $$[QT_INSTALL_DATA]/mkspecs/features
install_prf.files = $$OUT_PWD/meegoimquick.prf

install_pkgconfig.path = $${M_IM_INSTALL_LIBS}/pkgconfig
install_pkgconfig.files = $$OUT_PWD/maliit-plugins-quick-$${MALIIT_PLUGINS_QUICK_INTERFACE_VERSION}.pc

INSTALLS += target \
    headers \
    install_prf \
    install_pkgconfig \

QMAKE_EXTRA_TARGETS += check
check.target = check
check.depends += lib$${TARGET}.so.$${VERSION}
