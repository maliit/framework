include(../../config.pri)

VERSION = 0.1.0
TEMPLATE = lib
TARGET = $$MALIIT_PLUGINS_QUICK_LIB
INCLUDEPATH_BASE = ../..
INCLUDEPATH += \
        $${INCLUDEPATH_BASE} \
        $${INCLUDEPATH_BASE}/src \
        $${INCLUDEPATH_BASE}/common \

# Input
HEADERSINSTALL = \
        minputmethodquickplugin.h \
        minputmethodquick.h \
        maliitquick.h \
        mkeyoverridequick.h

HEADERS += $$HEADERSINSTALL \
        mkeyoverridequick_p.h

SOURCES += \
        minputmethodquickplugin.cpp \
        minputmethodquick.cpp \
        mkeyoverridequick.cpp

QT = core $$QT_WIDGETS xml
contains(QT_MAJOR_VERSION, 5) {
    QT += quick1
} else {
    QT += declarative
}

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

target.path += $$M_IM_INSTALL_LIBS

OBJECTS_DIR = .obj
MOC_DIR = .moc

QMAKE_CLEAN += $$OBJECTS_DIR/*.gcno $$OBJECTS_DIR/*.gcda

headers.path += $$M_IM_INSTALL_HEADERS/$$MALIIT_PLUGINS_QUICK_HEADER
headers.files += $$HEADERSINSTALL

!enable-legacy {
    outputFiles(maliit-plugins-quick-$${MALIIT_PLUGINS_QUICK_INTERFACE_VERSION}.pc)
} else {
    outputFiles(meegoimquick.prf)
}

QT_PRF_DIR = $$[QT_INSTALL_DATA]/mkspecs/features
QT_PREFIX = $$[QT_INSTALL_PREFIX]
enforce-install-prefix {
    QT_PRF_DIR = $$replace(QT_PRF_DIR, $$QT_PREFIX, $$M_IM_PREFIX)
}

install_prf.path = $$QT_PRF_DIR
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
